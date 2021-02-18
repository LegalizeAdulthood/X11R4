#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)canvas.c 20.21 89/12/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/cnvs_impl.h>
#include <xview_private/win_keymap.h>
#include <xview_private/draw_impl.h>

Attr_attribute  canvas_context_key = (Attr_attribute) 0;
Attr_attribute  canvas_view_context_key = (Attr_attribute) 0;

/* ARGSUSED */
int
canvas_init(parent, canvas_public, avlist)
    Xv_Window       parent;
    Canvas          canvas_public;
    Attr_attribute  avlist[];
{
    Xv_canvas          *canvas_object = (Xv_canvas *) canvas_public;
    Canvas_info        *canvas;
    Xv_Drawable_info   *info;

    DRAWABLE_INFO_MACRO(canvas_public, info);

    if (canvas_context_key == (Attr_attribute) 0) {
	canvas_context_key = xv_unique_key();
    }
    if (canvas_view_context_key == (Attr_attribute) 0) {
	canvas_view_context_key = xv_unique_key();
    }
    canvas = xv_alloc(Canvas_info);

    /* link to object */
    canvas_object->private_data = (Xv_opaque) canvas;
    canvas->public_self = canvas_public;

    status_set(canvas, fixed_image);
    status_set(canvas, auto_expand);
    status_set(canvas, auto_shrink);
    status_set(canvas, retained);
    /*
     * If a dynamic colors are to be used in this canvas do not
     * generate repaints on colormap segment changes.
     */
    if (xv_dynamic_color(info) == FALSE) {
        status_set(canvas, cms_repaint);
    }

    return XV_OK;
}


int
canvas_destroy(canvas_public, stat)
    Canvas          canvas_public;
    Destroy_status  stat;
{
    Canvas_info    *canvas = CANVAS_PRIVATE(canvas_public);

    if (stat == DESTROY_CLEANUP) {
	free((char *) canvas);
    }
    return XV_OK;
}
