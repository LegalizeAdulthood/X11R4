/* $Header: obj_block.c,v 1.8 89/07/02 15:58:44 moraes Exp $ */
/* 
 *  The block edit routines - treated as a separate pseudo-object
 *  because they are so similar. The code is somewhat intricate - the
 *  result of trying to cram as many operations into as little code as
 *  possible, so that the code to do something stayed in the one place.
 *  Sigh! macros might have been nicer.
 */
#include <values.h>

#include "xpic.h"
#include "windows.h"
#include "spline.h"
#include "gels.h"
#include "draw.h"
#include "input.h"
#include "newfonts.h"
#include "assert.h"

static int x_1, y_1, x_2, y_2;			/* Corners of box, ellipse, ends of line */
static int xmin, xmax, ymin, ymax;	/* Bounding box */
static Gel *gel;
static Gel *oldgel;
static Cell *cell;
static Box *bp;
static Box adjbox;
static int lastX, lastY;
static int first_time = FALSE;

/*
 *  For the editing constructs, we also may have a DRAG_MODE (where the
 *  object is dragged around, or an ASK_MODE where the user is asked to
 *  confirm the operation (usually a delete, with a click
 */
block_event(evtype, mx, my)
{
	char *err;
	register Gel *tmp;
	
	switch(evtype) {
	case MOTION | START_MODE:
	case MOTION | ASK_MODE:
	case RIGHT  | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
	case REDRAW | ASK_MODE:
		break;
	case MOTION | END_MODE:
		/* rubber band the box corner */
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		x_2 = mx;
		y_2 = my;
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		break;
	case MOTION | DRAG_MODE:  
		/*
		 *  move the box around on the cursor - use the second corner as
		 *  the mouse corner
		 */
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		x_1 += mx - x_2;
		y_1 += my - y_2;
		x_2 = mx;
		y_2 = my;
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		break;
	case LEFT | START_MODE:
		/* start the box */
		first_time = TRUE;
		if (editType == PASTE) {
			if (KillBuffer == NULL) {
				message("Nothing to paste. Delete something first");
				break;
			}
			gel = CopyGel(KillBuffer, MAXINT);
			MoveGel(gel, mx - KillX, my - KillY);
			bp = GetBBox(gel);
			x_1 = bp->ll.x;
			y_1 = bp->ll.y;
			lastX = x_2 = bp->ur.x;
			lastY = y_2 = bp->ur.y;
			drawingMode = DRAG_MODE;
		} else if (editType == GET) {
			if (cell)
				FreeCell(cell);
			cell = ReadCell("Get from file ? ", (char *) NULL);
			if ( cell == NULL) 
				break;
			else if (cell->gelList == NULL) {
				message("Can't paste an empty cell!");
				FreeCell(cell);
				cell = NULL;
				break;
			}
			gel = cell->gelList;
			cell->gelList = NULL;
			bp = GetBBox(gel);
			x_1 = bp->ll.x;
			y_1 = bp->ll.y;
			lastX = x_2 = bp->ur.x;
			lastY = y_2 = bp->ur.y;
			drawingMode = DRAG_MODE;
		} else if (editType == SCALE || editType == ROTATE) {
			message("SCALE/ROTATE not yet implemented");
			break;
		} else {
			x_1 = x_2 = mx;
			y_1 = y_2 = my;
			drawingMode = END_MODE;
		}
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		break;
	case LEFT | END_MODE:
		/* Won't get here for GET, PASTE - they go straight to DRAG */
		/* End the box, find the contained gels, highlight them */
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		lastX = x_2 = mx;
		lastY = y_2 = my;
		xmin = MIN(x_1, mx);
		xmax = MAX(x_1, mx);
		ymin = MIN(y_1, my);
		ymax = MAX(y_1, my);
		/*
		 *  The very nature of ADJUST means that we want
		 *  intersecting Gels - for the others, we want only
		 *  those that are strictly contained within the box
		 */
		if (editType == ADJUST) {
			oldgel = FindIntersectingGels(&(CurrentCell->gelList), 
			 xmin, ymin, xmax, ymax);
			gel = CopyGel(oldgel, MAXINT);
			adjbox.ll.x = xmin;
			adjbox.ll.y = ymin;
			adjbox.ur.x = xmax;
			adjbox.ur.y = ymax;
			err = "No intersecting elements";
		} else {
			oldgel = FindContainedGels(&(CurrentCell->gelList), 
			 xmin, ymin, xmax, ymax);
			gel = CopyGel(oldgel, MAXINT);
			err = "No contained elements";
		}
		if (oldgel == NULL) {
			message(err);
			drawingMode = START_MODE;
			break;
		}
		for (tmp = gel; tmp != NULL; tmp = tmp->next)
			GelDraw(tmp, ERASE);
		GelHilite(gel);
		if (editType == DELETE || editType == CHANGE_ATTRIBUTE) {
			drawingMode = ASK_MODE;
			(void) sprintf(errstring, "Click Left button to confirm %s",
			 (editType == DELETE) ? "Delete" : "Change");
			message(errstring);
		} else if (editType == COPY || editType == MOVE || 
		 editType  == ADJUST) {
		 	if (editType == COPY) {
				/* 'gel' must be a copy of the gels selected */
				(void) PushUnderUndo(&(CurrentCell->gelList), oldgel, 
				 CurrentCell->undo);
				oldgel = NULL;
			}
			drawingMode = DRAG_MODE;
			/* Draw the rubber banded box for drag */
			box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		} else if (editType == PUT) {
			if (cell)
				FreeCell(cell);
			(void) PushGel(&(CurrentCell->gelList), oldgel);
			if ((cell = NewCell((char *) NULL, nullfile)) != NULL) {
				cell->gelList = gel;
				cell->saved |= MODIFIED;
				/* 
				 *  If we try to write out a cell with a name of
				 *  nullfile, it will ask for the name
				 */
				(void) WriteCell(cell, backupOnWrite);
				cell->gelList = NULL;
			}
			GelUnHilite(gel);
			for (tmp = gel; tmp != NULL; tmp = tmp->next)
				GelDraw(tmp, DRAW);
			FreeGel(gel);
			gel = NULL;
			drawingMode = START_MODE;
		} else { /* shouldn't get here, since SCALE, ROTATE aren't working! */ 
			(void) sprintf(errstring,
			 "Unknown editType %d in block_event - LEFT", editType);
			message(errstring);
			(void) PushGel(&(CurrentCell->gelList), oldgel);
			GelUnHilite(gel);
			FreeGel(gel);
			gel = NULL;
			drawingMode = START_MODE;
		}
		break;
	case LEFT | DRAG_MODE:
		/* 
		 *  Every time LEFT is clicked in DRAG mode, we move all
		 *  the objects in the gel list here, highlight them,
		 *  and stay in this mode - gives the users multiple
		 *  tries at adjusting the block since it isn't
		 *  completely WYSIWYG. PUT doesn't get here since it
		 *  ends in END_MODE, DELETE and CHANGE_ATTRIB go via
		 *  ASK_MODE
		 */
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		x_1 += mx - x_2;
		y_1 += my - y_2;
		x_2 = mx;
		y_2 = my;
		if (first_time) {
			first_time = FALSE;
		 	if (editType == COPY) {
				GelUnHilite(gel);
				for (tmp = gel; tmp != NULL; tmp = tmp->next) 
					GelDraw(tmp, DRAW);
			} else if (editType == MOVE || editType == ADJUST) {
				GelUnHilite(gel);
				for (tmp = gel; tmp != NULL; tmp = tmp->next) 
					GelDraw(tmp, ERASE);
			} else { /* GET, PASTE */
				/* Do nothing */
			}
		} else { /* Not first time, so we zonk the last position */
			GelUnHilite(gel);
		}
		if (editType == ADJUST) {
			AdjustGel(gel, &adjbox, mx - lastX, my - lastY);
			adjbox.ll.x = MIN(x_1, x_2);
			adjbox.ll.y = MIN(y_1, y_2);
			adjbox.ur.x = MAX(x_1, x_2);
			adjbox.ur.y = MAX(y_1, y_2);
		} else
			MoveGel(gel, mx - lastX, my - lastY);
		lastX = mx;
		lastY = my;
		GelHilite(gel);
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		break;
	case RIGHT | DRAG_MODE:
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		if ((!first_time) || editType == COPY || editType == MOVE || 
		 editType == ADJUST) {
			GelUnHilite(gel);
		}
		if (!(first_time && (editType == GET || editType == PASTE))) {
			for (tmp = gel; tmp != NULL; tmp = tmp->next) 
				GelDraw(tmp, DRAW);
			FreeGel(CurrentCell->undoList);
			CurrentCell->undoList = NULL;
			if (editType != GET && editType != PASTE) {
				CurrentCell->undoList = oldgel;
				oldgel = NULL;
			}
			FreeGel(KillBuffer);
			KillBuffer = CopyGel(gel, MAXINT);
			KillX = mx;
			KillY = my;
			CurrentCell->undo = PushGel(&(CurrentCell->gelList), 
			 gel);
			CurrentCell->saved |= MODIFIED;
		}
		gel = NULL;
		drawingMode = START_MODE;
		break;		
	case LEFT  | ASK_MODE:
		/* Delete, change the gels */
		GelUnHilite(gel);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = oldgel;
		oldgel = NULL;
		if (editType == DELETE) {
			CurrentCell->undo = 0;
			FreeGel(KillBuffer);
			KillBuffer = gel;
			KillX = mx;
			KillY = my;
		} else { /* CHANGE_ATTRIB */
			ChangeAttrib(gel, line_type, line_arrow, lineThickness, 
			 fill_type, 
			 textVertAlign | textHorizAlign, fontType, textSize);
			for (tmp = gel; tmp != NULL; tmp = tmp->next) 
				GelDraw(tmp, DRAW);
			CurrentCell->undo = PushGel(&(CurrentCell->gelList), gel);
		}
		gel = NULL;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case RIGHT | ASK_MODE:
	case MIDDLE | ASK_MODE:
		/* Abort */
		GelUnHilite(gel);
		for (tmp = gel; tmp != NULL; tmp = tmp->next) 
			GelDraw(tmp, DRAW);
		(void) PushUnderUndo(&(CurrentCell->gelList), gel, CurrentCell->undo);
		gel = NULL;
		drawingMode = START_MODE;
		break;
	case MIDDLE | DRAG_MODE:
		/* Abort, unhilite the gels - delete any copies */
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		/* PUT only gets as far as END_MODE - won't get here */
		if ((!first_time) || editType == COPY || editType == MOVE || 
		 editType == ADJUST) {
			GelUnHilite(gel);
		}
		if (editType == COPY || editType == PASTE || editType == GET) {
			FreeGel(gel);
			if (cell)
				FreeCell(cell);
			cell = NULL;
		} else if (editType == MOVE || editType == ADJUST) {
			FreeGel(gel);
			gel = oldgel;
			for (tmp = gel; tmp != NULL; tmp = tmp->next) 
				GelDraw(tmp, DRAW);
			(void) PushUnderUndo(&(CurrentCell->gelList), gel,
			 CurrentCell->undo);
		} else { /* can't happen, since we don't have SCALE/ROTATE */
			(void) sprintf(errstring,
			 "Unknown editType %d in block_event - MIDDLE", editType);
			message(errstring);
		}
		gel = NULL;
		drawingMode = START_MODE;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		/* Abort - stop rubber banding */
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		/* redraw the rubber band */
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		break;
	case REDRAW | DRAG_MODE:
		/* highlight the gels, redraw the drag box */
		box(picWin, x_1, y_1, x_2, y_2, gcBlock);
		if (!first_time || (editType == COPY || editType == MOVE 
		 || editType == ADJUST)) 
			GelHilite(gel);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown BLOCK mode %d", drawingMode);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "block_event");
}

block_abort()
{
	/* Fudge up a RIGHT button pressed event - safer thing to do */
	block_event((RIGHT | drawingMode), 0, 0);
}
	
