#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_init_x.c 20.15 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/xv_debug.h>
#include <stdio.h>		/* stderr & BUFSIZ */
#include <sys/types.h>
#include <xview/pkg.h>
#include <xview/xv_error.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <strings.h>

#ifdef _XV_DEBUG
Xv_private int  xv_ask_for_debugger;
#endif

static int      error_aborts;
static int      error_to_ignore;

Xv_private void
xv_set_default_font(display, screen, font)
    register Display   *display;
    int             	screen;
    Font             	font;
{
    GC              gc = DefaultGC(display, screen);

    XSetFont(display, gc, font);
}

#ifdef _XV_DEBUG
Xv_private void
xv_dump_fonts(display)
    Display        *display;
{
    char          **temp;
    int             font_count, i;

    temp = (char **) XListFonts(display, "*", 100, &font_count);
    (void) fprintf(stderr,
		   "There are %d fonts available, named:\n", font_count);
    for (i = 0; i < font_count; i++) {
	(void) fprintf(stderr, "\t%s\n", temp[i]);
    }
    XFreeFontNames(temp);
}

#endif

/*
 * Modified version of X11R1 FCS Xlib routine `XlibInt`_XDefaultError.
 */
Xv_private int
xv_print_error(dpy, event)
    register Display	*dpy;
    XErrorEvent	        *event;
{
    char            buffer[BUFSIZ];
    char            mesg[BUFSIZ];
    char            number[32];
    char           *mtype = "XlibMessage";
    char            dummy[BUFSIZ], more[BUFSIZ];

    XGetErrorText(dpy, (int) event->error_code, buffer, BUFSIZ);
    XGetErrorDatabaseText(dpy, mtype, "XError", "X Error", mesg, BUFSIZ);
    (void) sprintf(dummy, "%s: %s", mesg, buffer);

    xv_error(XV_NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
	     XV_ERROR_STRING, dummy, 0);

    XGetErrorDatabaseText(dpy, mtype, "MajorCode", "Req Maj code %d",
			  mesg, BUFSIZ);
    (void) sprintf(more, mesg, event->request_code);
    (void) strcpy(dummy, more);

    (void) sprintf(number, "%d", event->request_code);
    XGetErrorDatabaseText(dpy, "XRequest", number, "", buffer, BUFSIZ);
    (void) sprintf(more, "; %s", buffer);
    (void) strcat(dummy, more);

    XGetErrorDatabaseText(dpy, mtype, "MinorCode", "Req Min code %d",
			  mesg, BUFSIZ);
    (void) sprintf(more, mesg, event->minor_code);
    (void) strcat(dummy, more);

    XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID 0x%x",
			  mesg, BUFSIZ);
    (void) sprintf(more, mesg, event->resourceid);
    (void) strcat(dummy, more);

    XGetErrorDatabaseText(dpy, mtype, "ErrorSerial", "Err Ser #%d",
			  mesg, BUFSIZ);
    (void) sprintf(more, mesg, event->serial);
    (void) strcat(dummy, more);

    XGetErrorDatabaseText(dpy, mtype, "CurrentSerial", "Curr Ser #%d",
			  mesg, BUFSIZ);
    (void) sprintf(more, mesg, dpy->request);
    (void) strcat(dummy, more);

    xv_error(XV_NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
	     XV_ERROR_STRING, dummy, 0);

    if (event->error_code == BadImplementation)
	return 0;
    return 1;
}

Xv_private int
xv_x_error_handler(dpy, event)
    Display        *dpy;
    XErrorEvent    *event;
{
    if (xv_print_error(dpy, event) == 0)
	return 0;
    if (error_aborts) {
#ifdef _XV_DEBUG
	if (error_aborts != 1 || xv_ask_for_debugger) {
	    xv_ask_for_debugger = TRUE;
	    xv_take_breakpoint();
	    return 0;
	} else
#endif
	    exit(1);
    }
    return (0);
}

static int
xv_x_error_handler2(dpy, event)
    Display        *dpy;
    XErrorEvent    *event;
{
    if (event->error_code == error_to_ignore)
	return 0;
    return xv_x_error_handler(dpy, event);
}

Xv_private int
xv_x_set_focus_error_handler(dpy, event)
    Display        *dpy;
    XErrorEvent    *event;
{
    if ((event->error_code == BadMatch) &&
	(event->request_code == X_SetInputFocus))
	return 0;
    return xv_x_error_handler(dpy, event);
}





Xv_private int
xv_set_error_handler(func, abort_on_error)
    int             (*func) ();
    int             abort_on_error;
{
    if ((int) func == 1) {
	error_aborts = abort_on_error;
	(void) XSetErrorHandler(xv_x_error_handler);
    } else if ((int) func == 2) {
	error_aborts = ((abort_on_error >> 16) != 0);
	error_to_ignore = abort_on_error & 0xFFFF;
	(void) XSetErrorHandler(xv_x_error_handler2);
    } else {
	(void) XSetErrorHandler(func);
    }
}
