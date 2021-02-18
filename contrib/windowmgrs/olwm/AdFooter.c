/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) AdFooter.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>

/* events in the adornment window that are interesting */
#define FOOTER_EVENT_MASK        (ExposureMask)

/* attributes change mask */
#define ATTR_MASK	(CWEventMask |CWBorderPixel |CWBackPixel)

extern GC		DrawNormalGC;
extern GC		DrawReverseGC;
extern Bool		f3dUsed;
extern XFontStruct	*NormalFontInfo;

extern Graphics_info *olgx_gismisc;

/*
 * MakeFooter  -- create the footers to a window. Return a WinInfo structure.
 */
WinInfo *
MakeFooter(dpy, frame, x, y, footerWidth, footerHeight)
Display	*dpy;
WinInfo	*frame;
int	x, y, footerHeight;
int	footerWidth;
{
	Window	newwin;
	XSetWindowAttributes attributes;
	WinInfo	*winInfo;
	int	DrawFooter(), EventFooter(), ConfigFooter();
	int	PropFooter();
	int	DestroyFooter();

	attributes.event_mask = FOOTER_EVENT_MASK;
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	newwin = XCreateWindow(dpy, frame->self,
			 x, y,
			 footerWidth, footerHeight,
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 InputOutput,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 ATTR_MASK,
			 &attributes);

	/* map the title bar */
	XMapWindow(dpy, newwin);

	winInfo = WICreateInfo(newwin);
	winInfo->drawfunc = DrawFooter;
	winInfo->eventfunc = EventFooter;
	winInfo->propfunc = PropFooter;
	winInfo->configfunc = ConfigFooter;
	winInfo->frame = frame;
	winInfo->destroyfunc = DestroyFooter;
	winInfo->width = footerWidth;
	winInfo->height = footerHeight;
	winInfo->x = x;
	winInfo->y= y;

	return winInfo;
}


/*
 * DrawFooter -- draw the footer
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
DrawFooter(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	int	twidth;
	char *s;
	char *GetLeftFooter();
	char *GetRightFooter();

#ifdef PREOLGX
	XFillRectangle(dpy, winInfo->self, DrawReverseGC, 0, 0, 
			winInfo->width, winInfo->height);

	/* draw the left footer */
	if (GetLeftFooter(winInfo->frame))
		XDrawString(dpy, winInfo->self, DrawNormalGC,
			    10, 3*winInfo->height/4,
			    GetLeftFooter(winInfo->frame), 
			    strlen(GetLeftFooter(winInfo->frame)));

	/* draw the right footer */
	if (GetRightFooter(winInfo->frame))
	{
		twidth = XTextWidth(NormalFontInfo, 
				    GetRightFooter(winInfo->frame),
				    strlen(GetRightFooter(winInfo->frame)));

		XDrawString(dpy, winInfo->self, DrawNormalGC,
		    winInfo->width - twidth - 10, 3*winInfo->height/4,
		    GetRightFooter(winInfo->frame), 
		    strlen(GetRightFooter(winInfo->frame)));
	}
#endif
	if ( f3dUsed ) {
	    olgx_draw_box(olgx_gismisc, winInfo->self, 0, 0, winInfo->width,
		winInfo->height, OLGX_NORMAL, True);
	} else {
	    XFillRectangle(dpy, winInfo->self, DrawReverseGC, 0, 0, 
			winInfo->width, winInfo->height);
	}

	/* draw the left footer */
	if (s = GetLeftFooter(winInfo->frame))
		olgx_draw_text(olgx_gismisc, winInfo->self, s,
			10, 3*winInfo->height/4, 0, False, OLGX_NORMAL);

	/* draw the right footer */
	if (s = GetRightFooter(winInfo->frame))
	{
		twidth = XTextWidth(NormalFontInfo, s, strlen(s));

		olgx_draw_text(olgx_gismisc, winInfo->self, s,
			winInfo->width-twidth-10, 3*winInfo->height/4,
			0, False, OLGX_NORMAL);
	}
			
			
}

/*
 * PropFooter -- a property has changed on the footer
 */
static
PropFooter(dpy, winInfo, pEvent)
Display	*dpy;
WinInfo	*winInfo;
XEvent	*pEvent;
{
	extern	Atom	AtomLeftFooter, AtomRightFooter;

	if  ((pEvent->xproperty.atom == AtomLeftFooter)||
	     (pEvent->xproperty.atom == AtomRightFooter))
			DrawFooter(dpy, winInfo);
}



/*
 * EventFooter -- handle events for the footer
 */
static
EventFooter(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinInfo	*winInfo;
{
	switch (event->type)
	{
	case Expose:
		DrawFooter(dpy, winInfo);
		break;

	default:
		break;
	}
}

/*
 * ConfigFooter -- the size of our window has changed.  Resize ourselves.
 */
static
ConfigFooter(dpy, winInfo, x, y, footerWidth, footerHeight)
Display	*dpy;
WinInfo	*winInfo;
int x, y, footerWidth, footerHeight;
{
	XWindowChanges	wc;
	unsigned int	mask;

	mask =  CWWidth | CWHeight | CWX | CWY; 
	wc.x = x;
	wc.y = y;
	wc.width = footerWidth;
	wc.height = footerHeight;

	/* we keep track of only the width */
	winInfo->width = footerWidth;

	XConfigureWindow(dpy, winInfo->self, mask, &wc);
}


/*
 * DestroyFooter -- destroy the footer and free any allocated
 *	data.
 */
static 
DestroyFooter(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	/* free our data and throw away window */
	free((char *)(winInfo->priv.ptr));
	XDestroyWindow(dpy, winInfo->self);
	WIDeleteInfo(winInfo->self);
}
