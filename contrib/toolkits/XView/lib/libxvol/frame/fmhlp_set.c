#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fmhlp_set.c 1.18 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/Xlib.h>
#include <xview_private/fm_impl.h>
#include <xview_private/frame_help.h>
#include <xview_private/draw_impl.h>
#include <xview_private/wmgr_decor.h>
#include <xview/server.h>


Pkg_private     Xv_opaque
frame_help_set_avlist(frame_public, avlist)
    Frame           frame_public;
    Frame_attribute avlist[];
{
    Frame_attribute *attrs;
    Xv_Drawable_info *info;
    Xv_opaque       server_public;
    Frame_help_info *frame = FRAME_HELP_PRIVATE(frame_public);
    int             result = XV_OK;
    int             add_decor, delete_decor;
    Atom            add_decor_list[WM_MAX_DECOR], delete_decor_list[WM_MAX_DECOR];

    DRAWABLE_INFO_MACRO(frame_public, info);
    server_public = xv_server(info);
    add_decor = delete_decor = 0;

    for (attrs = avlist; *attrs; attrs = frame_attr_next(attrs)) {
	switch (attrs[0]) {

	  case FRAME_SHOW_LABEL:	/* same as FRAME_SHOW_HEADER */
	    attrs[0] = (Frame_attribute) ATTR_NOP(attrs[0]);
	    if (status_get(frame, show_label) == (int) attrs[1])
		break;

	    status_set(frame, show_label, (int) attrs[1]);

	    if ((int) attrs[1])
		add_decor_list[add_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_HEADER);
	    else
		delete_decor_list[delete_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_HEADER);
	    break;

	  case XV_LABEL:
	    {
		extern char    *xv_app_name;
		Frame_class_info *frame_class = FRAME_CLASS_FROM_HELP(frame);

		*attrs = (Frame_attribute) ATTR_NOP(*attrs);
		if (frame_class->label) {
		    free(frame_class->label);
		}
		if ((char *) attrs[1]) {
		    frame_class->label = (char *) calloc(1,
					     strlen((char *) attrs[1]) + 1);
		    strcpy(frame_class->label, (char *) attrs[1]);
		} else {
		    if (xv_app_name) {
			frame_class->label = (char *) calloc(1,
						   strlen(xv_app_name) + 6);
			sprintf(frame_class->label, "%s Help", xv_app_name);
		    } else {
			frame_class->label = NULL;
		    }
		}
		(void) frame_display_label(frame_class);
		break;
	    }

	  case XV_END_CREATE:
	    (void) wmgr_set_win_attr(frame_public, &(frame->win_attr));
	    break;

	  default:
	    break;

	}
    }

    if (add_decor)
	wmgr_add_decor(frame_public, add_decor_list, add_decor);

    if (delete_decor)
	wmgr_delete_decor(frame_public, delete_decor_list, delete_decor);

    return (Xv_opaque) result;
}
