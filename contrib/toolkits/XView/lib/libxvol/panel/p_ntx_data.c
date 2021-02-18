#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_ntx_data.c 20.3 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>

Pkg_private int panel_num_up_button_init();

Xv_pkg xv_panel_num_up_button_pkg = {
    "Numeric Button Up Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_button),
    &xv_panel_item_pkg,
    panel_num_up_button_init,
    NULL,
    NULL,
    NULL,
    NULL			/* no find proc */
};

Pkg_private int panel_num_down_button_init();

Xv_pkg xv_panel_num_down_button_pkg = {
    "Numeric Button Down Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_button),
    &xv_panel_item_pkg,
    panel_num_down_button_init,
    NULL,
    NULL,
    NULL,
    NULL			/* no find proc */
};

Pkg_private int         panel_num_text_init();
Pkg_private Xv_opaque   panel_num_text_set_avlist();
Pkg_private Xv_opaque   panel_num_text_get_attr();
Pkg_private int         panel_num_text_destroy();

Xv_pkg xv_panel_num_text_pkg = {
    "Numeric Text Item", ATTR_PKG_PANEL,
    sizeof(Xv_panel_num_text),
    &xv_panel_item_pkg,
    panel_num_text_init,
    panel_num_text_set_avlist,
    panel_num_text_get_attr,
    panel_num_text_destroy,
    NULL                        /* no find proc */
};
