#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)server.c 20.60 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <strings.h>
#include <xview/win_input.h>
#include <xview/win_struct.h>
#include <xview/notify.h>
#include <xview/win_notify.h>
#include <xview/defaults.h>
#include <xview_private/ntfy.h>
#include <xview_private/ndet.h>
#include <X11/Xlib.h>
#include <xview_private/svr_impl.h>
#include <xview_private/win_kmdata.h>
#include <xview_private/draw_impl.h>
#include <xview_private/xv_color.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>


extern char    *getenv();

Xv_private_data char *shell_prompt;


#ifdef _XV_DEBUG
Xv_private_data int server_gather_stats;
#endif

static void     server_init_atoms();
static Notify_value scheduler();
static Notify_func default_scheduler;	/* = 0 */

Xv_private Notify_value xv_input_pending();
Xv_private void xv_do_enqueued_input();

/* global default server parameters */
Xv_private_data Xv_Server xv_default_server;
Xv_private_data Xv_Screen xv_default_screen;
Xv_private_data Display *xv_default_display;

Pkg_private void server_init_modifier();

static unsigned int string_to_modmask();
static Server_atom_type save_atom();


static void
server_build_keymap_table(server)
    Server_info    *server;
{
    /* BUG: This should be improved to allow the user to bind in there own
     *      keymap, and semantic mapping table.  Or at least allow the user
     *      to rebind individual keys.			
     */
    server->xv_map = win_keymap;
    server->sem_map = win_keymap_sem;
    server->ascii_map = win_ascii_sem;
}


/*ARGSUSED*/
Pkg_private int
server_init(parent, server_public, avlist)
    Xv_opaque       parent;
    Xv_Screen       server_public;
    Attr_avlist     avlist;
{
    register Server_info	*server = (Server_info *) NULL;
    Xv_server_struct 		*server_object;
    int             		result = XV_ERROR;	/* Assume the worst */
    Attr_avlist    		attrs;
    char           		*home,	        /* pathname to home directory */
    				*server_name,
    				filename[128];	/* assume home filename
						   length < 118 chars */
    unsigned char		pmap[256];      /* pointer mapping list */

    server_name = (char *) defaults_get_string("server.name",
					       "Server.Name",
					       getenv("DISPLAY"));
    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {
	  case XV_NAME:
	    server_name = (char *) attrs[1];
	    *attrs = ATTR_NOP(*attrs);
	    break;
	  default:
	    break;
	}
    }

    /* Allocate private data and set up forward/backward links. */
    server = (Server_info *) xv_alloc(Server_info);
    server->public_self = server_public;
    server_object = (Xv_server_struct *) server_public;
    server_object->private_data = (Xv_opaque) server;

    if (!(server->xdisplay = server_init_x(server_name))) {
	goto Error_Return;
    }
    /* Create keycode maps */
    (void) server_build_keymap_table(server);

    if (notify_set_input_func(server->xdisplay, xv_input_pending,
			      win_server_fd(server->xdisplay))
	== NOTIFY_FUNC_NULL) {
	notify_perror("server_init");
	goto Error_Return;
    }
    /* Screen creation requires default server to be set. */
    if (!xv_default_server) {
	xv_default_server = server_public;
	xv_default_display = (Display *)server->xdisplay;
#ifdef X11_DEBUG
	if (server_gather_stats) {
	    XStartStat(xv_default_display);
	    fprintf(stdout, "Xlib statistics gathering on\n");
	}
#endif
    }
    /*
     * Now that a server connection has been established, initialize the
     * defaults database. Note - This assumes that server_init will be called
     * only once per X11 server.
     */
    defaults_init_db();		/* init Resource Manager */
    defaults_load_db((char *) NULL);	/* load server database */
    if (home = getenv("HOME"))
	(void) strcpy(filename, home);
    (void) strcat(filename, "/.Xdefaults");
    defaults_load_db(filename);	/* load user's database */

    server->screens[0] = xv_create(server_public, SCREEN,
				   SCREEN_NUMBER, 0,
				   NULL);
    if (!server->screens[0]) {
	goto Error_Return;
    }

    /*
     * On a color display check to see if colors were specifed for
     * control areas in the defaults file.
     */  
    if (XDisplayPlanes(server->xdisplay, 0) > 1) {
        char            *str;
        unsigned char    red[2], green[2], blue[2];
	Xv_cmsdata	 cms_data;
 
        str = (char *) defaults_get_string("OpenWindows.WindowColor",
					  "OpenWindows.WindowColor",
					  (char *) NULL); 
	/*
	 * The control color applies only to the background.
	 * The foreground is always black.
	 */
        if (str != NULL) {
	    xv_string_to_rgb(str, &red[0], &green[0], &blue[0]);
	} else {
	    red[0] = green[0] = blue[0] = 255;
	}
	red[1] = green[1] = blue[1] = 0;

	cms_data.type = XV_STATIC_CMS;
	cms_data.size = 2;
	cms_data.rgb_count = 2;
	cms_data.index = 0;
	cms_data.red = red;
	cms_data.green = green;
	cms_data.blue = blue;
	/* xv_set() interface required for screen_alloc_cms() */
	(void) screen_alloc_cms(server->screens[0], XV_CONTROL_CMS, &cms_data);
    }

    if (xv_default_server != server_public) {
	(void) XV_SL_ADD_AFTER(SERVER_PRIVATE(xv_default_server),
			SERVER_PRIVATE(xv_default_server), server);
    } else {
	XV_SL_INIT(server);
	xv_default_screen = server->screens[0];
	(void) xv_set_scheduler();
    }

    /* Info to keep time stamp for events */
    server->tv.tv_sec = server->tv.tv_usec = 0;
    /* Used in atom hash table */
    server->atom_context = (XID) XAllocID((Display *)server->xdisplay);
    server_init_atoms(server_public);
    selection_init_agent(server_public, xv_default_screen);
    server_init_modifier((Display *) server->xdisplay);

    /* Be prepared to handle a mouse with only one or two physical buttons */
    server->nbuttons = XGetPointerMapping(server->xdisplay, pmap, 256);
    if (server->nbuttons < 3)
	server->but_two_mod =
	      string_to_modmask(defaults_get_string("mouse.modifier.button2",
						    "Mouse.Modifier.Button2",
						    "Shift"));
    if (server->nbuttons < 2) 
	server->but_three_mod =
	      string_to_modmask(defaults_get_string("mouse.modifier.button3",
						    "Mouse.Modifier.Button3",
						    "Ctrl"));
    return XV_OK;

Error_Return:
    if (server) {
	if (xv_default_server == server_public) {
	    xv_default_server = (Xv_Server) NULL;
	}
	free((char *) server);
    }
    return (result);
}


Pkg_private int
server_destroy(server_public, status)
    Xv_Server       server_public;
    Destroy_status  status;
{
    /*
     * The Notifier knows about both screen and server objects.  When the
     * entire process is dying, the Notifier calls the destroy routines for
     * the objects in an arbitrary order.  We attempt to change the ordering
     * so that the screen(s) are destroyed before the server(s), so that the
     * screen(s) can always assume that the server(s) are valid. In addition,
     * destruction of a server causes destruction of every object attached to
     * that server.  [BUG ALERT!  Not currently implemented.]
     */
    Server_info    *server = SERVER_PRIVATE(server_public);
    int             i;

    /* Give screens a chance to clean up. */
    for (i = 0; i < MAX_SCREENS; i++) {
	if (server->screens[i]) {
	    if (NOTIFY_DESTROY_VETOED ==
		notify_post_destroy(server->screens[i], status,
				    NOTIFY_IMMEDIATE)) {
		return XV_ERROR;
	    }
	}
    }

    switch (status) {
      case DESTROY_PROCESS_DEATH:
      case DESTROY_CLEANUP:
#ifdef X11_DEBUG
	if (server_gather_stats && (xv_default_server == server_public)) {
	    XStopStat(xv_default_display);
	    fprintf(stdout, "Xlib statistics gathering off\n");
	    XPrintStat(stdout);
	}
#endif
	XCloseDisplay(server->xdisplay);
	/* Try to catch client or library errors. */
	server->xdisplay = 0;
	if (xv_default_server == server_public) {
	    (void) notify_set_scheduler_func(default_scheduler);
	    /* Remove our scheduler else will deref null server */
	    xv_default_server = (Xv_Server) NULL;
	    xv_default_display = (Display *) NULL;
	    xv_default_screen = (Xv_Screen) NULL;
	}
	if (status == DESTROY_CLEANUP) {
	    free((char *) XV_SL_REMOVE(SERVER_PRIVATE(server_public), server));
	}			/* else waste of time fixing up heap or list
				 * links. */
	break;
      default:
	break;
    }

    return XV_OK;
}

Xv_private void
xv_scheduler_internal()
{
    Server_info    *server;

    /* flush any output now */
    XV_SL_TYPED_FOR_ALL(SERVER_PRIVATE(xv_default_server), server,
			Server_info *) {
	xv_do_enqueued_input(server->xdisplay, FALSE);
	XFlush(server->xdisplay);	/* flush the output buffer */
    }
}

/*
 * invoke the default scheduler, then flush all servers.
 */
static          Notify_value
scheduler(n, clients)
    int             n;
    Notify_client   clients[];
{
    Notify_value    status = (default_scheduler) (n, clients);

    /*
     * WARNING: we only want to process events from servers when the notifier
     * is ready to run, not whenever the notifier gets called (e.g. as a
     * result of xv_destroy() calling notify_post_destroy()). The notifier is
     * ready to run either after xv_main_loop() calls notify_start(), or
     * after the client calls notify_do_dispatch() or notify_dispatch().
     */
    if ((status == NOTIFY_DONE) &&
	(ndet_flags & (NDET_STARTED | NDET_DISPATCH))) {
	xv_scheduler_internal();
    }
    return status;
}

static int
xv_set_scheduler()
{
    /*
     * register a scheduler and an input handler with the notifier for this
     * process.
     */
    default_scheduler = notify_set_scheduler_func(scheduler);
    if (default_scheduler == NOTIFY_FUNC_NULL) {
	notify_perror("xv_set_scheduler");
	return XV_ERROR;
    }
    return XV_OK;
}

static void
server_init_atoms(server_public)
    Xv_Server       server_public;
{
    Server_info    *server = SERVER_PRIVATE(server_public);
    Atom            atom;

    /*
     * do not create the SERVER_JOURNAL_ATOM atom if it does not already
     * exists
     */
    atom = XInternAtom(server->xdisplay, "JOURNAL_SYNC", TRUE);
    if (atom == BadValue || atom == BadAlloc) {
	xv_error(XV_NULL, XV_ERROR_NON_RECOVERABLE, 
		 XV_ERROR_INTERNAL, XV_ERROR_STRING,
		 "Can't create SERVER_JOURNAL_ATOM atom",
		 XV_ERROR_PKG, ATTR_PKG_SERVER,
		 NULL);
    }
    if (atom == None) {		/* not in journalling mode */
	server->journalling = FALSE;
    } else {			/* in journalling mode */
	int             status, actual_format;
	long            nitems, bytes;
	Atom            actual_type;
	char           *data;	/* default prompt */
	char           *shell_ptr;
	shell_prompt = (char *) calloc(40, sizeof(char));

	/* check to see if this property hangs of the root window */

	status = XGetWindowProperty(server->xdisplay,
			    DefaultRootWindow((Display *) server->xdisplay),
				atom, 0, 2, False, XA_INTEGER, &actual_type,
				    &actual_format, &nitems, &bytes, &data);

	if (status != Success || actual_type == None) {
	    server->journalling = FALSE;
	    XFree(data);
	} else {
	    server->journalling = TRUE;
	    if ((shell_ptr = getenv("PROMPT")) == NULL) {
		shell_prompt[0] = '%';
	    } else {
		(void) strcpy(shell_prompt, shell_ptr);
	    }
	    (void) xv_set(server_public, SERVER_JOURNAL_SYNC_ATOM, atom, NULL);
	}
    }
}

Xv_private      Server_atom_type
server_get_atom_type(server_public, atom)
    Xv_Server       server_public;
    Atom            atom;
{
    Server_atom_type    type;
    Server_info        *server = SERVER_PRIVATE(server_public);

    if (XFindContext(server->xdisplay, server->atom_context, 
		     (XContext) atom, &type) != XCNOENT)
	return ((Server_atom_type) type);
    else {
	char *atomName;

	if ((int) atom <= XA_LAST_PREDEFINED)      /* Cache predefined atoms */
		return (save_atom(server, atom, SERVER_WM_UNKNOWN_TYPE));

	atomName = XGetAtomName(server->xdisplay, atom);

	if (!strcmp(atomName, "_OL_WIN_ATTR"))
		type = save_atom(server, atom, SERVER_WM_WIN_ATTR_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_ADD"))
		type = save_atom(server, atom, SERVER_WM_ADD_DECOR_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_DEL"))
		type = save_atom(server, atom, SERVER_WM_DELETE_DECOR_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_CLOSE"))
		type = save_atom(server, atom, SERVER_WM_DECOR_CLOSE_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_FOOTER"))
		type = save_atom(server, atom, SERVER_WM_DECOR_FOOTER_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_RESIZE"))
		type = save_atom(server, atom, SERVER_WM_DECOR_RESIZE_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_HEADER"))
		type = save_atom(server, atom, SERVER_WM_DECOR_HEADER_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_OK"))
		type = save_atom(server, atom, SERVER_WM_DECOR_OK_TYPE);
	else if (!strcmp(atomName, "_OL_DECOR_PIN"))
		type = save_atom(server, atom, SERVER_WM_DECOR_PIN_TYPE);
	else if (!strcmp(atomName, "_OL_SCALE_SMALL"))
		type = save_atom(server, atom, SERVER_WM_SCALE_SMALL_TYPE);
	else if (!strcmp(atomName, "_OL_SCALE_MEDIUM"))
		type = save_atom(server, atom, SERVER_WM_SCALE_MEDIUM_TYPE);
	else if (!strcmp(atomName, "_OL_SCALE_LARGE"))
		type = save_atom(server, atom, SERVER_WM_SCALE_LARGE_TYPE);
	else if (!strcmp(atomName, "_OL_SCALE_XLARGE"))
		type = save_atom(server, atom, SERVER_WM_SCALE_XLARGE_TYPE);
	else if (!strcmp(atomName, "_OL_PIN_STATE"))
		type = save_atom(server, atom, SERVER_WM_PIN_STATE_TYPE);
	else if (!strcmp(atomName, "_OL_WIN_BUSY"))
		type = save_atom(server, atom, SERVER_WM_WIN_BUSY_TYPE);
	else if (!strcmp(atomName, "_OL_WINMSG_STATE"))
		type = save_atom(server, atom, SERVER_WM_WINMSG_STATE_TYPE);
	else if (!strcmp(atomName, "_OL_WINMSG_ERROR"))
		type = save_atom(server, atom, SERVER_WM_WINMSG_ERROR_TYPE);
	else if (!strcmp(atomName, "_OL_WT_BASE"))
		type = save_atom(server, atom, SERVER_WM_WT_BASE_TYPE);
	else if (!strcmp(atomName, "_OL_WT_CMD"))
		type = save_atom(server, atom, SERVER_WM_WT_CMD_TYPE);
	else if (!strcmp(atomName, "_OL_WT_PROP"))
		type = save_atom(server, atom, SERVER_WM_WT_PROP_TYPE);
	else if (!strcmp(atomName, "_OL_WT_HELP"))
		type = save_atom(server, atom, SERVER_WM_WT_HELP_TYPE);
	else if (!strcmp(atomName, "_OL_WT_NOTICE"))
		type = save_atom(server, atom, SERVER_WM_WT_NOTICE_TYPE);
	else if (!strcmp(atomName, "_OL_WT_OTHER"))
		type = save_atom(server, atom, SERVER_WM_WT_OTHER_TYPE);
	else if (!strcmp(atomName, "_OL_MENU_FULL"))
		type = save_atom(server, atom, SERVER_WM_MENU_FULL_TYPE);
	else if (!strcmp(atomName, "_OL_MENU_LIMITED"))
		type = save_atom(server, atom, SERVER_WM_MENU_LIMITED_TYPE);
	else if (!strcmp(atomName, "_OL_NONE"))
		type = save_atom(server, atom, SERVER_WM_NONE_TYPE);
	else if (!strcmp(atomName, "_OL_PIN_IN"))
		type = save_atom(server, atom, SERVER_WM_PIN_IN_TYPE);
	else if (!strcmp(atomName, "_OL_PIN_OUT"))
		type = save_atom(server, atom, SERVER_WM_PIN_OUT_TYPE);
	else if (!strcmp(atomName, "WM_TAKE_FOCUS"))
		type = save_atom(server, atom, SERVER_WM_TAKE_FOCUS_TYPE);
	else if (!strcmp(atomName, "XV_DO_DRAG_MOVE"))
		type = save_atom(server, atom, SERVER_DO_DRAG_MOVE_TYPE);
	else if (!strcmp(atomName, "XV_DO_DRAG_COPY"))
		type = save_atom(server, atom, SERVER_DO_DRAG_COPY_TYPE);
	else if (!strcmp(atomName, "XV_DO_DRAG_LOAD"))
		type = save_atom(server, atom, SERVER_DO_DRAG_LOAD_TYPE);
	else if (!strcmp(atomName, "_OL_WIN_DISMISS"))
		type = save_atom(server, atom, SERVER_WM_DISMISS_TYPE);
	else if (!strcmp(atomName, "WM_SAVE_YOURSELF"))
		type = save_atom(server, atom, SERVER_WM_SAVE_YOURSELF_TYPE);
	else if (!strcmp(atomName, "WM_PROTOCOLS"))
		type = save_atom(server, atom, SERVER_WM_PROTOCOLS_TYPE);
	else if (!strcmp(atomName, "WM_DELETE_WINDOW"))
		type = save_atom(server, atom, SERVER_WM_DELETE_WINDOW_TYPE);
	else if (!strcmp(atomName, "WM_COMMAND"))
		type = save_atom(server, atom, SERVER_WM_COMMAND_TYPE);
	else if (!strcmp(atomName, "WM_CHANGE_STATE"))
		type = save_atom(server, atom, SERVER_WM_CHANGE_STATE_TYPE);
	else if (!strcmp(atomName, "_OL_BORDER_SIZES"))
		type = save_atom(server, atom, SERVER_WM_BORDER_SIZES_TYPE);
	else if (!strcmp(atomName, "_OL_DFLT_BIN"))
		type = save_atom(server, atom, SERVER_WM_DEFAULT_BUTTON_TYPE);
	else 
		type = save_atom(server, atom, SERVER_WM_UNKNOWN_TYPE);

	XFree(atomName);
	return ((Server_atom_type) type);
    }
}

static Server_atom_type
save_atom(server, atom, type)
Server_info	*server;
Atom		 atom;
Server_atom_type type;
{
	(void) XSaveContext(server->xdisplay, server->atom_context,
		     (XContext) atom, (caddr_t) type);
	server->atom[(int) type] = (Xv_opaque) atom;
	return (type); 
}




/*
 * BUG:  use_default_mapping should be set by comparing the default keycode
 * to keysym table.
 */
Xv_private void
server_journal_sync_event(server_public, type)
    Xv_Server       server_public;
    int             type;

{
    Server_info    *server = SERVER_PRIVATE(server_public);
    Atom            sync_atom = (Atom) xv_get(server_public, 
				     (Attr_attribute)SERVER_JOURNAL_SYNC_ATOM);
    XEvent          send_event;
    XClientMessageEvent *cme = (XClientMessageEvent *) & send_event;
    unsigned int    mask;
    Display        *dpy = (Display *) server->xdisplay;
    /*
     * Xv_Drawable_info       *info;
     */

    cme->type = ClientMessage;
    cme->serial = ClientMessage;/* should prob leave this alone */
    cme->send_event = 1;
    cme->display = dpy;
    /* get the xid of the root window -- not 100% correct */
    /*
     * DRAWABLE_INFO(xv_get(xv_get(server_public,SERVER_NTH_SCREEN,0),XV_ROOT),
     * info); cme->window = xv_xid(info); */
    cme->window = DefaultRootWindow((Display *) server->xdisplay),
    cme->message_type = sync_atom;
    cme->format = 32;
    cme->data.l[0] = type;
    mask = 0;
    XSync(dpy, 0);		/* make sure journal process has been
				 * scheduled and is waiting for the sync
				 * event */
    (void) XSendEvent(dpy, cme->window, 0, mask, cme);
    XSync(dpy, 0);		/* make sure journal event has occurred */
}

xv_string_to_rgb(buffer, red, green, blue)
    char            *buffer;
    unsigned char   *red;
    unsigned char   *green;
    unsigned char   *blue;

{
        int     hex_buffer;
        unsigned char   *conv_ptr;
        (void) sscanf(buffer, "#%6x", &hex_buffer);

        conv_ptr = (unsigned char *) &hex_buffer;
        *red = conv_ptr[1];
        *green = conv_ptr[2];
        *blue = conv_ptr[3];
}

static unsigned int
string_to_modmask(str)
char *str;
{
	if (strcmp(str, "Shift") == 0) 
		return ShiftMask;
	else if (strcmp(str, "Ctrl") == 0) 
		return ControlMask;
	else if (strcmp(str, "Meta") == 0) 
		return Mod1Mask;
	else  { /* Punt for now, just return Mod1Mask */
		/* What really needs to be done here is look up the 
		   modifier mapping from the server and add the new modifier
		   keys we are now interested in.   			     */
		xv_error(XV_NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			 XV_ERROR_STRING,
		  "Only support Shift, Ctrl and Meta as mouse button modifiers",
			 XV_ERROR_PKG_NAME, "Server",
			 NULL);
		return Mod1Mask;
	}
}

Pkg_private Xv_opaque
server_intern_atom(server, type, atom_name)
register Server_info *server;
Server_atom_type      type;
char 		     *atom_name;
{
	if (server->atom[(int) type] == NULL) {
	    server->atom[(int) type] = (Xv_opaque) XInternAtom(server->xdisplay,
							       atom_name, FALSE);
		if ((Atom) server->atom[(int) type] == BadValue ||
		    (Atom) server->atom[(int) type] == BadAlloc) {
			xv_error(XV_NULL, XV_ERROR_NON_RECOVERABLE,
				 XV_ERROR_INTERNAL, XV_ERROR_STRING,
				 "Can't create/get window manager atom",
				 XV_ERROR_PKG, ATTR_PKG_SERVER,
				 NULL);
		}
		(void) XSaveContext(server->xdisplay, server->atom_context,
			     (XContext) server->atom[(int) type],
			     (caddr_t) type); 
	}
	return((Xv_opaque) server->atom[(int) type]);
}
