/* $Header: obj_box.c,v 1.4 89/07/02 15:58:47 moraes Exp $ */
/*
 *  The box object routines
 */
#include <math.h>

#include "xpic.h"
#include "windows.h"
#include "gels.h"
#include "draw.h"
#include "assert.h"

static int x_1, y_1, x_2, y_2;			/* Corners of box, ellipse, ends of line */
static int xmin, xmax, ymin, ymax;	/* Bounding box */

box_event(evtype, mx, my)
{
	switch(evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
	case RIGHT  | END_MODE:
		break;
	case MOTION | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcInvert);
		x_2 = mx;
		y_2 = my;
		box(picWin, x_1, y_1, x_2, y_2, gcInvert);
		break;
	case LEFT | START_MODE:
		x_1 = x_2 = mx;
		y_1 = y_2 = my;
		drawingMode = END_MODE;
		box(picWin, x_1, y_1, x_2, y_2, gcInvert);
		break;
	case LEFT | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcInvert);
		x_2 = mx;
		y_2 = my;
		xmin = MIN(x_1, mx);
		xmax = MAX(x_1, mx);
		ymin = MIN(y_1, my);
		ymax = MAX(y_1, my);
		box(picWin, x_1, y_1, x_2, y_2, gcNormal);
		AddBoxGel(&(CurrentCell->gelList), xmin, ymin, xmax, ymax,
		 line_type | fill_type, lineThickness);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = NULL;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case MIDDLE | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcInvert);
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcInvert);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown BOX mode %d", drawingMode);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "box_event");
}


box_abort()
{
	box_event((MIDDLE | drawingMode), 0, 0);
}
	

box_adj(evtype, gel, mx, my)
int evtype;
Gel *gel;
int mx, my;
{
	static Gel *boxgel;
	/*
	 *  Will not need to process MOTION|START_MODE, RIGHT|START_MODE,
	 *  REDRAW|START_MODE - these are taken care of in
	 *  the adj_element routine.
	 */
	switch(evtype) {
	case MOTION | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, tmpGcInvert);
		x_2 = mx;
		y_2 = my;
		box(picWin, x_1, y_1, x_2, y_2, tmpGcInvert);
		break;
	case LEFT | START_MODE:
		GetBoxCorners(&x_1, &y_1, &x_2, &y_2, &gel->b_box, mx, my);
		boxgel = gel;
		drawingMode = END_MODE;
		setwidth(tmpGcNormal, boxgel->linewidth);
		setwidth(tmpGcInvert, boxgel->linewidth);
		SETDASHES(tmpGcNormal, getlinestyle(boxgel->attributes))
		SETDASHES(tmpGcInvert, getlinestyle(boxgel->attributes))
		box(picWin, x_1, y_1, x_2, y_2, tmpGcInvert);
		break;
	case LEFT | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, tmpGcInvert);
		x_2 = mx;
		y_2 = my;
		xmin = MIN(x_1, mx);
		xmax = MAX(x_1, mx);
		ymin = MIN(y_1, my);
		ymax = MAX(y_1, my);
		box(picWin, x_1, y_1, x_2, y_2, tmpGcNormal);
		AddBoxGel(&(CurrentCell->gelList), xmin, ymin, xmax, ymax,
		 boxgel->attributes, boxgel->linewidth);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = boxgel;
		boxgel = NULL;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, tmpGcInvert);
		GelDraw(boxgel, DRAW);
		(void) PushUnderUndo(&(CurrentCell->gelList), boxgel,
		 CurrentCell->undo);
		boxgel = NULL;
		if (evtype == (MIDDLE | END_MODE))
			ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case MIDDLE | START_MODE:
		ClearGelFlags(CurrentCell->gelList);
		break;
	case REDRAW | END_MODE:
		box(picWin, x_1, y_1, x_2, y_2, tmpGcInvert);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown mode %d in box_adj", 
		 evtype);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "box_adj");
}

/*
 *  Puts box the corner of b that is farthest from mx, my in x_1, y_1, and
 *  closer corner in x_2, y_2 (which will get moved)
 */
GetBoxCorners(x_1, y_1, x_2, y_2, b, mx, my)
int *x_1, *x_2, *y_1, *y_2;
Box *b;
int mx, my;
{
	int d1, d2, tmp;
	
	*x_1 = b->ll.x;
	*y_1 = b->ll.y;
	*x_2 = b->ur.x;
	*y_2 = b->ur.y;
	/* Make sure x_1, y_1 is farthest point in box from mouse coords */
	d1 = ABS(*x_1 - mx);
	d2 = ABS(*x_2 - mx);
	if (d1 < d2) {
		tmp = *x_1;
		*x_1 = *x_2;
		*x_2 = tmp;
	}
	d1 = ABS(*y_1 - my);
	d2 = ABS(*y_2 - my);
	if (d1 < d2) {
		tmp = *y_1;
		*y_1 = *y_2;
		*y_2 = tmp;
	}
}


/*
 * Finds distance of point from a box. This is the distance of the closest
 * edge of the box to the point. We compute it by using the four logical
 * conditions to determine which region of the box the point lies in.
 */
/*
 *       6             7             5
 *
 *               +------------+
 *               |            |
 *      14       |    15      |     13
 *               |            |
 *               +------------+
 *
 *      10            11             9
 */
/*
 * In regions 5, 6, 9, 10, the distance is from the nearest corner. For
 * regions 7, 11, 13, 14, the distance is from the nearest edge, and for
 * region 15, the distance is the minimum of the distance from all the edges
 */
int
box_distance(gel, xp, yp)
Gel *gel;
int xp, yp;
{
	int region = 0;
	int dist;
	Box *b = &gel->b_box;
	int dx_1 = xp - b->ll.x;
	int dy_1 = yp - b->ll.y;
	int dx_2 = b->ur.x - xp;
	int dy_2 = b->ur.y - yp;

	if (dx_1 >= 0)
		region += 1;
	if (dx_2 >= 0)
		region += 2;
	if (dy_1 >= 0)
		region += 4;
	if (dy_2 >= 0)
		region += 8;

	switch(region) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		ASSERT(0, "Impossible region in box");
		break;
	case 5:
		dist = sqrt((double) (dx_2 * dx_2 + dy_2 * dy_2));
		break;
	case 6:
		dist = sqrt((double) (dx_1 * dx_1 + dy_2 * dy_2));
		break;
	case 7:
		dist = -dy_2;
		break;
	case 8:
		ASSERT(0, "Impossible region in box");
		break;
	case 9:
		dist = sqrt((double) (dx_2 * dx_2 + dy_1 * dy_1));
		break;
	case 10:
		dist = sqrt((double) (dx_1 * dx_1 + dy_1 * dy_1));
		break;
	case 11:
		dist = -dy_1;
		break;
	case 12:
		ASSERT(0, "Impossible region in box");
		break;
	case 13:
		dist = -dx_2;
		break;
	case 14:
		dist = -dx_1;
		break;
	case 15:
		dist = MIN(dx_1, dx_2);
		dist = MIN(dist, dy_1);
		dist = MIN(dist, dy_2);
		break;
	}
	return(dist);
}
