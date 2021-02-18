/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************************************************\
* 							  *
* 	BULL WINDOW MANAGER for X11 .			  *
* 							  *
* 	MODULE defining the ClientWindow Object Class.	  *
* 							  *
\*********************************************************/

 /* includes */
#include	<stdio.h>
#include	"EXTERN.h"
#include 	"wool.h"
#include	"wl_atom.h"
#include	"wl_string.h"
#include	"wl_list.h"
#include 	"wl_number.h"
#include 	"gwm.h"
#include	"wl_event.h"
#include 	"wl_fsm.h"
#include 	"wl_pixmap.h"
#include 	"wl_cursor.h"
#include 	"wl_client.h"
#ifdef SHAPE
#include	"X11/extensions/shape.h"
#endif /* SHAPE */

/*  local constants  */

/*  external  */

extern Wob      SetUpClientWindow();
extern Bar      NewBar(), BarOpen();
extern Wob      LookUpWob();
extern ClientWindow LookUpClient();
extern          UserAskWindow();
extern          UserMessage();
extern          FSMAction();
extern WOOL_METHOD WLMenu[];
extern XError(), NoXError();


/*  local  */

extern		ClientWindowEventHandler(), UpdateClientWindowGeometry();
extern		ReconfigureClientWindow();
ClientWindow	NewClientWindow(), NewClientWindowIcon(), 
		window_is_still_alive(), NewIconWindow();

extern ClientWindowOpen(), ClientWindowClose();

WOB_METHOD      ClientWindowClass[] = {
				       0,	/* METHODS_ARRAY */
				       WobEval,
				       WobPrint,
				       WobRelease,
				       WobExecute,
				       WobSet,
				       WobGetCValue,
				       ClientWindowOpen,
				       ClientWindowClose,
				       ClientWindowEventHandler,
				       (WOB_METHOD) wool_undefined_method_1,
				       WobGetDimensions,
				       (WOB_METHOD) wool_undefined_method_2,
				       (WOB_METHOD) wool_undefined_method_2,
				        ReconfigureClientWindow,
			(WOB_METHOD) wool_undefined_method_2,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1,
			(WOB_METHOD) wool_undefined_method_1
};

/* routines */

/*
 * To decorate a window, we must:
 * 	- see if it is decorable
 * 	- get all info about it
 * 	- decorate it via NewClientWindow
 */

ClientWindow
DecorateWindow(window, parent, must_save_set)
Window		window;
ClientWindow    parent;
int		must_save_set;		/* 0 for screen, 1 for windows */
{
    ClientWindow    cw;
    WOOL_Client	    wl_client;
    int local_zrt_size = zrt_size;
    XWindowAttributes wa;

    XSync(dpy, 0);
    ErrorStatus = 0;
    wa.override_redirect = 0;
    XGetWindowAttributes(dpy, window, &wa);
    if (wa.override_redirect || ErrorStatus) /* do not manage */
	return NULL;

    /* be careful, we might have decorated it in beetween */
    if(cw = (ClientWindow) LookUpClient(window))
    	return cw;
    GWM_window_being_decorated = cw = NewClientWindow(parent, window);
    SetTarget(cw);
    UpdateAllCachedProperties(cw);	/* some preliminar get_properties */
    cw -> colormap = wa.colormap;	/* update attributes */
    MatchWoolDescription(cw, &wl_client); /* get the descriptions */
    UpdateClientWindowFields(cw, wl_client);
    UpdateClientWindowGeometry(cw);	/* sets bars,etc */
    if (must_save_set && !(cw -> client_wob))	/* in case of GWM dying */
	XChangeSaveSet(dpy, window, SetModeInsert);
    ClientWindowRecordClient(cw);	/* register it */
    ClientWindowOpen(cw);		/* create the windows */
    zrt_gc(local_zrt_size);
    return cw;
}

/* The icon window is realized only on the first call to "iconify-window" or
 * "window-icon".
 */

ClientWindow
RealizeIconWindow(cw)
ClientWindow	cw;
{
    ClientWindow 	icon = NewIconWindow(cw);
    WOOL_Client 	wl_icon = (WOOL_Client) cw -> icon_description;

    SetTarget(icon);
    icon -> cached_props = cw -> cached_props;
    UpdateClientWindowFields(icon, wl_icon);
    MakeIconWindow(cw);		/* makes central plug */
    if(icon -> client)
    	ClientWindowRecordClient(icon);
    UpdateClientWindowGeometry(icon);
    cw -> icon_description = NULL;
    decrease_reference(wl_icon);
    UpdateClientWindowIconSize(icon);
    ClientWindowOpen(icon);
    return icon;    
}

/* Creates a wob for that new client window.
 */

ClientWindow
NewClientWindow(parent, window)
ClientWindow	parent;
Window		window;
{
    ClientWindow    cw = (ClientWindow) NewWob(sizeof(struct _ClientWindow));

    cw -> parent = (Wob) parent;
    cw -> screen = parent -> screen;
    cw -> status = ClientWindowStatus;
    cw -> client = window;
    cw -> window = cw;
    cw -> client_wob = LookUpWob(window);
    cw -> type = ClientWindowClass;
    return cw;
}

ClientWindow
NewIconWindow(cw)
ClientWindow	cw;
{
    ClientWindow    icon =
    (ClientWindow) NewWob(sizeof(struct _ClientWindow));

    cw -> icon = icon;
    icon -> parent = cw -> parent;
    icon -> screen = cw -> screen;
    icon -> window = cw;
    icon -> icon = icon;
    icon -> status = IconStatus;
    icon -> type = ClientWindowClass;
    return icon;
}

/*
 * updates now the properties cached in the ClientWindow structure and icon
 */

#define CWSTR(a) (((WOOL_String)(cw -> a)) -> string)

UpdateAllCachedProperties(cw)
ClientWindow    cw;
{
    Window          window = cw -> client;
    char           *machinename;
    XClassHint      classhints;
    Atom            actualtype;
    int             actualformat;
    unsigned long   nitems;
    unsigned long   bytesafter;

    CWCachedProperties cprops = (CWCachedProperties)
    Malloc(sizeof(struct _CWCachedProperties));

 					/* get all fixed properties */
    bzero(cprops, sizeof(struct _CWCachedProperties));
    cw -> cached_props = cprops;

    classhints.res_name = classhints.res_class = NULL;
    XGetWindowProperty(dpy, window, XA_WM_CLIENT_MACHINE, 0,
		       MAX_TEMP_STRING_SIZE,
		       False, XA_STRING, &actualtype, &actualformat,
		       &nitems, &bytesafter, &machinename);
    if (machinename) {
	increase_reference(cprops -> machinename = (WOOL_OBJECT)
			   WLString_make(machinename));
	XFree(machinename);
    } else
	increase_reference(cprops -> machinename = (WOOL_OBJECT)
			   DefaultMachineName);

    XGetClassHint(dpy, window, &classhints);
    increase_reference(cprops -> clientclass = (classhints.res_class ?
			   (WOOL_OBJECT) WLString_make(classhints.res_class)
				       : (WOOL_OBJECT) DefaultClientClass));
    increase_reference(cprops -> clientname = (classhints.res_name ?
			    (WOOL_OBJECT) WLString_make(classhints.res_name)
					: (WOOL_OBJECT) DefaultClientName));
    if (classhints.res_name)
	XFree(classhints.res_name);
    if (classhints.res_class)
	XFree(classhints.res_class);

 					/* update all variable props */
    Update_XA_WM_NAME(cw);
    Update_XA_WM_ICON_NAME(cw);
    Update_XA_WM_HINTS(cw);
    Update_XA_WM_NORMAL_HINTS(cw);
    Update_XA_WM_TRANSIENT_FOR(cw);
    Update_XA_WM_COLORMAP_WINDOWS(cw);
    Update_XA_WM_PROTOCOLS(cw);

    GetPreviousWM_STATE(cw);		/* get the previous WM_STATE if any */

    UpdateClientWindowSize(cw);		/* check window real size */
}

/*
 * update ONE property
 * cw MUST be a window, not an icon
 */

UpdateCachedProperty(cw, property_atom)
ClientWindow    cw;
Atom		property_atom;
{
    switch (property_atom) {		/* predefined atoms */
    case XA_WM_NAME:
	Update_XA_WM_NAME(cw);
	break;
    case XA_WM_ICON_NAME:
	Update_XA_WM_ICON_NAME(cw);
	break;
    case XA_WM_HINTS:
	Update_XA_WM_HINTS(cw);
	break;
    case XA_WM_NORMAL_HINTS:
	Update_XA_WM_NORMAL_HINTS(cw);
	break;
    case XA_WM_TRANSIENT_FOR:
	Update_XA_WM_TRANSIENT_FOR(cw);
	break;
    default:				/* non-predefined props */
	if (property_atom == XA_WM_COLORMAP_WINDOWS)
	    Update_XA_WM_COLORMAP_WINDOWS(cw);
	else if (property_atom == XA_WM_PROTOCOLS)
	    Update_XA_WM_PROTOCOLS(cw);
    }
}

/* individual methods to update or create cached properties
 */

Update_XA_WM_NAME(cw)
ClientWindow	cw;
{
    char           *name = 0;

    XFetchName(dpy, cw -> client, &(name));
    decrease_reference(cw -> cached_props -> windowname);
    if (!(name) || (*(name) == '\0'))
	increase_reference(cw -> cached_props -> windowname = (WOOL_OBJECT)
			   DefaultWindowName);
    else {
	ReplaceDotWithUnderscores(name);
	increase_reference(cw -> cached_props -> windowname = (WOOL_OBJECT)
			   WLString_make(name));
	XFree(name);
    }
}

Update_XA_WM_ICON_NAME(cw)
ClientWindow	cw;
{
    char           *name = 0;

    XGetIconName(dpy, cw -> client, &name);
    if (!(name) || (*(name) == '\0')) {
	increase_reference(cw -> cached_props -> iconname = (WOOL_OBJECT)
			   DefaultIconName);
    } else {
	increase_reference(cw -> cached_props -> iconname = (WOOL_OBJECT)
			   WLString_make(name));
	XFree(name);
    }
}

Update_XA_WM_HINTS(cw)
ClientWindow	cw;
{
    XWMHints       *windowhints;
    ClientWindow    group_leader;

    if (windowhints = XGetWMHints(dpy, cw -> client)) {
	if (GWM_backup_old_property)
	    bcopy(&(cw -> cached_props -> wm_hints),
	          &GWM_backup_of_old_property, sizeof(XWMHints));
	bcopy(windowhints, &(cw -> cached_props -> wm_hints),
	      sizeof(XWMHints));

	/* window groups */
	if ((windowhints -> flags & WindowGroupHint)
	    && windowhints -> window_group) {
	    if (windowhints -> window_group == cw->client)
		AddWindowToGroupLeader(cw, cw);
	    else if ((group_leader = LookUpClient(windowhints -> window_group))
		     || (windowhints -> window_group != Context -> root
			 &&
			 (group_leader = DecorateWindow(
			windowhints -> window_group, Context->rootWob, 1)))) {
		AddWindowToGroupLeader(cw, group_leader);
	    }
	}
	XFree(windowhints);
    }
}

Update_XA_WM_NORMAL_HINTS(cw)
ClientWindow	cw;
{
    if (!XGetNormalHints(dpy, cw -> client,
			 &(cw -> cached_props -> normal_hints))) {
	cw -> cached_props -> normal_hints.flags = 0;
    }
    CheckConsistency(&(cw -> cached_props -> normal_hints));
}

Update_XA_WM_TRANSIENT_FOR(cw)
ClientWindow	cw;
{
    ClientWindow    main_cw;

    decrease_reference(cw -> cached_props -> transient_for);
    XGetTransientForHint(dpy, cw -> client,
			 &(cw -> cached_props -> transient_for));
    if (cw -> cached_props -> transient_for) {
	if ((main_cw = LookUpClient(cw -> cached_props -> transient_for))
	    || (cw -> cached_props -> transient_for !=
		(WOOL_OBJECT) Context -> root
		&&
		cw -> cached_props -> transient_for !=
		(WOOL_OBJECT) cw -> client
		&&
		(main_cw = DecorateWindow(cw -> cached_props -> transient_for,
					 Context->rootWob, 1)))) {
	    increase_reference(cw -> cached_props -> transient_for =
			       (WOOL_OBJECT) WLNumber_make(main_cw));
	} else {
	    cw -> cached_props -> transient_for = 0;
	}
    }
}

Update_XA_WM_COLORMAP_WINDOWS(cw)
ClientWindow    cw;
{
    Atom            actualtype;
    int             actualformat;
    unsigned long   nitems;
    unsigned long   bytesafter;
    Window	   *colormap_windows = NULL, *old_colormap_windows;
    int		    result, i;

    result = XGetWindowProperty(dpy, cw -> client,
	    	XA_WM_COLORMAP_WINDOWS, 0, WM_COLORMAP_WINDOWS_PROP_Length,
    		False, XA_WM_COLORMAP_WINDOWS, &actualtype,
		&actualformat, &nitems, &bytesafter, &colormap_windows);

    if(result == Success && actualtype != None && actualformat == 32){
	old_colormap_windows = cw -> cached_props -> colormap_windows;
	cw -> cached_props -> colormap_windows =
	    (Window *) Malloc ((nitems+1) * sizeof (Window));
	cw -> cached_props -> colormap_windows_size = 1;
	cw -> cached_props -> colormap_windows[0] = cw -> client;
	for(i = 0; i< nitems; i++)
	    if(colormap_windows[i] != cw -> client
	       && !LookUpClient(colormap_windows[i])) {
	        cw -> cached_props -> colormap_windows[
	       	    cw -> cached_props -> colormap_windows_size++] =
			colormap_windows[i];
		if (colormap_windows[i] != cw -> client) {
		    XSelectInput(dpy, colormap_windows[i], ColormapChangeMask);
		}
	       }
	if(cw -> cached_props -> colormap_windows_size == 1){
	    Free(cw -> cached_props -> colormap_windows);
	    cw -> cached_props -> colormap_windows_size = 0;
	}
	cw -> cached_props -> colormap_windows_index = 0;
	if(old_colormap_windows)
	    Free(old_colormap_windows);
    }

    if(colormap_windows)
    	XFree(colormap_windows);
}

Update_XA_WM_PROTOCOLS(cw)
ClientWindow    cw;
{
    Atom            actualtype;
    int             actualformat;
    unsigned long   nitems;
    unsigned long   bytesafter;
    Window         *protocols = NULL;
    int             result, i;

    result = XGetWindowProperty(dpy, cw -> client,
				XA_WM_PROTOCOLS, 0, WM_PROTOCOLS_PROP_Length,
				False, XA_ATOM, &actualtype,
			   &actualformat, &nitems, &bytesafter, &protocols);

    if (result == Success && actualtype != None && actualformat == 32) {
	cw -> cached_props -> wm_take_focus = 0;
	cw -> cached_props -> wm_save_yourself = 0;
	cw -> cached_props -> wm_delete_window = 0;
	for (i = 0; i < nitems; i++) {
	    if (protocols[i] == XA_WM_TAKE_FOCUS)
		cw -> cached_props -> wm_take_focus = 1;
	    else if (protocols[i] == XA_WM_SAVE_YOURSELF)
		cw -> cached_props -> wm_save_yourself = 1;
	    else if (protocols[i] == XA_WM_DELETE_WINDOW)
		cw -> cached_props -> wm_delete_window = 1;
	}
    }
    if (protocols)
	XFree(protocols);
}

/*
 * release their storage
 */

FreeCachedProperties(cprops)
CWCachedProperties cprops;
{
    decrease_reference(cprops -> machinename);
    decrease_reference(cprops -> clientclass);
    decrease_reference(cprops -> clientname);
    decrease_reference(cprops -> windowname);
    decrease_reference(cprops -> iconname);
    decrease_reference(cprops -> transient_for);
    DelayedFree(cprops);
}

/* Updating WM_STATE is setting it on the window
 */

Update_XA_WM_STATE(cw)
ClientWindow    cw;
{
    struct _WM_STATE_PROP wm_state;
    int             trap_x_errors = 0;

    cw = cw -> window;
    if (!(cw -> client_wob || cw -> cached_props -> user_icon)) {
	if (cw -> mapped)
	    wm_state.state = WM_STATE_Normal;
	else if (cw -> icon && cw -> icon -> mapped)
	    wm_state.state = WM_STATE_Iconified;
	else {
	    wm_state.state = WM_STATE_Withdrawn;
	    trap_x_errors = 1;
	}
	if (wm_state.state != cw -> cached_props -> wm_state) {
	    cw -> cached_props -> wm_state = wm_state.state;
	    wm_state.icon = (cw -> icon ?
			     cw -> icon -> hook :
			     (cw -> cached_props -> user_icon ?
			      cw -> cached_props -> user_icon -> hook : 0));
	    if (trap_x_errors)
		XSetErrorHandler(NoXError);
	    XChangeProperty(dpy, cw -> client, XA_WM_STATE, XA_WM_STATE,
		      32, PropModeReplace, &wm_state, WM_STATE_PROP_Length);
	    if (trap_x_errors) {
		XSync(dpy, 0);
		XSetErrorHandler(XError);
	    }
	}
    }
}
    
/* first time a window is mapped, decode the WM_STATE from previous wm
 */

GetPreviousWM_STATE(cw)
ClientWindow	cw;
{
    Atom            actualtype;
    int             actualformat;
    unsigned long   nitems;
    unsigned long   bytesafter;
    WM_STATE_PROP   wm_state;

    if (Success == XGetWindowProperty(dpy, cw -> client, XA_WM_STATE, 0,
		      WM_STATE_PROP_Length, False, XA_WM_STATE, &actualtype,
			  &actualformat, &nitems, &bytesafter, &wm_state)) {
	if (wm_state && (wm_state -> state == WM_STATE_Iconified
	    || wm_state -> state == WM_STATE_Withdrawn)) {
	    cw -> cached_props -> wm_hints.flags |= StateHint;
	    cw -> cached_props -> wm_hints.initial_state =
		wm_state -> state;
	}
    }
}

/*
 * copies information from descriptor to client
 */

UpdateClientWindowBar(parent, field, wl_bar, dir)
ClientWindow    parent;
Bar 	       *field;
WOOL_Bar	wl_bar;
int		dir;
{
    wl_bar = (WOOL_Bar) wool_type_or_evaluate(wl_bar, WLBar);
    if (wl_bar == (WOOL_Bar) NIL)
	*field = NULL;
    else {
	*field = NewBar(parent, wl_bar);
	(*field) -> direction = dir;
    }
}

UpdateClientWindowFields(cw, wl_client)
ClientWindow    cw;
WOOL_Client     wl_client;
{
    cw -> inner_borderwidth = wl_client -> inner_borderwidth;
    cw -> box.borderwidth = wl_client -> borderwidth;
    cw -> box.borderpixel = wl_client -> borderpixel;
    cw -> box.background = wl_client -> background;
    UpdateClientWindowBar(cw, &(cw -> titlebar), wl_client -> titlebar,
			  HORIZONTAL);
    UpdateClientWindowBar(cw, &(cw -> leftbar), wl_client -> leftbar,
			  VERTICAL);
    UpdateClientWindowBar(cw, &(cw -> rightbar), wl_client -> rightbar,
			  VERTICAL);
    UpdateClientWindowBar(cw, &(cw -> basebar), wl_client -> basebar,
			  HORIZONTAL);
    increase_reference(cw -> cursor =
		       wool_type_or_evaluate(wl_client -> cursor, WLCursor));
    increase_reference(cw -> bordertile =
		  wool_type_or_evaluate(wl_client -> bordertile, WLPixmap));
    increase_reference(cw -> fsm =
		       wool_type_or_evaluate(wl_client -> fsm, WLFsm));
    increase_reference(cw -> menu =
		       wool_type_or_evaluate(wl_client -> menu, WLMenu));
    increase_reference(cw -> property = (WOOL_OBJECT) wl_client -> property);
    increase_reference(cw -> opening = (WOOL_OBJECT) wl_client -> opening);
    increase_reference(cw -> closing = (WOOL_OBJECT) wl_client -> closing);
    increase_reference(cw -> grabs = (WOOL_OBJECT) wl_client -> grabs);
    increase_reference(cw -> icon_plug =
		       (WOOL_OBJECT) wl_client -> icon_plug);
}

/*
 * here we create or get the XWindow to put in the icon -> client field
 */

MakeIconWindow(cw)
ClientWindow cw;
{
    WOOL_Plug       wl_plug = (WOOL_Plug) cw -> icon -> icon_plug;
    Plug            plug;

    wl_plug = (WOOL_Plug) WOOL_send(WOOL_eval, wl_plug, (wl_plug));
    if (wl_plug -> type == WLNumber) {	/* icon window managed by client */
	cw -> icon -> client = (Window)
	    ((WOOL_Number) wl_plug) -> number;
    } else if (wl_plug -> type == WLPlug) {	/* icon field is plug */
	plug = (Plug) NewPlug(Context->rootWob, wl_plug);
	UpdatePlugGeometry(plug);
	PlugOpen(plug);
	cw -> icon -> client = plug -> hook;
	cw -> icon -> client_wob = (Wob) plug;
    }
}

/*
 * we set here the size of the cw before calling MatchWool, for dims queries
 */

UpdateClientWindowSize(cw)
ClientWindow    cw;
{
    Window          root;
    int             depth, width, height;
    int             must_move = 0, must_resize = 0;

    XGetGeometry(dpy, cw -> client, &root,
		 &(cw -> box.x), &(cw -> box.y),
		 &(cw -> box.width), &(cw -> box.height),
		 &(cw -> box.borderwidth), &depth);
    /* update the position according to hints */
    cw -> inner_width = cw -> box.width;
    cw -> inner_height = cw -> box.height;
    if (!GWM_ProcessingExistingWindows) {
	if (cw -> cached_props -> normal_hints.flags & USPosition) {
	    if (cw -> box.x != cw -> cached_props -> normal_hints.x) {
		must_move = 1;
		cw -> box.x = cw -> cached_props -> normal_hints.x;
	    }
	    if (cw -> box.y != cw -> cached_props -> normal_hints.y) {
		must_move = 1;
		cw -> box.y = cw -> cached_props -> normal_hints.y;
	    }
	} else if (cw -> cached_props -> normal_hints.flags & PPosition) {
	    if (cw -> box.x != cw -> cached_props -> normal_hints.x) {
		must_move = 1;
		cw -> box.x = cw -> cached_props -> normal_hints.x;
	    }
	    if (cw -> box.y != cw -> cached_props -> normal_hints.y) {
		must_move = 1;
		cw -> box.y = cw -> cached_props -> normal_hints.y;
	    }
	} 
    }
    /* update the size according to hints */
    if (!GWM_ProcessingExistingWindows) {
	if (cw -> cached_props -> normal_hints.flags & USSize) {
	    if (cw -> inner_width != cw -> cached_props ->
	    	normal_hints.width) {
		must_resize = 1;
		cw -> inner_width = cw -> cached_props -> normal_hints.width;
	    }
	    if (cw -> inner_height != cw -> cached_props ->
	        normal_hints.height) {
		must_resize = 1;
		cw -> inner_height = cw -> cached_props ->
			normal_hints.height;
	    }
	} else if (cw -> cached_props -> normal_hints.flags & PSize) {
	    if (cw -> inner_width != cw -> cached_props ->
	    	normal_hints.width) {
		must_resize = 1;
		cw -> inner_width = cw -> cached_props -> normal_hints.width;
	    }
	    if (cw -> inner_height != cw -> cached_props ->
	        normal_hints.height) {
		must_resize = 1;
		cw -> inner_height = cw -> cached_props ->
			normal_hints.height;
	    }
	} else {
	    width = cw -> inner_width;
	    height = cw -> inner_height;
	    conform_to_hints(&(cw -> cached_props -> normal_hints),
	    			&width, &height);
	    if ((cw -> inner_width != width) ||
		(cw -> inner_height != height)) {
		must_resize = 1;
		cw -> inner_width = width;
		cw -> inner_height = height;
	    }
	}
    }
    if (must_move) {			/* MOVE HOOK */
	if (must_resize)
	    XMoveResizeWindow(dpy, cw -> client, cw -> box.x, cw -> box.y,
			      cw -> inner_width, cw -> inner_height);
	else
	    XMoveWindow(dpy, cw -> client, cw -> box.x, cw -> box.y);
        SendSyntheticMoveEvent(cw);
    }
    else if (must_resize)
	XResizeWindow(dpy, cw -> client,
		      cw -> inner_width, cw -> inner_height);
}

/* before opening an icon, place it according to hints
 */

UpdateClientWindowIconSize(icon)
ClientWindow icon;
{
    if (icon -> cached_props -> wm_hints.flags & IconPositionHint) {
	icon -> box.x = icon -> cached_props -> wm_hints.icon_x;
	icon -> box.y = icon -> cached_props -> wm_hints.icon_y;
    }
}

/*
 * Here we add physically all the gadgets around a client window.
 * A new window is opened. (First Map Request, called from SetUpClient.)
 * The description of the window to be created is in setup.
 *     Create the surrounding window.
 *     Set up window geometry, bars, plugs if any,
 *     reparent the client window.
 */

ClientWindowOpen(cw)
ClientWindow    cw;
{
    check_window_size(cw);
    /* create the WOB windows, and map them */
    cw -> curstate = (int) WOOL_send(WOOL_open, cw -> fsm, (cw -> fsm));
    cw -> hook =
	XCreateSimpleWindow(dpy, Context -> root,
			    cw -> box.x, cw -> box.y,
			    cw -> box.width, cw -> box.height,
			    cw -> box.borderwidth,
			    cw -> box.borderpixel,
			    ParentRelative);
    cw -> status |= TopLevelXWindowStatus;
    WobRecordHook(cw);
    BarOpen(cw -> titlebar);
    BarOpen(cw -> leftbar);
    BarOpen(cw -> rightbar);
    BarOpen(cw -> basebar);

    if (cw -> cursor != NIL)
	XDefineCursor(dpy, cw -> hook,
		      ((WOOL_Cursor) cw -> cursor) -> cursor);
    if (cw -> bordertile != NIL)
	XSetWindowBorderPixmap(dpy, cw -> hook,
			       ((WOOL_Pixmap) cw -> bordertile) -> pixmap);

    if (cw -> client) {			/* Reparent client Window */
	XUnmapWindow(dpy, cw -> client);
#ifdef SHAPE
	/* if we decorate a shaped window, reshape ourselves */
	if (cw -> client_shaped)
	    ClientWindowSetShape(cw);
	else
#endif /* SHAPE */
	    XSetWindowBorderWidth(dpy, cw -> client, cw -> inner_borderwidth);
	XReparentWindow(dpy, cw -> client, cw -> hook,
			cw -> inner_x, cw -> inner_y);
    }
    if (cw -> client_wob) {
	cw -> client_wob -> parent = (Wob) cw;
	cw -> client_wob -> status &= ~TopLevelXWindowStatus;
	cw -> client_wob -> input_mask |= ClientClientMask;
	XSelectInput(dpy, cw -> client, cw -> client_wob -> input_mask);
    } else {
	XSelectInput(dpy, cw -> client, ClientClientMask);
    }
    XSelectInput(dpy, cw -> hook, ClientMask |
		 ((WOOL_Fsm) cw -> fsm) -> mask);
    /* set global grabs */
    WLClient_set_grabs(cw -> grabs, cw -> hook);
    /* put window in window list */
    Context -> WindowCount++;
    if (Context -> rootWob -> next)
	Context -> rootWob -> next -> previous = cw;
    cw -> next = Context -> rootWob -> next;
    Context -> rootWob -> next = cw;
    cw -> previous = Context -> rootWob;

#ifndef NOBASEDIMS
    if (cw -> cached_props -> normal_hints.flags & PWinGravity)
	ObeyWinGravityHint(cw);
#endif /* NOBASEDIMS */

    XSync(dpy, 0);
    GWM_window_being_decorated = 0;

    /* process pending events */
    if (!GWM_ProcessingExistingWindows)
	GWM_ProcessEvents(0);

    /* execute "opening" WOOL code */
    SetTarget(cw);
    GWM_Window_being_opened = cw;
    wool_eval_and_catch_errors(cw -> opening);
    GWM_Window_being_opened = 0;
    decrease_reference(cw -> opening);
    cw -> opening = NULL;
}

/*
 * Take a client window, recursivly set geometry by the sons, and then
 * set the bars geometry
 */

UpdateClientWindowGeometry(cw)
ClientWindow	cw;
{
    UpdateBarWidth(cw -> titlebar);
    UpdateBarWidth(cw -> leftbar);
    UpdateBarWidth(cw -> rightbar);
    UpdateBarWidth(cw -> basebar);

    ClientWindowComputeGeometry(cw);
    ClientWindowComputeBarDims(cw);

    UpdateBarLength(cw -> titlebar);
    UpdateBarLength(cw -> leftbar);
    UpdateBarLength(cw -> rightbar);
    UpdateBarLength(cw -> basebar);

    /* now we must stay in place */

    cw -> box.x -= cw -> box.borderwidth + cw -> inner_borderwidth +
	cw -> inner_x - cw -> old_inner_borderwidth;
    cw -> box.y -= cw -> box.borderwidth + cw -> inner_borderwidth +
	cw -> inner_y - cw -> old_inner_borderwidth;
}

ClientWindowComputeGeometry(cw)
ClientWindow cw;
{
    Window          root;
    int             client_x, client_y;
    unsigned int    client_width, client_height;
    unsigned int    client_borderwidth, client_depth;

    if (cw -> client) {
	XGetGeometry(dpy, cw -> client,
		 &root, &client_x, &client_y, &client_width, &client_height,
		     &client_borderwidth, &client_depth);
#ifdef SHAPE
	/* if we decorate a shaped window, do not touch borderwidth */
	
	if (GWM_ShapeExtension) {
	    int xws, yws, xbs, ybs;
	    unsigned int wws, hws, wbs, hbs;
	    int boundingShaped, clipShaped;
	    
	    XShapeSelectInput (dpy, cw -> client, True);
	    XShapeQueryExtents (dpy, cw -> client,
				&boundingShaped, &xws, &yws, &wws, &hws,
				&clipShaped, &xbs, &ybs, &wbs, &hbs);
	    if (boundingShaped) {
		cw -> client_shaped = 1;
		cw -> inner_borderwidth = client_borderwidth = 0;
	    }
	}
#endif /* SHAPE */
	cw -> old_inner_borderwidth = client_borderwidth;
	if (cw -> inner_borderwidth == ANY)
	    cw -> inner_borderwidth = client_borderwidth;
	else
	    client_borderwidth = cw -> inner_borderwidth;
	cw -> inner_width = client_width;
	cw -> inner_height = client_height;
	cw -> box.x = client_x + client_borderwidth;
	cw -> box.y = client_y + client_borderwidth;
    } else {
	NaturalBarLength(cw -> titlebar);
	NaturalBarLength(cw -> leftbar);
	NaturalBarLength(cw -> rightbar);
	NaturalBarLength(cw -> basebar);
	cw -> box.x = cw -> box.y = 0;
	if (cw -> leftbar || cw -> rightbar) {
	    cw -> inner_width = 0;
	    cw -> inner_height = Max(
			  (cw -> leftbar ? cw -> leftbar -> box.height : 0),
		       (cw -> rightbar ? cw -> rightbar -> box.height : 0));
	    cw -> inner_borderwidth =
		Max((cw -> leftbar ? cw -> leftbar -> box.borderwidth : 0),
		  (cw -> rightbar ? cw -> rightbar -> box.borderwidth : 0));
	} else {
	    cw -> inner_height = 0;
	    cw -> inner_width = Max(
			 (cw -> titlebar ? cw -> titlebar -> box.width : 0),
			  (cw -> basebar ? cw -> basebar -> box.width : 0));
	    cw -> inner_borderwidth =
		Max((cw -> titlebar ? cw -> titlebar -> box.borderwidth : 0),
		    (cw -> basebar ? cw -> basebar -> box.borderwidth : 0));
	}
    }
}

ClientWindowComputeBarDims(cw)
ClientWindow cw;
{
    int             titleheight, baseheight, leftwidth, rightwidth;

    cw -> box.width = cw -> inner_width + 2 * cw -> inner_borderwidth;
    cw -> box.height = cw -> inner_height + 2 * cw -> inner_borderwidth;

    titleheight = (cw -> titlebar ? cw -> titlebar -> box.height
	+ 2 * cw -> titlebar -> box.borderwidth : 0);
    baseheight = (cw -> basebar ? cw -> basebar -> box.height
	+ 2 * cw -> basebar -> box.borderwidth : 0);
    leftwidth = (cw -> leftbar ? cw -> leftbar -> box.width
	+ 2 * cw -> leftbar -> box.borderwidth : 0);
    rightwidth = (cw -> rightbar ? cw -> rightbar -> box.width
	+ 2 * cw -> rightbar -> box.borderwidth : 0);

    /* sets the dimensions of the box itself */
    cw -> inner_x = leftwidth;
    cw -> inner_y = titleheight;
    cw -> box.width += leftwidth + rightwidth;
    cw -> box.height += titleheight + baseheight;

    /* set the titlebar (height and borderwidth are already set) */
    if(cw -> titlebar) {
    cw -> titlebar -> box.x = 0;
    cw -> titlebar -> box.y = 0;
    cw -> titlebar -> box.width =
	cw -> box.width - 2 * cw -> titlebar -> box.borderwidth;
    }
    /* set the leftbar (width and borderwidth are already set) */
    if(cw -> leftbar) {
    cw -> leftbar -> box.x = 0;
    cw -> leftbar -> box.y = titleheight;
    cw -> leftbar -> box.height = cw -> box.height
	- titleheight - baseheight
	- 2 * cw -> leftbar -> box.borderwidth;
    }
    /* set the rightbar (width and borderwidth are already set) */
    if(cw -> rightbar) {
    cw -> rightbar -> box.x = cw -> box.width
	- 2 * cw -> rightbar -> box.borderwidth
	- cw -> rightbar -> box.width;
    cw -> rightbar -> box.y = titleheight;
    cw -> rightbar -> box.height = cw -> box.height
	- titleheight - baseheight
	- 2 * cw -> rightbar -> box.borderwidth;
    }
    /* set the basebar (height and borderwidth are already set) */
    if(cw -> basebar) {
    cw -> basebar -> box.x = 0;
    cw -> basebar -> box.y = cw -> box.height - baseheight;
    cw -> basebar -> box.width =
	cw -> box.width - 2 * cw -> basebar -> box.borderwidth;
    }
}

/* destroys all data concerning the window
 * suppose that the client window is already destroyed, so we do not need to
 * destroy it ourselves
 */

ClientWindowClose(cw)
ClientWindow    cw;
{
    ClientWindow    icon = cw -> icon;
    CWCachedProperties cprops = cw -> cached_props;

    if (cprops -> client_group)
	RemoveWindowFromGroup(cw);
    ClientWindowClose_aux(cw -> window);
    if (icon)
	ClientWindowClose_aux(icon);
    FreeCachedProperties(cprops);
    if (TargetWindow == cw || TargetWindow == icon)
	SetTarget(Context -> rootWob);
}

ClientWindowClose_aux(cw)
ClientWindow	cw;
{
    SetTarget(cw);
    if (!(GWM_is_ending || GWM_is_restarting) && cw -> closing)
	wool_eval_and_catch_errors(cw -> closing);
    BarClose(cw -> titlebar);
    BarClose(cw -> leftbar);
    BarClose(cw -> rightbar);
    BarClose(cw -> basebar);
    decrease_reference(cw -> opening);
    decrease_reference(cw -> closing);
    decrease_reference(cw -> grabs);
    decrease_reference(cw -> icon_plug);
    decrease_reference(cw -> icon_description);
    /* remove from list */
    if (cw -> previous) {
	Context -> WindowCount--;
	cw -> previous -> next = cw -> next;
	if (cw -> next)
	    cw -> next -> previous = cw -> previous;
    }
    if (cw -> client)
	XDeleteContext(dpy, cw -> client, client_context);
    WobRelease(cw);
}

ClientWindowMap(cw)
ClientWindow	cw;
{
    if (cw && !(cw -> mapped)) {
	if (cw -> client)
	    XMapWindow(dpy, cw -> client);
	XMapWindow(dpy, cw -> hook);
	cw -> mapped = 1;
	Update_XA_WM_STATE(cw);
    }
    return;
}

ClientWindowUnmap(cw)
ClientWindow	cw;
{
    if (cw && cw -> mapped) {
	XUnmapWindow(dpy, cw -> hook);
	if (cw -> client && !cw -> client_wob)
	    XUnmapWindow(dpy, cw -> client);
	cw -> mapped = 0;
	Update_XA_WM_STATE(cw);
    }
    return;
}

ClientWindowInitialMap(cw)
ClientWindow	cw;
{
    int             mapping = 1;

    if (!cw)
	return;
    if (cw -> cached_props -> wm_hints.flags & StateHint) {
	switch (cw -> cached_props -> wm_hints.initial_state) {
	case WM_STATE_Iconified:
	    if (!(cw -> icon))
		RealizeIconWindow(cw);
	    cw = cw -> icon;
	    break;
	case WM_STATE_Withdrawn:
	    if (!GWM_Mapall)
	    	mapping = 0;
	    break;
	}
    }
    if (mapping) {
	XMapWindow(dpy, cw -> hook);
	cw -> mapped = 1;
	if (cw -> client)
	    XMapWindow(dpy, cw -> client);
    }
    Update_XA_WM_STATE(cw);
}

int
Mapped(w) 
Window w;
{
    XWindowAttributes wa;

    XGetWindowAttributes(dpy, w, &wa);
    return (wa.map_state != IsUnmapped);
}

ClientWindowEventHandler(cw, evt)
ClientWindow	cw;
Event		evt;
{
    switch (evt -> type) {
    case MapRequest:
	XMapWindow(dpy, cw -> client);
    case MapNotify:
	if (!(cw -> opening) && !cw -> mapped &&
	    !(cw -> status & IconStatus)) {	/* client is mapping its
						 * window */
	    if (cw -> icon) {
		SetTarget(cw -> icon);
		WOOL_send(WOOL_eval, WL_iconify_window_call,
			  (WL_iconify_window_call));
	    } else {
		XMapWindow(dpy, cw -> hook);
		cw -> mapped = 1;
		Update_XA_WM_STATE(cw);
	    }
	}
	break;

    case UnmapNotify:
	if (cw -> mapped) {
	    XUnmapWindow(dpy, cw -> hook);
	    cw -> mapped = 0;
	    Update_XA_WM_STATE(cw);
	}
	break;

    case ConfigureRequest:
	ConfigureRequestEventHandler(cw, evt);
	break;

    case DestroyNotify:
	if (evt -> xdestroywindow.window == cw -> client) {
	    ClientWindowClose(cw);
	}
	break;

    case ColormapNotify:		/* if is the active one, re-install */
	if (evt -> xcolormap.new) {
	    Colormap        old_colormap = cw -> colormap;

	    SetTarget(cw);
	    cw -> colormap = evt -> xcolormap.colormap;
	    if (Context -> InstalledColormapCW == cw)
		if (cw -> colormap)
		    XInstallColormap(dpy, cw -> colormap);
		else if (old_colormap)
		    XInstallColormap(dpy, Context -> rootWob -> colormap);
	}				/* else install/uninstall result */
	break;

    case GWMUserEvent:
	WLFsm_action(cw -> fsm, cw, evt);
	if (GWM_Propagate_user_events) {
	    if (cw -> client_wob)
		WOOL_send(WOOL_process_event, cw -> client_wob,
			  (cw -> client_wob, evt));
	    if (cw -> titlebar)
		WOOL_send(WOOL_process_event, cw -> titlebar,
			  (cw -> titlebar, evt));
	    if (cw -> leftbar)
		WOOL_send(WOOL_process_event, cw -> leftbar,
			  (cw -> leftbar, evt));
	    if (cw -> rightbar)
		WOOL_send(WOOL_process_event, cw -> rightbar,
			  (cw -> rightbar, evt));
	    if (cw -> basebar)
		WOOL_send(WOOL_process_event, cw -> basebar,
			  (cw -> basebar, evt));
	}
	break;

    case ClientMessage:		/* iccc: for iconifying window */
	SetTarget(cw);
	if (evt -> xclient.message_type == XA_WM_CHANGE_STATE
	    && evt -> xclient.data.l[0] == IconicState) {
	    WOOL_send(WOOL_eval, WL_iconify_window_call,
		      (WL_iconify_window_call));
	    /* iccc: for deleting window */
	} else if (evt -> xclient.message_type == XA_WM_PROTOCOLS
		&& evt -> xclient.data.l[0] == (long) XA_WM_DELETE_WINDOW) {
	    ClientWindowClose(cw);
	} else {
	    WLFsm_action(cw -> fsm, cw, evt);
	}
	break;

    case PropertyNotify:
	SetTarget(cw);
	GWM_backup_old_property = 1;
	UpdateCachedProperty(cw -> window, evt -> xproperty.atom);
	GWM_backup_old_property = 0;
	/* no break here is purposeful! */

    default:

#ifdef SHAPE
	if (GWM_ShapeExtension &&
	    evt -> type == GWM_ShapeEventBase + ShapeNotify)
	    ShapeNotifyEventHandler(cw, evt);
	else
#endif /* SHAPE */
	    {
		WLFsm_action(cw -> fsm, cw, evt);
	    }
    }
}

/*
 * ConfigureRequestEventHandler is used to decode what operation is really
 * intended by the configure event to only perform the appropriate action
 */
/* TO_DO: do only one XConfigure...*/

ConfigureRequestEventHandler(cw, evt)
ClientWindow    cw;
XConfigureRequestEvent           *evt;
{
    XWindowChanges  values;

    /* move: move hook */		/* MOVE HOOK */
    if (evt -> value_mask & (CWX | CWY)) {
	XMoveWindow(dpy, cw -> hook,
		    (evt -> value_mask | CWX ?
		     cw -> box.x = evt -> x - cw -> inner_x
		     - cw -> box.borderwidth : cw -> box.x),
		    (evt -> value_mask | CWY ?
		     cw -> box.y = evt -> y - cw -> inner_y
		     - cw -> box.borderwidth : cw -> box.y));
    	SendSyntheticMoveEvent(cw);
    }
    /* resize is handled via recomputing */
    if (evt -> value_mask & (CWWidth | CWHeight | CWBorderWidth)) {
	values.width = evt -> width;
	values.height = evt -> height;
	values.border_width = evt -> border_width;
	XConfigureWindow(dpy, cw -> client,
		   evt -> value_mask & (CWWidth | CWHeight | CWBorderWidth),
			 &values);
	ReconfigureClientWindow(cw, cw -> client);
	/* TO_DO Here send resize event */
    }
    /* stacking change is independant of the rest and done last */
    if (evt -> value_mask & CWStackMode) {
	Wob             sibling = 0;

	if (evt -> value_mask & CWSibling) {	/* % to a sibling */
	    sibling = (Wob) LookUpClient(evt -> above);
	    if (!sibling)
		sibling = (Wob) LookUpWob(evt -> above);
	}
	values.sibling = (Window) sibling;
	values.stack_mode = evt -> detail;
	XConfigureWindow(dpy, cw -> hook,
	      evt -> value_mask & (CWStackMode | (sibling ? CWSibling : 0)),
			 &values);
    }
}

UnDecorateWindow(cw)
ClientWindow	cw;
{
    Window          client = cw -> window -> client;
    Wob             wob = cw -> window -> client_wob;

    int             oldbw = cw -> window -> old_inner_borderwidth;

    cw = cw -> window;
    if (wob) {
	wob -> parent = (Wob) Context -> rootWob;
    }
    Update_XA_WM_STATE(cw);
    if (cw -> mapped)
	XUnmapWindow(dpy, cw -> client);
    XReparentWindow(dpy, client, Context -> root,
		    cw -> box.x + cw -> inner_borderwidth - oldbw
		    + cw -> inner_x
		    + cw -> box.borderwidth,
		    cw -> box.y + cw -> inner_borderwidth - oldbw
		    + cw -> inner_y
		    + cw -> box.borderwidth);
    if (!cw -> client_shaped)
	XSetWindowBorderWidth(dpy, client, oldbw);
    if (cw -> client_wob)
	cw -> client_wob -> status &= TopLevelXWindowStatus;
    if (cw -> mapped)
	XMapWindow(dpy, cw -> client);
    ClientWindowClose(cw);
}

ReconfigureClientWindow(cw, culprit)
ClientWindow	cw;
Wob		culprit;	/* cw, client or bar */
{
    int             x = cw -> box.x, y = cw -> box.y;
    int             inner_x = cw -> inner_x, inner_y = cw -> inner_y;

    if (culprit == (Wob) cw -> client || culprit == cw -> client_wob) {
	ClientWindowComputeGeometry(cw);
    }
    ClientWindowComputeBarDims(cw);
    if (culprit != (Wob) cw -> titlebar)
	ReconfigureBar(cw -> titlebar, cw);
    if (culprit != (Wob) cw -> leftbar)
	ReconfigureBar(cw -> leftbar, cw);
    if (culprit != (Wob) cw -> rightbar)
	ReconfigureBar(cw -> rightbar, cw);
    if (culprit != (Wob) cw -> basebar)
	ReconfigureBar(cw -> basebar, cw);
    XMoveResizeWindow(dpy, cw -> hook, cw -> box.x = x, cw -> box.y = y,
		      cw -> box.width, cw -> box.height);
    if (inner_x != cw -> inner_x || inner_y != cw -> inner_y)
	XMoveWindow(dpy, cw -> client, cw -> inner_x, cw -> inner_y);
#ifdef SHAPE
    /* do not reshape if client reshaped, it will send an event later */
    if (cw -> client_shaped && culprit != (Wob) cw -> client)
	ClientWindowSetShape(cw);
#endif /* SHAPE */
}

/*
 * transforms outer sizes in inner ones
 */

dims_outer_to_inner(cw, pw, ph)
ClientWindow	cw;
int *pw, *ph;
{
    *pw = *pw - (cw -> box.width - cw -> inner_width);
    *ph = *ph - (cw -> box.height - cw -> inner_height);
}

/****************\
* 		 *
* window groups	 *
* 		 *
\****************/

/* Maintains the group list of a group_leader
 * In the client_group field of the cw, we find:
 * 	- for the leaded, the cw of the leader.
 * 	- for the leader, the list (WOOL_List) of grouped sons
 * 	  first one being the leader itself
 */

AddWindowToGroupLeader(cw, group_leader)
ClientWindow cw, group_leader;
{
    WOOL_List      *gwlgp = (WOOL_List *) & (group_leader ->
    cached_props -> client_group);

    /* if leader's group is not yet initialised, do it */
    if (!(*gwlgp)) {
	if (cw != group_leader) {	/* follower */
	    AddWindowToGroupLeader(group_leader, group_leader);
	    AddWindowToGroupLeader(cw, group_leader);
	} else {		/* leader */
	    increase_reference(*gwlgp = wool_list_make(1));
	    increase_reference((*gwlgp) -> list[0] = (WOOL_OBJECT)
			       WLNumber_make(group_leader));
	}
	/* leader is initialized */
    } else if ((*gwlgp) -> type == WLList) {
	int             i;
	WOOL_List       old_list;

	for (i = 0; i < (*gwlgp) -> size; i++)
	    if ((*gwlgp) -> list[i] == (WOOL_OBJECT) cw)
		return;
	old_list = *gwlgp;
	increase_reference(*gwlgp = wool_list_make((*gwlgp) -> size + 1));
	copy_n_objects(old_list -> list, (*gwlgp) -> list, old_list -> size);
	decrease_reference(old_list);
	increase_reference((*gwlgp) -> list[(*gwlgp) -> size - 1]
			   = (WOOL_OBJECT) WLNumber_make(cw));
	if((cw -> cached_props -> client_group)
            && (cw -> cached_props -> client_group -> type == WLList))
	    decrease_reference(cw -> cached_props -> client_group);
	cw -> cached_props -> client_group = (WOOL_OBJECT) group_leader;
    }
}

/* called when a window is closed if client_group not void
 */

RemoveWindowFromGroup(cw)
ClientWindow cw;
{
    WOOL_List      *gwlgp = (WOOL_List *) & (cw ->
			    cached_props -> client_group), list;
    int             i;
    ClientWindow    son;

    if ((*gwlgp) -> type == WLList) {	/* leader */
	list = (*gwlgp);
	for (i = 0; i < list -> size; i++) {
	    son = (ClientWindow) ((WOOL_Number) list -> list[i]) -> number;
	    son -> cached_props -> client_group = NULL;
	}
	decrease_reference(list);
    } else {			/* follower */
	list = (WOOL_List)
	((ClientWindow) (cw -> cached_props -> client_group)) ->
		cached_props -> client_group;

	cw -> cached_props -> client_group = NULL;
	i = 0;
	while (i < list -> size) {
	    if (((WOOL_Number) list -> list[i]) -> number == (Num) cw)
		break;
	    i++;
	}
	if (i != list -> size)
	    (list -> size)--;
	ASSERT(list -> size != 1);	/* list should at least have leader */
	while (i++ < list -> size)
	    list -> list[i - 1] = list -> list[i];
    }
}
	    
/*
 * wool interface
 */

WOOL_OBJECT
wool_window_group_get()
{
    ClientWindow    cw = TargetWindow -> window;

    if (cw -> cached_props -> client_group) {	/* group exists */
	if (cw -> cached_props -> client_group -> type == WLList) {/* lead */
	    return cw -> cached_props -> client_group;
	} else {		/* follower */
	    return ((ClientWindow) (cw -> cached_props -> client_group))
		    -> cached_props -> client_group;
	}
    } else {			/* no group */
	return NIL;
    }
}

WOOL_OBJECT
wool_window_group_set(group_leader_num)
WOOL_Number group_leader_num;
{
    ClientWindow    cw = TargetWindow -> window;
    ClientWindow    group_leader;

    if (group_leader_num -> type == WLNumber) {
	group_leader = (ClientWindow) group_leader_num -> number;
    } else if (group_leader_num -> type == WLList) {
	group_leader = (ClientWindow)
	    ((WOOL_Number) ((WOOL_List) group_leader_num) -> list[0])
	    -> number;
    } else if(cw-> cached_props -> client_group) {
	 RemoveWindowFromGroup(cw);
	 return NIL;
    }
    AddWindowToGroupLeader(cw, group_leader);
    return (WOOL_OBJECT) group_leader_num;
}

ClientWindowRecordClient(cw)
ClientWindow cw;
{
    XSaveContext(dpy, cw -> client, client_context, cw);
}

#ifdef SHAPE
/* non-rectangular extension:
 * set the frame to the shape of client +bars
 */

ClientWindowSetShape(cw)
ClientWindow cw;
{
    XShapeCombineShape (dpy, cw -> hook, ShapeBounding,
			cw -> inner_x, cw -> inner_y,
			cw -> client, ShapeBounding,
			ShapeSet);
    if (cw -> titlebar)
	XShapeCombineShape (dpy, cw -> hook, ShapeBounding,
			0, 0,
			cw -> titlebar -> hook, ShapeBounding,
			ShapeUnion);
    if (cw -> leftbar)
	XShapeCombineShape (dpy, cw -> hook, ShapeBounding,
			0, cw -> inner_y,
			cw -> leftbar -> hook, ShapeBounding,
			ShapeUnion);
    if (cw -> rightbar)
	XShapeCombineShape (dpy, cw -> hook, ShapeBounding,
			cw -> inner_x + cw -> inner_width
		        + 2 * cw -> inner_borderwidth, cw -> inner_y,
			cw -> rightbar -> hook, ShapeBounding,
			ShapeUnion);
    if (cw -> basebar)
	XShapeCombineShape (dpy, cw -> hook, ShapeBounding,
			0, cw -> inner_y + cw -> inner_height
		        + 2 * cw -> inner_borderwidth,
			cw -> titlebar -> hook, ShapeBounding,
			ShapeUnion);
}

/* what to do if shape changes
 */

ShapeNotifyEventHandler(cw, evt)
ClientWindow    cw;
XShapeEvent    *evt;
{
    if (evt -> kind == ShapeBounding && evt -> window == cw -> client) {
	cw -> client_shaped = evt -> shaped;
	if (evt -> shaped)
	    ClientWindowSetShape(cw);
	else
	    XShapeCombineMask (dpy, cw -> hook, ShapeBounding,
			       0, 0, None, ShapeSet);
    }
}

WOOL_OBJECT
wool_window_has_shaped_client()
{
    if (TargetWindow -> client_shaped)
	return TRU;
    else
	return NIL;
}

#endif /* SHAPE */
