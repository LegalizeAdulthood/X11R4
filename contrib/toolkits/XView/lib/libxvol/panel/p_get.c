#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_get.c 20.23 89/10/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */


#include <xview_private/panel_impl.h>
#include <xview/scrollbar.h>

static int      shrink_to_fit();

#define MAX_NEGATIVE_SHRINK 2000
#define SHRINK_MARGIN       4


Pkg_private     Xv_opaque
panel_get_attr(panel_public, status, attr, valist)
    Panel           panel_public;
    int            *status;
    register Panel_attr attr;
    va_list         valist;
{
    register Panel_info *panel = PANEL_PRIVATE(panel_public);

    switch (attr) {
      case OPENWIN_VIEW_CLASS:
	return (Xv_opaque) PANEL_VIEW;
      case PANEL_CLIENT_DATA:
	return panel->client_data;

	/* laf */
      case PANEL_BOLD_FONT:
	return (Xv_opaque) panel->bold_font;

      case PANEL_LABEL_BOLD:
	return (Xv_opaque) label_bold_flag(panel);

      case PANEL_LABEL_INVERTED:
	return (Xv_opaque) label_inverted_flag(panel);

      case PANEL_BLINK_CARET:
	return (Xv_opaque) blinking(panel);

      case PANEL_CARET_ITEM:
	return (Xv_opaque) (panel->caret ? ITEM_PUBLIC(panel->caret) : 0);

      case PANEL_FIRST_ITEM:
	return (Xv_opaque) (panel->items ? ITEM_PUBLIC(panel->items) : 0);
	break;

      case PANEL_EVENT_PROC:
	return (Xv_opaque) panel->event_proc;

      case PANEL_BACKGROUND_PROC:
	return (Xv_opaque) panel->ops->handle_event;

      case PANEL_REPAINT_PROC:
	return (Xv_opaque) panel->repaint_proc;

      case PANEL_ITEM_X:
	return (Xv_opaque) panel->item_x;

      case PANEL_ITEM_Y:
	return (Xv_opaque) panel->item_y;

      case PANEL_ITEM_X_GAP:
	return (Xv_opaque) panel->item_x_offset;

      case PANEL_ITEM_Y_GAP:
	return (Xv_opaque) panel->item_y_offset;

      case PANEL_EXTRA_PAINT_WIDTH:
	return (Xv_opaque) panel->extra_width;

      case PANEL_EXTRA_PAINT_HEIGHT:
	return (Xv_opaque) panel->extra_height;

      case PANEL_LAYOUT:
	return (Xv_opaque) panel->layout;

      case PANEL_ACCEPT_KEYSTROKE:
	return (Xv_opaque) wants_key(panel);

      case PANEL_DEFAULT_ITEM:
	return (panel->default_item ? panel->default_item :
		panel->items ? ITEM_PUBLIC(panel->items) : 0);

      case WIN_FIT_WIDTH:
	/* laf */
	return (Xv_opaque) shrink_to_fit(panel, TRUE, panel_shrink_margin(panel));

      case WIN_FIT_HEIGHT:
	/* laf */
	return (Xv_opaque) shrink_to_fit(panel, FALSE, panel_shrink_margin(panel));

      case WIN_TYPE:		/* SunView1.X compatibility */
	return (Xv_opaque) PANEL_TYPE;

      default:
	xv_check_bad_attr(&xv_panel_pkg, attr);
	*status = XV_ERROR;
	return (Xv_opaque) 0;

    }
}


static int
shrink_to_fit(panel, do_width, excess)
    register Panel_info *panel;
    int             do_width;
    register int    excess;
{
    register Item_info *ip;
    register int    low_point = 0;
    register int    right_point = 0;
    int             new_size;
    Scrollbar       sb;

    if (!do_width) {
	for (ip = panel->items; ip; ip = ip->next)
	    low_point = MAX(low_point, ip->rect.r_top + ip->rect.r_height);
	new_size = low_point + excess + panel->extra_height;
	sb = xv_get(PANEL_PUBLIC(panel), WIN_HORIZONTAL_SCROLLBAR);
	if (sb)
	    new_size += (int) xv_get(sb, XV_HEIGHT);
    } else {
	for (ip = panel->items; ip; ip = ip->next)
	    right_point = MAX(right_point, ip->rect.r_left + ip->rect.r_width);
	new_size = right_point + excess + panel->extra_width;
	sb = xv_get(PANEL_PUBLIC(panel), WIN_VERTICAL_SCROLLBAR);
	if (sb)
	    new_size += (int) xv_get(sb, XV_WIDTH);
    }
    return new_size;
}

static
panel_shrink_margin(panel)
    register Panel_info *panel;
{
    register int    v;

    switch (window_font_size_for_client(PANEL_PUBLIC(panel))) {
      case 8:
	v = 8;
	break;
      case 10:
	v = 8;
	break;
      case 12:
	v = 12;
	break;
      case 14:
	v = 16;
	break;
      case 16:
	v = 20;
	break;
      case 19:
	v = 23;
	break;
      case 24:
	v = 28;
	break;
      case 32:
	v = 32;
	break;
    }
    return (v);
}
