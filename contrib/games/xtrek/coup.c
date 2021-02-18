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
#include <sys/types.h>
#include "defs.h"
#include "data.h"

/* throw a coup */

GLOBAL void coup (p)
  register struct player *p;
{
  register int    i;
  register struct planet *l;

  if (p->p_kills < 1.0) {
    warning (p, "You must have one kill to throw a coup.");
    return;
  }

  if (!(ISORBIT(p))) {
    warning (p, "You must orbit your home planet to throw a coup.");
    return;
  }

  for (i = 0, l = &planets[0]; i < numplanets; i++, l++) {
    if ((l->pl_owner == p->p_empire) && (l->pl_armies > 0))
    {
      warning (p, "You already own a planet!");
      return;
    }
  }

  l = &planets[p->p_planet];
  if (!(l->pl_flags & PLHOME) || l->pl_orig_owner != p->p_empire)
    RETWARN (p, "You must orbit your home planet to throw a coup.");

  if (l->pl_armies > 4)
    RETWARN(p, "Too many armies on planet to throw a coup.");

  if (l->pl_couptime > 0)
    RETWARN (p, "Planet not yet ready for a coup.");

  if (l->pl_flags & PLCOUP)	/* Avoid race conditions */
    return;

  /* The cases are now met.  We can have a coup. */
  l->pl_flags |= PLCOUP;
  p->p_stats.st_coups++;
}
