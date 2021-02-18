#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)item_get.c 20.28 89/08/18 Copyr 1984 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>

Pkg_private     Xv_opaque
item_get_attr(item_public, status, which_attr, valist)
    Panel_item      item_public;
    int            *status;
    register Panel_attr which_attr;
    va_list         valist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);

    switch (which_attr) {
      case PANEL_VALUE_X:
	return (Xv_opaque) ip->value_rect.r_left;

      case PANEL_VALUE_Y:
	return (Xv_opaque) ip->value_rect.r_top;

      case PANEL_LABEL_STRING:
	if (is_string(&ip->label))
	    return (Xv_opaque) image_string(&ip->label);
	else
	    return (Xv_opaque) NULL;

      case PANEL_LABEL_FONT:
	if (is_string(&ip->label))
	    return (Xv_opaque) image_font(&ip->label);
	else
	    return (Xv_opaque) NULL;

      case PANEL_LABEL_BOLD:
	if (is_string(&ip->label))
	    return (Xv_opaque) image_bold(&ip->label);
	else
	    return (Xv_opaque) NULL;

      case PANEL_LABEL_BOXED:
	return (Xv_opaque) image_boxed(&ip->label);

      case PANEL_LABEL_INVERTED:
	return (Xv_opaque) image_inverted(&ip->label);

      case PANEL_LABEL_IMAGE:
	if (is_pixrect(&ip->label))
	    return (Xv_opaque) image_pixrect(&ip->label);
	else
	    return (Xv_opaque) NULL;

      case PANEL_LABEL_WIDTH:
	return (Xv_opaque) (ip->label_width ? ip->label_width :
			    ip->label_rect.r_width);

      case PANEL_LABEL_X:
	return (Xv_opaque) ip->label_rect.r_left;

      case PANEL_LABEL_Y:
	return (Xv_opaque) ip->label_rect.r_top;

	/* these attrs must still be supported in addition to XV_X */
	/* and XV_Y because they can be used on the panel. When used */
	/* on the panel PANEL_ITEM_X != XV_X on the panel */
      case XV_X:
      case PANEL_ITEM_X:
	return (Xv_opaque) ip->rect.r_left;

      case XV_Y:
      case PANEL_ITEM_Y:
	return (Xv_opaque) ip->rect.r_top;

      case XV_WIDTH:
	return (Xv_opaque) ip->rect.r_width;

      case XV_HEIGHT:
	return (Xv_opaque) ip->rect.r_height;

      case PANEL_ITEM_RECT:
      case XV_RECT:
	return (Xv_opaque) & ip->rect;

      case XV_SHOW:
	return (Xv_opaque) ! hidden(ip);

      case PANEL_NOTIFY_PROC:
	return (Xv_opaque) ip->notify;

      case PANEL_NOTIFY_STATUS:
	return (Xv_opaque) ip->notify_status;

      case PANEL_EVENT_PROC:
	return (Xv_opaque) ip->ops->handle_event;

      case PANEL_NEXT_ITEM:
	return (Xv_opaque) (ip->next ? ITEM_PUBLIC(ip->next) : 0);

      case PANEL_LAYOUT:
	return (Xv_opaque) ip->layout;

#ifdef SUNVIEW1
      case PANEL_MENU_TITLE_STRING:
	return is_string(&ip->menu_title) ?
	    (Xv_opaque) image_string(&ip->menu_title) : NULL;

      case PANEL_MENU_TITLE_IMAGE:
	return is_pixrect(&ip->menu_title) ?
	    (Xv_opaque) image_pixrect(&ip->menu_title) : NULL;

      case PANEL_MENU_TITLE_FONT:
	return is_string(&ip->menu_title) ?
	    (Xv_opaque) image_font(&ip->menu_title) : NULL;

      case PANEL_TYPE_IMAGE:
	return (Xv_opaque) ip->menu_type_pr;

      case PANEL_MENU_CHOICE_STRINGS:
      case PANEL_MENU_CHOICE_IMAGES:
      case PANEL_MENU_CHOICE_FONTS:
      case PANEL_MENU_CHOICE_VALUES:
	return NULL;
#endif

      case PANEL_SHOW_MENU:
	return (Xv_opaque) TRUE;/* Obsolete attribute; retained for
				 * compatibility. */

      case PANEL_SHOW_MENU_MARK:
	return (Xv_opaque) show_menu_mark(ip);

      case PANEL_MENU_MARK_IMAGE:
	return (Xv_opaque) ip->menu_mark_on;

      case PANEL_MENU_NOMARK_IMAGE:
	return (Xv_opaque) ip->menu_mark_off;

      case PANEL_ACCEPT_KEYSTROKE:
	return (Xv_opaque) wants_key(ip);

      case PANEL_CLIENT_DATA:
	return ip->client_data;

      case PANEL_ITEM_COLOR:
	return (Xv_opaque) ip->color_index;

      case XV_OWNER:
	return ((Xv_opaque) (ip->panel != NULL) ? PANEL_PUBLIC(ip->panel) : NULL);

      case PANEL_INACTIVE:
	return (Xv_opaque) inactive(ip);

      case PANEL_ITEM_MENU:
	return ip->menu;

      case PANEL_ITEM_CLASS:
	return (Xv_opaque) (ip->item_type);
	break;

      case PANEL_MENU_ITEM:
	return (Xv_opaque) is_menu_item(ip);

      default:
	if (xv_check_bad_attr(&xv_panel_item_pkg, which_attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	return (Xv_opaque) NULL;
    }
}

Pkg_private Pixfont *
item_get_panel_font(ip)
    Item_info      *ip;
{
    Pixfont        *font = (Pixfont *) xv_get(PANEL_PUBLIC(ip->panel), WIN_FONT);

    return (font);
}
