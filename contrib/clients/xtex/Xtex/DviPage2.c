/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

/*
 * Support drawing routines for TeXsun and TeX
 *
 *      Copyright, (C) 1987, 1988 Tim Morgan, UC Irvine
 *
 * At the time these routines are called, the values of hh and vv should
 * have been updated to the upper left corner of the graph (the position
 * the \special appears at in the dvi file).  Then the coordinates in the
 * graphics commands are in terms of a virtual page with axes oriented the
 * same as the Imagen and the SUN normally have:
 *
 *                      0,0
 *                       +-----------> +x
 *                       |
 *                       |
 *                       |
 *                      \ /
 *                       +y
 *
 * Angles are measured in the conventional way, from +x towards +y.
 * Unfortunately, that reverses the meaning of "counterclockwise"
 * from what it's normally thought of.
 *
 * A lot of floating point arithmetic has been converted to integer
 * arithmetic for speed.  In some places, this is kind-of kludgy, but
 * it's worth it.
 */


#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include "dvi-simple.h"
#include <assert.h>

#include "DviPageP.h"

extern char *ProgName;

#include	<math.h>
#include	<ctype.h>

#define texPicConvHoriz(w,x)\
  ( ((int) x * (int) w -> dviPage.pixelsPerInchHoriz) / (int) 1000 )

#define texPicConvVert(w,x)\
  ( ((int) x * (int) w -> dviPage.pixelsPerInchVert) / (int) 1000 )

/*
 * Draw a line from (fx,fy) to (tx,ty).
 * Right now, we ignore pen_size.
 */
static void line_btw(w, fx, fy, tx, ty)
     DviPageWidget w;
     int fx, fy, tx, ty;
{
  int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
  int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);
  
  fx = texPicConvHoriz(w, fx);
  fy = texPicConvVert(w, fy);
  tx = texPicConvHoriz(w, tx);
  ty = texPicConvVert(w, ty);
  
  if ( fx == tx && fy == ty ) {
    fx += rx;
    fy += ry;

    if ( XPointInRegion(w -> dviPage.updateRegion, fx, ry) ) {
      XDrawPoint(XtDisplay(w), XtWindow(w),
		 w -> dviPage.globalGC, fx, fy);
    }
  }
  else {
    XDrawLine(XtDisplay(w), XtWindow(w),
	      w -> dviPage.globalGC,
	      fx + rx, fy + ry, tx + rx, ty + ry);
  }
}

/*
 * Draw a dot at (x,y)
 */
static void dot_at(w, x, y)
     DviPageWidget w;
     int x;
     int y;
{
  line_btw(w, x, y, x+1, y);
}

/*
 * Apply the requested attributes to the last path (box) drawn.
 * Attributes are reset.
 */
static void
  do_attribute_path(w, last_min_x, last_max_x, last_min_y, last_max_y)
DviPageWidget w;
int last_min_x, last_max_x, last_min_y, last_max_y;
{
  
#ifdef UNDEF
  /*
   *	This is the code as it appears in TeXsun. I dont have the time to
   *	make this do the shading in X-11. If someone does this, please post
   *	diffs to the net.
   *
   *	Dirk Grunwald
   */
  
  static struct pixrect *shade_pr;
  
  if (last_min_x < last_max_x && last_min_y < last_max_y) {
    if (whiten) {
      do_op(last_min_x, last_min_y,
	    xconv(last_max_x) - xconv(last_min_x),
	    yconv(last_max_y) - yconv(last_min_y),
	    PIX_SRC, (struct pixrect *) 0);
    }
    else if (blacken) {
      do_op(last_min_x, last_min_y,
	    xconv(last_max_x) - xconv(last_min_x),
	    yconv(last_max_y) - yconv(last_min_y),
	    PIX_NOT(PIX_SRC), (struct pixrect *) 0);
    }
    else if (shade) {
      if (!shade_pr) {
	shade_pr = mem_create(3, 3, 1);
	if (!shade_pr) Fatal("Out of memory -- cannot create pixrect");
	pr_put(shade_pr, 0, 0, 1);
      }
      if (buildToPixmap) {
	pr_replrop(pasteUpPixmap[currentShrink][currentLeaf],
		   xconv(last_min_x) + pen_size,
		   yconv(last_min_y) + pen_size,
		   xconv(last_max_x) - xconv(last_min_x) - pen_size,
		   yconv(last_max_y) - yconv(last_min_y) - pen_size,
		   PIX_SRC, shade_pr, 0, 0);
      }
      else {
	int offset = currentLeaf*leaf_w[currentShrink]
	  pw_replrop(ptube,
		     xconv(last_min_x) + pen_size + offset,
		     yconv(last_min_y) + pen_size,
		     xconv(last_max_x) - xconv(last_min_x) - pen_size + offset,
		     yconv(last_max_y) - yconv(last_min_y) - pen_size,
		     PIX_SRC, shade_pr, 0, 0);
      }
    }
  }
  shade = blacken = whiten = FALSE;
#endif
}

/*****************************************************************************/

#define	TWOPI		(3.14159265359*2.0)
#define	MAX_PEN_SIZE	7	/* Max pixels of pen width */

/* Unfortunately, these values also appear in dvisun.c */
#define	xRESOLUTION(w)	( w -> dviPage.pixelsPerInchHoriz )
#define	yRESOLUTION(w)	( w -> dviPage.pixelsPerInchVert )

/*
 * Set the size of the virtual pen used to draw in milli-inches
 */

/* ARGSUSED */
static void set_pen_size(w, cp)
     DviPageWidget w;
     char *cp;
{
  int ps;
  
  if (sscanf(cp, " %d ", &ps) != 1) {
    error(0,0, "illegal .ps command format: %s", cp);
    return;
  }
  w -> dviPage.penSize
    = (ps*(xRESOLUTION(w)+yRESOLUTION(w)) + 1000) / 2000;
  if (w -> dviPage.penSize < 1) {
    w -> dviPage.penSize = 1;
  }
  else {
    if (w -> dviPage.penSize > MAX_PEN_SIZE) {
      w -> dviPage.penSize = MAX_PEN_SIZE;
    }
  }
}


/*
 * Print the line defined by previous path commands
 */
static void flush_path(w)
     DviPageWidget w;
{
  register int i;
  int last_min_x, last_max_x, last_min_y, last_max_y;
  int *xx = w -> dviPage.xx;
  int *yy = w -> dviPage.yy;
  int path_len = w -> dviPage.pathLen;
  
  last_min_x = 30000;	last_min_y = 30000;
  last_max_x = -30000; last_max_y = -30000;
  for (i=1; i < path_len; i++) {
    if (xx[i] > last_max_x) last_max_x = xx[i];
    if (xx[i] < last_min_x) last_min_x = xx[i];
    if (yy[i] > last_max_y) last_max_y = yy[i];
    if (yy[i] < last_min_y) last_min_y = yy[i];
    line_btw(w, xx[i], yy[i], xx[i+1], yy[i+1]);
  }
  if (xx[path_len] > last_max_x) last_max_x = xx[path_len];
  if (xx[path_len] < last_min_x) last_min_x = xx[path_len];
  if (yy[path_len] > last_max_y) last_max_y = yy[path_len];
  if (yy[path_len] < last_min_y) last_min_y = yy[path_len];
  
  w -> dviPage.pathLen = 0;
  
  do_attribute_path(w, last_min_x, last_max_x, last_min_y, last_max_y);
}

/*
 * Print a dashed line along the previously defined path, with
 * the dashes/inch defined.
 */
static void flush_dashed(w, cp, dotted)
     DviPageWidget w;
     char *cp;
     int dotted;
{
  int i, numdots, x0, y0, x1, y1;
  int cx0, cy0, cx1, cy1;
  float inchesperdash;
  double d, spacesize, a, b, dx, dy, milliperdash;
  
  int *xx = w -> dviPage.xx;
  int *yy = w -> dviPage.yy;
  int path_len = w -> dviPage.pathLen;
  
  if (sscanf(cp, " %f ", &inchesperdash) != 1) {
    error(0,0,"illegal format for dotted/dashed line: %s", cp);
    return;
  }
  if (path_len <= 1 || inchesperdash <= 0.0) {
    error(0,0,"illegal conditions for dotted/dashed line");
    return;
  }
  milliperdash = inchesperdash * 1000.0;
  x0 = xx[1];	y0 = yy[1];
  x1 = xx[2];	y1 = yy[2];
  dx = x1 - x0;
  dy = y1 - y0;
  if (dotted) {
    numdots = sqrt(dx*dx + dy*dy) / milliperdash + 0.5;
    for (i=0; i <= numdots; i++) {
      a = (float) i / (float) numdots;
      cx0 = x0 + a*dx + 0.5;
      cy0 = y0 + a*dy + 0.5;
      dot_at(w,cx0, cy0);
    }
  }
  else {
    d = sqrt(dx*dx + dy*dy);
    if (d <= 2.0*milliperdash)
      line_btw(w, x0, y0, x1, y1);
    else {
      numdots = d / (2.0*milliperdash) + 1.0;
      spacesize = (d - numdots * milliperdash) / (numdots - 1);
      for (i=0; i<numdots-1; i++) {
	a = i * (milliperdash + spacesize) / d;
	b = a + milliperdash / d;
	cx0 = x0 + a*dx + 0.5;
	cy0 = y0 + a*dy + 0.5;
	cx1 = x0 + b*dx + 0.5;
	cy1 = y0 + b*dy + 0.5;
	line_btw(w, cx0, cy0, cx1, cy1);
	b += spacesize / d;
      }
      cx0 = x0 + b*dx + 0.5;
      cy0 = y0 + b*dy + 0.5;
      line_btw(w, cx0, cy0, x1, y1);
    }
  }
  
  w -> dviPage.pathLen = 0;
}


/*
 * Add a point to the current path
 */
static void add_path(w, cp)
     DviPageWidget w;
     char *cp;
     
{
  int pathx, pathy;
  
  if (++( w -> dviPage.pathLen) >= MAXPOINTS) {
    error(0,0, "Too many points");
    return;
  }
  if (sscanf(cp, " %d %d ", &pathx, &pathy) != 2) {
    error(0,0,"Malformed path command");
    return;
  }
  w-> dviPage.xx[w -> dviPage.pathLen] = pathx;
  w-> dviPage.yy[w -> dviPage.pathLen] = pathy;
}

/*
 * Draw an arc
 */
static void arc(w, cp)
     DviPageWidget w;
     char *cp;
{
  int xc, yc, xrad, yrad, n;
  float start_angle, end_angle;
  int angS, angE;
  Bool doit = True;
    
  int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
  int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);
  
  if (sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad,
	     &start_angle, &end_angle) != 6) {
    error(0,0,"illegal arc specification: %s", cp);
    return;
  }
  
  xc = texPicConvHoriz(w, xc);
  yc = texPicConvVert(w, yc);
  xrad = texPicConvHoriz(w, xrad);
  yrad = texPicConvVert(w, yrad);
  
  angS = (int) ( (start_angle * 360 * 64) / TWOPI);
  angE = (int) ( ((end_angle - start_angle) * 360 * 64) / TWOPI);
  
  if ( w -> dviPage.haveBackingStore ) {
    doit = XRectInRegion( w -> dviPage.updateRegion,
			 xc + rx - xrad, yc + ry - yrad,
			 2 * xrad, 2 * yrad) != RectangleOut;
  }
    
  if (doit) {
    XDrawArc(XtDisplay(w), XtWindow(w), w -> dviPage.globalGC,
	     xc + rx - xrad, yc + ry - yrad,
	     2 * xrad, 2 * yrad, -angS, -angE);
  }
}


/*
 * APPROXIMATE integer distance between two points
 */
#define	dist(x0, y0, x1, y1)	(abs(x0-x1)+abs(y0-y1))

/*
 * Draw a spline along the previously defined path
 */
static void flush_spline(widget)
     DviPageWidget widget;
{
  int xp, yp, N, lastx=(-1), lasty;
  int t1, t2, t3, steps, j;
  register int i, w;
  
  int *xx = widget -> dviPage.xx;
  int *yy = widget -> dviPage.yy;
  int path_len = widget -> dviPage.pathLen;
  
#ifdef	lint
  lasty = -1;
#endif
  N = path_len + 1;
  xx[0] = xx[1];	yy[0] = yy[1];
  xx[N] = xx[N-1];	yy[N] = yy[N-1];
  for (i=0; i<N-1; i++) {	/* interval */
    steps = (dist(xx[i], yy[i], xx[i+1], yy[i+1]) +
	     dist(xx[i+1], yy[i+1], xx[i+2], yy[i+2])) / 80;
    for (j=0; j<steps; j++) {	/* points within */
      w = (j*1000 + 500) / steps;
      t1 = w * w / 20;
      w -= 500;
      t2 = (750000 - w * w) / 10;
      w -= 500;
      t3 = w * w / 20;
      xp = (t1*xx[i+2] + t2*xx[i+1] + t3*xx[i] + 50000) / 100000;
      yp = (t1*yy[i+2] + t2*yy[i+1] + t3*yy[i] + 50000) / 100000;
      if (lastx > -1) line_btw(widget, lastx, lasty, xp, yp);
      lastx = xp;
      lasty = yp;
    }
  }
  
  widget -> dviPage.pathLen = 0;
}


/*
 * Shade the last box, circle, or ellipse
 */
static void shade_last(w)
     DviPageWidget w;
{
  w -> dviPage.blacken = False;
  w -> dviPage.whiten = False;
  w -> dviPage.shade = True;
}


/*
 * Make the last box, circle, or ellipse, white inside (shade with white)
 */
static void whiten_last(w)
     DviPageWidget w;
{
  w -> dviPage.blacken = False;
  w -> dviPage.whiten = True;
  w -> dviPage.shade = False;
}


/*
 * Make last box, etc, black inside
 */
static void blacken_last(w)
     DviPageWidget w;
{
  w -> dviPage.blacken = True;
  w -> dviPage.whiten = False;
  w -> dviPage.shade = False;
}

/*
 * Draw the bounding box for a \psfig special.
 *
 * expected format of the command string is
 *
 * width height bbllx bblly bburx bbury
 *
 * *ll* means lower-left, *ur* means upper-right.
 *
 * We just draw the bounding box.
 */
psfigBegin(w,cp)
DviPageWidget w;
char *cp;
{
  int bbllx, bblly;
  int bburx, bbury;
  int width, height;

  int rx = fastFromSpHoriz(w, w -> dviPage.dviStackPointer -> h);
  int ry = fastFromSpVert(w, w -> dviPage.dviStackPointer -> v);

  sscanf(cp, " %d %d %d %d %d %d ",
	 &width, &height,
	 &bbllx, &bblly, &bburx, &bbury);

  width = fastFromSpHoriz(w, width);
  height = fastFromSpVert(w, height);

  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.globalGC,
	    rx, ry, rx + width, ry );
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.globalGC,
	    rx + width, ry, rx + width, ry + height);
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.globalGC,
	    rx + width, ry + height, rx, ry + height);
  XDrawLine(XtDisplay(w), XtWindow(w),
	    w -> dviPage.globalGC,
	    rx, ry + height, rx, ry);
}

/*****************************************************************************/

/*
 *	The code to handle the \specials generated by tpic was modified
 *	by Dirk Grunwald using the code Tim Morgan at Univ. of Calif, Irvine
 *	wrote for TeXsun.
 */

#define	COMLEN	64

void doSpecial(w, cp)
     DviPageWidget w;
     char *cp;
{
  char command[COMLEN], *orig_cp;
  register int len;
  
  orig_cp = cp;
  while (isspace(*cp)) ++cp;
  len = 0;
  while (!isspace(*cp) && *cp && len < COMLEN-1) command[len++] = *cp++;
  command[len] = '\0';
  if (strcmp(command, "pn") == 0) set_pen_size(w,cp);
  else if (strcmp(command, "fp") == 0) flush_path(w);
  else if (strcmp(command, "da") == 0) flush_dashed(w, cp, 0);
  else if (strcmp(command, "dt") == 0) flush_dashed(w, cp, 1);
  else if (strcmp(command, "pa") == 0) add_path(w, cp);
  else if (strcmp(command, "ar") == 0) arc(w, cp);
  else if (strcmp(command, "sp") == 0) flush_spline(w);
  else if (strcmp(command, "sh") == 0) shade_last(w);
  else if (strcmp(command, "wh") == 0) whiten_last(w);
  else if (strcmp(command, "bk") == 0) blacken_last(w);
  else if (strcmp(command, "ps::[begin]") == 0) psfigBegin(w, cp);
  else if (strncmp(command, "ps:",3) == 0)		/* do nothing */;
  else if (strncmp(command, "ln03", 4) == 0) 		/* do nothing */;
  else {
    error(0,0, "special \"%s\" not implemented", orig_cp);
  }
}
