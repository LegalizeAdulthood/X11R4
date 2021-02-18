#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Gadget.c,v 5.17 89/11/22 15:14:45 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

/*
 * MODIFICATION HISTORY
 *
 * 001 -- Jortan Hubbard, Ardent Computer.
 * Gadget boxes in title bars.
 * 1.2 -- Jordan Hubbard, Ardent Computer.
 *  GadgetBorders used in placement calculations, initial
 *  gadget positions calculated correcly. (Chris Thewalt fixes).
 * 1.3 -- Color handling changed to deal with multiple colormaps.
 *
 */

#include "awm.h"
#include "X11/Xutil.h"

extern XContext AwmContext;

AwmInfoPtr IsGadgetWin(w, gnum)
Window w;
int *gnum;
{
     AwmInfoPtr awi;
     int i;

     Entry("IsGadgetWin")

     if (!XFindContext(dpy, w, AwmContext, &awi)) {
          if (!awi->gadgets)
               Leave((AwmInfoPtr)NULL)
	  for (i = 0; i < NumGadgets; i++)
	       if (awi->gadgets[i] == w) {
                    if (gnum)
                    *gnum = i;
	            Leave(awi)
               }
          Leave((AwmInfoPtr)NULL)
     }
}

/*
 * This routine adds and/or places  gadgets onto a title bar.
 */
void PutGadgets(w)
Window w;
{
     int i, dir;
     int rx, lx, nx, ny;
     XWindowAttributes xwa;
     XSetWindowAttributes swa;
     unsigned long valuemask;
     GadgetDecl *gdec;
     AwmInfo *awi;
     Boolean change = FALSE;
     Window tmp;

     Entry("PutGadgets")

     /* are there any gadgets declared? */
     if (NumGadgets < 1)
	  Leave_void
     /* does this window have a title? */
     if (!(awi = IsTitled(w)))
	  Leave_void

     if (!(awi->attrs & AT_GADGETS))
	  Leave_void
     /* if the window already has gadgets, this is a reshuffle */
     if (awi->gadgets)
	  change = TRUE;
     else {
	  awi->gadgets = (Window *)malloc(sizeof(Window) * (NumGadgets + 1));
	  /* why the + 1, I can't say, except that otherwise it get overrun */
     }
     if (!awi->gadgets) {
	  Warning("PutGadgets: Can't allocate memory for gadgets!");
	  Leave_void
     }
     if (! XGetWindowAttributes(dpy, awi->title, &xwa)) {
	  (void) sprintf(error_msg, "PutGadgets: Can't get window attributes for title bar window %x, client window %x", awi->title, awi->client);
	  Warning(error_msg);
	  Leave_void
     }
     /* define some initial positioning information */
     dir = RightGadgetGravity;
     lx = 0;
     rx = xwa.width - GadgetBorder;
     swa.event_mask = (ButtonPressMask | ButtonReleaseMask | ExposureMask);
     swa.colormap = awi->client_cmap;
     valuemask = CWEventMask | CWColormap;
     for (i = 0; i < NumGadgets && Gadgets[i]; i++) {
	  gdec = Gadgets[i];

	  /* Does gadget have its own gravity? */
	  if (gdec->gravity != NoGadgetGravity)
	       dir = gdec->gravity;
	  ny = xwa.height - gdec->high - (GadgetBorder * 2);
	  if (ny < 0) {
	       ny = 0;
               gdec->high = xwa.height - (GadgetBorder * 2);
          }
	  else
	       ny = (ny / 2) - 1;
	  if (ny < 0)
	       ny = 0;
	  if (dir == LeftGadgetGravity) {
	       nx = lx + gdec->offset;  /* position of outside left border */
	       if (!gdec->offset && (lx != 0))
		    /* not first time through and no offset, add pad */
		    nx += GadgetPad;
	       if (nx < 0)
		    nx = 0;
	       lx = nx + gdec->wide + GadgetBorder;
	       /* with no pad, one gadget border width between gadgets. */
               dir = RightGadgetGravity;
	  }
	  else if (dir == RightGadgetGravity) {	
	       nx = rx - gdec->wide - gdec->offset - GadgetBorder;
	       if (!gdec->offset && (rx != (xwa.width - GadgetBorder)))
		    nx -= GadgetPad;
	       if (nx > (xwa.width - gdec->wide - (GadgetBorder * 2)))
		    nx = xwa.width - gdec->wide - (GadgetBorder * 2);
	       /* keep gadget fully within titlebar even if neg offset */
	       rx = nx;
               dir = LeftGadgetGravity;
	  }
	  else /* Center Gravity */
	       nx = ((xwa.width - gdec->wide) / 2) + gdec->offset - GadgetBorder;
	  if (change == FALSE || !awi->gadgets[i]) {
	       tmp = awi->gadgets[i] = XCreateWindow(dpy, awi->title, nx, ny,
						     gdec->wide, gdec->high,
						     GadgetBorder,
						     xwa.depth, xwa.class,
						     xwa.visual, valuemask,
						     &swa);
	       if (!tmp) {
		    (void) sprintf(error_msg, "PutGadgets: Can't create gadget #%d",
			    i);
		    Warning(error_msg);
		    Leave_void
	       }
	       /* must do this before mapping */
	       SetGadgetColor(awi, i);
	       XMapWindow(dpy, tmp);
	       XSaveContext(dpy, tmp, AwmContext, awi);
	  }
	  else
	       XMoveWindow(dpy, awi->gadgets[i], nx, ny);
     }
     if (change == FALSE) /* make sure the window array is null terminated */
          awi->gadgets[i] = (Window)NULL;
     PaintGadgets(w);
     Leave_void
}

int FreeGadget(n)
int n;
{
     Entry("FreeGadget")

     if (Gadgets[n]->data)
	  XFree(Gadgets[n]->data);
     if (Gadgets[n]->name)
	  free(Gadgets[n]->name);
}

int CheckGadgets()
{
     int i, status = 0;

     Entry("CheckGadgets")

     for (i = 0; i < NumGadgets; i++)
	  if (!Gadgets[i]) {
	       (void) sprintf(error_msg, "Gadget #%d is not declared!",
		       i);
	       Warning(error_msg);
	       fprintf(stderr, "All gadgets from 0 to %d must be declared.\n",
		       NumGadgets - 1);
	       status = 1;
	  }
     Leave(status)
}

void ResetGadgets()
{
     int i;

     Entry("ResetGadgets")

     for (i = 0; i < NumGadgets; i++) {
	  FreeGadget(i);
	  free(Gadgets[i]);
	  Gadgets[i] = 0;
     }
     /* reset overall gadget height */
     gadgetHeight = 0;
     NumGadgets = 0;
     Leave_void
}
     
int DestroyGadgets(w)
Window w;
{
     int i;
     AwmInfoPtr awi;

     Entry("DestroyGadgets")

     if (!(awi = IsTitled(w)))
	  Leave_void
     if (!awi->gadgets)
	  Leave_void
     for (i = 0; i < NumGadgets && Gadgets[i]; i++) {
	  XDeleteContext(dpy, awi->gadgets[i], AwmContext);
	  XDestroyWindow(dpy, awi->gadgets[i]);
     }
     free(awi->gadgets);
     awi->gadgets = 0;
     Leave_void
}

    /* reset all gadget colormaps and colors */
void SetGadgetColors(awi)
AwmInfoPtr awi;
{
    int i;

    Entry("SetGadgetColors")

    if (awi->gadgets) {
	for (i = 0; i < NumGadgets && Gadgets[i]; i++) {
	    SetGadgetColor(awi, i);
	}
    }

    Leave_void
}

    /* reset one gadget colormap and color */
void SetGadgetColor(awi, i)
AwmInfoPtr awi;
int i;
{
    XSetWindowAttributes swa;
    unsigned long valuemask;
    GadgetDecl *gdec;

    Entry("SetGadgetColor")

    swa.colormap = awi->client_cmap;
    valuemask = CWColormap;
    gdec = Gadgets[i];
    swa.background_pixmap = 0;
    if (gdec->data) {
	/* XXX small Optimization - if this pixmap is same as last, 
	    don't recreate */
	swa.background_pixmap = 
	XCreatePixmapFromBitmapData(dpy, awi->gadgets[i], gdec->data,
			   gdec->wide, gdec->high,
			   GetPixel(gdec->forecolor, awi->client_cmap),
			   GetPixel(gdec->backcolor, awi->client_cmap),
			   awi->client_depth);
	if (!swa.background_pixmap) {
	    (void) sprintf(error_msg, "SetGadgetColor: Unable to create pixmap for gadget[%d], using grey.", i);
	    Warning(error_msg);
	    swa.background_pixmap = GrayPixmap;
	}
	valuemask |= CWBackPixmap;
    }
    else {
	swa.background_pixel = GetPixel(gdec->backcolor, awi->client_cmap);
	valuemask |= CWBackPixel;
    }

    XChangeWindowAttributes(dpy, awi->gadgets[i], valuemask, &swa);
    if (swa.background_pixmap && (swa.background_pixmap != GrayPixmap))
	XFreePixmap(dpy, swa.background_pixmap);
    Leave_void
}

void PaintGadgets(w)
Window w;
{
     int i, y, x = 0;
     AwmInfoPtr awi;
     GadgetDecl *gdec;
     XWindowAttributes xwa;

     Entry("PaintGadgets")

     if (!(awi = IsTitled(w)))
	  Leave_void
     if (!awi->gadgets)
	  Leave_void
     XGetWindowAttributes(dpy, awi->client, &xwa);
     for (i = 0; i < NumGadgets && Gadgets[i]; i++) {
	  gdec = Gadgets[i];
	  if (!gdec->data) {
	       XGCValues gcv;

	       gcv.font = gdec->fontInfo->fid;
	       gcv.foreground = GetPixel(gdec->forecolor, xwa.colormap);
	       gcv.background = GetPixel(gdec->backcolor, xwa.colormap);
	       XChangeGC(dpy, awi->winGC, (GCFont | GCForeground |
					   GCBackground), &gcv);
	       if (strlen(gdec->name) == 1) { /* it's a glyph (we think) */
		    XFontStruct *real_font;
		    XCharStruct char_info;
		    int char_number, index, byte1, byte2;

		    real_font = gdec->fontInfo;
		    char_number = gdec->name[0];
		    char_info = real_font->max_bounds;
		    index = char_number;

		    if (real_font->per_char) {
			 if (!real_font->min_byte1 && !real_font->max_byte1) {
			      if (char_number < real_font->min_char_or_byte2 ||
				  char_number > real_font->max_char_or_byte2)
				   index = real_font->default_char;
			      index -= real_font->min_char_or_byte2;
			 } else {
			      byte2 = index & 0xff;
			      byte1 = (index>>8) & 0xff;
			      if (byte1 < real_font->min_byte1 ||
				  byte1 > real_font->max_byte1 ||
				  byte2 < real_font->min_char_or_byte2 ||
				  byte2 > real_font->max_char_or_byte2) {
				   byte2 = real_font->default_char & 0xff;
				   byte1 = (real_font->default_char>>8)&0xff;
			      }
			      byte1 -= real_font->min_byte1;
			      byte2 -= real_font->min_char_or_byte2;
			      index = byte1 * (real_font->max_char_or_byte2 -
					       real_font->min_char_or_byte2 + 1) +
						    byte2;
			 }
			 char_info = real_font->per_char[index];
		    }
		    x = 1 - GadgetBorder - char_info.lbearing;
		    y = char_info.ascent;
	       }
	       else {
		    y = (gdec->high + (gdec->fontInfo->max_bounds.ascent -
				       gdec->fontInfo->max_bounds.descent)) / 2;
		    x = GadgetBorder;
	       }
	       XDrawString(dpy, awi->gadgets[i], awi->winGC, x,
			   y, gdec->name, strlen(gdec->name));
	  }
     }
     Leave_void
}
