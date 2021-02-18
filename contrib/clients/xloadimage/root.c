/* root.c:
 *
 * this loads an image onto the root window
 *
 * jim frost 10.03.89
 *
 * Copyright 1989 Jim Frost.  See included file "copyright.h" for complete
 * copyright information.
 */

#include "copyright.h"
#include "xloadimage.h"

void imageOnRoot(disp, scrn, image, install, verbose)
     Display      *disp;
     int           scrn;
     Image        *image;
     unsigned int  install;
     unsigned int  verbose;
{ Pixmap   pixmap;
  Colormap xcmap;

  if (! sendImageToX(disp, scrn, DefaultVisual(disp, scrn), image,
		     &pixmap, &xcmap, verbose))
    exit(1);
  XSetWindowBackgroundPixmap(disp, RootWindow(disp, scrn), pixmap);
  XSetWindowColormap(disp, RootWindow(disp, scrn), xcmap);
  if (install)
    XInstallColormap(disp, scrn, xcmap);
  XClearWindow(disp, RootWindow(disp, scrn));
  XFreeColormap(disp, xcmap);
  XFreePixmap(disp, pixmap);
  XSetCloseDownMode(disp, RetainPermanent);
}
