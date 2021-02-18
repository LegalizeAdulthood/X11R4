/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************************************\
* 					        *
* GWM: icccm.c				        *
* Miscellaneous utilities to enforce the ICCCM  *
* 					        *
\***********************************************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_list.h"
#include "gwm.h"
#include "X11/Xatom.h"
#include "X11/Xutil.h"
#include "wl_number.h"

extern XError(), NoXError();
DECLARE_strchr;

/* Used to tell a client that it has been moved
 */

SendSyntheticMoveEvent(cw)
ClientWindow	cw;
{
    XConfigureEvent	event;

    if(cw -> client_wob)		/* do not send to ourselves */
	return;
    event.type = ConfigureNotify;
    event.display = dpy;
    event.event = cw -> client;
    event.window = cw -> client;
    event.x = cw -> inner_x + cw ->box.x + cw -> box.borderwidth;
    event.y = cw -> inner_y + cw ->box.y + cw -> box.borderwidth;
    event.width = cw -> inner_width;
    event.height = cw -> inner_height;
    event.border_width = cw -> inner_borderwidth;
    event.above = cw -> hook;
    event.override_redirect = False;
    XSendEvent(dpy, cw -> client, False, StructureNotifyMask, &event);
}

/* for defining another icon for WM_STATE property
 */

WOOL_OBJECT
wool_wm_state_user_icon_get()
{
    if (TargetWindow -> cached_props -> user_icon)
	return (WOOL_OBJECT)
	    WLNumber_make(TargetWindow -> cached_props -> user_icon);
    else
	return NIL;
}

WOOL_OBJECT
wool_wm_state_user_icon_set(number)
WOOL_Number	number;
{
    must_be_number(number, 0);
    TargetWindow -> cached_props -> user_icon = (ClientWindow)
	number -> number;
    Update_XA_WM_STATE(TargetWindow);
    return (WOOL_OBJECT) number;
}

/* updating this window's WM_STATE once another icon is defined
 */

WOOL_OBJECT
wool_wm_state_update()
{
    Update_XA_WM_STATE(TargetWindow);
    return NIL;
}

/* getting this window's WM_STATE once another icon is defined
 */

WOOL_OBJECT
wool_wm_state_get(argc, argv)
int     argc;
WOOL_Number argv[];
{
    ClientWindow    cw = TargetWindow;
    Atom            actualtype;
    int             actualformat;
    unsigned long   nitems;
    unsigned long   bytesafter;
    WM_STATE_PROP   wm_state;

    if (argc) {
	must_be_number(argv[0], 0);
	cw = (ClientWindow) argv[0] -> number;
    }
    cw = cw -> window;
    if ((Success == XGetWindowProperty(dpy, cw -> client, XA_WM_STATE, 0,
		      WM_STATE_PROP_Length, False, XA_WM_STATE, &actualtype,
			    &actualformat, &nitems, &bytesafter, &wm_state))
	&& wm_state) {
	switch (wm_state -> state) {
	case WM_STATE_Iconified:
	    return (WOOL_OBJECT) WA_icon;
	case WM_STATE_Normal:
	    return (WOOL_OBJECT) WA_window;
	}
    }
    return NIL;
}

/* Colormaps
 * colormap focus management:
 */

WOOL_OBJECT
wool_set_colormap_focus(argc, argv)
int	argc;
WOOL_Number argv[];
{
    ClientWindow    cw = TargetWindow;

    if (argc) {
	if (argv[0] == (WOOL_Number) NIL) {
	    GWM_set_default_colormap();
	    Context -> InstalledColormapCW = Context -> rootWob;
	    return NIL;
	}
	must_be_number(argv[0], 0);
	cw = (ClientWindow) argv[0] -> number;
    }
    if (cw -> colormap) {
	if (cw -> colormap != Context -> InstalledColormapCW -> colormap)
	    XInstallColormap(dpy, cw -> colormap);
	if (cw -> cached_props -> colormap_windows)
	    cw -> cached_props -> colormap_windows_index = 0;
    } else {
	GWM_set_default_colormap();
    }
    Context -> InstalledColormapCW = cw;
    return NIL;
}

GWM_set_default_colormap()
{
    if (Context -> InstalledColormapCW -> colormap &&
	Context -> InstalledColormapCW -> colormap
	!= Context -> rootWob -> colormap)
	XInstallColormap(dpy, Context -> rootWob -> colormap);
}

/* Sub windows colormaps
 * colormap focus management:
 */

WOOL_OBJECT
wool_set_subwindow_colormap_focus(argc, argv)
int	argc;
WOOL_Number argv[];
{
    ClientWindow    cw = TargetWindow;
    int             index;
    XWindowAttributes wa;
    Colormap        current_colormap;

    if (argc) {
	must_be_number(argv[0], 0);
	index = argv[0] -> number;
    }
    if (cw == Context -> InstalledColormapCW &&
	cw -> cached_props -> colormap_windows) {
	if (argc) {			/* absolute index */
	    cw -> cached_props -> colormap_windows_index =
		index % cw -> cached_props -> colormap_windows_size;
	    XGetWindowAttributes(dpy,
				 cw -> cached_props -> colormap_windows[
			cw -> cached_props -> colormap_windows_index], &wa);
	    if (wa.colormap)
		XInstallColormap(dpy, wa.colormap);
	} else {			/* increment index */
	    XGetWindowAttributes(dpy,
				 cw -> cached_props -> colormap_windows[
			cw -> cached_props -> colormap_windows_index], &wa);
	    current_colormap = wa.colormap;
	    for (index = cw -> cached_props -> colormap_windows_index + 1;
		 index % cw -> cached_props -> colormap_windows_size !=
		 cw -> cached_props -> colormap_windows_index;
		 index++) {
		index = index % cw -> cached_props -> colormap_windows_size;
		XGetWindowAttributes(dpy,
			cw -> cached_props -> colormap_windows[index], &wa);
		if (wa.colormap != current_colormap) {
		    cw -> cached_props -> colormap_windows_index = index;
		    XInstallColormap(dpy, wa.colormap);
		    break;
		}
	    }
	}
    }
    return NIL;
}


/* updating a colormap for a watched window in a WM_COLORMAP_WINDOWS list
 */

Update_colormap_in_colormap_windows_list(evt)
XColormapEvent	*evt;
{
    ClientWindow    cw;

    ContextOfXWindow(evt -> window);
    cw = Context -> InstalledColormapCW;
    if (cw -> cached_props -> colormap_windows_index &&
	evt -> window == cw -> cached_props -> colormap_windows[
			       cw -> cached_props -> colormap_windows_index]
	&& evt -> new)
	if (evt -> colormap)
	    XInstallColormap(dpy, evt -> colormap);
	else
	    XInstallColormap(dpy, Context->rootWob -> colormap);
}

/* declare accepted icons sizes for future clients
 */

WOOL_OBJECT
wool_set_wm_icon_sizes(argc, argv)
int     argc;
WOOL_Number argv[];
{
    int i;
    XIconSize	icon_sizes;

    if(argc != 6)
    return wool_error(BAD_NUMBER_OF_ARGS, argc);
    for(i=0;i<6;i++){
	must_be_number(argv[i], i);
    }
    icon_sizes.min_width = argv[0] -> number;
    icon_sizes.min_height = argv[1] -> number;
    icon_sizes.max_width = argv[2] -> number;
    icon_sizes.max_height = argv[3] -> number;
    icon_sizes.width_inc = argv[4] -> number;
    icon_sizes.height_inc = argv[5] -> number;
    XSetIconSizes(dpy, Context->root, &icon_sizes, 6);
    return NIL;
}

/* get WM_COMMAND
 */

WOOL_OBJECT
wool_get_wm_command()
{
    Atom actual_type;
    int actual_format = 0;
    unsigned long nitems = 0L, leftover = 0L;
    unsigned char *prop = NULL;
    WOOL_List list;
    int i, nstrings = 0, n;
    unsigned char *p;

    if (XGetWindowProperty (dpy, TargetWindow -> client, XA_WM_COMMAND,
			    0L, 1000000L, False,
			    AnyPropertyType, &actual_type, &actual_format,
			    &nitems, &leftover, &prop) == Success &&
	actual_type != None) {
	if (!prop || !nitems)
	    return NIL;
	for (i = 0; i < nitems; i++)
	    if (prop[i] == '\0')
		nstrings ++;
	list = wool_list_make(nstrings);
	p = prop;
	for (n = 0; n < nstrings; n++) {
	    increase_reference(list -> list[n] =
			       (WOOL_OBJECT) WLString_make(p));
	    while(*p++);
	}
	XFree(prop);
	return (WOOL_OBJECT) list;
    }

    return NIL;

}

/* When not-yet mapped configures its window, we must execute the 
 * configure request!
 */

ConfigureUnmappedWindow(evt)
XConfigureRequestEvent	*evt;
{
    XWindowChanges	window_changes;

    window_changes.x = evt -> x;
    window_changes.y = evt -> y;
    window_changes.width = evt -> width;
    window_changes.height = evt -> height;
    window_changes.border_width = evt -> border_width;
    window_changes.sibling = evt -> above;
    window_changes.stack_mode = evt -> detail;

    XConfigureWindow(dpy, evt -> window, evt -> value_mask, &window_changes);
}

/* PROTOCOLS communication via user events
 */

send_protocol_message(window, protocol, timestamp, data_size, data)
Window	window;
Atom   	protocol;
Time 	timestamp;			/* 0 for CurrentTime */
int 	data_size;			/* 0 to 3 32-bit quantities */
Card32 *data;
{
    XClientMessageEvent event;

    event.type = ClientMessage;
    event.window = window;
    event.message_type = XA_WM_PROTOCOLS;
    event.format = 32;
    event.data.l[0] = (long) protocol;
    if (timestamp)
	event.data.l[1] = (long) timestamp;
    else
	event.data.l[1] = (long) CurrentTime;
    data_size = Min(3, data_size);
    if (data_size)
	bcopy(data, &(event.data.l[2]), data_size * 4);
    if (data_size < 3)
	bzero(&(event.data.l[2 + data_size]), (3 - data_size) * 4);

    XSetErrorHandler(NoXError);
    XSendEvent(dpy, window, False, 0, &event);
    XSync(dpy, 0);
    XSetErrorHandler(XError);
}

/* WM_DELETE_WINDOW
 * if window participate in protocol, sends message and returns TRUE
 * else unmaps it and its associated icon (go to withdrawn) and returns NIL
 */

WOOL_OBJECT
wool_delete_window(argc, argv)
int argc;
WOOL_Number *argv;
{
    ClientWindow    cw = TargetWindow -> window;

    if (argc) {
	must_be_number(argv[0], 0);
	cw = ((ClientWindow) argv[0] -> number) -> window;
    }
    if (cw -> client
	&& cw -> cached_props -> wm_delete_window) {
	send_protocol_message(cw -> client, XA_WM_DELETE_WINDOW, 0, 0, 0);
	return TRU;
    } else {
	return NIL;
    }
}

/* WM_SAVE_YOURSELF
 * if window participate in protocol, sends message and returns TRUE
 * else returns NIL
 */

WOOL_OBJECT
wool_save_yourself(argc, argv)
int argc;
WOOL_Number *argv;
{
    ClientWindow    cw = TargetWindow -> window;

    if (argc) {
	must_be_number(argv[0], 0);
	cw = ((ClientWindow) argv[0] -> number) -> window;
    }
    if (cw -> client && !cw -> client_wob
	&& cw -> cached_props -> wm_save_yourself) {
	send_protocol_message(cw -> client, XA_WM_SAVE_YOURSELF, 0, 0, 0);
	return TRU;
    } else {
	return NIL;
    }
}

/* Gravity: 123
 *          456
 *          789
 * (Gravity - 1) % 3: x: 0 1 2
 * (Gravity - 1) / 3: y: 0 1 2
 */

#ifndef NOBASEDIMS
/* obeys the Window Gravity hints
 * supposes that the hint is really set
 */

ObeyWinGravityHint(cw)
ClientWindow    cw;
{
    int x = cw -> box.x, y = cw -> box.y;

    switch((cw -> cached_props -> normal_hints.win_gravity - 1) % 3) {
    case 0:
	x = x + cw -> inner_x + cw -> box.borderwidth - 
	    (cw -> old_inner_borderwidth - cw -> inner_borderwidth);
	break;
    case 2:
	x = x - (cw -> box.borderwidth + cw -> box.width -
		 (cw -> inner_x + cw -> inner_borderwidth)) +
		     (cw -> old_inner_borderwidth - cw -> inner_borderwidth);
	break;
    }
    switch((cw -> cached_props -> normal_hints.win_gravity - 1) / 3) {
    case 0:
	y = y + cw -> inner_y + cw -> box.borderwidth - 
	    (cw -> old_inner_borderwidth - cw -> inner_borderwidth);
	break;
    case 2:
	y = y - (cw -> box.borderwidth + cw -> box.height -
		 (cw -> inner_y + cw -> inner_borderwidth)) +
		     (cw -> old_inner_borderwidth - cw -> inner_borderwidth);
	break;
    }
    if (cw -> box.x != x || cw -> box.y != y) {
	cw -> box.x = x;
	cw -> box.y = y;
	XMoveWindow(dpy, cw -> hook, x, y);
    }
}

#endif /* NOBASEDIMS */

/* misc utilities */

ReplaceDotWithUnderscores(string)
char * string;
{
    while (string = strchr(string, '.')) {
	*string = '_';
    }
}
