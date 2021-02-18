#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Menu.c,v 5.27 89/11/22 15:11:19 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1989 by Tektronix, Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the names of Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Digital Equipment, Tektronix and M.I.T.
 * make no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * DIGITAL EQUIPMENT, TEKTRONIX, AND M.I.T.  DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL DIGITAL EQUIPMENT, TEKTRONIX, OR M.I.T.  BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */



/*
 * MODIFICATION HISTORY
 *
 * 000 -- M. Gancarz, DEC Ultrix Engineering Group
 * 001 -- L. Guarino Reid, DEC Ultrix Engineering Group, Western Software Lab
 *	  February 16, 1987
 *	  Change menu implementation so that it uses EnterWindow, LeaveWindow,
 *	  and MouseMotion events to track the mouse, instead of polling.
 * 002 -- L. Guarino Reid, DEC Ultrix Engineering Group, Western Software Lab
 *	  April 30, 1987. Convert to X11.
 * 003 -- L. Guarino Reid, DEC Ultrix Engineering Group, Western Software Lab
 *	  June 18, 1987. Change call to system to handle signals move smoothly.
 */

#ifndef lint
static char *sccsid = "@(#)Menu.c	3.8	1/24/86";
#endif

#include <signal.h>
#include "awm.h"

extern Window BoundContext;	/* binding context - in awm.c for menu hack */
extern Window ContextWindow;	/* Button window - in awm.c for menu hack */
extern int ContextX, ContextY;	/* mouse X and Y at ButtonPress. */

#define DisplayLine(w, pane, width, height, str, fg, bg, inv) \
	XSetForeground(dpy, MenuBgGC, bg); \
	XFillRectangle(dpy, w, MenuBgGC, 0, pane, width, height); \
        XSetForeground(dpy, MenuGC, fg); \
	XDrawString(dpy, w, MenuGC, HMenuPad, pane + VMenuPad + MFontInfo->ascent, str, strlen(str));


/* This code is bogus! */
#ifdef broken
Bool alternateGC = True;	/* true if only 2 colors are used */

#define DisplayLine(w, pane, width, height, str, fg, bg, inv) \
         if (alternateGC) { \
	     if (inv) \
	         XFillRectangle(dpy, w, MenuInvGC, 0, pane, width, height); \
	     else \
                 XDrawString(dpy, w, MenuGC, HMenuPad, pane + VMenuPad + MFontInfo->ascent, str, strlen(str)); \
         } else { \
             XSetForeground(dpy, MenuBgGC, bg); \
	     XFillRectangle(dpy, w, MenuBgGC, 0, pane, width, height); \
             XSetForeground(dpy, MenuGC, fg); \
             XDrawString(dpy, w, MenuGC, HMenuPad, pane + VMenuPad + MFontInfo->ascent, str, strlen(str)); \
         }
#endif

#ifdef SYSV
#ifndef hpux
#define vfork() fork()
#endif /* hpux */
#endif /* SYSV */

/* the following procedure is a copy of the implementation of system.
 */
execute(s)
char *s;
{
	int pid, w;
	register int (*istat)(), (*qstat)();

	if ((pid = vfork()) == 0) {
	     /* don't permit ctl-C to tekwm to kill the shell unless tekwm
		is receiving signals (i.e. in fg on BSD) */
		execl("/bin/sh", "sh", "-c", s, 0);
		_exit(127);
	}
        /* if shell command is not backgrounded, we wait until it finishes.  If
	   we get signalled from prgp, wait for shell to die.  Then we really
	   should exit??? */ 
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	return(status);
}

Bool Menu(window, mask, button, x, y, menu)
Window window;				/* Event window. */
int mask;				/* Button/key mask. */
int button;				/* Button event detail. */
int x, y;				/* Event mouse position. */
MenuInfo *menu;
{
    XEvent button_event;		/* Button event packet. */
    Window button_window;			/* Button event window. */
    int event_x, event_y;		/* location of motion event */
    Bool func_stat;			/* Function status return. */
    Window sub_window;			/* Current subwindow. */
    Window un_window = 0;		/* non-window value */
    int cur_item = 0;			/* Current menu item. */
    int hi_lite = 0;			/* Current highlighted item. */
    int i;				/* Iteration counter. */
    int autocontext;			/* Context for automatic execution */
    int hlfg, hlbg;			/* Hi-liter pixels. */
    ActionLine *ml;			/* Menu lines pointer. */
    char *hlname;			/* Pointer to hi-liter name. */
    char *strbuf;			/* String buffer for IsTextNL. */
    static struct timeval timeout = {(long) 0, (long) 130000};
					/* 130 msec delay for warp test */
    int	    rootx, rooty, junk_mask;	/* temps for QueryPointer */
    Window  junk_root, subw;
    int	    x2, y2;
    AwmInfoPtr awi;
    Boolean autoselect = FALSE;

    /*
     * Map the menu.
     */
    MapMenu(menu, x, y);
    /*
     * Change the cursor.
     */
    /* force a grab in case the grab that got us here is screwed up 
      (happening on titles and buttons, why I don't know - is somebody 
       setting owner_events False?) */
    if( XGrabPointer(dpy, menu->w, TRUE, EVENTMASK,
		     GrabModeAsync, GrabModeAsync,
		     None, MenuCursor, CurrentTime) != GrabSuccess)
         Error( "Could not grab pointer for menu" );

    event_x = (menu->width >> 2) * 3;
    if (((awi = GetAwmInfo(ContextWindow)) != NULL) &&
	(awi->attrs & AT_GADGETS_AUTOEXEC))
	autocontext = WINDOW | BORDER | TITLE | GADGET;
    else
	autocontext = WINDOW | BORDER | TITLE;

    if (Autoselect) {
        event_y = (menu->iheight >> 1) * 3;
        XWarpPointer(dpy, None, menu->w, 0, 0, 0, 0, event_x, event_y);
	goto hilite;
    }
    else {
		/* position in middle of second box (first item) */
		/* event_x/y needed for XWarp shenanigans */
        event_y = menu->iheight >> 1;
        XWarpPointer(dpy, None, menu->w, 0, 0, 0, 0, event_x, event_y);
        XFlush(dpy);
    }

    /*
     * Main loop.
     */
    while (TRUE) {

        /*
         *  Get next event for menu.
         */
        if (!GetButton(&button_event)) continue;
	switch (button_event.type) {

    /* because awm selects button events in windows outside menu, we need to 
       ignore such here, except for ButtonRelease. Note that button must be
       grabbed with owner_events = True for this to work. */
            case LeaveNotify:
	        /*
		 * Formerly:
	         * If the mouse has moved out of the menu sideways, abort
	         * the menu operation.
		 * Now:
		 * De-select any selected item, by positioning on menu title.
	         */
		if(menu->w != ((XLeaveWindowEvent * )&button_event)->window) {
		    break;
		}
	        event_x = ((XLeaveWindowEvent * )&button_event)->x;
	        event_y = ((XLeaveWindowEvent * )&button_event)->y;
/*		if (event_x < 0 || event_x > menu->width) {
		    return(FALSE);
        	}
		CancelMenu(menu, button); This old code was bizarre!  It was
		never exercised in tekwm because Leave/Enter never got here */
		event_y = 0;
		goto hilite;

            case EnterNotify:
		if(menu->w != ((XEnterWindowEvent * )&button_event)->window) {
		    break;
		}
	        event_x = ((XEnterWindowEvent * )&button_event)->x;
	        event_y = ((XEnterWindowEvent * )&button_event)->y;
		goto hilite;
            case MotionNotify:
		/* Eat up any outstanding motion events */    
		while (XCheckTypedEvent(dpy, MotionNotify,
					     &button_event))
			if (EventDebug)
			    print_event_info("menu", &button_event);
		if(menu->w != ((XPointerMovedEvent * )&button_event)->window) {
		    break;
		}
		event_x = ((XPointerMovedEvent * )&button_event)->x;
	        event_y = ((XPointerMovedEvent * )&button_event)->y;
hilite:
        	/*
         	* If the mouse has moved below or above the menu, but is still
         	* within the same vertical plane, reset vertical position
         	* to unselect any item (i.e. in title)
		* If the mouse is still outside the menu, ditto.
         	*/
        	if ((event_y >= menu->height) || (event_y < 0) ||
		    (event_x < 0 || event_x > menu->width)) 
		  event_y = 0;

        	/*
         	* If the mouse has moved to another item in the menu,
         	* highlight the new item.
         	*/
        	cur_item = event_y / menu->iheight;
        	if (cur_item != hi_lite) {

		    /*
		    * Remove highlighting on old item.
		    */
		    if (hi_lite) {
                	DisplayLine(menu->w, hi_lite * menu->iheight,
                            menu->width, menu->iheight, hlname,
                            hlfg, hlbg, 1);
			XFlush(dpy);
		    }

		    /*
		    * Highlight new item.
		    */
		    hi_lite = cur_item;
		    if (cur_item) {
                	for(i = 1, ml = menu->line; ml; i++, ml = ml->next) {
			    if (i == cur_item) break;
               	 	}
		        /* invert usage of colors, so easiest defaults work */
                	DisplayLine(menu->w, cur_item * menu->iheight,
				    menu->width, menu->iheight, ml->name,
				    menu->hlbg.pixel, menu->hlfg.pixel, 1);
			XFlush(dpy);
			/* stash text and colors for restoration */
             		hlfg = ml->fg.pixel;
            		hlbg = ml->bg.pixel;
            		hlname = ml->name;
		    }
		}
        	break;

            case ButtonPress:
		if (button == -1) {
		    /* This is a sub menu - stash Button from press to check
		       against release, otherwise, cancel menu */
		    button = button_event.xbutton.button;
		    break;
		}
		/* fall through - will go to default: and abort */

            case ButtonRelease:
	        /* have we released the invoking button? */
		if (button_event.xbutton.button == button) {
		    /*
		     * If no item was selected, 
		     * close the menu, reset the pointer's position and return
		     */
		    /* Reset the cursor position unless moving or outside menu.
		     * In order to detect movement reliably, we must insure a
		     * mimimum time delay between button release and the warp
		     * request getting processed, and allow one pixel motion.
		     */

		    select(0, 0, 0, 0, &timeout);
		    XWarpPointer(dpy, menu->w, RootWindow(dpy, scr),
		/* use motionNotify position - button may be root-relative */
				 event_x - 1, 
				 event_y - 1, 3, 3, x, y - 1);
		    /* HACK!  XXX shift up one pixel so next pick does not land
		     * in the menu.  Remove this when we add removal of menu
		     * except selected item, so we don't creep up.  When we
		     * do, we must be sure that menu item never lands over
		     * invocation position.
		     */
		    /*
		     * If warp succeeded and button is pressed again during
		     * wait, we must fake it out by resetting its
		     * position/window to be where the pointer nominally is
		     * after = actually was when menu was invoked.  Window must
		     * be actual tekwm context window.  Since this is not
		     * passed in, get it from a global!  Assume subwindow and
		     * root_x/y members are not used though root_x/y should be
		     * close enough.
		     */
		    XQueryPointer(dpy, RootWindow(dpy, scr), &junk_root,
				  &subw, &rootx, &rooty, &x2, &y2, &junk_mask);
		    /* sync up now so we are assured of seeing all pre-warp
		       button events */
		    if (XCheckMaskEvent(dpy, ButtonPressMask, &button_event)) {
			if ((rootx == x) && (rooty == y - 1)) {
				/* HACK!  XXX */
			    button_event.xbutton.window = ContextWindow;
			    button_event.xbutton.x = ContextX;
			    button_event.xbutton.y = ContextY - 1;
				/* HACK!  XXX */
			}
			XPutBackEvent(dpy, &button_event);
		    }
		    if (!cur_item) {
			UnmapMenu(menu);
		        return(TRUE);
		    }

		     /*
		      * Get a pointer to the menu line selected.
		      */
		     --cur_item;
		     for(i = 0, ml = menu->line; ml; i++, ml = ml->next) {
		         if (i == cur_item) break;
		     }

		     /*
		      * Perform the selected menu line action.
		      */
		     switch (ml->type) {

		         case IsWindowCommand:
			     /*	if this menu is bound only to titlebar,
			      * gadget, border context, or window, auto
			      * select the window the menu was invoked over.
			      */
			     if ((BoundContext & (WINDOW | BORDER | TITLE | 
				 GADGET | ROOT | ICON))
				 != (BoundContext & autocontext)) {
				 UnmapMenu(menu);
			        /* else change cursor and grab next button
				 * event to select the target window */
				if (XGrabPointer( dpy, RootWindow(dpy, scr),
					       TRUE, EVENTMASK, GrabModeAsync,
					       GrabModeAsync, None,
					       TargetCursor, CurrentTime )
				    != GrabSuccess )
				    Error( "Could not grab pointer" );
				if (!GetContext(&window, &mask, &button, 
					    &event_x, &event_y, menu))
				    break;
				/* wait for the next ButtonRelease */
				while (TRUE) {
				    if (GetButton(&button_event) &&
					button_event.type == ButtonRelease)
				    break;
				}
			     }
			     XUngrabPointer( dpy, CurrentTime );
			     if (window == menu->w)
				 break;
			     strbuf = malloc(strlen(ml->text) + 30);
			     /*	allow extra room for exactly one sprintf
				conversion spec */
			     (void) sprintf(strbuf, ml->text, window);
		             UnmapMenu(menu);
			     /*	XXX subshell got signal; should assume it is
				intended for tekwm and exit??? */
		             if(execute(strbuf) == -1) {
				 /* Cleanup();
				 exit(0); */
			     }
		             break;

		         case IsShellCommand:
		             UnmapMenu(menu);
			     /*	subshell got signal; should assume it is intended for
				tekwm and exit??? */
		             if(execute(ml->text) == -1) {
				 /* Cleanup();
				 exit(0); */
			     }
		             break;

		         case IsText:
		             UnmapMenu(menu);
		             XStoreBytes(dpy, ml->text, strlen(ml->text));
		             break;

		         case IsTextNL:
		             UnmapMenu(menu);
		             strbuf = (char *)malloc(strlen(ml->text) + 2);
		             strcpy(strbuf, ml->text);
		             strcat(strbuf, "\n");
		             XStoreBytes(dpy, strbuf, strlen(strbuf));
		             free(strbuf);
		             break;

		         case IsFunction:
		         case IsDownFunction:
			     /*	if this menu is bound only to titlebar, gadget,
			      * border context, or window, auto-select the
			      * window the menu was invoked over. */
			     if ((BoundContext & (WINDOW | BORDER | TITLE | 
				 GADGET | ROOT | ICON))
				 == (BoundContext & autocontext)) {
				 UnmapMenu(menu);
				/* immediately invoke function regardless, 
				   but grab if Down func */
				if (ml->type == IsDownFunction) {
				   if (XGrabPointer( dpy, RootWindow(dpy, scr),
					       TRUE, EVENTMASK, GrabModeAsync,
					       GrabModeAsync, None,
					       TargetCursor, CurrentTime )
				      != GrabSuccess )
				      Error( "Could not grab pointer" );
				   func_stat = (*ml->func) (
					       window, mask, -1, x, y);
				 /* Inform Down func that it must eat the Press
				    via a null button.  Pass the point in the
				    window where the menu was invoked. */
				}
				else
				    func_stat = (*ml->func) (
					        window, mask, button, x, y);
				if (!func_stat && ml->type == IsDownFunction) {
	   /* Down func did not wait for the next ButtonRelease - eat it */
				    while (TRUE) {
					if (GetButton(&button_event) &&
					    button_event.type == ButtonRelease)
					break;
				    }
				}
			     }
			     else {
			        /* else change cursor and grab next button
				 * event to select the target window */
				if (XGrabPointer( dpy, RootWindow(dpy, scr),
					       TRUE, EVENTMASK, GrabModeAsync,
					       GrabModeAsync, None,
					       TargetCursor, CurrentTime )
				    != GrabSuccess )
				    Error( "Could not grab pointer" );
				if (!GetContext(&sub_window, &mask, &button, 
					    &event_x, &event_y, menu))
				    break;
				UnmapMenu(menu);
				if (sub_window != menu->w)
				    func_stat = (*ml->func) (
					sub_window, mask, button, event_x, 
					event_y);
				else func_stat = FALSE;
				if (!func_stat) {
		   /* func did not wait for the next ButtonRelease - eat it */
				    while (TRUE) {
					if (GetButton(&button_event) &&
					    button_event.type == ButtonRelease)
					break;
				    }
				}
			     }
			     XUngrabPointer( dpy, CurrentTime );
		             break;

		         case IsImmFunction:
		             UnmapMenu(menu);
 		            (*ml->func) (
			      un_window, mask, button, event_x, 
			      event_y);
		             break;
		 
		         case IsMenuFunction:
		             UnmapMenu(menu);
		             func_stat = 
			     	Menu(window, mask, -1, x, y, ml->menu);
				/* Pass an invalid button to sub menu to
				   telll it to ignore the button press.  Pass
				   along the window the top menu was bound to
				   so that auto-select works in sub menus */
		             return(func_stat);
		             break;

		         default:
 		            Error("Menu -> Internal type error.");
		     }
		     return(TRUE);
		  
                 } 
		 /* else a different button was released. Fall through: */
            default:
                    /*
                     * Some other button event occurred, so abort the menu
                     * operation.
                     */
		    /* swallow all ButtonRelease events. */
		    /* must re-grab to keep control when menu is unmapped */
		    if(XGrabPointer(dpy, RootWindow(dpy, scr), TRUE, EVENTMASK,
				    GrabModeAsync, GrabModeAsync,
				    None, MenuCursor, CurrentTime) != GrabSuccess)
			Error( "Could not grab pointer for menu" );
		    ResetCursor(button);
		    /* Reset the cursor position unless moving or left menu */
		    select(0, 0, 0, 0, &timeout);
		    XWarpPointer (dpy, menu->w, RootWindow(dpy, scr),
						  button_event.xbutton.x - 1, 
						  button_event.xbutton.y - 1, 3, 3, x, y - 1);
					/* HACK!  XXX */
			/* its so hard to press second button, release all, and
			   press again before the timeout that fixing this here
			   is not worth the code */
		    UnmapMenu(menu);
		    WaitForButtonsUp();
		    XUngrabPointer(dpy, CurrentTime);
                    return(TRUE);
	}
    }
}


/*
 * Create the menu windows for later use.
 */
CreateMenus()
{
    MenuLink *ptr;

    for(ptr = Menus; ptr; ptr = ptr->next)
        InitMenu(ptr->menu);
}

/*
 * Initialize a menu.
 */
InitMenu(menu)
MenuInfo *menu;
{
    ActionLine *ml;		/* Menu lines pointer. */
    int width;			/* Width of an item name. */
    int maxwidth;		/* Maximum width of item names. */
    int len;			/* Length of an item name. */
    int count = 1;		/* Number of items + 1 for name. */

    /*
     * Determine the name of the longest menu item.
     */
    maxwidth = XTextWidth(MFontInfo, menu->name, strlen(menu->name));
    if (maxwidth == 0)
        Error("InitMenu -> Couldn't get length of menu name");

    for(ml = menu->line; ml; ml = ml->next) {
        if ((len = strlen(ml->name)) == 0)
            break;
        width = XTextWidth(MFontInfo, ml->name, len);
        if (width == 0) 
	  Error("InitMenu -> Couldn't get length of menu item name");
        if (width > maxwidth) maxwidth = width;
        count++;
    }

    /*
     * Get the color cells for the menu items.
     */
    GetMenuColors(menu);

    /*
     * Stash the menu parameters in the menu info structure.
     */
    menu->iheight = MFontInfo->ascent + MFontInfo->descent + (VMenuPad << 1);
    menu->height = menu->iheight * count;
    menu->width = maxwidth + (HMenuPad << 1);
    menu->image = NULL;

    /*
     * Create the menu window.
     */
    menu->w = XCreateSimpleWindow(dpy, RootWindow(dpy, scr),
                            0, 0,
                            menu->width,
                            menu->height,
                            MBorderWidth,
                            MBorder, MBackground);
    if (menu->w == NULL) Error("InitMenu -> Couldn't create menu window");
    if (SaveUnder) {
	XSetWindowAttributes xswa;

	xswa.save_under = True;
	XChangeWindowAttributes(dpy, menu->w, CWSaveUnder, &xswa);
    }

    /*
     * Store the window name.
     */
    XStoreName(dpy, menu->w, menu->name);

    /*
     * Define a cursor for the window.
     */
    XDefineCursor(dpy, menu->w, MenuCursor);

    /*
     * We want enter, leave, and mouse motion events for menus.
     */
    XSelectInput(dpy, menu->w, 
    	(EnterWindowMask | LeaveWindowMask | PointerMotionMask));
}

/*
 * Map a menu.
 */
MapMenu(menu, x, y)
MenuInfo *menu;
int x, y;
{
    int item;
    Window w;
    ActionLine *ml;
    XWindowChanges values;

    w = menu->w;

    /*
     * Move the menu into place, normalizing the coordinates, if necessary;
     * then map it.
     */
    x -= (menu->width >> 1);
    if (x < 0) x = 0;
    else if (x + menu->width >= ScreenWidth)
        x = ScreenWidth - menu->width - (MBorderWidth << 1);
    if (y < 0) y = 0;
    else if (y + menu->height >= ScreenHeight)
        y = ScreenHeight - menu->height - (MBorderWidth << 1);
    values.x = x;
    values.y = y;
    values.stack_mode = Above;
    XConfigureWindow(dpy, w, CWX|CWY|CWStackMode, &values);

    /*
     * Map the window and draw the text items.
     */
    XMapWindow(dpy, w);
    /* invert usage of colors, so easiest defaults work */
    DisplayLine(w, 0, menu->width, menu->iheight, menu->name,
                menu->bg.pixel, menu->fg.pixel, 0);

#ifdef broken
    if (alternateGC) {
        XFillRectangle(dpy, menu->w, MenuInvGC, 0, 0,
		       menu->width, menu->iheight);
        XDrawRectangle(dpy, menu->w, MenuInvGC, 1, 1,
		       menu->width - 3, menu->iheight - 3);
    } else {
#endif
    /* draw a background box around the title */
    XSetForeground(dpy, MenuBgGC, menu->bg.pixel );
    XDrawRectangle(dpy, menu->w, MenuBgGC, 1, 1, menu->width - 3, 
		       menu->iheight - 3);
#ifdef broken
    }
#endif

    item = menu->iheight;
    for(ml = menu->line; ml; ml = ml->next) {
        DisplayLine(w, item, menu->width, menu->iheight, ml->name,
                    ml->fg.pixel, ml->bg.pixel, 0);
        item += menu->iheight;
    }

    /*
     * Position the mouse cursor in the menu header (or in the first item
     * if "autoselect" is set).
     */

    XFlush(dpy);
}

/*
 * Unmap a menu, restoring the contents of the screen underneath
 * if necessary. (Restore portion is a future.)
 */
UnmapMenu(menu)
MenuInfo *menu;
{
    /*
     * Unmap and flush.
     */
    XUnmapWindow(dpy, menu->w);
    XFlush(dpy);
}

/*
 * Get the context for invoking a window manager function.
 * Return current mask and button state, allowing user to release modifiers.
 * (Formerly) Do not allow the user to change buttons - cancel and return 
 * false if he does.
 */
Bool GetContext(w, mask, button, x, y, menu)
Window *w;
int *mask;				/* Button/key mask. */
int *button;				/* Button event detail. */
int *x, *y;
MenuInfo *menu;
{
    XEvent button_event;  /* Button input event. */

    while (TRUE) {

        /*
         * Get the next mouse button event.  Spin our wheels until
         * a matching button event is returned (ie. GetButton == TRUE).
         * Note that mouse events within an icon window are handled
         * in the "GetButton" function or by the icon's owner if
         * it is not uwm.
         */
        if (!GetButton(&button_event)) continue;

        /*
         * If the button event received is not a ButtonPress event
         * then continue until we find one.
         */
        if (button_event.type != ButtonPress) continue;

        /*
         * Okay, determine the event window and mouse coordinates.
         */
        status = XTranslateCoordinates(dpy, 
				    button_event.xany.window, 
				    RootWindow(dpy, scr),
                                    button_event.xbutton.x, 
                                    button_event.xbutton.y, 
                                    x, y,
                                    w);

        if (status == FAILURE) continue;

        if (*w == 0)
            *w = RootWindow(dpy, scr);
	*mask = button_event.xbutton.state;
	*button = button_event.xbutton.button;

        return(TRUE);
    }
}

/*
 * Get the color cells for a menu.  
 */
GetMenuColors(menu)
MenuInfo *menu;
{
    register ActionLine *ml;		/* Menu lines pointer. */

    /*
     * If we have more than 2 colors available, then attempt to get
     * the color map entries requested by the user.
     * Otherwise, default to standard black and white.
     *
     * The boolean "alternateGC" is true iff we can use just two colors.
     * This code is bogus! With awm's resources, we do not know when
     * we have all black&white menus, so we can't optimize this case out.
     */

    if (DisplayCells(dpy, scr) > 2) {
        /*
         * Get the menu header colors first.
         */
        if (!(menu->foreground && menu->background &&
              XParseColor(dpy, DefaultColormap(dpy, scr), menu->foreground, &menu->fg) &&
              XAllocColor(dpy, DefaultColormap(dpy, scr), &menu->fg) &&
              XParseColor(dpy, DefaultColormap(dpy, scr), menu->background, &menu->bg) &&
              XAllocColor(dpy, DefaultColormap(dpy, scr), &menu->bg))) {
            menu->fg.pixel = MForeground;
            menu->bg.pixel = MBackground;
#ifdef broken
	    alternateGC = False;
#endif
        }

        /*
         * Get the menu highlight colors.
         */
        if (!(menu->fghighlight && menu->bghighlight &&
              XParseColor(
	        dpy, DefaultColormap(dpy, scr), menu->fghighlight, &menu->hlfg) &&
              XAllocColor(dpy, DefaultColormap(dpy, scr), &menu->hlfg) &&
              XParseColor(
	        dpy, DefaultColormap(dpy, scr), menu->bghighlight, &menu->hlbg) &&
              XAllocColor(dpy, DefaultColormap(dpy, scr), &menu->hlbg))) {
            menu->hlfg.pixel = MHlForeground;
            menu->hlbg.pixel = MHlBackground;
#ifdef broken
	    alternateGC = False;
#endif
        }

        /*
         * Get the menu item colors.
         */
        for(ml = menu->line; ml; ml = ml->next) {
            if (!(ml->foreground && ml->background &&
                  XParseColor(dpy, DefaultColormap(dpy, scr), ml->foreground, &ml->fg) &&
                  XAllocColor(dpy, DefaultColormap(dpy, scr), &ml->fg) &&
                  XParseColor(dpy, DefaultColormap(dpy, scr), ml->background, &ml->bg) &&
                  XAllocColor(dpy, DefaultColormap(dpy, scr), &ml->bg))) {
                ml->fg.pixel = MTextForeground;
                ml->bg.pixel = MTextBackground;
#ifdef broken
		alternateGC = False;
#endif
	    }
	}
    } else {

        /*
         * Only 2 colors available, so default to standard black and white.
         */
        menu->fg.pixel = MForeground;
        menu->bg.pixel = MBackground;
        menu->hlfg.pixel = MHlForeground;
        menu->hlbg.pixel = MHlBackground;
        for(ml = menu->line; ml; ml = ml->next) {
            ml->fg.pixel = MTextForeground;
            ml->bg.pixel = MTextBackground;
        }
    }
}
