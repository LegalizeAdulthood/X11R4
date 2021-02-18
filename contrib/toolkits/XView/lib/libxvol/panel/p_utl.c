#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_utl.c 20.34 89/10/24";
#endif
#endif

/*************************************************************************/
/* p_util.c                                     */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/*************************************************************************/

#include <xview_private/panel_impl.h>
#include <xview/font.h>
#include <xview/sun.h>
#include <xview/openmenu.h>
#include <xview/pixwin.h>
#include <xview_private/draw_impl.h>
#include <X11/X.h>

/* caret handling functions */
extern void     (*panel_caret_on_proc) ();
extern void     (*panel_caret_invert_proc) ();
extern void     (*panel_seln_hilite_proc) ();

/* selection service functions */
extern void     (*panel_seln_inform_proc) ();
extern void     (*panel_seln_destroy_proc) ();

extern struct pr_size xv_pf_textwidth();
Xv_private GC 	    xv_find_proper_gc();

static short    empty_image[] = {0};
static          mpr_static(panel_empty_pr, 0, 0, 1, empty_image);

static short    caret_image[7] = {
    0x1000, 0x3800, 0x3800, 0x7C00, 0x7C00, 0xFE00, 0xFE00
};
static          mpr_static(panel_caret_pr, 7, 7, 1, caret_image);

static short    ghost_caret[7] = {
    0x1000, 0x2800, 0x5400, 0xAA00, 0x5400, 0x2800, 0x1000
};

mpr_static(panel_ghost_caret_pr, 7, 7, 1, ghost_caret);

static unsigned short gray50_dat[16] = {
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555,
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555
};

mpr_static(panel_gray_pr, 16, 16, 1, gray50_dat);

static unsigned short gray12_data[16] = {	/* 12 % grey pattern */
    0x8080, 0x1010, 0x0202, 0x4040,
    0x0808, 0x0101, 0x2020, 0x0404,
    0x8080, 0x1010, 0x0202, 0x4040,
    0x0808, 0x0101, 0x2020, 0x0404
};

mpr_static(panel_shade_pr, 16, 16, 1, gray12_data);


/*****************************************************************************/
/* panel_enclosing_rect                                                      */
/*****************************************************************************/

Pkg_private     Rect
panel_enclosing_rect(r1, r2)
    register Rect  *r1, *r2;
{
    /* if r2 is undefined then return r1 */
    if (r2->r_left == -1)
	return (*r1);

    return rect_bounding(r1, r2);
}

/*****************************************************************************/
/* panel_update_extent                                                       */
/* called from panel_attr.c                                                  */
/*****************************************************************************/

Pkg_private void
panel_update_extent(panel, rect)
    Panel_info     *panel;
    Rect            rect;
{
    int             v_end, h_end;

    v_end = panel_height(panel);
    if (rect.r_top + rect.r_height > v_end) {
	v_end = rect.r_top + rect.r_height + panel->extra_height;
	(void) xv_set(PANEL_PUBLIC(panel),
		      CANVAS_MIN_PAINT_HEIGHT, v_end, 0);
    }
    h_end = panel_width(panel);
    if (rect.r_left + rect.r_width > h_end) {
	h_end = rect.r_left + rect.r_width + panel->extra_width;
	(void) xv_set(PANEL_PUBLIC(panel),
		      CANVAS_MIN_PAINT_WIDTH, h_end, 0);
    }
}


/*****************************************************************************/
/* font char/pixel conversion routines                                       */
/*****************************************************************************/

Pkg_private
panel_col_to_x(font, col)
    Pixfont        *font;
    int             col;
{
    return (col * font->pf_defaultsize.x);
}


Pkg_private
panel_row_to_y(font, line)
    Pixfont        *font;
    int             line;
{
    return (line * font->pf_defaultsize.y);
}


Pkg_private
panel_x_to_col(font, x)
    Pixfont        *font;
    int             x;
{
    return (x / font->pf_defaultsize.x);
}


Pkg_private
panel_y_to_row(font, y)
    Pixfont        *font;
    int             y;
{
    return (y / font->pf_defaultsize.y);
}


/*****************************************************************************/
/* panel_make_image                                                           */
/*
 * if value is NULL, use "" or the empty pixrect instead. /****************************************************************************
 */

Pkg_private struct pr_size
panel_make_image(font, dest, type_code, value, bold_desired, inverted_desired)
    Pixfont        *font;
    Panel_image    *dest;
    int             type_code;
    Xv_opaque       value;
    int             bold_desired;
    int             inverted_desired;
{
    struct pr_size  size;
    char           *str;

    size.x = size.y = 0;
    dest->im_type = type_code;
    image_set_inverted(dest, inverted_desired);
    switch (type_code) {
      case IM_STRING:
	if (!value)
	    value = (Xv_opaque) "";
	if (!(str = panel_strsave(value)))
	    return (size);
	if (image_string(dest))
	    free(image_string(dest));
	image_set_string(dest, str);
	image_set_font(dest, font);
	image_set_bold(dest, bold_desired);
	size = xv_pf_textwidth(strlen(str), font, str);
	size.y = font->pf_defaultsize.y;	/* Use max height of font
						 * (ascent + descent) */
	if (bold_desired)
	    size.x += 1;
	break;

      case IM_PIXRECT:
	if (!value)
	    value = (Xv_opaque) & panel_empty_pr;
	image_set_pixrect(dest, (Pixrect *) value);
	size = ((Pixrect *) value)->pr_size;
	break;
    }
    return size;
}

Pkg_private void
panel_image_set_font(image, font)
    Panel_image    *image;
    Pixfont        *font;
{
    if (image_font(image)) {
	(void) xv_set(image_font(image), XV_DECREMENT_REF_COUNT, 0);
    }
    image_font(image) = font;
    if (font) {
	(void) xv_set(font, XV_INCREMENT_REF_COUNT, 0);
    }
}

/*****************************************************************************/
/* panel_successor -- returns the next unhidden item after ip.                */
/*****************************************************************************/

Pkg_private Item_info *
panel_successor(ip)
    register Item_info *ip;
{
    if (!ip)
	return NULL;

    for (ip = ip->next; ip && hidden(ip); ip = ip->next);

    return ip;
}

/*****************************************************************************/
/* panel_append                                                              */
/*****************************************************************************/

Pkg_private Item_info *
panel_append(ip)
    register Item_info *ip;
{
    Panel_info     *panel = ip->panel;
    register Item_info *ip_cursor;
    Rect            deltas;

    if (!panel->items)
	panel->items = ip;
    else {
	for (ip_cursor = panel->items;
	     ip_cursor->next != NULL;
	     ip_cursor = ip_cursor->next);
	ip_cursor->next = ip;
    }
    ip->next = NULL;

    /* item rect encloses the label and value */
    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);

    return (ip);
}


/*****************************************************************************/
/* panel_unlink                                                              */
/*****************************************************************************/

Pkg_private void
panel_unlink(ip, destroy)
    register Item_info *ip;
    int             destroy;	/* boolean: unlink is part of a destroy
				 * operation */
{
    Panel_info     *panel = ip->panel;
    register Item_info *prev_ip, *cur_ip;

    /* unlinked item is no longer current */
    if (panel->current == ip)
	panel->current = NULL;

    /* we assume that the caret is off by the time we are called */

    if (!destroy)
	/* remove from any private list */
	(*ip->ops->remove) (ip);

    /* find the previous item */
    for (prev_ip = (Item_info *) NULL, cur_ip = panel->items;
	 cur_ip != ip; prev_ip = cur_ip, cur_ip = cur_ip->next);

    /* unlink ip */
    if (prev_ip)
	prev_ip->next = ip->next;
    else
	panel->items = ip->next;

    /* NULL out parent pointer */
    ip->panel = NULL;

    /* update the default position of the next created item */
    (void) panel_find_default_xy(panel);
}



/*****************************************************************************/
/* panel_find_default_xy                                                     */
/* computes panel->item_x, panel->item_y, and panel->max_item_y based on the */
/* geometry of the current items in the panel.                               */
/* First the lowest "row" is found, then the default position is on that     */
/* row to the right of any items which intersect that row.                   */
/* The max_item_y is set to the height of the lowest item rectangle on the   */
/* lowest row.                                                               */
/*****************************************************************************/

Pkg_private
panel_find_default_xy(panel)
    Panel_info     *panel;
{
    register Item_info *ip;
    register int    lowest_top = PANEL_ITEM_Y_START;
    register int    rightmost_right = PANEL_ITEM_X_START;
    register int    lowest_bottom = PANEL_ITEM_Y_START;

    if (!panel->items) {
	panel->max_item_y = 0;
	panel->item_x = PANEL_ITEM_X_START;
	panel->item_y = PANEL_ITEM_Y_START;
	return;
    }
    /*
     * Horizontal layout: find the lowest row of any item Vertical layout:
     * find the lowest row in the current column
     */
    for (ip = panel->items; ip; ip = ip->next) {
	if (panel->layout == PANEL_VERTICAL) {
	    if (ip->rect.r_left >= panel->current_col_x)
		lowest_bottom = MAX(lowest_bottom, rect_bottom(&ip->rect));
	} else {
	    lowest_top = MAX(lowest_top, ip->rect.r_top);
	    lowest_bottom = MAX(lowest_bottom, rect_bottom(&ip->rect));
	}
    }

    /*
     * Horizontal layout: find the rightmost position on the lowest row
     * Vertical layout: find the rightmost position of any item
     */
    for (ip = panel->items; ip; ip = ip->next)
	if (panel->layout == PANEL_VERTICAL ||
	    rect_bottom(&ip->rect) >= lowest_top)
	    rightmost_right = MAX(rightmost_right, rect_right(&ip->rect));

    /* update the panel info */
    panel->max_item_y = lowest_bottom - lowest_top;	/* offset to next row */
    panel->item_x = rightmost_right + panel->item_x_offset;
    panel->item_y = lowest_top;
    panel->lowest_bottom = lowest_bottom;
    panel->rightmost_right = rightmost_right;

    /* advance to the next row if vertical layout or past right edge */
    if (panel->layout == PANEL_VERTICAL ||
	panel->item_x > viewable_width(panel, panel->paint_window->pw)) {
	panel->item_x = panel->current_col_x;
	panel->item_y = lowest_bottom + panel->item_y_offset;
	panel->max_item_y = 0;
    }
}


/*****************************************************************************/
/* panel_layout                                                               */
/* lays out the generic item, label & value rects in ip and calls the item's */
/* layout proc.                                                              */
/*****************************************************************************/

Pkg_private
panel_layout(ip, deltas)
    register Item_info *ip;
    register Rect  *deltas;
{
    /* item rect */
    ip->rect.r_left += deltas->r_left;
    ip->rect.r_top += deltas->r_top;

    /* label rect */
    ip->label_rect.r_left += deltas->r_left;
    ip->label_rect.r_top += deltas->r_top;

    /* value rect */
    ip->value_rect.r_left += deltas->r_left;
    ip->value_rect.r_top += deltas->r_top;

    /* item */
    (*ip->ops->layout) (ip, deltas);
}				/* panel_layout */


/****************************************************************************/
/* panel_paint_image                                                        */
/* paints image in pw in rect.                                              */
/****************************************************************************/
Pkg_private
panel_paint_image(panel, image, rect, grey_out, color_index)
    Panel_info     *panel;
    Panel_image    *image;
    Rect           *rect;
    register int    grey_out;
    int             color_index;
{
    int             top;
    Xv_Window       pw;
    int		    color;
    int		    color_op;
    int		    text_color_op;

    if (color_index < 0) {
	color = 0;
	color_op = PIX_SET;
	text_color_op = PIX_SRC;
    } else {
	color = PIX_COLOR(color_index);
	color_op = PIX_SRC | color;
	text_color_op = color_op;
    }

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	switch (image->im_type) {
      case IM_STRING:
	/* baseline offset */
	top = rect->r_top +
	    panel_fonthome(image_font(image), image_string(image)) +
	    (rect->r_height - image_font(image)->pf_defaultsize.y) / 2;
	panel_paint_text(pw, panel->gc, color_index, image_font(image),
			 rect->r_left, top, image_string(image));
	break;

      case IM_PIXRECT:
	(void) xv_rop(pw, rect->r_left, rect->r_top,
		      image_pixrect(image)->pr_width,
		      image_pixrect(image)->pr_height,
		      color_op, image_pixrect(image), 0, 0);
	break;
    }

    if (image_boxed(image)) {
	xv_draw_rectangle(pw, rect->r_left, rect->r_top,
			  rect->r_width - 1, rect->r_height - 1,
			  LineSolid, color_op);
    }
    if (image_inverted(image)) {
	xv_rop(pw, rect->r_left, rect->r_top, rect->r_width, rect->r_height,
	       PIX_NOT(PIX_DST) | color, NULL, 0, 0);
    }
    if (grey_out) {
	xv_replrop(pw, rect->r_left, rect->r_top,
		   rect->r_width, rect->r_height, PIX_SRC & PIX_DST | color,
		   &panel_gray_pr, 0, 0);
    }
    PANEL_END_EACH_PAINT_WINDOW
}


/****************************************************************************/
/* panel_paint_pixrect                                                      */
/****************************************************************************/
Pkg_private
panel_paint_pixrect(panel, pr, rect, grey_out, color_index)
    Panel_info     *panel;
    Pixrect        *pr;
    Rect           *rect;
    int             grey_out;
    int             color_index;
{
    Xv_Window       pw;
    int		    color;
    int		    color_op;

    if (color_index < 0) {
	color = 0;
	color_op = PIX_SET;
    } else {
	color = PIX_COLOR(color_index);
	color_op = PIX_SRC | color;
    }
    
    if (!pr->pr_width && !pr->pr_height)
	return;

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	(void) xv_rop(pw, rect->r_left, rect->r_top,
		      pr->pr_width, pr->pr_height,
		      color_op, pr, 0, 0);

    /* laf */
    if (grey_out) {
	xv_replrop(pw, rect->r_left, rect->r_top,
		   rect->r_width, rect->r_height, PIX_SRC & PIX_DST | color,
		   &panel_gray_pr, 0, 0);
    }
    PANEL_END_EACH_PAINT_WINDOW
}



/****************************************************************************/
/* panel_invert                                                             */
/* inverts the rect r using panel's pixwin.                                 */
/****************************************************************************/
Pkg_private
panel_invert(panel, r, color_index)
    Panel_info     *panel;
    register Rect  *r;
    int		    color_index;
{
    Xv_Window       pw;
    int		    color;

    color = color_index < 0 ? 0 : PIX_COLOR(color_index);
    PANEL_EACH_PAINT_WINDOW(panel, pw)
	(void) pw_writebackground(pw, r->r_left, r->r_top,
				  r->r_width, r->r_height,
				  PIX_NOT(PIX_DST) | color);
    PANEL_END_EACH_PAINT_WINDOW
}

Pkg_private
panel_clear(panel, r)
    Panel_info     *panel;
    register Rect  *r;
{
    Xv_Window       pw;

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	(void) pw_writebackground(pw, r->r_left, r->r_top,
				  r->r_width, r->r_height, PIX_CLR);
    PANEL_END_EACH_PAINT_WINDOW
}

/*****************************************************************************/
/* panel_strsave                                                             */
/*****************************************************************************/

Pkg_private char *
panel_strsave(source)
    char           *source;
{
    char           *dest;
    extern char    *strcpy();

    dest = (char *) malloc((u_int) (strlen(source) + 1));
    if (!dest)
	return NULL;

    (void) strcpy(dest, source);
    return dest;
}

/*****************************************************************************/
/* miscellaneous utilities                                                   */
/*****************************************************************************/

/*
 * Return max baseline offset for specified string.  This should be the same
 * value returned by XTextExtents in overall_return.descent.
 */
panel_fonthome(font, str)
    Pixfont        *font;
    char           *str;
{
    register int    max_home = 0;

    for (; *str; str++) {
	if (font->pf_char[*str].pc_home.y < max_home)
	    max_home = font->pf_char[*str].pc_home.y;
    }
    return -(max_home);
}

panel_nullproc()
{
    return 0;
}

Pkg_private void
panel_caret_on(panel, on)
    Panel_info     *panel;
    int             on;
{
    (*panel_caret_on_proc) (panel, on);
}

Pkg_private void
panel_caret_invert(panel)
    Panel_info     *panel;
{
    (*panel_caret_invert_proc) (panel);
}


Pkg_private void
panel_seln_hilite(selection)
    Panel_selection *selection;
{
    (*panel_seln_hilite_proc) (selection);
}


Pkg_private void
panel_seln_inform(panel, event)
    Panel_info     *panel;
    Event          *event;
{
    (*panel_seln_inform_proc) (panel, event);
}


Pkg_private void
panel_seln_destroy(panel)
    Panel_info     *panel;
{
    (*panel_seln_destroy_proc) (panel);
}


Pkg_private
panel_free_choices(choices, first, last)
    Panel_image    *choices;
    int             first, last;
{
    register int    i;		/* counter */

    if (!choices || last < 0)
	return;

    /* free the choice strings */
    for (i = first; i <= last; i++) {
	panel_free_image(&choices[i]);
    }

    free((char *) choices);
}				/* panel_free_choices */

Pkg_private void
panel_free_image(image)
    Panel_image    *image;
{
    if (is_string(image)) {
	free(image_string(image));
	if (image_font(image))
	    (void) xv_set(image_font(image), XV_DECREMENT_REF_COUNT, 0);
    }
}

#ifndef LAF_COMPLETE
Pkg_private
window_font_size_for_client(panel)
    Panel           panel;
{

    return 14;
}

#endif				/* LAF_COMPLETE */


Pkg_private void
panel_set_bold_label_font(ip)
    register Item_info *ip;
{
    Pixfont        *std_font = image_font(&ip->label);
    Pixfont        *bold_font;

    bold_font = (Pixfont *) xv_find(PANEL_PUBLIC(ip->panel), FONT,
				 FONT_FAMILY, xv_get(std_font, FONT_FAMILY),
				    FONT_STYLE, FONT_STYLE_BOLD,
				    FONT_SIZE, xv_get(std_font, FONT_SIZE),
				    0);
    if (bold_font == NULL) {
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "Unable to find bold font; using standard font",
		 XV_ERROR_PKG_NAME, "Panel",
		 0);
	bold_font = std_font;
    }
    image_set_font(&ip->label, bold_font);
}


Pkg_private void
panel_paint_text(pw, gc, color_index, font, x, y, str)
    Xv_opaque	pw;	/* paint window */
    GC		gc;	/* Graphics Context */
    int		color_index;
    Xv_opaque	font;
    int		x, y;	/* baseline starting position */
    char	*str;	/* text to paint */
{
    Display	*display;
    XID		font_xid;
    Xv_Drawable_info *info;
    Drawable	xid;

    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    xid = xv_xid(info);

    if (!gc)
	gc = xv_find_proper_gc(display, pw, info, PW_TEXT);

    if (color_index < 0) {
	xv_set_gc_op(display, info, gc, PIX_SRC,
		     XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
    } else {
	xv_set_gc_op(display, info, gc, PIX_SRC | PIX_COLOR(color_index),
		     XV_USE_OP_FG, XV_DEFAULT_FG_BG);
    }
    font_xid = (XID) xv_get(font, XV_XID);
    XSetFont(display, gc, font_xid);

    XDrawImageString(display, xid, gc, x, y, str, strlen(str));
}
