/* damage.c */

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



GLOBAL void repair_damage (p)
  register struct player *p;
{
    register int    repair_work, scratch1, scratch2, variance;
  int             i, maxshield = p->ship.maxshields, oldrepair;

  if (p->p_damage || p->p_shield < maxshield)
    goto isdamage;				/* Yuck (I'm guilty, Mike) */

  for (i=0; i < NSUBSYSTEMS; i++)
    if (p->p_sysdamage[i])
      goto isdamage;
  
  /* if no damage then punt out of here */
  CLRREPAIR(p);
  return;

isdamage:
  repair_work = p->ship.repair * (1 + ((p->p_stats.st_losses == 0) ? 0 : 
                         p->p_stats.st_kills / p->p_stats.st_losses / 10));

  if (ISREPAIR(p) && p->p_speed == 0)
    repair_work *= 2;
  if ((ISORBIT(p)) && !hostilePlanet (&planets[p->p_planet], p))
  {
    if (!(planets[p->p_planet].pl_flags & PLREPAIR))
      repair_work *= 2;
    else
      repair_work *= 4;
  }
  scratch1 = (p->p_subdamage + repair_work);
  if (scratch1 < 100)
    p->p_subdamage = scratch1;
  else
  {
    int damage = p->p_damage - scratch1 / 100;
    if (damage < 0)
    {
      p->p_damage = 0;
      p->p_subdamage = 0;
    }
    else
    {
      p->p_damage = damage;
      p->p_subdamage = scratch1 % 100;
    }
  }
  scratch1 = repair_work / 2;		/* A nibble of work to use as needed */
  scratch2 = 30;			/* loop N times that work is not used */
  oldrepair = repair_work;		/* store for later use with shields */

#define do_fixes(s,x,y,v,r) \
		if ((s) && (v) < (r)) { \
			(s) -= (x); \
			if ((s) < 0) { \
				(y) -= (s); \
				(s) =0; \
			} \
			else \
				(y)-=(x); \
			(x) = 0; \
		}

  for (;;)
  {
    variance = random () % 100;
    if (scratch1)
    {
      if (!(scratch2--))
	break;
    }
    else if (repair_work > 0)
    {
      scratch1 = repair_work;
      repair_work = 0;
    }
    else
      break;
    do_fixes (p->p_sysdamage[(int)SRS], scratch1, repair_work, variance, 100)
      else
    do_fixes (p->p_sysdamage[(int)Cloak], scratch1, repair_work, variance, 90)
      else
    do_fixes (p->p_sysdamage[(int)Torp], scratch1, repair_work, variance, 80)
      else
    do_fixes (p->p_sysdamage[(int)Phaser], scratch1, repair_work, variance, 70)
      else
    do_fixes (p->p_sysdamage[(int)Shield], scratch1, repair_work, variance, 60)
      else
   do_fixes (p->p_sysdamage[(int)Cooling], scratch1, repair_work, variance, 55)
      else
    do_fixes (p->p_sysdamage[(int)Lock], scratch1, repair_work, variance, 50)
      else
    do_fixes (p->p_sysdamage[(int)LRS], scratch1, repair_work, variance, 45)
      else
    do_fixes (p->p_sysdamage[(int)Trans], scratch1, repair_work, variance, 40)
  }
  if (FAILED(p, Shield))
    return;
  repair_work = oldrepair;
  scratch1 = p->p_subshield + (ISSHIELD(p) ? repair_work / 2 : repair_work);
  if (scratch1 < 100)
    p->p_subshield = scratch1;
  else
  {
    int shield = p->p_shield + scratch1 / 100;

    if (shield > maxshield)
    {
      p->p_shield = maxshield;
      p->p_subshield = 0;
    }
    else
    {
      p->p_shield = shield;
      p->p_subshield = (scratch1 % 100);
    }
  }
}

GLOBAL void damage_ship (p, attacker, damage, how)
  register struct player *p;
  register int    attacker, damage;
  DeathReason             how;
{
  struct player *other = &players[attacker];

  if (how != KPlanet && other->p_hostile & p->p_mask)
    other->p_swar |= p->p_mask;
  /* Note that if a player is at peace with the victim, then the damage 
     was caused either accidently, or because the victim was 
     at war with, or hostile to, the player. In either case, 
     we don't consider the damage to be an act of war. */

  if (ISSHIELD(p))
  {
    int shield = p->p_shield - damage;

    if (shield < 0)
    {
      damage = -shield;
      p->p_shield = 0;
    }
    else
    {
      p->p_shield = shield;
      return;
    }
  }
  p->p_damage += damage;
  if (p->p_damage < p->ship.maxdamage)
  {
    p->p_newdamage += damage + 1;
    return;
  }
  p->p_status = PExplode;
  p->p_explode = PEXPTIME;
  if (how != KPlanet)
  {
    if (attacker != p->p_no)
    {
      float           killvalue = 1.0 + (p->p_armies + p->p_kills) * 0.1;

      other->p_kills += killvalue;
      other->p_stats.st_kills += killvalue;
      empstats[other->p_empire].empire_kills += killvalue;
      if (other->p_stats.st_maxkills < other->p_kills)
	other->p_stats.st_maxkills = other->p_kills;
    }
    killmess (p, other);
  }
  empstats[p->p_empire].empire_losses++;
  p->p_stats.st_losses++;
  p->p_whydead = how;
  p->p_whodead = attacker;
}

GLOBAL void break_ship (p)
  register struct player *p;
{
  register int    newdamage = p->p_newdamage;
  register int    loop = random () % ((newdamage / 16 + p->p_damage / 64) + 1);
  register int    pound = newdamage * 6 + 2;

  p->p_newdamage = 0;

  while (loop--) {
    int i;
    for (i=0; i < NSUBSYSTEMS; i++) 
      if ((random() % pound) >= sysfail[i])
        p->p_sysdamage[i] += (random () % newdamage) + sysfail[i];
  }

  if (FAILED(p, Cloak))  CLRCLOAK(p);
  if (FAILED(p, Shield)) CLRSHIELD(p);
  if (ISENG(p)) CLRTURBO(p);
  if (FAILED(p, Lock)) {
    CLRPLOCK(p);
    CLRPLLOCK(p);
  }
  if (FAILED(p, Trans)) {
    CLRBEAMDOWN(p);
    CLRBEAMUP(p);
  }
}

