/* $Header: obj_xxx.c,v 1.5 89/07/02 15:59:06 moraes Exp $ */
/*
 *  The xxx object routines 
 */
#include "xpic.h"
#include "windows.h"
#include "spline.h"
#include "gels.h"
#include "draw.h"
#include "input.h"
#include "newfonts.h"
#include "assert.h"


xxx_event(evtype, mx, my)
{
	switch(evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
		break;
	case MOTION | END_MODE:
	case LEFT | START_MODE:
	case LEFT | END_MODE:
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
	case REDRAW | END_MODE:
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown XXX mode %d", drawingMode);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "xxx_event");
}


xxx_abort()
{
	/*
	 *  We need to simulate a button press event that will terminate
	 *  or abort gracefully, and sicen we don't know our mouse
	 *  position, and pass 0, 0, the action taken must be
	 *  independent of mouse location. For some objects, that's the
	 *  MIDDLE button. For others, its the RIGHT button.
	 */
	xxx_event((APPROPRIATE_BUTTON | drawingMode), 0, 0);
}
	

xxx_adj(evtype, gel, mx, my)
int evtype;
Gel *gel;
int mx, my;
{
	/*
	 *  Will not need to process MOTION|START_MODE, RIGHT|START_MODE,
	 *  REDRAW|START_MODE - these are taken care of in
	 *  the adj_element routine.
	 */
	switch(evtype) {
	case MOTION | END_MODE:
		break;
	case LEFT | START_MODE:
		break;
	case LEFT | END_MODE:
		break;
	case RIGHT | END_MODE:
		break;
	case MIDDLE | START_MODE:
		break;
	case MIDDLE | END_MODE:
		break;
	case REDRAW | END_MODE:
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown mode %d in text_adj", 
		 evtype);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "xxx_adj");
}

/*
 * Finds distance of point from the gel. This is computed using a gel
 * dependent rule
 */
xxx_distance(gel, mx, my)
Gel *gel;
int mx, my;
{
}
