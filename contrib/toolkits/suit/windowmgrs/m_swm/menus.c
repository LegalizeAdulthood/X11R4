/*
 *	Original Source: ./contrib/windowmgrs/twm/menus.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*****************************************************************************/
/**       Copyright 1988 by Evans & Sutherland Computer Corporation,        **/
/**                          Salt Lake City, Utah                           **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    name  of Evans & Sutherland  not be used in advertising or publi-    **/
/**    city pertaining to distribution  of the software without  specif-    **/
/**    ic, written prior permission.                                        **/
/**                                                                         **/
/**    EVANS  & SUTHERLAND  DISCLAIMS  ALL  WARRANTIES  WITH  REGARD  TO    **/
/**    THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILI-    **/
/**    TY AND FITNESS, IN NO EVENT SHALL EVANS &  SUTHERLAND  BE  LIABLE    **/
/**    FOR  ANY  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY  DAM-    **/
/**    AGES  WHATSOEVER RESULTING FROM  LOSS OF USE,  DATA  OR  PROFITS,    **/
/**    WHETHER   IN  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS    **/
/**    ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE  OR PER-    **/
/**    FORMANCE OF THIS SOFTWARE.                                           **/
/*****************************************************************************/

/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/***********************************************************************
 *
 * $Header: menus.c,v 1.2 88/10/18 11:36:25 jim Exp $
 *
 * swm menu code
 *
 * 17-Nov-87 Thomas E. LaStrange		File created
 *
 ***********************************************************************/

#ifndef lint
static char RCSinfo[] =
"$Header: menus.c,v 1.2 88/10/18 11:36:25 jim Exp $";
#endif

#include <stdio.h>
#include <signal.h>
#include "swm.h"
#include "gc.h"
#include "menus.h"
#include "events.h"
#include "util.h"
#include "gram.h"
#include "pull.bm"

#ifdef macII
#define vfork fork
#endif

int RootFunction = NULL;
MenuRoot *MenuList = NULL;		/* head of the menu list */
MenuRoot *LastMenu = NULL;		/* the last menu */
MenuRoot *ActiveMenu = NULL;		/* the active menu */
MenuRoot *Windows = NULL;		/* the SwmWindows menu */
MenuItem *ActiveItem = NULL;		/* the active menu item */
MouseButton Mouse[MAX_BUTTONS+1][NUM_CONTEXTS][MOD_SIZE];
MouseButton DefaultFunction;
MouseButton WindowFunction;
FuncKey FuncKeyRoot;			/* head of function key list */
int MoveFunction;			/* either F_MOVE or F_FORCEMOVE */

/***********************************************************************
 *
 *  Procedure:
 *	InitMenus - initialize menu roots
 *
 ***********************************************************************
 */

void
InitMenus()
{
    int i, j, k;
    FuncKey *key, *tmp;

    for (i = 0; i < MAX_BUTTONS+1; i++)
	for (j = 0; j < NUM_CONTEXTS; j++)
	    for (k = 0; k < MOD_SIZE; k++)
	    {
		Mouse[i][j][k].func = NULL;
		Mouse[i][j][k].item = NULL;
	    }

    DefaultFunction.func = NULL;
    WindowFunction.func = NULL;

    Mouse[1][C_TITLE][0].func = F_RAISE;
    Mouse[2][C_TITLE][0].func = F_MOVE;
    Mouse[3][C_TITLE][0].func = F_LOWER;

    Mouse[1][C_ICON][0].func = F_ICONIFY;
    Mouse[2][C_ICON][0].func = F_MOVE;
    Mouse[3][C_ICON][0].func = F_LOWER;

    for (key = FuncKeyRoot.next; key != NULL;)
    {
	free(key->name);
	tmp = key;
	key = key->next;
	free(tmp);
    }
    FuncKeyRoot.next = NULL;

}

/***********************************************************************
 *
 *  Procedure:
 *	AddFuncKey - add a function key to the list
 *
 *  Inputs:
 *	name	- the name of the key
 *	cont	- the context to look for the key press in
 *	mods	- modifier keys that need to be pressed
 *	func	- the function to perform
 *	win_name- the window name (if any)
 *	action	- the action string associated with the function (if any)
 *
 ***********************************************************************
 */

void
AddFuncKey(name, cont, mods, func, win_name, action)
    char *name;
    int cont, mods, func;
    char *win_name;
    char *action;
{
    FuncKey *tmp;
    KeySym keysym;

    if ((keysym = XStringToKeysym(name)) == NoSymbol)
    {
	fprintf(stderr, "swm: unknown key name \"%s\"\n", name);
	return;
    }

    /* see if there already is a key defined for this context */
    for (tmp = FuncKeyRoot.next; tmp != NULL; tmp = tmp->next)
    {
	if (tmp->keysym == keysym &&
	    tmp->cont == cont &&
	    tmp->mods == mods)
	    break;
    }

    if (tmp == NULL)
    {
	tmp = (FuncKey *) malloc(sizeof(FuncKey));
	tmp->next = FuncKeyRoot.next;
	FuncKeyRoot.next = tmp;
    }

    tmp->name = name;
    tmp->keysym = keysym;
    tmp->keycode = XKeysymToKeycode(dpy, keysym);
    tmp->cont = cont;
    tmp->mods = mods;
    tmp->func = func;
    tmp->win_name = win_name;
    tmp->action = action;
}

/***********************************************************************
 *
 *  Procedure:
 *	NewMenuRoot - create a new menu root
 *
 *  Returned Value:
 *	(MenuRoot *)
 *
 *  Inputs:
 *	name	- the name of the menu root
 *
 ***********************************************************************
 */

MenuRoot *
NewMenuRoot(name)
    char *name;
{
    MenuRoot *tmp;
    unsigned long valuemask;
    XSetWindowAttributes attributes;

    CreateGCs();

    tmp = (MenuRoot *) malloc(sizeof(MenuRoot));
    tmp->name = name;
    tmp->prev = NULL;
    tmp->first = NULL;
    tmp->last = NULL;
    tmp->items = 0;
    tmp->width = 0;
    tmp->mapped = FALSE;
    tmp->pull = FALSE;
    tmp->active = TRUE;
    tmp->shadow = XCreateSimpleWindow(dpy, Root,
	0, 0, 10, 10, 1, MenuShadowColor, MenuShadowColor);
    tmp->w = XCreateSimpleWindow(dpy, Root,
	0, 0, 10, 10, 1, MenuC.fore, MenuC.back);
    XSelectInput(dpy, tmp->w, LeaveWindowMask);

    if (SaveUnder)
    {
	valuemask = CWSaveUnder;
	attributes.save_under = True;
	XChangeWindowAttributes(dpy, tmp->shadow, valuemask, &attributes);
	XChangeWindowAttributes(dpy, tmp->w, valuemask, &attributes);
    }

    if (MenuList == NULL)
    {
	MenuList = tmp;
	MenuList->next = NULL;
    }

    if (LastMenu == NULL)
    {
	LastMenu = tmp;
	LastMenu->next = NULL;
    }
    else
    {
	LastMenu->next = tmp;
	LastMenu = tmp;
	LastMenu->next = NULL;
    }

    if (strcmp(name, SWM_WINDOWS) == 0)
	Windows = tmp;

    return (tmp);
}

/***********************************************************************
 *
 *  Procedure:
 *	AddToMenu - add an item to a root menu
 *
 *  Returned Value:
 *	(MenuItem *)
 *
 *  Inputs:
 *	menu	- pointer to the root menu to add the item
 *	item	- the text to appear in the menu
 *	action	- the string to possibly execute
 *	sub	- the menu root if it is a pull-right entry
 *	func	- the numeric function
 *
 ***********************************************************************
 */

MenuItem *
AddToMenu(menu, item, action, sub, func)
    MenuRoot *menu;
    char *item, *action;
    MenuRoot *sub;
    int func;
{
    unsigned long valuemask;
    XSetWindowAttributes attributes;
    MenuItem *tmp;
    int width;

#ifdef DEBUG
    fprintf(stderr, "adding menu item=\"%s\", action=%s, sub=%d, f=%d\n",
	item, action, sub, func);
#endif

    tmp = (MenuItem *) malloc(sizeof(MenuItem));
    tmp->root = menu;

    if (menu->first == NULL)
    {
	menu->first = tmp;
	tmp->prev = NULL;
    }
    else
    {
	menu->last->next = tmp;
	tmp->prev = menu->last;
    }
    menu->last = tmp;

    tmp->item = item;
    tmp->action = action;
    tmp->next = NULL;
    tmp->sub = NULL;
    tmp->pull = NULL;
    tmp->state = 0;
    tmp->func = func;

    width = XTextWidth(MenuFont.font, item, strlen(item));
    if (width <= 0)
	width = 1;
    if (width > menu->width)
	menu->width = width;

    if (tmp->func != F_TITLE)
    {
	tmp->w = XCreateSimpleWindow(dpy, menu->w,
	    0, menu->items * (MenuFont.height + 4),
	    width, MenuFont.height + 4,
	    0,
	    MenuC.fore, MenuC.back);
	XSelectInput(dpy, tmp->w, EnterWindowMask
	    | LeaveWindowMask | ExposureMask);
    }
    else
    {
	tmp->w = XCreateSimpleWindow(dpy, menu->w,
	    -1, menu->items * (MenuFont.height + 4),
	    width, MenuFont.height + 2,
	    1,
	    MenuC.fore, MenuTitleC.back);
	XSelectInput(dpy, tmp->w, ExposureMask);
    }

    if (sub != NULL)
    {
	Pixmap pm;

	tmp->sub = sub;
	pm = MakeCenteredPixmap(tmp->w, MenuNormalGC,
	    pull_width, MenuFont.height + 4,
	    pull_bits, pull_width, pull_height);

	valuemask = CWEventMask | CWBackPixmap;
	attributes.background_pixmap = pm;
	attributes.event_mask = EnterWindowMask | LeaveWindowMask;

	tmp->pull = XCreateWindow(dpy, tmp->w,
	    0, 0,
	    pull_width, MenuFont.height + 4,
	    0, d_depth, CopyFromParent,
	    d_visual, valuemask, &attributes);

	XMapWindow(dpy, tmp->pull);

	menu->pull = TRUE;
	XSaveContext(dpy, tmp->pull, MenuContext, tmp);
    }
    menu->items += 1;

    XSaveContext(dpy, tmp->w, MenuContext, tmp);

    if (menu->items == 1)
	XSaveContext(dpy, tmp->root->w, MenuContext, tmp);

    return (tmp);
}

/***********************************************************************
 *
 *  Procedure:
 *	PopUpMenu - pop up a pull down menu
 *
 *  Inputs:
 *	menu	- the root pointer of the menu to pop up
 *	x	- the x location of the mouse
 *	y	- the y location of the mouse
 *
 ***********************************************************************
 */

void
PopUpMenu(menu, x, y)
    MenuRoot *menu;
    int x, y;
{
    unsigned long valuemask;
    XSetWindowAttributes attributes;
    int m_height;
    XWindowChanges xwc, pwc;
    unsigned int xwcm, pwcm;
    MenuItem *tmp, *tmp1;
    SwmWindow *tmp_win;

    if (menu == NULL)
	return;

    if (menu == Windows)
    {
	/* this is the swm windows menu,  let's go ahead and build it */

	for (tmp = menu->first; tmp->next != NULL;)
	{
	    XDeleteContext(dpy, tmp->w, MenuContext);
	    XDestroyWindow(dpy, tmp->w);

	    tmp1 = tmp;
	    tmp = tmp->next;
	    free(tmp1);
	}

	if (ActiveMenu != NULL)
	    menu->prev = ActiveMenu;
	else
	    menu->prev = NULL;
	menu->first = NULL;
	menu->last = NULL;
	menu->items = 0;
	menu->width = 0;
	menu->mapped = FALSE;
	menu->pull = FALSE;

	AddToMenu(menu, "SWM Windows", NULL, NULL, F_TITLE);
	for (tmp_win = SwmRoot.next; tmp_win != NULL; tmp_win = tmp_win->next)
	{
	    AddToMenu(menu, tmp_win->window_name, tmp_win, NULL, F_POPUP);
	}
    }

    if (menu->items == 0)
	return;

    XGrabPointer(dpy, Root, True,
	ButtonReleaseMask,
	GrabModeAsync, GrabModeAsync,
	Root, LeftArrowCursor, CurrentTime);

    if (ActiveMenu != NULL)
	ActiveMenu->active = FALSE;

    menu->active = TRUE;
    ActiveMenu = menu;

    if (menu->mapped != TRUE)
    {
	if (menu->pull == TRUE)
	{
	    menu->width += pull_width + 10;
	}

	xwcm = 0;
	xwcm |= CWWidth;
	xwc.width = menu->width + 10;

	pwcm = 0;
	pwcm |= CWX;
	pwc.x = xwc.width - pull_width;

	valuemask = CWBackingStore;
	attributes.backing_store = Always;

	for (tmp = menu->first; tmp != NULL; tmp = tmp->next)
	{
	    if (BackingStore)
		XChangeWindowAttributes(dpy, tmp->w, valuemask, &attributes);

	    XConfigureWindow(dpy, tmp->w, xwcm, &xwc);
	    if (tmp->pull != NULL)
	    {
		XConfigureWindow(dpy, tmp->pull, pwcm, &pwc);
	    }
	    if (tmp->func != F_TITLE)
		tmp->y = 5;
	    else
	    {
		tmp->y = xwc.width - XTextWidth(MenuFont.font, tmp->item,
		    strlen(tmp->item));
		tmp->y /= 2;
	    }
	}
    }
    menu->mapped = TRUE;

    m_height = menu->items * (MenuFont.height + 4);

    if ((x + 10) > MyDisplayWidth)
	x = (MyDisplayWidth - 30);

    if ((y + m_height + 10) > MyDisplayHeight)
	y = (MyDisplayHeight - m_height);

    xwcm = CWX | CWY | CWWidth | CWHeight;

    xwc.x = x - menu->width + 10;
    if (xwc.x < 0)
	xwc.x = 0;
    xwc.y = y - ((MenuFont.height + 4) / 2);
    xwc.width = menu->width + 10;
    xwc.height = m_height;

    XConfigureWindow(dpy, menu->w, xwcm, &xwc);

    xwc.x = xwc.x + 5;
    xwc.y = xwc.y + 5;

    XConfigureWindow(dpy, menu->shadow, xwcm, &xwc);
    XWarpPointer(dpy, None, menu->w, 0, 0, 0, 0, 
     menu->width - 10, (MenuFont.height + 4) / 2);
    XMapSubwindows(dpy, menu->w);
    XRaiseWindow(dpy, menu->shadow);
    XMapRaised(dpy, menu->w);
    XMapWindow(dpy, menu->shadow);
}

/***********************************************************************
 *
 *  Procedure:
 *	FindMenuRoot - look for a menu root
 *
 *  Returned Value:
 *	(MenuRoot *)  - a pointer to the menu root structure 
 *
 *  Inputs:
 *	name	- the name of the menu root 
 *
 ***********************************************************************
 */

MenuRoot *
FindMenuRoot(name)
    char *name;
{
    MenuRoot *tmp;

    for (tmp = MenuList; tmp != NULL; tmp = tmp->next)
    {
	if (strcmp(name, tmp->name) == 0)
	    return (tmp);
    }
    return NULL;
}

/***********************************************************************
 *
 *  Procedure:
 *	ExecuteFunction - execute a swm root function
 *
 *  Inputs:
 *	func	- the function to execute
 *	action	- the menu action to execute 
 *	w	- the window to execute this function on
 *	tmp_win	- the swm window structure
 *	event	- the event that caused the function
 *	context - the context in which the button was pressed
 *	pulldown- flag indicating execution from pull down menu
 *
 ***********************************************************************
 */

void
ExecuteFunction(func, action, w, tmp_win, event, context, pulldown)
    int func;
    char *action;
    Window w;
    SwmWindow *tmp_win;
    XEvent event;
    int context;
    int pulldown;
{
    static Time last_time = 0;

    char tmp[200];
    char *ptr;
    int len;
    char buff[MAX_FILE_SIZE];
    int count, fd;
    MenuRoot *root, *tmp_root;
    MenuItem *item, *tmp_item;

    Window myParent;
    SwmWindow *t;
    SwmWindow *desktop;
    int  did;
    Window desktopWindow;
    XWindowAttributes info;
    Window junkroot, parent, *children;
    Window newParent;
    Window junkFrame, junkWindow;
    int i, nchildren;
    int rx, ry, rw, rh, rb, rd;
    int junkFx, junkFy, addFx, addFy;
    int addingX, addingY;

    XGrabPointer(dpy, Root, True,
	ButtonReleaseMask,
	GrabModeAsync, GrabModeAsync,
	Root, ClockCursor, CurrentTime);

    switch (func)
    {
    case F_NOP:
    case F_TITLE:
	break;

    case F_DESKTOP:

        XQueryPointer(dpy,Root,&JunkRoot,&junkFrame,&JunkX,&JunkY,
                &addingX,&addingY,&JunkMask);
        XQueryPointer(dpy,junkFrame,&JunkRoot,&junkWindow,&junkFx,&junkFy,
                &addFx,&addFy,&JunkMask);
        XGetGeometry(dpy, junkWindow, &JunkRoot, &rx, &ry, &rw, &rh, &rb, &rd);
        XGetWindowAttributes(dpy,junkWindow,&info);
        desktopWindow = XCreateSimpleWindow(dpy,junkWindow,
                addFx-rx,addFy-ry,
                MyDisplayWidth/2, MyDisplayHeight/2,
                info.border_width,
                Black,DefaultC.back);
        XReparentWindow(dpy,desktopWindow, junkWindow,
                 addingX,addingY);
        XSetStandardProperties(dpy,desktopWindow,"desktop","desktop",
                None,NULL,0,NULL);
        XSelectInput(dpy, desktopWindow,EnterWindowMask | LeaveWindowMask |
                SubstructureRedirectMask | KeyPressMask | ButtonPressMask |
                ButtonReleaseMask | ExposureMask | StructureNotifyMask);
        Desktop =1;
        XQueryTree(dpy, junkWindow, &root, &parent, &children, &nchildren);
        XQueryTree(dpy, desktopWindow, &root, &parent, &children, &nchildren);
        NewParent1 = parent;
        SuitWindow(desktopWindow);
        return;
	break;

    case F_HIDELIST:
        /* No Operation */
	break;

    case F_BEEP:
	XBell(dpy, screen);
        
	break;

    case F_POPUP:
	tmp_win = (SwmWindow *)action;
	if (WindowFunction.func != NULL)
	{
	   ExecuteFunction(WindowFunction.func, WindowFunction.item->action,
	       w, tmp_win, event, C_FRAME, FALSE);
	}
	else
	{
	    DeIconify(tmp_win);
	    XMapWindow(dpy, tmp_win->w);
	    XMapRaised(dpy, tmp_win->frame);
	}
	break;

    case F_RESIZE:
	if (DeferExecution(context, func, MoveCursor))
	    return;

        if (context == 0)
           return;

	if (pulldown)
	    XWarpPointer(dpy, None, Root, 
		0, 0, 0, 0, event.xbutton.x_root, event.xbutton.y_root);

        if (w == tmp_win->dummyicon_w)
            return ;
        else if ((w != tmp_win->icon_w))
	{
	    EventHandler[EnterNotify] = HandleUnknown;
	    EventHandler[LeaveNotify] = HandleUnknown;
	    EventHandler[Expose] = HandleUnknown;
	    StartResize(event, tmp_win);
	    return;
	}
	break;


    case F_ZOOM:
	if (DeferExecution(context, func, DotCursor))
	    return;
        printf("F_ZOOM: TempContext Context %d, %d\n", TempContext, context);
        if (context != 0)
	   fullzoom(tmp_win, ZOOM_VERT);

	break;

    case F_FULLZOOM:
	if (DeferExecution(context, func, DotCursor))
	    return;
        if (context != 0)
	   fullzoom(tmp_win, ZOOM_FULL);
	break;

    case F_MOVE:
    case F_FORCEMOVE:

	if (DeferExecution(context, func, MoveCursor))
	    return;

        if (context == 0)
	    return;

	MoveFunction = func;

	 if (pulldown)
	    XWarpPointer(dpy, None, Root, 
		0, 0, 0, 0, event.xbutton.x_root, event.xbutton.y_root); 

        /* if (context == C_ICON)
           {XEvent revent;
           XNextEvent(dpy, revent);
           } */

	EventHandler[EnterNotify] = HandleUnknown;
	EventHandler[LeaveNotify] = HandleUnknown;
	EventHandler[Expose] = HandleUnknown;

	/* redraw the text in the title bar or the icon window if
	 * needed, we have disabled expose event handling so we must
	 * do it here
	 */
	if (context == C_TITLE)
	{
            DrawWindowName(tmp_win);
	}
	else if (context == C_ICON)
	{
	    XDrawImageString(dpy, tmp_win->icon_label,
		IconNormalGC,
		tmp_win->icon_x, tmp_win->icon_y,
		tmp_win->icon_name, strlen(tmp_win->icon_name));
	}

	XGrabServer(dpy);

	XGrabPointer(dpy, event.xbutton.root, True,
	    ButtonReleaseMask,
	    GrabModeAsync, GrabModeAsync,
	    Root, MoveCursor, CurrentTime);

	if (context == C_ICON)
 	    w = tmp_win->icon_w;
	else if (w == tmp_win->dummyicon_w)
	    return;
        else if (w != tmp_win->icon_w)
	    w = tmp_win->frame;

	DragX = event.xbutton.x;
	DragY = event.xbutton.y;

	if (context == C_WINDOW)
	    DragY += tmp_win->title_height;
		 
	DragWindow = w;


	XGetGeometry(dpy, w, &JunkRoot, &JunkX, &JunkY,
	    &DragWidth, &DragHeight, &JunkBW,
	    &JunkDepth);

        MoveOutline(Root,
	    event.xbutton.x_root-DragX-BorderWidth,
	    event.xbutton.y_root-DragY-BorderWidth,
	    DragWidth + 2 * BorderWidth,
	    DragHeight + 2 * BorderWidth);

	if ((event.xbutton.time - last_time) < 400)
	{
	    int width, height;

	    ConstMove = TRUE;
	    ConstMoveDir = MOVE_NONE;
	    ConstMoveX = event.xbutton.x_root - DragX - BorderWidth;
	    ConstMoveY = event.xbutton.y_root - DragY - BorderWidth;
	    width = DragWidth + 2 * BorderWidth;
	    height = DragHeight + 2 * BorderWidth;
	    ConstMoveXL = ConstMoveX + width/3;
	    ConstMoveXR = ConstMoveX + 2*(width/3);
	    ConstMoveYT = ConstMoveY + height/3;
	    ConstMoveYB = ConstMoveY + 2*(height/3);

	    XWarpPointer(dpy, None, DragWindow,
		0, 0, 0, 0, DragWidth/2, DragHeight/2);

	    XQueryPointer(dpy, DragWindow, &JunkRoot, &JunkChild,
		&JunkX, &JunkY, &DragX, &DragY, &JunkMask);
	}

	last_time = event.xbutton.time;
	return;
        break;

    case F_FUNCTION:
	{
	    MenuRoot *mroot;
	    MenuItem *mitem;

	    if ((mroot = FindMenuRoot(action)) == NULL)
	    {
		fprintf(stderr, "swm: couldn't find function \"%s\"\n", action);
		return;
	    }

	    if (NeedToDefer(mroot) && DeferExecution(context, func, DotCursor))
		return;
	    else
	    {
		for (mitem = mroot->first; mitem != NULL; mitem = mitem->next)
		{
		    ExecuteFunction(mitem->func, mitem->action, w, tmp_win,
			event, context, pulldown);
		}
	    }
	}
	break;

    case F_DEICONIFY:
    case F_ICONIFY:
	if (DeferExecution(context, func, DotCursor))
	    return;

        if (context == 0)
            return;

        TempContext = C_ICON;

	if (tmp_win->icon)
	{
	    DeIconify(tmp_win);
	}
        else if (func == F_ICONIFY)
	{
	    SwmWindow *t;

	    if (!tmp_win->iconified)
	    {
		int final_x, final_y;
                int rx, ry;

		if (tmp_win->wmhints &&
		    tmp_win->wmhints->flags & IconPositionHint)
		{
		    final_x = tmp_win->wmhints->icon_x;
		    final_y = tmp_win->wmhints->icon_y;
		}

                final_x=final_y=0;
                MoveIcon(&final_x, &final_y);

                if (SetIconTable(tmp_win, &final_x, &final_y))
                  break;

		XMoveWindow(dpy, tmp_win->icon_w, final_x, final_y);
                XMoveWindow(dpy, tmp_win->dummyicon_w, final_x, final_y);
		tmp_win->iconified = TRUE;
	    }

	    Iconify(tmp_win);
	}
	SetHints(tmp_win);
        SetNormal(tmp_win);
        TempContext = C_NO_CONTEXT;
	break;

    case F_RAISELOWER:

	if (DeferExecution(context, func, DotCursor))
	    return;

        if (context == 0)
           return;

	{
	    int vis;

	    if (w == tmp_win->icon_w)
		vis = tmp_win->icon_vis;
	    else
	    {
		w = tmp_win->frame;
		vis = tmp_win->frame_vis;
	    }

	    if (vis == VisibilityUnobscured)
		XLowerWindow(dpy, w);
	    else
		XRaiseWindow(dpy, w);
	}
	break;
	
    case F_RAISE:

	if (DeferExecution(context, func, DotCursor))
	    return;

        if (context == 0)
           return;

	if (w == tmp_win->icon_w)
	    XRaiseWindow(dpy, tmp_win->icon_w);
	else
	    XRaiseWindow(dpy, tmp_win->frame);

	break;

    case F_LOWER:

	if (DeferExecution(context, func, DotCursor))
	    return;

        if (context == 0)
           return;

	if (w == tmp_win->icon_w)
	    XLowerWindow(dpy, tmp_win->icon_w);
	else
	    XLowerWindow(dpy, tmp_win->frame);

	break;

    case F_FOCUS:
	if (DeferExecution(context, func, DotCursor))
	    return;

	if (tmp_win->icon == FALSE)
	{
	    if (Focus != NULL && Focus != tmp_win)
	    {
		if (Highlight && Focus->highlight)
		{
		    XSetWindowBorderPixmap(dpy, Focus->frame, GrayTile);
		    XSetWindowBorderPixmap(dpy, Focus->title_w, GrayTile);
		    XSetWindowBorderPixmap(dpy, Focus->w, GrayTile);
		}
                SetNormal(Focus);
	    }
            SetHilite(tmp_win);
	    XSetWindowBorder(dpy, tmp_win->frame, BorderColor);
	    XSetWindowBorder(dpy, tmp_win->title_w, BorderColor);
            XSetWindowBorder(dpy, tmp_win->w, BorderColor);
	    XSetInputFocus(dpy, tmp_win->w, RevertToPointerRoot,
		    CurrentTime);
	    FocusRoot = FALSE;
	    Focus = tmp_win;
	}
	break;

    case F_DESTROY:
	if (DeferExecution(context, func, SkullCursor))
            {
	    return;
            }

        if (context == 0)
           {
           XUngrabServer(dpy);
           return;
           }

        if (tmp_win->next != NULL)
           {
	    tmp_win->prev->next = tmp_win->next;
	    tmp_win->next->prev = tmp_win->prev;
            }
        else
            {
            tmp_win->prev->next = NULL; 
            }

        switch(tmp_win->windowFlag)
         {
         SwmWindow *t;
         case 0:
           free((char *)tmp_win);

           KillClient(tmp_win);
           KillDesktop(tmp_win);
           XKillClient(dpy, tmp_win->w);
           break;
         case 1:
           KillClient(tmp_win);
           DestroyWindow(tmp_win);
           XSync(dpy);
           break;
         default:
           if (tmp_win != 0)
              {
              KillClient(tmp_win);
              DestroyWindow(tmp_win);
              XSync(dpy);
              }
         }
	break;

    case F_CIRCLEUP:
        XCirculateSubwindowsUp(dpy, RootDesktop->w);
	break;

    case F_CIRCLEDOWN:
        XCirculateSubwindowsDown(dpy, RootDesktop->w);
	break;

    case F_VERSION:
	XMapRaised(dpy, VersionWindow);
	break;

    case F_EXEC:
	Execute(action);
	break;

    case F_UNFOCUS:
	FocusOnRoot();
	break;

    case F_CUT:
	strcpy(tmp, action);
	strcat(tmp, "\n");
	XStoreBytes(dpy, tmp, strlen(tmp));
	break;

    case F_CUTFILE:
	ptr = XFetchBytes(dpy, &count);
	if (count != 0)
	{
	    if (sscanf(ptr, "%s", tmp) == 1)
	    {
		ptr = ExpandFilename(tmp);
		fd = open(ptr, 0);
		if (fd >= 0)
		{
		    count = read(fd, buff, MAX_FILE_SIZE - 1);
		    if (count > 0)
			XStoreBytes(dpy, buff, count);

		    close(fd);
		}
		else
		{
		    fprintf(stderr, "swm: couldn't open \"%s\"\n", tmp);
		}
	    }
	    XFree(ptr);
	}
	else
	{
	    fprintf(stderr, "swm: nothing in the cut buffer\n");
	}
	break;

    case F_FILE:
	action = ExpandFilename(action);
	fd = open(action, 0);
	if (fd >= 0)
	{
	    count = read(fd, buff, MAX_FILE_SIZE - 1);
	    if (count > 0)
		XStoreBytes(dpy, buff, count);

	    close(fd);
	}
	else
	{
	    fprintf(stderr, "swm: couldn't open \"%s\"\n", action);
	}
	break;

    case F_SWMRC:
	len = strlen(action);
	if (len == 0)
	    ptr = NULL;
	else
	{
	    ptr = (char *)malloc(len+1);
	    if (ptr == NULL)
	    {
		fprintf(stderr, "swm: out of memory\n");
		exit(1);
	    }
	    strcpy(ptr, action);
	    ptr = ExpandFilename(ptr);
	}

	/* first get rid of the existing menu structure and destroy all
	 * windows */
	for (root = MenuList; root != NULL;)
	{
	    for (item = root->last; item != NULL;)
	    {
		if (item->pull != NULL)
		{
		    XDeleteContext(dpy, item->pull, MenuContext);
		    XDestroyWindow(dpy, item->pull);
		}
		XDeleteContext(dpy, item->w, MenuContext);
		XDestroyWindow(dpy, item->w);

		tmp_item = item;
		item = item->prev;
		free(tmp_item);
	    }

	    XDeleteContext(dpy, root->w, MenuContext);
	    XDestroyWindow(dpy, root->shadow);
	    XDestroyWindow(dpy, root->w);

	    tmp_root = root;
	    root = root->next;
	    free(tmp_root);
	}
	MenuList = NULL;
	LastMenu = NULL;
	/*
	ActiveMenu = NULL;
	ActiveItem = NULL;
	*/

	UngrabAllButtons();
	UngrabAllKeys();

	ParseSwmrc(ptr);

	GrabAllButtons();
	GrabAllKeys();
	break;

    case F_REFRESH:
	w = XCreateSimpleWindow(dpy, Root,
	    0, 0, 9999, 9999, 0, Black, Black);
	XMapWindow(dpy, w);
	XDestroyWindow(dpy, w);
	XFlush(dpy);
	break;

    case F_WINREFRESH:
	if (DeferExecution(context, func, DotCursor))
	    return;

	if (context == C_ICON)
	    w = XCreateSimpleWindow(dpy, tmp_win->icon_w,
		0, 0, 9999, 9999, 0, Black, Black);
	else
	    w = XCreateSimpleWindow(dpy, tmp_win->frame,
		0, 0, 9999, 9999, 0, Black, Black);

	XMapWindow(dpy, w);
	XDestroyWindow(dpy, w);
	XFlush(dpy);
	break;

    case F_QUIT:

        KillClient(RootDesktop);
        KillDesktop(RootDesktop);
        Done();
        XKillClient(dpy, RootDesktop->w);
	break;
    }
    XUngrabPointer(dpy, CurrentTime);
}

/********************************
*
*   KillClient
*        Kill Client Prosses
*
*        tmp_win : Swm Window
*
*                    1889.8.17  FXIS
*
*********************************/
KillClient(tmp_win)
SwmWindow *tmp_win;
{
SwmWindow *t;
Window    myParent;

        free((char *)tmp_win);

        myParent = tmp_win -> w;
           for ( t = SwmRoot.next; t != NULL; t = t -> next)
               {
                if ((t->parentId==myParent)&&(t->windowFlag==2)&&(t->initWindow!=1))
                     {
                        if (t->next != NULL)
                           {
                           t->prev->next = t->next;
                           t->next->prev = t->prev;
                           }
                        else
                           t->prev->next = NULL;

                        XUnmapWindow(dpy, t->frame);
                        XKillClient(dpy, t->w);
                        DestroyWindow(t);
                        XSync(dpy);
                     }
                }
}

/*******************************
*
*   KillDesktop
*     DestroyDesktopWindow & Kill Clients
*
*      tmp_win:SwmWindow (RootDesktop->w)
*
*                                89.8.23 FXIS
*
********************************/
KillDesktop(tmp_win)
SwmWindow *tmp_win;
{
 SwmWindow *t;
 Window myParent;

         myParent = tmp_win -> w;
         for ( t = SwmRoot.next; t != NULL; t = t -> next)
               {
                if ((t -> parentId == myParent) && (t -> windowFlag == 1))
                     {
                        KillClient(t);
                        if (t->next != NULL)
                           {
                           t->prev->next = t->next;
                           t->next->prev = t->prev;
                           }
                        else
                           t->prev->next = NULL;

                        DestroyWindow(t);
                        XSync(dpy);
                     }
                }
}
/***********************************************************************
 *
 *  Procedure:
 *	DeferExecution - defer the execution of a function to the
 *	    next button press if the context is C_ROOT
 *
 *  Inputs:
 *	context	- the context in which the mouse button was pressed
 *	func	- the function to defer
 *	cursor	- the cursor to display while waiting
 *
 ***********************************************************************
 */

int
DeferExecution(context, func, cursor)
int context, func;
Cursor cursor;
{

    if (context == C_ROOT)
    {
	XGrabPointer(dpy, Root, True,
	    ButtonPressMask | ButtonReleaseMask,
	    GrabModeAsync, GrabModeAsync,
	    Root, cursor, CurrentTime);

	RootFunction = func;

	return (TRUE);
    }
    
    return (FALSE);
}


/***********************************************************************
 *
 *  Procedure:
 *	NeedToDefer - checks each function in the list to see if it
 *		is one that needs to be defered.
 *
 *  Inputs:
 *	root	- the menu root to check
 *
 ***********************************************************************
 */

NeedToDefer(root)
MenuRoot *root;
{
    MenuItem *mitem;

    for (mitem = root->first; mitem != NULL; mitem = mitem->next)
    {
	switch (mitem->func)
	{
	case F_RESIZE:
	case F_MOVE:
	case F_FORCEMOVE:
	case F_DEICONIFY:
	case F_ICONIFY:
	case F_RAISELOWER:
	case F_RAISE:
	case F_LOWER:
	case F_FOCUS:
	case F_DESTROY:
	case F_WINREFRESH:
	case F_ZOOM:
	case F_FULLZOOM:
	    return TRUE;
	}
    }
    return FALSE;
}

/***********************************************************************
 *
 *  Procedure:
 *	Execute - execute the string by /bin/sh
 *
 *  Inputs:
 *	s	- the string containing the command
 *
 ***********************************************************************
 */

void
Execute(s)
    char *s;
{
    int status, pid, w;
    register int (*istat) (), (*qstat) ();

    if ((pid = vfork()) == 0)
    {
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGHUP, SIG_DFL);
	execl("/bin/sh", "sh", "-c", s, 0);
	_exit(127);
    }
    istat = signal(SIGINT, SIG_IGN);
    qstat = signal(SIGQUIT, SIG_IGN);
    while ((w = wait(&status)) != pid && w != -1);
    if (w == -1)
	status = -1;
    signal(SIGINT, istat);
    signal(SIGQUIT, qstat);
}

/***********************************************************************
 *
 *  Procedure:
 *	FocusOnRoot - put input focus on the root window
 *
 ***********************************************************************
 */

void
FocusOnRoot()
{
    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    if (Focus != NULL)
    {
	if (Highlight && Focus->highlight)
	{
	    XSetWindowBorderPixmap(dpy, Focus->frame, GrayTile);
	    XSetWindowBorderPixmap(dpy, Focus, GrayTile);
	    XSetWindowBorderPixmap(dpy, Focus->w, GrayTile);
	}
            SetNormal(Focus);
    }
    Focus = NULL;
    FocusRoot = TRUE;
}

DeIconify(tmp_win)
SwmWindow *tmp_win;
{
    SwmWindow *t;
    Window myParent;

    /* de-iconify group members (if any) */
    if (tmp_win->group == tmp_win->w)
    {
	for (t = SwmRoot.next; t != NULL; t = t->next)
	{
	    if (tmp_win->group == t->group &&
		tmp_win->group != t->w && t->icon)
	    {
		if (t->icon_on)
		    Zoom(t, t->icon_w, t->frame);
		else
		    Zoom(tmp_win, tmp_win->icon_w, t->frame);

		XMapWindow(dpy, t->w);
		if (NoRaiseDeicon)
		    XMapWindow(dpy, t->frame);
		else
		    XMapRaised(dpy, t->frame);

		XUnmapWindow(dpy, t->icon_w);
		t->icon = FALSE;
		t->icon_on = FALSE;
	    }
	}
    }

    /* now de-iconify the main window */
    if (tmp_win->icon)
    {
	if (tmp_win->icon_on)
	    Zoom(tmp_win, tmp_win->icon_w, tmp_win->frame);
	else if (tmp_win->group != NULL)
	{
	    for (t = SwmRoot.next; t != NULL; t = t->next)
	    {
		if (tmp_win->group == t->w && t->icon_on)
		{
		    Zoom(t, t->icon_w, tmp_win->frame);
		    break;
		}
	    }
	}
    }

    if (tmp_win -> windowFlag == 1)
    {
        myParent = tmp_win -> w;
        for ( t = SwmRoot.next; t != NULL; t = t -> next)
        {
                if ((t -> parentId == myParent) && (t -> windowFlag == 2))
                {
                        if (t -> icon == TRUE)
                        {
                                XUnmapWindow(dpy, t -> w);
                                XUnmapWindow(dpy, t -> frame);
                                XMapSubwindows(dpy, t -> icon_w);
                                XMapWindow(dpy, t -> icon_w);
                                XUnmapWindow(dpy, t->dummyicon_w);
                        }
                        else
                        {
                                XMapWindow(dpy, t -> w);
                                if (NoRaiseDeicon)
                                        XMapWindow(dpy, t -> frame);
                                else
                                        XMapRaised(dpy, t -> frame);
                                XUnmapWindow(dpy, t->icon_w);
                                XUnmapWindow(dpy, t -> icon_bm_w);
                                t->icon = FALSE;
                                t -> icon_on = FALSE;
                        }
                }
        }
    }

    XMapWindow(dpy, tmp_win->w);
    if (NoRaiseDeicon)
	XMapWindow(dpy, tmp_win->frame);
    else
	XMapRaised(dpy, tmp_win->frame);

    XUnmapWindow(dpy, tmp_win->icon_w);
    if (tmp_win->iconified)
      {
       XMapSubwindows(dpy, tmp_win->dummyicon_w);
       XMapWindow(dpy, tmp_win->dummyicon_w);
      }
    if (WarpCursor && tmp_win->icon)
    {
	XWarpPointer(dpy, None, tmp_win->frame,
	    0, 0, 0, 0, 30, 8);
    }
    tmp_win->icon = FALSE;
    tmp_win->icon_on = FALSE;
}

Iconify(tmp_win)
SwmWindow *tmp_win;
{
    SwmWindow *t;
    int iconify;
    Window myParent;

    iconify = !tmp_win->iconify_by_unmapping;
    if (iconify)
    {
	XMapSubwindows(dpy, tmp_win->icon_w);
        XMapWindow(dpy, tmp_win->icon_w);
        XUnmapWindow(dpy, tmp_win->dummyicon_w);
    
        if(tmp_win -> windowFlag == 1)
        {
            myParent = tmp_win -> w;
            for ( t = SwmRoot.next; t != NULL; t = t -> next)
            {
                if ((t -> parentId == myParent) && (t -> windowFlag == 2)
		    && (tmp_win -> icon == 0))
                {
                    XUnmapWindow(dpy, t -> icon_w);
                    XUnmapWindow(dpy, t -> icon_bm_w);
                    XUnmapWindow(dpy, t->frame);
                    XUnmapWindow(dpy, t->w);
                 }
              }
          }
    }

    /* iconify group members first */
    if (tmp_win->group == tmp_win->w)
    {
	for (t = SwmRoot.next; t != NULL; t = t->next)
	{
	    if (tmp_win->group == t->group && tmp_win->group != t->w)
	    {
		if (iconify)
		{
		    if (t->icon_on)
			Zoom(t, t->icon_w, tmp_win->icon_w);
		    else
			Zoom(t, t->frame, tmp_win->icon_w);
		}

		XUnmapWindow(dpy, t->frame);
		XUnmapWindow(dpy, t->w);
		XUnmapWindow(dpy, t->icon_w);
		if (Highlight && tmp_win->highlight)
		{
		    XSetWindowBorderPixmap(dpy, tmp_win->frame, GrayTile);
		    XSetWindowBorderPixmap(dpy, tmp_win->title_w, GrayTile);
		    XSetWindowBorderPixmap(dpy, tmp_win->w, GrayTile);
		}
		if (t == Focus)
		{
		    XSetInputFocus(dpy, PointerRoot,
			RevertToPointerRoot, CurrentTime);
		    Focus = NULL;
		    FocusRoot = TRUE;
		}
	    }
	}
    }

    if (iconify)
	Zoom(tmp_win, tmp_win->frame, tmp_win->icon_w);
    XUnmapWindow(dpy, tmp_win->frame);
    XUnmapWindow(dpy, tmp_win->w);

    if (Highlight && tmp_win->highlight)
    {
	XSetWindowBorderPixmap(dpy, tmp_win->frame, GrayTile);
	XSetWindowBorderPixmap(dpy, tmp_win->title_w, GrayTile);
	XSetWindowBorderPixmap(dpy, tmp_win->w, GrayTile);
    }

    if (tmp_win == Focus)
    {
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot,
	    CurrentTime);
	Focus = NULL;
	FocusRoot = TRUE;
    }
    tmp_win->icon = TRUE;
    if (iconify)
	tmp_win->icon_on = TRUE;
    else
	tmp_win->icon_on = FALSE;
}
