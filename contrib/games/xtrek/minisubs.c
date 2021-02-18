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
#include "defs.h"
#include "data.h"
#include "polar.h"


GLOBAL void     cool_systems (p)
  register struct player *p;
{
  register int    cool_w, cool_e, temp_e, temp_w;

  if (FAILED (p, Cooling))
  {
    check_overheat (p, p->p_wtemp, p->p_etemp);
    return;
  }
  cool_w = p->ship.wcool;
  cool_e = p->ship.ecool;
  if (!ISSHIELD (p) && !ISCLOAK (p))
  {
    cool_w *= g_coolbonus;
    cool_e *= g_coolbonus;
  }
  temp_w = p->p_wtemp - cool_w;
  temp_e = p->p_etemp - cool_e;
  if (temp_w < 0)
  {
    temp_e += temp_w;
    temp_w = 0;
  }
  if (temp_e < 0)
  {
    temp_w += temp_e;
    if (temp_w < 0)
      temp_w = 0;
    temp_e = 0;
  }
  if (ISWEP (p) && (--p->p_wtime <= 0))
    CLRWEP (p);

  if (ISENG (p) && (--p->p_etime <= 0))
    CLRENG (p);

  check_overheat (p, temp_w, temp_e);
  p->p_wtemp = temp_w;
  p->p_etemp = temp_e;
}


GLOBAL void     check_overheat (p, temp_w, temp_e)
  register struct player *p;
  register int    temp_w, temp_e;
{
  register int    rnd;

  if (!(ISWEP (p)) && temp_w > 1000 && ((rnd = random ()) % temp_w) > 800)
  {
    SETWEP (p);
    p->p_wtime = (rnd % PWEAPLOCKVAR) + PWEAPLOCKMIN;
  }
  if (!(ISENG (p)) && temp_e > 1000 && ((rnd = random ()) % temp_e) > 850)
  {
    SETENG (p);
    p->p_etime = (rnd % PENGLOCKVAR) + PENGLOCKMIN;
    p->p_desspeed = 0;
#ifdef TURBO_OPTION
    CLRTURBO (p);
    p->turboclicks = 0;
#endif
  }
}


GLOBAL void     add_fuel (p)
  register struct player *p;
{
  register int    refuel = p->ship.recharge;
  int             ship_fuel;

  if ((ISORBIT (p)) && !hostilePlanet (&planets[p->p_planet], p))
  {
    if (!(planets[p->p_planet].pl_flags & PLFUEL))
      refuel *= 2;
    else
      refuel *= 4;
  }
  ship_fuel = p->p_fuel + refuel;
  if (ship_fuel < 0)
  {
    p->p_desspeed = 0;
    cloak_off (p);
  }
  else
  {
    if (ship_fuel > p->ship.maxfuel)
      ship_fuel = p->ship.maxfuel;
  }
  p->p_fuel = ship_fuel;
}


GLOBAL void     player_orbit (p)
  register struct player *p;
{
  int             dir;
  struct planet  *planet;

  if (!(ISORBIT (p)))
    return;
  dir = (unsigned char) (p->p_dir + 2);
  p->p_dir = dir;
  p->p_desdir = dir;
  planet = &planets[p->p_planet];
  p->p_x = POLAR_Y (ORBDIST, dir) + planet->pl_x;
  p->p_y = -POLAR_X (ORBDIST, dir) + planet->pl_y;
}


GLOBAL void     space_move (p)
  register struct player *p;
{
  register int    desspeed, maxspeed, speed, ispeed;
  int             maxdam, subspeed, subdiv;

  if (ISORBIT (p))
    return;
#ifdef TURBO_OPTION
  if (ISTURBO (p))
    speed = p->ship.turbospeed;
  else
#endif
    speed = p->p_speed;
  if (p->p_dir != p->p_desdir)
  {
    if (speed <= 1)
    {
      p->p_dir = p->p_desdir;
      p->p_subdir = 0;
    }
    else
    {
      p->p_subdir += (8 * (int) p->ship.turns) / (speed * speed);
      subdiv = p->p_subdir / 8;
      if (subdiv)
      {
	if (subdiv > dirdiff (p->p_dir, p->p_desdir))
	  p->p_dir = p->p_desdir;
	else if ((unsigned char) (p->p_dir - p->p_desdir) > 127)
	  p->p_dir += subdiv;
	else
	  p->p_dir -= subdiv;
	p->p_subdir %= 8;
      }
    }
  }
  /* Alter and charge for speed */
  if (ISENG (p))
  {
    maxspeed = 0;
    desspeed = 0;
  }
  else
  {
    maxspeed = p->ship.maxspeed;
    if (p->p_damage)
    {
      maxdam = p->ship.maxdamage;
      maxspeed *= (float) ((maxdam - p->p_damage) / (float) maxdam);
    }
    desspeed = p->p_desspeed;
    if (desspeed > maxspeed)
      desspeed = maxspeed;
  }
  if (!ISTURBO (p))
  {
    /* This stuff is not used when in TURBO mode. */
    if (speed > maxspeed)
      desspeed = maxspeed;
    if (desspeed != speed)
    {
      subspeed = p->p_subspeed;
      if (desspeed > speed)
	subspeed += p->ship.accint;
      else if (desspeed < speed)
	subspeed -= p->ship.decint;
      subdiv = subspeed / 100;
      if (subdiv)
      {
	subspeed %= 100;
	ispeed = speed + subdiv;
	if (desspeed > speed)
	{
	  if (ispeed > desspeed)
	    ispeed = desspeed;
	}
	else if (desspeed < speed)
	{
	  if (ispeed < desspeed)
	    ispeed = desspeed;
	}
	speed = ispeed;
      }
      p->p_subspeed = subspeed;
      p->p_speed = speed;
    }
  }
  if (!speed)
    return;
  {
    int             cost = (speed * p->ship.warpcost) / 16;

    p->p_fuel -= cost;
    p->p_etemp += cost;
  }

  /* Move them... */
  p->p_x += POLAR_X (speed * WARP1, p->p_dir);
  p->p_y += POLAR_Y (speed * WARP1, p->p_dir);

  /* Bounce off the side of the galaxy */
  if (p->p_x < 0)
  {
    p->p_x = -p->p_x;
    if (p->p_dir == 192)
      p->p_dir = p->p_desdir = 64;
    else
      p->p_dir = p->p_desdir = 64 - (p->p_dir - 192);
  }
  else if (p->p_x > GWIDTH)
  {
    p->p_x = GWIDTH - (p->p_x - GWIDTH);
    if (p->p_dir == 64)
      p->p_dir = p->p_desdir = 192;
    else
      p->p_dir = p->p_desdir = 192 - (p->p_dir - 64);
  }
  if (p->p_y < 0)
  {
    p->p_y = -p->p_y;
    if (p->p_dir == 0)
      p->p_dir = p->p_desdir = 128;
    else
      p->p_dir = p->p_desdir = 128 - p->p_dir;
  }
  else if (p->p_y > GWIDTH)
  {
    p->p_y = GWIDTH - (p->p_y - GWIDTH);
    if (p->p_dir == 128)
      p->p_dir = p->p_desdir = 0;
    else
      p->p_dir = p->p_desdir = 0 - (p->p_dir - 128);
  }
}


GLOBAL int      dirdiff (a, b)
  unsigned char   a, b;
{
  unsigned char   diff = ABS (a - b);

  return diff > 128 ? 256 - diff : diff;
}


GLOBAL void     adjust_alert (p)
  register struct player *p;
{
  int             center_x = p->p_x, center_y = p->p_y;
  int             red;
  int             yellow;
  int             mask = p->p_swar | p->p_hostile;
  register struct player *other;

  red = 0;
  yellow = 0;
  for (other = &players[0]; other < &players[MAXPLAYER]; other++)
  {
    int             delta_x, delta_y;
    int             distance_sqr;

    if (other->p_status != PAlive)
      continue;
    if (!(other->p_mask & mask))
      continue;
    delta_x = other->p_x - center_x;
    if (delta_x >= YRANGE || delta_x <= -YRANGE)
      continue;
    delta_y = other->p_y - center_y;
    if (delta_y >= YRANGE || delta_y <= -YRANGE)
      continue;
    distance_sqr = delta_x * delta_x + delta_y * delta_y;
    if (distance_sqr >= YRANGE * YRANGE)
      continue;
    yellow = 1;
    if (distance_sqr < RRANGE * RRANGE)
    {
      red = 1;
      break;
    }
  }
  if (red)
    p->p_alert = Red;
  else if (yellow)
    p->p_alert = Yellow;
  else
    p->p_alert = Green;
}
