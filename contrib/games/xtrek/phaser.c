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
#include "polar.h"


LOCAL int       check_phaser (p)
  register struct player *p;
{
  if (FAILED (p, Phaser))
    warning (p, "Phasers inoperative.");

  else if (ISREPAIR (p))
    warning (p, "Can't fire while repairing.");

  else if (ISCLOAK (p))
    warning (p, "Can't fire while cloaked.");

  else if (ISWEP (p))
    warning (p, "Weapons overheated.");

  else if (p->p_fuel < COST (p, Phaser))
    warning (p, "Not enough fuel for phaser.");

  else
    return PHNEW;

  return PHFREE;
}


LOCAL void      do_phaser_damage (p, target, dist)
  register struct player *p, *target;
  int             dist;
{
  char            buf[80];
  register int    phaserdist = p->ship.phaserdist;
  int             halfrange = phaserdist / 2;
  int             damage = p->ship.phaserdamage;

  if (dist > halfrange)
  {
    damage -= (damage * (dist - halfrange)) / halfrange;
    if (damage <= 0)
      damage = 1;
  }
  damage_ship (target, p->p_no, damage, KPhaser);
  if (p->do_bell)
  {
    sprintf (buf, "Phaser hit %s for %d points.", target->p_name, damage);
    warning (p, buf);
  }
}


LOCAL void      do_phaser (p, my_ph)	/* Do a single phaser pulse */
  register struct player *p;
  register struct phaser *my_ph;
{
  int             i, hitang;
  register struct player *j, *target;
  uchar           dir, course;
  int             range = -1, trange, phasercost, lock, py, px;
  int             p_agressive;

  if (check_phaser (p) != PHNEW)
  {
    my_ph->ph_status = PHFREE;
    return;
  }
  p_agressive = ISRHOSTILE (p) ? ALLEMPIRE : (p->p_swar | p->p_hostile);
  py = p->p_y, px = p->p_x;
  switch (my_ph->ph_status)
  {
  case PHNEW:
  case PHMISS:
    lock = ISPLOCK (p);
    course = my_ph->ph_dir;
    target = (struct player *) 0;
    for (i = MAXPLAYER, j = &players[MAXPLAYER - 1]; i--; j--)
    {
      if (j->p_status != PAlive || j == p)
	continue;
      if (p_agressive & j->p_mask)
      {
	dir = POLAR_DIRECTION (j->p_x - px, j->p_y - py);
	hitang = (lock && (p->p_playerl == i)) ? PHITANGL : PHITANG;
	if (angdist (dir, course) < hitang)
	{
	  trange = POLAR_DISTANCE (j->p_x - px, j->p_y - py);
	  if (target == 0)
	  {
	    target = j;
	    range = trange;
	  }
	  else if (range > trange)	/* WHAT? */
	  {
	    target = j;
	    range = trange;
	  }
	}
      }
    }
    if (target == 0 || range > p->ship.phaserdist)
    {
      my_ph->ph_status = PHMISS;
      warning (p, "Phaser missed!!!");
      break;
    }
    my_ph->ph_target = target->p_no;
    my_ph->ph_status = PHHIT;
    do_phaser_damage (p, target, range);
    break;

  case PHHIT:
    j = &players[my_ph->ph_target];
    if (j->p_status != PAlive)
    {
      my_ph->ph_status = PHFREE;
      return;
    }
    range = POLAR_DISTANCE (j->p_x - px, j->p_y - py);
    if (range > p->ship.phaserdist)
    {
      my_ph->ph_status = PHMISS;
      my_ph->ph_dir = POLAR_DIRECTION (j->p_x - px, j->p_y - py);
      warning (p, "Phaser missed!!!");
      break;
    }
    do_phaser_damage (p, j, range);
    break;

  case PHFREE:
    return;
  }
  phasercost = COST (p, Phaser);
  p->p_fuel -= phasercost;
  p->p_wtemp += phasercost / 8;
}


GLOBAL void     udphaser ()
{
  register int    i;
  register struct phaser *j;

  for (i = MAXPLAYER, j = &phasers[MAXPLAYER - 1]; i--; j--)
    switch (j->ph_status)
    {
    case PHFREE:
      break;

    case PHCOOL:
      if (--j->ph_pulses <= 0)
	j->ph_status = PHFREE;
      break;

    default:
      if (j->ph_pulses-- > 0)
	do_phaser (&players[i], j);
      else
      {
	j->ph_status = PHCOOL;
	j->ph_pulses = j->ph_cool;
      }
    }
}


GLOBAL void     phaser (p, course)	/* initiate firing sequence. */
  register struct player *p;
  unsigned char   course;
{
  struct phaser  *owned = &phasers[p->p_no];

  if (owned->ph_status != PHFREE)
  {
    warning (p, "Phasers not ready to fire.");
    return;
  }
  if (check_phaser (p) != PHNEW)
    return;
  owned->ph_status = PHNEW;
  owned->ph_dir = course;
  owned->ph_pulses = p->ship.ph_pulses;
  owned->ph_cool = 10 - owned->ph_pulses / 2;
  p->p_stats.st_phasers++;
  empstats[p->p_empire].empire_phasers++;
}
