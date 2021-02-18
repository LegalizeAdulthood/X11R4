/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************************\
*   				       *
* 	BULL WINDOW MANAGER for X11    *
* 				       *
* 		error handling.	       *
* 				       *
\**************************************/

/*  include  */
#include 	<stdio.h>
#include	<errno.h>
#include 	"EXTERN.h"
#include 	"gwm.h"
#include	"wl_atom.h"
#include	"yacc.h"

/*  external  */

extern void     UserMessage();

/*  global  */

/* request error codes */

#define MAX_X_EVENT 64

#ifdef SECURE
char *eventtype[MAX_X_EVENT] = {
	"zero",
	"one",
	"KeyPress",
	"KeyRelease",
	"ButtonPress",
	"ButtonRelease",
	"MotionNotify",
	"EnterNotify",
	"LeaveNotify",
	"FocusIn",
	"FocusOut",
	"KeymapNotify",
	"Expose",
	"GraphicsExpose",
	"NoExpose",
	"VisibilityNotify",
	"CreateNotify",
	"DestroyNotify",
	"UnmapNotify",
	"MapNotify",
	"MapRequest",
	"ReparentNotify",
	"ConfigureNotify",
	"ConfigureRequest",
	"GravityNotify",
	"ResizeRequest",
	"CirculateNotify",
	"CirculateRequest",
	"PropertyNotify",
	"SelectionClear",
	"SelectionRequest",
	"SelectionNotify",
	"ColormapNotify",
	"ClientMessage",
	"MappingNotify",
	"LASTEvent",
	"GWMUserEvent"
};

#endif /* SECURE */

#ifdef DO_BUS_ERROR
#define DoABusError() *((int *)0) = 1
#else /* DO_BUS_ERROR */
#define DoABusError()
#endif /* DO_BUS_ERROR */

/*  routines  */

/*
 * Called by server on error
 */

XError(display, error)
Display        *display;
XErrorEvent    *error;
{
    char            msg[MAX_TEMP_STRING_SIZE], buffer[MAX_TEMP_STRING_SIZE];
    char            number[32];

    if (GWM_window_being_decorated
	&& error -> error_code == BadWindow
	&& error -> resourceid == GWM_window_being_decorated -> client) {
	ClientWindowClose(GWM_window_being_decorated);
	GWM_window_being_decorated = 0;
    }
    if (wool_do_print_errors) {
	if (GWM_Synchronize) {		/* only meaningful when sync */
	    if (wool_is_reading_file) {
		wool_printf("\"%s\"", wool_is_reading_file);
		wool_printf(": line %d, ", yylineno);
	    }
	    wool_stack_dump(0);
	}
	wool_puts("Gwm X error: ");
	sprintf(number, "%d", error -> request_code);
	XGetErrorDatabaseText(dpy, "XRequest", number, "", buffer,
			      MAX_TEMP_STRING_SIZE);
	sprintf(msg, "%s(0x%x): ", buffer, error -> resourceid);
	wool_puts(msg);
	XGetErrorText(display, error -> error_code, msg,
		      MAX_TEMP_STRING_SIZE);
	wool_puts(msg);
	if (error -> minor_code)
	    sprintf(msg, " (minor = %d)", error -> minor_code);
	wool_newline();
	yyoutflush();
    }
    ErrorStatus = error -> error_code;
    wool_return_from_wool_error();
    DoABusError();
}

NoXError(display, error)
Display        *display;
XErrorEvent    *error;
{
    ErrorStatus = error -> error_code;
}

XFatalHandler(display)
Display        *display;
{
    fprintf(stderr, "\007GWM: X Fatal Error\n");
    if (errno == EPIPE) {
	(void) fprintf(stderr,
		       "Connection # %d to server broken.\n", display -> fd);
    }
    perror("GWM");
    GWM_end(-1);
}

Error(s1, s2)
char           *s1, *s2;
{
    UserMessage(s1, s2);
}

void
UserMessage(s1, s2)
char           *s1, *s2;
{
    wool_puts(s1);
    wool_puts(s2);
    wool_newline();
}

/* what to do when a wool-error occurs?
 */

wool_error_handler()
{
    if (GWM_ServerGrabbed)
	remove_grab(0);
    if (GWM_is_restarting || GWM_is_ending)
	GWM_BusErrorSignalHandler(0);	/* skip wool when error in ending */
    if (GWM_Window_being_opened)
    	XMapWindow(dpy, GWM_Window_being_opened -> hook);
}
