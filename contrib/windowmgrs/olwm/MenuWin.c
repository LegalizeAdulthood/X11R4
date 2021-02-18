/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) MenuWin.c 5.1 89/12/14 Crucible";
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "menu.h"
#include "olwm.h"

/* Externals */
extern int		FlashTime;	/* button flash time in microsec */
extern long		MenuBackColor;

extern void		*IntMalloc();

static int bindex_last;		/* set to last menu button pressed; cleared
				 * when button released or a submenu 
				 * is brought up */

/* MakeMenuWin() creates a pinned menu.
 * This is done in the following general steps:
 *	create a new window for the menu
 *	create a copy of the menu
 *	adjust the copied menu
 *	map and display the menu contents
 */
/*ARGSUSED*/	/* idx, parWinInfo args will be used later */
MakeMenuWin(dpy, parWinInfo, menu, idx)
Display	*dpy;
WinInfo	*parWinInfo;
Menu	*menu;
int	idx;
{
	unsigned long		valuemask;
	Window			win;
	XSetWindowAttributes	attributes;
	WinInfo			*winInfo;
	int			EventMenuWin();
	XWMHints		wmHints;
	NewXSizeHints		sizeHints;
	Atom			atomList[3];
	Window			winRoot;
	unsigned int		borderWidth, depthReturn;
	int			x, y;
	Menu			*originalMenu;

	/* Save a pointer to the menu we are going 
	 * to put in a window.  The only reason we
	 * do this is because we need to set the 
	 * 'currentlyDisplayed flag in the menu
	 * structure. */
	originalMenu = menu;

	/* create window for menu */
	valuemask = CWEventMask | CWBackPixel;
	attributes.event_mask = (ButtonPressMask | ButtonReleaseMask |
				 ExposureMask | PropertyChangeMask);
	attributes.background_pixel = bg1Color;

	/* the window height must be adjusted because the title of the
	 * menu will no longer be in the menu.
	 */
	win = XCreateWindow(dpy, RootWindow(dpy, DefaultScreen(dpy)),
			    menu->x, menu->y,
			    menu->width, menu->height - menu->titleHeight,
			    0,
			    DefaultDepth(dpy, DefaultScreen(dpy)),
			    InputOutput,
			    DefaultVisual(dpy, DefaultScreen(dpy)),
			    valuemask,
			    &attributes);
	
	winInfo = WICreateInfo(win);
	winInfo->self = win;
	winInfo->x = menu->x;
	winInfo->y = menu->y;

	/* save away a copy of the menu for later use */
	winInfo->priv.ptr = (void *)IntMalloc((unsigned)sizeof(Menu));
	*(Menu *)(winInfo->priv.ptr) = *menu;
	menu = (Menu *)(winInfo->priv.ptr);
	menu->originalMenu = originalMenu;

	/* set up functions */
	winInfo->eventfunc = EventMenuWin;

	/* first we set the properties defining what kind of OL window it is */
	atomList[0] = AtomWTOther;
	atomList[1] = AtomMenuLimited;
	atomList[2] = AtomPinIn;
	XChangeProperty(dpy, win, AtomWinAttr, AtomWinAttr, 
			32, PropModeReplace, 
			/* lint will complain about this cast */
			(unsigned char *)atomList, 3);

	/* add a window mark and a push-pin */
	/*atomList[0] = AtomDecorClose;*/
	atomList[0] = AtomDecorPin;
	atomList[1] = AtomDecorHeader;
	XChangeProperty(dpy, win, AtomDecorAdd, XA_ATOM, 
			32, PropModeReplace, 
			/* lint will complain about this cast */
			(unsigned char *)atomList, 2);

	/* now set the size hints */
	sizeHints.flags = USPosition | USSize;
	XChangeProperty(dpy, 
			win, 
			XA_WM_NORMAL_HINTS,
			XA_WM_SIZE_HINTS, 
			32,
			PropModeReplace,
			/* lint will complain about this cast */
			(unsigned char *)&sizeHints,
			sizeof(NewXSizeHints)/sizeof(long));

	/* and the wmHints.... */
	wmHints.flags = InputHint | StateHint;
	wmHints.initial_state = NormalState;
	/* global input */
	wmHints.input = False;

	XChangeProperty(dpy, 
			win, 
			XA_WM_HINTS,
			XA_WM_HINTS, 
			32,
			PropModeReplace,
			/* lint will complain about this cast */
			(unsigned char *)&wmHints,
			sizeof(XWMHints)/sizeof(long));



	XStoreName(dpy, win, menu->title);

	/* we are ready for the window to be adorned */
	winInfo->frame = CheckAndMapWin(dpy, win, True, True);

	/* fix up our copy of the menu structure to reflect our
	 * reparenthood
	 */
	menu->title = NULL;
	menu->x = menu->y = 0;
	menu->window = win;
	menu->titleHeight = menu->titleWidth = 0;
	XGetGeometry(dpy, win,
		     &winRoot,
		     &x, &y, 
		     &winInfo->width, &winInfo->height,
		     &borderWidth,
		     &depthReturn);
	
	/* menu contents will be drawn in response to window mapping */

	/* We could have a timing problem here???  The user could
	 * push the MB_MENU button again before the DrawMenu
	 * routine finishes and we set this flag.  Then the menu
	 * would not have a grayed out pushpin. */
	originalMenu->currentlyDisplayed = True;
}


/*
 * EventMenuWin -- called when there is an event within a pinned menu
 *	window
 */
EventMenuWin(dpy, event, winInfo)
Display *dpy;
XEvent  *event;
WinInfo *winInfo;
{
	Menu	*menu = (Menu *)(winInfo->priv.ptr);
	int	bindex;
	Menu	*originalMenu = menu->originalMenu; /* menu this is derived from */
	Atom 	actType;
	int	actFormat;
	unsigned	long	nItems, remain;
	long	*pinState;
	int	ex, ey;		/* adjusted event location */
	int 	findButton(), pressButton();
	void	afterProcessing();
	Button	*button;

	switch (event->type)
	{
	case ButtonPress:
		ex = event->xbutton.x;
		ey = event->xbutton.y;
		bindex = findButton(menu, &ex, &ey, winInfo);
		bindex_last = bindex;
		if (bindex == -1)
			break;
		pressButton(dpy, menu, winInfo, 
			    ex, ey, bindex, MouseButton(dpy,event));
		break;

	case ButtonRelease:
		ex = event->xbutton.x;
		ey = event->xbutton.y;

		if (MouseButton(dpy,event) == MB_ADJUST)
			break;

		bindex = findButton(menu, &ex, &ey, winInfo);

		/* Make sure that the button under the ButtonRelease
		 * is the same one that the ButtonPress went down on.
		 */
		if ((bindex == bindex_last) &&
		    (bindex_last >= 0) &&
		    (bindex_last < menu->buttonCount))
		{
			button = &(menu->buttons[bindex_last]);
			if (button->stacked)
				ExecDefaultAction(dpy, winInfo,
					(Menu *)(button->action.submenu),
					True);
			else
			{
				(*button->action.callback)(dpy,
					winInfo, menu, bindex_last, True);
			}
		}

		afterProcessing(dpy, menu, winInfo, bindex_last);
		break;
	
	case ClientMessage:
		if (event->xclient.message_type==AtomDeleteWindow)
		{
			/* make our window die */
			/* lint will complain about this cast */
			free((char *)(winInfo->priv.ptr));
			XDestroyWindow(dpy, winInfo->self);

			/* we also need to destroy the frame */
			(*winInfo->frame->destroyfunc)(dpy, 
				winInfo->frame);

			/* Set this to False so we won't gray out the
			 * pushpin next time it is drawn.
			 */
			originalMenu->currentlyDisplayed = False;
		}
		break ;

	case Expose:
		/* draw in the menu contents */
		DrawMenu(dpy, menu);
		break;

	case PropertyNotify:
		/* If this frame's pushpin has been 'pulled out'
		 * get rid of the window.  We get a PropertyNotify
		 * when the pushpin routine changes the state
		 * of this frame's pushpin propety with 
		 * XChangeProperty.  We could get a PropertyNotify
		 * for other reasons also, so check that it is
		 * a pushpin change.
		 */
		if ((event->xproperty.atom == AtomPushpinState) &&
		    (event->xproperty.state == PropertyNewValue))
		{
			/* Now we know the pushpin changed state.
			 * Let's see if it is now out.
			 */
			XGetWindowProperty(dpy, 
					   winInfo->self, 
					   AtomPushpinState,
					   0L, 
					   LONG_LENGTH(long), /* pinState */
					   False, 
					   XA_INTEGER, 
					   &actType, 
					   &actFormat, 
					   &nItems, 
					   &remain, 
					   /* lint complains about this cast */
					   (unsigned char **)&pinState);
	
			if (*pinState == 0)	/* pin is out */
			{
				/* make our window die */
				/* lint will complain about this cast */
				free((char *)(winInfo->priv.ptr));
				XDestroyWindow(dpy, winInfo->self);
	
				/* we also need to destroy the frame */
				(*winInfo->frame->destroyfunc)(dpy, 
					winInfo->frame);

				/* Set this to False so we won't gray out the
				 * pushpin next time it is drawn.
				 */
				originalMenu->currentlyDisplayed = False;
			}
		}  /* End if pushpin changed state */
		break;


	default:
		if (PrintOrphans)
			DebugEvent(event, "EventMenuWin");
		break;
	}
}

/* findButton - find the button corresponding to an x,y position
 *		modifies the x,y position to the correct popup location
 *		for cascaded menus.
 * returns: button index or -1 if button not found
 */
static int
findButton(menu, ex, ey, winInfo)
Menu *menu;
int *ex, *ey;
WinInfo *winInfo;
{
	int bindex;

	/* figure out if the button press is in a button */
	for (bindex=0; bindex < menu->buttonCount; bindex++)
	{
		if (PointInRect(*ex, *ey,
				menu->buttons[bindex].activeX,
				menu->buttons[bindex].activeY,
				menu->buttons[bindex].activeW,
				menu->buttons[bindex].activeH))
		{
			*ex = winInfo->frame->x + winInfo->x +
			     menu->buttons[bindex].activeX +
			     menu->buttons[bindex].activeW;
			*ey = winInfo->frame->y + winInfo->y +
			     menu->buttons[bindex].activeY;
			return bindex;
		}
	}
	return -1;
}

/* pressButton -- a user has moved the pointer in such a way that a
 *	button should be pressed (that is, highlighted, ready for
 *	activation.  If any other button is pressed, unpress that button
 *	along the way.
 */
static int
pressButton(dpy, menu, winInfo, ex, ey, bindex, mbutton)
Display *dpy;
Menu *menu;
WinInfo *winInfo;
int ex, ey;
int bindex;
int mbutton;
{
	void afterProcessing();

	if ((mbutton == MB_MENU) || (mbutton = MB_SELECT))
	{
		lightButton(dpy, menu, bindex);

		/* if the pressed button is a button stack, 
		 * call menu shower with the new menu.
		 * Must do after processing here, since we will never
		 * a button-release event.
		 */
		if ((menu->buttons[bindex].stacked == True) &&
		    (menu->buttons[bindex].action.submenu != NULL) &&
		    (mbutton == MB_MENU))
		{
			MenuShow(dpy, winInfo->frame,
				 (Menu *)(menu->buttons[bindex].action.submenu),
				 ex, ey);
			afterProcessing(dpy, menu, winInfo,bindex);
		}
	}
}

static void
afterProcessing(dpy, menu, winInfo, bindex)
Display *dpy;
Menu *menu;
WinInfo *winInfo;
int bindex;
{
	Menu	*originalMenu = menu->originalMenu; /* menu this is derived from */
	Atom 	actType;
	int	actFormat;
	unsigned long	nItems, remain;
	long	*pinState;

	/* unhighlight button */
	if ((bindex >= 0) && (bindex < menu->buttonCount))
		UnlightButton(dpy, menu, bindex);

	/* if our push-pin is out then we should go away */
	XGetWindowProperty(dpy, 
			   winInfo->self, 
			   AtomPushpinState,
			   0L, 
			   LONG_LENGTH(long), 	/* pinState */
			   False, 
			   XA_INTEGER, 
			   &actType, 
			   &actFormat, 
			   &nItems, 
			   &remain, 
			   /* lint will complain about this cast */
			   (unsigned char **)&pinState);

	if (*pinState == 0)	/* pin is out */
	{
		/* make our window die */
		/* lint will complain about this cast */
		free((char *)(winInfo->priv.ptr));
		XDestroyWindow(dpy, winInfo->self);

		/* we also need to destroy the frame */
		(*winInfo->frame->destroyfunc)(dpy, 
			winInfo->frame);

		/* Set this to False so we won't gray out the
		 * pushpin next time it is drawn.
		 */
		originalMenu->currentlyDisplayed = False;
	}
}
