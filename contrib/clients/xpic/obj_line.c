/* $Header: obj_line.c,v 1.6 89/07/02 15:58:56 moraes Exp $ */
/* Procedures for the line object */

#include <values.h>
#include <math.h>

#include "xpic.h"
#include "windows.h"
#include "gels.h"
#include "draw.h"
#include "assert.h"

static int x_1, y_1, x_2, y_2;			/* Corners of box, ellipse, ends of line */
static int xmin, xmax, ymin, ymax;	/* Bounding box */
static PointList *ptList;

/* Handles events for LINE mode */
line_event(evtype, mx, my)
int evtype;		/* One of MOUSE, LEFT, MIDDLE, RIGHT */
int mx, my;		/* Snapped position of the mouse */
{
	switch (evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
		break;
	case MOTION | END_MODE:
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 1))
			Arrow(picDpy, picWin, x_2, y_2, x_1, y_1, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, x_1, y_1, x_2, y_2, gcInvert);
		line(picWin, x_1, y_1, x_2, y_2, gcInvert);
		x_2 = mx;
		y_2 = my;
		line(picWin, x_1, y_1, x_2, y_2, gcInvert);
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 1))
			Arrow(picDpy, picWin, x_2, y_2, x_1, y_1, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, x_1, y_1, x_2, y_2, gcInvert);
		break;
	case LEFT | START_MODE:
		xmin = xmax = x_1 = x_2 = verts[0].x = mx;
		ymin = ymax = y_1 = y_2 = verts[0].y = my;
		nVerts = 1;
		drawingMode = END_MODE;
		break;
	case LEFT | END_MODE:
		if (nVerts + 1 >= maxVerts) {
			maxVerts += INC_VERTS;
#ifdef DEBUG
			(void) fprintf(stderr, "Reallocing verts to %d\n", maxVerts);
#endif
			if ((verts = (XPoint *) realloc((char *) verts, 
			 (unsigned) (maxVerts * sizeof(XPoint)))) == NULL) {
				message("No more memory for vertices");
				break;
			}
		}
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 1))
			Arrow(picDpy, picWin, x_2, y_2, x_1, y_1, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, x_1, y_1, x_2, y_2, gcInvert);
		line(picWin, x_1, y_1, x_2, y_2, gcInvert);
		verts[nVerts].x = x_2 = mx;
		verts[nVerts].y = y_2 = my;
		xmin = MIN(xmin, mx);
		xmax = MAX(xmax, mx);
		ymin = MIN(ymin, my);
		ymax = MAX(ymax, my);
		line(picWin, x_1, y_1, x_2, y_2, gcInvert);
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 1))
			Arrow(picDpy, picWin, x_2, y_2, x_1, y_1, gcNormal);
		nVerts++;
		x_1 = x_2;
		y_1 = y_2;
		break;
	case RIGHT | END_MODE:
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 1))
			Arrow(picDpy, picWin, x_2, y_2, x_1, y_1, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, x_1, y_1, x_2, y_2, gcInvert);
		line(picWin, x_1, y_1, x_2, y_2, gcInvert);
		drawingMode = START_MODE;
		if (nVerts <= 1)
			break;
		if ((ptList = NewPtList(verts, nVerts)) == NULL)
			message("No more memory for line/spline");
		else {
			int i;
			/*
			 * erase the whole line - we must do it a segment at a time,
			 * because that's the way we drew it when rubber banding. If we
			 * draw the entire polyline at one go, then the dashes don't
			 * match. Then we redraw the polyline at one go.
			 */
			for (i = nVerts - 1; i > 0; i--) {
				line(picWin, verts[i-1].x, verts[i-1].y,
				 verts[i].x, verts[i].y, gcInvert);
			}
			drawlines(picDpy,picWin, gcNormal, verts, nVerts, CoordModeOrigin);
			if (((line_arrow & EN_ARROW) != 0) && (nVerts > 1))
				Arrow(picDpy, picWin, verts[nVerts - 2].x, verts[nVerts - 2].y,
				 x_1, y_1, gcNormal);
			AddLineGel(&(CurrentCell->gelList), LINE, ptList,
			 line_type | line_arrow, xmin, ymin, xmax, ymax, lineThickness);
			FreeGel(CurrentCell->undoList);
			CurrentCell->undoList = NULL;
			CurrentCell->undo = 1;
			CurrentCell->saved |= MODIFIED;
		}
		break;
	case MIDDLE | END_MODE:
		if ((line_arrow & ST_ARROW) != 0)
			Arrow(picDpy, picWin, verts[1].x, verts[1].y, verts[0].x, 
			 verts[0].y, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, x_1, y_1, x_2, y_2, gcInvert);
		verts[nVerts].x = mx;
		verts[nVerts].y = my;
		/*
		 * erase the whole line - we must do it a segment at a time, because
		 * that's the way we drew it when rubber banding. If we draw the
		 * entire polyline at one go, then the dashes don't match
		 */
		for (; nVerts > 0; nVerts--) {
				line(picWin, verts[nVerts-1].x, verts[nVerts-1].y,
				 verts[nVerts].x, verts[nVerts].y, gcInvert);
		}
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		drawlines(picDpy, picWin, gcInvert, verts, nVerts, CoordModeOrigin);
		line(picWin, x_1, y_1, x_2, y_2, gcInvert);
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 1))
			Arrow(picDpy, picWin, x_2, y_2, x_1, y_1, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, x_1, y_1, x_2, y_2, gcInvert);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown LINE mode %d", drawingMode);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "line_event");
}
	

line_abort()
{
	line_event((RIGHT | drawingMode), 0, 0);
}


line_adj(evtype, gel, mx, my)
int evtype;
Gel *gel;
int mx, my;
{
	static XPoint *v;
	static XPoint *adjusted;
	static int arrowstyle, start, end, npts;
	static Gel *linegel, *oldlinegel;
	/*
	 *  Will not need to process MOTION|START_MODE, RIGHT|START_MODE,
	 *  REDRAW|START_MODE - these are taken care of in
	 *  the adj_element routine.
	 */
	switch(evtype) {
	case MOTION | END_MODE:
		DrawLineSection(v, npts, tmpGcInvert, start, end);
		adjusted->x = mx;
		adjusted->y = my;
		DrawLineSection(v, npts, tmpGcInvert, start, end);
		break;
	case LEFT | START_MODE:
		linegel = CopyGel(gel, 1);
		oldlinegel = gel;
		gel = NULL;
		GetClosestLinePoint(linegel, mx, my, &v, &npts, &adjusted, 
		 &start, &end);
		/* Line has been erased in element_adjust, so we redraw inverted */
		GelDraw(linegel, INVERT);
		drawingMode = END_MODE;
		setwidth(tmpGcNormal, linegel->linewidth);
		setwidth(tmpGcInvert, linegel->linewidth);
		SETDASHES(tmpGcNormal, getlinestyle(linegel->attributes))
		SETDASHES(tmpGcInvert, getlinestyle(linegel->attributes))
		arrowstyle = getlinearrow(linegel->attributes);
		start = start && (arrowstyle & ST_ARROW);
		end = end && (arrowstyle & EN_ARROW);
		break;
	case LEFT | END_MODE:
		DrawLineSection(v, npts, tmpGcInvert, start, end);
		adjusted->x = mx;
		adjusted->y = my;
		update_box(linegel->b_box, mx, my);
		GelDraw(linegel, DRAW);
		(void) PushGel(&(CurrentCell->gelList), linegel);
		linegel = NULL;
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = oldlinegel;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		DrawLineSection(v, npts, tmpGcInvert, start, end);
		GelDraw(oldlinegel, DRAW);
		(void) PushUnderUndo(&(CurrentCell->gelList), oldlinegel,
		 CurrentCell->undo);
		oldlinegel = NULL;
		FreeGel(linegel);
		linegel = NULL;
		if (evtype == (MIDDLE | END_MODE))
			ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case MIDDLE | START_MODE:
		ClearGelFlags(CurrentCell->gelList);
		break;
	case REDRAW | END_MODE:
		DrawLineSection(v, npts, tmpGcInvert, start, end);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown mode %d in line_adj", 
		 evtype);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "line_adj");
}


DrawLineSection(v, npts, gc, start, end)
XPoint *v;
GC gc;
int start, end, npts;
{
	drawlines(picDpy, picWin, gc, v, npts, CoordModeOrigin);

	if (start)
		Arrow(picDpy, picWin, v[1].x, v[1].y, v[0].x, v[0].y, gc);
	if (end)
		Arrow(picDpy, picWin, v[0].x, v[0].y, v[1].x, v[1].y, gc);
}


/*
 *  Finds the closest point in the line gel 'g' to mx, my and
 *  puts the points in 'v'. Caller must allocate space for v. 'adjusted'
 *  will point to the closest point in the gel pointlist, and start and
 *  end will be set depending on whether the point is the start or end
 *  point. 'npts' is the number of points in v, usually 3, but 2 if one
 *  of the points is an endpoint.
 */
GetClosestLinePoint(g, mx, my, v, npts, adjusted, start, end)
Gel *g;
XPoint **v;
XPoint **adjusted;
int *start, *end, *npts;
int mx, my;
{
	register int i;
	int mindist = MAXINT;
	int dist;
	int closest;
	int n = ((PointList *) g->data)->nVerts;
	XPoint *vertices = ((PointList *) g->data)->v;

	*adjusted = vertices;
	for (i = 0; i < n; i++, vertices++) {
		dist = (vertices->x - mx)*(vertices->x - mx) + 
		 (vertices->y - my)*(vertices->y - my);
		if (dist < mindist) {
			closest = i;
			*adjusted = vertices;
			mindist = dist;
		}
	}
	*npts = 3;
	if (closest == 0) {
		*start = TRUE;
		*v = *adjusted;
		(*npts)--;
	} else {
		*start = FALSE;
		*v = *adjusted - 1;
	}
	if (closest == n - 1) {
		*end = TRUE;
		(*npts)--;
	} else {
		*end = FALSE;
	}
	ASSERT((*npts != 1), "One point line");
}


/*
 * Finds distance of point from a line. This is the distance of the closest
 * segment of the line from the point. The distance of a segment from the
 * point is the perpendicular distance of the point from the segment, if the
 * perpendicular intersects the segment, else it is the distance of the
 * closest endpoint. All distances are integer distances, good enough for us.
 */
int
line_distance(gel, xp, yp)
Gel *gel;
int xp, yp;
{
	int n = ((PointList *) gel->data)->nVerts;
	XPoint *v1 = ((PointList *) gel->data)->v;
	XPoint *v2 = v1;
	int dx;
	int dy;
	double t;
	int distsqr;
	int closest = MAXINT;
	int xm, ym;

	for(v2++; --n > 0; v1++, v2++) {
		/* Compute intersection of perpendicular with line segment */
		dx = v2->x - v1->x;
		dy = v2->y - v1->y;
		if (dx == 0) {
			xm = v1->x;
			ym = yp;
			t = ym - v1->y;
			t /= dy;
		} else if (dy == 0) {
			xm = xp;
			ym = v1->y;
			t = xm - v1->x;
			t /= dx;
		} else {
			double slope = dy;
			double c = (v2->x * v1->y - v1->x * v2->y);

			slope /= dx;
			c /= dx;

			xm = (yp + xp / slope - c) / (slope + 1.0 / slope);
			ym = slope * xm + c;
			t = xm - v1->x;
			t /= dx;
		}
		/*
		 * If perpendicular intersects an extension of the segment, then use
		 * the closer endpoint
		 */
		if (t < 0.0) {
			xm = v1->x;
			ym = v1->y;
		} else if (t > 1.0) {
			xm = v2->x;
			ym = v2->y;
		}
		distsqr = (xp - xm) * (xp - xm) + (yp - ym) * (yp - ym);
		closest = MIN(distsqr, closest);
	}
	return((int) sqrt((double) closest));
}
