/***********************************************************

$Header: Home.c,v 3.0 89/11/20 09:24:40 mikew Exp $

Copyright 1989 by Mike Wexler, Santa Clara, Ca.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Mike Wexler or not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

MIKE WEXLER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include "SM.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Shell.h>
#include <X11/Xaw/Vendor.h>
#include "Home.h"
#include "ICC.h"
#include "WinInfo.h"
#include "State.h"

Widget          homeForm;
Widget	       	confirmPopup;
Widget        	confirmForm;

/*
 * Description: Handle the user pressing the shutdown button on the home form.
 * 
 */

/* ARGSUSED */
static
void 
QuitButton(w, event, params, numParams)
    Widget          w;		/* the Quit Button Widget */
    XEvent	   *event;	/* the event that caused the is action */
    char	   *params;	/* params from the translation table */
    Cardinal	   *numParams;	/* number of params */
{
    XtPopup(confirmPopup, XtGrabNonexclusive);
    return;
}

/*
 * Description: Handle the user press the cancel button on the confirm screen.
 * 
 * Side Effects: Causes the confirmForm to be Unmanaged and the quitButton to
 */

/*ARGSUSED*/
static
void 
CancelButton(w, event, params, numParams)
    Widget          w;		/* the cancel button widget */
    XEvent	   *event;	/* the event that caused the action */
    char	   *params;	/* params from the translation table */
    Cardinal	   *numParams;	/* the number of params */
{
    XtPopdown(confirmPopup);
    return;
}

/*
 * Description: Handle the confirm button being pushed on the confirmation
 * 		screen.
 * 
 * Side Effects: Terminates program.
 */

/* ARGSUSED */
static
void 
ShutdownButton(w, event, params, numParams)
    Widget          w;		/* The quit button widget */
    XEvent	   *event;	/* the event that caused the action */
    String	   *params;	/* params from the translation table */
    Cardinal	   *numParams;	/* the number of params */
{
    Boolean         saveState;
    XrmValue	    paramString;
    XrmValue	    paramBoolean;

    if (*numParams == 1) {
	paramString.addr = params[0];
	paramString.size = sizeof(char *);
	XtConvert(w, XtRString, &paramString, XtRBoolean, &paramBoolean);
	saveState = *((Boolean *) paramBoolean.addr);
    } else {
	saveState = True;
    }
    if (saveState) {
	State        state;

	StateInit(&state);
	if (StateGet(&state, XtDisplay(homeForm)) >= 0)
	  (void) StateSave(&state);
	StateCleanup(&state);
    }
    XtDestroyWidget((Widget) homeForm);
    exit(0);
}

/*
 * Description: Create the top-level widgets.
 * 
 * Outputs:	Widget - the Home form's widget.
 * 
 */

Widget 
HomeSetup(parent)
    Widget          parent;	/* the parent shell widget */
{
    Widget          saveStateButton;
    Widget          dontSaveStateButton;
    Widget          cancelButton;
    Widget          quitButton;
    XtTranslations  newTranslations;
    static XtActionsRec actionsTable[] = {
	{"quit", QuitButton},
	{"shutdown", ShutdownButton},
	{"cancel", CancelButton},
    };
    static char quitTranslations[] = 
      "<Btn1Up>:	notify()unset()quit()";

    static char saveStateTranslations[] = 
      "<Btn1Up>:	notify()unset()shutdown(True)";
    static char dontSaveStateTranslations[] = 
      "<Btn1Up>:	notify()unset()shutdown(False)";
    static char cancelTranslations[] = 
      "<Btn1Up>:	notify()unset()cancel()";

    XtAddActions(actionsTable, XtNumber(actionsTable));
    newTranslations = XtParseTranslationTable(quitTranslations);
    homeForm = XtCreateWidget("homeForm", formWidgetClass, parent, NULL, 0);

    quitButton = XtCreateManagedWidget("quitButton", commandWidgetClass,
				       homeForm, NULL, 0);
    XtOverrideTranslations(quitButton, newTranslations);

    confirmPopup = XtCreatePopupShell("confirmPopup", vendorShellWidgetClass,
				  homeForm, NULL, 0);

    confirmForm = XtCreateWidget("confirmForm", formWidgetClass, confirmPopup,
				 NULL, 0);

    (void) XtCreateManagedWidget("confirmLabel", labelWidgetClass,
					 confirmForm, NULL, 0);

    saveStateButton = XtCreateManagedWidget("saveStateButton", commandWidgetClass,
				      confirmForm, NULL, 0);
    newTranslations = XtParseTranslationTable(saveStateTranslations);
    XtOverrideTranslations(saveStateButton, newTranslations);

    cancelButton = XtCreateManagedWidget("cancelButton", commandWidgetClass,
				     confirmForm, NULL, 0);
    newTranslations = XtParseTranslationTable(cancelTranslations);
    XtOverrideTranslations(cancelButton, newTranslations);

    dontSaveStateButton = XtCreateManagedWidget("dontSaveStateButton",
						commandWidgetClass,
						confirmForm, NULL, 0);
    newTranslations = XtParseTranslationTable(dontSaveStateTranslations);
    XtOverrideTranslations(dontSaveStateButton, newTranslations);

    return (homeForm);
}

