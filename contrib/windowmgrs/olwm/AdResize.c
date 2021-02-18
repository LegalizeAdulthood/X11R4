/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) AdResize.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>

#include "bitmaps/ulresize.h"
#include "bitmaps/llresize.h"
#include "bitmaps/urresize.h"
#include "bitmaps/lrresize.h"

extern Cursor		RootWinCursor;
extern Graphics_info	*olgx_gismisc;
extern Bool		f3dUsed;

/* the lower resize corners are handled slightly differently from the
 * uppers. The uppers are partialy overlapped by the title window, the
 * lowers on the other hand, are fully visible. This means that the 
 * "background pixmap" way of drawing the resize corners does not work
 * for the lower corners.
 */

#define LOWER_EVENT_MASK       (ButtonPressMask | ExposureMask)
#define UPPER_EVENT_MASK       (ButtonPressMask | ExposureMask)

/*
 * forward function declarations for this module
 */
int SelectResize();


/* 
 * MakeURResize-- create the upper right resize window with the passed 
 *	frame window as parent.  Return WinInfo structure.
 */
WinInfo *
MakeURResize(dpy, frame, width)
Display	*dpy;
WinInfo	*frame;
int	width;
{
	WinInfo			*winInfo;
	Window			newWin;
	unsigned long		valuemask = 
		CWEventMask | CWBorderPixel | CWBackPixel | CWWinGravity;
	XSetWindowAttributes	attributes;
	int			DrawURResize();
	int			EventURResize();
	int			DestroyURResize();

	attributes.event_mask = UPPER_EVENT_MASK;
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	attributes.win_gravity = NorthEastGravity;

	newWin = XCreateWindow(dpy, frame->self,
			 width - urresize_width, 0,
			 urresize_width, urresize_height,
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 CopyFromParent,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 valuemask,
			 &attributes);

	XMapWindow(dpy, newWin);
	winInfo = WICreateInfo(newWin);
	winInfo->frame = frame;
	winInfo->eventfunc = EventURResize;
	winInfo->destroyfunc = DestroyURResize;
	winInfo->selectfunc = SelectResize;
	winInfo->drawfunc = DrawURResize;
	winInfo->width = urresize_width;
	winInfo->height = urresize_height;

	return winInfo;
}


/* 
 * MakeULResize-- create the upper left resize window with the passed 
 *	frame window as parent.  Return WinInfo structure.
 */
WinInfo *
MakeULResize(dpy, frame)
Display	*dpy;
WinInfo	*frame;
{
	WinInfo			*winInfo;
	Window			newWin;
	unsigned long		valuemask = 
		CWEventMask | CWBorderPixel | CWBackPixel | CWWinGravity;
	XSetWindowAttributes	attributes;
	int			DrawULResize(), EventULResize();
	int			DestroyULResize();

	attributes.event_mask = UPPER_EVENT_MASK;
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	attributes.win_gravity = NorthWestGravity;

	newWin = XCreateWindow(dpy, frame->self,
			 0, 0,
			 ulresize_width, ulresize_height,
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 CopyFromParent,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 valuemask,
			 &attributes);

	XMapWindow(dpy, newWin);
	winInfo = WICreateInfo(newWin);
	winInfo->destroyfunc = DestroyULResize;
	winInfo->frame = frame;
	winInfo->eventfunc = EventULResize;
	winInfo->selectfunc = SelectResize;
	winInfo->drawfunc = DrawULResize;
	winInfo->width = ulresize_width;
	winInfo->height = ulresize_height;

	return winInfo;
}


/* 
 * MakeLLResize-- create the lower left resize window with the passed 
 *	frame window as parent.  Return WinInfo structure.
 */
WinInfo *
MakeLLResize(dpy, frame, height)
Display	*dpy;
WinInfo	*frame;
int	height;
{
	WinInfo			*winInfo;
	Window			newWin;
	unsigned long		valuemask = 
		CWEventMask | CWBorderPixel | CWBackPixel | CWWinGravity;
	XSetWindowAttributes	attributes;
	int			DrawLLResize(), EventLLResize();
	int			DestroyLLResize();

	attributes.event_mask = LOWER_EVENT_MASK;
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	attributes.win_gravity = SouthWestGravity;

	newWin = XCreateWindow(dpy, frame->self,
			 0, height - llresize_height,
			 llresize_width, llresize_height,
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 CopyFromParent,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 valuemask,
			 &attributes);
	XMapWindow(dpy, newWin);

	winInfo = WICreateInfo(newWin);
	winInfo->eventfunc = EventLLResize;
	winInfo->selectfunc = SelectResize;
	winInfo->drawfunc = DrawLLResize;
	winInfo->frame = frame;
	winInfo->destroyfunc = DestroyLLResize;
	winInfo->width = llresize_width;
	winInfo->height = llresize_height;

	return winInfo;
}

/* 
 * MakeLRResize-- create the lower right resize window with the passed 
 *	frame window as parent.  Return WinInfo structure.
 */
WinInfo *
MakeLRResize(dpy, frame, width, height)
Display	*dpy;
WinInfo	*frame;
int	width, height;
{
	WinInfo			*winInfo;
	Window			newWin;
	unsigned long		valuemask =
		CWEventMask | CWBorderPixel | CWBackPixel | CWWinGravity;
	XSetWindowAttributes	attributes;
	int			DrawLRResize(), EventLRResize();
	int			DestroyLRResize();

	attributes.event_mask = LOWER_EVENT_MASK;
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	attributes.win_gravity = SouthEastGravity;

	newWin = XCreateWindow(dpy, frame->self,
			 width - lrresize_width, height - lrresize_height,
			 lrresize_width, lrresize_height,
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 CopyFromParent,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 valuemask,
			 &attributes);

	XMapWindow(dpy, newWin);
	winInfo = WICreateInfo(newWin);
	winInfo->eventfunc = EventLRResize;
	winInfo->selectfunc = SelectResize;
	winInfo->drawfunc = DrawLRResize;
	winInfo->frame = frame;
	winInfo->destroyfunc = DestroyLRResize;
	winInfo->width = lrresize_width;
	winInfo->height = lrresize_height;

	return winInfo;
}

/*
 * DrawULResize -- draw the upper left resize corner
 * This is not strictly necessary, but is included for
 * completeness.
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
DrawULResize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	olgx_draw_resize_corner(olgx_gismisc, winInfo->self, 0, 0, 
		OLGX_UPPER_LEFT);
}


/*
 * DrawURResize -- draw the upper right resize corner
 * This is not strictly necessary, but is included for
 * completeness.
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
DrawURResize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	olgx_draw_resize_corner(olgx_gismisc, winInfo->self, 0, 0, 
		OLGX_UPPER_RIGHT);
}


/*
 * DrawLLResize -- draw the lower left resize corner
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
DrawLLResize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	olgx_draw_resize_corner(olgx_gismisc, winInfo->self, 0, 0, 
		OLGX_LOWER_LEFT);
}


/*
 * DrawLRResize -- draw the lower right resize corner
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
DrawLRResize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	olgx_draw_resize_corner(olgx_gismisc, winInfo->self, 0, 0, 
		OLGX_LOWER_RIGHT);
}

/*
 * EventURResize -- we's got an event in the upper right resize box.
 */
static
EventURResize(dpy, event,  winInfo)
Display *dpy;
XEvent  *event;
WinInfo *winInfo;
{
	switch(event->type)
	{
	case Expose:
		DrawURResize(dpy,winInfo);
		break;

	case ButtonPress:
		if (MouseButton(dpy, event) != MB_SELECT)
			return;

		UserResizeWin(dpy, winInfo->frame,
			&winInfo->frame->x, 
			&winInfo->frame->y, 
			&winInfo->frame->width, 
			&winInfo->frame->height, 
			AnchorLL);

		/* put corner back */
		DrawURResize(dpy, winInfo);

		((FrameInfo *)(winInfo->frame->priv.ptr))->configMask =
		    CWX | CWY | CWWidth | CWHeight;
		(*(winInfo->frame)->configfunc)(dpy,winInfo->frame);
		break;
	
	default:
		if (PrintOrphans)
			DebugEvent(event, "URResize");
		break;
	}
}

/*
 * EventLRResize -- we's got an event in the lower right resize box.
 */
static
EventLRResize(dpy, event,  winInfo)
Display *dpy;
XEvent  *event;
WinInfo *winInfo;
{
	switch(event->type)
	{
	case Expose:
		DrawLRResize(dpy, winInfo);
		break;

	case ButtonPress:
		if (MouseButton(dpy, event) != MB_SELECT)
			return;

		UserResizeWin(dpy, winInfo->frame,
				&winInfo->frame->x, 
				&winInfo->frame->y, 
				&winInfo->frame->width, 
				&winInfo->frame->height, 
				AnchorUL);

		/* put corner back */
		DrawLRResize(dpy, winInfo);

		((FrameInfo *)(winInfo->frame->priv.ptr))->configMask =
		    CWX | CWY | CWWidth | CWHeight;
		(*(winInfo->frame)->configfunc)(dpy,winInfo->frame);
		break;

	default:
		if (PrintOrphans)
			DebugEvent(event, "LRResize");
		break;
	}
}


/*
 * EventLLResize -- we's got an event in the lower left resize box.
 */
static
EventLLResize(dpy, event,  winInfo)
Display *dpy;
XEvent  *event;
WinInfo *winInfo;
{
	switch(event->type)
	{
	case Expose:
		DrawLLResize(dpy, winInfo);
		break;

	case ButtonPress:
		if (MouseButton(dpy, event) != MB_SELECT)
			return;

		UserResizeWin(dpy, winInfo->frame,
				&winInfo->frame->x, 
				&winInfo->frame->y, 
				&winInfo->frame->width, 
				&winInfo->frame->height, 
				AnchorUR);

		/* put corner back */
		DrawLLResize(dpy, winInfo);

		((FrameInfo *)(winInfo->frame->priv.ptr))->configMask =
		    CWX | CWY | CWWidth | CWHeight;
		(*(winInfo->frame)->configfunc)(dpy,winInfo->frame);
		break;

	default:
		if (PrintOrphans)
			DebugEvent(event, "LLResize");
		break;
	}
}


/*
 * EventULResize -- we's got an event in the upper left resize box.
 */
static
EventULResize(dpy, event,  winInfo)
Display *dpy;
XEvent  *event;
WinInfo *winInfo;
{
	switch(event->type)
	{
	case Expose:
		DrawULResize(dpy, winInfo);
		break;

	case ButtonPress:
		if (MouseButton(dpy, event) != MB_SELECT)
			return;

		UserResizeWin(dpy, winInfo->frame,
				&winInfo->frame->x, 
				&winInfo->frame->y, 
				&winInfo->frame->width, 
				&winInfo->frame->height, 
				AnchorLR);

		/* put corner back */
		DrawULResize(dpy, winInfo);

		((FrameInfo *)(winInfo->frame->priv.ptr))->configMask =
		    CWX | CWY | CWWidth | CWHeight;
		(*(winInfo->frame)->configfunc)(dpy,winInfo->frame);
		break;

	default:
		if (PrintOrphans)
			DebugEvent(event, "ULResize");
		break;
	}
}

/*
 * DestroyLRResize -- destroy the lower right resize window and free any allocated
 *	data.
 */
static 
DestroyLRResize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	XDestroyWindow(dpy, winInfo->self);
	WIDeleteInfo(winInfo->self);
}


/*
 * DestroyLLResize -- destroy the lower left resize window and free any allocated
 *	data.
 */
static 
DestroyLLResize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	XDestroyWindow(dpy, winInfo->self);
	WIDeleteInfo(winInfo->self);
}


/*
 * DestroyULResize -- destroy the upper left resize window and free any allocated
 *	data.
 */
static 
DestroyULResize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	XDestroyWindow(dpy, winInfo->self);
	WIDeleteInfo(winInfo->self);
}


/*
 * DestroyURResize -- destroy the upper right resize window and free any allocated
 *	data.
 */
static 
DestroyURResize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	XDestroyWindow(dpy, winInfo->self);
	WIDeleteInfo(winInfo->self);
}

/* 
 * SelectResize -- the frame has been selected/deselected; change background
 *	colour to match and repaint resize corner window
 */
static
SelectResize(dpy, winInfo, select)
Display *dpy;
WinInfo *winInfo;
Bool select;
{
	/* In 2d there is no difference between 
	 * selected and unselected resize corners.
	 */
	if (f3dUsed)
	{
		XSetWindowBackground(dpy,
				     winInfo->self,
				     select ? bg2Color : bg1Color);

		/* This makes sure that the exposed area of the 
		 * resize corner is draw in the right color.
		 * If the resize corners were tucked under the corner
		 * of the pane, similar to the way the upper resize
		 * corners are tucked under the titlebar, then
		 * this wouldn't be necessary...
		 */
		XClearWindow(dpy, winInfo->self);

		(*winInfo->drawfunc)(dpy, winInfo);
	}
}
