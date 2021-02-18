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
 * $Header: /mit/tax/Xless/RCS/popup.c,v 1.6 89/10/11 20:34:02 carlo Exp $
 */

#include "xless.h"

extern XFontStruct *buttonfont;
extern XFontStruct *labelfont;
extern XFontStruct *textfont;
extern Cursor dialogcur;
extern char sel[];
extern char new_sel[];
extern char new_sel_file[];

extern void Cancel();
extern void ECancel();
extern void Confirm();
extern void NewConfirm();
extern void Reload();
extern void NewFile();

/*
 *	Function Name: DialogBox
 *	Description:   This function creates a single dialog box.
 *	Arguments:  top		father widget;
 *		    conf_func	confirm function (callback);
 *		    c_data	callback data (for conf_func);
 *		    label	label for the dialog box;
 *		    selection 	user's input (back).
 *	Returns:    popup	the dialogbox widget id.
 */

Widget DialogBox(top, conf_func, c_data, label, conflab, selection)
Widget top;
void (*conf_func)();
caddr_t c_data;
char *label;
char *conflab;
char *selection;
{
  static XtActionsRec actions[2];
  static String myreturn = "#override <Key>Return:   set() notify() unset()\n";
  XtAccelerators accel;
  static int init = 0;
  Widget popup, pane, lbl, input, confirm, cancel;
  Arg xtargv[15];
  int xtargc;
  XtCallbackRec xtcalls[2];
  unsigned long fh=0;
  char trans[1024];

	actions[0].string = "Nothing";
	actions[0].proc = (XtActionProc)NULL;

	if(!init) {
		XtAddActions(actions,XtNumber(actions));
		init = 1;
	}

	accel = XtParseAcceleratorTable(myreturn);

	xtcalls[1].callback = NULL;
	xtcalls[1].closure = (caddr_t)NULL;

	XtSetArg(xtargv[0], XtNallowShellResize, True);
	popup = XtCreatePopupShell("popup",transientShellWidgetClass,
		top,xtargv,1);
	xtargc = 0;
	pane = XtCreateManagedWidget("form", formWidgetClass, popup,
			xtargv, xtargc);
	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNjustify,XtJustifyLeft ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNborderWidth,0 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNlabel, label ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfont,labelfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNright,XtChainLeft ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNleft,XtChainLeft ); xtargc++;
	lbl = XtCreateManagedWidget("label", labelWidgetClass, pane,
				xtargv, xtargc);
	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNfont,textfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNeditType,XttextEdit ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNstring,selection ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNlength,MAX_INPUT ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,DEF_INPUT_W ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNresizable,True ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtextOptions,resizeWidth ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,0 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,lbl ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNright,XtChainLeft ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNleft,XtChainLeft ); xtargc++;
	input = XtCreateManagedWidget("input", asciiStringWidgetClass,
					pane, xtargv, xtargc);
	XtSetKeyboardFocus(pane, input);

	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,dialogcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,5 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,input ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNright,XtChainLeft ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNleft,XtChainLeft ); xtargc++;
	xtcalls[0].callback =  conf_func;
	xtcalls[0].closure = c_data;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	XtSetArg(xtargv[xtargc],XtNaccelerators,accel); xtargc++;
	confirm  = XtCreateManagedWidget(conflab, commandWidgetClass, pane,
					xtargv, xtargc);
	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,dialogcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,5 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,2 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,input ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,confirm ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNright,XtChainLeft ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNleft,XtChainLeft ); xtargc++;
	xtcalls[0].callback =  Cancel ;
	xtcalls[0].closure = (caddr_t)popup;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	cancel  = XtCreateManagedWidget("Cancel", commandWidgetClass, pane,
					xtargv, xtargc);

	XtInstallAccelerators(input, confirm);

	return(popup);
}

/*
 *	Function Name:	ErrorBox
 *	Description:	This function creates the error dialog box.
 *	Parameter:	top	father widget;
 *	Returns:	widget	id of the error box.
 */

Widget ErrorBox(top,label)
Widget top;
char *label;
{
  Widget err_pop, panerr, error, okay;
  Arg xtargv[15];
  int xtargc;
  XtCallbackRec xtcalls[2];
  static String myreturn = "#override <Key>Return:   set() notify() unset()\n";
  XtAccelerators accel;

	accel = XtParseAcceleratorTable(myreturn);

	xtcalls[1].callback = NULL ;
	xtcalls[1].closure = (caddr_t) NULL ;

	xtargc = 0;
	XtSetArg(xtargv[0], XtNallowShellResize, True);
	err_pop = XtCreatePopupShell("err_pop",transientShellWidgetClass,
			top,xtargv,1);
	panerr  = XtCreateManagedWidget("form", formWidgetClass, err_pop,
				xtargv, xtargc);
	XtSetArg(xtargv[xtargc], XtNjustify,XtJustifyLeft ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNborderWidth,0 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNlabel, label); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfont,labelfont ); xtargc++;
	error = XtCreateManagedWidget("error", labelWidgetClass, panerr,
			xtargv, xtargc);
	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNfont,labelfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,dialogcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,1 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,error ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNright,XtChainLeft ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNleft,XtChainLeft ); xtargc++;
	xtcalls[0].callback =  ECancel ;
	xtcalls[0].closure = (caddr_t)err_pop;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	XtSetArg(xtargv[xtargc],XtNaccelerators,accel); xtargc++;
	okay  = XtCreateManagedWidget("Ok !", commandWidgetClass, panerr,
			xtargv, xtargc);

	XtInstallAccelerators(panerr, okay);

	return(err_pop);
}

/*
 *	Function Name: MakeDialog
 *	Description:   This function create all the dialog boxes 
 *		       used in the application and allocates memory
 *		       for application data.
 *	Parameters: top		father widget.
 *	Returns:    Pointer to memory containing application data.
 */

SearchData *MakeDialog(top)
Widget top;
{
  Widget popup, err_pop, new_pop, change_pop;
  SearchData *sstruct;

     if ((sstruct = (SearchData *) malloc(sizeof(SearchData))) == NULL){
	  fprintf(stderr,"xless: could not allocate memory\n");
	  return(NULL);
     }

     popup = DialogBox(top,Confirm,(caddr_t)sstruct,"Search for:","Search",sel);

     new_pop = DialogBox(top,NewConfirm,(caddr_t)sstruct,
			"Enter filename:", "Ok", new_sel);

     change_pop = DialogBox(top,NewFile,(caddr_t)sstruct,
			"Enter filename:", "Ok", new_sel_file);

     sstruct->popup = popup;
     sstruct->new_popup = new_pop;
     sstruct->change_pop = change_pop;
     sstruct->father = top;
     sstruct->selection = sel;
     sstruct->new_selection = new_sel;
     sstruct->new_sel_file = new_sel_file;

     return(sstruct);
}
