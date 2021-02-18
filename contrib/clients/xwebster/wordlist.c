/* -*-C-*-
********************************************************************************
*
* File:         wordlist.c
* RCS:          $Header: wordlist.c,v 1.8 89/03/06 17:17:19 mayer Exp $
* Description:  Module to display a list of words
* Author:       Niels Mayer, HPLabs
* Created:      Wed Aug 31 21:17:29 1988
* Modified:     Mon Mar  6 17:17:00 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wordlist.c,v 1.8 89/03/06 17:17:19 mayer Exp $";

#include "xwebster.h"
#include "user_prefs.h"
#include <Xw/SWindow.h>
#include <Xw/RCManager.h>
#include <Xw/PButton.h>

static Widget wordlist_Wgt = NULL;
static Widget wordlist_scroller_Wgt = NULL;
static Widget wordlist[16384];	/* hopefully, people won't be listing this many words */
static int    wordlist_base = 0; /* points to the first elt to be managed */
static int    wordlist_idx = 0;	/* points to the elt after last in wordlist */

extern XtCallbackProc Controlpanel_Define_Callback();

/******************************************************************************
 *
 ******************************************************************************/
Wordlist_Init(parent_Wgt)
     Widget parent_Wgt;
{
  wordlist_scroller_Wgt = XtCreateManagedWidget("wordlist_scroller", 
						XwswindowWidgetClass,
						parent_Wgt, NULL, 0);
  wordlist_Wgt = XtCreateManagedWidget("wordlist", 
				       XwrowColWidgetClass,
				       wordlist_scroller_Wgt, NULL, 0);
}


/******************************************************************************
 * Clears all the buttons.
 ******************************************************************************/
Wordlist_Reset()
{
  register int i;

  /*
   * the following seems to prevent a ridiculous amount of display updating 
   * thatwas occuring as each widget got deleted and others moved to fill the
   * gap
   */
  XtUnmanageChildren(wordlist, wordlist_idx);

  /*
   * Now we can delete all the word widgets with a minimum of display noize
   */
  for (i=0; (i < wordlist_idx); i++) {
    XtDestroyWidget(wordlist[i]);
  }
  wordlist_idx = 0;
  wordlist_base = 0;
}

/******************************************************************************
 *
 ******************************************************************************/
static XtCallbackProc Wordlist_Button_Destroy_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  XtFree(client_data);		/* frees the string containing the word */
}

/******************************************************************************
 *
 ******************************************************************************/
Wordlist_Add(str)
     char* str;
{
  Widget wordbutton_Wgt;
  char* word;

  ARGLIST_RESET();
  ARGLIST_ADD(XtNlabel, str);
  wordbutton_Wgt = XtCreateWidget("word", XwpushButtonWidgetClass,
			wordlist_Wgt, ARGLIST());
  word = COPY_STRING(str);
  XtAddCallback(wordbutton_Wgt, XtNselect, 
		Controlpanel_Define_Callback, word);

  XtAddCallback(wordbutton_Wgt, XtNdestroyCallback, /* on destroy, of Wgt, dealloc the word */
		Wordlist_Button_Destroy_Callback, word);
  XtSetSensitive(wordbutton_Wgt, FALSE);
  wordlist[wordlist_idx++] = wordbutton_Wgt;

  /*
   * Refresh the display of words every <num_words_before_refresh> words
   */
  if ((wordlist_idx % user_prefs.num_words_before_refresh) == 0) {
    XtManageChildren(&wordlist[wordlist_base], (wordlist_idx - wordlist_base));
    wordlist_base += user_prefs.num_words_before_refresh;
  }
}

/******************************************************************************
 * 
 ******************************************************************************/
Wordlist_Show()
{
  register int i;
  
  for (i=0; (i < wordlist_idx); i++) {
    XtSetSensitive(wordlist[i], TRUE);
  }
  XtManageChildren(wordlist, wordlist_idx);
}

/******************************************************************************
 * Desensitizes wordlist buttons
 ******************************************************************************/
Wordlist_Deactivate()
{
  register int i;

  for (i=0; (i < wordlist_idx); i++) {
    XtSetSensitive(wordlist[i], FALSE);
  }
}

/******************************************************************************
 * Resensitizes wordlist buttons
 ******************************************************************************/
Wordlist_Reactivate()
{
  register int i;

  ARGLIST_RESET();
  ARGLIST_ADD(XtNset, FALSE);
  for (i=0; (i < wordlist_idx); i++) {
    XtSetSensitive(wordlist[i], TRUE);
    XtSetValues(wordlist[i], ARGLIST());
  }
}
