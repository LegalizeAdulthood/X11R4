/* $Header: event.c,v 1.5 89/07/02 15:58:10 moraes Exp $ */
/* 
 *  The event handler determines which object's event processor to call,
 *  and passes it the event type, and object mode. Each object module
 *  must provide a procedure called xxx_event, where xxx is the object
 *  name, which takes the event code, and returns the new editing state.
 *  The event code consistes of the current drawingMode (START_MODE, END_MODE,
 *  DRAG_MODE, ASK_MODE), or'ed with the event type (REDRAW, MOTION, LEFT,
 *  MIDDLE, RIGHT). The object event routine will usually take some
 *  appropriate action, and return the new drawingMode. The object
 *  module must also provide a xxx_abort routine, which can be invoked
 *  by the CleanUpMode routine if the user hits a menu button whil estill
 *  editing something.
 */

#include <values.h>
#include "xpic.h"
#include "windows.h"
#include "newfonts.h"
#include "gels.h"
#include "draw.h"
#include "spline.h"

static int mx, my;					/* Mouse coordinates after snap */

/*
 *  processes events in the pic window
 */
/*ARGSUSED*/
void picEventHandle(w, data, event)
Widget w;
caddr_t data;
XEvent *event;
{
	int event_mode = drawingMode;

	switch (event->type) {
	case Expose:
		event_mode |= REDRAW;
		/*
		 *  mx, my not important in Expose - all the object event
		 *  processor will do is redraw the last rubber banded object if
		 *  any, at the last position.
		 */
		/* Can do selective redraw, but it is hardly  necessary */
		PicRedraw(CurrentCell->gelList, &picBox);
		break;
	case MotionNotify:
		event_mode |= MOTION;
		/*
		 *  We use the MotionNotify event as a hint, and ask for the
		 *  mouse position. We use the mouse position to rubber badn -
		 *  this is essentially "jump" rubber banding, and is possible
		 *  because we have the compress_motion flag on the Window
		 *  widget set to TRUE to make sure the MotionNotify events are
		 *  hints, and not a stream of actual mouse movements.
		 */
		{
			Window root_return, child_return;
			int root_x_return, root_y_return, win_x_return, win_y_return;
			unsigned int mask_return;
			
			(void) XQueryPointer(picDpy, picWin, &root_return, &child_return, 
			 &root_x_return, &root_y_return, &win_x_return, &win_y_return, 
			 &mask_return);
			mx = snap(win_x_return, mouseResolution);
			my = snap(win_y_return, mouseResolution);
		}
		/* Usually, if in start, then ignore, else rubber_band or drag */
		break;
	case ButtonPress:
		mx = snap(event->xbutton.x, mouseResolution);
		my = snap(event->xbutton.y, mouseResolution);
		switch (event->xbutton.button) {
		case Button1:
			event_mode |= LEFT;
			/* Decision button */
			break;
		case Button3:
			event_mode |= RIGHT;
			/* termination button for lines, splines, editing actions */
			break;
		case Button2:
			event_mode |= MIDDLE;
			/* Abort current action */
			break;
		default:
#ifdef DEBUG
			(void) sprintf(errstring, "unknown button in picEventhandle - %d",
			 event->xbutton.button);
			message(errstring);
#endif
			break;
		}
		break;
	case ConfigureNotify:
		event_mode |= REDRAW;
		if ((event->xconfigure.width != picWinWidth) 
		 || (event->xconfigure.height != picWinHeight)) {
#ifdef DEBUG
			(void) fprintf(stderr, "Window Size changed to %dx%d\n", 
			 event->xconfigure.width, event->xconfigure.height);
#endif
			picWinWidth = picBox.ur.x = event->xconfigure.width;
			picWinHeight = picBox.ur.y = event->xconfigure.height;
			pageWidth = picWinWidth / gridSpacing + 0.5;
			pageHeight = picWinHeight / gridSpacing + 0.5;
		}
		/* Can do selective redraw, but it is hardly  necessary */
		XClearWindow(picDpy, picWin);
		PicRedraw(CurrentCell->gelList, &picBox);
		break;
	default:
		/* Various other Notify types will end up here. */
#ifdef DEBUG
		(void) sprintf(errstring,
		 "picEventHandle: Unknown event %d", event->type);
		message(errstring);
#endif
		return;
	}

	/* Now call the object event handler  - wouldn't this be easy in C++*/
	switch (objectType) {
	case LINE:
		line_event(event_mode, mx, my);
		break;
	case SPLINE:
		spline_event(event_mode, mx, my);
		break;
	case BOX:
		box_event(event_mode, mx, my);
		break;
	case CIRCLE:
		circle_event(event_mode, mx, my);
		break;
	case ELLIPSE:
		ellipse_event(event_mode, mx, my);
		break;
	case TEXT:
		text_event(event_mode, mx, my);
		break;
	case BLOCK:
		block_event(event_mode, mx, my);
		break;
	case ELEMENT:
		element_event(event_mode, mx, my);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Unknown object %d", objectType);
		message(errstring);
#endif
		break;
	}
}


/*
 *  This gets called when the user presses a button to change a
 *  selection - it cleans up any rubber bands appropriately, by calling
 *  the object procedure xxx_abort, where xxx is the object name.
 */
/* !! remove the xxx_abort procedures by calling xxx_event() directly */
void CleanUpMode()
{
	switch(objectType) {
	case LINE:
		line_abort();
		break;
	case SPLINE:
		spline_abort();
		break;
	case BOX:
		box_abort();
		break;
	case ELLIPSE:
		ellipse_abort();
		break;
	case CIRCLE:
		circle_abort();
		break;
	case TEXT:
		text_abort();
		break;
	case BLOCK:
		block_abort();
		break;
	case ELEMENT:
		element_abort();
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Unknown object in cleanup? %d", 
		 objectType);
		message(errstring);
#endif
		break;
	}
}


/* 
 *  The routine GelDraw to draw an element takes a pointer to a Gel and
 *  depending on the type, invokes different drawing methods
 */
void GelDraw(g, func)
Gel *g;
int func;
{
	XFontStruct *font;
	int pad, n;
	PointList *pt;
	Conic *conic;
	TextString *text;
	GC gc;
	
	switch (func) {
	case DRAW:
		gc = tmpGcNormal;
		break;
	case ERASE:
		gc = tmpGcErase;
		break;
	case INVERT:
	case HILITE:
		gc = tmpGcInvert;
		break;
	}

	SETDASHES(gc, getlinestyle(g->attributes))
	if (func == HILITE) 
		setwidth(gc, g->linewidth + 2);
	else
		setwidth(gc, g->linewidth);

#ifdef DRAWBBOX
	/* Draw bounding boxes for all stuff except Boxes */
	if (g->type != BOX)
		box(picWin, g->b_box.ll.x, g->b_box.ll.y, 
		 g->b_box.ur.x, g->b_box.ur.y, gcBlock);
#endif

	switch (g->type) {
	case LINE:
		pt = (PointList *) g->data;
		drawlines(picDpy, picWin, gc, pt->v, pt->nVerts, CoordModeOrigin);
		if (pt->nVerts == 1)
			break;
		if (g->attributes & ST_ARROW)
			Arrow(picDpy, picWin, pt->v[1].x, pt->v[1].y, 
			 pt->v[0].x, pt->v[0].y, gc);
		if (g->attributes & EN_ARROW)
			Arrow(picDpy, picWin, 
			 pt->v[pt->nVerts - 2].x, pt->v[pt->nVerts - 2].y, 
			 pt->v[pt->nVerts - 1].x, pt->v[pt->nVerts - 1].y, gc);
		break;
	case SPLINE:
		pt = (PointList *) g->data;
		FlattenSpline(pt->v, pt->nVerts-1, &flatVerts2, &n, &flatSize2);
		drawlines(picDpy, picWin, gc, flatVerts2, n, CoordModePrevious);
		if (pt->nVerts == 1)
			break;
		if (g->attributes & ST_ARROW)
			Arrow(picDpy, picWin, pt->v[2].x, pt->v[2].y, 
			 pt->v[1].x, pt->v[1].y, gc);
		if (g->attributes & EN_ARROW)
			Arrow(picDpy, picWin, 
			 pt->v[pt->nVerts - 3].x, pt->v[pt->nVerts - 3].y,
			 pt->v[pt->nVerts - 2].x, pt->v[pt->nVerts - 2].y, gc);
		break;
	case BOX:
		box(picWin, g->b_box.ll.x, g->b_box.ll.y, g->b_box.ur.x, 
		 g->b_box.ur.y, gc);
		break;
	case CIRCLE:
		conic = (Conic *) g->data;
		ellipse(picWin, conic->centre.x, conic->centre.y, 
		 conic->xrad, conic->xrad, gc);
		break;
	case ELLIPSE:
		conic = (Conic *) g->data;
		ellipse(picWin, conic->centre.x, conic->centre.y, 
		 conic->xrad, conic->yrad, gc);
		break;
	case TEXT:
		text = (TextString *) g->data;
		font = ChangeFont(&text->font->sizes[text->sizeindex], &pad);
		setfont(gc, font->fid);
		/*
		 *  This weird location for drawing text is a result of the
		 *  sloppy bounding box calculation, which is teh X10 heritage -
		 *  using the bounding box lower left corner as the control
		 *  point for text. Since text in X11 uses the baseline as the y
		 *  coordinate, this code is crude. The text drawing needs
		 *  cleaning up, especially for proper space padding
		 */
		drawtext(picWin, g->b_box.ll.x, g->b_box.ur.y, text->str,
		 text->length, gc, pad);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "GelDraw: Unknown Gel type - %d", g->type);
		message(errstring);
#endif
		break;
	}
}


/* These are rather sloppy - need to be cleaned up a bit */
void GelHilite(g)
Gel *g;
{
	extern void SetWorkingCursor(), SetWaitCursor();

	/* !! Should do something interesting for color */
	SetWaitCursor();
	for (; g != NULL; g = g->next) {
		if (!(g->int_flags & HILITED)) {
			g->int_flags |= HILITED;
			if (g->type != TEXT) {
				GelDraw(g, HILITE);
			} else {
				XFillRectangle(picDpy, picWin, gcGray,
				 g->b_box.ll.x, g->b_box.ll.y,
				 (unsigned) (g->b_box.ur.x - g->b_box.ll.x),
				 (unsigned) (g->b_box.ur.y - g->b_box.ll.y));
				GelDraw(g, INVERT);
			}
		}
	}
	SetWorkingCursor();
}


void GelUnHilite(g)
Gel *g;
{
	extern void SetWorkingCursor(), SetWaitCursor();

	/* !! Should do something interesting for color */
	SetWaitCursor();
	for (; g != NULL; g = g->next) {
		if (g->int_flags & HILITED) {
			g->int_flags &= ~HILITED;
			if (g->type != TEXT) {
				GelDraw(g, HILITE);
			} else {
				/* Make sure we have a correct bounding box */
				GelDraw(g, INVERT);
				XFillRectangle(picDpy, picWin, gcGray,
				 g->b_box.ll.x, g->b_box.ll.y,
				 (unsigned) (g->b_box.ur.x - g->b_box.ll.x),
				 (unsigned) (g->b_box.ur.y - g->b_box.ll.y));
			}
		}
	}
	SetWorkingCursor();
}


/*
 *  The routine PicRedraw takes three parameters - a pointer to a Gel,
 *  and a Box which defines the Clip area needed to be drawn. */
/*
 *  Ideally, any Gel whose bbox intersects the Box passed in is redrawn.
 *  This allows the efficient use of ExposeRegion events to redraw small
 *  sections quickly. For now, we just redraw the whole thing.
 */
/* !! proper clipped redraw - note that the rubber banded stuff
 will pose problems */
/*ARGSUSED*/
void PicRedraw(g, clip)
Gel *g;
Box *clip;
{
	extern void SetWorkingCursor(), SetWaitCursor();

	SetWaitCursor();
	CalcBBox(g, MAXINT);
	for(; g != NULL; g = g->next)
		GelDraw(g, DRAW);
	SetWorkingCursor();
}


