#ifndef lint
static char	*RCSid = "$Header$";
#endif

/*
 * util.c - utility routines.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log$
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <sys/param.h>
#include <stdio.h>
#include <pwd.h>

#include "xpostit.h"

/*
 * ByeBye - clean up and exit.
 */
void
ByeBye()
{
	/*
	 * If saving notes is on, save all notes.
	 */
	if (app_res.save_notes)
		SaveAllNotes();

	XtUnmapWidget(toplevel);
	XCloseDisplay(display);

	exit(0);
}

/*
 * SetNoteDir - determine the path to the note directory.
 */
void
SetNoteDir()
{
	char *getenv();
	char *home, *s;
	struct passwd *pwd;
	char fname[MAXPATHLEN];

	/*
	 * If it's an absolute path name,
	 * we're done.
	 */
	if (app_res.note_dir[0] == '/')
		return;

	/*
	 * Find the user's home directory.
	 */
	if ((home = getenv("HOME")) == NULL) {
		if ((pwd = getpwuid(getuid())) == NULL) {
			fprintf(stderr, "xpostit: who are you?\n");
			exit(1);
		}

		home = pwd->pw_dir;
	}

	/*
	 * Save the path.
	 */
	sprintf(fname, "%s/%s", home, app_res.note_dir);

	s = SafeAlloc(strlen(fname) + 1);
	app_res.note_dir = s;
	strcpy(s, fname);
}

/*
 * MakeFname - make a file name from a note index number.
 */
char *
MakeFname(index)
register int index;
{
	char *s;
	char fname[MAXPATHLEN];

	sprintf(fname, "%s/%s%d", app_res.note_dir, PostItNoteFname, index);
	s = SafeAlloc(strlen(fname) + 1);
	strcpy(s, fname);
	return(s);
}

/*
 * SafeAlloc - allocate n bytes of memory, exit if we run out.
 */
char *
SafeAlloc(nbytes)
register int nbytes;
{
	char *malloc();
	register char *s;

	if ((s = malloc(nbytes)) == NULL) {
		fprintf(stderr, "xpostit: out of memory.\n");
		exit(1);
	}

	bzero(s, nbytes);
	return(s);
}
