/* $Header: obj_ell.c,v 1.4 89/07/02 15:58:54 moraes Exp $ */
/*
 *  The ellipse object routines
 */
#include <math.h>

#include "xpic.h"
#include "windows.h"
#include "gels.h"
#include "draw.h"
#include "assert.h"

static int x_1, y_1, x_2, y_2;			/* Corners of box, ellipse, ends of line */
static int xmin, xmax, ymin, ymax;	/* Bounding box */
static int xc, yc, xr, yr;			/* centre, radius of circles, ellipse */

ellipse_event(evtype, mx, my)
{
	switch(evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
	case RIGHT  | END_MODE:
		break;
	case MOTION | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		ellipse(picWin, xc, yc, xr, yr, gcInvert);
		x_2 = mx;
		y_2 = my;
		xc = (x_2 + x_1) / 2;
		yc = (y_2 + y_1) / 2;
		xr = ABS(x_2 - xc);
		yr = ABS(y_2 - yc);
		ellipse(picWin, xc, yc, xr, yr, gcInvert);
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		break;
	case LEFT | START_MODE:
		x_1 = x_2 = xc = mx;
		y_1 = y_2 = yc = my;
		xr = yr = 0;
		drawingMode = END_MODE;
		ellipse(picWin, xc, yc, xr, yr, gcInvert);
		break;
	case LEFT | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		ellipse(picWin, xc, yc, xr, yr, gcInvert);
		x_2 = mx;
		y_2 = my;
		xc = (x_2 + x_1) / 2;
		yc = (y_2 + y_1) / 2;
		xr = ABS(x_2 - xc);
		yr = ABS(y_2 - yc);
		xmin = MIN(x_1, mx);
		xmax = MAX(x_1, mx);
		ymin = MIN(y_1, my);
		ymax = MAX(y_1, my);
		ellipse(picWin, xc, yc, xr, yr, gcNormal);
		AddConicGel(&(CurrentCell->gelList), ELLIPSE, xc, yc, xr, yr,
		 line_type | fill_type, xmin, ymin, xmax, ymax, lineThickness);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = NULL;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case MIDDLE | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		ellipse(picWin, xc, yc, xr, yr, gcInvert);
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		ellipse(picWin, xc, yc, xr, yr, gcInvert);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Unknown ELLIPSE mode %d", drawingMode);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "ellipse_event");
}


ellipse_abort()
{
	ellipse_event((MIDDLE | drawingMode), 0, 0);
}
	

ellipse_adj(evtype, gel, mx, my)
int evtype;
Gel *gel;
int mx, my;
{
	static Gel *ellgel;
	/*
	 *  Will not need to process MOTION|START_MODE, RIGHT|START_MODE,
	 *  REDRAW|START_MODE - these are taken care of in
	 *  the adj_element routine.
	 */
	switch(evtype) {
	case MOTION | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		ellipse(picWin, xc, yc, xr, yr, tmpGcInvert);
		x_2 = mx;
		y_2 = my;
		xc = (x_2 + x_1) / 2;
		yc = (y_2 + y_1) / 2;
		xr = ABS(x_2 - xc);
		yr = ABS(y_2 - yc);
		ellipse(picWin, xc, yc, xr, yr, tmpGcInvert);
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		break;
	case LEFT | START_MODE:
		GetBoxCorners(&x_1, &y_1, &x_2, &y_2, &gel->b_box, mx, my);
		xc = (x_1 + x_2) / 2;
		yc = (y_1 + y_2) / 2;
		xr = ABS(x_2 - xc);
		yr = ABS(y_2 - yc);
		ellgel = gel;
		drawingMode = END_MODE;
		setwidth(tmpGcNormal, ellgel->linewidth);
		setwidth(tmpGcInvert, ellgel->linewidth);
		SETDASHES(tmpGcNormal, getlinestyle(ellgel->attributes))
		SETDASHES(tmpGcInvert, getlinestyle(ellgel->attributes))
		ellipse(picWin, xc, yc, xr, yr, tmpGcInvert);
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		break;
	case LEFT | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		ellipse(picWin, xc, yc, xr, yr, tmpGcInvert);
		x_2 = mx;
		y_2 = my;
		xc = (x_2 + x_1) / 2;
		yc = (y_2 + y_1) / 2;
		xr = ABS(x_2 - xc);
		yr = ABS(y_2 - yc);
		xmin = MIN(x_1, mx);
		xmax = MAX(x_1, mx);
		ymin = MIN(y_1, my);
		ymax = MAX(y_1, my);
		ellipse(picWin, xc, yc, xr, yr, tmpGcNormal);
		AddConicGel(&(CurrentCell->gelList), ELLIPSE, xc, yc, xr, yr, 
		 ellgel->attributes, xmin, ymin, xmax, ymax, ellgel->linewidth);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = ellgel;
		ellgel = NULL;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		ellipse(picWin, xc, yc, xr, yr, tmpGcInvert);
		GelDraw(ellgel, DRAW);
		(void) PushUnderUndo(&(CurrentCell->gelList), ellgel,
		 CurrentCell->undo);
		ellgel = NULL;
		if (evtype == (MIDDLE | END_MODE))
			ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case MIDDLE | START_MODE:
		ClearGelFlags(CurrentCell->gelList);
		break;
	case REDRAW | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		ellipse(picWin, xc, yc, xr, yr, tmpGcInvert);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Unknown mode %d in ellipse_adj", 
		 evtype);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "ellipse_adj");
}

/*
 * Finds distance of point from an ellipse. This is taken as distance to the
 * intersection of the line from the point to the centre of the ellipse. It's
 * a fudge, because I don't know of a closed form solution to the problem of
 * finding the perpendicular distance of a point from an ellipse, and I really
 * don't want to iterate.
 */
int
ellipse_distance(gel, xp, yp)
Gel *gel;
int xp, yp;
{
	int dist;
	Conic *conic = (Conic *) gel->data;
	double a = conic->xrad;
	double b = conic->yrad;
	double slope;
	int dy = (yp - conic->centre.y);
	int dx = (xp - conic->centre.x);
	double xm, ym;

	if (dx == 0) {
		dist = ABS(dy) - conic->yrad;
		dist = ABS(dist);
	} else if (dy == 0) {
		dist = ABS(dx) - conic->xrad;
		dist = ABS(dist);
	} else {
		slope = dy;
		slope /= dx;
		xm = 1.0 / sqrt(1.0 / (a * a) + (slope * slope) / (b * b));
		if (dx < 0)
			xm = -xm;
		ym = slope * xm + conic->centre.y;
		xm += conic->centre.x;
		dist = sqrt((xp - xm) * (xp - xm) + (yp - ym) * (yp - ym));
	}
	return(dist);
}
