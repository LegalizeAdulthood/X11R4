/* -*-C-*-
********************************************************************************
*
* File:         controlpanel.c
* RCS:          $Header: controlpanel,v 1.9 89/03/06 16:11:55 mayer Exp $
* Description:  Control Panel for Webster
* Author:       Niels Mayer, HPLabs
* Created:      Wed Aug 31 21:40:58 1988
* Modified:     Mon Mar  6 16:08:53 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       G-Job
*
* xwebster - dictionary browser
*
* Copyright 1988 Hewlett-Packard Company
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose and without fee is hereby granted, provided
* that the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of HP not be used in advertising or
* publicity pertaining to distribution of the software without specific,
* written prior permission.  HP makes no representations about the
* suitability of this software for any purpose.  It is provided "as is"
* without express or implied warranty.
*
* Please send any improvements, bug fixes, useful modifications, and comments
* to mayer@hplabs.hp.com.
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: controlpanel,v 1.9 89/03/06 16:11:55 mayer Exp $";

#include "xwebster.h"
#include "user_prefs.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <Xw/PButton.h>
#include <Xw/SText.h>
#include <Xw/TextEdit.h>
#include <Xw/TitleBar.h>

static Widget control_panel_Wgt = NULL;
static Widget word_input_Wgt = NULL;
static Widget define_button_Wgt = NULL;
static Widget complete_button_Wgt = NULL;
static Widget endings_button_Wgt = NULL;
static Widget titlebar_Wgt = NULL;
static Widget quit_button_Wgt = NULL;
static Widget help_button_Wgt = NULL;
static int    controlpanel_active = TRUE;
static int    cur_word_is_from_textedit = TRUE;

extern int   Webster_Send(/* char* buf */);  /* from xwebster.c */

/******************************************************************************
 * note that this callback is used for 3 different callbacks:
 * 1) called from the controlpanel define_button_Wgt with client_data==NULL,
 *    this proc. assumes that word to be defined is in the word_input_Wgt.
 * 2) called from Controlpanel_Define_Action (which is a actionproc that is
 *    called from the word_input_Wgt) with client_data==NULL, this proc.
 *    again assumes that word to be defined is in the word_input_Wgt.
 * 3) called from a callback on a wordbutton_Wgt in module Wordlist, 
 *    client_data is set to the word to be defined. In this case, 
 *    subsequent calls to Controlpanel_Cur_Word_Is_From_TextEdit()
 *    will return false.
 ******************************************************************************/
XtCallbackProc Controlpanel_Define_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  char buf[BUFSIZ];
  extern Display* display;

  if (!controlpanel_active) {
    XBell(display, 100);
    return(NULL);
  }

  Display_Def_Text_Reset();	/* clear the def buffer to show text is coming */
  ARGLIST_RESET();
  ARGLIST_ADD(XtNset, TRUE);	/* show em which button they hit even if a key-*/
  XtSetValues(define_button_Wgt, ARGLIST()); /*-action resulted in the callback*/
  Controlpanel_Deactivate();	/* disallow further input till ready. */
  
  if (client_data == NULL) {	/* IF not called from Wordlist word button */
    char* word;			/* THEN get the word from word_input_Wgt */
    word = (char *) XwTextCopyBuffer(word_input_Wgt);
    sscanf(word, "%[^\n]", temptext); /* get only one word, no newlines */
    XtFree(word);
    sprintf(buf, "DEFINE %s\r\n", temptext);
    cur_word_is_from_textedit = TRUE;
  }
  else {			/* ELSE get the word from the callback data */
    sprintf(buf, "DEFINE %s\r\n", (char*)client_data);
    Controlpanel_Set_Input_Word((char*)client_data);
    cur_word_is_from_textedit = FALSE;
  }
  if (!Webster_Send(buf))
    Controlpanel_Reactivate();
}

/******************************************************************************
 *
 ******************************************************************************/
static void Controlpanel_Define_Action(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  Controlpanel_Define_Callback(w, NULL, NULL);
}

/******************************************************************************
 *
 ******************************************************************************/
static XtCallbackProc Controlpanel_Complete_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  char buf[BUFSIZ];
  char* word;
  extern Display* display;

  if (!controlpanel_active) {
    XBell(display, 100);
    return(NULL);
  }

  ARGLIST_RESET();
  ARGLIST_ADD(XtNset, TRUE);	/* show em which button they hit even if a key-*/
  XtSetValues(complete_button_Wgt, ARGLIST()); /*-action resulted in the callback*/
  Controlpanel_Deactivate();	/* disallow further input till ready. */
  
  word = (char *) XwTextCopyBuffer(word_input_Wgt);
  sscanf(word, "%[^\n]", temptext); /* don't get newlines */
  XtFree(word);
  sprintf(buf, "COMPLETE %s\r\n", temptext);
  cur_word_is_from_textedit = TRUE;

  if (!Webster_Send(buf))
    Controlpanel_Reactivate();
}

/******************************************************************************
 *
 ******************************************************************************/
static void Controlpanel_Complete_Action(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  Controlpanel_Complete_Callback(w, NULL, NULL);
}

/******************************************************************************
 *
 ******************************************************************************/
static XtCallbackProc Controlpanel_Endings_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  char buf[BUFSIZ];
  char* word;
  extern Display* display;

  if (!controlpanel_active) {
    XBell(display, 100);
    return(NULL);
  }
  
  ARGLIST_RESET();
  ARGLIST_ADD(XtNset, TRUE);	/* show em which button they hit even if a key-*/
  XtSetValues(endings_button_Wgt, ARGLIST()); /*-action resulted in the callback*/
  Controlpanel_Deactivate();	/* disallow further input till ready. */
  
  word = (char *) XwTextCopyBuffer(word_input_Wgt);
  sscanf(word, "%[^\n]", temptext); /* get only one word, no newlines */
  XtFree(word);
  sprintf(buf, "ENDINGS %s\r\n", temptext);
  cur_word_is_from_textedit = TRUE;

  if (!Webster_Send(buf))
    Controlpanel_Reactivate();
}

/******************************************************************************
 *
 ******************************************************************************/
static void Controlpanel_Endings_Action(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  Controlpanel_Endings_Callback(w, NULL, NULL);
}


/******************************************************************************
 *
 ******************************************************************************/
static XtCallbackProc Controlpanel_Quit_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  /* 
   * note that the quit button is the only one that remains active all the time.
   * That's becauese it's ok to quit xwebster anytime even when it hasn't
   * completed it's transaction with the webster server.
   */
  extern int webster_Socket;	/* in main.c */
  close(webster_Socket);
  exit(0);
}


/******************************************************************************
 *
 ******************************************************************************/
static XtCallbackProc Controlpanel_Help_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  extern Display* display;

  if (!controlpanel_active) {
    XBell(display, 100);
    return(NULL);
  }

  Display_Def_Show_Help();
}


/******************************************************************************
 *
 ******************************************************************************/
static void Controlpanel_Noop_Action(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  extern Display* display;

  XBell(display, 100);
}


XtActionsRec word_input_Wgt_actions_table [] = {
  {"webster-define-word",   Controlpanel_Define_Action},
  {"webster-complete-word", Controlpanel_Complete_Action},
  {"webster-list-endings",  Controlpanel_Endings_Action},
  {"webster-no-op",         Controlpanel_Noop_Action},
};

/******************************************************************************
 *
 ******************************************************************************/
Controlpanel_Init(w)
     Widget w;
{
  control_panel_Wgt = XtCreateManagedWidget("control_panel", 
					    XwtitlebarWidgetClass, 
					    w, NULL, 0);
  word_input_Wgt = XtCreateManagedWidget("word_input", XwtexteditWidgetClass,
					 control_panel_Wgt, NULL, 0);
  { 
    XFontStruct *font;
    ARGLIST_RESET();
    ARGLIST_ADD(XtNfont, &font);
    XtGetValues(word_input_Wgt, ARGLIST());
    ARGLIST_RESET();
    ARGLIST_ADD(XtNheight, (Dimension) (FONTHEIGHT(font) + 6));
    ARGLIST_ADD(XtNwidth,  (Dimension) (user_prefs.word_input_Wgt_width
     * FONTWIDTH(font)));
    XtSetValues(word_input_Wgt, ARGLIST());
  }
  XtAddActions(word_input_Wgt_actions_table, 
	       XtNumber(word_input_Wgt_actions_table));
  
  define_button_Wgt = XtCreateManagedWidget("define_button", XwpushButtonWidgetClass, 
					    control_panel_Wgt, NULL, 0);
  XtAddCallback(define_button_Wgt, XtNselect, 
		Controlpanel_Define_Callback, NULL);

  complete_button_Wgt = XtCreateManagedWidget("complete_button", XwpushButtonWidgetClass, 
					      control_panel_Wgt, NULL, 0);
  XtAddCallback(complete_button_Wgt, XtNselect, 
		Controlpanel_Complete_Callback, NULL);

  endings_button_Wgt = XtCreateManagedWidget("endings_button", XwpushButtonWidgetClass, 
					     control_panel_Wgt, NULL, 0);
  XtAddCallback(endings_button_Wgt, XtNselect, 
		Controlpanel_Endings_Callback, NULL);

  {/*
    * force the controlpanel to have no sash's -- do this by setting XtNmin
    * and XtNmax constraint resources to the same values -- the height of the
    * widget. See XWVPANED(3x) for details.
    */
    Dimension height;
    ARGLIST_RESET();
    ARGLIST_ADD(XtNheight, &height);
    XtGetValues(control_panel_Wgt, ARGLIST());
    ARGLIST_RESET();
    ARGLIST_ADD(XtNmin, height); 
    ARGLIST_ADD(XtNmax, height);
    XtSetValues(control_panel_Wgt, ARGLIST());
  }
}

/******************************************************************************
 *
 ******************************************************************************/
Controlpanel_Titlebar_Init(parent_Wgt)
     Widget parent_Wgt;
{
  extern char xwebster_version[];

  ARGLIST_RESET();
  ARGLIST_ADD(XtNstring, xwebster_version);
  titlebar_Wgt = XtCreateManagedWidget("titlebar", XwtitlebarWidgetClass, 
				       parent_Wgt, ARGLIST());
  { /** force the title{back,fore}ground to be the same as {back,fore}ground**/
    Pixel foreground, background;
    ARGLIST_RESET();
    ARGLIST_ADD(XtNforeground, &foreground);
    ARGLIST_ADD(XtNbackground, &background);
    XtGetValues(titlebar_Wgt, ARGLIST());
    ARGLIST_RESET();
    ARGLIST_ADD(XtNtitleForeground, foreground); 
    ARGLIST_ADD(XtNtitleBackground, background);
    XtSetValues(titlebar_Wgt, ARGLIST());
  }
  quit_button_Wgt = XtCreateManagedWidget("quit_button", XwpushButtonWidgetClass,
					  titlebar_Wgt, NULL, 0);
  XtAddCallback(quit_button_Wgt, XtNselect, Controlpanel_Quit_Callback, NULL);
  
  help_button_Wgt = XtCreateManagedWidget("help_button", XwpushButtonWidgetClass,
					  titlebar_Wgt, NULL, 0);
  XtAddCallback(help_button_Wgt, XtNselect, Controlpanel_Help_Callback, NULL);

  {/*
    * force the titlebar to have no sash's -- do this by setting XtNmin
    * and XtNmax constraint resources to the same values -- the height of the
    * widget. See XWVPANED(3x) for details.
    */
    Dimension height;
    ARGLIST_RESET();
    ARGLIST_ADD(XtNheight, &height);
    XtGetValues(titlebar_Wgt, ARGLIST());
    ARGLIST_RESET();
    ARGLIST_ADD(XtNmin, height); 
    ARGLIST_ADD(XtNmax, height);
    XtSetValues(titlebar_Wgt, ARGLIST());    
  }
}


/******************************************************************************
 *
 ******************************************************************************/
Controlpanel_Titlebar_Set_Label(str)
     char* str;
{
  ARGLIST_RESET(); 
  ARGLIST_ADD(XtNstring, str);
  XtSetValues(titlebar_Wgt, ARGLIST());
}


/******************************************************************************
 *
 ******************************************************************************/
Controlpanel_Deactivate()
{
  controlpanel_active = FALSE;
  
#ifndef SLOW_X_SERVER
  XtSetSensitive(define_button_Wgt, FALSE);
  XtSetSensitive(complete_button_Wgt, FALSE);
  XtSetSensitive(endings_button_Wgt, FALSE);
  /* XtSetSensitive(quit_button_Wgt, FALSE); --> allow quit while server busy*/
  XtSetSensitive(help_button_Wgt, FALSE);
  Wordlist_Deactivate();
#endif
}

/******************************************************************************
 *
 ******************************************************************************/
Controlpanel_Reactivate()
{
  controlpanel_active = TRUE;

#ifndef SLOW_X_SERVER
  XtSetSensitive(define_button_Wgt, TRUE);
  XtSetSensitive(complete_button_Wgt, TRUE);
  XtSetSensitive(endings_button_Wgt, TRUE);
  /* XtSetSensitive(quit_button_Wgt, TRUE); */
  XtSetSensitive(help_button_Wgt, TRUE);
  Wordlist_Reactivate();
#endif

  /* 
   * the following is a lamo hack to fix a "feature" of PushButtons -- when they're
   *  pressed, they invert colors. if the push callback then insensitizes them,
   *  upon resensitizing they'll not invert back to the "off" color. So force them
   *  off here. 
   */
  ARGLIST_RESET();
  ARGLIST_ADD(XtNset, FALSE);
  XtSetValues(define_button_Wgt, ARGLIST());
  XtSetValues(complete_button_Wgt, ARGLIST());
  XtSetValues(endings_button_Wgt, ARGLIST());
  /*XtSetValues(quit_button_Wgt, ARGLIST());*/
  /*XtSetValues(help_button_Wgt, ARGLIST());*/
}

/******************************************************************************
 *
 ******************************************************************************/
Controlpanel_Set_Input_Word(word)
     char* word;
{
  XwTextUnsetSelection(word_input_Wgt); /* XwTextClearBuffer() should do this */
  XwTextClearBuffer(word_input_Wgt);
  XwTextInsert(word_input_Wgt, word);
}

/******************************************************************************
 *
 ******************************************************************************/
int Controlpanel_Cur_Word_Is_From_TextEdit()
{
  return(cur_word_is_from_textedit);
}
