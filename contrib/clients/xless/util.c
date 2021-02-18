/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carlo Lisa
 *	   MIT Project Athena
 *	   carlo@athena.mit.edu
 *
 * $Header: /mit/tax/Xless/RCS/util.c,v 1.3 89/10/11 20:34:11 carlo Exp $
 */

#include "xless.h"

extern Widget box;
extern XlessResources resources;

extern void NewFile();
extern void PopupMessage();
extern Widget ErrorBox();

XFontStruct *buttonfont;
XFontStruct *labelfont;
XFontStruct *textfont;
Cursor stdcur;
Cursor dialogcur;

/*
 *	Function name:	SetPopup
 *	Description:	This function pops up the specified dialog box.
 *	Arguments:	wdg - the widget to be popped up.
 *	Returns:	None.
 */

void SetPopup(wdg)
Widget wdg;
{
  Position x=0, y=0;
  Arg args[5];
  Window rwin;
  Window chwin;
  int rx, ry, wx, wy;
  Dimension wd = 0;
  Dimension he = 0;
  unsigned int mask;

    /* Make the popup shell "wdg" come up at the current pointer position */
    XQueryPointer(XtDisplay(box),XtWindow(box),&rwin,&chwin,&rx,&ry,
			&wx,&wy,&mask);

    XtSetArg(args[0], XtNwidth, &wd);
    XtSetArg(args[1], XtNheight, &he);
    XtGetValues(wdg, args, (Cardinal)2);
    if (wd == 0) wd = DIALOGWIDTH;
    if (he == 0) he = DIALOGHEIGHT;

    x = rx - wd/2;
    y = ry - he/2;

    XtSetArg(args[0], XtNx, x);
    XtSetArg(args[1], XtNy, y);
    XtSetValues(wdg, args, (Cardinal)2);

    /* Popup the widget */
    XtPopup(wdg, XtGrabNone);
}

/*
 *	Function name:	NotFound
 *	Description:	This function pops up the "not found"  dialog box.
 *	Arguments:	wdg - the widget to be popped up.
 *	Returns:	None.
 */

void NotFound(top)
Widget top;
{
	PopupMessage(top,"String not found .....");
}

/*
 *	Function Name: Find
 *	Description:   This function search the specified string in
 *		       the specified chunk of memory.
 *	Arguments:     data   - a pointer to the data read from the file
 *			        and stored in dynamic memory.
 *		       string - a pointer to the string to search.
 *	Returns:       #char offset from the current cursor position
 *		       to the beginning of string;
 *		       -1 if string not found.
 */

XtTextPosition Find(data,string)
char *data;
char *string;
{
	char *tmpd, *tmps;
	XtTextPosition beg = 0;

	tmpd = data;
	tmps = string;

	while ((*tmpd != '\0')&&(*tmps != '\0')) {
		if (*tmpd == *tmps) {
			tmpd++;
			tmps++;
		}
		else {
			tmpd++;
			tmps = string;
		}
	}
	if (*tmps == '\0') {
		beg = tmpd - data - strlen(string);
		return(beg);
	}
	else
		return ((XtTextPosition)(-1));
}

/*
 *	Function name:	DoSearch
 *	Description:	This function executes the search for the 
 *			specified string.
 *	Arguments:	tex       - the widget to be popped up.
 *			data      - pointer to memory containing the file.
 *			sele      - string to search for.
 *			father    - the father widget.
 *	Returns:	None.
 */

void DoSearch(tex,data,sele,father)
Widget tex;
char *data;
char *sele;
Widget father;
{
    XtTextPosition beg, end, offset1, offset2;
    int len;
    char *search_buf;

    len = strlen(sele);
    offset1 = XtTextGetInsertionPoint(tex);
    search_buf = data + offset1;
    if ((offset2 = Find(search_buf,sele)) != -1) {
	beg = offset1 + offset2;
    	end = beg + len;
    	XtTextSetInsertionPoint(tex, beg+len);
    	XtTextSetSelection(tex, beg, end);
    }
    else 
	NotFound(father);
}

/*
 *	Function name:	CheckFonts
 *	Description:	This function checks the resource DB for the
 *			user specified fonts.
 *	Arguments:	None.
 *	Returns:	None.
 */

void CheckFonts()
{
	if (!(buttonfont = resources.fonts.button))
		buttonfont = resources.fonts.standard;
	if (!(labelfont = resources.fonts.label))
		labelfont = resources.fonts.standard;
	if (!(textfont = resources.fonts.text))
		textfont = resources.fonts.standard;

	dialogcur = resources.cursors.dialog;
	stdcur = resources.cursors.dialog;
	
	if (!buttonfont || !labelfont || !textfont) {
	   fprintf(stderr,"xless: unable to open any of the specified fonts\n");
	   exit(1);
	}
}

/*
 *	Function name:	SetReadText
 *	Description:	This function set a new text in a
 *			read-only text widget. 
 *	Arguments:	w      - the asciiText widget.
 *			string - the new text source.
 *	Returns:	None.
 */

SetReadText(w,string)
     Widget w;
     char *string;
{
  Arg arg[2];
  XtTextSource source;

  /* Create a new source for the text widget, and put in the new string */
  XtSetArg(arg[0], XtNstring, string);
  XtSetArg(arg[1], XtNeditType, XttextRead);
  source = XtStringSourceCreate(w, arg, (Cardinal) 2);
  XtTextSetSource(w,source,(XtTextPosition) 0);
}

/*
 *	Function name:	PopupMessage
 *	Description:	This function pops up a dialog box with the
 *			specified message.
 *	Arguments:	w       - the father widget.
 *			message - the message.
 *	Returns:	None.
 */

void PopupMessage(w,message)
  Widget w;
  char *message;
{
    Widget ebox;

	ebox = ErrorBox(w,message);
	SetPopup(ebox);
}
