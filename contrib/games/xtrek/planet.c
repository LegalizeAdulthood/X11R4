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

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <X11/Xlib.h>

#include "defs.h"
#include "data.h"

LOCAL int       pl_warning[MAXPLANETS];	/* To keep planets shut up for awhile */

extern int      tm_robots[];

GLOBAL void     create_random_planets ()
{
  int             planet_number;

  bcopy (pdata, planets, sizeof pdata);
  for (planet_number = 0; planet_number < numplanets; planet_number++)
  {
    int             flags;

    flags = planets[planet_number].pl_flags;
    if (flags & PLHOME)
      flags |= PLREPAIR | PLFUEL;
    else
    {
      int             rand;

      rand = random ();
      if (rand & 1)
	flags |= PLFUEL;
      if (!(rand & 6))
	flags |= PLREPAIR;
    }
    planets[planet_number].pl_flags = flags;
  }
}


GLOBAL void     save_planets ()
{
  static int      plfd;
  extern off_t    lseek ();

  if (plfd <= 0)
    return;
  lseek (plfd, 0, 0);
  write (plfd, planets, sizeof (pdata));
}


GLOBAL void     udplanets ()
{
  register int    i, n_arms;
  register struct planet *pl;

  for (i = numplanets, pl = &planets[numplanets - 1]; i--; pl--)
  {
    n_arms = pl->pl_armies;
    if (n_arms < 10)
      pl->pl_armies++;
    else if ((random () % 64) > n_arms)
    {
      if (n_arms > 32)
	pl->pl_armies++;
      else
	pl->pl_armies += (64 - n_arms) / 16;
    }
  }
}


GLOBAL void     plfight ()
{
  register int    h;
  register struct planet *l;

  for (h = numplanets, l = &planets[numplanets - 1]; h--; l--)
  {
    struct player  *player;

    if (l->pl_flags & PLCOUP)
    {
      l->pl_flags &= ~PLCOUP;
      l->pl_owner = l->pl_orig_owner;
    }
    l->pl_flags &= ~PLREDRAW;
    if (pl_warning[h] > 0)
      pl_warning[h]--;
    for (player = &players[0]; player < &players[MAXPLAYER]; player++)
      if (player->p_status == PAlive)
      {
	/* Do damage by planets */
	{
	  int             delta_x;
	  int             delta_y;

	  delta_x = player->p_x - l->pl_x;
	  if (delta_x >= PFIREDIST * 3 || delta_x <= -PFIREDIST * 3)
	    continue;
	  delta_y = player->p_y - l->pl_y;
	  if (delta_y >= PFIREDIST * 3 || delta_y <= -PFIREDIST * 3)
	    continue;
	  l->pl_flags |= PLREDRAW;
	  if (delta_x >= PFIREDIST || delta_x <= -PFIREDIST ||
	      delta_y >= PFIREDIST || delta_y <= -PFIREDIST)
	    continue;
	  if (delta_x * delta_x + delta_y * delta_y >= PFIREDIST * PFIREDIST)
	    continue;
	}
	if (!hostilePlanet (l, player))
	  continue;
	if (l->pl_armies > 0)
	{
	  damage_ship (player, h, l->pl_armies / 2 + 2, KPlanet);
	  if (player->p_status == PExplode)
	  {
	    char            buf[80];
	    char            buf1[80];

	    sprintf (buf, "Enemy destroyed!");
	    sprintf (buf1, "%-3s->%-3s", l->pl_name, empires[l->pl_owner].abbrev);
	    pmessage (buf, l->pl_owner, MEMPIRE, buf1);
	  }
	}

	/* do bombing */
	if (!ISORBIT (player) || !ISBOMB (player))
	  continue;
	if (player->p_planet != l->pl_no)
	  continue;

	if (pl_warning[h] <= 0)		/* Warn owning empire */
	{
	  char            buf[80];
	  char            buf1[80];

	  pl_warning[h] = 50 / PLFIGHTFUSE;
	  sprintf (buf, "We are under attack!");
	  sprintf (buf1, "%-3s->%-3s", l->pl_name, empires[l->pl_owner].abbrev);
	  pmessage (buf, l->pl_owner, MEMPIRE, buf1);
	}
	/* Send in a robot if there are no other defenders and 	the planet is
	   in the empire's home space 		 */

	if (tcount[l->pl_owner] == 0 && tm_robots[l->pl_owner] == 0)
	{
	  startrobot (l->pl_owner, PFRSTICKY | PFRHARD, 0, 0);
	  tm_robots[l->pl_owner] = (random () % 60 + 60) / EMPIREFUSE;
	}
	{
	  struct player  *other;

	  for (other = &players[0]; other < &players[MAXPLAYER]; other++)
	  {
	    if (ISROBOT (other) && other->p_empire == l->pl_owner)
	    {
	      SETASSIST (other);
	      other->p_desx = l->pl_x;
	      other->p_desy = l->pl_y;
	    }
	  }
	}
	if (l->pl_armies <= MINARMIES)
	  continue;
	if ((random () % 100) >= 50)
	{
	  l->pl_armies--;
	  player->p_kills += 0.02;
	  empstats[player->p_empire].empire_armsbomb++;
	  player->p_stats.st_armsbomb++;
	}
      }
  }
}


GLOBAL void     beam (j)
  register struct player *j;
{
  register int    h;
  register struct planet *l;

  /* do beaming */

  for (h = numplanets, l = &planets[0]; h--; l++)
  {
    if (j->p_planet != l->pl_no)
      continue;
    if (ISBEAMUP (j))
    {
      if (l->pl_armies < MINARMIES)
	continue;
      if (j->p_armies >= j->ship.maxarmies)
	continue;
      /* XXX */
      if (j->p_armies == (int) (j->p_kills * 2.0))	/* replaced floor */
	continue;
      if (j->p_empire != l->pl_owner)
	continue;
      j->p_armies++;
      l->pl_armies--;
    }
    else if (ISBEAMDOWN (j))
    {
      if (j->p_armies == 0)
	continue;
      if (j->p_empire != l->pl_owner)
      {
	j->p_armies--;
	if (l->pl_armies > 0)
	{
	  l->pl_armies--;
	  j->p_kills += 0.02;
	  j->p_stats.st_armsbomb++;
	}
	else
	{				/* planet taken over */
	  l->pl_armies++;
	  l->pl_owner = j->p_empire;
	  empstats[j->p_empire].empire_planets++;
	  j->p_stats.st_planets++;
	  j->p_kills += 0.25;
	  checkwin (j);
	}
      }
      else
      {
	j->p_armies--;
	l->pl_armies++;
      }
    }
  }
}


/* This function is called when a planet has been taken over.
   It checks all the planets to see if the victory conditions
   are right.  If so, it blows everyone out of the game and
   resets the galaxy
*/
GLOBAL void     checkwin (winner)
  struct player  *winner;
{
  int             empire;
  int             own[MAXEMPIRES];

  for (empire = 0; empire < numempires; empire++)
    own[empire] = 0;

  {
    int             i = numplanets;
    aPlanet        *planet = &planets[0];

    while (--i >= 0)
    {
      own[planet->pl_owner]++;
      planet++;
    }
  }

  for (empire = 0; empire < numempires; empire++)
    if (own[empire] >= VICTORY)
    {
      struct player  *player;

      /* We have a winning empire */
      for (player = &players[0]; player < &players[MAXPLAYER]; player++)
      {
	if (player->p_status != PFree)
	{
	  player->p_status = PExplode;
	  player->p_explode = 0;
	  player->p_whydead = KWinner;
	  player->p_whodead = winner->p_no;
	}
      }
      winner->p_stats.st_conqs++;
      create_random_planets ();
      /* save_planets(); */
      return;
    }
}
