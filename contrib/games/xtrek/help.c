/* help.c */

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
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <math.h> 
#include <sys/types.h>

#include "defs.h"
#include "data.h"
#include "keys.h"

LOCAL char     *AUTHOR[] = {
  "",
  "---  XTREK Release Version 5.3 alpha ---",
  "",
  "Original By Chris Guthrie (chris@ic.berkeley.edu)",
  "And Ed James (edjames@ic.berkeley.edu)",
  "Later X11R3 Mods by Dan A. Dickey (ddickey@unix.eta.com)",
  "CMU overhaul by Jon Bennett at CMU (jcrb@cs.cmu.edu)",
  "Cleanup/Speedup/Fixes by Jim Anderson at CMU ",
  "Wizardry by John Myers at CMU (jgm@cs.cmu.edu)",
  "Config parser by Mike Bolotski and",
  "Dave Gagne at UBC (mikeb and daveg@ee.ubc.ca)",
  "Cleanup and improvements by Joe Keane at CMU (jgk@cs.cmu.edu)",
};

typedef struct hline
{
  char            key;
  char           *mess;
}               HelpLine;

LOCAL HelpLine  help_message[] = {
  {' ', "0-9 Set speed"},
  {FRACWARP_KEY, "Set fractional speed"},
  {SETCOURSE_KEY, "Set course"},
  {PHASER_KEY, "Fire phaser"},
  {TORP_KEY, "Launch torp"},
  {DET_OTHER_TORP_KEY, "Explode enemy torps"},
  {DET_OWN_TORP_KEY, "Detonate own torps"},
  {MINE_KEY, "Drop a mine"},
  {DET_OWN_MINE_KEY, "Destroy own mines"},
  {TELEPORT_KEY, "Teleport"},
  {SHIELDS_UP_KEY, "Put up shields"},
  {SHIELDS_DOWN_KEY, "Put down shields"},
  {TOGGLE_SHIELDS_KEY, "Toggle screens"},
  {LOCK_KEY, "Lock on to player/planet"},
  {FOLLOW_KEY, "Follow a locked ship"},
  {REPAIR_KEY, "Enter repair mode"},
  {BOMB_KEY, "Bomb planet"},
  {BEAM_UP_KEY, "Beam up armies"},
  {BEAM_DOWN_KEY, "Beam down armies"},
  {ORBIT_KEY, "Orbit planet"},
  {QUIT_KEY, "Quit"},
  {REPEAT_MESSAGE_KEY, "Review messages"},
  {CLOAK_KEY, "Toggle cloak mode"},
  {COUP_KEY, "Coup a planet"},
  {TOGGLE_COPILOT_KEY, "(Dis)Allow copilots"},
  {PLAYER_LIST_KEY, "List players"},
  {PLANET_LIST_KEY, "List planets"},
  {SCORE_LIST_KEY, "List scores"},
  {TOGGLE_STAT_KEY, "(Un)Map status window"},
  {TOGGLE_SHOW_SHIELDS_KEY, "Toggle show shields"},
  {TOGGLE_MAP_MODE_KEY, "Turn on/off map window updating"},
  {TOGGLE_BELL_KEY, "Turn on/off bell mode"},
  {TOGGLE_NAME_MODE_KEY, "Turn on/off name mode"},
  {CLRRHOSTILE_KEY, "Turn off HOSTILE mode"},
  {SETRHOSTILE_KEY, "Turn on HOSTILE mode"},
  {INFO_KEY, "Get info on player/planet"},
  {HELP_KEY, "(Un)Map this window"},
  {WAR_KEY, "(Un)Map war window"},
  {ROBOT_HORDE_KEY, "Hose robot mode"},
#ifdef TURBO_OPTION
  {TURBO_KEY, "Toggle turbo mode"},
#endif
  {0, ""}
};

#define MAXHELPWIDTH 40
#define MAXHELP 48
#define NCOL 4
#define NROW (MAXHELP/NCOL)

GLOBAL void     fillhelp (p)
  register struct player *p;
{
  register int    i = 0, row, column;
  char            buf[100];

  for (column = 0; column < NCOL; column++)
    for (row = 1; row < NROW; row++, i++)
    {
      if (help_message[i].key == 0)
	return;
      sprintf (buf, "%c  %s", help_message[i].key, help_message[i].mess);
      XDrawImageString (p->display, p->helpWin, p->dfgc,
			fontWidth (p->dfont) * (MAXHELPWIDTH * column + 1),
			fontHeight (p->dfont) * row + p->dfont->ascent, buf, strlen (buf));
    }
}

GLOBAL void     showMotd (p)
  register struct player *p;
{
  char            buf[BUFSIZ];
  FILE           *motd, *fopen ();
  int             i, length, top, center;

  /* Author Gratification */

  XClearWindow (p->display, p->w);
  for (i = 0; i < SIZEOF (AUTHOR); i++)
  {
    length = strlen (AUTHOR[i]);
    center = WINSIDE / 2 - (length * fontWidth (p->dfont)) / 2;
    XDrawImageString (p->display, p->w, p->dfgc, center,
		      i * fontHeight (p->dfont) + p->dfont->ascent, AUTHOR[i], length);
  }
  top = SIZEOF (AUTHOR) + 2;

  /* the follwing will print a motd */
  if (motd = fopen (MOTD, "r"))
  {
    for (i = top; fgets (buf, sizeof (buf), motd); i++)
    {
      length = strlen (buf);
      buf[length - 1] = NULL;
      if (length > 80)
	length = 80;
      XDrawImageString (p->display, p->w, p->dfgc, 20,
			i * fontHeight (p->dfont) + p->dfont->ascent, buf, length);
    }
    (void) fclose (motd);
  }
}


GLOBAL void     CreateHelpWindow (p, RootWin)
  aPlayer        *p;
  Window          RootWin;
{
  Display        *display;
  Window          window;
  XSizeHints      wininfo;

  display = p->display;
  window = XCreateWindow (display, RootWin,
			      0, YOFF + WINSIDE + 2 * BORDER + 2 * MSGSIZE,
			      (unsigned) WINSIDE * 2 + 1 * BORDER,
			      (unsigned) (NROW + 2) * fontHeight (p->dfont), BORDER,
			      p->depth,
			      InputOutput, (Visual *) CopyFromParent, 0L,
			      (XSetWindowAttributes *) 0);
  XStoreName (display, window, "xtrek-help");
  XSetWindowBackground (display, window, p->backColor);
  XSetWindowBorder (display, window, p->borderColor);
  wininfo.x = 0;
  wininfo.y = YOFF + WINSIDE + BORDER * 2 + MSGSIZE * 2;
  wininfo.width = WINSIDE * 2 + BORDER;
  wininfo.height = fontHeight (p->dfont) * 10;
  wininfo.min_width = WINSIDE * 2 + BORDER;
  wininfo.min_height = NROW * fontHeight (p->dfont);
  wininfo.max_width = WINSIDE * 2 + BORDER;
  wininfo.max_height = wininfo.min_height * 2;
  wininfo.flags = PPosition | PSize | PMinSize | PMaxSize;
  XSetNormalHints (display, window, &wininfo);
  p->helpWin = window;
}
