/* This file contains code from the JOVE screen editor */

/************************************************************************
 * JOVE is Copyright (C) 1986 by Jonathan Payne.  JOVE is               *
 * provided to you without charge, and with no warranty.  You may give  *
 * away copies of JOVE, including sources, provided that this notice is *
 * included in all the files.                                           *
 ************************************************************************/

/*
 *  Modified by Mark Moraes for use in a widget for the X Windows System
 *  Version 11. This file is still independent of the X Windows System.
 */

/* The routines in this file perform Tenex-style filename completion. 
   The routine to be called is 
	f_complete(buf, cursorposition, cols, c)
	char *buf;

   where 'buf' contains the filename so far. 'cursorposition' is the 
   location of the cursor in 'buf' - it should be at the end of the 'buf'.
   'cols' is the width of the screen used for typeout, (the listing of files)
   and 'c' is one of ' ', '\t', and '?'. For the former two, f_complete
   attempts to complete the name in 'buf', and for the latter, it
   lists out the files which match the name so far using the typeout
   routines.

   It requires two sets of external routines to do this - insert_s(), 
   add_mess() and rbell() are used for completion, and TOstart(), 
   Typeout(), and TOstop() are used for typeout. These are described 
   below 
 */
#include <ctype.h>
#include <signal.h>
#include <varargs.h>
#include <stdio.h>
#ifdef XWINDOWS
# include <X11/Xos.h>
#else
# include <strings.h>
# include <sys/types.h>
#endif
#include <sys/stat.h>
#ifdef DIRENT
# include <sys/param.h>
# include <dirent.h>
# ifndef DIRSIZE
#  define DIRSIZE(entry)	DIRSIZ
# endif
# ifndef direct
#  define direct	dirent
# endif
#else
# include <sys/dir.h>
# define DIRSIZE(entry)	DIRSIZ(entry)
#endif DIRENT

#define FILESIZE	128
#define TRUE		1
#define FALSE		0
#define min(x, y)	((x) < (y) ? (x) : (y))
#define max(x, y)	((x) > (y) ? (x) : (y))

static char *linebuf;
static int curchar;
static int maxCols;

extern char *malloc();
extern char *realloc();

/**********************External functions **********************************/
/* insert_s(at, s, len, curpos) char *at, *s; int len; int *curpos;
 *	deletes from 'at' to the end of the line, and inserts the first len
 *	characters of 's' there. It returns 'curpos' as the new end of the
 *	string being edited - the cursor should now be there
 */
extern void insert_s();

/* add_mess(s) char *s;
 *	inserts 's' at the end of the buffer, then waits a respectable
 *	interval, deletes 's', and returns
 */
extern void add_mess();

/* rbell()
 *	Rings a bell or attracts the user's attention in some other way 
 */
extern void rbell();

/* TOstart(s) char *s;
 *	Starts the typeout, and prints 's' as a title. Typeout is some 
 *	sort of overlay 'window' or something, for temporary output, 
 *	which can popup, and vanish after the user has read it. 
 */
extern TOstart();

/* Typeout(fmt, args) char *fmt; va_dcl args;
 *	Is like printf() - prints args according to format 'fmt'. 
 *	Is a <varargs> routine
 */
extern Typeout();

/* TOstop()
 *	End of typeout - this performs some sort of wait()
 *	- like for a keypress or a mouse click. It then cleans up
 *	the typeout and returns.
 */
extern TOstop();

char *xmalloc(n)
{
	extern char *malloc();
	char *p = malloc((unsigned) n);

	if (!p) {
		(void) fprintf(stderr, "out of memory in malloc\n");
		exit(-1);
	}
	return p;
}

char *xrealloc(s, n)
char *s;
{
	extern char *realloc();
	char *p = realloc(s, (unsigned) n);

	if (!p) {
		(void) fprintf(stderr, "out of memory in realloc\n");
		exit(-1);
	}
	return p;
}

/* Scandir returns the number of entries or -1 if the directory cannoot
   be opened or malloc fails. */

int
myscandir(dir, nmptr, qualify, sorter)
char	*dir;
char	***nmptr;
int	(*qualify)();
int	(*sorter)();
{
	DIR	*dirp;
	struct direct	*entry;
	char	**ourarray;
	int	nalloc = 10;
	int	nentries = 0;

	if ((dirp = opendir(dir)) == 0)
		return -1;
	ourarray = (char **) xmalloc(nalloc * sizeof (char *));
	while ((entry = readdir(dirp)) != 0) {
		if (qualify != 0 && (*qualify)(entry->d_name) == 0)
			continue;
		if (nentries == nalloc) {
			ourarray = (char **) xrealloc((char *) ourarray, (nalloc += 10) * sizeof (char *));
		}
		ourarray[nentries] = (char *) xmalloc((int) DIRSIZE(entry) + 1);
		null_ncpy(ourarray[nentries], entry->d_name, (int) DIRSIZE(entry));
		nentries++;
	}
	closedir(dirp);
	if ((nentries + 1) != nalloc)
		ourarray = (char **) xrealloc((char *) ourarray,
					((nentries + 1) * sizeof (char *)));
	if (sorter != 0)
		qsort((char *) ourarray, nentries, sizeof (char **), sorter);
	*nmptr = ourarray;
	ourarray[nentries] = 0;		/* guaranteed 0 pointer */

	return nentries;
}

freedir(nmptr, nentries)
char	***nmptr;
{
	char	**ourarray = *nmptr;

	while (--nentries >= 0)
		free(*ourarray++);
	free((char *) *nmptr);
	*nmptr = 0;
}

alphacomp(a, b)
char	**a,
	**b;
{
	return strcmp(*a, *b);
}

numcomp(s1, s2)
register char	*s1,
		*s2;
{
	register int	count = 0;

	while (*s1 != 0 && *s1++ == *s2++)
		count++;
	return count;
}

static char	*fc_filebase;
char	BadExtensions[128] = ".o";

static
bad_extension(name, bads)
char	*name,
	*bads;
{
	char	*ip;
	int	namelen = strlen(name),
		ext_len,
		stop = 0;

	do {
		if (ip = index(bads, ' '))
			*ip = 0;
		else {
			ip = bads + strlen(bads);
			stop++;
		}
		if ((ext_len = ip - bads) == 0)
			continue;
		if ((ext_len < namelen) &&
		    (strcmp(&name[namelen - ext_len], bads) == 0))
			return TRUE;
	} while ((bads = ip + 1), !stop);
	return FALSE;
}

f_match(file)
char	*file;
{
	int	len = strlen(fc_filebase);

	return ((len == 0) ||
		(strncmp(file, fc_filebase, strlen(fc_filebase)) == 0));
}

static
isdir(name)
char	*name;
{
	struct stat	stbuf;
	char	filebuf[FILESIZE];

	PathParse(name, filebuf);
	return ((stat(filebuf, &stbuf) != -1) &&
		(stbuf.st_mode & S_IFDIR) == S_IFDIR);
}

static
fill_in(dir_vec, n)
register char	**dir_vec;
{
	int	minmatch = 0,
    		numfound = 0,
    		lastmatch = -1,
		i,
		the_same = TRUE, /* After filling in, are we the same
				    as when we were called? */
		is_ntdir;	/* Is Newly Typed Directory name */
	char	bads[128];

	for (i = 0; i < n; i++) {
		(void) strcpy(bads, BadExtensions);
		/* bad_extension() is destructive */
		if (bad_extension(dir_vec[i], bads))
			continue;
		if (numfound)
			minmatch = min(minmatch,
				       numcomp(dir_vec[lastmatch], dir_vec[i]));
		else
			minmatch = strlen(dir_vec[i]);
		lastmatch = i;
		numfound++;
	}
	/* Ugh.  Beware--this is hard to get right in a reasonable
	   manner.  Please excuse this code--it's past my bedtime. */
	if (numfound == 0) {
		rbell();
		return;
	}
	if (minmatch > strlen(fc_filebase)) {
		the_same = FALSE;
		insert_s(fc_filebase, dir_vec[lastmatch], minmatch, &curchar);
	}
	is_ntdir = ((numfound == 1) &&
		    (curchar > 0) &&
		    (linebuf[curchar - 1] != '/') &&
		    (isdir(linebuf)));
	if (the_same && !is_ntdir) {
		add_mess((n == 1) ? " [Unique]" : " [Ambiguous]");
	}
	if (is_ntdir)
		insert_s(&linebuf[curchar], "/", 1, &curchar);
}

/*
 *  called when one of "\t ?" is typed.  Does the right thing,
 *  depending on which.
 */

f_complete(sbuf, curpos, cols, c)
char *sbuf;
{
	char	dir[FILESIZE],
		**dir_vec;
	int	nentries;
#ifdef TYPEOUT
	int	i;
#endif

	linebuf = sbuf;
	curchar = curpos;
	maxCols = cols;
	
	if (linebuf[curpos] != '\0')
		linebuf[curpos] = '\0';
		
	if ((fc_filebase = rindex(linebuf, '/')) != 0) {
		char	tmp[FILESIZE];

		null_ncpy(tmp, linebuf, (++fc_filebase - linebuf));
		if (tmp[0] == '\0')
			(void) strcpy(tmp, "/");
		PathParse(tmp, dir);
	} else {		
		fc_filebase = linebuf;
		(void) strcpy(dir, ".");
	}
	if ((nentries = myscandir(dir, &dir_vec, f_match, alphacomp)) == -1) {
		char err[FILESIZE];
		
		(void) sprintf(err, " [Unknown directory: %s]", dir);
		add_mess(err);
		return 1;
	}
	if (nentries == 0) {
		add_mess(" [No match]");
	} else if (c == ' ' || c == '\t')
		fill_in(dir_vec, nentries);
	else {
		/* we're a '?' */
#ifdef TYPEOUT
		int	maxlen = 0,
			ncols,
			col,
			lines,
			linespercol;

		TOstart("Completion");
		Typeout("(! means file will not be chosen unless typed explicitly)");
		Typeout((char *) 0);
		Typeout("Possible completions (in %s):", dir);
		Typeout((char *) 0);

		for (i = 0; i < nentries; i++)
			maxlen = max(strlen(dir_vec[i]), maxlen);
		maxlen += 4;	/* pad each column with at least 4 spaces */
		ncols = (maxCols - 2) / maxlen;
		linespercol = 1 + (nentries / ncols);

		for (lines = 0; lines < linespercol; lines++) {
			for (col = 0; col < ncols; col++) {
				int	isbad,
					which;
				char	bads[128];

				which = (col * linespercol) + lines;
				if (which >= nentries)
					break;
				(void) strcpy(bads, BadExtensions);
				isbad = bad_extension(dir_vec[which], bads);
				Typeout("%s%-*s", isbad ? "!" : "",
					maxlen - isbad, dir_vec[which]);
			}
			Typeout((char *) 0);
		}
		TOstop();
#endif
	}
	freedir(&dir_vec, nentries);
	return 1;
}
