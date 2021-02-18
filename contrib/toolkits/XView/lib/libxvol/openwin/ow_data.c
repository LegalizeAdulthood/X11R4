#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ow_data.c 1.11 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Global Defines:
 */

#include <xview_private/ow_impl.h>

Xv_pkg          xv_openwin_pkg = {
    "Open Window",		/* seal -> package name */
    (Attr_pkg) ATTR_PKG_OPENWIN,/* scrollbar attr */
    sizeof(Xv_openwin),		/* size of the scrollbar data struct */
    &xv_window_pkg,		/* pointer to parent */
    openwin_init,		/* init routine for scrollbar */
    openwin_set,		/* set routine */
    openwin_get,		/* get routine */
    openwin_destroy,		/* destroy routine */
    NULL			/* No find proc */
};
