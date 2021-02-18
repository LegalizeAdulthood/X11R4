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
 * $Header: /mit/tax/Xless/RCS/new.c,v 1.3 89/10/11 20:33:56 carlo Exp $
 */

#include "xless.h"
#include "XlessWin.icon"

extern char *InitData();
extern SearchData *MakeDialog();
extern void MakeToolbox();
extern Widget MakeTitle();
extern Widget MakeText();
extern Widget MakeMainFrame();
extern Display *disp;

/*
 *      Function Name: NewWindow
 *      Description:   This function reads a file and displays it
 *		       in a new xless window.
 *      Arguments:     new_file_name - a file pointer to the file that
 *				       we are opening.
 *      Returns:       None.
 */

void NewWindow(top, new_file_name)
Widget top;
char *new_file_name;
{
  Arg args[15];
  FILE * new_file;             /* The stream of the new file. */
  int num_args;
  char *new_memory;
  Widget new, new_outer, new_main_frame, new_text, lab;
  SearchData *ndialog_struct;
  int flag;
  char icon_name[MAX_INPUT];
  char message[128];

	if((strcmp(new_file_name,"\0")) == 0)
		return;

	if( (new_file = fopen(new_file_name, "r")) == NULL ) {
	    strcpy(message,"Couldn't open file: ");
	    strcat(message, new_file_name);
	    PopupMessage(top,message);
	    return;
	}

	/* This is a new window => put the button "close window" */
	flag = 1;

	num_args = 0;
	XtSetArg(args[num_args], XtNiconPixmap, 
		XCreateBitmapFromData(disp, XRootWindow(disp,0),
			XlessWin_bits, XlessWin_width,
			XlessWin_height)); num_args++;

	strcpy(icon_name, new_file_name);
	XtSetArg(args[num_args], XtNiconName, icon_name); num_args++;

	XtSetArg(args[num_args], XtNallowShellResize, TRUE); num_args++;

	new = XtCreateApplicationShell("new", applicationShellWidgetClass, 
                                   args, num_args);

	new_memory = InitData(new_file);

        new_outer = XtCreateManagedWidget("new_outer",vPanedWidgetClass,
					new,NULL,0);
        lab = MakeTitle(new_file_name,new_outer);

        new_main_frame = MakeMainFrame(new_outer);

        new_text = MakeText(new_main_frame,new_memory);

        ndialog_struct = MakeDialog(new);
	ndialog_struct->text = new_text;
	ndialog_struct->memory = new_memory;
	ndialog_struct->lab = lab;

        MakeToolbox(new_main_frame,ndialog_struct,flag,new,new_file_name);

	XtPopup(new,XtGrabNone);
}
