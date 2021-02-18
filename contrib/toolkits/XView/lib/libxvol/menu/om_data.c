#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)om_data.c 20.21 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * om_data.c: fix for shared libraries in SunOS4.0.  Code was isolated from
 * walkmenu_public.c
 */


#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>

#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <pixrect/pr_util.h>
#include <pixrect/memvar.h>

#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/pixwin.h>
#include <xview/win_input.h>
#include <xview/window.h>
#include <xview/cursor.h>
#include <xview/win_struct.h>

#include <xview_private/om_impl.h>
#include <xview/openmenu.h>

Pkg_private Xv_opaque menu_sets();
Pkg_private Xv_opaque menu_gets();
Pkg_private Xv_opaque menu_pkg_find();
Pkg_private int menu_create_item_internal();
Pkg_private Xv_opaque menu_item_sets();
Pkg_private Xv_opaque menu_item_gets();
Pkg_private int menu_item_destroy_internal();

short           menu_gray75_data[16] = {	/* 75% gray */
    0xFFFF, 0x5555, 0xFFFF, 0x5555, 0xFFFF, 0x5555, 0xFFFF, 0x5555,
    0xFFFF, 0x5555, 0xFFFF, 0x5555, 0xFFFF, 0x5555, 0xFFFF, 0x5555
};

mpr_static(menu_gray75_pr, 16, 16, 1, menu_gray75_data);

short           menu_gray50_data[16] = {	/* 50% gray */
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555,
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555
};

mpr_static(menu_gray50_pr, 16, 16, 1, menu_gray50_data);

Xv_pkg          xv_command_menu_pkg = {
    "Command Menu",		/* seal -> package name */
    ATTR_PKG_MENU,		/* menu attr */
    sizeof(Xv_menu),		/* size of the menu public data structure */
    &xv_generic_pkg,		/* pointer to parent */
    menu_create_internal,	/* init routine */
    menu_sets,
    menu_gets,
    menu_destroy_internal,
    NULL			/* no find proc */
};

Xv_pkg          xv_choice_menu_pkg = {
    "Choice Menu",		/* seal -> package name */
    ATTR_PKG_MENU,		/* menu attr */
    sizeof(Xv_menu),		/* size of the menu public data structure */
    &xv_generic_pkg,		/* pointer to parent */
    menu_create_internal,	/* init routine */
    menu_sets,
    menu_gets,
    menu_destroy_internal,
    NULL			/* no find proc */
};

Xv_pkg          xv_toggle_menu_pkg = {
    "Toggle Menu",		/* seal -> package name */
    ATTR_PKG_MENU,		/* menu attr */
    sizeof(Xv_menu),		/* size of the menu public data structure */
    &xv_generic_pkg,		/* pointer to parent */
    menu_create_internal,	/* init routine */
    menu_sets,
    menu_gets,
    menu_destroy_internal,
    NULL			/* no find proc */
};

Xv_pkg          xv_menu_item_pkg = {
    "Menu_item",
    ATTR_PKG_MENU,		/* menu item shares menu attrs */
    sizeof(Xv_menu_item),	/* size of the item public data structure */
    &xv_generic_pkg,
    menu_create_item_internal,
    menu_item_sets,
    menu_item_gets,
    menu_item_destroy_internal,
    menu_pkg_find
};
