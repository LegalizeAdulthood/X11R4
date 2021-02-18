/* planetlist.c */

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
#include "defs.h"
#include "data.h"


/*
** Open a window which contains all the planets and their current
** statistics.  Players will not know about planets that their empire
** has not orbited.
*/

GLOBAL void     planetlist (p)
  register struct player *p;
{
  Display        *display = p->display;
  Window          window = p->planetw;
  GC              gc = p->dfgc;
  int             font_ascent = p->dfont->ascent;
  int             font_height = fontHeight (p->dfont);
  int             y = font_ascent + font_height * 2;
  char            buf[BUFSIZ];
  struct planet  *planet;

  (void) sprintf (buf, "  # Planet Name      own armies REPAIR FUEL");
  XDrawImageString (display, window, gc, 0, font_ascent, buf, strlen (buf));
  for (planet = &planets[0]; planet < &planets[MAXPLANETS]; planet++)
  {
    if (!(planet->pl_info & FLAG (p->p_empire)))
      (void) sprintf (buf, " %2d %-16s", planet->pl_no, planet->pl_name);
    else
      (void) sprintf (buf, " %2d %-16s %3s %3d    %6s %4s",
		      planet->pl_no,
		      planet->pl_name,
		      empires[planet->pl_owner].abbrev,
		      planet->pl_armies,
		      (planet->pl_flags & PLREPAIR ? "REPAIR" : "      "),
		      (planet->pl_flags & PLFUEL ? "FUEL" : "    "));
    XDrawImageString (display, window, gc, 0, y, buf, strlen (buf));
    y += font_height;
  }
  XFlush (display);
}
