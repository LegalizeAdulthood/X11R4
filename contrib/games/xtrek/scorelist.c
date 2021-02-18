/* scorelist.c */

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


LOCAL char      hdr[] =
" # Name              Kills   Max Killed Skill    Torps Phasers Planets Bombed";

GLOBAL void     scorelist (p)
  register struct player *p;
{
  Display        *display = p->display;
  XFontStruct    *font = p->dfont;
  int             font_height = fontHeight(p->dfont);
  int             y;
  int             empnum;
  Window          window;

  window = XCreateWindow (display, p->mapw, 10, 10, 77 * fontWidth (font), (nplayers + numempires + 8) * font_height, 2, p->depth, InputOutput, (Visual *) CopyFromParent, 0L, (XSetWindowAttributes *) 0);
  XSetWindowBackground (display, window, p->backColor);
  XMapWindow (display, window);
  font_height = fontHeight (font);
  XDrawImageString (display, window, p->dfgc, 0, font_height, hdr, 77);

  y = font_height * 2;
  {
    struct player  *player;

    for (player = &players[0]; player < &players[MAXPLAYER]; player++)
    {
      double          player_kills;
      int             player_losses;
      char            buf[BUFSIZ];

      if (player->p_status == PFree)
	continue;
      player_kills = player->p_stats.st_kills;
      player_losses = player->p_stats.st_losses;
      sprintf (buf, " %1x %-16.16s %6.2f %5.2f %6d %5.2f %7d %7d %7d %6d",
	       player->p_no,
	       player->p_name,
	       player_kills,
	       player->p_stats.st_maxkills,
	       player_losses,
	       player_losses <= 0 ? 0 : player_kills / player_losses,
	       player->p_stats.st_torps,
	       player->p_stats.st_phasers,
	       player->p_stats.st_planets,
	       player->p_stats.st_armsbomb);
      XDrawImageString (display, window, p->dfgc, 0, y, buf, strlen (buf));
      y += font_height;
    }
  }

  for (empnum = 0; empnum < numempires; empnum++)
  {
    char            buf[BUFSIZ];

    y += font_height;
    sprintf (buf, " %1x %-16.16s %6.2f %5.2f %6d %5.2f %7d %7d %7d %6d",
	     empnum,
	     empires[empnum].name,
	     empstats[empnum].empire_kills,
	     empstats[empnum].empire_maxkills,
	     empstats[empnum].empire_losses,
	     empstats[empnum].empire_losses <= 0 ? 0 :
	     empstats[empnum].empire_kills / empstats[empnum].empire_losses,
	     empstats[empnum].empire_torps,
	     empstats[empnum].empire_phasers,
	     empstats[empnum].empire_planets,
	     empstats[empnum].empire_armsbomb);
    XDrawImageString (display, window, p->dfgc, 0, y, buf, strlen (buf));
  }

  XFlush (display);
  p->infomapped = 1;
  p->infow = window;
}
