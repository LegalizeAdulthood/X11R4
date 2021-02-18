/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) menu.c 5.1 89/12/14 Crucible";
/*
 * This file contains all of the functions for creating and displaying menus.
 *
 * Global Functions:
 * MenuInit	-- initialize menu stuff
 * MenuCreate	-- create a new menu
 * MenuShow	-- display a menu
 */

#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>
#include "bitmaps/pushpin.h"

/* Externals */
extern GC		MenuTitleNormalGC, MenuShadowGC, MenuCopyGC;
extern GC		MenuButtonNormalGC, MenuButtonReverseGC;
extern Cursor   	RootWinCursor;
extern XFontStruct	*MenuTitleFontInfo;
extern XFontStruct	*MenuButtonFontInfo;
extern XFontStruct	*NormalFontInfo;
extern int		FlashTime;	/* Button flash time in microsec. */
extern long		MenuForeColor, MenuBackColor;
extern int		PrintOrphans;

extern Graphics_info *olgx_gismisc;
extern Graphics_info *olgx_gisbold;
extern Graphics_info *olgx_gismenu;

/* Locals */
static XPoint		menuMarkPoints[4];

/* Menu info alloc support. */
#define MAXCHILDREN	8		/* More than enough. */
static MenuInfo	menuTable[MAXCHILDREN];

/* Menu callback support. */
static int	(*callback)() = NULL;	/* Button action. */
static Menu	*menuContext = NULL;	/* Context of button press. */

/* These menu modes delineate the two possible kinds of menu display.
 * MODE_DRAGRIGHT "Open-Look" style menu where the button is
 * held down and dragging it to the right causes the button activation.
 * MODE_CLICK is when the button is clicked to activate a button.
 */
#define MODE_DRAGRIGHT	1
#define MODE_CLICK	2

/* GC macros */
#define DRAWNORMTITLE_GC	(MenuTitleNormalGC)	/* fg on bg */
#define DRAWNORMBUTTON_GC	(MenuButtonNormalGC)	/* fg on bg */
#define DRAWREVBUTTON_GC	(MenuButtonReverseGC)	/* bg on fg */
#define MENUSHADOW_GC		(MenuShadowGC)		/* b/w sitpple */

/* Menu pointer status. */
#define S_ONBUTTON	10	/* Pointer is in a button area. */
#define S_ONMENU	11	/* Pointer is in a menu background area. */
#define S_ACTIVATE	12	/* Select action happened. */
#define S_OFFMENU 	13	/* Pointer is off of the menu entirely. */
#define S_ONPIN		14	/* Pointer is in the pushpin area. */
#define S_COMPLETE	15	/* Action/selection finished. */
#define S_PASSEVENTUP	16	/* Event is not meant for our window. */

/* Calculate fontheight from font info structure. */
#define FONT_HEIGHT(f)	((f)->ascent + (f)->descent)
#define BUTT_FONTHEIGHT	FONT_HEIGHT(MenuButtonFontInfo)
#define BUTT_FONTASCENT	(MenuButtonFontInfo->ascent)
#define TITLE_FONTASCENT	(MenuTitleFontInfo->ascent)

/* Label positioning. */
#define TEXT_HEIGHT	FONT_HEIGHT(MenuButtonFontInfo)
#define LABEL_GAP	(4)		/* Space above/below text in button. */

/* Height and curve raduis of button. */
#define BUTT_HEIGHT	(BUTT_FONTHEIGHT + (LABEL_GAP * 2))
#define BUTT_RADIUS	((BUTT_FONTHEIGHT + (LABEL_GAP * 2)) / 3)

/* Space between buttons (these should be adjusted for resolution). */
#define BUTT_VSPACE	0  /* There used to be space between buttons. */
#define BUTT_HSPACE	5

/* Space above and below the titlebar text, and the space below the
 * last button in the menu.
 */
#define HEAD_VSPACE	2

/* The size of the menu mark is dependant on the font height. */
#define MENUMARK_SIZE 6

/* Width of menu border. */
#define MENUBW		0

/* Distance to the left and down of drop shadow (altitude of menu). */
#define MENUSHADOW_OFF	(10)

/* Events in the menu window that are interesting. */
#define MENU_EVENT_MASK	(PropertyChangeMask | SubstructureNotifyMask)

/* Cheapo shortcut macro for the root window. */
#define RWIN(dpy)		RootWindow(dpy, DefaultScreen(dpy))

/* Handy interface to drawRoundRect.  These are our quicky rounded
 * corners.  Eventually we will use a font for the rounded corners.
 */
#define frameRoundRect(dpy, draw, gc, x, y, w, h, ew, eh) \
		drawRoundRect(dpy, draw, gc, x, y, w, h, ew, eh, 0)
#define fillRoundRect(dpy, draw, gc, x, y, w, h, ew, eh) \
		drawRoundRect(dpy, draw, gc, x, y, w, h, ew, eh, 1)

/* This routine is defined so that 'callback' can be initialized to
 * a non value.  We can't use NULL because someone might make a menu
 * that has a function pointer that is NULL, (e.g. they are developing
 * it), and when the user buttons on that menu button the callback
 * gets set to a function pointer, albeit a NULL pointer.  But,
 * the fact that 'callback' has been set tells us the user has made a 
 * selection.  Thus, to be able to recognize the fact that callback
 * has been set to something we need to initialize 'callback' to 
 * something besides NULL.  So, we initialize it to notSet.
 */
notSet()
{
}

/* This is the last event which happened.  It's used by menus
 * that have submenu(s) that have been activated.  When they come
 * back up the recursion chain a menu can look at this to
 * understand why its submenus exited.  This is needed for
 * 'click' menus because a user could click five levels of menus
 * down, and then choose something from the second menu, for example.
 */
XEvent	LastEvent;

/*
 * Global routines
 */

/*	
 * InitMenus	-- get the font and related menu initialization
 *	dpy	- display
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
InitMenus(dpy)
Display		*dpy;
{
	/* Most of the stuff that should be in here 
	 * is actually in InitGraphics.c
	 */
	
	/* Initialize the points in the menu mark.
	 * These points are drawn with CoordModeRelative.
	 * menuMarkPoints[0] is the point of the triangle that
	 * points off to the right.
	 */
	menuMarkPoints[0].x = 0;
	menuMarkPoints[0].y = 0;
	menuMarkPoints[1].x = -MENUMARK_SIZE;
	menuMarkPoints[1].y = -MENUMARK_SIZE/2;
	menuMarkPoints[2].x = 0;
	menuMarkPoints[2].y = MENUMARK_SIZE;
	menuMarkPoints[3].x = MENUMARK_SIZE;
	menuMarkPoints[3].y = -MENUMARK_SIZE/2;

}

/*
 * MenuCreate	-- fill out a new menu from an array of buttons
 *	dpy	- display to create menu on
 *	menu	- pointer to menu structure to be filled out
 */
MenuCreate(dpy, menu)
Display		*dpy;
Menu		*menu;
{
	Button	*buttons;		/* Copy of menu->buttons. */
	Button	*bp;			/* Temp button pointer. */
	int	count;			/* Copy of menu->buttonCount. */
	int	bindex;			/* Current button. */
	int	maxLabWidth;		/* Width of longest menu label. */
	int	menWidth, menHeight;	/* Width and height of menu. */
	int	nextY;			/* Button pos in menu. */
	int	hasStacked;		/* True if there are submenus. */ 
	XSetWindowAttributes attributes;

	buttons = menu->buttons;
	count = menu->buttonCount;

	/* Find longest menu entry. */
	for (maxLabWidth = 0, bindex = 0; bindex < count; bindex++)
	{
		maxLabWidth = MAX(maxLabWidth,
				  XTextWidth(MenuButtonFontInfo,
					     buttons[bindex].label,
					     strlen(buttons[bindex].label)));
		if (buttons[bindex].stacked)
			hasStacked = True;
	}

	maxLabWidth += 2 * BUTT_RADIUS;

	/* If any of the buttons have submenus, 
	 * make space for the menu mark. 
	 */
	if (hasStacked)
		maxLabWidth += BUTT_HSPACE + MENUMARK_SIZE;

	/* Calculate title parameters. */
	if (menu->title != NULL)
	{
		menu->titleWidth = XTextWidth(MenuTitleFontInfo,
					      menu->title,
					      strlen(menu->title));
		menu->titleHeight = HEAD_VSPACE + 
				    MAX(FONT_HEIGHT(MenuTitleFontInfo),
					pushpin_height) +
				    HEAD_VSPACE;

		if (menu->hasPushPin)
		{
			menu->titleX = BUTT_HSPACE + pushpin_width +
					BUTT_HSPACE;
			menu->titleY = HEAD_VSPACE + TITLE_FONTASCENT;

			menWidth = MAX(BUTT_HSPACE + maxLabWidth + BUTT_HSPACE,
				       (BUTT_HSPACE + pushpin_width +
					BUTT_HSPACE + menu->titleWidth +
					BUTT_HSPACE));
		}
		else
		{
			menWidth = MAX(BUTT_HSPACE + maxLabWidth + BUTT_HSPACE,
				       BUTT_HSPACE + menu->titleWidth +
				       BUTT_HSPACE);

			menu->titleX = (menWidth / 2) - (menu->titleWidth / 2);
			menu->titleY = HEAD_VSPACE + TITLE_FONTASCENT;
		}
	}
	else
	{
		menWidth = BUTT_HSPACE + maxLabWidth + BUTT_HSPACE;

		menu->titleX = 0;
		menu->titleY = 0;
		menu->titleWidth = 0;
		menu->titleHeight = 0;
	}
	
	/* Menu height is the sum of the buttons, the title height if any,
	 * the space above the first button, and the space below the last 
	 * button.
	 */
	menHeight = menu->titleHeight + HEAD_VSPACE +
		    ((BUTT_HEIGHT + BUTT_VSPACE) * count) +
		    HEAD_VSPACE;

	menu->width = menWidth;
	menu->height = menHeight;
	
	/* Place the pushpin.
	 * Pushpin is centered verticaly in case the font height
	 * is smaller than the pushpin height.
	 */
	menu->pushPinX = BUTT_HSPACE;
	menu->pushPinY = (menu->titleHeight - pushpin_height) / 2;

	/* Menu window. */
	attributes.event_mask = ButtonPressMask;
	attributes.background_pixel = bg1Color;
	attributes.save_under = DoesSaveUnders(DefaultScreenOfDisplay(dpy));
	menu->window = XCreateWindow(dpy, RWIN(dpy),
					0, 0,
					menWidth,
					menHeight,
					MENUBW,
					DefaultDepth(dpy, DefaultScreen(dpy)),
					InputOutput,
					DefaultVisual(dpy, DefaultScreen(dpy)),
					CWEventMask | CWSaveUnder | CWBackPixel,
					&attributes);

#ifdef SHADOW
	attributes.background_pixmap = pixmapGray;
	attributes.save_under = DoesSaveUnders(DefaultScreenOfDisplay(dpy));
	menu->shadow = XCreateWindow(dpy, RWIN(dpy),
					0, 0,
					menWidth,
					menHeight,
					0,
					DefaultDepth(dpy, DefaultScreen(dpy)),
					InputOutput,
					DefaultVisual(dpy, DefaultScreen(dpy)),
					CWBackPixmap |  CWSaveUnder,
					&attributes);
#endif /* SHADOW */

	/* Precalculate the button postions for faster 
	 * display/drawing and button-press checking.
	 * Because pinned menus don't have title windows,
	 * ( we just draw in the title ),
	 * these positions are calculated without the titleHeight.
	 */
	for(nextY = BUTT_VSPACE, bindex = 0; bindex < count; bindex++)
	{
		bp = &buttons[bindex];
		/* These describe the area of the button that will
		 * be hightlighted when the button is activated. Or
		 * one could say that these describe the area that
		 * will activate the button, since the button press code
		 * uses these to determine if a button press happens.
		 */
		bp->activeX = BUTT_HSPACE;
		bp->activeY = nextY;
		bp->activeW = menWidth - (2 * BUTT_HSPACE);
		bp->activeH = BUTT_HEIGHT;

		/* Actual location where the label will be displayed. */
		bp->labelX = BUTT_HSPACE + BUTT_RADIUS;
		bp->labelY = nextY + LABEL_GAP + BUTT_FONTASCENT;

		bp->menuMarkX = menWidth - 
			(BUTT_HSPACE + MENUMARK_SIZE);

		/* Move down to next button postion. */
		nextY += BUTT_HEIGHT + BUTT_VSPACE;
	}
}

/*
 * ExecDefaultAction(dpy, winInfo, menu, fPinned)
 *
 * Given a menu, execute the associated default action (if any)
 */  
ExecDefaultAction(dpy, winInfo, menu, fPinned)
Display *dpy;
WinInfo *winInfo;
Menu *menu;
Bool fPinned;
{
	FuncPtr defaultCallback;

	if (menu->buttonDefault < 0)
		return;

	defaultCallback = menu->buttons[menu->buttonDefault].action.callback;
	if (defaultCallback != NULL)
	{
		(*defaultCallback)(dpy, winInfo, menu, 
				   menu->buttonDefault, fPinned);
	}
}

/*
 * Draw menu contents into menu->window.
 */
DrawMenu(dpy, menu)
Display	*dpy;
Menu	*menu;
{
	int bindex;
	int byOff = 0;
	Window	win = menu->window;

	/* Draw the basic menu background if this menu isn't pinned */
	if (menu->originalMenu == NULL)
	{
		olgx_draw_box(olgx_gismenu, win, 0, 0, 
			menu->width, menu->height,  OLGX_NORMAL, True);
	}

	/* Draw the menu title. */
	if (menu->title != NULL)
	{
		if (menu->hasPushPin)
		{
			/* If the menu is already displayed, draw the
			 * pushpin grayed out to indicate that it can't
			 * be pinned again.
			 */
			if (menu->currentlyDisplayed)
			{
				olgx_draw_pushpin(olgx_gismisc, win, menu->pushPinX,
					menu->pushPinY,
					OLGX_ERASE | OLGX_PUSHPIN_OUT | OLGX_INACTIVE);
			}
			else
			{
				olgx_draw_pushpin(olgx_gismisc, win, menu->pushPinX,
					menu->pushPinY,
					OLGX_ERASE | OLGX_PUSHPIN_OUT);
			}
		}

		olgx_draw_text(olgx_gisbold, win, menu->title, menu->titleX,
			menu->titleY, 0, False, OLGX_NORMAL);

		olgx_draw_text_ledge(olgx_gismenu, win, 
			BUTT_HSPACE, menu->titleHeight-2, 
			menu->width-(BUTT_HSPACE*2));

		/* byOff is the gap between the top of the menu and the
		 * top of the first button.
		 */
		byOff = menu->titleHeight;
	}
	else  /* No title on this menu. */
		byOff = HEAD_VSPACE;

	/* Draw the buttons. */
	for (bindex=0; bindex < menu->buttonCount; bindex++)
		drawButton(dpy, win, &menu->buttons[bindex], byOff, (bindex==menu->buttonDefault), (menu->originalMenu != NULL));

}

/* Draw a normal button. 
 * if fDefault is true, a default ring will be drawn.
 * fIsPinned indicates whether this button is on a pinned menu.
 */
/*ARGSUSED*/	/* dpy arg is inside #ifdef PREOLGX - lint doesn't see it */
drawButton(dpy, win, button, yOffset, fDefault, fIsPinned)
Display	*dpy;
Window	win;
Button	*button;
int	yOffset;
Bool 	fDefault;
Bool 	fIsPinned;
{
#ifdef PREOLGX
	/* Fill button text area with white.
	 * We draw a filled rectangle one pixel wider all the
	 * way around so we can be sure to 'erase' the polygon
	 * drawn for the button when it was highlighted.  See
	 * O'Reilly book Vol #2 XFillPolygon:  "Some but not
	 * all of the path itself will be drawn."  Doing this
	 * is completely unnecessary for SOME servers.  I am
	 * not sure if it is necessary if that shows a bug
	 * in the server.
	 */


	XFillRectangle(dpy, win, DRAWREVBUTTON_GC,
		      button->activeX - 1, button->activeY + yOffset - 1,
		      button->activeW + 2, button->activeH + 2);

	/* draw button text */
	if (button->state == Enabled)
		XDrawString(dpy, win, DRAWNORMBUTTON_GC,
			    button->labelX, button->labelY + yOffset,
			    button->label, strlen(button->label));
	else
		XDrawString(dpy, win, MENUSHADOW_GC,
			    button->labelX, button->labelY + yOffset,
			    button->label, strlen(button->label));
	
	/* Draw menu mark. */
	if (button->stacked)
		drawMenuMark(dpy, win, DRAWNORMBUTTON_GC,
			     button->menuMarkX,
			     button->activeY + button->activeH/2 + yOffset);
#endif

	int state;

	state = OLGX_NORMAL | 
		((button->state == Enabled)? 0 : OLGX_INACTIVE) |
		OLGX_ERASE | 
		((button->stacked) ? OLGX_HORIZ_MENU_MARK : 0) |
		(fDefault? OLGX_DEFAULT : 0);
	if (!(fIsPinned && FShowPinnedMenuButtons || FShowMenuButtons))
	{
		/* setting this flag turns OFF painting of the menu buttons */
		state |= OLGX_MENU_ITEM;
	}

	olgx_draw_button(olgx_gismenu, win, button->activeX, 
			button->activeY+yOffset, button->activeW, 
			button->label, state);
			
}

/* Draw a reverse video button. */
/*ARGSUSED*/	/* dpy arg is inside #ifdef PREOLGX - lint doesn't see it */
drawRevButton(dpy, win, button, yOffset)
Display	*dpy;
Window	win;
Button	*button;
int	yOffset;
{
	int state;

	/* if the button is disabled, do nothing */
	if (button->state == Disabled)
		return;

#ifdef PREOLGX
	/* Fill button text area with black. */
	drawRoundRect(dpy, win, DRAWNORMBUTTON_GC,
		      button->activeX, button->activeY + yOffset,
		      button->activeW, button->activeH,
		      BUTT_RADIUS, BUTT_RADIUS,
		      True);

	/* Draw button text. */
	XDrawString(dpy, win, DRAWREVBUTTON_GC,
		    button->labelX, button->labelY + yOffset,
		    button->label, strlen(button->label));

	/* Draw menu mark. */
	if (button->stacked)
		drawMenuMark(dpy, win, DRAWREVBUTTON_GC,
			     button->menuMarkX,
			     button->activeY + button->activeH/2 + yOffset);
#endif

	state = OLGX_INVOKED | OLGX_ERASE | 
		((button->stacked) ? OLGX_HORIZ_MENU_MARK : 0);
	olgx_draw_button(olgx_gismenu, win, button->activeX, button->activeY + yOffset,
			button->activeW, 
			button->label, state | OLGX_MENU_ITEM);
}

static MenuInfo *
allocMenuInfo(menu)
Menu	*menu;
{
	MenuInfo	*new;
	int		i;

	/* Find an empty slot. */
	for(i=0; i<MAXCHILDREN; i++)
		if (!menuTable[i].valid)
			break;

	new = &menuTable[i];
	new->menu = menu;
	new->childActive = False;
	new->childMenu = (Menu *)NULL;
	/*new->leftPin = True;*/
	new->pinIn = False;
	new->litButton = -1;

	new->valid = True;
	return new;
}

static
unmapChildren(dpy, mInfo)
Display		*dpy;
MenuInfo	*mInfo;
{
	int		i;

	/* A little bit of recursion is used here to
	 * unmap the child menus in youngest to oldest order.
	 */
	if (mInfo->childActive)
	{
		for(i=0; i<MAXCHILDREN; i++)
			if (menuTable[i].valid &&
			    (menuTable[i].menu == mInfo->childMenu))
			{
				unmapChildren(dpy, &menuTable[i]);
#ifdef SHADOW
				XUnmapWindow(dpy, menuTable[i].menu->shadow);
#endif /* SHADOW */
				XUnmapWindow(dpy, menuTable[i].menu->window);
				menuTable[i].valid = False;
				break;
			}
	}

	mInfo->childActive = False;
}

Window		eventWindow;
int		activeButton = -1;	/* Index of button pressed. */

/*
 * MenuShow	-- display a menu at the given postion

 * Purpose:
 *
 *	This routine handles a menu, and its submenus.
 *
 * Input:
 *
 *	dpy	- display to show menu on
 *	winInfo - description of window 
 *	menu	- pointer to menu to be displayed
 *	x, y	- postion of upperleft corner of menu
 *
 * Output:
 *
 *	None
 *
 * Globals:
 *
 *	callback	- function pointer that (possibly) gets 
 *			  set by selecting one of the menu buttons.
 *
 *	menuTable	- array of the currently displayed menus.
 *
 */
int
MenuShow(dpy, winInfo, menu, x, y)
Display		*dpy;
WinInfo		*winInfo;
Menu		*menu;
int		x, y;
{
	int		i;

	/* Set callback to notSet.  Later, if we see that callback
	 * has been changed we'll know were finished.
	 * We have to initialize it to a noop function because
	 * the user could set up a callback function which is
	 * NULL.
	 */
	callback = notSet;

	/* Initialize the menu info alloc stuff. */
	/* memset takes int 2nd arg (uses as char) */
	/* lint will complain about this cast */
	memset((char *)menuTable, 0, sizeof(MenuInfo) * MAXCHILDREN);

	/* Grab the server to prevent anybody from
	 * sullying the underlying windows when the 
	 * menu window is mapped.
	 */
	XGrabServer(dpy);
	
	XGrabPointer(dpy, RWIN(dpy),
		     True,
		     ButtonReleaseMask | ButtonMotionMask |
		     ButtonPressMask | OwnerGrabButtonMask,
		     GrabModeAsync, GrabModeAsync,
		     None,
		     RootWinCursor,
		     CurrentTime);
        
	/* This is the routine which recursively handles all the
	 * menus.  It is calls itself to display and handle each
	 * menu.
	 */
	recursiveMenuShow(dpy, menu, x, y);

	/* User has finished, and (possibly) made a selection.
	 * let go of the server
	 */
	XUngrabServer(dpy);
	XUngrabPointer(dpy, CurrentTime);
	XFlush(dpy);

	/* Unmap any active menus. */
	for(i=MAXCHILDREN-1; i>=0; i--)
	{
		if (!menuTable[i].valid)
			continue;
		else
		{
#ifdef SHADOW
			XUnmapWindow(dpy, menuTable[i].menu->shadow);
#endif /* SHADOW */
			XUnmapWindow(dpy, menuTable[i].menu->window);
		}
	}

	/* Call the callback if it was set; or do default menu action. */
	if (callback != notSet)
		if (callback != NULL) 
			(*callback)(dpy, winInfo, menuContext, activeButton, False);
		else
		{
			ExecDefaultAction(dpy, winInfo, 
				(Menu *)(menuContext->buttons[activeButton].action.submenu),
				False);
		}
	
}  /* End MenuShow */


/*
 * recursiveMenuShow	-- display, and handle a menu at the given postion
 *
 *   Purpose:
 *
 *	This routine gets called to handle one menu.  If the user
 *	pulls off to the right or clicks on a button which has
 *	a submenu this routine will get called again to handle
 *	the submenu.  The recursive call to recursiveMenuShow is in
 *	the click and drag MenuEvent routines.
 *
 *	You should probably spend a minute or two looking at this
 *	so you will understand how the menus are recursively handled.
 *
 *	An example chain of calls for a user who creates a click
 *	menu, a click submenu of it, and a click submenu of the submenu,
 *	( a chain of three menus ), and then clicks in the root
 *	window to dismiss them, would be something like this:
 *
 *		<user button press in root window causes call to>
 *		Enter MenuShow
 *		  <grabs the server and calls>
 *		  Enter recursiveMenuShow
 *		    <displays the menu.>
 *		    <sees that the next event is a button release
 *		    <which means this will be a click menu and not
 *		    <a drag menu, so it calls>
 *		    Enter handleClickMenu
 *		      <gets the next event and passes it to>
 *		      Enter clickMenuEvent
 *			<calls checkMenuClickButton to find out what to do.>
 *		        Enter checkMenuClickButton
 *			<figures out what button was pressed.>
 *			Exit checkMenuClickButton
 *			<user pressed button so clickMenuEvent calls>
 *		        Enter recursiveMenuShow
 *			<displays the menu.>
 *			<sees the next event is a button release so it calls>
 *			  Enter handleClickMenu
 *			  <gets the next event and passes it to>
 *			    Enter clickMenuEvent
 *			      <calls checkMenuClickButton>
 *			      <to find out what to do>
 *			      Enter checkMenuClickButton
 *			      Exit checkMenuClickButton
 *			      <user pressed button so clickMenuEvent calls>
 *			      Enter recursiveMenuShow
 *			      <displays the menu.>
 *			      <sees that the next event is a button release>
 *			      <so it calls>
 *		    	        Enter handleClickMenu
 *				<gets the next event.>
 *				<sees that it is not in its window>
 *				<so it sets status to S_PASSEVENTUP>
 *			        Exit handleClickMenu
 *			      Exit recursiveMenuShow
 *			    Exit clickMenuEvent
 *			    <clickMenuEvent returns with status S_PASSEVENTUP>
 *			    <so handleClickMenu sets its event to the last>
 *			    <event that happend.>
 *			    <sees that it is not in its window>
 *			    <so it sets status to S_PASSEVENTUP>
 *			  Exit handleClickMenu
 *		        Exit recursiveMenuShow
 *		      Exit clickMenuEvent
 *		      <clickMenuEvent returns with status S_PASSEVENTUP>
 *		      <so handleClickMenu sets its event to the last>
 *		      <event that happend.>
 *		      <sees that it is not in its window>
 *		      <so it sets status to S_PASSEVENTUP>
 *		    Exit handleClickMenu
 *	          Exit recursiveMenuShow
 *	        Exit MenuShow
 *
 *  Input:
 *
 *	dpy	- display on which to show menu.
 *	winInfo - description of window 
 *	menu	- pointer to menu to be displayed
 * 	x, y	- postion of upperleft corner of menu
 *
 *  Output:
 *
 *	Returns a status of how the menu was exited.
 *
 */
int
recursiveMenuShow(dpy, menu, x, y)
Display		*dpy;
Menu		*menu;
int		x, y;
{
	XEvent		event;
	unsigned long	eMask;
	int		status;
	int		menuMode;

	/* Show the menu. */
	showMenu(dpy, menu, x, y);

	XGrabPointer(dpy, RWIN(dpy),
		     True,
		     ButtonReleaseMask | ButtonMotionMask |
		     ButtonPressMask | OwnerGrabButtonMask,
		     GrabModeAsync, GrabModeAsync,
		     None,
		     RootWinCursor,
		     CurrentTime);
        
	/* Look at the next event.  If it is a mouse movement that
	 * means this menu will be handled as a drag right
	 * menu.  If the user released the button, that means this
	 * menu will be handled as a click menu.
	 * If the menu will be a click menu we are only interested
	 * in button presses.  If the menu will be a drag right
	 * menu we are only interested in button releases and motion.
	 */
	do
	{
		XPeekEvent(dpy, &event);
		switch (event.type)
		{
		case ButtonRelease:
			XNextEvent(dpy, &event);  /* Swallow the event. */
			menuMode = MODE_CLICK;
			eMask = ButtonPressMask;
			break;

		case MotionNotify:
			menuMode = MODE_DRAGRIGHT;
			eMask = ButtonReleaseMask | ButtonMotionMask;
			break;

		default:
			XNextEvent(dpy, &event);  /* Swallow the event
						   * that we can't use. 
						   */
		}
	}
	while ((event.type != ButtonRelease) && (event.type != MotionNotify));

	/* Tell the server what sort of input events we want. */
	XChangeActivePointerGrab(dpy, eMask | OwnerGrabButtonMask,
				 RootWinCursor, CurrentTime);

	/* Depending upon the type of menu we will be dealing
	 * with, call the appropriate handler.
	 */
	switch (menuMode)
	{
	case MODE_CLICK:
		status = handleClickMenu(dpy, menu);
		break;
	case MODE_DRAGRIGHT:
		status = handleDragRightMenu(dpy);
		break;
	}

	return status;

}  /*  End recursiveMenuShow  */

/*
 * handleDragRightMenu -- handle a drag right menu.
 *
 * Purpose:
 *
 *	This routine handles the operation of a drag right
 *	menu, highlighting buttons and activating submenus.
 *
 *	The routine sits in a while loop and calls XNextEvent
 *	to get the next event.  It then calls dragMenuEvent
 *	to handle the event.  Depending upon the return status
 *	and the status of the callback function pointer it
 *	breaks out of the while loop and exits, or continues
 *	going around the loop.
 *
 *	Please Note:  when you are looking at this routine
 *		      remember that after the for loop in
 *		      which dragMenuEvent is called, any
 *		      number of submenus could have been
 *		      activated and the event which caused
 *		      them to end is not the event which
 *		      the local variable 'event' was set
 *		      to.
 *
 * Input:
 *
 *	dpy	- The display on which this menu is displayed.
 *
 * Output:
 *
 *	The routine returns a status of why the menu finished.
 *
 * Globals:
 *
 *	callback	- function pointer that (possibly) gets 
 *			  set by selecting one of the menu buttons.
 *
 *	menuTable	- array of the currently displayed menus.
 */
handleDragRightMenu(dpy)
Display		*dpy;
{

	int		i;
	WinInfo		*winInfo;
	MenuInfo	*mInfo;
	int		status;
	XEvent		event;
	XAnyEvent	*anyEvent = &(event.xany);
	int		bindex;

	/* We exit this loop by calling break.  There are numerous
	 * if <condition> then break constructs in the loop.
	 * Main structure of this loop is:
	 *
	 *	While forever loop
	 *		Get the next event
	 *		If it's in this menu
	 *			Call dragMenuEvent to handle it.
	 *		If the user has selected something, 
	 *		or dismissed the menu
	 *			Break out of while loop.
	 *	end while forever loop
	 */
	while(1)
	{
		XNextEvent(dpy, &event);

		/* Save this event.  If we return up the recursion stack
		 * the menu whose button activated us might be interested 
		 * in this.  Only click menus use it.
		 */
		LastEvent = event;

		/* We don't care about ButtonPress events.
		 * So skip over the rest of the code in the loop.
		 * We could get one of these while the user
		 * is holding down the right button for his/her
		 * drag menu and also hits the left button.
		 */
		if (event.type == ButtonPress)
		{
			continue;
		}

		/* Find the window on which the event occured.
		 * Since the pointer has been grabbed relative to the root
		 * window, the event window is actually the subwindow of the 
		 * event.
		 */
		switch (event.type)
		{
		case ButtonRelease:
			eventWindow = event.xbutton.subwindow;
			break;
		
		case MotionNotify:
			eventWindow = event.xbutton.subwindow;
			break;

		case Expose:
			winInfo = WIGetInfo(anyEvent->window);
			if ((winInfo != NULL) && (winInfo->eventfunc != NULL))
				(*winInfo->eventfunc)(dpy, &event, winInfo);
			else
				if (PrintOrphans)
					DebugEvent(&event, "Orphaned");
			continue;

		default:
			/* Ignore other events. */
			continue;
		}

		/* Find event related menu. (brute force table search) 
		 * i.e. it has to check every menu to see in which menu
		 * this event happened.
		 * The dragMenuEvent routine will handle lighting buttons,
		 * setting the callback, and calling recursiveMenuShow 
		 * for submenus. 
		 */
		for(i=0; i<MAXCHILDREN; i++)
		{
			if (!menuTable[i].valid)
				continue;

			mInfo = &menuTable[i];
			if (eventWindow == mInfo->menu->window)
			{
				status = dragMenuEvent(dpy, &event, 
						       mInfo, &bindex);
				break;
			}
		}  /* End for */

		if ((i == MAXCHILDREN) && (event.type == MotionNotify))
		{
			/* User is moving the mouse around not on any
			 * of the menus.  i.e. in the root window or
			 * in some other window.  Call dragMenuEvent so we 
			 * can be sure that any highlighted buttons get
			 * unhighlighted.
			 */
			status = dragMenuEvent(dpy, &event, mInfo, &bindex);
		}

		if ((i == MAXCHILDREN) && (event.type == ButtonRelease))
		{
			/* User let go of button not in a menu */
			status = S_COMPLETE;
			break;
		}

		/* If the user released the button while on a
		 * menu button, set the callback to that button's callback.
		 * If the user released the button while on the
		 * pushpin set the callback to the pinning function.
		 * If either of these didn't happen the user is just
		 * moving around with the button held down.
		 */
		if ((bindex != -1) && (status == S_COMPLETE) &&
		    (callback == notSet))
		{
			callback = mInfo->menu->buttons[bindex].action.callback;
			menuContext = mInfo->menu;
			activeButton = bindex;
			break;
		} else if ((status == S_ONPIN) && (callback == notSet))
		{
			callback = mInfo->menu->pinAction;
			menuContext = mInfo->menu;
			status = S_COMPLETE;
			break;
		}
		else if (status == S_COMPLETE)
			break;

	}  /* End while (1) */

	return status;

}  /*  End handleDragRightMenu  */


/*
 * handleClickMenu -- handle a click menu.
 *
 * Purpose:
 *
 *	This routine handles the operation of a click
 *	menu, highlighting buttons and activating submenus.
 *
 *	The routine sits in a while loop and calls XNextEvent
 *	to get the next event.  It then calls clickMenuEvent
 *	to handle the event.  Depending upon the return status
 *	and the status of the callback function pointer it
 *	breaks out of the while loop and exits, or continues
 *	going around the loop.
 *
 *	Please Note:  when you are looking at this routine
 *		      remember that after the for loop in
 *		      which clickMenuEvent is called, any
 *		      number of submenus could have been
 *		      activated and the event which caused
 *		      them to exit is not necessarily the 
 *		      event which the local variable 'event' 
 *		      was set to.  'event' would still be
 *		      the event which caused the submenu to
 *		      displayed.
 *
 * Input:
 *
 *	dpy	- The display on which this menu is displayed.
 *
 *	menu	- The menu which this routine will be handling.
 *
 * Output:
 *
 *	The routine returns a status of why the menu finished.
 *
 * Globals:
 *
 *	callback	- function pointer that (possibly) gets 
 *			  set by selecting one of the menu buttons.
 *
 *	menuTable	- array of the currently displayed menus.
 */
handleClickMenu(dpy, menu)
Display		*dpy;
Menu		*menu;

{
	int		i;
	MenuInfo	*mInfo;
	int		status;
	XEvent		event;
	int		bindex;

	status = -1;

	/* We exit this loop by calling break.  There are numerous
	 * if <condition> then break constructs in the loop.
	 * Main structure of this loop is:
	 *
	 *	While forever loop
	 *		Get the next event
	 *		If it's in this menu
	 *			Call clickMenuEvent to handle it.
	 *		If the user has selected something, 
	 *		or dismissed the menu
	 *			Break out of while loop.
	 *	end while forever loop
	 */
	while(1)
	{

		/* If the status returned by the clickMenuEvent routine
		 * was S_PASSEVENTUP that means that a submenu somewhere
		 * down the higheracrchey had a button press event which
		 * was not in its window.  It exits and tells its parent
		 * to check the last event and see if it was in its window.
		 * This continues on up the chain until the window which
		 * contained the event handles it, or it turns out that
		 * the event is not in any of the menus.
		 */
		if (status == S_PASSEVENTUP)
			event = LastEvent;
		else
		{
			XNextEvent(dpy, &event);
			/* Save this event.  If we return up the 
			 * recursion stack the menu which called 
			 * us might be interested in this.
		 	 */
			LastEvent = event;
		}

		/* If we got a ButtonRelease event we can't do anything
		 * with it, so skip the rest of the code in the loop.
		 * One case where we can have this happen is if the
		 * user presses and releases the button with the cursor
		 * on a menu but not on a button, or pushpin.  e.g. in
		 * the border or title area.  Thats the only way a press
		 * event doesn't cause some kind of action.
		 */
		if (event.type == ButtonRelease)
			continue;

		/* Find the window on which the event occured.
		 * Since the pointer has been grabbed relative to the root
		 * window, I would think the event window is actually 
		 * the subwindow of the event, but this is not the case.
		 * The event window for a button press event turns out to be 
		 * the window of the event.  The event window for a button
		 * release event is still the subwindow of the event though.
		 */
		switch (event.type)
		{
		case ButtonPress:
			eventWindow = event.xbutton.window;
			break;

		default:
			/* KeyPress, EnterNotify events, etc., that
			 * we don't do anything with.
			 */
			continue;
		}

		/* If the event happened in our window call clickMenuEvent
		 * to handle the event.
		 */
		if (eventWindow == menu->window)
			/* Find this menu in the menuTable. */
			for(i=0; i<MAXCHILDREN; i++)
			{
				if (!menuTable[i].valid)
					continue;

				mInfo = &menuTable[i];
				if (eventWindow == mInfo->menu->window)
				{
					status = clickMenuEvent(dpy, &event, 
								mInfo, &bindex);
					break;
				}
			}
		else
			/* The event didn't happen in this window. */
			return S_PASSEVENTUP;

		if (status == S_ACTIVATE)
			return S_ACTIVATE;


		if ((bindex != -1) && (status == S_COMPLETE) &&
		    (callback == notSet))
		{
			callback = mInfo->menu->buttons[bindex].action.callback;
			menuContext = mInfo->menu;
			activeButton = bindex;
			break;
		} 
		else if ((status == S_ONPIN) && (callback == notSet))
		{
			callback = mInfo->menu->pinAction;
			menuContext = mInfo->menu;
			break;
		}
		else if (status == S_COMPLETE)
			break;

	}  /* End while (1) */

	return S_COMPLETE;

}  /*  End handleClickMenu  */


/* 
 * dragMenuEvent -- handle an event on a drag menu.
 *
 * Purpose:
 *
 * 	This routine looks at the event passed to it and takes
 *	the appropriate action for that event.  If it is a motion
 *	event it will highlight/unhighlight any buttons, change
 *	the state of the pushpin, or activate any submenus.
 *	If it is a button release event it will set the callback
 *	for that button or pushpin, or dismiss the menu.
 *
 * Input:
 *
 *	dpy 	- The display on which this window will be drawn.
 *	pevent	- The event we will handle.
 *	mInfo	- Information about this menu.
 *
 * Output:
 *
 *	bindex	- Index of the button pressed.  If no button was
 *		  pressed this is set to -1.
 *
 *	Returns a status of the event on the menu.  S_ONPIN,
 *	S_COMPLETE, or 0 (means no action necessary).
 */
static int
dragMenuEvent(dpy, pevent, mInfo, bindex)
Display		*dpy;
XEvent		*pevent;
MenuInfo	*mInfo;
int		*bindex;
{
	int	status;
	int	rstatus = 0;  /* Event requires no response if 
			       * rstatus is 0.
			       */

	*bindex = -1;

	switch (pevent->type)
	{
	case MotionNotify:
		*bindex = checkMenuMotion(mInfo->menu, pevent, &status);

		/* If the push pin was in before and
		 * this event is not a S_ONPIN event,
		 * put the pin back out because we
		 * are no longer in the pin area. */
		if ((mInfo->pinIn) && (status != S_ONPIN))
		{
			olgx_draw_pushpin(olgx_gismisc, mInfo->menu->window,
				mInfo->menu->pushPinX,
				mInfo->menu->pushPinY,
				OLGX_ERASE | OLGX_PUSHPIN_OUT);
			mInfo->pinIn = False;
		}

		switch (status)
		{
		case S_ACTIVATE:
			/* Highlight the button if it isn't lit. */
			if ((mInfo->litButton != *bindex))
			{
				if (mInfo->childActive)
					unmapChildren(dpy, mInfo);

				UnlightButton(dpy, mInfo->menu,
					      mInfo->litButton);
				lightButton(dpy, mInfo->menu, *bindex);
				mInfo->litButton = *bindex;
			}
			else if(mInfo->menu->buttons[*bindex].stacked &&
				(mInfo->childActive == False))
			{
				/* Stacked button selected.
				 * This is not considered a complete
				 * action.
				 */
				mInfo->childActive = True;
				mInfo->childMenu = 
				(Menu *)(mInfo->menu->buttons[*bindex].
					 action.submenu);
				 rstatus = recursiveMenuShow(dpy, 
					 mInfo->childMenu, 
					 pevent->xbutton.x,
					 mInfo->menu->y +
					 mInfo->menu->buttons[*bindex].activeY +
					 ((mInfo->menu->title != NULL) ? 
						mInfo->menu->titleHeight : 0));

					/* Set bindex to -1 because this
					 * event just activated a menu
					 * and is not a button for a
					 * callback.
					 */
				*bindex = -1;
					
			}
			break;  /* S_ACTIVATE */

		case S_ONBUTTON:
			/* If this button isn't highlighted,
			 * and this menu has submenu displayed
			 * erase the submenu.  This is because we moved
			 * off a button, (and onto this one),
			 * which had created a submenu
			 * and we don't want it displayed any longer.
			 */
			if ((mInfo->litButton != *bindex) &&
			    (mInfo->childActive))
				unmapChildren(dpy, mInfo);

			/* Highlight the button if it isn't already lit. */
			if (mInfo->litButton != *bindex)
			{
				UnlightButton(dpy, mInfo->menu,
					      mInfo->litButton);
				lightButton(dpy, mInfo->menu, *bindex);
			}

			mInfo->litButton = *bindex;
			break;

		case S_ONPIN:
			if (!mInfo->pinIn)
			{
				olgx_draw_pushpin(olgx_gismisc, mInfo->menu->window,
					mInfo->menu->pushPinX,
					mInfo->menu->pushPinY,
					OLGX_ERASE | OLGX_PUSHPIN_IN);
			}
			mInfo->pinIn = True;
			/* Fall through so any submenus or highlighted
			 * buttons are erased. */


		case S_ONMENU:	/* Non-button area of menu. */
		case S_OFFMENU:	/* Off of menu entirely. */
			if (mInfo->childActive)
				unmapChildren(dpy, mInfo);

			/* unhighlight any hilit button */
			if (mInfo->litButton != -1)
				UnlightButton(dpy, mInfo->menu,
					      mInfo->litButton);
			mInfo->litButton = -1;
			break;

		}  /* End switch */
		break;

	case ButtonRelease:
		if (eventWindow != 0)
			*bindex = checkMenuDragButton(mInfo->menu, 
						      pevent, &status);
		else
			{
			*bindex = -1;
			status = S_OFFMENU;
			}

		if (status == S_ONMENU)
		{
			if (mInfo->childActive)
				unmapChildren(dpy, mInfo);

			/* Unhighlight any highlit button. */
			if (mInfo->litButton != -1)
				UnlightButton(dpy, mInfo->menu,
					      mInfo->litButton);
			mInfo->litButton = -1;
		}

		mInfo->litButton = *bindex;
		rstatus = (status == S_ONPIN) ? S_ONPIN : S_COMPLETE;
		break;
	}

	/* If rstatus == 0 that means no button events happened in
	 * this level of calls to menuEvent.  It is possible that
	 * a submenu was created by a call to recursiveMenuShow and that
	 * something happened in that or some lower level submenu.
	 * If the user chose something in a submenu the callback 
	 * routine would have been set.
	 * Check for just such an occurance here.  If it happened
	 * set the return status to S_ACTIVATE.
	 */
	if ((rstatus == 0) && (callback != notSet))
		{
		rstatus = S_ACTIVATE;
		}

	return rstatus;

}  /* End dragMenuEvent */


/* 
 * clickMenuEvent -- handle an event on a click menu.
 *
 * Purpose:
 *
 * 	This routine looks at the event passed to it and takes
 *	the appropriate action for that event.  
 *	It will highlight/unhighlight any buttons, change
 *	the state of the pushpin, or activate any submenus.
 *
 * Input:
 *
 *	dpy 	- The display on which this window will be drawn.
 *	pevent	- The event we will handle.
 *	mInfo	- Information about this menu.
 *
 * Output:
 *
 *	bindex	- Index of the button pressed.  If no button was
 *		  pressed this is set to -1.
 *
 *	Returns a status of the event on the menu.  S_ONPIN,
 *	S_COMPLETE, or 0 (meaning no action necessary).
 */
static int
clickMenuEvent(dpy, pevent, mInfo, bindex)
Display		*dpy;
XEvent		*pevent;
MenuInfo	*mInfo;
int		*bindex;
{
	int	status;
	int	rstatus = 0;	/* Event requires no response, if
				 * rstatus is 0.
				 */

	*bindex = -1;

	if (pevent->type == ButtonPress)
	{
		*bindex = checkMenuClickButton(mInfo->menu, pevent, &status);


		if((status == S_ACTIVATE) &&
		   mInfo->menu->buttons[*bindex].stacked)
		{
			unmapChildren(dpy, mInfo);

			/* Unhighlight any highlit button. */
			if (mInfo->litButton != -1)
				UnlightButton(dpy, mInfo->menu,
					      mInfo->litButton);

			/* Highlight the new button. */
			lightButton(dpy, mInfo->menu, *bindex);
			mInfo->litButton = *bindex;

			/* Stacked button selected.
			 * This is not considered a complete
			 * action.
			 */
			mInfo->childActive = True;
			mInfo->childMenu = 
			(Menu *)(mInfo->menu->buttons[*bindex].action.submenu);
			rstatus = recursiveMenuShow(dpy, mInfo->childMenu, 
				 mInfo->menu->buttons[*bindex].activeX +
				 mInfo->menu->buttons[*bindex].activeW +
				 mInfo->menu->x,
				 mInfo->menu->y +
				 mInfo->menu->buttons[*bindex].activeY +
				 ((mInfo->menu->title != NULL) ? 
					mInfo->menu->titleHeight : 0));

			/* Set bindex to -1 because this button
			 * is not for a callback.
			 */
			*bindex = -1;
		}
		else if (status == S_ONPIN)
		{
			mInfo->pinIn = True;
			olgx_draw_pushpin(olgx_gismisc, mInfo->menu->window,
				mInfo->menu->pushPinX,
				mInfo->menu->pushPinY,
				OLGX_PUSHPIN_IN | OLGX_ERASE);
			
			rstatus = S_ONPIN;
		}
		/* If a button has been activated, highlight it
		 * before the menu disappears.  This means a user
		 * has clicked on a button which doesn't have a
		 * submenu, but sets a callback.  e.g. Refresh
		 */
		else if (status == S_ACTIVATE)
		{
			/* Unhighlight any highlit button. */
			if (mInfo->litButton != -1)
				UnlightButton(dpy, mInfo->menu,
					      mInfo->litButton);

			/* Highlight the new button. */
			lightButton(dpy, mInfo->menu, *bindex);
			mInfo->litButton = *bindex;

			rstatus =  S_COMPLETE;
		}
		else if (status == S_COMPLETE)
			rstatus =  S_COMPLETE;

		/* Else we don't need
		 * to do anything.  rstatus stays 0.
		 */

	}  /* End if event is ButtonPress */
	else
		DebugEvent(pevent, "Unexpected in clickMenuEvent");

	/* If rstatus == 0 that means no button events happened in
	 * this level of calls to menuEvent.  It is possible that
	 * a submenu was created by a call to recursiveMenuShow and that
	 * something happened in that or some lower level submenu.
	 * If the user chose something in a submenu the callback 
	 * routine would have been set.
	 * Check for just such an occurance here.  If it happened
	 * set the return status to S_ACTIVATE.
	 */
	if ((rstatus == 0) && (callback != notSet))
		{
		rstatus = S_ACTIVATE;
		}

	return rstatus;

}  /* End clickMenuEvent */

static 
showMenu(dpy, menu, x, y)
Display		*dpy;
Menu		*menu;
int		x, y;
{
	int		dpyWidth, dpyHeight;
	MenuInfo	*menuInfo;
#ifdef SHADOW
	XWindowChanges	values;
#endif /* SHADOW */

	/* Insert the menu in the menu database. */
	/* return value is unused, but allocMenuInfo modifies
	 * a global (to this file) database: menuTable[]
	 */
	menuInfo = allocMenuInfo(menu);

	/* if menu has a default, line default button with current y;
	 * otherwise line first button of menu up with current y. 
	 */
	if (menu->buttonDefault > 0)
		y -= menu->buttonDefault * (BUTT_HEIGHT + BUTT_VSPACE);
	if (menu->title != NULL)
		y -= menu->titleHeight;

	/* Make sure the menu is going to fit on the screen. */
	dpyWidth = DisplayWidth(dpy, DefaultScreen(dpy));
	dpyHeight = DisplayHeight(dpy, DefaultScreen(dpy));
	if ((x + menu->width) > dpyWidth)
		x = dpyWidth - menu->width;

	if ((y + menu->height) > dpyHeight)
		y = dpyHeight - menu->height;

	if (y < 0)
		y = 0;

	/* Move the menu window to position. */
	XMoveWindow(dpy, menu->window, x, y);
#ifdef SHADOW
	XMoveWindow(dpy, menu->shadow, x + MENUSHADOW_OFF, y + MENUSHADOW_OFF);
#endif /* SHADOW */
	menu->x = x;
	menu->y = y;

	/* Map the menu window and its shadow.
	 * The OLWM designers want to see the menu appear first and
	 * then the shadow, NOT the shadow and then the menu.
	 * So, we have to mess around a bit to do this.
	 * To make the menu appear first, and then the shadow
	 * under it, we have to map the menu first and then
	 * change the stacking order before mapping the shadow.
	 */
	XMapRaised(dpy, menu->window);

#ifdef SHADOW
	values.sibling = menu->window;
	values.stack_mode = Below;
	XConfigureWindow(dpy, menu->shadow, CWStackMode|CWSibling, &values);
	XMapWindow(dpy, menu->shadow);
#endif /* SHADOW */

	DrawMenu(dpy, menu);
}

/*
 * PointInRect	-- check if a point is inside a rectangle
 */
int
PointInRect(x, y, rx, ry, rw, rh)
int x, y, rx, ry, rw, rh;
{
	if (((x)>(rx)) && ((x)<(rx)+(rw)) && ((y)>(ry)) && ((y)<(ry)+(rh)))
		return 1;
	else
		return 0;
}


/*
 * checkMenuDragButton	-- check a button event against a drag right menu.
 *	menu	- pointer to menu
 *	event	- pointer button event
 *      status  - pointer to storage for current status
 * sets status to S_ONMENU, S_ACTIVATE, S_ONPIN or S_OFFMENU.
 * returns index of current button or -1 if button is not appropriate
 */
static int
checkMenuDragButton(menu, bevent, status)
Menu	*menu;
XEvent	*bevent;
int	*status;
{
	int	i;
	int	yoff = 0;

	if (menu->title != NULL)
		yoff = menu->titleHeight;

	/* If the event window is not the menu window */
	if (bevent->xbutton.subwindow != menu->window)
	{
		*status = S_OFFMENU;
		return -1;
	}
	
	/* Check the event coordinates against each of the buttons.
	 * Since the button event is reported relative to root window
	 * it must be adjusted for the check.
	 */
	*status = S_ACTIVATE;
	for (i=0; i < menu->buttonCount; i++)
	{
		if (PointInRect(bevent->xbutton.x - menu->x, 
				bevent->xbutton.y - menu->y,
				menu->buttons[i].activeX,
				menu->buttons[i].activeY + yoff,
				menu->buttons[i].activeW,
				menu->buttons[i].activeH))
			return i;
	}

	/* Check the pushpin area. */
	if (menu->hasPushPin &&
	    PointInRect(bevent->xbutton.x - menu->x, 
			bevent->xbutton.y - menu->y,
			menu->pushPinX, menu->pushPinY,
			pushpin_width, pushpin_height) &&
	    !menu->currentlyDisplayed)
		*status = S_ONPIN;
	else
		*status = S_ONMENU;

	return -1;
}


/*
 * checkClickMenuButton	-- check a button event against a click menu.
 *	menu	- pointer to menu
 *	event	- pointer button event
 *      status  - pointer to storage for current status
 * sets status to S_ONMENU, S_ACTIVATE, or S_ONPIN.
 * returns index of current button or -1 if button is not appropriate
 */
static int
checkMenuClickButton(menu, bevent, status)
Menu	*menu;
XEvent	*bevent;
int	*status;
{
	int	i;
	int	yoff = 0;

	if (menu->title != NULL)
		yoff = menu->titleHeight;

	/* Check the event coordinates against each of the buttons.
	 * Unlike checkMenuDragButton, the button press event is
	 * reported relative to the menu window so we DO NOT need
	 * to adjust the coordinates of the button press event.
	 */
	*status = S_ACTIVATE;
	for (i=0; i < menu->buttonCount; i++)
	{
		if (PointInRect(bevent->xbutton.x, bevent->xbutton.y,
				menu->buttons[i].activeX,
				menu->buttons[i].activeY + yoff,
				menu->buttons[i].activeW,
				menu->buttons[i].activeH))
			return i;
	}

	/* Check the pushpin area. */
	if (menu->hasPushPin &&
	    PointInRect(bevent->xbutton.x, bevent->xbutton.y,
			menu->pushPinX, menu->pushPinY,
			pushpin_width, pushpin_height) &&
	    !menu->currentlyDisplayed)
		*status = S_ONPIN;
	else
		*status = S_ONMENU;

	return -1;
}

/*
 * checkMenuMotion	-- check a motion event against a menu
 *	menu	- pointer to menu
 *	mevent	- pointer motion event
 *	status	- pointer to storage for current status
 * sets status to S_ONBUTTON, S_ONMENU, S_ACTIVATE, S_ONPIN or S_OFFMENU.
 * returns index of current button or -1 if button is not appropriate
 */
static
checkMenuMotion(menu, mevent, status)
Menu	*menu;
XEvent	*mevent;
int	*status;
{
	int	i;
	int	yoff = 0;

	if (menu->title != NULL)
		yoff = menu->titleHeight;

	/* If the event window is not the menu window. */
	if (mevent->xmotion.subwindow != menu->window)
	{
		*status = S_OFFMENU;
		return -1;
	}
	
	/* Check the event coordinates against each of the buttons.
	 * Since the button event is reported relative to root window
	 * it must be adjusted for the check.
	 */
	for (i=0; i < menu->buttonCount; i++)
	{
		if (PointInRect(mevent->xmotion.x - menu->x, 
				mevent->xmotion.y - menu->y,
				menu->buttons[i].activeX,
				menu->buttons[i].activeY + yoff,
				menu->buttons[i].activeW,
				menu->buttons[i].activeH))
		{
			/* Event is in a button.
			 * Is it a button stack and if so,
			 * is it in the right half of the button?
			 */
			if ((menu->buttons[i].stacked) &&
			    ((mevent->xmotion.x - menu->x) > (menu->width/2)))
				*status = S_ACTIVATE;
			else
				*status = S_ONBUTTON;
			return i;
		}
	}

	/* Check the pushpin area. */
	if (menu->hasPushPin &&
	    PointInRect(mevent->xmotion.x - menu->x, 
			mevent->xmotion.y - menu->y,
			menu->pushPinX, menu->pushPinY,
			pushpin_width, pushpin_height) &&
	    !menu->currentlyDisplayed)
		*status = S_ONPIN;
	else
		*status = S_ONMENU;

	return -1;
}

/* Draw a normal (black on white) button. */
UnlightButton(dpy, menu, idx)
Display	*dpy;
Menu	*menu;
int	idx;
{
	int	yoff = 0;

	if (idx == -1)
		return;

	if (menu->title != NULL)
		yoff = menu->titleHeight;
	else
		yoff = HEAD_VSPACE;

	/* Draw an unselected button (normal). */
	drawButton(dpy, menu->window, &menu->buttons[idx], yoff, 
		(idx == menu->buttonDefault),(menu->originalMenu != NULL));
}

/* Draw a highlighted (white on black) button. */
lightButton(dpy, menu, idx)
Display	*dpy;
Menu	*menu;
int	idx;
{
	int	yoff = 0;

	if (idx == -1)
		return;

	if (menu->title != NULL)
		yoff = menu->titleHeight;
	else
		yoff = HEAD_VSPACE;

	/* Draw a highlighted button. */
	drawRevButton(dpy, menu->window, &menu->buttons[idx], yoff);
}
