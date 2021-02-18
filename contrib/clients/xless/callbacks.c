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
 * $Header: /mit/tax/Xless/RCS/callbacks.c,v 1.4 89/10/11 20:32:55 carlo Exp $
 */

#include "xless.h"

extern char sel[];
extern char new_sel[];
extern char new_sel_file[];

extern char *InitData();
extern void SetReadText();
extern Widget MakeText();
extern void NewWindow();

/*
 * Button callback functions.
 */

void Quit(widget,closure,callData)
    Widget widget;
    caddr_t closure;            /* Widget */
    caddr_t callData;
{
    XtDestroyWidget((Widget)closure);
    exit(0);
}

void Search(widget,closure,callData)
    Widget widget;
    caddr_t closure;            /* Widget */
    caddr_t callData;
{
	SetPopup((Widget)closure); /* Search dialog box */
}

void Confirm(widget,closure,callData)
    Widget widget;
    caddr_t closure;
    caddr_t callData;
{
	SearchData *sd = (SearchData *)closure;

	XtPopdown((Widget)sd->popup);
	DoSearch(sd->text,sd->memory,sd->selection,sd->father);
}

void CDoSearch(widget,closure,callData)
    Widget widget;
    caddr_t closure;
    caddr_t callData;
{
	SearchData *sd = (SearchData *)closure;

	DoSearch(sd->text,sd->memory,sd->selection,sd->father);
}

void Cancel(widget,closure,callData)
    Widget widget;
    caddr_t closure;            /* Widget */
    caddr_t callData;
{
	XtPopdown((Widget)closure);
}

void ECancel(widget,closure,callData)
    Widget widget;
    caddr_t closure;            /* Widget */
    caddr_t callData;
{
	XtPopdown((Widget)closure);
	XtDestroyWidget((Widget)closure);
}

void CallEditor(widget,closure,callData)
    Widget widget;
    caddr_t closure;
    caddr_t callData;
{
    SearchData *cad = (SearchData *)closure;
    char command[128];
    char *editor, *tmp, *tmpe;
    char *emacs = "emacs";
    int isemacs = 0;

	strcpy(command,"xterm -e ");

	if ((editor = getenv(EDITORVAR)) == NULL)
		editor = DEFEDITOR;
	strcat(command,editor);
#ifdef ATHENA
/* Emacs automaticaly pops up an X window */
	tmp = editor;
	tmpe = emacs;
	while ((*tmp != '\0')&&(*tmpe != '\0')) {
		if (*tmp == *tmpe) {
			tmp++;
			tmpe++;
			isemacs = 1;
		}
		else {
			tmp++;
			tmpe = emacs;
			isemacs = 0;
		}
	} 	
	if (isemacs) {		 /* editor is emacs */
		strcpy(command,editor);
	}
#endif ATHENA
	strcat(command," ");
	strcat(command,cad->file);
	strcat(command,"&");
	system(command);
}

void Reload(widget,closure,callData)
    Widget widget;
    caddr_t closure;
    caddr_t callData;
{
	SearchData *cad = (SearchData *)closure;
	FILE *file;
	char *new_data;
	char message[128];

	if ( (file = fopen(cad->file,"r")) == NULL) {
	   strcpy(message,"Couldn't open file: ");
	   strcat(message, cad->selection);
	   PopupMessage(cad->father,message);
	}
	else {
		new_data = InitData(file);
		free(cad->memory);
		SetReadText(cad->text, new_data);
		cad->memory = new_data;
	}
}

void NewFile(widget,closure,callData)
    Widget widget;
    caddr_t closure;
    caddr_t callData;
{
	SearchData *cad = (SearchData *)closure;
	FILE *file;
	char *new_data;
	char lbl[128];
	Arg arg[2];
	char message[128];

	XtPopdown((Widget)cad->change_pop);

	if((strcmp(cad->new_sel_file,"\0")) == 0)
		return;

	cad->new_sel_file = new_sel_file;
	if ( (file = fopen(cad->new_sel_file,"r")) == NULL) {
	   strcpy(message,"Couldn't open file: ");
	   strcat(message, cad->new_sel_file);
	   PopupMessage(cad->father,message);
	}
	else {
		new_data = InitData(file);
		free(cad->memory);
		SetReadText(cad->text, new_data);
		cad->memory = new_data;
		cad->file = cad->new_sel_file;
		strcpy(lbl,"Current file is:  ");
		strcat(lbl, cad->new_sel_file);
		XtSetArg(arg[0], XtNlabel, lbl);
		XtSetValues(cad->lab, arg, 1);
	}
}

void ChangeFile(widget,closure,callData)
    Widget widget;
    caddr_t closure;		/* Widget */
    caddr_t callData;
{
	SetPopup((Widget)closure); /* New file dialog box */
}

void AskFile(widget,closure,callData)
    Widget widget;
    caddr_t closure;            /* Widget */
    caddr_t callData;
{
	SetPopup((Widget)closure); /* New window dialog box */
}

void CloseWindow(widget,closure,callData)
    Widget widget;
    caddr_t closure;
    caddr_t callData;
{
	CallData *cad = (CallData *)closure;

	XtPopdown((Widget)cad->shl);
	free(cad->mem);
}

void NewConfirm(widget,closure,callData)
    Widget widget;
    caddr_t closure;
    caddr_t callData;
{
	SearchData *sd = (SearchData *)closure;

	XtPopdown((Widget)sd->new_popup);
	NewWindow(sd->father, sd->new_selection);
}
