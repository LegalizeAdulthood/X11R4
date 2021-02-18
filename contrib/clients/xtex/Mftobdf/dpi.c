/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdio.h>

#define MM_PER_INCH 22.5

/*
 *	A program for computing the dots-per-inch on a display for
 *	use in generating metafont programs. However,  don't believe
 *	what it says. E.g. for the 19" decstation monitor, this returns
 *	70dpi, but measurements by hand show that (75h, 78w) is closer.
 */

main()
{
  Display *DISP;
  int defaultScreen;
  Screen *screen;
  char *display = NULL;
  int maxHeight;
  int maxWidth;

  int mmHeight;
  int mmWidth;

  double inHeight;
  double inWidth;

  if ((DISP = XOpenDisplay(display)) == NULL) {
    
    fprintf(stderr, "[%s] Can't open display '%s'\n",
	    "dpi", XDisplayName(display));
    exit(1);
  }
  
  defaultScreen = XDefaultScreen(DISP);
  screen = ScreenOfDisplay(DISP, defaultScreen);
  
  maxHeight = XDisplayHeight(DISP, defaultScreen);
  maxWidth = XDisplayWidth(DISP, defaultScreen);

  mmWidth = WidthMMOfScreen( screen );
  mmHeight = HeightMMOfScreen( screen );

  inWidth = mmWidth / MM_PER_INCH;
  inHeight = mmHeight / MM_PER_INCH;

  fprintf(stderr,"height = %3d dots, width = %3d dots\n", maxHeight, maxWidth);
  fprintf(stderr,"height = %3d mm, width = %3d mm\n",
	  mmHeight, mmWidth);

  fprintf(stderr,"height = %3f in, width = %3f in\n",
	  inHeight, inWidth);

  fprintf(stderr,"dpi_h = %3f in, dpi_w = %3f in\n",
	  ( (double) maxHeight ) / ( inHeight ),
	  ( (double) maxWidth) / (inWidth) );

}
