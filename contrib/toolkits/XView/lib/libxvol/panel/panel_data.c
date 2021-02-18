#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)panel_data.c 1.11 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */
#include <xview_private/panel_impl.h>
Pkg_private int panel_init();
Pkg_private int panel_destroy();

Xv_pkg          xv_panel_pkg = {
    "Panel", ATTR_PKG_PANEL,
    sizeof(Xv_panel),
    &xv_canvas_pkg,
    panel_init,
    panel_set_avlist,
    panel_get_attr,
    panel_destroy,
    NULL			/* no find proc */
};
Pkg_private int panel_view_init();

Xv_pkg          xv_panel_view_pkg = {
    "Panel", ATTR_PKG_PANEL,
    sizeof(Xv_panel),
    &xv_canvas_view_pkg,
    panel_view_init,
    NULL,
    NULL,
    NULL,
    NULL			/* no find proc */
};

Pkg_private int slider_init();
Pkg_private Xv_opaque slider_set_avlist();
Pkg_private Xv_opaque slider_get_attr();
Pkg_private int slider_destroy();

Xv_pkg          xv_panel_slider_pkg = {
    "Slider Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_slider),
    &xv_panel_item_pkg,
    slider_init,
    slider_set_avlist,
    slider_get_attr,
    slider_destroy,
    NULL			/* no find proc */
};
