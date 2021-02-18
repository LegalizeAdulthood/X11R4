/* playerlist.c */

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


GLOBAL void playerlist (p)
  register struct player *p;
{
  struct player *player;
  Display *display = p->display;
  Window window = p->playerw;
  GC gc = p->dfgc;
  int font_height = fontHeight (p->dfont);
  int y = p->dfont->ascent;

  XDrawImageString (display, window, gc, 0, y,
           " # Team Name             Login    Display          dir spd", 58);
  y += font_height * 2;
  for (player = &players[0]; player < &players[MAXPLAYER]; player++)
  {
    char            buf[BUFSIZ];

    if (player->p_status != PAlive)
      continue;
    (void) sprintf (buf, " %1x  %1c   %-16.16s %-8s %-16.16s %3d %3.1f",
                      player->p_no, empires[player->p_empire].code,
 		      player->p_name, player->p_login, player->p_monitor,
 		      player->p_dir, (float) player->p_speed * 0.1);
    XDrawImageString (display, window, gc, 0, y, buf, strlen (buf));
    y += font_height;
  }
  XFlush (display);
}
