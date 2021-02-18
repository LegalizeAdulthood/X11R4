#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_set.c 20.31 89/10/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */


#include <xview_private/panel_impl.h>
#include <xview/font.h>
#include <xview/scrollbar.h>
#include <xview/xv_xrect.h>
#include <xview_private/draw_impl.h>

void            panel_refont();	/* laf */

extern Xv_xrectlist *screen_get_clip_rects();


Pkg_private     Xv_opaque
panel_set_avlist(panel_public, avlist)
    Panel           panel_public;
    register Attr_avlist avlist;
{
    register Panel_info *panel = PANEL_PRIVATE(panel_public);
    register Panel_attr attr;
    Xv_xrectlist    *clip_xrects;
    Display	    *display;
    int             extra_width = -1, extra_height = -1;
    Xv_Drawable_info *info;
    Scrollbar       new_h_scrollbar = 0;
    Panel_ops      *new_ops;
    Scrollbar       new_v_scrollbar = 0;
    Xv_opaque	    pw;
    Xv_Screen	    screen;
    Drawable	    xid;

    for (attr = (Panel_attr) avlist[0]; attr;
	 avlist = attr_next(avlist), attr = (Panel_attr) avlist[0]) {
	switch (attr) {
	  case PANEL_CLIENT_DATA:
	    panel->client_data = avlist[1];
	    break;

	    /* laf begin */
	  case PANEL_BOLD_FONT:{
		Pixfont        *new_bold_font = (Pixfont *) (avlist[1]);
		if (new_bold_font) {
		    if (panel->bold_font)
			(void) xv_set(panel->bold_font,
				      XV_DECREMENT_REF_COUNT, 0);
		    panel->bold_font = new_bold_font;
		    (void) xv_set(panel->bold_font,
				  XV_INCREMENT_REF_COUNT, 0);
		}
		break;
	    }
	    /* laf end */

	  case PANEL_BLINK_CARET:
	    if ((int) avlist[1])
		panel->status |= BLINKING;
	    else
		panel->status &= ~BLINKING;
	    panel_caret_on(panel, TRUE);
	    break;

	  case PANEL_CARET_ITEM:
	    panel_caret_on(panel, FALSE);
	    panel->caret = ITEM_PRIVATE(avlist[1]);
	    panel_caret_on(panel, TRUE);
	    break;

	  case PANEL_EVENT_PROC:
	    panel->event_proc = (int (*) ()) avlist[1];
	    if (!panel->event_proc)
		panel->event_proc = panel_nullproc;
	    break;

	  case PANEL_REPAINT_PROC:
	    panel->repaint_proc = (int (*) ()) avlist[1];
	    break;

	  case PANEL_BACKGROUND_PROC:
	    if (!ops_set(panel)) {
		new_ops = xv_alloc(Panel_ops);
		*new_ops = *panel->ops;
		panel->ops = new_ops;
		panel->flags |= OPS_SET;
	    }
	    panel->ops->handle_event = (void (*) ()) avlist[1];
	    if (!panel->ops->handle_event)
		panel->ops->handle_event = (void (*) ()) panel_nullproc;
	    break;

	  case PANEL_ITEM_X_GAP:
	    panel->item_x_offset = (int) avlist[1];
	    break;

	  case PANEL_ITEM_Y_GAP:
	    panel->item_y_offset = (int) avlist[1];
	    break;

	  case PANEL_EXTRA_PAINT_WIDTH:
	    extra_width = (int) avlist[1];
	    break;

	  case PANEL_EXTRA_PAINT_HEIGHT:
	    extra_height = (int) avlist[1];
	    break;

	  case PANEL_SHOW_MENU:
	    /*
	     * Ignored; menus are always shown where appropriate. Retained
	     * for compatibility.
	     */
	    break;

	  case PANEL_LABEL_BOLD:
	    if (avlist[1])
		panel->flags |= LABEL_BOLD;
	    else
		panel->flags &= ~LABEL_BOLD;
	    break;

	  case PANEL_LABEL_INVERTED:
	    if (avlist[1])
		panel->flags |= LABEL_INVERTED;
	    else
		panel->flags &= ~LABEL_INVERTED;
	    break;

	  case PANEL_LAYOUT:
	    switch ((Panel_setting) avlist[1]) {
	      case PANEL_HORIZONTAL:
	      case PANEL_VERTICAL:
		panel->layout = (Panel_setting) avlist[1];
		break;
	      default:		/* invalid layout */
		break;
	    }
	    break;

	  case PANEL_PAINT:
	    panel->repaint = (Panel_setting) avlist[1];
	    break;

	  case WIN_VERTICAL_SCROLLBAR:
	  case OPENWIN_VERTICAL_SCROLLBAR:
	    new_v_scrollbar = (Scrollbar) avlist[1];
	    break;

	  case WIN_HORIZONTAL_SCROLLBAR:
	  case OPENWIN_HORIZONTAL_SCROLLBAR:
	    new_h_scrollbar = (Scrollbar) avlist[1];
	    break;

	  case PANEL_ACCEPT_KEYSTROKE:
	    if (avlist[1])
		panel->flags |= WANTS_KEY;
	    else
		panel->flags &= ~WANTS_KEY;
	    break;

	  case PANEL_DEFAULT_ITEM:
	    panel->default_item = (Panel_item) avlist[1];
	    break;

	  case WIN_SET_FOCUS:{
		Xv_Drawable_info *info;
		Xv_Window       pw;

		ATTR_CONSUME(avlist[0]);

		if (!panel->caret)
		    return ((Xv_opaque) XV_ERROR);

		/*
		 * find the first paint window that can accept kbd input and
		 * set the input focus to it.  Only do this if we have a
		 * caret/text item.  Since panels always assigns its own
		 * input focus, there is no need to check xv_no_focus().
		 */
		PANEL_EACH_PAINT_WINDOW(panel, pw)
		    DRAWABLE_INFO_MACRO(pw, info);
		if (win_getinputcodebit((Inputmask *) xv_get(pw,
						WIN_INPUT_MASK), KBD_USE)) {
		    win_set_kbd_focus(panel_public, xv_xid(info));
		    return (XV_OK);
		}
		PANEL_END_EACH_PAINT_WINDOW
		    return ((Xv_opaque) XV_ERROR);
	    }

	  case XV_END_CREATE:
	    pw = panel->paint_window->pw;
	    DRAWABLE_INFO_MACRO(pw, info);
	    display = xv_display(info);
	    xid = xv_xid(info);
	    panel->gc = XCreateGC(display, xid, 0, 0);
	    screen = xv_screen(info);
	    clip_xrects = screen_get_clip_rects(screen);
	    if (clip_xrects->count)
		XSetClipRectangles(display, panel->gc, 0, 0,
                    clip_xrects->rect_array, clip_xrects->count, Unsorted);
	    break;

	  default:
	    xv_check_bad_attr(&xv_panel_pkg, attr);
	    break;
	}
    }

    /* set up any scrollbars */
    if (new_v_scrollbar != NULL &&
    (int (*) ()) xv_get(new_v_scrollbar, SCROLLBAR_NORMALIZE_PROC) == NULL) {
	xv_set(new_v_scrollbar,
	       SCROLLBAR_NORMALIZE_PROC, panel_normalize_scroll,
	       0);
    }
    if (new_h_scrollbar != NULL &&
    (int (*) ()) xv_get(new_h_scrollbar, SCROLLBAR_NORMALIZE_PROC) == NULL) {
	xv_set(new_h_scrollbar,
	       SCROLLBAR_NORMALIZE_PROC, panel_normalize_scroll,
	       0);
    }
    if (extra_width >= 0 && extra_width != panel->extra_width) {
	panel->extra_width = extra_width;
    }
    if (extra_height >= 0 && extra_height != panel->extra_height) {
	panel->extra_height = extra_height;
    }
    if (panel->extra_width == extra_width || panel->extra_height == extra_height) {
	panel_update_scrolling_size(panel_public);
    }
    return XV_OK;
}


Pkg_private void
panel_refont(panel, arg)
    register Panel_info *panel;
    int             arg;
{
    register Panel_item item;
    register Panel  panel_public = PANEL_PUBLIC(panel);
    register Item_info *ip;
    register Panel_image *label;
    register struct pixrect *choice_off_image, *choice_on_image;
    register struct pixrect *toggle_off_image, *toggle_on_image;
    Pixfont        *panel_font, *bold_font, *old_win_font, *old_bold_font, *new_win_font,
                   *new_bold_font;
    int             label_bold, font_size, bold_font_size, item_x, item_y, row_gap,
                    col_gap, left_margin, top_margin, item_row, item_col;

    old_win_font = (Pixfont *) xv_get(panel_public, XV_FONT);
    new_win_font = (old_win_font) ? (Pixfont *) xv_find(
							panel_public, FONT,
				   FONT_RESCALE_OF, old_win_font, (int) arg,
							0)
	: (Pixfont *) 0;
    if (new_win_font) {
	(void) xv_set(old_win_font, XV_INCREMENT_REF_COUNT, 0);
	(void) xv_set(panel_public, XV_FONT, new_win_font, 0);
	panel_font = new_win_font;
    } else
	panel_font = old_win_font;
    font_size = (int) xv_get(panel_font, FONT_SIZE);

    old_bold_font = panel->bold_font;
    new_bold_font = (old_bold_font) ? (Pixfont *) xv_find(
							  panel_public, FONT,
				  FONT_RESCALE_OF, old_bold_font, (int) arg,
							  0)
	: (Pixfont *) 0;
    if (new_bold_font) {
	bold_font = new_bold_font;
	(void) xv_set(panel_public, PANEL_BOLD_FONT, new_bold_font, 0);
	bold_font_size = (int) xv_get(new_bold_font, FONT_SIZE);
    } else {
	bold_font = old_bold_font;
	bold_font_size = font_size;
    }

    if ((!new_win_font) && (!new_bold_font))
	return;

    if (font_size <= 10) {
	choice_off_image = &panel_choice_off_12_pr;
	choice_on_image = &panel_choice_on_12_pr;
	toggle_off_image = &panel_toggle_off_10_pr;
	toggle_on_image = &panel_toggle_on_10_pr;
    } else if (font_size <= 12) {
	choice_off_image = &panel_choice_off_12_pr;
	choice_on_image = &panel_choice_on_12_pr;
	toggle_off_image = &panel_toggle_off_12_pr;
	toggle_on_image = &panel_toggle_on_12_pr;
    } else if (font_size <= 14) {
	choice_off_image = &panel_choice_off_14_pr;
	choice_on_image = &panel_choice_on_14_pr;
	toggle_off_image = &panel_toggle_off_14_pr;
	toggle_on_image = &panel_toggle_on_14_pr;
    } else if (font_size <= 19) {
	choice_off_image = &panel_choice_off_20_pr;
	choice_on_image = &panel_choice_on_20_pr;
	toggle_off_image = &panel_toggle_off_19_pr;
	toggle_on_image = &panel_toggle_on_19_pr;
    } else if (font_size <= 24) {
	choice_off_image = &panel_choice_off_24_pr;
	choice_on_image = &panel_choice_on_24_pr;
	toggle_off_image = &panel_toggle_off_19_pr;
	toggle_on_image = &panel_toggle_on_19_pr;
    } else if (font_size > 24) {
	choice_off_image = &panel_choice_off_32_pr;
	choice_on_image = &panel_choice_on_32_pr;
	toggle_off_image = &panel_toggle_off_19_pr;
	toggle_on_image = &panel_toggle_on_19_pr;
    }
    row_gap = (int) xv_get(panel_public, WIN_ROW_GAP);
    col_gap = (int) xv_get(panel_public, WIN_COLUMN_GAP);
    left_margin = (int) xv_get(panel_public, WIN_LEFT_MARGIN);
    top_margin = (int) xv_get(panel_public, WIN_TOP_MARGIN);

    PANEL_EACH_ITEM(panel_public, item)
	ip = ITEM_PRIVATE(item);
    if (new_win_font) {
	item_x = (int) xv_get(ITEM_PUBLIC(ip), PANEL_ITEM_X);
	item_y = (int) xv_get(ITEM_PUBLIC(ip), PANEL_ITEM_Y);
	item_col = column_from_absolute_x(item_x, col_gap, top_margin,
					  new_win_font);
	item_row = row_from_absolute_y(item_y, row_gap, left_margin,
				       new_win_font);
	(void) xv_set(ITEM_PUBLIC(ip),
		      PANEL_ITEM_X, xv_col(panel_public, item_col),
		      PANEL_ITEM_Y, xv_row(panel_public, item_row),
		      PANEL_PAINT, PANEL_NONE,
		      0);
    }
    label = &ip->label;
    if (is_string(label)) {

	label_bold = (int) xv_get(
				  ITEM_PUBLIC(ip), PANEL_LABEL_BOLD);

	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_LABEL_FONT, panel_font,
	       PANEL_LABEL_STRING, image_string(label),
	       0);

	if (label_bold) {
	    xv_set(ITEM_PUBLIC(ip),
		   PANEL_PAINT, PANEL_NONE,
		   PANEL_LABEL_BOLD, label_bold,
		   0);
	}
    }
    xv_set(ITEM_PUBLIC(ip),
	   PANEL_PAINT, PANEL_NONE,
	   PANEL_LABEL_FONT, panel_font,
	   0);
    switch (ip->item_type) {

      case PANEL_MESSAGE_ITEM:
	break;

      case PANEL_BUTTON_ITEM:{
	    char           *label = (char *) xv_get(
				       ITEM_PUBLIC(ip), PANEL_LABEL_STRING);
	    if (label)		/* don't scale image buttons */
		xv_set(ITEM_PUBLIC(ip),
		       PANEL_PAINT, PANEL_NONE,
		       PANEL_LABEL_STRING, label,
		       0);
	    break;
	}

      case PANEL_TOGGLE_ITEM:
	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_MARK_IMAGES, toggle_on_image, 0,
	       PANEL_NOMARK_IMAGES, toggle_off_image, 0,
	       PANEL_CHOICE_FONTS, panel_font, 0,
	       0);
	break;

      case PANEL_CHOICE_ITEM:
	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_MARK_IMAGES, choice_on_image, 0,
	       PANEL_NOMARK_IMAGES, choice_off_image, 0,
	       PANEL_CHOICE_FONTS, panel_font, 0,
	       0);
	break;

      case PANEL_TEXT_ITEM:
      case PANEL_SLIDER_ITEM:
	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_VALUE_FONT, panel_font,
	       0);
	break;


      default:
	break;
    }
    /*
     * undecided if we should paint it.  Damage will do it for free when it
     * is resized.
     */
    panel_paint(ITEM_PUBLIC(ip), PANEL_CLEAR);
    PANEL_END_EACH
	if (new_win_font) {
	(void) xv_set(panel_public, XV_FONT, old_win_font, 0);
	(void) xv_set(old_win_font, XV_DECREMENT_REF_COUNT, 0);
    }
}


static int
column_from_absolute_x(x_position, col_gap, left_margin, font)
    int             x_position, col_gap, left_margin;
    Pixfont        *font;
{
    x_position -= left_margin;
    return (x_position / (font->pf_defaultsize.x + col_gap));
}


static int
row_from_absolute_y(y_position, row_gap, top_margin, font)
    int             y_position, row_gap, top_margin;
    Pixfont        *font;
{
    y_position -= top_margin;
    return (y_position / (font->pf_defaultsize.y + row_gap));
}
