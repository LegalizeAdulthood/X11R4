#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb.c 1.23 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb.c
 * 
 * 
 * 
/* Include files:
 */

#include <xview_private/sb_impl.h>
#include <xview/openwin.h>
#include <xview/panel.h>
#include <xview/xv_error.h>

/*
 * Declaration of Functions Defined in This File (in order):
 */

Pkg_private int scrollbar_create_internal();




/******************************************************************/


Pkg_private int
scrollbar_create_internal(parent, sb_public, avlist)
    Xv_opaque       parent;
    Xv_opaque       sb_public;
    Xv_opaque      *avlist;
{
    Xv_scrollbar   *scrollbar = (Xv_scrollbar *) sb_public;
    Xv_scrollbar_info *sb;

    /*
     * Cannot attach a scrollbar to a control area (i.e., a panel without a
     * border).
     */
    if (parent &&		/* in case client used scrollbar_create */
	xv_get(parent, XV_IS_SUBTYPE_OF, PANEL) &&
	xv_get(parent, OPENWIN_SHOW_BORDERS) == FALSE) {
	xv_error(parent, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
	 XV_ERROR_STRING, "Cannot attach scrollbar to non-scrollable panel",
		 XV_ERROR_PKG_NAME, "Scrollbar",
		 0);
	return (XV_ERROR);
    }
    sb = xv_alloc(Xv_scrollbar_info);

    sb->public_self = sb_public;
    scrollbar->private_data = (Xv_opaque) sb;
    sb->managee = parent;
    sb->direction = SCROLLBAR_VERTICAL;
#ifdef SUNVIEW1
    sb->cable_image = NULL;
#endif				/* SUNVIEW1 */
    (void) scrollbar_init_common(sb);

    return XV_OK;
}
