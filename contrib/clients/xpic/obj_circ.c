/* $Header: obj_circ.c,v 1.5 89/07/02 15:58:49 moraes Exp $ */
/*
 *  The circle object routines
 */
#include <math.h>

#include "xpic.h"
#include "windows.h"
#include "spline.h"
#include "gels.h"
#include "draw.h"
#include "input.h"
#include "newfonts.h"
#include "assert.h"

static int xmin, xmax, ymin, ymax;	/* Bounding box */
static int xc, yc, xr, yr;			/* centre, radius of circles, ellipse */
static int rad;						/* radius of circle */

circle_event(evtype, mx, my)
{
	switch(evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
	case RIGHT  | END_MODE:
		break;
	case MOTION | END_MODE:
		ellipse(picWin, xc, yc, rad, rad, gcInvert);
		xr = mx - xc;
		yr = my - yc;
		rad = snap(isqrt(xr*xr + yr*yr), mouseResolution);
		if (rad == 0)
			rad = mouseResolution;
		ellipse(picWin, xc, yc, rad, rad, gcInvert);
		break;
	case LEFT | START_MODE:
		xc = mx;
		yc = my;
		rad = mouseResolution;
		drawingMode = END_MODE;
		ellipse(picWin, xc, yc, rad, rad, gcInvert);
		break;
	case LEFT | END_MODE:
		ellipse(picWin, xc, yc, rad, rad, gcInvert);
		xr = mx - xc;
		yr = my - yc;
		rad = snap(isqrt(xr*xr + yr*yr), mouseResolution);
		if (rad == 0)
			rad = mouseResolution;
		xmin = xc - rad;
		xmax = xc + rad;
		ymin = yc - rad;
		ymax = yc + rad;
		ellipse(picWin, xc, yc, rad, rad, gcNormal);
		AddConicGel(&(CurrentCell->gelList), CIRCLE, xc, yc, rad, rad, 
		 line_type | fill_type, xmin, ymin, xmax, ymax, lineThickness);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = NULL;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case MIDDLE | END_MODE:
		ellipse(picWin, xc, yc, rad, rad, gcInvert);
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		ellipse(picWin, xc, yc, rad, rad, gcInvert);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown CIRCLE mode %d", drawingMode);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "circle_event");
}


circle_abort()
{
	circle_event((MIDDLE | drawingMode), 0, 0);
}
	

circle_adj(evtype, gel, mx, my)
int evtype;
Gel *gel;
int mx, my;
{
	static Gel *circgel;
	/*
	 *  Will not need to process MOTION|START_MODE, RIGHT|START_MODE,
	 *  REDRAW|START_MODE - these are taken care of in
	 *  the adj_element routine.
	 */
	switch(evtype) {
	case MOTION | END_MODE:
		ellipse(picWin, xc, yc, rad, rad, tmpGcInvert);
		xr = mx - xc;
		yr = my - yc;
		rad = snap(isqrt(xr*xr + yr*yr), mouseResolution);
		if (rad == 0)
			rad = mouseResolution;
		ellipse(picWin, xc, yc, rad, rad, tmpGcInvert);
		break;
	case LEFT | START_MODE:
		xc = ((Conic *)gel->data)->centre.x;
		yc = ((Conic *)gel->data)->centre.y;
		rad = ((Conic *)gel->data)->xrad;
		circgel = gel;
		drawingMode = END_MODE;
		setwidth(tmpGcNormal, circgel->linewidth);
		setwidth(tmpGcInvert, circgel->linewidth);
		SETDASHES(tmpGcNormal, getlinestyle(circgel->attributes))
		SETDASHES(tmpGcInvert, getlinestyle(circgel->attributes))
		ellipse(picWin, xc, yc, rad, rad, tmpGcInvert);
		break;
	case LEFT | END_MODE:
		ellipse(picWin, xc, yc, rad, rad, tmpGcInvert);
		xr = mx - xc;
		yr = my - yc;
		rad = snap(isqrt(xr*xr + yr*yr), mouseResolution);
		if (rad == 0)
			rad = mouseResolution;
		xmin = xc - rad;
		xmax = xc + rad;
		ymin = yc - rad;
		ymax = yc + rad;
		ellipse(picWin, xc, yc, rad, rad, tmpGcNormal);
		AddConicGel(&(CurrentCell->gelList), CIRCLE, xc, yc, rad, rad, 
		 circgel->attributes, xmin, ymin, xmax, ymax, circgel->linewidth);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = circgel;
		circgel = NULL;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		ellipse(picWin, xc, yc, rad, rad, tmpGcInvert);
		GelDraw(circgel, DRAW);
		(void) PushUnderUndo(&(CurrentCell->gelList), circgel,
		 CurrentCell->undo);
		circgel = NULL;
		if (evtype == (MIDDLE | END_MODE))
			ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case MIDDLE | START_MODE:
		ClearGelFlags(CurrentCell->gelList);
		break;
	case REDRAW | END_MODE:
		ellipse(picWin, xc, yc, rad, rad, tmpGcInvert);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown mode %d in circle_adj", 
		 evtype);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "circle_adj");
}

/*
 * Finds distance of point from an circle. This is the distance to the
 * intersection of the line from the point to the centre of the circle.
 */
int
circle_distance(gel, xp, yp)
Gel *gel;
int xp, yp;
{
	int dist;
	Conic *conic = (Conic *) gel->data;
	double r = conic->xrad;
	int dy = (yp - conic->centre.y);
	int dx = (xp - conic->centre.x);

	if (dx == 0) {
		dist = ABS(dy) - conic->yrad;
	} else if (dy == 0) {
		dist = ABS(dx) - conic->xrad;
	} else {
		dist = sqrt((double) (dx * dx + dy * dy)) - r;
	}
	dist = ABS(dist);
	return(dist);
}
