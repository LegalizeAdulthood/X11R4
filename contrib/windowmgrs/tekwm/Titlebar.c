#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Titlebar.c,v 5.35 89/11/22 15:14:06 glennw Exp $";
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
 * 002 -- Jordan Hubbard, U.C. Berkeley.
 Title bar support routines.
 * 1.2 -- Fairly major restructuring to uncouple title bars and
 * border contexts.
 * 1.3 -- Handling of colors revamped significantly to deal with
 * different colormaps.
 */

#include "awm.h"
#include "X11/Xutil.h"
#include "X11/cursorfont.h"

int TB_wide, TB_high, TBB_wide, TBB_high;
int  B_wide,  B_high,  BB_wide,  BB_high;
int BC_wide, BC_high, BCB_wide, BCB_high;
int vPad;
extern XContext AwmContext;

#define BLANK_WIDE	16
#define BLANK_HIGH	16
static char blank_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static Boolean Title_res, Frame_res;

void Init_Titles()
{
     TFontInfo = GetFontRes("title.font", NULL, DEF_TITLE_FONT);
     TFontBoldInfo = GetFontRes("title.boldFont", NULL, NULL);
     TBackPixmapData = GetPixmapDataRes("title.pixmap", NULL,
					&TB_wide, &TB_high);
     TBoldPixmapData = GetPixmapDataRes("title.boldPixmap", NULL,
					&TBB_wide, &TBB_high);
     TForeground = GetStringRes("title.foreground", NULL, Foreground);
     TBackground = GetStringRes("title.background", NULL, Background);
     TTextForeground = GetStringRes("title.text.foreground", NULL, TForeground);
     TTextBackground = GetStringRes("title.text.background", NULL, TBackground);


     TCursor = GetIntRes("title.cursor", NULL, DEF_TITLE_CURSOR);

     TitleHeight = GetIntRes("title.height", NULL, 0);
     vPad = GetIntRes("title.vPad", "Title.Pad", DEF_TITLE_PAD);
     NameOffset = GetIntRes("windowName.offset", NULL, 0);
     /* we've specified a bold pixmap, but no back pixmap. Use blank. */
     if (TBoldPixmapData && !TBackPixmapData) {
	  TBackPixmapData = blank_bits;
	  TB_wide = BLANK_WIDE;
	  TB_high = BLANK_HIGH;
     }
     if (TitleHeight) {
	  /* non-zero height overrides derived height */
	  titleHeight = TitleHeight;
     }
     else {
	 int foo;

	 if(ShowName) {
             titleHeight = TFontInfo->max_bounds.ascent +
			   TFontInfo->max_bounds.descent + (vPad * 2);
	     if (TFontBoldInfo) {
		 foo = TFontBoldInfo->max_bounds.ascent +
		       TFontBoldInfo->max_bounds.descent + (vPad * 2);
		 if (foo > titleHeight)
		     titleHeight = foo;
	     }
         }
	 else {
	     titleHeight = 0;
	 }
	 if(UseGadgets) {
             foo = gadgetHeight + (GadgetBorder * 2);
	     if (foo > titleHeight)
		 titleHeight = foo;
	 }
     }
     TitleCursor = XCreateFontCursor(dpy, TCursor);
     if (!TitleCursor) {
	  (void) sprintf(error_msg, "Init_Titles: Can't get title cursor #%d!\n", TCursor);
	  Error(error_msg);
     }
     /* Gadgets can't exist without titles, so we'll init them here */
     GadgetBorder = GetIntRes("gadget.borderWidth", NULL, DEF_GADGET_BORDER);
     GadgetPad = GetIntRes("gadget.hPad", "Gadget.Pad", DEF_GADGET_PAD);

     Title_res = TRUE;
     /* many other places need this info: */
    /* use if a particular window has a title: */
     titleHeightExcess = titleHeight + WBorderWidth;
     titlePositionExcess = titleHeight + WBorderWidth;
     /* titlePositionExcess is the same, but name differently for clarity */
}

void Init_Frames()
{
     Entry("Init_Frames")

     BCForeground = GetStringRes("borderContext.foreground", NULL, Foreground);
     BCBackground = GetStringRes("borderContext.background", NULL, Background);

     BCursor = GetIntRes("borderContext.cursor", NULL, DEF_BCONTEXT_CURSOR);

     BCPixmapData = GetPixmapDataRes("borderContext.pixmap", NULL,
					&BC_wide, &BC_high);
     if (!BCPixmapData && (DisplayCells(dpy, scr) <= 2) && BorderHilite) {
	BCPixmapData = gray_bits;
	BC_wide = gray_width;
	BC_high = gray_height;
     }
     BCBoldPixmapData = GetPixmapDataRes("borderContext.boldPixmap", NULL,
					&BCB_wide, &BCB_high);
     FrameCursor = XCreateFontCursor(dpy, BCursor);
     if (!FrameCursor) {
	  (void) sprintf(error_msg, "Init_Frames: Can't get border cursor #%d!\n", BCursor);
	  Error(error_msg);
     }
    /* use if a particular window has a border context: */
     borderPositionExcess = BContext;
     borderHeightExcess = BContext * 2;
/* following needed for better clarity:
     borderCHeightExcess = BContext * 2;
     borderCWidthExcess = BContext * 2; */
     /* if no Context, BContext will be zero (unless wm_option overrides!) */
     frameWidthExcess = (BContext + WBorderWidth) * 2;
     frameHeightExcess = WBorderWidth * 2;
/* should be      frameHeightExcess = (BContext + WBorderWidth) * 2; */
     /* total frame inside = client inside + title inside + 3 borders 
	+ 2 borderContexts - we always have two borders even untitled */

     Frame_res = TRUE;
     Leave_void
}

void Init_Borders()
{
     Entry("Init_Borders")

     BBackground = GetStringRes("border.background", NULL, Background);

     BPixmapData = GetPixmapDataRes("borderPixmap", NULL, 
					&B_wide, &B_high);
     if (!BPixmapData && (DisplayCells(dpy, scr) <= 2) && BorderHilite) {
	/* default - use grey stipple on mono display, solid grey on color
	   for "no focus" pixmap. */
	BPixmapData = gray_bits;
	B_wide = gray_width;
	B_high = gray_height;
     }
     BBoldPixmapData = GetPixmapDataRes("border.boldPixmap", NULL,
					&BB_wide, &BB_high);
     Leave_void
}

char *GetTitleName(w)
Window w;
{
    char *cp = 0, *tmp;
     
    Entry("GetTitleName")
	  
    if (!XFetchName(dpy, w, &cp) || !cp) {
	tmp = untitledWindowName;
    }
    else {
	tmp = (char *)malloc(strlen(cp) + 3);
	/*
	 * We add a space on both ends for asthetic effect
	 * (only apparent if we're using inverse video).
	 */
	tmp[0] = ' ';
	strcpy(tmp + 1, cp);
	strcat(tmp, " ");
	free(cp);
    }
    Leave(tmp)
}

ComputeGravity(awi, deltax, deltay)
AwmInfoPtr awi;
int *deltax, *deltay;
{
    int x = 0;
    int y = 0;

    Entry("ComputeGravity")

    /* assumes that we are operating on a framed window */
    if (awi->attrs & AT_PINSIDE) {
	/* keep bits inside window from moving */
	x -= WBorderWidth - awi->border_width;
	if (awi->attrs & AT_BORDER)
	    x -= borderPositionExcess;
    }
    else switch (awi->win_gravity) {
    case CenterGravity:
    case NorthGravity:
    case SouthGravity:
	x -= WBorderWidth - awi->border_width;
        if (awi->attrs & AT_BORDER)
	    x -= borderPositionExcess;
	break;
    case EastGravity:
    case NorthEastGravity:
    case SouthEastGravity:
	x -= (WBorderWidth - awi->border_width) << 1;
        if (awi->attrs & AT_BORDER)
	    x -= borderHeightExcess;	 /* really Width = 2x position */
	break;
    case WestGravity:
    case NorthWestGravity:
    case SouthWestGravity:
	break;	
    default:
	(void) sprintf(error_msg, "ComputeGravity: Got unknown gravity %d\n", 
		       awi->win_gravity);
	Warning(error_msg);
    }
    *deltax = x;

    if (awi->attrs & AT_PINSIDE) {
	/* keep bits inside window from moving */
	y -= WBorderWidth - awi->border_width;
	if (awi->attrs & AT_BORDER)
	    y -= borderPositionExcess;
	if (awi->attrs & AT_TITLE)
	    y -= titlePositionExcess;
    }
    else switch (awi->win_gravity) {
    case CenterGravity:
    case EastGravity:
    case WestGravity:
	y -= WBorderWidth - awi->border_width;
        if (awi->attrs & AT_BORDER)
	    y -= borderPositionExcess;
	if (awi->attrs & AT_TITLE)
	    y -= titlePositionExcess >> 1;
	break;
    case SouthGravity:
    case SouthEastGravity:
    case SouthWestGravity:
	y -= (WBorderWidth - awi->border_width) << 1;
        if (awi->attrs & AT_BORDER)
	    y -= borderHeightExcess;	 /* really Width = 2x position */
	if (awi->attrs & AT_TITLE)
	    y -= titlePositionExcess;
	break;
    case NorthGravity:
    case NorthEastGravity:
    case NorthWestGravity:
	break;
    default:
	break;
    }
    *deltay = y;

    Leave_void
}

/* Compute position difference between frame and root-relative client window ul
   position, including current border */

FrameToClient(awi, deltax, deltay)
AwmInfoPtr awi;
int *deltax, *deltay;
{
    int x = 0;
    int y = 0;

    Entry("FrameToClient")

    /* assumes that we are operating on a framed window */
    if (awi->attrs & AT_BORDER) {
	x += borderPositionExcess;
	y += borderPositionExcess;
    }
    if (awi->attrs & AT_TITLE)
	y += titlePositionExcess;
    *deltax = x;
    *deltay = y;

    Leave_void
}

/* Compute position difference between frame and client window, adjusting for
   difference between actual client window border and client's requested border
   width */

FrameToClientBorder(awi, deltax, deltay)
AwmInfoPtr awi;
int *deltax, *deltay;
{
    int x, y;

    Entry("FrameToClientBorder")

    /* assumes that we are operating on a framed window */
    x = y = WBorderWidth - awi->border_width;
    if (awi->attrs & AT_BORDER) {
	x += borderPositionExcess;
	y += borderPositionExcess;
    }
    if (awi->attrs & AT_TITLE)
	y += titlePositionExcess;
    *deltax = x;
    *deltay = y;

    Leave_void
}

/*
 * Reparent window 'w' into a frame, making the frame the right
 * size to hold any other window related decorations that may be
 * added later. If window 'w' already has a frame, then it resizes
 * it if necessary.
 */
Window Reparent(w, from_wm)
Window w;
Boolean from_wm;
{
     Window foster;
     XWindowAttributes xwa;
     XSetWindowAttributes swa;
     XWindowChanges wc;
     XWMHints *wm_hints;
     XSizeHints sz_hints;
     Bool got_size_hints;
     XClassHint class_hints;
     Atom *Protocols;
     int Protocol_count;
     /* char *Command; */
     AwmInfoPtr awi;
     char *icon_name;
     int x, y, bw, width, height;
     int clnt_x, clnt_y;
     int lrx_lim, lry_lim;
     int deltax, deltay;
     unsigned long valuemask;
     Boolean decorated = FALSE;
     XWMDecoration *decoration;
     static XWMDecoration *new_decoration = NULL;
     Window junk;

     Entry("Reparent")

     awi = GetAwmInfo(w);
     if (!awi) {
	  (void) sprintf(error_msg, "Reparent: Window %x isn't registered!", w);
	  Warning(error_msg);
	  Leave((Window)NULL)
     }
     if ((awi->frame && (awi->state & ST_DECORATED)) || IsIcon(w, NULL))
	  /* don't decorate icons or their windows! */
	  Leave((Window)NULL)

     if (!Frame_res)
	  Init_Frames();

     if (awi->attrs & AT_TITLE && !Title_res)
	  Init_Titles();

     /* XXX Optimization: we should cache this info in awi! */
     if (!XGetWindowAttributes(dpy, awi->client, &xwa))
	/* protect against possible crash if client dies */
	  Leave((Window)NULL)
     bw = xwa.border_width;
     height = xwa.height;
     width = xwa.width;
     if (awi->frame) {
	 /* need to get root-relative position; performance here no big deal */
	 XTranslateCoordinates(dpy, awi->client, RootWindow(dpy, scr), 
			       0, 0, &xwa.x, &xwa.y, &junk);
     }
     else {
	 awi->border_width = bw;   
	 /* stash existing client border width at tekwm startup/initial map */
         /* Note - if wm is killed, border width is lost.  If XWMDecoration
	    property exists, it will override this setting. */
         XSetWindowBorderWidth(dpy, awi->client, WBorderWidth);
     }
     swa.event_mask = (SubstructureRedirectMask | SubstructureNotifyMask
		       | VisibilityChangeMask);
		/* interested in when entire window + decorations visible */
     swa.colormap = awi->client_cmap;
     valuemask = CWEventMask | CWColormap;
     clnt_y = clnt_x = 0;
     if (awi->attrs & AT_BORDER) {
	  XSetWindowAttributes cswa;
     
	  swa.event_mask |= (ButtonPressMask | ButtonReleaseMask);

	  cswa.do_not_propagate_mask =
	       (ButtonPressMask | ButtonReleaseMask);
	  XChangeWindowAttributes(dpy, w, CWDontPropagate, &cswa);
     }
     if (FrameFocus)
	  swa.event_mask |= (LeaveWindowMask | EnterWindowMask);
     if (awi->attrs & AT_RAISE)
	  swa.event_mask |= PointerMotionMask;
	  /* track pointer in all decorations */
     if (awi->attrs & AT_BORDER) {
	  decorated = TRUE;
     }
     /* convert client-requested position to frame */
     if (from_wm) {
         /* If the wm provided the position, we do not handle gravity, and
	    we have reset gravity to NorthWest.  For restart, pretend
	    that the client mapped the window where the frame is placed,
	    so the recorded Decoration offset is the usual client -> frame
	    relative position. */
	 if (awi->attrs & AT_TITLE)
	     xwa.y -= titlePositionExcess;
	 if (awi->attrs & AT_BORDER) {
	     xwa.y -= borderPositionExcess;
	     xwa.x -= borderPositionExcess;
         }
         x = xwa.x;
	 y = xwa.y;
     }
     else {
     /* Undo previous reparenting move of client window if this window was
        decorated by a window manager that exited/died.  Gross, but necessary 
	due to SaveSet effects!  Then obey win_gravity. */
     if (! awi->frame && (decoration = XGetWMDecoration(dpy, awi->client))) {
	/* absence of frame indicates that this is a first-time decorate */
	/* presence of WMDecoration sez this win was previously reparented */
         xwa.x -= decoration->deltax;
         xwa.y -= decoration->deltay;
	 awi->border_width = decoration->border_width;
         /* now we have the original map position - save it */
         XFree(decoration);
     }
     x = xwa.x;
     y = xwa.y;
     /* now apply our new reparenting move */
     if (got_size_hints = XGetNormalHints(dpy, awi->client, &sz_hints) && 
	 (sz_hints.flags & PWinGravity))
	 awi->win_gravity = sz_hints.win_gravity;
	 /* nothing to XFree() */
     else
	 awi->win_gravity = NorthWestGravity;
     /*	convert client position to frame, taking into account gravity hint */
     ComputeGravity(awi, &deltax, &deltay);
     x += deltax;
     y += deltay;
     }
     /*	convert client size to frame */
     height += frameHeightExcess;
     if (awi->attrs & AT_TITLE)
	 height += titleHeightExcess;
     if (awi->attrs & AT_BORDER) {
	 width += frameWidthExcess;
	 height += borderHeightExcess;
     }
     else
	 width += frameHeightExcess;
     if (Wall) {
	 lrx_lim = ScreenWidth - width;
	 lry_lim = ScreenHeight - height;
         if (x < 0)
	     x = 0;
         if (y < 0)
	     y = 0;
	 if (x > lrx_lim)
	     x = lrx_lim;
	 if (y > lry_lim)
	     y = lry_lim;
     }
     FrameToClient(awi, &clnt_x, &clnt_y);
     /* now compute effective client win move, and stash it for the next wm.
	Value is the offset that should be subtracted off root-relative 
	client window position to obtain the effective client-mapped 
	position for redecoration */
     if (!new_decoration)
	 new_decoration = XAllocWMDecoration();
     new_decoration->deltax = x - xwa.x + clnt_x;
     new_decoration->deltay = y - xwa.y + clnt_y;
     new_decoration->border_width = awi->border_width;
     /* mark this window as decorated in case we die */
     XSetWMDecoration(dpy, awi->client, new_decoration);

     if (!awi->frame) {
	  foster = XCreateWindow(dpy, RootWindow(dpy, scr), x, y, width,
				 height, 0, xwa.depth, xwa.class,
				 xwa.visual, valuemask, &swa);
	  if (!foster)
	      Error("Reparent: Can't create foster parent!");

	  awi->frame = foster;
	  awi->name = GetTitleName(awi->client);
	  SetFrameColors(awi);
	  if (awi->attrs & AT_BORDER)
	      XDefineCursor(dpy, foster, FrameCursor);

	  /* sneak up behind the client window... */
	  wc.sibling = awi->client;
	  wc.stack_mode = Below;
	  XConfigureWindow(dpy, foster, CWSibling|CWStackMode, &wc);
	  /* map frame if client is mapped */
	  if (xwa.map_state == IsViewable)
	     XMapWindow(dpy, foster);
	  /* and grab it! */
	  XAddToSaveSet(dpy, awi->client);
	  OutstandingReparents++;
	  XReparentWindow(dpy, awi->client, foster, clnt_x, clnt_y);
	  /* set border before it becomes visible again */
	  XSetWindowBorderWidth(dpy, awi->client, WBorderWidth);
	  /* leave client window at original mapping state, so client sees
	     proper events */
	  /* Make titlebar "frame" inherit characteristics of victim */
	  /* This code should be removed after WM_CLASS is used by std xprop */
	  /* also, remove size_hint setting in GetButton.c */
	  /* Hopefully there are no side effects on tekwm... */
	  if (awi->name)
	      XStoreName(dpy, foster, awi->name);
	  if (XGetIconName(dpy, w, &icon_name)) {
	      XSetIconName(dpy, foster, icon_name);
	      XFree(icon_name);
	  }
          if (wm_hints = XGetWMHints(dpy, awi->client)) {
    	       XSetWMHints(dpy, foster, wm_hints);
	       XFree(wm_hints);
	  }
	  if (got_size_hints) {
	      XSetNormalHints(dpy, foster, &sz_hints);
	      /* nothing to XFree() */
	  }
	  class_hints.res_name = class_hints.res_class = (char *)NULL;
	  if (XGetClassHint(dpy, awi->client, &class_hints)) {
	      XSetClassHint(dpy, foster, &class_hints);
	      XFree(class_hints.res_name);
	      XFree(class_hints.res_class);
	  }
	  if (XGetWMProtocols(dpy, awi->client, &Protocols, &Protocol_count)) {
	      XSetWMProtocols(dpy, foster, Protocols, Protocol_count);
	      XFree(Protocols);
	  }
	    /* oops, misunderstood the WM_COMMAND encoding.  This will 
	       have to wait.
	  if (XGetCommandString(dpy, awi->client, &Command)) {
	      XSetCommandString(dpy, foster, Command);
	      XFree(Command);
	  } */
	  /* end temp code */
	
	  XSaveContext(dpy, foster, AwmContext, awi);
     }
     else {
	  XMoveResizeWindow(dpy, awi->frame, x, y, width, height);
	  XMoveWindow(dpy, awi->client, clnt_x, clnt_y);
	  if (awi->attrs & AT_BORDER)
	       decorated = TRUE;
	  foster = awi->frame;
     }

     /* XReparentWindow does not generate Configure Notify, so send one */
     wc.x = x + clnt_x;
     wc.y = y + clnt_y;
     wc.width = xwa.width;	/* original client size */
     wc.height = xwa.height;
     SendSyntheticConfigureNotify(awi, &wc);

     if (decorated)
	  awi->state |= ST_DECORATED;
     Leave(foster)
}
     
Window AddTitle(w)
Window w;
{
     Window title;
     AwmInfoPtr awi;
     unsigned long valuemask;
     XWindowAttributes xwa;
     XSetWindowAttributes swa;
     XWindowChanges wc;

     Entry("AddTitle")

     awi = GetAwmInfo(w);

     if (!awi) {
	  (void) sprintf(error_msg, "AddTitle: Window %x isn't registered!", w);
	  Warning(error_msg);
	  Leave((Window)None)
     }
     if (!awi->frame) 
	  Leave((Window)None)
     if (awi->title)
	  Leave(awi->frame)
     if (!(awi->attrs & AT_TITLE))
	  Leave(awi->frame)

     if (!XGetWindowAttributes(dpy, awi->client, &xwa))
	/* protect against core dump in XCreateWindow if client dies */
	  Leave((Window)None)

     swa.event_mask = (ExposureMask | ButtonPressMask | ButtonReleaseMask);
     swa.colormap = awi->client_cmap;
     valuemask = CWEventMask | CWColormap;

     if (awi->attrs & AT_BORDER)
	     title = XCreateWindow(dpy, awi->frame, BContext, BContext, 
				   xwa.width, titleHeight, WBorderWidth,
				   xwa.depth, xwa.class, xwa.visual,
				   valuemask, &swa);
     else
	     title = XCreateWindow(dpy, awi->frame, 0, 0, 
				   xwa.width, titleHeight, WBorderWidth,
				   xwa.depth, xwa.class, xwa.visual,
				   valuemask, &swa);
     if (!title)
	  Error("AddTitle: Can't create title bar!");
     awi->title = title;
     SetTitleColors(awi);
     wc.sibling = awi->client;
     wc.stack_mode = Below;
     /* title must be below client window to prevent obscuring its
	border and suppressing VisibilityUnobscured events */
     XConfigureWindow(dpy, title, CWSibling|CWStackMode, &wc);
     XDefineCursor(dpy, title, TitleCursor);
     XMapWindow(dpy, title);
     awi->state |= ST_DECORATED;
     XSaveContext(dpy, title, AwmContext, awi);
     Leave(awi->frame)
}

Boolean FNoDecorate(w, mask, button, x, y)
Window w;
int mask, button, x, y;
{
     Entry("FNoDecorate")
	  
     if (w == RootWindow(dpy, scr))
	  Leave(FALSE)
     NoDecorate(w, FALSE);
     Leave(TRUE)
}

void NoDecorate(w, destroy)
Window w;
Boolean destroy; /* FALSE iff neither frame nor client is being destroyed */
{
     XWindowAttributes xwa, fwa;
     XWindowChanges wc;
     AwmInfoPtr awi;
     int deltax, deltay;
     static XWMDecoration *new_decoration = NULL;

     Entry("NoDecorate")

     awi = GetAwmInfo(w);
     if (!awi || !awi->frame || IsIcon(w, NULL))
	 Leave_void

     if (awi->state & ST_DECORATED) {
	  if (awi->gadgets)
	      DestroyGadgets(w);
	  if (awi->title) {
	      XDeleteContext(dpy, awi->title, AwmContext);
	      if (awi->back)
	          FreePixmapFromCache(awi->back);
	      if (awi->bold)
	          FreePixmapFromCache(awi->bold);
	      XDestroyWindow(dpy, awi->title);
	      awi->title = awi->back = awi->bold = (Drawable)NULL;
	  }
     }
     if (awi->frame) {
     XGetWindowAttributes(dpy, awi->client, &xwa);
     if (destroy) {
	  XDeleteContext(dpy, awi->frame, AwmContext);
	  if (awi->BC_back)
	       FreePixmapFromCache(awi->BC_back);
	  if (awi->BC_bold)
	       FreePixmapFromCache(awi->BC_bold);
	  if (awi->name && (awi->name != untitledWindowName))
	      free(awi->name);
	  awi->name = (char *)NULL;
	  /* restore event masks of frameless state */
	  xwa.your_event_mask |= VisibilityChangeMask;
          if (!FrameFocus)
	      xwa.your_event_mask |= (EnterWindowMask | LeaveWindowMask);
	  XSelectInput(dpy, awi->client, xwa.your_event_mask);
	  XDestroyWindow(dpy, awi->frame);
	  awi->frame = awi->BC_back = awi->BC_bold = (Drawable)NULL;
     }
     else {
	  if (awi->state & ST_DECORATED) {
	      XGetWindowAttributes(dpy, awi->frame, &fwa);
	      /* Move client window back to original position:
	         subtract out gravity effect from frame position.
		 But leave border width as set by wm. */
	      ComputeGravity(awi, &deltax, &deltay);
	      fwa.x -= deltax;
	      fwa.y -= deltay;
	      /* mark this window as undecorated in case we die */
	      if (!new_decoration)
		  new_decoration = XAllocWMDecoration();
	      new_decoration->deltax = 0;
	      new_decoration->deltay = 0;
	      new_decoration->border_width = awi->border_width;
	      /* border width remains under wm control */
	      XSetWMDecoration(dpy, awi->client, new_decoration);
	      XMoveResizeWindow(dpy, awi->frame, fwa.x, fwa.y,
			        xwa.width + (WBorderWidth * 2),
				xwa.height + (WBorderWidth * 2));
	      /* leave client window border (set by awm) visible */
	      XMoveWindow(dpy, awi->client, 0, 0);
	      /* but eliminate border Context */
	      awi->state ^= ST_DECORATED;

         /* XReparentWindow does not generate Configure Notify, so send one */
	      wc.x = fwa.x;
	      wc.y = fwa.y;
	      wc.width = xwa.width;	/* original client size */
	      wc.height = xwa.height;
	      SendSyntheticConfigureNotify(awi, &wc);
	  }
     }
     }
     XFlush(dpy);
     Leave_void
}

/* called for f.decorate */
Boolean FDecorate(window, mask, button, x, y)
Window window;
int mask;
int button;
int x, y;
{
     Window fr;
     AwmInfoPtr awi;
     XWindowAttributes xwa;

     Entry("FDecorate")

     if (window == RootWindow(dpy, scr))
	  Leave(FALSE)
     awi = GetAwmInfo(window);
     if (!awi) {
	  (void) sprintf(error_msg, "FDecorate: Window %x isn't registered!", window);
	  Warning(error_msg);
	  Leave(FALSE)
     }
     fr = Decorate(window, FALSE);
     if (fr) {
	 /* restore event masks of initially framed state */
	 /* XXX Optimization - cache WindowAttributes */
	 XGetWindowAttributes(dpy, awi->client, &xwa);
	 xwa.your_event_mask = xwa.your_event_mask & !VisibilityChangeMask;
         if (!FrameFocus)
	     xwa.your_event_mask = xwa.your_event_mask &
				   !(EnterWindowMask | LeaveWindowMask);
	 XSelectInput(dpy, awi->client, xwa.your_event_mask);
	 XMapWindow(dpy, fr);
	 XMapWindow(dpy, window);
     }	  /* else might have already been decorated - no need to map
	     frame or client if already decorated, never map client until
	     frame is mapped. */
     Leave(FALSE)
}

/* called on window creation - Window will not get mapped if InitiallyIconic*/
/* returns NULL if it was already decorated or failed */
Window Decorate(w, from_wm)
Window w;
Boolean from_wm;
{
     Window ret;

     Entry("Decorate")

     ret = Reparent(w, from_wm);
     if (ret) {
	  ret = AddTitle(ret);
	  if (ret)
	      PutGadgets(ret);
	      /* keep checking for client window deletion */
     }
     Leave(ret)
}

void PaintTitle(w, reversed)
Window w;
Boolean reversed;
{
     static GC gc, rgc;
     int x, y;
     XWindowAttributes xwa;
     XFontStruct *f_info;
     AwmInfoPtr awi;
     XGCValues gcv;

     Entry("PaintTitle")

     if (!(awi = IsTitled(w)))
	 Leave_void
     XGetWindowAttributes(dpy, awi->title, &xwa);
     if (reversed && TFontBoldInfo)
	  f_info = TFontBoldInfo;
     else
	  f_info = TFontInfo;
     gcv.font = f_info->fid;
     gcv.foreground = (reversed && !TFontBoldInfo) ?
	  GetPixel(TTextBackground, awi->client_cmap) :
	  GetPixel(TTextForeground, awi->client_cmap);
     gcv.background = (reversed && !TFontBoldInfo) ?
	  GetPixel(TTextForeground, awi->client_cmap) :
	  GetPixel(TTextBackground, awi->client_cmap);
     XChangeGC(dpy, awi->winGC, (GCFont | GCForeground | GCBackground), &gcv);
     XClearWindow(dpy, awi->title);
     if (ShowName && awi->name) {	/* print window names? */
	  int textlen;

	  textlen = XTextWidth(f_info, awi->name, strlen(awi->name));

	  if (!NameOffset)
	       x = (xwa.width - textlen) / 2;
	  else {
	       if (NameOffset < 0)
		    x = (xwa.width - textlen) + NameOffset;
	       else
		    x = NameOffset;
	       if (x + textlen > xwa.width)
		    x = (xwa.width - textlen) + 1;
	  }
	  y = (xwa.height + f_info->max_bounds.ascent -
	       f_info->max_bounds.descent) / 2;

	  XDrawImageString(dpy, awi->title, awi->winGC, x, y, awi->name,
			   strlen(awi->name));
     }
     Leave_void
}

AwmInfoPtr IsTitled(w)
Window w;
{
     AwmInfoPtr tmp;

     Entry("IsTitled")

     if (tmp = GetAwmInfo(w))
	  if (tmp->title)
	       Leave(tmp)
     Leave((AwmInfoPtr)NULL)
}

    /* reset title colormap and colors */
void SetTitleColors(awi)
AwmInfoPtr awi;
{
    unsigned long valuemask;
    XSetWindowAttributes swa;
    Pixel tfore;

    Entry("SetTitleColors")

    if (awi->title) {
	tfore = GetPixel(TForeground, awi->client_cmap);
	awi->TBackgroundPixel = GetPixel(TBackground, awi->client_cmap);
	swa.colormap = awi->client_cmap;
	valuemask = CWColormap;
	XChangeWindowAttributes(dpy, awi->title, valuemask, &swa);
	/* this duplicates setting at window creation; could be moved to
	   ReAllocateColors */
	if (TBackPixmapData) {
	    awi->back = GetPixmapFromCache(awi->title, TBackPixmapData,
					   TB_wide, TB_high,
					   tfore, awi->TBackgroundPixel,
					   awi->client_depth);
	    if (!awi->back)
	        Error("SetTitleColors: Can't create pixmap for title background");
        }
	else
            awi->back = NULL;
	if (TBoldPixmapData) {
	    awi->bold = GetPixmapFromCache(awi->title, TBoldPixmapData,
					   TBB_wide, TBB_high,
					   tfore, awi->TBackgroundPixel,
					   awi->client_depth);
	    if (!awi->bold)
	        Error("SetTitleColors: Can't create pixmap for bold background");
	}
	else
            awi->bold = NULL;
    }

    Leave_void
}

    /* reset frame colormap and colors */
void SetFrameColors(awi)
AwmInfoPtr awi;
{
    XSetWindowAttributes swa;
    unsigned long valuemask;
    Pixel bback;
    Pixel bcback;
    char *BPixmapData_tmp = NULL;
    int B_wide_tmp, B_high_tmp;

    Entry("SetFrameColors")

    /* Note - actually setting the borders and backgrounds here is
	unnecessary, as LightsOff always gets called afterwards.
	Optimization - cache pixels in awi for Highlighting */
    awi->WBorderPixel = GetPixel(WBorder, awi->client_cmap);
    bback = GetPixel(BBackground, awi->client_cmap);
    if (BPixmapData) { /* we have a border context tile */
	BPixmapData_tmp = BPixmapData;
	B_wide_tmp = B_wide;
	B_high_tmp = B_high;
    }
    else if (BorderHilite &&
	     !(awi->grayPixel = GetPixel("gray", awi->client_cmap))) {
	/* If we can't allocate a "gray" pixel, and we are doing
	   borderHilite, treat this colormap like a mono display. */
	BPixmapData_tmp = gray_bits;
	B_wide_tmp = gray_width;
	B_high_tmp = gray_height;
    }
    if (BPixmapData_tmp) { /* we have a normal border tile */
	awi->B_back = GetPixmapFromCache(awi->client,
					 BPixmapData_tmp,
					 B_wide_tmp, B_high_tmp,
					 awi->WBorderPixel, bback,
					 awi->client_depth);
	if (!awi->B_back)
	    Error("SetFrameColors: Can't get pixmap for borders.");
    }
    else {
	awi->B_back = NULL;
    }
    if (BBoldPixmapData) { /* we have a bold border tile */
	awi->B_bold = GetPixmapFromCache(awi->client,
					 BBoldPixmapData,
					 BB_wide, BB_high,
					 awi->WBorderPixel, bback,
					 awi->client_depth);
	if (!awi->B_bold)
	    Error("SetFrameColors: Can't get bold pixmap for borders.");
    }
    else
	awi->B_bold = NULL;

    if (awi->frame) {
        awi->BCForegroundPixel = GetPixel(BCForeground, awi->client_cmap);
        bcback = GetPixel(BCBackground, awi->client_cmap);
	swa.colormap = awi->client_cmap;
	valuemask = CWColormap;
	XChangeWindowAttributes(dpy, awi->frame, valuemask, &swa);
	/* this duplicates setting at window creation; could be moved to
	   ReAllocateColors.  Is there an XSetColormap? */
	if (BCPixmapData) { /* we have a border context tile */
	    BPixmapData_tmp = BCPixmapData;
	    B_wide_tmp = BC_wide;
	    B_high_tmp = BC_high;
	}
	else if (BorderContextHilite && 
		 !(awi->grayPixel ? TRUE : (awi->grayPixel = 
				       GetPixel("gray", awi->client_cmap)))) {
	    /* If we can't allocate a "gray" pixel, and we are doing
	       borderContextHilite, treat this colormap like a mono display. */
	    BPixmapData_tmp = gray_bits;
	    B_wide_tmp = gray_width;
	    B_high_tmp = gray_height;
	}
	else
	    BPixmapData_tmp = NULL;
	if (BPixmapData_tmp) { /* we have a border context tile */
	    awi->BC_back = GetPixmapFromCache(awi->frame,
					      BPixmapData_tmp,
					      B_wide_tmp, B_high_tmp,
					      awi->BCForegroundPixel, bcback,
					      awi->client_depth);
	    if (!awi->BC_back)
		Error("SetFrameColors: Can't get pixmap for border context.");
	}
	else {
	    awi->BC_back = NULL;
	}
	if (BCBoldPixmapData) { /* we have a bold border context tile */
	    awi->BC_bold = GetPixmapFromCache(awi->frame,
					      BCBoldPixmapData,
					      BCB_wide, BCB_high,
					      awi->BCForegroundPixel, bcback,
					      awi->client_depth);
	    if (!awi->BC_bold)
		Error("SetFrameColors: Can't get bold pixmap for border context.");
	}
	else
	    awi->BC_bold = NULL;
    }
    Leave_void
}

/*
 * These routines deal with a sort of "pixmap cache" that is needed
 * to minimize the number of pixmaps created by awm. Awm operates on the
 * assumption that the user may have an arbitrary number of windows at
 * arbitrary depths. Since awm likes to create resources at the same
 * depth as the client window, some sort of mechanism is needed to insure
 * that as few are created as possible.
 */

struct _resInfo {
     struct _resInfo *next;
     Drawable res; 
     char *res_data;
     Pixel res_fore, res_back;
     int res_depth;
     int ref_count;
} *CacheHead;

struct _resInfo *allocResNode()
{
     struct _resInfo *ptr;

     Entry("allocResNode")

     ptr = (struct _resInfo *)malloc(sizeof(struct _resInfo));
     if (!ptr)
	  Warning("allocResNode: Out of memory!");
     Leave(ptr)
}

/*
 * Look for pixmap in cache and returns it or allocate new pixmap,
 * store it in cache and return it.
 */
Drawable GetPixmapFromCache(d, data, wide, high, fore, back, depth)
Drawable d;
char *data;
int wide, high;
Pixel fore, back;
int depth;
{
     struct _resInfo *cptr, *iptr;

     cptr = iptr = CacheHead;
     while (cptr) {
	  if (cptr->res_depth > depth)
	       break;
	  if (cptr->res_depth == depth) {
	       if (cptr->res_data > data)
		    break;
	       else if (cptr->res_data == data) {
		    if (cptr->res_fore == fore && cptr->res_back == back) {
			 cptr->ref_count++;
			 Leave(cptr->res)
		    }
	       }
	  }
	  iptr = cptr;
	  cptr = cptr->next;
     }
     /* fell through, create another resource */
     if (!iptr || cptr == CacheHead)	/* need to initialize list */
	  CacheHead = iptr = allocResNode();
     else {
	  iptr->next = allocResNode();
	  iptr = iptr->next;
     }
     iptr->next = cptr;
     iptr->res_depth = depth;
     iptr->res_data = data;
     iptr->res_fore = fore;
     iptr->res_back = back;
     iptr->res = XCreatePixmapFromBitmapData(dpy, d, data, wide, high,
					     fore, back, depth);
     iptr->ref_count = 1;
     Leave(iptr->res)
}

/*
 * Free pixmap (and associated cache cell) if no longer needed.
 */
void FreePixmapFromCache(pix)
Pixmap pix;
{
     struct _resInfo *ptr, *tmp;

     ptr = tmp = CacheHead;
     while (ptr) {
	  if (ptr->res == pix)
	       break;
	  tmp = ptr;
	  ptr = ptr->next;
     }
     if (!ptr)
	  Leave_void
     if (--ptr->ref_count == 0) { /* last ref */
	  if (ptr == CacheHead)
	       CacheHead = CacheHead->next;
	  else
	       tmp->next = ptr->next;
	  XFreePixmap(dpy, ptr->res);
	  free(ptr);
     }
}
