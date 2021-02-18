/* window.c:
 *
 * display an image in a window
 *
 * jim frost 10.03.89
 *
 * Copyright 1989 Jim Frost.  See included file "copyright.h" for complete
 * copyright information.
 */

#include "copyright.h"
#include "xloadimage.h"
#include <X11/cursorfont.h>

static void setCursor(disp, window, iw, ih, ww, wh, cursor)
     Display      *disp;
     Window        window;
     unsigned int  iw, ih;
     unsigned int  ww, wh;
     Cursor       *cursor;
{ XSetWindowAttributes swa;

  if ((ww == iw) && (wh == ih))
    swa.cursor= XCreateFontCursor(disp, XC_icon);
  else if ((ww < iw) && (wh == ih))
    swa.cursor= XCreateFontCursor(disp, XC_sb_h_double_arrow);
  else if ((ww == iw) && (wh < ih))
    swa.cursor= XCreateFontCursor(disp, XC_sb_v_double_arrow);
  else
    swa.cursor= XCreateFontCursor(disp, XC_fleur);
  XChangeWindowAttributes(disp, window, CWCursor, &swa);
  XFreeCursor(disp, *cursor);
  *cursor= swa.cursor;
}

void imageInWindow(disp, scrn, image, winx, winy, winwidth, winheight, install,
		   verbose)
     Display      *disp;
     int           scrn;
     Image        *image;
     unsigned int  winx, winy, winwidth, winheight;
     unsigned int  install;
     unsigned int  verbose;
{ Pixmap               pixmap;
  Colormap             xcmap;
  XSetWindowAttributes swa;
  XSizeHints           sh;
  XGCValues            gcv;
  GC                   gc;
  Window               window;
  int                  pixx, pixy;
  int                  lastx, lasty, mousex, mousey;
  union {
    XEvent              event;
    XAnyEvent           any;
    XButtonEvent        button;
    XKeyEvent           key;
    XConfigureEvent     configure;
    XExposeEvent        expose;
    XMotionEvent        motion;
    XResizeRequestEvent resize;
  } event;

  /* figure out the window size.  unless specifically requested to do so,
   * we will not exceed 90% of display real estate.
   */

  lastx= (winwidth || winheight);
  if (!winwidth) {
    winwidth= image->width;
    if (winwidth > DisplayWidth(disp, scrn) * 0.9)
      winwidth= DisplayWidth(disp, scrn) * 0.9;
  }
  if (!winheight) {
    winheight= image->height;
    if (winheight > DisplayHeight(disp, scrn) * 0.9)
      winheight= DisplayHeight(disp, scrn) * 0.9;
  }

  if (! sendImageToX(disp, scrn, DefaultVisual(disp, scrn),
		     image, &pixmap, &xcmap, verbose))
    exit(1);
  swa.backing_store= NotUseful;
  swa.bit_gravity= NorthWestGravity;
  swa.cursor= XCreateFontCursor(disp, XC_watch);
  swa.colormap= xcmap;
  swa.event_mask= ButtonPressMask | Button1MotionMask | KeyPressMask |
    ExposureMask | StructureNotifyMask | EnterWindowMask | LeaveWindowMask;
  swa.save_under= False;
  window= XCreateWindow(disp, RootWindow(disp, scrn), 0, 0,
			image->width, image->height, 0,
			DefaultDepth(disp, scrn),
			InputOutput, CopyFromParent,
			CWBackingStore | CWBitGravity | CWCursor |
			CWColormap | CWEventMask | CWSaveUnder, &swa);
  XStoreName(disp, window, image->title);
  XSetIconName(disp, window, image->title);

  sh.width= winwidth;
  sh.height= winheight;
  sh.min_width= 1;
  sh.min_height= 1;
  sh.max_width= image->width;
  sh.max_height= image->height;
  sh.width_inc= 1;
  sh.height_inc= 1;
  sh.flags= PMinSize | PMaxSize | PResizeInc;
  if (lastx)
    sh.flags |= USSize;
  else
    sh.flags |= PSize;
  if (winx || winy) {
    sh.x= winx;
    sh.y= winy;
    sh.flags |= USPosition;
  }
  XSetNormalHints(disp, window, &sh);

  gcv.function= GXcopy;
  gc= XCreateGC(disp, window, GCFunction, &gcv);
  XMapWindow(disp, window);
  pixx= pixy= 0;
  lastx= lasty= -1;
  setCursor(disp, window, image->width, image->height,
	    winwidth, winheight, &(swa.cursor));

  for (;;) {
    XNextEvent(disp, &event);
    switch (event.any.type) {
    case ButtonPress:
      if (event.button.button == 1) {
	lastx= event.button.x;
	lasty= event.button.y;
	break;
      }
#ifdef NUKE_ME
      XDestroyWindow(disp, window);
      XFreeCursor(disp, swa.cursor);
      XFreePixmap(disp, pixmap);
      XFreeColormap(disp, xcmap);
      return;
#else
      break;
#endif

    case KeyPress: {
      char buf[128];
      KeySym ks;
      XComposeStatus status;

      XLookupString(&event.key,buf,128,&ks,&status);
      if (buf[0]=='q' || buf[0]=='Q') {
        XDestroyWindow(disp, window);
        XFreeCursor(disp, swa.cursor);
        XFreePixmap(disp, pixmap);
        XFreeColormap(disp, xcmap);
        return;
      }
    }
      break;

    case MotionNotify:
      mousex= event.button.x;
      mousey= event.button.y;
      while (XCheckTypedEvent(disp, MotionNotify, &event) == True) {
	mousex= event.button.x;
	mousey= event.button.y;
      }
      pixx += lastx - mousex;
      pixy += lasty - mousey;
      lastx= mousex;
      lasty= mousey;
      if (pixx < 0)
	pixx= 0;
      if (pixy < 0)
	pixy= 0;
      if (pixx + winwidth > image->width)
	pixx= image->width - winwidth;
      if (pixy + winheight > image->height)
	pixy= image->height - winheight;
      XCopyArea(disp, pixmap, window, gc,
		pixx, pixy, winwidth, winheight, 0, 0);
      break;

    case ConfigureNotify:

      /* if we got resized too big, fix it.  it's a stupid window manager.
       */

      if ((event.configure.width > image->width) ||
	  (event.configure.height > image->height)) {
	if (event.configure.width > image->width)
	  winwidth= image->width;
	else
	  winwidth= event.configure.width;
	if (event.configure.height > image->height)
	  winheight= image->height;
	else
	  winheight= event.configure.height;
	XResizeWindow(disp, window, winwidth, winheight);
      }
      else {
	winwidth= event.configure.width;
	winheight= event.configure.height;
      }

      /* configure the cursor to indicate which directions we can drag
       */

      if (pixx + winwidth > image->width)
	pixx= image->width - winwidth;
      if (pixy + winheight > image->height)
	pixy= image->height - winheight;
      setCursor(disp, window, image->width, image->height,
		winwidth, winheight, &(swa.cursor));

      /* repaint 
       */

      XCopyArea(disp, pixmap, window, gc,
		pixx, pixy, winwidth, winheight, 0, 0);
      break;

    case DestroyNotify:
      XFreeCursor(disp, swa.cursor);
      XFreePixmap(disp, pixmap);
      XFreeColormap(disp, xcmap);
      return;

    case Expose:
      XCopyArea(disp, pixmap, window, gc,
		pixx + event.expose.x, pixy + event.expose.y,
		event.expose.width, event.expose.height,
		event.expose.x, event.expose.y);
      break;

    case EnterNotify:
      if (install)
	XInstallColormap(disp, xcmap);
      break;

    case LeaveNotify:
      if (install)
	XUninstallColormap(disp, xcmap);
    }
  }
}
