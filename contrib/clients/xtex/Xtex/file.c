/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

#include "xtex.h"

#include <errno.h>
#include <sys/param.h>
#include <assert.h>

#ifdef X11R4
#  include <X11/Xaw/Label.h>
#  include <X11/Xaw/Text.h>
#  include <X11/Xaw/AsciiSrc.h>
#else
#  include <X11/Label.h>
#  include <X11/Text.h>
#endif

#include "dvi-simple.h"
#include "page.h"
#include "mark.h"
#include "widgets.h"

char TeXInputFileNameString[MAXPATHLEN+40];
char *TeXInputFileName;

char TeXInputDirNameString[MAXPATHLEN+40];
char *TeXInputDirName;

char fileNameInput[MAXPATHLEN];

/*
 *	If it's a file name, set to the directory of that file,
 *	otherwise, set to the directory
 */
void
setDirectory(dir)
char *dir;
{
  Arg argList[20];
  Cardinal args;

  char pDir[MAXPATHLEN];
  char *endPDir;
  int len;

  strncpy(pDir,dir,MAXPATHLEN);
  len = strlen(pDir);
  endPDir = &pDir[len];

  while (chdir(pDir) < 0) {
    /* skip back to last slash */
    while (endPDir != pDir && *endPDir != '/') endPDir--;

    if (endPDir == pDir) {
      getwd(pDir);
    }
    else {
      *endPDir = 0;
    }

  }

  getwd(pDir);

  strcpy(TeXInputDirNameString, "Directory: ");
  TeXInputDirName = &TeXInputDirNameString[strlen(TeXInputDirNameString)];
  strncpy(TeXInputDirName, pDir, MAXPATHLEN);

  if ( directoryLabel != NULL ) {
    args = 0;
    XtSetArg(argList[args], XtNlabel, TeXInputDirNameString); args++;
    XtSetValues( directoryLabel, argList, args);
  }
}

void setFileName(fileName)
char *fileName;
{
  if ( fileNameText && XtIsRealized( fileNameText ) ) {

#ifdef X11R4
    XawTextBlock tblk;
#else
    XtTextBlock tblk;
#endif

    int len = (fileName == 0) ? 0 : strlen(fileName);
    if ( len > 0 ) {
#ifdef X11R4
      Arg argList[20];
      Cardinal args;
      char *currentString;
      XawTextPosition start;
      XawTextPosition end;

      args = 0;
      XtSetArg(argList[args], XtNstring, &currentString); args++;
      XtGetValues(fileNameText, argList, args);

      start = 0; end = strlen(currentString);
#else
      XtTextPosition start = 0;
      XtTextPosition end = strlen(fileNameInput);
#endif

      tblk.firstPos = 0;
      tblk.length = len;
      tblk.ptr = fileName;
      tblk.format = FMT8BIT;
      
#ifdef X11R4
      XawTextDisableRedisplay(fileNameText);
      XawTextReplace( fileNameText, start, end, &tblk );
      XawTextEnableRedisplay(fileNameText);
#else
      XtTextDisableRedisplay(fileNameText);
      XtTextReplace( fileNameText, start, end, &tblk );
      XtTextEnableRedisplay(fileNameText);
#endif
    }
  }
  else {
    int len = (fileName == 0) ? 0 : strlen(fileName);
    bzero(fileNameInput, MAXPATHLEN);
    if ( len > 0 ) {
      strncpy(fileNameInput, fileName, MAXPATHLEN);
    }
  }
}

void
setFile(fileName, copy)
char *fileName;
int copy;
{
  extern double atof();
  DviDpi = (int) atof( xtexResources.dpiHorizString );

  TeXPageCloseFile();	/* close open pages stuff */

  if( DviInit(fileName, copy )) {
    disablePageLevel();
    TeXPageSlamPages();
    error(0, 0,"[fopen] Unable to open ``%s''", fileName);
  }
  else {
    enablePageLevel();

    error(0, 0, "%s the %d page file ``%s''",
	  (copy) ? "Copied" : "Opened",
	  DviTotalPages, DviFileName);

    strcpy(TeXInputFileNameString, "File: ");
    TeXInputFileName = &TeXInputFileNameString[strlen(TeXInputFileNameString)];
    strncpy(TeXInputFileName, DviFileName, MAXPATHLEN);

    if ( fileLabel != NULL) {
      Arg argList[20];
      Cardinal args;
      
      args = 0;
      XtSetArg(argList[args], XtNlabel, TeXInputFileNameString); args++;
      XtSetValues( fileLabel, argList, args );
    }
  }
  TeXFontNewFile();
  TeXMarkNewFile();
  TeXPageOpenFile();
}

/*
 *	Page related callbacks & routines
 */

enablePageLevel()
{
  Arg argList[20];
  Cardinal args;
      
  args = 0;
  XtSetArg(argList[args], XtNsensitive, True); args++;

  XtSetValues(pageButton, argList, args);
  XtSetValues(printAllButton, argList, args);
  XtSetValues(printMarkedButton, argList, args);
  XtSetValues(printUnmarkedButton, argList, args);

  XtSetValues(clearMarksButton, argList, args);
  if ( markMenuList ) {
    XtSetValues(markMenuList, argList, args);
  }
}

disablePageLevel()
{
  Arg argList[20];
  Cardinal args;
      
  args = 0;
  XtSetArg(argList[args], XtNsensitive, False); args++;

  XtSetValues(pageButton, argList, args);
  XtSetValues(printAllButton, argList, args);
  XtSetValues(printMarkedButton, argList, args);
  XtSetValues(printUnmarkedButton, argList, args);

  XtSetValues(clearMarksButton, argList, args);
  if ( markMenuList ) {
    XtSetValues(markMenuList, argList, args);
  }
}

static void actionQuit(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  exit(0);
}

static void actionNoop(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  XBell(XtDisplay(w), 100);
}

static void actionFile(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  static lastOpenKind = 0;

  char *fileName;
  
  if ( *num_params < 1 ) {
    lastOpenKind = 1;		/* default to copy */
  }
  else if ( strncmp(params[0],"close", 5) == 0 ) {
    DviFini();
    TeXFontNewFile();
    TeXMarkNewFile();
    TeXPageCloseFile();
    TeXPageSlamPages();
    disablePageLevel();
  }
  else {
    int doCd = 0;

    if ( strncmp(params[0],"open", 4) == 0 ) {
      lastOpenKind = 0;		/* default to copy */
    }
    else if ( strncmp(params[0],"copy", 4) == 0 ) {
      lastOpenKind = 1;		/* default to copy */
    }
    else if ( strncmp(params[0],"cd", 2) == 0 ) {
      doCd = 1;
    }
    else if ( strncmp(params[0],"reopen", 6) == 0 ) {
      /* use old value */
    }
    else {
      error(0,0,"syntax: xtex-file(open/copy/reopen/close[,filename])");
      lastOpenKind = 0;
    }
    
    if ( *num_params == 2 ) {
      fileName = params[1];
    }
    else if ( *num_params == 1 ) {
#ifdef X11R4
      Arg argList[20];
      Cardinal args;
      char *currentString;

      args = 0;
      XtSetArg(argList[args], XtNstring, &currentString); args++;
      XtGetValues(fileNameText, argList, args);
      fileName = currentString;
#else
      fileName = fileNameInput;
#endif
    }
    else {
      error(0,0,"syntax: xtex-file(open/copy/reopen/close[,filename])");
      fileName = fileNameInput;
    }
    
    if ( fileName == NULL ) {
      error(0, 0, "No file specified?");
    } 
    else {
      if ( doCd ) {
	setDirectory( fileName );
      }
      else {
	setFile( fileName, lastOpenKind );
      }
    }
  }
}

static void actionPopupPage(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  if (DviFile != 0 ) {
    TeXPageBuild();
  }
}

static XtActionsRec fileInputActionsTable[] = {
  {"xtex-file",   actionFile},
  {"xtex-page-popup",   actionPopupPage},
  {"xtex-quit",   actionQuit},
  {"xtex-noop",   actionNoop}
};

void
TeXFileInstallActions()
{
  XtAddActions(fileInputActionsTable, XtNumber(fileInputActionsTable));
}


void
  BuildFileLevel()
{
  Arg argList[20];
  Cardinal args;
  int len = strlen(fileNameInput);

  args = 0;
#ifdef X11R4
  XtSetArg(argList[args], XtNeditType,XawtextEdit); args++;
  XtSetArg(argList[args], XtRAsciiType, XawAsciiString); args++;
#else
  XtSetArg(argList[args], XtNeditType,XttextEdit); args++;
#endif
  XtSetArg(argList[args], XtNstring, fileNameInput); args++;
  XtSetArg(argList[args], XtNlength, MAXPATHLEN); args++;
  fileNameText =
    XtCreateManagedWidget("fileName",
#ifdef X11R4
			  asciiTextWidgetClass, topPane,
#else
			  asciiStringWidgetClass, topPane,
#endif
			  argList, args);

  quitButton =
    XtCreateManagedWidget("quitButton",
			  commandWidgetClass, topPane,
			  0,0);
  openButton =
    XtCreateManagedWidget("openButton",
			  commandWidgetClass, topPane,
			  0,0);
  copyButton =
    XtCreateManagedWidget("copyButton",
			  commandWidgetClass, topPane,
			  0,0);
  closeButton =
    XtCreateManagedWidget("closeButton",
			  commandWidgetClass, topPane,
			  0,0);
  cdButton =
    XtCreateManagedWidget("cdButton",
			  commandWidgetClass, topPane,
			  0,0);
  pageButton
    = XtCreateManagedWidget("pageButton",
			    commandWidgetClass, topPane,
			    0,0);

  printAllButton
    = XtCreateManagedWidget("printAllButton",
			    commandWidgetClass, topPane,
			    0,0);
  printMarkedButton
    = XtCreateManagedWidget("printMarkedButton",
			    commandWidgetClass, topPane,
			    0,0);
  printUnmarkedButton
    = XtCreateManagedWidget("printUnmarkedButton",
			    commandWidgetClass, topPane,
			    0,0);

  clearMarksButton
    = XtCreateManagedWidget("clearMarksButton",
			    commandWidgetClass, topPane,
			    0,0);
  disablePageLevel();
  
  args = 0;
  XtSetArg(argList[args], XtNlabel, TeXInputDirNameString); args++;
  XtSetArg(argList[args], XtNresize, True); args++;
  directoryLabel = 
    XtCreateManagedWidget("directoryLabel",
			  labelWidgetClass, topPane,
			  argList, args);

  args = 0;
  XtSetArg(argList[args], XtNlabel, TeXInputFileNameString); args++;
  XtSetArg(argList[args], XtNresize, True); args++;
  fileLabel = 
    XtCreateManagedWidget("fileLabel",
			  labelWidgetClass, topPane,
			  argList, args);
}
