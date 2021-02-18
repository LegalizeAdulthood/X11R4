/* $Header: obj_elem.c,v 1.6 89/07/02 15:58:51 moraes Exp $ */
/*
 *  The element editing routines.
 */
#include "xpic.h"
#include "windows.h"
#include "spline.h"
#include "gels.h"
#include "input.h"
#include "newfonts.h"
#include "assert.h"

static Gel *gel;
static int origX, origY;
static int lastX, lastY;

/* 
 *  Basic rationale: START_MODE means wandering around with the mouse, a
 *  LEFT click means selecting the nearest object. It gets attached to
 *  the mouse, and goes into END_MODE. A right click puts it back where
 *  it was, and marks it so that the next LEFT click won't select it,
 *  but will instead select the next-nearest object. A MIDDLE button
 *  aborts, restoring the object to its old position, and back to
 *  START_MODE. If DELETE is desired, the object gets highlighted
 *  instead of attached to the mouse, and the user is asked to confirm
 *  the delete with a mouse click. Adjust puts you in an appropriate
 *  mode - for circles, ellipses, boxes and text it is as if you are
 *  creating the object again - for lines, splines, it selects the
 *  nearest point and rubber-bands it. You can move it anywhere you
 *  like. Scale and rotate are a bit more complicated - later.
 */
element_event(evtype, mx, my)
{
	switch (editType) {
	case COPY:
	case MOVE:
		move_element(evtype, mx, my);
		break;
	case CHANGE_ATTRIBUTE:
	case DELETE:
		delete_element(evtype, mx, my);
		break;
	case ADJUST:
		adjust_element(evtype, mx, my);
		break;
	case ROTATE:
		rotate_element(evtype, mx, my);
		break;
	case SCALE:
		scale_element(evtype, mx, my);
		break;
	case PASTE:
		move_element(evtype, mx, my);
		break;
	case PUT:
	case GET:
		message("Get/Put only allowed in BLOCK mode");
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Unknown ELEMENT editType - %d", editType);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "element_event");
}


element_abort()
{
	element_event((MIDDLE | drawingMode), 0, 0);
}
	

/*
 *  This is called from element_event if the editType is MOVE, COPY, or
 *  PASTE. We do slightly different things for each
 */
move_element(evtype, mx, my)
{
	int remove_old_gel = (editType == MOVE);
	
	switch(evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case REDRAW | START_MODE:
		break;
	case MIDDLE | START_MODE:
		ClearGelFlags(CurrentCell->gelList);
		break;
	case MOTION | END_MODE:
		GelDraw(gel, INVERT);
		MoveGel(gel, mx - lastX, my - lastY);
		lastX = mx;
		lastY = my;
		GelDraw(gel, INVERT);
		break;
	case LEFT | START_MODE:
		origX = lastX = mx;
		origY = lastY = my;
		if (editType == PASTE) {
			if (KillBuffer == NULL) {
				message("Nothing to paste. Delete something first");
				return;
			}
			gel = CopyGel(KillBuffer, 1);
			MoveGel(gel, mx - KillX, my - KillY);
		} else { /* MOVE, COPY */
			if ((gel = SelectGel(CurrentCell->gelList, mx, my)) == NULL)
				return;
			if (remove_old_gel) {
				GelDraw(gel, ERASE);
				gel = RemoveGel(&(CurrentCell->gelList), gel);
			} else {
				gel = CopyGel(gel, 1);
			}
		}
		GelDraw(gel, INVERT);
		drawingMode = END_MODE;
		break;
	case LEFT | END_MODE:
		GelDraw(gel, INVERT);
		MoveGel(gel, mx - lastX, my - lastY);
		lastX = mx;
		lastY = my;
		GelDraw(gel, DRAW);
		FreeGel(KillBuffer);
		KillBuffer = CopyGel(gel, 1);
		KillX = mx;
		KillY = my;
		(void) PushGel(&(CurrentCell->gelList), gel);
		FreeGel(CurrentCell->undoList);
		if (editType == MOVE) {
			/*
			 *  To undo a move, we need to clobber the new
			 *  gel (in its moved position), and put the old
			 *  gel back
			 */
			CurrentCell->undoList = CopyGel(gel, 1);
			MoveGel(CurrentCell->undoList, 
			 origX - lastX, origY - lastY);
		} else {/* COPY, PASTE */
			/*
			 *  For Copy or Paste, we only need to zonk the
			 *  new gel
			 */
			CurrentCell->undoList = NULL;
		}
		gel = NULL;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		GelDraw(gel, INVERT);
		if (editType == PASTE || editType == COPY) {
			FreeGel(gel);
			gel = NULL;
		} else if (editType == MOVE) {
			MoveGel(gel, origX - lastX, origY - lastY);
			GelDraw(gel, DRAW);
			(void) PushUnderUndo(&(CurrentCell->gelList), gel,
			 CurrentCell->undo);
		} 
#ifdef DEBUG
		else {
			(void) sprintf(errstring, "move_element: RIGHT editType = %d",
			 editType);
			message(errstring);
		}
#endif
		if (evtype == (MIDDLE | END_MODE))
			ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		GelDraw(gel, INVERT);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown mode %d in move_element", 
		 evtype);
		message(errstring);
#endif
		break;
	}
}


/*
 *  This should actually be called ClearSelectedGels since that's what
 *  it does - it does not affect the HILITED flag, for instance
 */
ClearGelFlags(g)
register Gel *g;
{
#ifdef DEBUG
	int cleared = 0;
#endif

	for(;g != NULL; g= g->next) {
#ifdef DEBUG
		if (g->int_flags & SELECTED) cleared++;
#endif
		g->int_flags &= ~SELECTED;
	}
	
#ifdef DEBUG
	(void) sprintf(errstring, "Cleared %d elements which had been selected",
	 cleared);
	message(errstring);
#endif
}


/*
 *  This is called with editTypes of DELETE or CHANGE_ATTRIBUTE. The
 *  first simply deletes the selected gel after confirmation, the latter
 *  changes the gel's attributes to the current ones after confirmation.
 *  END_MODE simply means wait to confirm.
 */
delete_element(evtype, mx, my)
{
	switch(evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case REDRAW | START_MODE:
	case MOTION | END_MODE:
		break;
	case MIDDLE | START_MODE:
		ClearGelFlags(CurrentCell->gelList);
		break;
	case LEFT | START_MODE:
		if ((gel = SelectGel(CurrentCell->gelList, mx, my)) == NULL)
			return;
		origX = mx;
		origY = my;
		gel = RemoveGel(&(CurrentCell->gelList), gel);
		GelDraw(gel, ERASE);
		GelHilite(gel);
		(void) sprintf(errstring, "Click Left button to confirm %s", 
		 (editType == DELETE) ? "Delete" : "Change");
		message(errstring);
		drawingMode = END_MODE;
		break;
	case LEFT | END_MODE:
		GelUnHilite(gel);
		GelDraw(gel, ERASE);
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = gel;
		if (editType == DELETE) {
			CurrentCell->undo = 0;
			FreeGel(KillBuffer);
			KillBuffer = CopyGel(gel, 1);
			KillX = origX;
			KillY = origY;
		} else { /* editType == CHANGE_ATTRIBUTE */
			CurrentCell->undo = 1;
			gel = CopyGel(gel, 1);
			ChangeAttrib(gel, line_type, line_arrow, lineThickness,
			 fill_type, 
			 textVertAlign | textHorizAlign, fontType, textSize);
			GelDraw(gel, DRAW);
			(void) PushGel(&(CurrentCell->gelList), gel);
			gel = NULL;
		}
		CurrentCell->saved |= MODIFIED;
		ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		GelUnHilite(gel);
		GelDraw(gel, DRAW);
		(void) PushUnderUndo(&(CurrentCell->gelList), gel, CurrentCell->undo);
		if (evtype == (MIDDLE | END_MODE))
			ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		GelHilite(gel);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown mode %d in delete_element", 
		 evtype);
		message(errstring);
#endif
		break;
	}
}


/*
 *  This is not as symmetric as the others because we first want to
 *  select the gel, and then, depending on the type of gel, we need to
 *  process the events differently. Each object should have an xxx_adj
 *  call which will be called by this routine.
 */
adjust_element(evtype, mx, my)
{
	/* Need to select the Gel first */
	switch (evtype) {
	case MOTION | START_MODE:
	case RIGHT | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
	 	/* Do nothing */
	 	return;
 	case LEFT | START_MODE:
		if ((gel = SelectGel(CurrentCell->gelList, mx, my)) == NULL)
			return;
		origX = lastX = mx;
		origY = lastY = my;
		GelDraw(gel, ERASE);
		gel = RemoveGel(&(CurrentCell->gelList), gel);
		break;
	default:
		/* This must be processed individually for each type of gel */
		break;
	}

	/*
	 *  we get here after selecting a gel - or with a gel already
	 *  selected. So we now call the appropriate xxx_adj to do the work.
	 */
	switch(gel->type) {
	case TEXT:
		text_adj(evtype, gel, mx, my);
		break;
	case BOX:
		box_adj(evtype, gel, mx, my);
		break;
	case ELLIPSE:
		ellipse_adj(evtype, gel, mx, my);
		break;
	case CIRCLE:
		circle_adj(evtype, gel, mx, my);
		break;
	case LINE:
		line_adj(evtype, gel, mx, my);
		break;
	case SPLINE:
		spline_adj(evtype, gel, mx, my);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Unknown gel->type %d in adjust_element", 
		 gel->type);
		message(errstring);
#endif
		break;
	}
}


/*ARGSUSED*/
scale_element(evtype, mx, my)
{
}


/*ARGSUSED*/
rotate_element(ev_type, mx, my)
{
}
