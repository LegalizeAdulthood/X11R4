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
#include "widgets.h"

Widget topPane;
Widget topFile;
Widget topPages;
Widget topError;

/*
 *	Widgets related to file I/O
 */
Widget topFileBox;
Widget fileNameText;
Widget openButton;
Widget copyButton;
Widget cdButton;
Widget closeButton;
Widget quitButton;
Widget directoryLabel;
Widget fileLabel;

/*
 *	Widgets related to the pages table
 */
Widget pageButton;
Widget printAllButton;
Widget printMarkedButton;
Widget printUnmarkedButton;

Widget clearMarksButton;
Widget markMenuViewPort;
Widget markMenuList;

/*
 *	Error dialog widgets
 */

Widget errorMsgBox;
Widget errorText;

#ifdef USE_CLEARBUTTON
Widget clearButton;
#endif

void
  BuildTopLevelWidget(topLevel)
Widget topLevel;
{

  Arg argList[20];
  Cardinal args;

  args = 0;
  XtSetArg(argList[args], XtNresize, True); args++;

#ifndef USE_TABLE
  topPane = 
    XtCreateManagedWidget("topLevel",
			  formWidgetClass, topLevel,
			  argList, args);
#else
  topPane = 
    XtCreateManagedWidget("topLevel",
			  tableWidgetClass, topLevel, argList, args);
#endif

  BuildFileLevel();
  BuildErrorBox();

  args = 0;
  XtSetArg(argList[args], XtNtop, XtChainTop); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetValues(fileNameText, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetValues(quitButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNfromHoriz, quitButton); args++;
  XtSetValues(openButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNfromHoriz, openButton); args++;
  XtSetValues(copyButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNfromHoriz, copyButton); args++;
  XtSetValues(closeButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileNameText); args++;
  XtSetArg(argList[args], XtNfromHoriz, closeButton); args++;
  XtSetValues(cdButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, openButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, quitButton); args++;
  XtSetValues(pageButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, quitButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, pageButton); args++;
  XtSetValues(printAllButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, quitButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, printAllButton); args++;
  XtSetValues(printMarkedButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, quitButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, printMarkedButton); args++;
  XtSetValues(printUnmarkedButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, quitButton); args++;
  XtSetArg(argList[args], XtNfromHoriz, printUnmarkedButton); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(clearMarksButton, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, pageButton); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresizable, True); args++;
  XtSetArg(argList[args], XtNresize, True); args++;
  XtSetValues(directoryLabel, argList, args);

  args = 0;
  XtSetArg(argList[args], XtNfromVert, directoryLabel); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresizable, True); args++;
  XtSetArg(argList[args], XtNresize, True); args++;
  XtSetValues(fileLabel, argList, args);


#ifdef USE_CLEARBUTTON
  args = 0;
  XtSetArg(argList[args], XtNfromVert, fileLabel); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresize, False); args++;
  XtSetArg(argList[args], XtNresizable, False); args++;
  XtSetValues(clearButton, argList, args);
#endif

  args = 0;

#ifdef USE_CLEARBUTTON
  XtSetArg(argList[args], XtNfromVert, clearButton); args++;
#else
  XtSetArg(argList[args], XtNfromVert, fileLabel); args++;
#endif

  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  XtSetArg(argList[args], XtNresizable, True); args++;
  XtSetValues(errorText, argList, args);

  /* don't manage the mark menu viewport until the list is built, or 
     it'll be oddly sized */

  args = 0;
  XtSetArg(argList[args], XtNresize, True); args++;
  XtSetArg(argList[args], XtNmappedWhenManaged, False); args++;
  XtSetArg(argList[args], XtNfromVert, errorText); args++;
  XtSetArg(argList[args], XtNleft, XtChainLeft); args++;
  
  markMenuViewPort =
    XtCreateWidget("markMenuViewPort",
		   viewportWidgetClass, topPane,
		   argList, args);
  {
    extern void TeXMarkMenuSelect();
    XtCallbackRec menuCallBacks [2];
    char ** foo[2];
    foo[0] = foo[1] = 0;

    menuCallBacks[0].callback = TeXMarkMenuSelect;
    menuCallBacks[0].closure = NULL;
    menuCallBacks[1].callback = NULL;
    menuCallBacks[1].closure = NULL;

    args = 0;
    XtSetArg(argList[args], XtNresize, True); args++;
    XtSetArg(argList[args], XtNcallback, (caddr_t) menuCallBacks); args++;
    XtSetArg(argList[args], XtNlist, foo); args++;
    markMenuList =
      XtCreateManagedWidget("markMenuList", listWidgetClass, markMenuViewPort,
			    argList, args);
  }
  
  BuildMarkMenuPopUp( );
  XtManageChild( markMenuViewPort );
}
