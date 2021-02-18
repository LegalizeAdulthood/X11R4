/* -*-C-*-
********************************************************************************
*
* File:         w_utils.c
* RCS:          $Header: w_utils.c,v 1.1 89/11/25 04:31:28 mayer Exp $
* Description:  Various X Functionality
* Author:       Niels Mayer, HPLabs
* Created:      Fri Sep 29 01:24:38 1989
* Modified:     Sat Nov 25 04:31:04 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r4 contrib tape release
*
* WINTERP 1.0 Copyright 1989 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
**
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_utils.c,v 1.1 89/11/25 04:31:28 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>	/* defines XC_crosshair */
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

/******************************************************************************
 * (X_ALLOC_COLOR <color>)
 * where <color> is a string, either a colorname from /usr/lib/X11/rgb.txt
 * or a hexadecimal color specification "#RRGGBB".
 * it returns a Pixel-value for the color.
******************************************************************************/
LVAL Wut_Prim_XAllocColor()
{
  extern Display* display;	/* global in winterp.c */
  extern Screen*  screen;	/* global in winterp.c */
  extern Colormap colormap;	/* global in winterp.c */
  XColor        screenColor;
  LVAL          str_color;

  str_color = xlgastring();
  xllastarg();
  
  if (!XParseColor(display, colormap, getstring(str_color), &screenColor))
    xlerror("XParseColor() couldn't parse color specification.", str_color);
  if (!XAllocColor(display, colormap, &screenColor))
    xlerror("XAllocColor() couldn't allocate specified color.", str_color);
  return (cv_pixel(screenColor.pixel));
}


/******************************************************************************
 * (GET_MOUSED_WIDGET)
 * evaluating this function will change the cursor to a crossbar, indicating
 * that the user is to 'click' the mouse to designate an object on the screen.
 * If the user clicks on a visual item within WINTERP, this fucntion will
 * return the WIDGETOBJ associated with the visual item. 
 ******************************************************************************/
LVAL Wut_UserClick_To_WidgetObj()
{
  extern Display* display;	/* global in winterp.c */
  extern Window   root_win;	/* global in winterp.c */
  extern LVAL     Wcls_WidgetID_To_WIDGETOBJ();	/* from w_classes.c */
  extern XmGadget _XmInputInGadget(); /* in Xm/GadetUtils.c extern'd in XmP.h */
  Cursor	  cursor = XCreateFontCursor(display, XC_crosshair);
  Window          parent_win, cur_win, child_win;
  int             win_x, win_y;
  Widget          widget_id, gadget_id;
  XEvent	  event;
  Bool            xtc_ok;
  
  xllastarg();

  if (GrabSuccess != XGrabPointer(display, root_win, 0, ButtonPressMask|ButtonReleaseMask,
				  GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime))
    xlfail("GET_MOUSED_WIDGET -- couldn't grab pointer (XGrabPointer() failed).");
  
  XWindowEvent(display, root_win, ButtonPressMask, &event); /* remove the buttonpress from the queue*/
  XWindowEvent(display, root_win, ButtonReleaseMask, &event); /* get the buttonrelease event */
  XUngrabPointer(display, CurrentTime);
  XFlush(display);

  if (!event.xbutton.subwindow)
    xlfail("GET_MOUSED_WIDGET aborted -- you clicked on the root window.");

  parent_win = event.xbutton.window; /* ASSERT event.xbutton.window == root_win, due to using XWindowEvent(root_win) */
  win_x      = event.xbutton.x;
  win_y      = event.xbutton.y;
  cur_win    = event.xbutton.subwindow;
  while ((xtc_ok = XTranslateCoordinates(display,
					 parent_win, cur_win,
					 win_x, win_y, /* give the x,y coords of event in parent_w */
					 &win_x, &win_y, /* return the x,y coords relative to cur_win */
					 &child_win)) /* returns child window of cur_win if that contains coords, else nil */
	 && child_win) {
#ifdef DEBUG_WINTERP_1
    fprintf(stderr, "parent_win=%lx, cur_win=%lx, child_win=%lx\n", parent_win, cur_win, child_win);
#endif
    parent_win = cur_win;
    cur_win    = child_win;
  }

#ifdef DEBUG_WINTERP_1
  fprintf(stderr, "	Smallest window containing userclick is %lx\n", cur_win);
#endif

  if (!xtc_ok)
    xlfail("Bug in GET_MOUSED_WIDGET -- XTranslateCoordinates() failed.");

  if (!(widget_id = XtWindowToWidget(display, cur_win)))
    xlfail("GET_MOUSED_WIDGET -- Couldn't find widget associated with window.\n	(Is the selected widget/window inside a different application?).\n");

  /* if the widget is a composite it may be managing a gadget -- attempt to retrieve it by looking up x,y coords in manager */
  if (XtIsComposite(widget_id) &&
      (gadget_id = (Widget) _XmInputInGadget(widget_id, win_x, win_y)))
    return (Wcls_WidgetID_To_WIDGETOBJ(gadget_id)); /* then return the WIDGETOBJ assoc'd with gadget */
  else
    return (Wcls_WidgetID_To_WIDGETOBJ(widget_id)); /* otherwise, we return the WIDGETOBJ assoc'd with smallest window */
}
