/* This file is part of xdir, an X-based directory browser.
 * It contains functions for dealing with xdir's resources.
 *
 *	Created: 9/5/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/resources.c,v $
 *	$Athena: resources.c,v 1.1 88/09/16 01:30:02 treese Exp $
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_resources_c[] = "$Header: resources.c,v 2.1 89/03/25 02:53:18 treese Exp $";
#endif

#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#define RESOURCE_DEFS		/* This file defines the resource variable. */

#include "xdir.h"

/* Resource global variables. */

char *HelpFile;
char *ViewHelpFile;
char *AboutFile;
Boolean ShowDotFiles;
Boolean ShowBackupFiles;
char *ShowDotFilesMsg;
char *HideDotFilesMsg;
char *ShowBackupFilesMsg;
char *HideBackupFilesMsg;

/* Resource table. */

static XtResource resources[] = {
  {"helpFile", XtCFile, XtRString, sizeof(char *),
	(Cardinal) &HelpFile, XtRString, HELPFILE},
  {"viewHelpFile", XtCFile, XtRString, sizeof(char *),
	(Cardinal) &ViewHelpFile, XtRString, VIEWHELP},
  {"aboutFile", XtCFile, XtRString, sizeof(char *),
	(Cardinal) &AboutFile, XtRString, ABOUTFILE},
  {"showDotFiles", XtCBoolean, XtRBoolean, sizeof(Boolean),
	(Cardinal) &ShowDotFiles, XtRString, "True"},
  {"showBackupFiles", XtCBoolean, XtRBoolean, sizeof(Boolean),
	(Cardinal) &ShowBackupFiles, XtRString, "True"},
  {"showDotFilesMsg", XtCString, XtRString, sizeof(char *),
	(Cardinal) &ShowDotFilesMsg, XtRString, "Show Dot Files"},
  {"hideDotFilesMsg", XtCString, XtRString, sizeof(char *),
	(Cardinal) &HideDotFilesMsg, XtRString, "Hide Dot Files"},
  {"showBackupFilesMsg", XtCString, XtRString, sizeof(char *),
	(Cardinal) &ShowBackupFilesMsg, XtRString, "Show Backup Files"},
  {"hideBackupFilesMsg", XtCString, XtRString, sizeof(char *),
	(Cardinal) &HideBackupFilesMsg, XtRString, "Hide Backup Files"},
};

/* Function:	InitResources() initializes the resource database.
 * Arguments:	w:	Widget to use.
 * Returns:	Nothing.
 * Notes:
 */

void
InitResources(w)
Widget w;
{
	XtGetApplicationResources(w, NULL, resources, XtNumber(resources),
					NULL, (Cardinal) 0);
}
