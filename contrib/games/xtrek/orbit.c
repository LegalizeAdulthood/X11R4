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
#include <sys/types.h>
#include "defs.h"
#include "data.h"
#include "polar.h"


/* orbit a planet */

GLOBAL void     orbit (p)
  register struct player *p;
{
  int             player_x;
  int             player_y;
  struct planet  *planet;

  if (ISORBIT (p))
  {
    warning (p, "You're already in orbit.");
    return;
  }
  if (p->p_speed > ORBSPEED)
  {
    char            buf[80];

    sprintf (buf, "Speed must be %1.1f or less to orbit.", (float) ORBSPEED * 0.1);
    warning (p, buf);
    return;
  }
  player_x = p->p_x;
  player_y = p->p_y;
  for (planet = &planets[0]; planet < &planets[MAXPLANETS]; planet++)
  {
    int             delta_x, delta_y;
    int             dir;

    delta_x = player_x - planet->pl_x;
    if (delta_x >= ORBDIST || delta_x <= -ORBDIST)
      continue;
    delta_y = player_y - planet->pl_y;
    if (delta_y >= ORBDIST || delta_y <= -ORBDIST)
      continue;
    if (delta_x * delta_x + delta_y * delta_y >= ORBDIST * ORBDIST)
      continue;
    dir = POLAR_DIRECTION (delta_x, delta_y);
    planet->pl_info |= p->p_mask;
    p->p_speed = 0;
    p->p_desspeed = 0;
    p->p_dir = dir + 64;
    p->p_planet = planet->pl_no;
    CLRPLLOCK (p);
    CLRFOLLOW (p);
    SETORBIT (p);
    p->p_x = planet->pl_x + POLAR_X (ORBDIST, dir);
    p->p_y = planet->pl_y + POLAR_Y (ORBDIST, dir);
    return;
  }
  warning (p, "You're not within orbiting distance of any planet.");
}
