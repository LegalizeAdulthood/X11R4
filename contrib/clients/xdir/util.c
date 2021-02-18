/* This file is part of xdir, an X-based directory browser.
 * It contains miscellaneous utility functions.
 *
 *	Created: 8/13/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/util.c,v $
 *	$Athena: util.c,v 1.1 88/09/16 01:30:09 treese Locked $
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_util_c[] = "$Header: util.c,v 2.1 89/03/25 02:48:10 treese Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include "xdir.h"

/* Function:	SaveString() creates a copy of a string.
 * Arguments:	string:	String to save.
 * Returns:	A pointer to the new copy (char *).
 * Notes:
 */

char *
SaveString(string)
char *string;
{
	char *new;

	new = malloc(strlen(string) + 1);
	strcpy(new, string);
	return(new);
}

#ifdef ABORT_ON_XT_ERRORS
/* Function:	AbortOnXtError() aborts the program on an X error.
 * Arguments:	msg:	Toolkit error message.
 * Returns:	Nothing.
 * Notes:
 */

static void
AbortOnXtError(msg)
char *msg;
{
	fprintf(stderr, "%s\n", msg);
	abort();
}
#endif /* ABORT_ON_XT_ERRORS */

/* Function:	InitErrorHandlers() initializes error handling for xdir.
 * Arguments:	None.
 * Returns:	Nothing.
 * Notes:
 */

void
InitErrorHandlers()
{
#ifdef ABORT_ON_XT_ERRORS
	XtSetErrorHandler(AbortOnXtError); 
#endif /* ABORT_ON_XT_ERRORS */
}

/* Function:	IsDirectory() tests to see if a pathname is a directory.
 * Arguments:	path:	Pathname of file to test.
 * Returns:	True or False.
 * Notes:	False is returned if the directory is not accessible.
 */

Boolean
IsDirectory(path)
char *path;
{
	char fullpath[MAXPATHLEN];
	struct stat statbuf;

	if (path == NULL)
		return(False);
	MakeFullPath(path, fullpath);
	if (stat(fullpath, &statbuf)) {
		perror("xdir");
		return(False);
	}
	if (statbuf.st_mode & S_IFDIR) 
		return(True);
	else
		return(False);
}

/* Function:	MakeFullPath() creates the full pathname for the given file.
 * Arguments:	filename:	Name of the file in question.
 *		pathname:	Buffer for full name.
 * Returns:	Nothing.
 * Notes:
 */

void
MakeFullPath(filename, pathname)
char *filename;
char *pathname;
{
	strcpy(pathname, CurrentDirectory);
	strcat(pathname, "/");
	strcat(pathname, filename);
}

/* Function:	SetDirIconName() sets the icon name of the directory browser.
 * Arguments:	dir: Name to set with.
 * Returns:
 * Notes:
 *	This function creates an icon name of the form "xdir: directory".
 */

void
SetDirIconName(dir)
char *dir;
{
	Arg args[10];
	Cardinal arg_cnt;
	char iconname[MAXPATHLEN];

	strcpy(iconname, "xdir: ");
	strcat(iconname, dir);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNiconName, iconname); arg_cnt++;
	XtSetValues(TopWidget, args, arg_cnt);
}

/* Function:	GetWidgetHeight() gets the height of a widget.
 * Arguments:	w:	widget in question.
 * Returns:	The height (integer).
 * Notes:
 */

int
GetWidgetHeight(w)
Widget w;
{
    static int height;
    static Arg args[] = { {XtNheight, (XtArgVal)&height} } ;

    XtGetValues( w, args, (Cardinal)1 );
    return(height);
}

/* Function:	GetWidgetWidth() gets the width of a widget.
 * Arguments:	w:	Widget in question.
 * Returns:	The width (an integer).
 * Notes:
 */

int
GetWidgetWidth(w)
Widget w;
{
    static int width;
    static Arg args[] = { {XtNwidth, (XtArgVal)&width} } ;

    XtGetValues( w, args, (Cardinal)1 );
    return(width);
}

/* Function:	GetWidgetXLoc() gets the x location of a widget.
 * Arguments:	w:	Widget in question.
 * Returns:	The x coordinate (an integer).
 * Notes:
 */

int
GetWidgetXLoc(w)
Widget w;
{
    static int x;
    static Arg args[] = { {XtNx, (XtArgVal)&x} } ;

    XtGetValues(w, args, (Cardinal)1);
    return(x);
}

/* Function:	GetWidgetYLoc() gets the y location of a widget.
 * Arguments:	w:	Widget in question.
 * Returns:	The y coordinate (an integer).
 * Notes:
 */

int
GetWidgetYLoc(w)
Widget w;
{
    static int y;
    static Arg args[] = { {XtNy, (XtArgVal)&y} } ;

    XtGetValues(w, args, (Cardinal) 1);
    return(y);
}
