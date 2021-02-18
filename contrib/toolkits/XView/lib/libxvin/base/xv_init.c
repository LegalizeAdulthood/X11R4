#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_init.c 20.30 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifdef _XV_DEBUG
#include <xview_private/xv_debug.h>
#else
#include <stdio.h>
#endif
#include <xview/pkg.h>
#include <xview/xview_xvin.h>
#include <X11/Xlib.h>

#ifdef _XV_DEBUG
Xv_private void xv_dump_fonts();

Xv_private_data int list_fonts;	/* = FALSE; -A-R */
#endif

/* FIXME: This should probably become integrated into the get/set paradigm */
Xv_error_action(*xv_error_proc) ();
extern void     xv_usage();
static int      xv_init_called;	/* = FALSE */

Xv_private void xv_init_x_pr();
Xv_private void xv_set_destroy_interposer();
Xv_private int  xv_handle_xio_errors();

Xv_private_data int disable_screen_saver;
Xv_private_data int lockstep_with_server;
Xv_private_data char *xv_app_name;

Xv_public_data Display *xv_default_display;
Xv_public_data Xv_Server xv_default_server;
Xv_public_data Xv_Screen xv_default_screen;


/*
 * Initialize XView.
 */
/*VARARGS*/
void
xv_init(va_alist)
va_dcl
{
    Attr_attribute     avlist[ATTR_STANDARD_SIZE];
    register Attr_avlist attrs;
    va_list         valist;
    void            (*help_proc) () = xv_usage;
    int             parse_result = 0;
    int             argc = 0;
    char          **argv;
    Attr_avlist     attrs_start;
    char           *server_name = (char *) NULL;

    /* can only be called once */
    if (xv_init_called)
	return;

#ifdef _XV_DEBUG
    /* Modify some system-wide defaults when not running production code. */
    disable_screen_saver = TRUE;
#endif

    xv_init_called = TRUE;

    (void) xv_set_destroy_interposer();

    xv_error_proc = (Xv_error_action(*) ()) 0;

    /* initialize the pixrect-to-x rop op table */
    xv_init_x_pr();
    
    /* silence the shut-down error messages, can turn on with option flag */
    (void) XSetIOErrorHandler(xv_handle_xio_errors);

#ifdef ATTR_MAKE
    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    attrs_start = (Attr_avlist) avlist;
#else
    attrs_start = (Attr_avlist) &va_alist;
    if (*attrs_start == (Attr_attribute) ATTR_LIST) {
	va_start(valist);
	(void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
	va_end(valist);
	attrs_start = (Attr_avlist) avlist;
    }
#endif				/* ATTR_MAKE */

    /* Preparse "-display <name>" for server creation */
    for (attrs = attrs_start; *attrs; attrs = attr_next(attrs)) {
	switch ((Xv_attr) attrs[0]) {
	  case XV_INIT_ARGS:
	    argc = (int) attrs[1];
	    argv = (char **) attrs[2];
	    break;

	  case XV_INIT_ARGC_PTR_ARGV:
	    argc = *(int *) attrs[1];
	    argv = (char **) attrs[2];
	    break;

	  default:
	    break;
	}
    }
    for (; argc--; argv++) {
	if (strcmp(*argv, "-display") == 0 ||
	    strcmp(*argv, "-Wr") == 0) {
	    server_name = *++argv;
	    break;
	}
    }

    /*
     * Check if any SERVER object has been created.  If not, then create one
     * to make sure that we read the defaults database from the correct
     * server before we parse cmd-line args. (xv_parse_cmdline stores the
     * parsed flags in the defaults database.)
     */
    if (!xv_has_been_initialized()) {
	if (server_name)
	    (void) xv_create(XV_NULL, SERVER, XV_NAME, server_name, 0);
	else
	    (void) xv_create(XV_NULL, SERVER, 0);
    }
    for (attrs = attrs_start; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case XV_USAGE_PROC:
	    help_proc = (void (*) ()) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case XV_INIT_ARGS:
	    argc = (int) attrs[1];
	    argv = (char **) attrs[2];

	    if (xv_app_name) {
		free(xv_app_name);
	    }
	    if (argv[0]) {
		xv_app_name = calloc(1, (unsigned) (strlen(argv[0]) + 1));
		(void) strcpy(xv_app_name, argv[0]);
	    }
	    parse_result = xv_parse_cmdline(xv_app_name, &argc, argv, FALSE);
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case XV_INIT_ARGC_PTR_ARGV:
	    argv = (char **) attrs[2];

	    if (xv_app_name) {
		(void) free(xv_app_name);
	    }
	    if (argv[0]) {
		xv_app_name = calloc(1, (unsigned) (strlen(argv[0]) + 1));
		(void) strcpy(xv_app_name, argv[0]);
	    }
	    parse_result = xv_parse_cmdline(xv_app_name, 
					    (int *) attrs[1], argv, TRUE);
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case XV_ERROR_PROC:{
		xv_error_proc = (Xv_error_action(*) ()) attrs[1];
		ATTR_CONSUME(attrs[0]);
		break;
	    }

	  default:
	    break;
	}
    }

    if (parse_result == -1) {
	/* Following routine often, but not always, calls exit(). */
	help_proc(xv_app_name);
    }
    /*
     * BUG: can't do this till after first create #ifdef _XV_DEBUG if
     * (list_fonts) { xv_dump_fonts(xv_default_display); } #endif
     */
}

/*ARGSUSED*/
Xv_private int
xv_handle_xio_errors(display)
    Display *display;
{
    /* do nothing, be quiet */
    exit(0);
}
