#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)window_cms.c 20.32 89/10/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview/base.h>
#include <xview/window.h>
#include <xview_private/windowimpl.h>
#include <xview_private/draw_impl.h>
#include <xview_private/xv_color.h>

Attr_attribute  xv_cms_name_key = NULL;
void            window_set_cms();

Xv_private
window_set_cms_name(win_public, new_name)
    Xv_Window       win_public;
    char           *new_name;
{
    Xv_Drawable_info *info;
    Xv_Colormap_Seg *cms, *screen_find_cms();
    char           *cur_name = (char *) NULL;

    DRAWABLE_INFO_MACRO(win_public, info);
    if (xv_depth(info) < 2) {
	return;
    }
    if (!xv_cms_name_key) {
	xv_cms_name_key = xv_unique_key();
    }
    cur_name = (char *) xv_get(win_public, XV_KEY_DATA, xv_cms_name_key);

    if (cms = screen_find_cms(xv_screen(info), new_name)) {
	if (cur_name) {
	    free(cur_name);
	    xv_set(win_public, XV_KEY_DATA, xv_cms_name_key, NULL, 0);
	}
	if (xv_cms(info) != cms) {
	    window_set_cms(win_public, cms, XV_CMS_BACKGROUND(cms), 
			   XV_CMS_FOREGROUND(cms));
	}
    } else {
	if (cur_name) {
	    if (strcmp(cur_name, new_name) == 0)
		return;
	    else
		xv_free(cur_name);
	}
	if (strlen(new_name) > CMS_NAMESIZE) {
	    cur_name = (char *) malloc(CMS_NAMESIZE + 1);
	    strncpy(cur_name, new_name, CMS_NAMESIZE);
	    cur_name[CMS_NAMESIZE] = '\0';
	} else {
	    cur_name = (char *) malloc(strlen(new_name) + 1);
	    strcpy(cur_name, new_name);
	}
	xv_set(win_public, XV_KEY_DATA, xv_cms_name_key, cur_name, 0);
    }
}

Xv_private
window_set_cms_data(win_public, cms_data)
    Xv_Window       win_public;
    Xv_cmsdata     *cms_data;
{
    Xv_Drawable_info *info;
    Xv_Colormap_Seg *cms, *screen_alloc_cms();
    char           *cur_name = (char *) NULL;

    DRAWABLE_INFO_MACRO(win_public, info);
    if (xv_depth(info) < 2) {
	return;
    }
    if ((cms_data->type == XV_STATIC_CMS) && (xv_dynamic_color(info))) {
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "Cannot set a static colormap segment on a window created\n\
with a dynamic visual",
		 XV_ERROR_PKG, ATTR_PKG_WIN,
		 0);
	return;
    } else if ((cms_data->type == XV_DYNAMIC_CMS) && (!xv_dynamic_color(info))) {
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "Cannot set a dynamic colormap segment on a window created\n\
with a static visual",
		 XV_ERROR_PKG, ATTR_PKG_WIN,
		 0);
	return;
    }
    if (!xv_cms_name_key) {
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
	"Attempting to set colormap segment data before naming the segment",
		 XV_ERROR_PKG, ATTR_PKG_WIN,
		 0);
	return;
    }
    cur_name = (char *) xv_get(win_public, XV_KEY_DATA, xv_cms_name_key);

    cms = screen_alloc_cms(xv_screen(info),
		    cur_name ? cur_name : xv_cms(info)->cms_name, cms_data);
    if (cms == NULL) {
	cms = (Xv_Colormap_Seg *) xv_get(xv_screen(info), SCREEN_DEFAULT_CMS);
    }
    window_set_cms(win_public, cms, XV_CMS_BACKGROUND(cms), 
		   XV_CMS_FOREGROUND(cms));

    if (cur_name) {
	xv_free(cur_name);
	xv_set(win_public, XV_KEY_DATA, xv_cms_name_key, NULL, 0);
    }
}

Pkg_private void
window_set_cms(win_public, cms, bg, fg)
    Xv_Window       win_public;
    Xv_Colormap_Seg *cms;
    short	    bg;
    short	    fg;
{
    Window_info    *win = WIN_PRIVATE(win_public);
    Xv_Drawable_info *info;
    XSetWindowAttributes attrs;
    XID             old_xid;
    short           old_fg, old_bg;
    unsigned long   val_mask = 0;

    DRAWABLE_INFO_MACRO(win_public, info);
    if (xv_depth(info) < 2) {
	return;
    }

    old_xid = xv_cmap(info);
    old_fg = xv_fg(info);
    old_bg = xv_bg(info);

    /* BUG - Ref counting is required before clobbering old cms. */
    info->cms = cms;

    if (old_xid != xv_cmap(info)) {
	attrs.colormap = xv_cmap(info);
	val_mask |= CWColormap;
    }
    if (old_fg != fg) {
	attrs.border_pixel = xv_fg(info) = fg;
	val_mask |= CWBorderPixel;
    }
    if (old_bg != bg) {
	xv_bg(info) = bg;
	/* Transparent windows have no background color */
	if (!win->transparent) {
	    attrs.background_pixel = bg;
	    val_mask |= CWBackPixel;
	}
    }
    if (val_mask) {
	XChangeWindowAttributes(xv_display(info), xv_xid(info),
				val_mask, &attrs);
	if (val_mask & CWBackPixel) {
	    XClearWindow(xv_display(info), xv_xid(info));
	}
    }

    /*
     * The colormap segment change is posted to the passed object. 
     * The assumption is that the object understands how to propagate the 
     * change in its environment. An example might be an object like canvas 
     * that has multiple windows. 
     */
    if (win->created) {
        xv_set(win_public, WIN_CMS_CHANGE, 0);
    }

    /*
     * Bug - This will not be necessary if the window manager is ICCM
     * compliant.
     */
    if (info->cms->cms_cmap !=
	(Xv_Colormap *) xv_get(xv_screen(info), SCREEN_DEFAULT_CMAP)) {
	xv_set(win_public,
	       WIN_CONSUME_EVENTS,
	       LOC_WINENTER,
	       LOC_WINEXIT,
	       0,
	       0);
    }
}
