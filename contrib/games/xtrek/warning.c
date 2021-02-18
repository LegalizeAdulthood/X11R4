/*
 * Copyright 1989 Jon Bennett, Mike Bolotski, David Gagne, Dan Lovinger
 * Copyright 1986 Chris Gutherie
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <X11/Xlib.h>
#include <stdio.h>
#include <math.h>
#include <signal.h>
#include "defs.h"
#include "data.h"


/*
 * The warning in text will be printed in the warning window.
 */
GLOBAL void     warning (p, text)
  register struct player *p;
  register char  *text;
{
  Display        *display;
  Window          window;
  XFontStruct    *font;

  if (ISROBOT (p))
    return;
  display = p->display;
  window = p->warnw;
  font = p->dfont;
  p->warntimer = udcounter + WARNTIME;
  /* WARNTIME updates later the line will be cleared */
  if (p->warncount > 0)
    XClearArea (display, window, 5, 5, fontWidth (font) * p->warncount, fontHeight (font), False);
  p->warncount = strlen (text);
  XDrawImageString (display, window, p->dfgc, 5, 5 + font->ascent, text, p->warncount);
  XFlush (display);
}
