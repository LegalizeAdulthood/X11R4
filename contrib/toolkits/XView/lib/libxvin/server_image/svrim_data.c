#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)svrim_data.c 1.11 89/10/17";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 * 
 */

#include <sys/types.h>
#include <stdio.h>
#include <pixrect/pixrect.h>
#include <pixrect/memvar.h>
#include <xview_private/svrim_impl.h>

Xv_pkg          xv_server_image_pkg = {
    "Server Image",			/* seal -> package name */
    ATTR_PKG_IMAGE,		/* image attr */
    sizeof(Xv_server_image),	/* size of the image data struct */
    &xv_drawable_pkg,		/* pointer to parent */
    server_image_create_internal,	/* init routine for icon */
    server_image_set_internal,	/* set routine */
    server_image_get_internal,	/* get routine */
    server_image_destroy_internal,	/* destroy routine */
    NULL			/* no find proc */
};
