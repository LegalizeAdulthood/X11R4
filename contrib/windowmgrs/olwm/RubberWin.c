/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) RubberWin.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "menu.h"
#include "olwm.h"

extern int	DefScreen;
extern GC	RootGC;
extern Cursor	RootWinCursor;
extern Cursor	MoveWinCursor;
extern Cursor	ResizeCursor;
extern	int	lastButton;

/*
 * UserMoveWin	-- allow user to interactively place a window
 *
 * If winInfo is not NULL that means we only want to move the
 * window pointed to by winInfo, not the whole selection list.
 * So pass this info to drawBoxes so it doesn't draw a box
 * around every member of the selection list.
 */
UserMoveWin(dpy, eventInit, offX, offY, winInfo)
Display		*dpy;
XButtonEvent	*eventInit;
int		*offX, *offY;
WinInfo		*winInfo;
{
	int	initX, initY;
	int	mouseX, mouseY;
	XEvent	event;
	Bool	windowPlaced;
	int	rounder, divider;

	XGrabPointer(dpy, RootWindow(dpy, DefScreen),
		     False, 
		     (ButtonReleaseMask | ButtonPressMask | PointerMotionMask),
		     GrabModeAsync, GrabModeAsync,
		     None,
		     MoveWinCursor,
		     CurrentTime);

	/*
	 * Grab the server, so that the outline(s) don't
	 * corrupt or get corrupted.
	 */
	XGrabServer(dpy);

	initX = eventInit->x_root;
	initY = eventInit->y_root;

	*offX = 0;
	*offY = 0;

	if ( eventInit->state & ControlMask ) {
	    rounder = 5;
	    divider = 10;
	} else {
	    rounder = 0;
	    divider = 1;
	}

	/*
	 * Draw initial outline
	 */
	drawBoxes(dpy, *offX, *offY, winInfo);

	for (windowPlaced = False; windowPlaced != True; )
	{
		XMaskEvent(dpy, ButtonReleaseMask|PointerMotionMask, &event);
	
		switch (event.type)
		{
		case ButtonRelease:
			windowPlaced = True;
			break;
		
		case MotionNotify:
			/* erase old box(es) */
			drawBoxes(dpy, *offX, *offY, winInfo);

			/*
			 * there may be multiple motion events queued up,
			 * consume them and just use the last one.
			 */
			while (XCheckTypedEvent(dpy, MotionNotify, &event))
				;

			/* set new position */
			mouseX = event.xmotion.x_root;
			mouseY = event.xmotion.y_root;

			*offX = (mouseX - initX + rounder) / divider;
			*offY = (mouseY - initY + rounder) / divider;

			/* draw new box(es) */
			drawBoxes(dpy, *offX, *offY, winInfo);
			break;
		
		default:
			if (PrintOrphans)
				DebugEvent(&event, "UserMoveWin");
			break;
		}
	}

	/*
	 * erase outline
	 */
	drawBoxes(dpy, *offX, *offY, winInfo);

	/*
	 * let go of the server
	 */
	XUngrabServer(dpy);
	XUngrabPointer(dpy, CurrentTime);
	XFlush(dpy);
}

UserResizeWin(dpy, frame, winx, winy, winw, winh, anchor)
Display		*dpy;
WinInfo		*frame;
int		*winx, *winy;
unsigned int	*winw, *winh;
int		anchor;
{
	int			mouseX, mouseY;
	Window			mRoot, mChild;
	int			relX, relY;
	int			curMask;
	XEvent			event;
	Bool			windowPlaced;
	int			newx, newy;
	unsigned int		neww, newh;
	int			firsttime = 1;
	NewXSizeHints		*sizeHints;
	int			minw, minh, widthInc, heightInc, maxw, maxh;
	int			anchorX, anchorY;
	Bool			boxDrawn = False;
	XWindowAttributes	windowAttributes;
	int			borderHeight;
	int			borderWidth;

	/*
	 * Local macro for drawing outline of window.  Use the "do {} while(0)"
	 * trick so that this macro can be used as a pseudo-function call
	 * between an "if" and the "else".
	 */
#define	RESIZEBOX()							\
	do {								\
	    boxDrawn = True;						\
	    XDrawRectangle(dpy, DefaultRootWindow(dpy),			\
			   RootGC, newx, newy, neww, newh);		\
	    XDrawRectangle(dpy, DefaultRootWindow(dpy),			\
			   RootGC, newx+1, newy+1, neww-2, newh-2);	\
	} while ( 0 )

	/* set initial position and size */
	newx = *winx;
	newy = *winy;
	neww = *winw;
	newh = *winh;

	/* calculate minimums, maximums, and increments */
	sizeHints = GetSizeHints(frame);

	/* Because the application's window might have a height
	 * or width increment for resizing we have to remove
	 * the height and width of the framing borders when we
	 * calculate window size increments.  For example, an
	 * xterm will have a height increment equal to the height
	 * of the font it is using so the xterm won't have half
	 * a line of text at the bottom of the window.
	 */
	XGetWindowAttributes(dpy, GetPane(frame), &windowAttributes);

	/* The height (in pixels) of the top and bottom borders
	 * is the height of the framing window minus the height
	 * of the application's actual pane window.
	 */
	borderHeight = (int)(newh - windowAttributes.height);
	borderWidth = (int)(neww - windowAttributes.width);

	if (sizeHints)	/* if we have size hints */
	{
		if (sizeHints->flags & PMinSize)
		{
			minw = sizeHints->min_width + borderWidth;
			minh = sizeHints->min_height + borderHeight;
		}
		else
		{
			/* no specified minimum */
			minw = borderWidth;
			minh = borderHeight;
		}

		if (sizeHints->flags & PMaxSize)
		{
			maxw = sizeHints->max_width + borderWidth;
			maxh = sizeHints->max_height + borderHeight;
		}
		else
		{
			/* no hint so max is display size */
			maxw =  DisplayWidth(dpy, DefaultScreen(dpy));
			maxh =  DisplayHeight(dpy, DefaultScreen(dpy));
		}

		if (sizeHints->flags & PResizeInc)
		{
			widthInc = sizeHints->width_inc;
			heightInc = sizeHints->height_inc;
		}
		else
		{
			/* no hint so increment is not restricted */
			widthInc = heightInc = 1;
		}
	}
	else		/* no size hints */
	{
		minw = borderWidth;
		minh = borderHeight;
		maxw =  DisplayWidth(dpy, DefaultScreen(dpy));
		maxh =  DisplayHeight(dpy, DefaultScreen(dpy));
		widthInc = heightInc = 1;
	}

	/* find out anchor point */
	switch(anchor)
	{
		case AnchorUL:	/* mouse defines LR corner */
			anchorX = newx;
			anchorY = newy;
			break;

		case AnchorUR:	/* mouse defines LL corner */
			anchorX = (int)(newx + neww);
			anchorY = newy;
			break;

		case AnchorLL:	/* mouse defines UR corner */
			anchorX = newx;
			anchorY = (int)(newy + newh);
			break;

		case AnchorLR:	/* mouse defines UL corner */
			anchorX = (int)(newx + neww);
			anchorY = (int)(newy + newh);
			break;
	}
		

	/*
	 * Change the cursor.
	 */
	XGrabPointer(dpy, RootWindow(dpy, DefScreen),
		     True, (ButtonReleaseMask | PointerMotionMask),
		     GrabModeAsync, GrabModeAsync,
		     None,
		     ResizeCursor,
		     CurrentTime);

	/*
	 * Grab the server, so that the outline doesnt
	 * corrupt or get corrupted.
	 */
	XGrabServer(dpy);

	/* Get initial mouse pointer location */
	XQueryPointer(dpy, RootWindow(dpy, DefScreen),
		      &mRoot, &mChild,
		      &mouseX, &mouseY,
		      &relX, &relY, &curMask);
	

	for (windowPlaced = False; windowPlaced != True; )
	{
		XMaskEvent(dpy, ButtonReleaseMask|PointerMotionMask, &event);
		switch (event.type)
		{
		case ButtonRelease:
			/* set new window position */
			*winx = newx;
			*winy = newy;
			*winw = neww;
			*winh = newh;

			windowPlaced = True;
			break;
		
		case MotionNotify:
			/* erase old box */
			if (!firsttime)
				RESIZEBOX();
			else
				firsttime = 0;
			
			/*
			 * there may be multiple motion events queued up,
			 * consume them and just use the last one.
			 */
			while (XCheckTypedEvent(dpy, MotionNotify, &event))
				;
			XQueryPointer(dpy, RootWindow(dpy, DefScreen),
				      &mRoot, &mChild,
				      &mouseX, &mouseY,
				      &relX, &relY, &curMask);

			/* set new position */
			mouseX = event.xmotion.x_root;
			mouseY = event.xmotion.y_root;

			/* Window sizing using the window's XSizeHints
			 * is done so the following formula is met:
			 *
			 *	width = min_width + (i * width_inc)
			 *	height= min_height+ (j * height_inc)
			 *
			 * Where i and j are the number of size increments
			 * the window should be.  For example, an xterm
			 * might have the following values:
			 *
			 *	j = 24
			 *	min_height = 33
			 *	height_inc = 13 (for a font 13 pixels high)
			 */
			switch (anchor)
			{
				case AnchorUL:	/* mouse defines LR corner */
					/* make sure we don't go negative */
					if (mouseX < anchorX + minw)	
						mouseX = anchorX + minw;

					/* make sure we don't go negative */
					if (mouseY < anchorY + minh)	
						mouseY = anchorY + minh;

					neww = (unsigned)(mouseX - *winx);
					newh = (unsigned)(mouseY - *winy);


					/* align width to widthInc boundary */
					neww -= (neww - minw) % widthInc;

					/* align height to widthInc boundary */
					newh -= (newh - minh) % heightInc;
					break;

				case AnchorUR:	/* mouse defines LL corner */
					/* make sure we don't go negative */
					if (mouseX > anchorX - minw)	
						mouseX = anchorX - minw;

					/* make sure we don't go negative */
					if (mouseY < anchorY + minh)	
						mouseY = anchorY + minh;
					
					neww = (unsigned)(anchorX - mouseX);
					newh = (unsigned)(mouseY - *winy);

					/* adjust to widthInc boundary */
					neww -= (neww - minw) % widthInc;

					/* align height to widthInc boundary */
					newh -= (newh - minh) % heightInc;

					/* calculate new origin */
					newx = (int)(anchorX - neww);
					break;

				case AnchorLL:	/* mouse defines UR corner */
					/* make sure we don't go negative */
					if (mouseX < anchorX + minw)	
						mouseX = anchorX + minw;

					/* make sure we don't go negative */
					if (mouseY > anchorY - minh)	
						mouseY = anchorY - minh;

				      	neww = (unsigned)(mouseX - *winx);
				      	newh = (unsigned)(anchorY - mouseY);

                                     	/* adjust to widthInc boundary */
				     	neww -= (neww - minw) % widthInc;

		     			/* align height to widthInc boundary */
				     	newh -= (newh - minh) % heightInc;

					/* calculate new origin */
					newy = anchorY - newh;
					break;

				case AnchorLR:	/* mouse defines UL corner */
					/* make sure we don't go negative */
					if (mouseX > anchorX - minw)	
						mouseX = anchorX - minw;

					/* make sure we don't go negative */
					if (mouseY > anchorY - minh)	
						mouseY = anchorY - minh;

				      	neww = (unsigned)(anchorX - mouseX);
				      	newh = (unsigned)(anchorY - mouseY);

                                     	/* adjust to widthInc boundary */
				     	neww -= (neww - minw) % widthInc;

		     			/* align height to widthInc boundary */
				     	newh -= (newh - minh) % heightInc;

					/* calculate new origin */
					newx = (int)(anchorX - neww);
					newy = (int)(anchorY - newh);
					break;
			}

			/* ???? not doing anything with aspect ratios */

			/* make sure width does not exceed limits */
			if (neww < minw)
				neww = (unsigned)minw;
			if (neww > maxw)
				neww = (unsigned)maxw;

			/* make sure height does not exceed limits */
			if (newh < minh)
				newh = (unsigned)minh;
			if (newh > maxh)
				newh = (unsigned)maxh;

			/* draw new box */
			RESIZEBOX();
			break;
		
		default:
			if (PrintOrphans)
				DebugEvent(&event, "UserResizeWin");
			break;
		}
	}

	/*
	 * erase outline
	 */
	if (boxDrawn == True)
		RESIZEBOX();
	XFlush(dpy);

	/*
	 * let go of the server
	 */
	XUngrabServer(dpy);
	XUngrabPointer(dpy, CurrentTime);
	XFlush(dpy);

	/*
	 * do I need to change the cursor back?
	 */

}

/*
 * drawBoxes -- draw the XOR boxes signifying the positions of the windows being
 * 	moved.
 *
 * If oneWinInfo is not NULL that means we should only draw a box around the
 * window pointed to by oneWinInfo, and not the whole selection list.
 */
drawBoxes(dpy, offX, offY, oneWinInfo)
Display	*dpy;
int	offX, offY;
WinInfo	*oneWinInfo;
{
	WinInfo	*winInfo = (WinInfo *)0;
	WinInfo	*iconWinInfo;

	if (oneWinInfo != (WinInfo *)0)
	{
		XDrawRectangle(dpy, RootWindow(dpy, DefScreen),
			       RootGC, 
    			       oneWinInfo->x + offX, 
    			       oneWinInfo->y + offY, 
    			       oneWinInfo->width - 1, 
    			       oneWinInfo->height - 1);
		XDrawRectangle(dpy, RootWindow(dpy, DefScreen),
			       RootGC, 
    			       oneWinInfo->x + offX + 1, 
    			       oneWinInfo->y + offY + 1, 
    			       oneWinInfo->width - 3, 
    			       oneWinInfo->height - 3);
	}  /* End if only drawing box around one window. */
	else
	{
		while(winInfo = EnumSelections(winInfo))
		{
			WinInfo *w;
			if (winInfo->winState == Iconic)
			    w = GetIcon(winInfo);
			else
			    w = winInfo;

			XDrawRectangle(dpy, RootWindow(dpy, DefScreen),
				       RootGC, 
				       w->x + offX, 
				       w->y + offY, 
				       w->width - 1,
				       w->height - 1);
			XDrawRectangle(dpy, RootWindow(dpy, DefScreen),
				       RootGC, 
				       w->x + offX + 1, 
				       w->y + offY + 1, 
				       w->width - 3,
				       w->height - 3);
		}  /* End while still more in select list. */
	}  /* End else */

	XFlush(dpy);
}

/*
 * TraceBoundingBox -- trace an XOR box with the initial point specified
 *	by pEvent.  Return the dimensions of the box in boxX, boxY, boxW,
 *	and boxH.
 */
Time
TraceBoundingBox(dpy, pEvent, pboxX, pboxY, pboxW, pboxH)
Display	*dpy;
XEvent	*pEvent;
short	*pboxX, *pboxY, *pboxW, *pboxH;
{
	XEvent	event;
	Bool	boxFinished;
	XPoint	points[5];
	static  void DrawDouble();

	/*
	 * Change the cursor.
	 */
	XGrabPointer(dpy, RootWindow(dpy, DefScreen),
		     True, 
		     (ButtonReleaseMask | ButtonPressMask | PointerMotionMask),
		     GrabModeAsync, GrabModeAsync,
		     None,
		     RootWinCursor,
		     CurrentTime);

	/*
	 * Grab the server, so that the outline(s) don't
	 * corrupt or get corrupted.
	 */
	XGrabServer(dpy);

	/* Due to a bug (in the X11R3 server?) we have to set the
	 * lastButton, used in rootEvent of RootWin.c, to a
	 * button that is not checked for.  There are problems
	 * with MotionNotify events getting through to the rootEvent
	 * loop when they shouldn't.  They cause TraceBoundingBox
	 * to be called again, without a button down.
	 */
	lastButton = Button5;

	/* these values are fixed */
	points[0].x = pEvent->xbutton.x_root;
	points[0].y = pEvent->xbutton.y_root;
	points[1].x = pEvent->xbutton.x_root;
	points[3].y = pEvent->xbutton.y_root;
	points[4].x = pEvent->xbutton.x_root;
	points[4].y = pEvent->xbutton.y_root;

	/* these are the values that vary */
	points[1].y = pEvent->xbutton.y_root;
	points[2].x = pEvent->xbutton.x_root;
	points[2].y = pEvent->xbutton.y_root;
	points[3].x = pEvent->xbutton.x_root;

	/*
	 * Draw initial outline
	 */
	DrawDouble(dpy,points);

	for (boxFinished = False; boxFinished != True; )
	{
		XMaskEvent(dpy, ButtonReleaseMask|PointerMotionMask, &event);
		switch (event.type)
		{
		case ButtonRelease:
			boxFinished = True;
			break;
		
		case MotionNotify:
			/* erase old box */
			DrawDouble(dpy,points);

			/*
			 * there may be multiple motion events queued up,
			 * consume them and just use the last one.
			 */
			while (XCheckTypedEvent(dpy, MotionNotify, &event))
				;

			/* set new position */
			points[1].y = event.xmotion.y_root;
			points[2].x = event.xmotion.x_root;
			points[2].y = event.xmotion.y_root;
			points[3].x = event.xmotion.x_root;

			/* draw new box */
			DrawDouble(dpy,points);
			break;
		
		default:
			if (PrintOrphans)
				DebugEvent(&event, "TraceBounding");
			break;
		}
	}

	/*
	 * erase outline
	 */
	DrawDouble(dpy,points);

	/*
	 * let go of the server
	 */
	XUngrabServer(dpy);
	XUngrabPointer(dpy, CurrentTime);

	if (points[0].x < points[2].x)
	{
		*pboxX = points[0].x;
		*pboxW = points[2].x - points[0].x;
	}
	else
	{
		*pboxX = points[2].x;
		*pboxW = points[0].x - points[2].x;
	}

	if (points[0].y < points[2].y)
	{
		*pboxY = points[0].y;
		*pboxH = points[2].y - points[0].y;
	}
	else
	{
		*pboxY = points[2].y;
		*pboxH = points[0].y - points[2].y;
	}
	XFlush(dpy);
	return event.xbutton.time;
}


/*
 * Draw a double-width rectangle by drawing the original rectangle, and then
 * by drawing it again inset by one point.  We could simply set the linewidth
 * to be greater than 1 in RootGC, but this goes through slow wide-line drawing
 * code on most servers.  HACK: we know the outer[] array has exactly 5 points.
 */
static void
DrawDouble( dpy, outer )
    Display *dpy;
    XPoint outer[];
{
    XPoint inner[5];
    XPoint delta;

    XDrawLines(dpy, DefaultRootWindow(dpy),
	RootGC, outer, 5, CoordModeOrigin);

    delta.x = (outer[0].x < outer[2].x) ? 1 : -1;
    delta.y = (outer[0].y < outer[2].y) ? 1 : -1;

    inner[0].x = outer[0].x + delta.x;
    inner[0].y = outer[0].y + delta.y;

    inner[1].x = outer[1].x + delta.x;
    inner[1].y = outer[1].y - delta.y;

    inner[2].x = outer[2].x - delta.x;
    inner[2].y = outer[2].y - delta.y;

    inner[3].x = outer[3].x - delta.x;
    inner[3].y = outer[3].y + delta.y;

    inner[4] = inner[0];

    XDrawLines(dpy, DefaultRootWindow(dpy),
	RootGC, inner, 5, CoordModeOrigin);
}
