/*
 * Copyright 1989 Digital Equipment Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  Digital Equipment Corporation makes no representations
 * about the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 * DIGITAL EQUIPMENT CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Terry Weissman
 *          weissman@decwrl.dec.com
 */

/* lines.c -- paint a bunch of lines with minimal requests. */

/* %%% Too lazy to compress these; maybe I'll do it later. */


#include "roids.h"

void BeginLines() {}

void AddLine(fromx, fromy, tox, toy, gc)
int fromx, fromy, tox, toy;
GC gc;
{
    XDrawLine(dpy, gamewindow, gc, fromx, fromy, tox, toy);
}

void EndLines() {}


#define BETWEEN(x, a, b) ((a <= b && x >= a && x <= b) ||	\
			  (a > b && x <= a && x >= b))

Boolean CheckIntersects(ax, ay, bx, by, cx, cy, dx, dy)
int ax, ay, bx, by, cx, cy, dx, dy;
{
    float m1, m2, b1, b2, x, y;
    if (bx == ax) {
	if (dx == cx) return FALSE; /* Parallel; hacking, assume they miss. */
	m2 = ((float) (dy - cy)) / ((float) (dx - cx));
	b2 = cy - cx*m2;
	x = ax;
	y = m2 * x + b2;
	return (BETWEEN(y, ay, by) && BETWEEN(x, cx, dx) &&
		BETWEEN(y, cy, dy));
    }
    if (dx == cx) {
	return CheckIntersects(cx, cy, dx, dy, ax, ay, bx, by);
    }
    m1 = ((float) (by - ay)) / ((float) (bx - ax));
    m2 = ((float) (dy - cy)) / ((float) (dx - cx));
    b1 = ay - ax*m1;
    b2 = cy - cx*m2;
    if (m1 == m2) {		/* Parallel lines */
	return FALSE;		/* Hack. */
    }
    x = (b2 - b1) / (m1 - m2);
    y = m1 * x + b1;
    return (BETWEEN(x, ax, bx) && BETWEEN(y, ay, by) &&
	    BETWEEN(x, cx, dx) && BETWEEN(y, cy, dy));
}
