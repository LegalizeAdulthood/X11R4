/* -*-C-*-
********************************************************************************
*
* File:         display_def.c
* RCS:          $Header: display_def.,v 1.10 89/03/06 17:17:26 mayer Exp $
* Description:  Module for clearing, appending to and displaying a definition.
* Author:       Niels Mayer, HPLabs
* Created:      Wed Aug 31 19:17:42 1988
* Modified:     Mon Mar  6 17:16:58 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: display_def.,v 1.10 89/03/06 17:17:26 mayer Exp $";

#include "xwebster.h"
#include "user_prefs.h"
#include <Xw/SText.h>
#include <Xw/SWindow.h>
#include <Xw/TextEdit.h>

static Widget display_Wgt = NULL;
static char   text[16384]; /* hopefully no def will be longer than this! */
static int    end_idx = 0;


/******************************************************************************
 * KLUDGE_O_RAMA: on select, invert the text in the StaticText widget to show 
 * that something's being done and store the definition in the cut buffer.
 ******************************************************************************/
static XtCallbackProc Display_Select_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Pixel foreground;
  Pixel background;
  char* displ_text;

  ARGLIST_RESET();
  ARGLIST_ADD(XtNforeground, &foreground);
  ARGLIST_ADD(XtNbackground, &background);
  XtGetValues(widget, ARGLIST());
  ARGLIST_RESET();
  ARGLIST_ADD(XtNforeground, background);
  ARGLIST_ADD(XtNbackground, foreground);
  XtSetValues(widget, ARGLIST());
  
  ARGLIST_RESET();
  ARGLIST_ADD(XtNstring, &displ_text);
  XtGetValues(display_Wgt, ARGLIST());
  XStoreBuffer(XtDisplay(widget), displ_text, strlen(displ_text), 0);
}


/******************************************************************************
 * KLUDGE_O_RAMA: on release, uninvert the text in the StaticText widget.
 ******************************************************************************/
static XtCallbackProc Display_Release_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Pixel foreground;
  Pixel background;

  /* 
   * Uninvert the display that was inverted in Display_Select_Callback
   */
  ARGLIST_RESET();
  ARGLIST_ADD(XtNforeground, &foreground);
  ARGLIST_ADD(XtNbackground, &background);
  XtGetValues(widget, ARGLIST());
  ARGLIST_RESET();
  ARGLIST_ADD(XtNforeground, background);
  ARGLIST_ADD(XtNbackground, foreground);
  XtSetValues(widget, ARGLIST());
}


/******************************************************************************
 *
 ******************************************************************************/
Display_Def_Init(w)
     Widget w;
{
  XFontStruct *font;

  if (user_prefs.use_editor_as_display) {
    /* 
     * Create a TextEdit widget to use for the display.
     */
    ARGLIST_RESET();
    ARGLIST_ADD(XtNstring, user_prefs.help_text);
    display_Wgt = XtCreateManagedWidget("display", XwtexteditWidgetClass, 
					w, ARGLIST());
    ARGLIST_RESET();
    ARGLIST_ADD(XtNfont, &font);
    XtGetValues(display_Wgt, ARGLIST());
    ARGLIST_RESET();
    ARGLIST_ADD(XtNheight, (Dimension) (user_prefs.display_height
     *FONTHEIGHT(font) + 6));
    ARGLIST_ADD(XtNwidth,  (Dimension) (user_prefs.display_width
     *FONTWIDTH(font)));
    XtSetValues(display_Wgt, ARGLIST());
  }
  else {
    /*
     * Create a statictextwidget within a scrolled window as the display.
     */
    Widget display_scroller_Wgt = XtCreateManagedWidget("display_scroller", 
							XwswindowWidgetClass, w, 
							NULL, 0);
    ARGLIST_RESET();
    ARGLIST_ADD(XtNstring, user_prefs.help_text);
    display_Wgt = XtCreateManagedWidget("display", 
					XwstatictextWidgetClass, 
					display_scroller_Wgt, ARGLIST());
    XtAddCallback(display_Wgt, XtNselect, Display_Select_Callback, NULL);
    XtAddCallback(display_Wgt, XtNrelease, Display_Release_Callback, NULL);

    ARGLIST_RESET();
    ARGLIST_ADD(XtNfont, &font);
    XtGetValues(display_Wgt, ARGLIST());
    ARGLIST_RESET();
    ARGLIST_ADD(XtNheight, (Dimension) (user_prefs.display_height
     *FONTHEIGHT(font) + 6));
    ARGLIST_ADD(XtNwidth,  (Dimension) (user_prefs.display_width
     *FONTWIDTH(font)));
    XtSetValues(display_scroller_Wgt, ARGLIST());
  }
}


/******************************************************************************
 *
 ******************************************************************************/
Display_Def_Text_Reset()
{
  if (user_prefs.use_editor_as_display) {
    XwTextUnsetSelection(display_Wgt); /* XwTextClearBuffer() should do this */
    XwTextClearBuffer(display_Wgt);
  }
  else {
    ARGLIST_RESET();
    ARGLIST_ADD(XtNstring, "");
    XtSetValues(display_Wgt, ARGLIST());
    end_idx = 0;
  }
}


/******************************************************************************
 *
 ******************************************************************************/
Display_Def_Text_Append(intext)
     char* intext;
{
  if (user_prefs.use_editor_as_display) {
    XwTextInsert(display_Wgt, intext);
  }
  else {
    int intext_length = strlen(intext);
    strncpy(&text[end_idx], intext, intext_length);
    end_idx += intext_length;
  }
}


/******************************************************************************
 *
 ******************************************************************************/
Display_Def_Refresh()
{
  if (user_prefs.use_editor_as_display) {
    /*
     * When done writing definition to display_Wgt, move cursor back to home.
     */
    XwTextSetInsertPos(display_Wgt, 0);	
  }
  else {
    /*
     * When done writing def to the buffer, display the text.
     */
    text[end_idx] = '\000';
    ARGLIST_RESET();
    ARGLIST_ADD(XtNstring, text);
    XtSetValues(display_Wgt, ARGLIST());
  }
}


/******************************************************************************
 *
 ******************************************************************************/
Display_Def_Show_Help()
{
  ARGLIST_RESET();
  ARGLIST_ADD(XtNstring, user_prefs.help_text);
  XtSetValues(display_Wgt, ARGLIST());
}
