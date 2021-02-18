#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)svr_x.c 20.29 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview/pkg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#ifdef _XV_DEBUG
#include <xview_private/xv_debug.h>
#endif
#include <xview/win_event.h>
#include <X11/Xlib.h>
#include <xview/sel_svc.h>
#include <xview/server.h>
#include <xview_private/svr_impl.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

extern Display *XOpenDisplay();

/*
 * The following table describes the default key mappings for special 
 * XView keys. The first default key mapping is attempted. If this 
 * fails, then the machine we're on probably doesn't
 * have sufficient function keys, so we try the second mapping, and
 * so on. Right now, the rest of XView only supports the default mapping
 * so we set NUM_KEYSYM_SETS to 1 and only supply one set of keys to use.
 * In the future, this will go away and we'll provide a more elegant
 * and flexible way for the user to map function keys to XView functions.
 */

#define NUM_KEYSYM_SETS	1
static KeySym	default_fkey_keysyms[NUM_KEYSYM_SETS][SELN_FN_NUM] = {
		{			/* default keysyms */
			XK_F13,
			XK_F16,
			XK_F18,
			XK_F19,
			XK_F20
		}
};

#define MAX_RETRIES	10	/* max number of mapping retries */

/* Production code debugging aid */
Xv_private_data int disable_screen_saver;	/* = 0; -A-R */
Xv_private_data int lockstep_with_server;	/* = 0; -A-R */

static int
my_sync(display)
    Display        *display;
{
    XSync(display, 0);
}

Pkg_private     Xv_opaque
server_init_x(server_name)
    char           *server_name;
{
    register Display *display;

    /* Hook up to the specified (X) window server */
    display = XOpenDisplay(server_name);
    if (display == NULL) {
	xv_error(XV_NULL, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, "Cannot open display on window server",
		 XV_ERROR_STRING, server_name,
		 XV_ERROR_PKG, ATTR_PKG_SERVER,
		 NULL);
	goto Return;
    }
    /*
     * Mark underlying file descriptor for connection as close-on-exec so
     * that we don't have to chase it down in terminal emulators, etc.
     */
    (void) fcntl(ConnectionNumber(display), F_SETFD, 1);

    if (lockstep_with_server && !XSynchronize(display, TRUE))
	(void) XSetAfterFunction(display, my_sync);
    if (disable_screen_saver)
	XSetScreenSaver(display, 0, 0, DefaultBlanking, DefaultExposures);

Return:
    return ((Xv_opaque) display);
}


/*
 * keycode_in_map(map, keycode)
 *
 * returns the associated modifier index if the specified keycode is in the 
 * given modifier map. Otherwise, returns -1.
 */

static int
keycode_in_map(map, keycode)
	XModifierKeymap *map;
	KeyCode keycode;
{
	register int i, max;

	max = 8 * map->max_keypermod;
	for (i = 0; i < max; i++) {
		if (map->modifiermap[i] == keycode) {
			return (i / map->max_keypermod);
		}
	}
	return -1;
}

/*
 * server_init_modifier(display)
 *
 * 1) Designates the meta keys as Mod1 modifier keys.
 * 2) Inserts all the keys in the array default_fkey_keysyms[] into
 * 	the server's modifier map (all under the same modifier; any
 *	of the modifiers Mod2-Mod5 may be used). This function then
 *	sets SELN_FNMASK to be the appropriate mask for whatever modifier
 *	the keys were designated as.
 */
Pkg_private void
server_init_modifier(display)
	Display	*display;
{
	XModifierKeymap *map;
	int             base, row, offset, i, dest_row;
	int		keysym_set, result, retry_count;

	for (keysym_set = 0; keysym_set < NUM_KEYSYM_SETS; keysym_set++) {
		if (!(map = XGetModifierMapping(display))) {
			return;
		}

		/* Insert the meta keys as "Mod1" modifiers  */
		map = XInsertModifiermapEntry(map,
				XKeysymToKeycode(display, (KeySym) XK_Meta_L), 
				Mod1MapIndex);
		map = XInsertModifiermapEntry(map,
				XKeysymToKeycode(display, (KeySym) XK_Meta_R), 
				Mod1MapIndex);

		/*
		 * Find the first unused row in the modifier map.
		 * An unused row will have all zeros in it.
		 */
		dest_row = 0;
		for (row = Mod2MapIndex; row < Mod5MapIndex && !dest_row; 
								row++) {
			base = row * map->max_keypermod;
			for (offset = 0; (offset < map->max_keypermod)  &&
				(map->modifiermap[base + offset] == 0); 
				offset++);
			if (offset == map->max_keypermod) {
				dest_row = row;
			}
		}

		/* 
		 * Convert the appropriate keysym array to keycodes, and
		 * attempt to insert into the map that we have.
		 * Also ensure that none of the keycodes we're going to add are
		 * already in the map. X servers will not allow the same keycode
		 * to appear in the map twice. If we do find a keycode
		 * already in the map, then assume that we're already set up
		 * and exit, setting SELN_FNMASK properly.
		 */
		for (i = 0; i < SELN_FN_NUM; i++) {
			KeyCode keycode;
			int 	found_row;
			keycode = XKeysymToKeycode(display,
					   default_fkey_keysyms[keysym_set][i]);
			if ((found_row = keycode_in_map(map, keycode)) >= 0) {
				SELN_FNMASK = 1 << found_row;
				XFreeModifiermap(map);
				return;
			}
			else if (dest_row) {
				map = XInsertModifiermapEntry(map, keycode,
					dest_row);
			}
		}

		if (!dest_row) {	/* no free rows */
			SELN_FNMASK = 1 << Mod5MapIndex;
			XFreeModifiermap(map);
			return;
		}

		/*
		 * Attempt to install the modifier mapping.
		 * If successful, exit this function. If not, try another 
		 * set of keysyms.
		 */
		for (retry_count = 0;
			((result = XSetModifierMapping(display, map)) 
				== MappingBusy && retry_count < MAX_RETRIES);
			retry_count++) {
				sleep(1);/* if busy, wait 1 sec and retry */
		}
  	  	if (result == Success) {
    			XFreeModifiermap(map);
    			SELN_FNMASK = 1 << dest_row;
			return;
		}
	}

	/* all our attempts failed */
	xv_error(XV_NULL, XV_ERROR_RECOVERABLE, 
		 XV_ERROR_INTERNAL, XV_ERROR_STRING,
		 "Error from call to XSetModifierMapping", XV_ERROR_PKG, 
		 ATTR_PKG_SERVER, NULL);

    	XFreeModifiermap(map);
    	SELN_FNMASK = 1 << dest_row;

}


Xv_private void
server_set_seln_function_pending(server_public, flag)
    Xv_Server       server_public;
    int             flag;
{
    Server_info    *server = SERVER_PRIVATE(server_public);
    server->seln_function_pending = flag ? TRUE : FALSE;
}

Xv_private int
server_get_seln_function_pending(server_public)
    Xv_Server       server_public;
{
    return (SERVER_PRIVATE(server_public)->seln_function_pending);
}
