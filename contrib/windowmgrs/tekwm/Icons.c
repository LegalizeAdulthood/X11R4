#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Icons.c,v 5.20 89/11/22 15:11:04 glennw Exp $";
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
 * 000 -- L. Guarino Reid, DEC Ultrix Engineering Group
 * 001 -- Jordan Hubbard, Ardent Computer
 *  Many modifications for titled windows.
 * 1.2 -- Jordan Hubbard, Ardent Computer.
 * vPad and hPad usage corrected (Chris Thewalt fix).
 * Clipping now set on right GC. 
 * 1.3 -- Changed the way icon button events are handled.
 */
 
#include "awm.h"
#include "X11/Xutil.h"

extern XContext AwmContext;

/* the client should pass us a bitmap (single-plane pixmap with background=0
 * and foreground = 1).  It is our responsibility to convert it to a pixmap
 * of the appropriate depth for a window tile and also color it with the
 * appropriate background and foreground pixels (IFore/Background, not the
 * ITextFore/Background in IconGC).
 *
 */

static Pixmap MakePixmapFromKnownBitmaps( bitmap, clipmask, width, height)
Pixmap bitmap, clipmask;
unsigned int width, height;
{
     Pixmap tile;
     GC iGC;
     XGCValues gcv;
     
     Entry("MakePixmapFromKnownBitmaps")
	  
     tile = XCreatePixmap( dpy, RootWindow(dpy, scr), width, height,
			  DefaultDepth(dpy, scr) );
     gcv.foreground = IForeground;
     gcv.background = IBackground;
     iGC = XCreateGC(dpy, RootWindow(dpy, scr), (GCForeground | GCBackground),
		     &gcv);
     if (clipmask)
	  XSetClipMask(dpy, iGC, clipmask);
     XCopyPlane(dpy, bitmap, tile, iGC, 0, 0, width, height, 0, 0, 1 );
     XFreeGC(dpy, iGC);
     Leave(tile)
}

static Pixmap MakePixmapFromBitmap( bitmap, clipmask, width_return, height_return )
Pixmap bitmap, clipmask;
unsigned int *width_return, *height_return;
{
     Pixmap tile;
     Window junkW;
     int junk;
     unsigned int width, height;
     
     Entry("MakePixmapFromBitmap")
	  
     if (!XGetGeometry( dpy, bitmap, &junkW, &junk, &junk,
		       &width, &height, &junk, &junk )) {
	  Warning( "client passed invalid pixmap for icon." );
	  Leave( NULL )
     }
     tile = MakePixmapFromKnownBitmaps( bitmap, clipmask, width, height);
     if (width_return)
	  *width_return = width;
     if (height_return)
	  *height_return = height;
     Leave(tile)
}

char *GetIconName(window)
Window window;
{
     char *name;
     Status status;
     long len, after;
     Atom a_type;
     int a_form;
     
     Entry("GetIconName")
     
     status = XGetWindowProperty(dpy, window, XA_WM_ICON_NAME, 0, 256,
				 False, XA_STRING, &a_type, &a_form, &len, &after,
				 &name);
     
     if (status != Success || len == 0) {
	  status = XGetWindowProperty(dpy, window, XA_WM_NAME, 0, NAME_LEN,
				      False, XA_STRING, &a_type, &a_form, &len,
				      &after, &name);
	  if (status != Success)
	       Leave((char *)NULL)
     }
     if (a_form == 0 || len == 0)	/* we have no bananas */
	  Leave((char *)NULL)
     if (a_form != 8) {
	  Warning("Window has icon_name in wrong format");
	  if (name)
	       free(name);
	  Leave((char *)NULL)
     }
     if (len > NAME_LEN) {
	  Warning("Window has icon_name greater than maximum length");
	  len = NAME_LEN;
     }
     if (!len)
	  Leave((char *)NULL)
     name[len] = 0;
     Leave(name)
}

Boolean IsIcon(icon, win)
Window icon;
Window *win;
{
     AwmInfoPtr awi;

     Entry("IsIcon")

     if (win)
	  *win = 0;
	/* Note - win is always NULL in existing tekwm code */
     awi = GetAwmInfo(icon);
     if (!awi)
	  Leave(FALSE)
     if ((awi->icon == icon) || (awi->client_icon == icon)) {
	  if (win)
	       *win = awi->client;
	  Leave(TRUE)
     }
     else if (awi->client == icon || awi->frame == icon) {
	  if (win)
	       *win = awi->icon;
     }
     Leave(FALSE)
}

/* get rid of icon */
RemoveIcon(window)
Window window;
{
     AwmInfoPtr awi;

     Entry("RemoveIcon")
     
     if ((awi = GetAwmInfo(window)) != NULL) {
	  if (awi->iconPixmap != (Pixmap)NULL) {
	      /* destroy client pixmap and pixmap window */
	     if (awi->client_icon) {
		 XDestroyWindow(dpy, awi->client_icon);
	     }
	     XFreePixmap(dpy, awi->iconPixmap);
	     awi->iconPixmap = (Pixmap)NULL;
	  }
	  if (awi->icon != (Drawable)NULL) {
	      XDeleteContext(dpy, awi->icon, AwmContext);
	      XDestroyWindow(dpy, awi->icon);
	      awi->icon = (Drawable)NULL;
	  }
	  if (awi->client_icon) {
	      /* don't destroy client icon windows! */
	      XDeleteContext(dpy, awi->client_icon, AwmContext);
	      awi->client_icon = (Drawable)NULL;
	  }
     }
     Leave_void
}

UpdateWMState(awi)
AwmInfoPtr awi;
{
    XWMState set_wm_state;

    Entry("UpdateWMState")

    if (awi->state & ST_ICON)
	set_wm_state.state = IconicState;
    else if (awi->state & ST_WINDOW)
	set_wm_state.state = NormalState;
    else
	/* neither set inferred to mean Withdrawn */
	set_wm_state.state = WithdrawnState;
    if (awi->icon == (Window)0 && awi->client_icon == (Window)0) {
        /* we have not created the icon yet, but it might be a client icon */
	if (awi->wm_hints->flags & IconWindowHint)
	    set_wm_state.icon = awi->wm_hints->icon_window;
	else
	    set_wm_state.icon = None;
    }
    else {
	if (awi->client_icon == (Window)0)
	    set_wm_state.icon = awi->icon;
	else
	    /* ICCCM sez that this is the least ancestor that contains the
	       client icon and no other icon. */
	    set_wm_state.icon = awi->client_icon;
    }
    XSetWMState(dpy, awi->client, &set_wm_state);
    Leave_void
}

/*
 * Determine the size of the icon window for the standard string.
 */ 
char *GetDefaultIconSize(window, icon_w, icon_h, str_len, return_name)
     Window window;
     unsigned int *icon_w, *icon_h;
     int *str_len;
     Bool return_name;
{
     char *name; /* Event window name.  */

     Entry("GetDefaultIconSize")

     name = GetIconName(window);
     if (!name)
	 name = UnnamedIcon;
     GetIconSize(name, icon_w, icon_h, str_len);
     if (return_name) {
	 Leave(name);
     }
     else {
	 if (name != UnnamedIcon)
	    free (name);
	 Leave((char *)NULL);
     }
}

/*
 * Determine the size of the icon window given the string to display.
 */ 
void
GetIconSize(name, icon_w, icon_h, str_len)
     char *name;
     unsigned int *icon_w, *icon_h;
     int *str_len;
{
     Entry("GetIconSize")

     *str_len = strlen(name);
     /*
      * Fix up sizes by padding.
      */
     *icon_h = IFontInfo->ascent + IFontInfo->descent + (VIconPad << 1);
     *icon_w = XTextWidth(IFontInfo, name, *str_len) + (HIconPad << 1);
     if (*icon_w == 0) {
	  *icon_w = *icon_h;
     }
     /* make sure we get something even if no padding or text */
     Leave_void;
}

/* 
 * Given the initial x position in name_x, and the client and naem window
 * widths, compute actual client and name window x.
 */

void
AdjustIconX(client_x, name_x, client_w, name_w)
    int *name_x, *client_x;
    unsigned int client_w, name_w;
{
    int delx;
    Entry("AdjustIconX")

    if ((delx = ((int)client_w - (int)name_w) / 2) < 0)
	/* If text icon larger than window icon, window icon is right of left
	   overall edge */
	*client_x = *name_x - delx;
    else {
	*client_x = *name_x;
	*name_x += delx;
    }
    Leave_void
}

Window MakeIcon(window, iconify, placed)
Window window;                          /* associated window. */
Boolean iconify;			/* True=iconify, false=newiconify */
Boolean *placed;			/* FALSE=place it with newiconify */
{
     AwmInfoPtr awi;
     int icon_x, icon_y;		/* Icon U. L. X and Y coordinates. */
     unsigned int icon_w, icon_h;	/* Icon width and height. */
     int client_x;			/* Client Icon Window X. */
     int mask;				/* Icon event mask */
     XSetWindowAttributes iconValues;	/* for icon window creation */
     unsigned long iconValues_mask;	/* for above */
     XWMHints *wmhints;			/* see if icon position provided */
     XWMHints *XGetWMHints();
     Pixmap clip = 0;
     Window AddIcon();
     Window AddClientIcon();
     Window iconw;
     Window iW;
     int i, ix, iy;
     int str_len;
     unsigned int border_width, iw, ih;

     Entry("MakeIcon")

     /*
      *	Select "unmap window", "button", and "Visibility" events for all icon
      * windows.  Select "key pressed" if doing user-modifiable icon name.
      * Select "window exposure" unless this is a client bitmap or window (i.e.
      * no icon name to redraw).  Select EnterWindowMask for Colormap
      * installation.
      */
#define ALLICONMASK (VisibilityChangeMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask)

#ifdef NO_ICON_TYPIN
     mask = (ExposureMask | ALLICONMASK);
#else
     mask = (ExposureMask | ALLICONMASK | KeyPressMask);
#endif

     if (!(awi = GetAwmInfo(window)))
	/* This had better not fail! */
	Leave((Window)NULL)
     /*
      * Set the icon border attributes.  Applies to client icon window/pixmap
      * too, but background only applies to client bitmap and wm icon.
      */ 
     iconValues.border_pixel = IBorder;
     iconValues.background_pixmap = IBackPixmap;
     iconValues_mask = CWBorderPixel | CWBackPixmap;
 
     if ((wmhints = XGetWMHints(dpy, window)) &&
	 (wmhints->flags & IconPositionHint)) {
	  /* upper left corner of overall icon */
	  icon_x = wmhints->icon_x;
	  icon_y = wmhints->icon_y;
	  *placed = TRUE;
     } else {
	  icon_x = icon_y = 0;
	  if (iconify) {
	       /*
		* f.iconify: switch to f.newiconify to get real position
	        */
	       *placed = FALSE;
	  }
	  /* else NewIconify ignores initial position, as well as *placed */
     }
     
     /*
      * Process window manager hints.
      */ 
     if (wmhints) {
	  if ((wmhints->flags & IconWindowHint) && !awi->user_icon) {
	      if (!XGetGeometry(dpy, wmhints->icon_window, &iW, &ix, &iy,
		  &iw, &ih, &border_width, &i)) {
		  Warning("tekwm: client icon window does not exist!\n");
	      }
	      else {
	       iconw = AddClientIcon(window, wmhints->icon_window, 
				     ALLICONMASK);
	       XUnmapWindow(dpy, iconw);
	       /* make absolutely sure that client icon window will get 
		  mapped to initialize cached Visibility state */
		  XRaiseWindow(dpy, iconw);
		  /* make sure it starts out on top */
		  XSetWindowBorderWidth(dpy, iconw, IBorderWidth);
		  XSetWindowBorder(dpy, iconw, IBorder);
	          /* ICCCM says we control the border */
		   /* Also make a text icon label.  Position overall bbox
		      according to hints.  Center both windows on overall
		      bbox, top/bottom borders touch */
		  (void) GetDefaultIconSize(window, &icon_w, &icon_h, 
					    &str_len, FALSE);
		  AdjustIconX(&client_x, &icon_x, iw, icon_w);
		  XMoveWindow(dpy, iconw, client_x, icon_y);
		  icon_y += ih + (IBorderWidth << 1);
	      }
	  }
	  else if ((wmhints->flags & IconPixmapHint) || awi->user_icon) {
	      /* make a client icon bitmap */
	      if (awi->user_icon) {
		 /* use icon supercedes client icon */
		  if (awi->user_mask)
		      clip = awi->user_mask;
		  iconValues.background_pixmap =
		      MakePixmapFromKnownBitmaps(awi->user_icon, clip,
					   awi->user_width, awi->user_height);
		  iw = awi->user_width;
		  ih = awi->user_height;
	      }
	      else {
		  if (wmhints->flags & IconMaskHint)
		      clip = wmhints->icon_mask;
		  iconValues.background_pixmap =
		      MakePixmapFromBitmap(wmhints->icon_pixmap, clip,
					   &iw, &ih );
	      }

	      if (iconValues.background_pixmap) {
		  awi = GetAwmInfo(window);
		  awi->iconPixmap = iconValues.background_pixmap;
		  (void) GetDefaultIconSize(window, &icon_w, &icon_h, 
					    &str_len, FALSE);
		  AdjustIconX(&client_x, &icon_x, iw, icon_w);
		  AddClientIcon(window, XCreateWindow(
				    dpy, RootWindow(dpy, scr),
				    client_x, icon_y, iw, ih, IBorderWidth, 0,
				 CopyFromParent, CopyFromParent,
				 iconValues_mask, &iconValues),
				ALLICONMASK);
		  icon_y += ih + (IBorderWidth << 1);
	      }
	      else {
		  if (!awi->user_icon) {
		  /* client error - skip client bitmap, and correct it! */
		      iconValues.background_pixmap = IBackPixmap;
		      wmhints->flags &= ~IconPixmapHint;
		  }
		  (void) GetDefaultIconSize(window, &icon_w, &icon_h, 
					    &str_len, FALSE);
	      }
	  }
	  else (void) GetDefaultIconSize(window, &icon_w, &icon_h, 
					 &str_len, FALSE);
	  XFree(wmhints);
     }
     else (void) GetDefaultIconSize(window, &icon_w, &icon_h, &str_len, FALSE);

     /*
      * Create the icon name window.  Background always wm's.
      */
    iconValues.background_pixmap = IBackPixmap;

     /*
      * Here we assume that if they want save unders, they'll also want
      * backing store on the icons. Perhaps a dangerous assumption, but
      * one we have to make at this point.
      * I dunno what this is all about - should it apply to client icon
      * windows?
      */
     if (SaveUnder) {
	  iconValues.backing_store = WhenMapped;
	  iconValues_mask |= CWBackingStore;
     }
     iconw = AddIcon(window,
		   XCreateWindow(
				 dpy, RootWindow(dpy, scr),
				 icon_x, icon_y,
				 icon_w, icon_h,
				 IBorderWidth, 0, CopyFromParent, CopyFromParent,
				 iconValues_mask, &iconValues),
		   mask);
     /*
      *	Use the text cursor whenever the mouse is in the wm icon name window,
      * only if icon text editing enabled.
      */

#ifndef NO_ICON_TYPIN     
     XDefineCursor(dpy, iconw, TextCursor);
#endif     
     Leave(iconw);
}

Window AddIcon(window, icon, mask)
Window window, icon;
int mask;
{
     AwmInfoPtr awi;

     Entry("AddIcon")

     if (icon == NULL)
	  Leave(NULL)
     XSelectInput(dpy, icon, mask);

     awi = GetAwmInfo(window);
     awi->icon = icon;
     XSaveContext(dpy, icon, AwmContext, awi);
     Leave(icon)
}

Window AddClientIcon(window, icon, mask)
Window window, icon;
int mask;
{
     AwmInfoPtr awi;

     Entry("AddClientIcon")

     if (icon == NULL)
	  Leave(NULL)
     XSelectInput(dpy, icon, mask);

     awi = GetAwmInfo(window);
     awi->client_icon = icon;
     XSaveContext(dpy, icon, AwmContext, awi);
     Leave(icon)
}

/* 
 * from awi->icon Attributes, compute overall icon dimensions
 */

void
AdjustIconGeom(awi, InfoPtr, cliIconInfo)
     AwmInfoPtr awi;
     XWindowAttributes *InfoPtr, *cliIconInfo;
{
    int status;

    Entry("AdjustIconGeom")

    if (awi->client_icon) {
	/* compute overall dimensions */
	status = XGetWindowAttributes(dpy, awi->client_icon, cliIconInfo);
	if (status == FAILURE) {
	    /* major hosed! */
	    Warning("tekwm: internal error: client icon window destroyed!");
	    Leave_void;
	}
	else {
/* overall icon size: width = max (client, wm), 
		      height = client + wm + 2 borders, 
		      x = min(client, wm) */
	    InfoPtr->x = MIN(cliIconInfo->x, InfoPtr->x);
	    InfoPtr->y = cliIconInfo->y;
	    InfoPtr->width = MAX(cliIconInfo->width, InfoPtr->width);
	    InfoPtr->height = cliIconInfo->height + InfoPtr->height +
			      (cliIconInfo->border_width << 1);
	}
    }
    Leave_void;
}
