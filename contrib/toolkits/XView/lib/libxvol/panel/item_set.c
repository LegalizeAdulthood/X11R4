#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)item_set.c 20.56 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */


#include <xview_private/panel_impl.h>
#include <xview/openmenu.h>

Xv_private      panel_item_parent();

Pkg_private void panel_adjust_label_size();

static Xv_opaque item_set_generic();
static Xv_opaque item_set_ops();

extern struct pr_size xv_pf_textwidth();

extern Pixrect  panel_pulldown_pr;
extern Pixrect  panel_pullright_pr;


Pkg_private     Xv_opaque
item_set_avlist(item_public, avlist)
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    short           caret_was_on = FALSE;
    Panel_setting   saved_repaint = ip->repaint;


    /*
     * BUG ALERT -- this code is called after the item specific set procedure
     * -- thus the real work has already been done. That is we may have
     * already overwritten part of the caret. To do this right we need to
     * move turning the caret off and on into the item specific attr handlers
     */

    /* if a client has called panel_item_parent this item may not */
    /* have a parent -- do nothing in this case */
    if (ip->panel == NULL) {
	return ((Xv_opaque) XV_ERROR);
    }
    if (ip->panel->caret_on) {
	caret_was_on = TRUE;
	panel_caret_on(ip->panel, FALSE);
    }
    /* set any generic attributes */
    (void) item_set_generic(ip, avlist);


    /* set the menu attributes */
    (void) panel_set_menu(ip, avlist);

    /* set the ops vector attributes */
    (void) item_set_ops(ip, avlist);

    if (panel_painted(ip->panel))
	(void) panel_redisplay_item(ip, ip->repaint, TRUE);

    /*
     * restore the item's original repaint behavior.  This allows clients to
     * specify a 'one time' repaint policy for this call to panel_set() only.
     */
    ip->repaint = saved_repaint;

    /* if current item is caret item, turn the caret on regardless */
    if (caret_was_on || ip == ip->panel->caret)
	panel_caret_on(ip->panel, TRUE);

    return XV_OK;
}


static          Xv_opaque
item_set_generic(ip, avlist)
    register Item_info *ip;
    register Attr_avlist avlist;
{
    register Panel_attribute attr;
    register Panel_info *panel = ip->panel;
    register Panel_image *label = &ip->label;
    int             item_x, item_y;
    int             label_x, label_y;
    int             next_col_gap;
    int             next_row_gap;
    int             start_new_col = FALSE;
    int             start_new_row = FALSE;
    int             value_x, value_y;
    short           item_x_changed = FALSE;
    short           item_y_changed = FALSE;
    short           label_size_changed = FALSE;
    short           label_x_changed = FALSE;
    short           label_y_changed = FALSE;
    short           value_x_changed = FALSE;
    short           value_y_changed = FALSE;
    short           layout_changed = FALSE;
    short           label_font_changed = FALSE;
    int             label_type = -1;	/* no new label yet */
    int             label_bold = label_bold_flag(panel);
    int		    label_boxed = image_boxed(label);
    int             label_inverted = image_inverted(label);
    struct pr_size  label_size;
    Xv_opaque       label_data;
    Rect            deltas, *view_rect;
    short           potential_new_rect = FALSE;

    while (attr = (Panel_attribute) * avlist++) {
	switch (attr) {
	  case XV_SHOW:
	    if ((int) *avlist++) {
		if (hidden(ip)) {
		    ip->flags &= ~HIDDEN;
		    (*ip->ops->restore) (ip);
		    panel_update_extent(panel, ip->rect);
		}
	    } else if (!hidden(ip)) {
		panel_clear_item(ip);
		ip->flags |= HIDDEN;
		(*ip->ops->remove) (ip);
	    }
	    break;

	  case PANEL_SHOW_MENU:
	    /*
	     * Ignored; menus are always shown where applicable. Retained for
	     * compatibility.
	     */
	    avlist++;
	    break;

	    /* PANEL_ITEM_X attr must still be supported in addition to XV_X */
	    /* and XV_Y because they can be used on the panel. When used */
	    /* on the panel PANEL_ITEM_X != XV_X on the panel */

	  case XV_X:
	  case PANEL_ITEM_X:
	    item_x = (int) *avlist++;
	    ip->flags |= ITEM_X_FIXED;
	    item_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case XV_Y:
	  case PANEL_ITEM_Y:
	    item_y = (int) *avlist++;
	    ip->flags |= ITEM_Y_FIXED;
	    item_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_NEXT_COL:
	    next_col_gap = (int) *avlist++;
	    start_new_col = TRUE;
	    break;

	  case PANEL_NEXT_ROW:
	    next_row_gap = (int) *avlist++;
	    start_new_row = TRUE;
	    break;

	  case PANEL_LABEL_X:
	    label_x = (int) *avlist++;
	    ip->flags |= LABEL_X_FIXED;
	    label_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_Y:
	    label_y = (int) *avlist++;
	    ip->flags |= LABEL_Y_FIXED;
	    label_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_VALUE_X:
	    value_x = (int) *avlist++;
	    ip->flags |= VALUE_X_FIXED;
	    value_x_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_VALUE_Y:
	    value_y = (int) *avlist++;
	    ip->flags |= VALUE_Y_FIXED;
	    value_y_changed = TRUE;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_IMAGE:
	    label_type = IM_PIXRECT;
	    label_data = *avlist++;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_STRING:
	    label_type = IM_STRING;
	    label_data = *avlist++;
	    potential_new_rect = TRUE;
	    break;

	  case PANEL_LABEL_FONT:
	    /* Sunview1 attribute: ignored for compatibility */
	    avlist++;
	    break;

	  case PANEL_LABEL_BOLD:
	    if (ip->item_type == PANEL_MESSAGE_ITEM) {
		if (*avlist++) {
		    panel_set_bold_label_font(ip);
		    label_font_changed = TRUE;
		    potential_new_rect = TRUE;
		}
	    } else {
		avlist = attr_skip(attr, avlist);
	    }
	    break;

	  case PANEL_LABEL_BOXED:
	    label_boxed = (int) *avlist++;
	    image_set_boxed(label, label_boxed);
	    break;

	  case PANEL_LABEL_INVERTED:
	    label_inverted = (int) *avlist++;
	    image_set_inverted(label, label_inverted);
	    break;

	  case PANEL_LABEL_WIDTH:
	    ip->label_width = (int) *avlist++;
	    break;

	  case PANEL_LAYOUT:
	    switch ((Panel_setting) * avlist) {
	      case PANEL_HORIZONTAL:
	      case PANEL_VERTICAL:
		ip->layout = (Panel_setting) * avlist++;
		layout_changed = TRUE;
		potential_new_rect = TRUE;
		break;

	      default:
		/* invalid layout */
		avlist++;
		break;
	    }
	    break;

	  case PANEL_MENU_ITEM:
	    if (*avlist++)
		ip->flags |= IS_MENU_ITEM;
	    else
		ip->flags &= ~IS_MENU_ITEM;
	    break;

	  case PANEL_NOTIFY_PROC:
	    ip->notify = (int (*) ()) *avlist++;
	    if (!ip->notify)
		ip->notify = panel_nullproc;
	    break;

	  case PANEL_NOTIFY_STATUS:
	    ip->notify_status = (int) *avlist++;
	    break;

	  case PANEL_PAINT:
	    ip->repaint = (Panel_setting) * avlist++;
	    break;

	  case PANEL_ACCEPT_KEYSTROKE:
	    if (*avlist++)
		ip->flags |= WANTS_KEY;
	    else
		ip->flags &= ~WANTS_KEY;
	    break;

	  case PANEL_ITEM_MENU:
	    if (ip->menu)
		xv_set(ip->menu, XV_DECREMENT_REF_COUNT, 0);
	    if (ip->menu = (Xv_opaque) (*avlist++))
		xv_set(ip->menu,
		       MENU_PULLDOWN, ip->panel->layout == PANEL_HORIZONTAL,
		       XV_INCREMENT_REF_COUNT,
		       0);
	    if (label_type == -1) {
		label_type = label->im_type;	/* force label (image) to be
						 * remade */
		if (label_type == IM_STRING)
		    label_data = (Xv_opaque) image_string(label);
		else
		    label_data = (Xv_opaque) image_pixrect(label);
	    }
	    break;

	  case PANEL_CLIENT_DATA:
	    ip->client_data = *avlist++;
	    break;

	  case PANEL_INACTIVE:
	    if (*avlist++)
		ip->flags |= INACTIVE;
	    else
		ip->flags &= ~INACTIVE;
	    break;

	  case PANEL_ITEM_COLOR:
	    ip->color_index = (int) *avlist++;
	    break;

	  case XV_END_CREATE:
	    /* move the item if not fixed and past the right edge */
	    if (!(item_fixed(ip) || label_fixed(ip) || value_fixed(ip))) {
		/* only move it down if its advantageous, i.e. the item is */
		/* not in column 0 */
		view_rect = viewable_rect(panel, panel->paint_window->pw);
		if ((ip->rect.r_left > 0) &&
		    (rect_right(&ip->rect) > rect_right(view_rect))) {
		    deltas.r_left = PANEL_ITEM_X_START - ip->rect.r_left;
		    deltas.r_top = panel->max_item_y + panel->item_y_offset;
		    /* tell the item to move */
		    (void) panel_layout(ip, &deltas);
		}
	    }
	    ip->flags |= CREATED;

	    /*
	     * for scrolling computations, note new extent of panel, tell
	     * scrollbars
	     */
	    if (!hidden(ip))
		panel_update_extent(ip->panel, ip->rect);
	    break;

	    /*
	     * *** FIXME: All the unused panel attributes must be listed here
	     * so that xv_check_bad_attr can correctly call out illegal
	     * attributes. This ugliness will be removed when a generic item
	     * package is implemented.
	     */
	  case PANEL_BACKGROUND_PROC:
	  case PANEL_BLINK_CARET:
	  case PANEL_BOLD_FONT:
	  case PANEL_CARET_ITEM:
	  case PANEL_CHOICES_BOLD:
	  case PANEL_CHOICE_FONT:
	  case PANEL_CHOICE_FONTS:
	  case PANEL_CHOICE_IMAGE:
	  case PANEL_CHOICE_IMAGES:
	  case PANEL_CHOICE_STRING:
	  case PANEL_CHOICE_STRINGS:
	  case PANEL_CHOICE_X:
	  case PANEL_CHOICE_XS:
	  case PANEL_CHOICE_Y:
	  case PANEL_CHOICE_YS:
	  case PANEL_CHOOSE_NONE:
	  case PANEL_CHOOSE_ONE:
	  case PANEL_DEFAULT_ITEM:
	  case PANEL_DEFAULT_VALUE:
	  case PANEL_DISPLAY_LEVEL:
	  case PANEL_EVENT_PROC:
	  case PANEL_EXTRA_PAINT_HEIGHT:
	  case PANEL_EXTRA_PAINT_WIDTH:
	  case PANEL_FEEDBACK:
	  case PANEL_FIRST_ITEM:
	  case PANEL_ITEM_CLASS:
	  case PANEL_ITEM_RECT:
	  case PANEL_ITEM_X_GAP:
	  case PANEL_ITEM_Y_GAP:
	  case PANEL_LIST_CLIENT_DATA:
	  case PANEL_LIST_CLIENT_DATAS:
	  case PANEL_LIST_DELETE:
	  case PANEL_LIST_DISPLAY_ROWS:
	  case PANEL_LIST_FONT:
	  case PANEL_LIST_FONTS:
	  case PANEL_LIST_GLYPH:
	  case PANEL_LIST_GLYPHS:
	  case PANEL_LIST_HEIGHT:
	  case PANEL_LIST_INSERT:
	  case PANEL_LIST_NROWS:
	  case PANEL_LIST_ROW_HEIGHT:
	  case PANEL_LIST_SELECT:
	  case PANEL_LIST_SELECTED:
	  case PANEL_LIST_STRING:
	  case PANEL_LIST_STRINGS:
	  case PANEL_LIST_WIDTH:
	  case PANEL_MARK_IMAGE:
	  case PANEL_MARK_IMAGES:
	  case PANEL_MARK_X:
	  case PANEL_MARK_XS:
	  case PANEL_MARK_Y:
	  case PANEL_MARK_YS:
	  case PANEL_MASK_CHAR:
	  case PANEL_MAX_VALUE:
	  case PANEL_MENU_MARK_IMAGE:
	  case PANEL_MENU_NOMARK_IMAGE:
	  case PANEL_MIN_VALUE:
	  case PANEL_NEXT_ITEM:
	  case PANEL_NOMARK_IMAGE:
	  case PANEL_NOMARK_IMAGES:
	  case PANEL_NOTIFY_LEVEL:
	  case PANEL_NOTIFY_STRING:
	  case XV_OWNER:
	  case PANEL_READ_ONLY:
	  case PANEL_REPAINT_PROC:
	  case PANEL_SHOW_MENU_MARK:
	  case PANEL_SHOW_RANGE:
	  case PANEL_SHOW_VALUE:
	  case PANEL_SLIDER_WIDTH:
	  case PANEL_TOGGLE_VALUE:
	  case PANEL_VALUE:
	  case PANEL_VALUE_DISPLAY_LENGTH:
	  case PANEL_VALUE_FONT:
	  case PANEL_VALUE_STORED_LENGTH:
	  case PANEL_VALUE_UNDERLINED:
	  case PANEL_CHOICE_OFFSET:
	  case PANEL_TEXT_CURSOR:
	    avlist = attr_skip(attr, avlist);
	    break;

	  default:
	    xv_check_bad_attr(&xv_panel_item_pkg, attr);
	    avlist = attr_skip(attr, avlist);
	    break;
	}
    }

    /*
     * first handle any item position changes.
     */
    if (start_new_row) {
	if (!item_x_changed)
	    item_x = PANEL_ITEM_X_START;
	item_y = panel->lowest_bottom +
	    (next_row_gap >= 0 ? next_row_gap : panel->item_y_offset);
    }
    if (start_new_col) {
	if (!item_y_changed)
	    item_y = PANEL_ITEM_Y_START;
	item_x = panel->rightmost_right +
	    (next_col_gap >= 0 ? next_col_gap : panel->item_x_offset);
	panel->current_col_x = item_x;
    }
    if (start_new_row || start_new_col || item_x_changed || item_y_changed) {
	rect_construct(&deltas, 0, 0, 0, 0);
	/* compute item offset */
	if (start_new_row || start_new_col || item_x_changed)
	    deltas.r_left = item_x - ip->rect.r_left;
	if (start_new_row || start_new_col || item_y_changed)
	    deltas.r_top = item_y - ip->rect.r_top;
	if (deltas.r_left || deltas.r_top)
	    /*
	     * ITEM_X or ITEM_Y has changed, so re-layout item in order to
	     * cause the entire item to change position.
	     */
	    (void) panel_layout(ip, &deltas);
    }
    /*
     * Now handle label size changes .
     */

    /*
     * update label font & boldness if needed. Note that this may be set
     * again below if a new label string or image has been given.
     */
    if (is_string(label) && label_font_changed) {
	label_size = xv_pf_textwidth(strlen(image_string(label)), image_font(label),
				     image_string(label));
	if (ip->label_width) {
	    label_size.x = ip->label_width;
	    panel_adjust_label_size(ip->item_type, &label_size,
			    ip->menu, ip->panel->layout, image_font(label));
	} else
	    panel_adjust_label_size(ip->item_type, &label_size,
			    ip->menu, ip->panel->layout, image_font(label));
	ip->label_rect.r_width = label_size.x;
	ip->label_rect.r_height = label_size.y;
	label_size_changed = TRUE;
    }
    /* free old label, allocate new */
    if (set(label_type)) {
	label_size = panel_make_image(image_font(label), label, label_type,
				    label_data, label_bold, label_inverted);
	if (ip->label_width) {
	    label_size.x = ip->label_width;
	    panel_adjust_label_size(ip->item_type, &label_size,
			    ip->menu, ip->panel->layout, image_font(label));
	} else
	    panel_adjust_label_size(ip->item_type, &label_size,
			    ip->menu, ip->panel->layout, image_font(label));
	ip->label_rect.r_width = label_size.x;
	ip->label_rect.r_height = label_size.y;
	label_size_changed = TRUE;
    }
#ifdef SUNVIEW1
    /*
     * if the label has changed, menu title may be dirty.
     */
    if (label_size_changed)
	ip->menu_status.title_dirty = TRUE;
#endif

    /*
     * use default positions for label or value if not specified.
     */
    if (layout_changed || label_size_changed || label_x_changed ||
	label_y_changed) {
	/*
	 * layout, label position or size has changed, so re-compute default
	 * value position.
	 */
	if (label_x_changed)
	    ip->label_rect.r_left = label_x;
	if (label_y_changed)
	    ip->label_rect.r_top = label_y;

	/* now move the value if it's not fixed */
	fix_positions(ip);
    }
    if (value_x_changed || value_y_changed) {
	/*
	 * value position has changed, so re-compute default label position.
	 */
	rect_construct(&deltas, 0, 0, 0, 0);
	if (value_x_changed) {
	    deltas.r_left = value_x - ip->value_rect.r_left;
	    ip->value_rect.r_left = value_x;
	}
	if (value_y_changed) {
	    deltas.r_top = value_y - ip->value_rect.r_top;
	    ip->value_rect.r_top = value_y;
	}
	if (deltas.r_left || deltas.r_top) {
	    /*
	     * VALUE_X or VALUE_Y has changed, so tell item to shift all its
	     * components (choices, marks etc.).
	     */
	    (*ip->ops->layout) (ip, &deltas);

	    /* now move the label if it's not fixed */
	    fix_positions(ip);
	}
    }
    /* make sure the item rect encloses the label and value */
    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);

    /* for scrolling computations, note new extent of panel, tell scrollbars */
    if (potential_new_rect && !hidden(ip))
	panel_update_extent(panel, ip->rect);

    return XV_OK;
}


/* Adjust size for borders around label */
Pkg_private void
panel_adjust_label_size(item_type, size, menu, item_layout, font)
    Panel_item_type item_type;
    struct pr_size *size;
    Xv_opaque       menu;	/* menu attached to this button (0 => none) */
    Panel_setting   item_layout;/* PANEL_HORIZONTAL or PANEL_VERTICAL */
    Pixfont        *font;
{
    if (item_type == PANEL_BUTTON_ITEM) {

	if (size->y <= 12) {
	    size->x += 15;	/* left text margin = 7, right = width-9 */
	    size->y += 11;	/* top text margin = 4, bottom = height-8 */
	} else if ((size->y > 12) && (size->y < 19)) {
	    size->x += 17;	/* left text margin = 8, right = width-10 */
	    size->y += 12;	/* top text margin = 5, bottom = height-8 */
	} else {
	    size->x += 20;	/* left text margin = 9; right = width-12 */
	    size->y += 15;	/* top text margin = 5; bottom = height-11 */
	}
	if (menu) {
	    if (item_layout == PANEL_VERTICAL) {
		size->x += panel_pullright_pr.pr_width;
		if (size->y < panel_pullright_pr.pr_height)
		    size->y = panel_pullright_pr.pr_height;
	    } else {
		size->x += panel_pulldown_pr.pr_width;
		if (size->y < panel_pulldown_pr.pr_height)
		    size->y = panel_pulldown_pr.pr_height;
	    }
	}
    }
}


/* fix_positions - of label and value rects */

static
fix_positions(ip)
    register Item_info *ip;
{
    if (!value_fixed(ip)) {
	struct rect     deltas;

	/* compute the value position which is to the right of the label */
	/* remember the old value rect position */
	rect_construct(&deltas, 0, 0, 0, 0);
	deltas.r_left = ip->value_rect.r_left;
	deltas.r_top = ip->value_rect.r_top;
	switch (ip->layout) {
	  case PANEL_HORIZONTAL:
	    /* after => to right of */
	    ip->value_rect.r_left = rect_right(&ip->label_rect) + 1 +
		(ip->label_rect.r_width ? LABEL_X_GAP : 0);
	    ip->value_rect.r_top = ip->label_rect.r_top;
	    break;

	  case PANEL_VERTICAL:
	    /* after => below */
	    ip->value_rect.r_left = ip->label_rect.r_left;
	    ip->value_rect.r_top = rect_bottom(&ip->label_rect) + 1 +
		(ip->label_rect.r_height ? LABEL_Y_GAP : 0);
	    break;
	}
	/* delta is new postion minus old position */
	deltas.r_left = ip->value_rect.r_left - deltas.r_left;
	deltas.r_top = ip->value_rect.r_top - deltas.r_top;
	if (deltas.r_left || deltas.r_top)
	    /*
	     * VALUE_X or VALUE_Y has changed, so tell item to shift all its
	     * components (choices, marks etc.).
	     */
	    (*ip->ops->layout) (ip, &deltas);
    }
    panel_fix_label_position(ip);
}


Pkg_private
panel_fix_label_position(ip)
    register Item_info *ip;
{
    if (label_fixed(ip))
	return;

    /* compute the label position as before the value. */
    switch (ip->layout) {
      case PANEL_HORIZONTAL:
	/* before => to left of */
	ip->label_rect.r_left = ip->value_rect.r_left;
	if (ip->label_rect.r_width > 0)
	    ip->label_rect.r_left -= ip->label_rect.r_width + LABEL_X_GAP;
	ip->label_rect.r_top = ip->value_rect.r_top;
	break;

      case PANEL_VERTICAL:
	/* before => above */
	ip->label_rect.r_left = ip->value_rect.r_left;
	ip->label_rect.r_top = ip->value_rect.r_top;
	if (ip->label_rect.r_height > 0)
	    ip->label_rect.r_top -= ip->label_rect.r_height + LABEL_Y_GAP;
	break;
    }
}

static          Xv_opaque
item_set_ops(ip, avlist)
    register Item_info *ip;
    register Attr_avlist avlist;
{
    register Panel_attr attr;
    register Panel_ops *new_ops;

    while (attr = (Panel_attr) * avlist++) {
	switch (attr) {
	  case PANEL_EVENT_PROC:
	    if (!ops_set(ip)) {
		new_ops = xv_alloc(Panel_ops);
		*new_ops = *ip->ops;
		ip->ops = new_ops;
		ip->flags |= OPS_SET;
	    }
	    ip->ops->handle_event = (void (*) ()) *avlist++;
	    if (!ip->ops->handle_event)
		ip->ops->handle_event = (void (*) ()) panel_nullproc;
	    break;

	  default:
	    /* skip past what we don't care about */
	    avlist = attr_skip(attr, avlist);
	    break;
	}
    }
    return XV_OK;
}


/****************************************************************

                 panel_item_parent

panel_item_parent has been supplied for internal SPD use only.
In particular the information management (database) group uses
it. The function is designed to let a client unparent and reparent
a panel item. One can implement a cache of items with this function
where the items are shared between multiple panels. We provided
this routine rather then making XV_OWNER settable
because too much of the item code expects to have a parent. We
would have had to rewrite too much of the panel code to make this
work from an attribute. As it is we provide this functionality
by letting the client use this routine to unset and reset an
item's parent. While an item has no parent a user can't set
any of its attributes. Code has been put in to prevent this.
The client can however get an items attributes that aren't
depenedent on the panel. If the item's attriibute does depend on
the panel NULL is returned.


The parent parameter is used to both reset and unset an item's
parent. If parent == NULL then the item will not have a parent
after it is removed from its current panel. If the parent != NULL
then the item will be reparented to parent

****************************************************************/

Xv_private
panel_item_parent(item, parent)
    Panel_item      item;
    Panel           parent;
{
    register Item_info *ip = ITEM_PRIVATE(item);
    register Panel_info *new_panel = NULL;
    register Panel_info *current_panel = ip->panel;

    if (parent != NULL) {
	new_panel = PANEL_PRIVATE(parent);
    }
    if (current_panel != NULL) {
	/* clear the  item */
	if (!current_panel->destroying) {
	    if (ip == current_panel->caret) {
		panel_caret_on(current_panel, FALSE);
	    }
	    panel_clear_item(ip);
	}
	/* unlink the item */
	panel_unlink(ip, FALSE);/* unlink is not part of a destroy */
    }
    if (new_panel != NULL) {
	ip->panel = new_panel;

	/* relink the item */
	panel_append(ip);
	(*ip->ops->restore) (ip);

	/* don't repaint -- the client is probably doing a set */
	/* right after this to reposition the item */
    }
}
