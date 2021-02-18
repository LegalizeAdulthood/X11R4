/* $Header: util.c,v 1.4 89/04/21 03:31:41 xwindows Exp $ */
/*
 *  Generally useful procedures
 */
/* This file contains code from the JOVE screen editor.  */

/************************************************************************
 * JOVE is Copyright (C) 1986 by Jonathan Payne.  JOVE is               *
 * provided to you without charge, and with no warranty.  You may give  *
 * away copies of JOVE, including sources, provided that this notice is *
 * included in all the files.                                           *
 ************************************************************************/

/*
 * Modified by Mark Moraes for use in a widget for the X Windows System
 * Version 11. This file is still independent of the X Windows System unless
 * XWINDOWS is defined.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <errno.h>
#include <pwd.h>
#ifdef XWINDOWS
#include <X11/Xos.h>
#else
#include <strings.h>
#endif


char *
StripName(name)
char *name;
{
	char *begin = rindex (name, '/');
	char *end, *result;
	int length;
	extern char *malloc();

	begin = (begin ? begin+1 : name);
	end = index (begin, '.');
	length = (end ? (end - begin) : strlen (begin));
	if ((result = malloc ((unsigned) (length + 1))) == 0)
		return(0);
	(void) strncpy (result, begin, length);
	result [length] = '\0';
	return (result);
}

/* Get a user's home directory */
get_hdir(user, buf)
register char	*user,
				*buf;
{
	struct passwd	*pw;

	/* Rewind the passwd file - we may have searched once already */
	(void) setpwent();
	if (((pw = getpwnam(user)) != NULL) && (pw->pw_dir != NULL)) {
			(void) strcpy(buf, pw->pw_dir);
			return;
	}
}

/*
 * gets the users home directory and stores it in a static area, so that
 * subsequent calls get it cheaply. Uses the HOME environment variable, if
 * that fails, uses the password file entry, else returns "/" if even that
 * fails.
 */
char *
getmyhomedir()
{
	static char *cp = NULL;
	extern char *getenv();
	extern char *malloc();
	struct passwd *pw;

	if (cp)
		return cp;
		
	if ((cp = getenv("HOME")) == NULL) {
		(void) setpwent();
		pw = getpwuid(getuid());
		if (pw != NULL && pw->pw_dir != NULL) {
			cp = malloc((unsigned) (strlen(pw->pw_dir) + 1));
			if (cp != NULL)
				(void) strcpy(cp, pw->pw_dir);
		}
	}
	if (cp == NULL)
		cp = "/";
	return(cp);
}
			
/* Replace ~/ with $HOME/ and ~user/ with get_hdir/ */
PathParse(name, intobuf)
char	*name,
		*intobuf;
{
	intobuf[0] = '\0';
	if (*name == '\0')
		return;

	if (*name == '~') {
		if (name[1] == '/' || name[1] == '\0') {
			(void) strcpy(intobuf, getmyhomedir());
			name++;
		} else {
			char	*uendp = index(name, '/'),
					unamebuf[32];

			if (uendp == 0)
				uendp = name + strlen(name);
			name = name + 1;
			null_ncpy(unamebuf, name, uendp - name);
			get_hdir(unamebuf, intobuf);
			name = uendp;
		}
	} else if (*name == '\\')
		name++;
	(void) strcat(intobuf, name);
}

int
file_backup(fname, defaultmode)
char *fname;
u_short defaultmode;
{
	char	*s;
	register int	i;
	int	fd1,
		fd2;
	char	tmp1[BUFSIZ],
			tmp2[BUFSIZ];
	int	mode;
	
	(void) strcpy(tmp1, fname);
	
	if ((s = rindex(tmp1, '/')) == NULL)
		(void) sprintf(tmp2, "#%s~", fname);
	else {
		*s++ = '\0';
		(void) sprintf(tmp2, "%s/#%s~", tmp1, s);
	}

	if ((fd1 = open(fname, 0)) < 0)
		return -1;

	if (defaultmode == 0)
		mode = 0666;
	else
		mode = defaultmode;
		
	if ((fd2 = creat(tmp2, mode)) < 0) {
		(void) close(fd1);
		return -1;
	}

	while ((i = read(fd1, tmp1, sizeof(tmp1))) > 0)
		(void) write(fd2, tmp1, i);

	(void) close(fd1);

	if (close(fd2) == -1)
		return -1;

	return 0;
	    
}


null_ncpy(to, from, n)
char	*to,
		*from;
{
	(void) strncpy(to, from, n);
	to[n] = '\0';
}

