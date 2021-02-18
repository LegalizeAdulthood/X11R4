/* This file is part of xdir, an X-based directory browser.
 * It contains general xdir definitions.
 *
 *	Created: 8/13/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/xdir.h,v $
 *	$Athena: xdir.h,v 1.1 88/09/16 01:30:33 treese Locked $
 */

/* Configuration definitions. */

#define HELPFILE	"/mit/StaffTools/lib/xdir/xdir.help"
#define VIEWHELP	"/mit/StaffTools/lib/xdir/xdir-view.help"
#define ABOUTFILE	"/mit/StaffTools/lib/xdir/xdir.about"

#ifndef MAIN	/* External variables. */

extern char CurrentSelectionName[];
extern int CurrentSelectionIndex;
extern char CurrentDirectory[];
extern Widget MyListWidget;
extern Widget DirLabel;
extern Widget TopWidget;
extern Widget MyViewport;

#endif /* MAIN */

#ifndef RESOURCE_DEFS	/* External variables specified by resources. */

extern char *HelpFile;
extern char *ViewHelpFile;
extern char *AboutFile;
extern Boolean ShowDotFiles;
extern Boolean ShowBackupFiles;
extern char *ShowDotFilesMsg;
extern char *HideDotFilesMsg;
extern char *ShowBackupFilesMsg;
extern char *HideBackupFilesMsg;

#endif /* RESOURCE_DEFS */

/* From the C library. */

extern int errno;
extern char *sys_errlist[];

/* Useful constants. */

#define EOS	'\0'			/* End-of-string. */
#define TWIDDLE	'~'		        /* Emacs backup file suffix. */

#define NOEDIT	0			/* File is read-only. */
#define	EDIT	1			/* File can be edited. */

/* Type definitions. */

typedef void (*VoidProc)();

/* Xdir function declarations. */

char **MakeFileList();
void Error(), PrintFileList(), ErrAbort();
char *SaveString();
void InitCommands(), InitTextDisplay(), InitErrorHandlers(),
  DisplayFile(), DoChangeDir(), InitErrorMessages();
void MakeFullPath();
Boolean IsDirectory();
void SetDirIconName();
int GetWidgetWidth(), GetWidgetHeight();
int GetWidgetXLoc(), GetWidgetYLoc();
void InitActions();
void ShowHelp();
void SetTextFile();
void InitResources();
void InitMenus(),
	InitActionsMenu(),
	InitConfigureMenu(),
	InitCommandsMenu(),
	InitButtonActions(),
	InitHelp();
void AddMenuEntry();

/* C library function declarations. */

char *malloc(), *calloc(), *realloc();

