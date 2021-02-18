/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) Icon.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "menu.h"
#include "olwm.h"

/* externals */
extern Menu     IconFullMenu;
extern Menu     FrameFullMenu;

extern void	*IntMalloc();
extern void	SetWMState();
extern Bool	GetWMState();

/* events in the icon window that are interesting */
#define ICON_EVENT_MASK        (ButtonPressMask | ButtonReleaseMask | \
				 ExposureMask | Button1MotionMask)

/* This event mask is for clients who handle their own icons. */
#define ICON_EVENT_MASK_2        (ButtonPressMask | ButtonReleaseMask | \
				 /*ExposureMask |*/ Button1MotionMask)

/* size of icon window */
#define ICON_WIN_WIDTH	60
#define ICON_WIN_HEIGHT	60
#define ICON_GAP	10

extern XFontStruct	*IconFontInfo;
extern Pixmap	pixIcon;
extern GC	DrawWhiteGC;
extern GC	DrawBlackGC;
extern GC	DrawReverseGC;
extern GC	DrawNormalGC;
extern GC	IconNormalGC;
extern GC	RootGC;
extern IconPreference IconPlacement;

static IconSlot	*iconSlotsRoot = (IconSlot *)0;
static int	iconMaxX, iconMaxY;	/* current icon placement boundries */
static int	iconXincr, iconYincr;	/* icon position deltas */

int		DoubleClickTime = 500;

/* 
 * MakeIcon	-- create (an unmapped) icon window
 */
/*ARGSUSED*/	/* width arg will be used later */
WinInfo *
MakeIcon(dpy, frame, width)
Display		*dpy;
WinInfo		*frame;
unsigned int	width;
{
	WinInfo			*winInfo;
	Window			newWin;
	unsigned long		valueMask = 
				   CWEventMask | CWBorderPixel | CWBackPixel;
	XSetWindowAttributes	attributes;
	int			DrawIcon(), EventIcon();
	int			SelectIcon(), ConfigIcon();
	int			DestroyIcon();
	int			PropIcon();
	int			ix, iy;
	IconSlot		*iconSlot, *getIconSlot();
	IconInfo		*iconInfo;
	XWMHints		*wmHints;
	Window			winRoot;
	int			winX, winY;
	unsigned int		winWidth, winHeight;
	unsigned int		borderWidth, depthReturn;
	XWindowChanges		wc;
	unsigned int		mask;

	wmHints = GetWMHints(frame);

	if (wmHints && (wmHints->flags & IconPositionHint)) {
	    ix = wmHints->icon_x;
	    iy = wmHints->icon_y;
	    iconSlot = (IconSlot *) 0;
	} else {
	    iconSlot = getIconSlot(dpy, &ix, &iy);
	}

	/* if client has an icon window, we only place it in the proper
	 * location, record it in our database, and make sure that we get
	 * the button press events on it.
	 */
	if (wmHints && (wmHints->flags & IconWindowHint))
	{
		newWin = wmHints->icon_window;

		/* This is a temporary fix. When the code for proper
		 * handling of icon selection happens, the icon will
		 * be reparented to allow a border to be drawn.
		 * For now, just reparent the icon to the root window.
		 * This fixes a couple of bugs related to XView clients
		 * that have parented icon windows.
		 */
		XReparentWindow(dpy,  newWin,
				RootWindow(dpy, DefaultScreen(dpy)),
				ix, iy);

		/* get the current width and height of the icon window */
		XGetGeometry(dpy, newWin,
			     &winRoot,
			     &winX, &winY, 
			     &winWidth, &winHeight,
			     &borderWidth,
			     &depthReturn);
	}
	else
	{
		/* make a window for the icon */
		attributes.event_mask = ICON_EVENT_MASK;
		attributes.border_pixel = borderColor;
		attributes.background_pixel=WhitePixel(dpy, DefaultScreen(dpy));

		winWidth = ICON_WIN_WIDTH;
		winHeight = ICON_WIN_HEIGHT;
		winX = ix;
		winY = iy;
		newWin = XCreateWindow(dpy, RootWindow(dpy, DefaultScreen(dpy)),
				 ix, iy,
				 ICON_WIN_WIDTH, ICON_WIN_HEIGHT,
				 1,
				 DefaultDepth(dpy, DefaultScreen(dpy)),
				 InputOutput,
				 DefaultVisual(dpy, DefaultScreen(dpy)),
				 valueMask,
				 &attributes);
	}

	/* register the window in our database */
	winInfo = WICreateInfo(newWin);
	winInfo->x = ix;
	winInfo->y = iy;
	winInfo->width = winWidth;
	winInfo->height = winHeight;
	winInfo->winState = Iconic;

	winInfo->frame = frame;

	/* set up the standard functions */
	winInfo->drawfunc = DrawIcon;
	winInfo->eventfunc = EventIcon;
	winInfo->propfunc = PropIcon;
	winInfo->destroyfunc = DestroyIcon;
	winInfo->configfunc = ConfigIcon;
	winInfo->selectfunc = SelectIcon;
	winInfo->self = newWin;

	iconInfo = (IconInfo *)IntMalloc((unsigned)sizeof(IconInfo));
	iconInfo->iconslot = iconSlot;
	winInfo->priv.ptr = (void *)iconInfo;

	if (wmHints && (wmHints->flags & IconWindowHint))
	{
		iconInfo->iconWin = newWin;

		/* Move the icon window to the proper place on the desk top */
		mask =  CWX | CWY; 
		winInfo->x = wc.x = ix;
		winInfo->y = wc.y = iy;
		XConfigureWindow(dpy, iconInfo->iconWin, mask, &wc);

		/* we need those Button Presses */
		XSelectInput(dpy, iconInfo->iconWin, ICON_EVENT_MASK_2);
	}
	else if (wmHints && (wmHints->flags & IconPixmapHint))
	{
		SetIconName(dpy, winInfo);

		/* find the icon size */
		XGetGeometry(dpy, wmHints->icon_pixmap,
			     &winRoot,
			     &winX, &winY, 
			     &iconInfo->iconWidth, &iconInfo->iconHeight,
			     &borderWidth,
			     &depthReturn);

		/* create a pixmap to hold the data */
		iconInfo->iconPix = XCreatePixmap( dpy, 
				RootWindow(dpy, DefaultScreen(dpy)), 
				iconInfo->iconWidth, iconInfo->iconHeight,
				DefaultDepth(dpy, DefaultScreen(dpy)) );


		/* copy the bitmap into place */
		XCopyPlane( dpy, wmHints->icon_pixmap, iconInfo->iconPix, 
			DrawBlackGC, 0, 0, 
			iconInfo->iconWidth, iconInfo->iconHeight, 
			0, 0, (unsigned long)1 );
		iconInfo->iconX = (ICON_WIN_WIDTH - iconInfo->iconWidth) / 2;
		iconInfo->iconY = ((ICON_WIN_HEIGHT - (ICON_WIN_HEIGHT/3) 
					- iconInfo->iconHeight)/2);
		
		/* make sure winInfo->self get used */
		iconInfo->iconWin = (Window)0;
	}
	else
	{
		SetIconName(dpy, winInfo);

		iconInfo->iconPix = pixIcon;

		XGetGeometry(dpy, iconInfo->iconPix,
			     &winRoot,
			     &winX, &winY, 
			     &iconInfo->iconWidth, &iconInfo->iconHeight,
			     &borderWidth,
			     &depthReturn);
		iconInfo->iconX = (ICON_WIN_WIDTH - iconInfo->iconWidth) / 2;
		iconInfo->iconY = ((ICON_WIN_HEIGHT - (ICON_WIN_HEIGHT/3) 
					- iconInfo->iconHeight)/2);
	}

	return winInfo;
}


/*
 * SetIconName -- get the icon name.
 *	This routine is called both when window is first mapped and
 *	when the icon name changes.
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
SetIconName(dpy, winInfo)
Display		*dpy;
WinInfo 	*winInfo;
{
	int		fontHeight;
	IconInfo	*iconInfo = (IconInfo *)(winInfo->priv.ptr);

	fontHeight = (IconFontInfo->max_bounds.ascent 
			+ IconFontInfo->max_bounds.descent);

	iconInfo->name = GetIconName(winInfo->frame);
	iconInfo->nameLength = strlen(iconInfo->name);
	iconInfo->nameWidth = XTextWidth(IconFontInfo, iconInfo->name, 
				iconInfo->nameLength);

	iconInfo->nameX = (ICON_WIN_WIDTH - iconInfo->nameWidth) / 2;
	iconInfo->nameY = 2*ICON_WIN_HEIGHT/3
		+ (ICON_WIN_HEIGHT/3)/2 + fontHeight/2;
}



/*
 * Iconify	-- iconify the frame associated with the given winInfo
 *	winInfo	- pointer to WinInfo associated with shine mark
 */
Iconify(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	WinInfo		*iconWinInfo = GetIcon(winInfo->frame);
	IconInfo	*iconInfo = (IconInfo *)(iconWinInfo->priv.ptr);
	WinState	state;
	FollowBlock	*followBlock;
	int		ii;

	/* if already iconfied, dont do anything silly */
	if (winInfo->frame->winState == Iconic)
	{
		return;
	}

	/* if we have a leader we cannot iconify */
	if (GetLeader(winInfo->frame))
	{
		return;
	}

	/* map any icon windows */
	if (iconInfo->iconWin)
		XMapRaised(dpy, iconInfo->iconWin);
	else
		XMapRaised(dpy, iconWinInfo->self);

	if (winInfo->frame->winState == Normal)
	{
		DrawIconToWindowLines(dpy, iconWinInfo, winInfo->frame);
	}

	/* do we have any follower windows?  If so we need to unmap all those
	 * that are currently mapped by the client
	 */
	for(followBlock = GetFollowers(winInfo->frame);
	    followBlock;
	    followBlock = followBlock->next)
	{
		for(ii=0; ii < FOLLOWNUM; ii++)
		{
			if (followBlock->followers[ii])
			{
				state = followBlock->followers[ii]->winState;
				if (state == Normal)
				{
					DrawIconToWindowLines(dpy, 
						iconWinInfo, 
						followBlock->followers[ii]);
					followBlock->followers[ii]->winState 
						= Iconic;
					++followBlock->followers[ii]->
					    	pendingUnmaps;
					RemoveSelection(dpy, 
						followBlock->followers[ii]);
					XUnmapWindow(dpy, 
					     followBlock->followers[ii]->self);
					XUnmapWindow(dpy, 
					  GetPane(followBlock->followers[ii]));
					SetWMState(dpy,
					  followBlock->followers[ii]);
				}
			}
		}
	}

	XUnmapWindow(dpy, winInfo->frame->self);
	XUnmapWindow(dpy, GetPane(winInfo->frame));
	winInfo->frame->winState = Iconic;
	++winInfo->frame->pendingUnmaps;
	SetWMState(dpy, winInfo->frame);
}

/*
 * DeIconify	-- deiconify the frame associated with the given winInfo
 *	winInfo	- pointer to WinInfo associated with icon
 */
DeIconify(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	WinInfo		*iconWinInfo = GetIcon(winInfo->frame);
	IconInfo	*iconInfo = (IconInfo *)(iconWinInfo->priv.ptr);
	FollowBlock	*followBlock;
	int		ii;
	WinState	state;
	Bool		selectWindows = False;


	/* if already open, dont do anything silly */
	if (winInfo->frame->winState == Normal)
		return;

	DrawIconToWindowLines(dpy, iconWinInfo, winInfo->frame);

	XMapRaised(dpy, winInfo->frame->self);
	XMapRaised(dpy, GetPane(winInfo->frame));

	/* do we have any follower windows?  If so we need to map all those
	 * that we unmapped when we iconified
	 */
	for(followBlock = GetFollowers(winInfo->frame);
	    followBlock;
	    followBlock = followBlock->next)
	{
		for(ii=0; ii < FOLLOWNUM; ii++)
		{
			if (followBlock->followers[ii])
			{
				state = followBlock->followers[ii]->winState;
				if (state == Iconic)
				{
					DrawIconToWindowLines(dpy, 
						iconWinInfo, 
						followBlock->followers[ii]);
					followBlock->followers[ii]->winState 
						= Normal;
					XMapRaised(dpy, 
					     followBlock->followers[ii]->self);
					XMapRaised(dpy, 
					  GetPane(followBlock->followers[ii]));
					SetWMState(dpy,
					  followBlock->followers[ii]);
				}
			}
		}
	}

	/* un-map any icon windows */
	if (iconInfo->iconWin)
		XUnmapWindow(dpy, iconInfo->iconWin);
	else
		XUnmapWindow(dpy, iconWinInfo->self);
	winInfo->frame->winState = Normal;
	SetWMState(dpy, winInfo->frame);
}

/*
 * DrawIcon -- draw the icon
 */
static
DrawIcon(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	IconInfo	*iconInfo = (IconInfo *)(winInfo->priv.ptr);

	/* icon windows receive their own exposure events */
	if (!iconInfo->iconWin)	
	{
		/* copy in icon pixmap */
		XCopyArea(dpy, iconInfo->iconPix, winInfo->self, DrawWhiteGC,
			  0, 0,
			  iconInfo->iconWidth, iconInfo->iconHeight,
			  iconInfo->iconX, iconInfo->iconY);

		/* put in icon name */
		XDrawString(dpy, winInfo->self, IconNormalGC,
			    iconInfo->nameX, iconInfo->nameY,
			    iconInfo->name, iconInfo->nameLength);
	}
}

/*
 * PropIcon -- a property has changed on the pane.  We need to see if the
 *	hints have changed.
 */
static
PropIcon(dpy, winInfo, pEvent)
Display	*dpy;
WinInfo	*winInfo;
XEvent	*pEvent;
{
	if (pEvent->xproperty.atom == XA_WM_ICON_NAME)
		SetIconName(dpy, winInfo);
}

/*
 * EventIcon -- we got an event on the icon window.  This coule be either
 *	a client's icon window, or one that we created.  The client icon
 *	window should only get ButtonPress events 
 */
static
EventIcon(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinInfo	*winInfo;
{
	WinInfo			*selInfo;
	int			offX, offY;
	static	XButtonEvent	lastSelect;
	static Bool		hadSelect = False;	/* state - is 
							 * MB_SELECT down? */
	static int		buttonPressX, buttonPressY;	/* position 
								 * of button
								 * press */

#define	bevent	(event->xbutton)

	switch (event->type)
	{
	case MappingNotify:
		/* we ignore these notifications */
		break;

	case Expose:
		DrawIcon(dpy, winInfo);
		break;

	case ButtonRelease:
		switch (MouseButton(dpy, event))
		{
		case MB_SELECT:
			hadSelect = False;

			/* if we got this then we had a click (versus a drag)
			 * since UserMoveWin eats the ButtonRelease at the end
			 * of a drag 
			 */
			ClearSelections(dpy);
			AddSelection(dpy, winInfo->frame, event->xbutton.time);

			/* raise the icon window */
			XRaiseWindow(dpy, winInfo->self);
			break;
		}
		break;

	case MotionNotify:
		/* we get this only after a button press */
		if (hadSelect == False) /* watch for erroneous motions */
                {
                        break;
                }

                /* See if we have moved more than the threshold amount. */
                if ((ABS(event->xmotion.x - buttonPressX) < MOVETHRESH) &&
                    (ABS(event->xmotion.y - buttonPressY) < MOVETHRESH))
                        break;


		/* If we are not on the selection list move ourselves,
		 * else we are on the selection list so move all the windows.
		 */
		if (!IsSelected(winInfo->frame))
		{
			UserMoveWin(dpy, &lastSelect, &offX, &offY, winInfo);

			/* if frame is iconified, we move icon */
#ifdef NOT_USED
			if (winInfo->winState == Iconic)
				winInfo = GetIcon(winInfo);
#endif /* NOT_USED */
			winInfo->x += offX;
			winInfo->y += offY;

			/* call configuration function */
			(*winInfo->configfunc)(dpy, winInfo);
		}
		else
		{
			UserMoveWin(dpy,
				    &lastSelect,
				    &offX, &offY, 
				    (WinInfo *)0);

			/* move all the windows that were moved */
			selInfo = (WinInfo *)0;
			while(selInfo = EnumSelections(selInfo))
			{
				/* if frame is iconified, we move icon */
				if (selInfo->winState == Iconic) {
				    selInfo = GetIcon(selInfo);
				} else {
				    ((FrameInfo *)(selInfo->priv.ptr))->
					configMask = CWX | CWY;
				}
	
				selInfo->x += offX;
				selInfo->y += offY;
	
				/* call configuration function */
				(*selInfo->configfunc)(dpy, selInfo);
	
			}
		}  /* End else */
		break;

	case ButtonPress:
		switch (MouseButton(dpy, event))
		{
		case MB_MENU:
			MenuShow(dpy, winInfo, &IconFullMenu, 
				bevent.x_root, bevent.y_root);
			break;

		case MB_SELECT:
			buttonPressX = event->xbutton.x;
			buttonPressY = event->xbutton.y;
			hadSelect = True;

			/* check last click time to see if we is in double
			 * click land.
			 */
			if (bevent.time - lastSelect.time <= DoubleClickTime)
			{
				/* we have a double click */
				DeIconify(dpy, winInfo);
			}
			lastSelect = event->xbutton;
			break;

		case MB_ADJUST:
			ToggleSelection(dpy, winInfo->frame,
					event->xbutton.time);
			break;
		}
		break;

	default:
		if (PrintOrphans)
			DebugEvent(event, "EventIcon");
		break;
	}
}

InitIcon(dpy)
Display	*dpy;
{
	switch (IconPlacement)
	{
	case AlongTop:
		iconMaxX = 0;
		iconMaxY = 0;
		iconXincr = ICON_WIN_WIDTH;
		iconYincr = ICON_WIN_HEIGHT;
		break;

	case AlongBottom:
		iconMaxX = 0;
		iconMaxY = DisplayHeight(dpy, DefaultScreen(dpy)) - 
			   ICON_WIN_HEIGHT;
		iconXincr = ICON_WIN_WIDTH;
		iconYincr = -ICON_WIN_HEIGHT;
		break;

	case AlongLeft:
		iconMaxX = 0;
		iconMaxY = 0;
		iconXincr = ICON_WIN_WIDTH;
		iconYincr = ICON_WIN_HEIGHT;
		break;

	case AlongRight:
		iconMaxX = DisplayWidth(dpy, DefaultScreen(dpy)) -
			   ICON_WIN_HEIGHT;
		iconMaxY = 0;
		iconXincr = -ICON_WIN_WIDTH;
		iconYincr = ICON_WIN_HEIGHT;
		break;
	}
}

static IconSlot *
getIconSlot(dpy, newx, newy)
Display	*dpy;
int	*newx, *newy;
{
	IconSlot	*is;

	/* the strategy for locating an appropriate icon
	 * position is this:
	 * 1 - search the list for unoccupied slots.
	 * then, if no unoccupied slots are found:
	 * 2 - allocate a new icon slot either at the end of a
	 *     row, or if that would fall off of the screen, start
	 *     a new row.
	 */

	/* search for an unoccupied slot */
	for (is = iconSlotsRoot; 
	     (is != (IconSlot *)0) && (is->next != (IconSlot *)0);
	     is = is->next)
	{
		if (is->occupied == False)
		{
			is->occupied = True;
			is->x = *newx = is->orgx;
			is->y = *newy = is->orgy;
			return is;
		}
	}

	/* allocate a new slot */
	if (is == (IconSlot *)0)	/* first slot */
	{
		is = (IconSlot *)IntMalloc((unsigned)sizeof(IconSlot));
		iconSlotsRoot = is;
	}
	else	/* add a slot at the end */
	{
		is->next = (IconSlot *)IntMalloc((unsigned)sizeof(IconSlot));
		is = is->next;
	}

	/* null terminate the list */
	is->next = (IconSlot *)0;

	/* find an appropriate place for the new icon */
	switch (IconPlacement)
	{
	case AlongTop:
		is->x = is->orgx = iconMaxX;
		is->y = is->orgy = iconMaxY;

		/* update iconMaxX, iconMaxY */
		if (iconMaxX + iconXincr + ICON_WIN_WIDTH >
		    DisplayWidth(dpy, DefaultScreen(dpy)))
		{
			/* next icon will fall off of screen */
			iconMaxX = 0;
			iconMaxY += iconYincr;	/* check this later ??? */
		}
		else
			iconMaxX += iconXincr;
		break;

	case AlongBottom:
		is->x = is->orgx = iconMaxX;
		is->y = is->orgy = iconMaxY;

		/* update iconMaxX, iconMaxY */
		if (iconMaxX + iconXincr + ICON_WIN_WIDTH >
		    DisplayWidth(dpy, DefaultScreen(dpy)))
		{
			/* next icon will fall off of screen */
			iconMaxX = 0;
			iconMaxY += iconYincr;	/* check this later ??? */
		}
		else
			iconMaxX += iconXincr;
		break;

	case AlongLeft:
		is->x = is->orgx = iconMaxX;
		is->y = is->orgy = iconMaxY;

		/* update iconMaxX, iconMaxY */
		if (iconMaxY + iconYincr + ICON_WIN_HEIGHT >
		    DisplayHeight(dpy, DefaultScreen(dpy)))
		{
			/* next icon will fall off of screen */
			iconMaxY = 0;
			iconMaxX += iconXincr;	/* check this later ??? */
		}
		else
			iconMaxY += iconYincr;
		break;

	case AlongRight:
		is->x = is->orgx = iconMaxX;
		is->y = is->orgy = iconMaxY;

		/* update iconMaxX, iconMaxY */
		if (iconMaxY + iconYincr + ICON_WIN_HEIGHT >
		    DisplayHeight(dpy, DefaultScreen(dpy)))
		{
			/* next icon will fall off of screen */
			iconMaxY = 0;
			iconMaxX += iconXincr;	/* check this later ??? */
		}
		else
			iconMaxY += iconYincr;
		break;
	}

	*newx = is->x;
	*newy = is->y;

	is->occupied = True;
	return is;
}

/*
 * DestroyIcon -- destroy the icon window as well as free'ing up
 *	any extra data.
 */
static
DestroyIcon(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	IconInfo *iconInfo = (IconInfo *)(GetIcon(winInfo->frame)->priv.ptr);

	WIDeleteInfo(winInfo->self);

	/* free up screen position */
	if ( iconInfo->iconslot )
	    iconInfo->iconslot->occupied = False;

	if (!iconInfo->iconWin)
	{
		/* lint will complain about this cast */
		free((char *)(winInfo->priv.ptr));
		XDestroyWindow(dpy, winInfo->self);
	}
}

/*
 * DrawIconToWindowLines -- draw the open (close) lines when a window is
 *	becoming an icon or vice-versa
 */
DrawIconToWindowLines(dpy, iconInfo, winInfo)
Display	*dpy;
WinInfo	*winInfo, *iconInfo;
{
	int	ii;

	XGrabServer(dpy);

	for(ii=0; ii < 3 ; ii++)
	{
		/* draw the close lines */
		XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC, 
				iconInfo->x, 
				iconInfo->y,
				winInfo->x, 
				winInfo->y);
		XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC, 
				iconInfo->x, 
				(int)(iconInfo->y + iconInfo->height),
				winInfo->x, 
				(int)(winInfo->y + winInfo->height));
		XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC, 
				(int)(iconInfo->x + iconInfo->width), 
				iconInfo->y,
				(int)(winInfo->x + winInfo->width), 
				winInfo->y);
		XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC, 
				(int)(iconInfo->x + iconInfo->width), 
				(int)(iconInfo->y + iconInfo->height),
				(int)(winInfo->x + winInfo->width), 
				(int)(winInfo->y + winInfo->height));

		XFlush(dpy);
		/* erase the close lines */
		XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC, 
				iconInfo->x, 
				iconInfo->y,
				winInfo->x, 
				winInfo->y);
		XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC, 
				iconInfo->x, 
				(int)(iconInfo->y + iconInfo->height),
				winInfo->x, 
				(int)(winInfo->y + winInfo->height));
		XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC, 
				(int)(iconInfo->x + iconInfo->width), 
				iconInfo->y,
				(int)(winInfo->x + winInfo->width), 
				winInfo->y);
		XDrawLine(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootGC, 
				(int)(iconInfo->x + iconInfo->width), 
				(int)(iconInfo->y + iconInfo->height),
				(int)(winInfo->x + winInfo->width), 
				(int)(winInfo->y + winInfo->height));
		XFlush(dpy);
	}

	XUngrabServer(dpy);
}

/*
 * SelectIcon -- based on the value of select, make this icon selected
 *	or not.
 */
static
SelectIcon(dpy, winInfo, select)
Display	*dpy;
WinInfo	*winInfo;
{
	XWindowChanges		values;

	if (select == True)
	{
		values.border_width = 3;
		/*
		winInfo->x--;
		winInfo->y--;
		values.x = winInfo->x;
		values.y = winInfo->y;
		*/
		winInfo->x -= 2;
		winInfo->y -= 2;
		values.x = winInfo->x;
		values.y = winInfo->y;
		XConfigureWindow(dpy, winInfo->self, CWX | CWY | CWBorderWidth, 
					&values);
	}
	else
	{
		values.border_width = 1;
		/*
		winInfo->x++;
		winInfo->y++;
		values.x = winInfo->x;
		values.y = winInfo->y;
		*/
		winInfo->x += 2;
		winInfo->y += 2;
		values.x = winInfo->x;
		values.y = winInfo->y;
		XConfigureWindow(dpy, winInfo->self, CWX | CWY | CWBorderWidth, 
					&values);
	}
} 

/*
 * ConfigIcon -- our window has been reconfigured.  Need to reconfigure
 *	the icon 
 */
static
ConfigIcon(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	XWindowChanges	wc;
	unsigned int	mask;

	mask = CWX | CWY | CWWidth | CWHeight; 
	wc.width = winInfo->width;
	wc.height = winInfo->height;
	wc.x = winInfo->x;
	wc.y = winInfo->y;
	XConfigureWindow(dpy, winInfo->self, mask, &wc);
}

/* IconFullAction -- handle actions from the Icon menus */
int
IconFullAction(dpy, winInfo, menu, idx, pinnedMenu)
Display *dpy;
WinInfo *winInfo;
Menu    *menu;
int     idx;
Bool    pinnedMenu;
{
        WinInfo *tempInfo = (WinInfo *)0;

        if (pinnedMenu || IsSelected(winInfo))
                while(tempInfo = EnumSelections(tempInfo))
                        DoIconFullAction(dpy, tempInfo, menu, idx);
        else
        {
                DoIconFullAction(dpy, winInfo, menu, idx);
        }
}


/*
 * DoIconFullAction -- routine called for each window when a menu action is
 *      selected from the full frame menu.
 */
/*ARGSUSED*/	/* menu arg will be used later */
DoIconFullAction(dpy, winInfo, menu, idx)
Display *dpy;
WinInfo *winInfo;
Menu    *menu;
int     idx;
{
        FrameInfo	*frameInfo;
        XEvent		clientEvent;
        Window          w;
        int             width, height;

#define FFA_OPEN                0
#define FFA_FULLSIZE            1
#define FFA_PROPERTIES          2
#define FFA_BACK                3
#define FFA_REFRESH             4
#define FFA_QUIT                5

        frameInfo = (FrameInfo *)(winInfo->frame->priv.ptr);

        switch(idx)

        {
        case FFA_OPEN:
                DeIconify(dpy, winInfo);
                break;

        case FFA_FULLSIZE:
		DeIconify(dpy, winInfo);
                FullSize(dpy, winInfo->frame);
                break;

        case FFA_PROPERTIES:
                /* send a ClientMessage of type WM_SHOW_PROPERTIES */
                clientEvent.xclient.type = ClientMessage;
                clientEvent.xclient.message_type = AtomShowProperties;
                clientEvent.xclient.format = 32;
                clientEvent.xclient.display = dpy;
                clientEvent.xclient.window = GetPane(winInfo->frame);

                /* send the event */
                XSendEvent(dpy, clientEvent.xclient.window, 
			   False, NoEventMask, &clientEvent);
                break;

        case FFA_BACK:
                /* lower the window */
                XLowerWindow(dpy, (GetIcon(winInfo->frame))->self);
                break;

        case FFA_REFRESH:
                /* refresh the window.  We do this by creating a window on top
                 * of the window to refresh and then immediately destroy it
                 */
                GetUnFramedSize(winInfo->frame, &width, &height);

                w = XCreateSimpleWindow(dpy, GetPane(winInfo->frame), 0, 0,
                                   width, height, 0,
                                   (Pixmap) 0, (Pixmap) 0);
                XMapRaised(dpy, w);
                XDestroyWindow(dpy, w);
                XFlush(dpy);
                break;

        case FFA_QUIT:
                /* If the client has WM_DELETE_WINDOW in its
                 * protocols atom we need to send a client message.
                 */
                if (frameInfo->protocols & DELETE_WINDOW)
                {
                        /* send a ClientMessage of type WM_DELETE_WINDOW */
                        /* this will do nothing on non-open look clients */
                        clientEvent.xclient.type = ClientMessage;
                        clientEvent.xclient.message_type = AtomProtocols;
                        clientEvent.xclient.format = 32;
                        clientEvent.xclient.display = dpy;
                        clientEvent.xclient.window = GetPane(winInfo->frame);
                        clientEvent.xclient.data.l[0] = AtomDeleteWindow;

                        /* This should really be the time of the event. */
                        clientEvent.xclient.data.l[1] = CurrentTime;

                        /* send the event */
                        XSendEvent(dpy,
                                   clientEvent.xclient.window,
                                   False,
                                   NoEventMask,
                                   &clientEvent);
                }
                else    /* messy but guaranteed disposal of client */
                        XKillClient(dpy, GetPane(winInfo->frame));
                break;
        }
}


