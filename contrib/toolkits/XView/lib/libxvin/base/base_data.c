#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)base_data.c 1.11 89/08/17";
#endif
#endif
#include <xview_private/draw_impl.h>
#include <xview/generic.h>
/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */
extern int      generic_destroy();
extern Xv_opaque generic_get(), generic_set_avlist();

Xv_pkg          xv_generic_pkg = {
    "Generic",
    ATTR_PKG_GENERIC,
    sizeof(Xv_generic_struct),
    NULL,			/* No parent package */
    NULL,			/* No init procedure */
    generic_set_avlist,
    generic_get,
    generic_destroy,
    NULL			/* No find procedure */
};

Xv_pkg          xv_drawable_pkg = {
    "Drawable",
    ATTR_PKG_DRAWABLE,
    sizeof(Xv_drawable_struct),
    &xv_generic_pkg,
    drawable_init,
    NULL,			/* No set allowed */
    drawable_get_attr,
    drawable_destroy,
    NULL,			/* No find procedure */
};
