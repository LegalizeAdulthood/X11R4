#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_btn.c 20.61 89/10/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/openmenu.h>
#include <xview/pixwin.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>

extern struct pr_size xv_pf_textwidth();

Pkg_private int panel_button_init();
Pkg_private Item_info *panel_finditem();


static short    button_arrow_image[] = {
#include <images/panel_button_arrow.pr>
};
static          mpr_static(panel_button_arrow_pr, 16, 14, 1, button_arrow_image);

static short    panel_pulldown_image[] = {
#include <images/panel_pulldown.pr>
};
static          mpr_static(panel_pulldown_pr, 16, 8, 1, panel_pulldown_image);

static short    panel_pullright_image[] = {
#include <images/panel_pullright.pr>
};
static          mpr_static(panel_pullright_pr, 16, 9, 1, panel_pullright_image);

static unsigned short pin_in_data[] = {
#include <images/pin_in.pr>
};
static          mpr_static(menu_pin_in_pr, 32, 16, 1, pin_in_data);


/*
 * 
 * ---------------------------------------------------------------------------
 * 
 */
/*
 * The following arrays describe the outlines and interiors of the buttons,
 * and their shadow lines.  The number suffix is the maximum font size that
 * can use the described outline. Note: button_interior<size> is also used as
 * the default button (interior) outline. For each pr_pos entry, a negative
 * number implies that the width or height of the rectangle is added to the x
 * or y coordinate, respectively. These arrays are also referenced by the
 * menu package.
 */
static struct pr_pos button_outline12[24] = {
    {5, 0}, {-7, 0}, {-6, 1}, {-5, 1}, {-3, 3}, {-3, 4}, {-2, 4}, {-2, -6},
    {-3, -6}, {-3, -5}, {-5, -3}, {-6, -3}, {-6, -2}, {4, -2}, {4, -3}, {3, -3},
    {1, -5}, {1, -6}, {0, -7}, {0, 5}, {1, 4}, {1, 3}, {3, 1}, {4, 1}
};

static struct pr_pos button_interior12[24] = {
    {6, 2}, {-8, 2}, {-7, 3}, {-6, 3}, {-5, 4}, {-5, 5}, {-4, 6}, {-4, -8},
    {-5, -7}, {-5, -6}, {-6, -5}, {-7, -5}, {-8, -4}, {6, -4}, {5, -5}, {4, -5},
    {3, -6}, {3, -7}, {2, -8}, {2, 6}, {3, 5}, {3, 4}, {4, 3}, {5, 3}
};

static struct pr_pos button_shadow12[10] = {
    {-1, 6}, {-1, -6}, {-2, -5}, {-2, -4}, {-3, -4}, {-3, -3}, {-4, -2}, {-5, -2},
    {-6, -1}, {6, -1}
};

static struct pr_pos button_outline14[24] = {
    {6, 0}, {-8, 0}, {-7, 1}, {-6, 1}, {-3, 4}, {-3, 5}, {-2, 5}, {-2, -7},
    {-3, -7}, {-3, -6}, {-6, -3}, {-7, -3}, {-8, -2}, {5, -2}, {5, -3}, {4, -3},
    {1, -6}, {1, -7}, {0, -8}, {0, 6}, {1, 5}, {1, 4}, {4, 1}, {5, 1}
};

static struct pr_pos button_interior14[28] = {
    {7, 2}, {-9, 2}, {-8, 3}, {-7, 3}, {-6, 4}, {-5, 5}, {-5, 6}, {-4, 7},
    {-4, -9}, {-5, -8}, {-5, -7}, {-6, -6}, {-7, -5}, {-8, -5}, {-9, -4}, {7, -4},
    {6, -5}, {5, -5}, {4, -6}, {3, -7}, {3, -8}, {2, -9}, {2, 7}, {3, 6},
    {3, 5}, {4, 4}, {5, 3}, {6, 3}
};

static struct pr_pos button_shadow14[8] = {
    {-1, 7}, {-1, -7}, {-2, -6}, {-2, -5}, {-5, -2}, {-6, -2}, {-7, -1}, {7, -1}
};

static struct pr_pos button_outline19[24] = {
    {7, 0}, {-10, 0}, {-9, 1}, {-8, 1}, {-4, 5}, {-4, 6}, {-3, 6}, {-3, -9},
    {-4, -9}, {-4, -8}, {-8, -4}, {-9, -4}, {-9, -3}, {6, -3}, {6, -4}, {5, -4},
    {1, -8}, {1, -9}, {0, -10}, {0, 7}, {1, 6}, {1, 5}, {5, 1}, {6, 1}
};

static struct pr_pos button_interior19[24] = {
    {8, 2}, {-11, 2}, {-10, 3}, {-9, 3}, {-6, 6}, {-6, 7}, {-5, 8}, {-5, -11},
    {-6, -10}, {-6, -9}, {-9, -6}, {-10, -6}, {-11, -5}, {8, -5}, {7, -6}, {6, -6},
    {3, -9}, {3, -10}, {2, -11}, {2, 8}, {3, 7}, {3, 6}, {6, 3}, {7, 3}
};

static struct pr_pos button_shadow19a[6] = {
    {-2, 8}, {-2, -9}, {-3, -9}, {-3, -8}, {-9, -2}, {7, -2}
};
static struct pr_pos button_shadow19b[5] = {
    {-1, 10}, {-1, -8}, {-2, -8}, {-9, -1}, {9, -1}
};
static struct pr_pos button_shadow19c[4] = {
    {-8, -1}, {-2, -7}, {-2, -6}, {-6, -2}
};


/*
 * 
 * ---------------------------------------------------------------------------
 * 
 */

static void     begin_preview(), cancel_preview(), accept_preview(),
                accept_menu(), paint();


static Panel_ops ops = {
    panel_default_handle_event,	/* handle_event() */
    begin_preview,		/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    cancel_preview,		/* cancel_preview() */
    accept_preview,		/* accept_preview() */
    accept_menu,		/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    paint,			/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc/* layout() */
};

typedef struct button_info {
    Panel_item      public_self;/* back pointer to object */
}               Button_info;

#define BUTTON_PRIVATE(item) \
	XV_PRIVATE(Button_info, Xv_panel_button, item)
#define BUTTON_FROM_ITEM(ip)	BUTTON_PRIVATE(ITEM_PUBLIC(ip))

#define PANEL_HANDLE	1	/* unique key for item's public panel handle */

Xv_private void panel_paint_button();

static void     button_menu_done_proc();
static void     panel_paint_button_image();
static void     compute_xy_offset();
static void     compute_label_offset();
static void     paint_outline();
static void     take_down_cmd_frame();

static int      paint_pr();
static int      paint_text();

static Menu     generate_menu();

Xv_private void xv_paint_lines();
Xv_private void xv_invert_polygon();
Xv_private void xv_gray_polygon();
Xv_private Pixrect panel_shade_pr;
Xv_private void panel_paint_busy_button();

/* ARGSUSED */
Pkg_private int
panel_button_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_button *item_object = (Xv_panel_button *) item_public;
    Button_info    *dp;

    dp = xv_alloc(Button_info);

    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static button ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_BUTTON_ITEM;

    /*
     * BUG: since set is not called yet, append will not have the right size
     * for the item.
     */
    (void) panel_append(ip);

    return XV_OK;
}


static void
paint(ip)
    Item_info      *ip;
{
    panel_paint_button_image(ip, &ip->label, &ip->label_rect,
			     (int) inactive(ip), FALSE, ip->color_index, ip->menu,
			     ip->label_width);
}


/* ARGSUSED */
static void
begin_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Panel_image     image;
    Menu_class      menu_class;
    Menu            default_menu;
    Menu            submenu;
    Menu_item       default_menu_item;
    Rect            choice_rect;
    Xv_Window       pw;
    struct pr_size  size;
    int             selected = FALSE;	/* assume MENU_TOGGLE class */
    int             pin_is_default;	/* boolean */
    int             right_margin;

    if (ip->menu) {

	/* Get the pixrect or string of the default item */
	submenu = ip->menu;
	do {
	    if ((default_menu = generate_menu(submenu)) == NULL)
		return;
	    default_menu_item = (Menu_item) xv_get(default_menu, MENU_DEFAULT_ITEM);
	} while (submenu = (Menu) xv_get(default_menu_item, MENU_PULLRIGHT));
	pin_is_default = xv_get(default_menu, MENU_PIN) &&
	    xv_get(default_menu_item, MENU_TITLE);
	if (pin_is_default ||
	    !(image_string(&image) = (char *) xv_get(default_menu_item, MENU_STRING))) {
	    image.im_type = IM_PIXRECT;
	    image_pixrect(&image) = pin_is_default ? &menu_pin_in_pr :
		(Pixrect *) xv_get(default_menu_item, MENU_IMAGE);
	    size.x = 0;
	    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
		pw_writebackground(pw, ip->label_rect.r_left, ip->label_rect.r_top,
		  (int) ip->label_rect.r_width, (int) ip->label_rect.r_height, PIX_CLR);
	    PANEL_END_EACH_PAINT_WINDOW
	} else {
	    image.im_type = IM_STRING;
	    image_font(&image) = image_font(&ip->label);
	    image_bold(&image) = image_bold(&ip->label);
	    size = xv_pf_textwidth(strlen(image_string(&ip->label)),
			  image_font(&ip->label), image_string(&ip->label));
	    size.x += panel_pullright_pr.pr_width;
	    if (ip->label_width > size.x)
		size.x = ip->label_width;
	}
	/*
	 * Replace the button stack with the default item pixrect or string.
	 * If it's a string, truncate the string to the width of the original
	 * button stack string.
	 */
	menu_class = (Menu_class) xv_get(default_menu, MENU_CLASS);
	switch (menu_class) {
	  case MENU_COMMAND:
	    /* Paint the default string or image, overwriting the menu mark */
	    panel_paint_button_image(ip, &image, &ip->label_rect,
			      FALSE, FALSE, ip->color_index, FALSE, size.x);
	    /* Invert the button contents */
	    panel_paint_button_image(ip, &image, &ip->label_rect,
				FALSE, TRUE, ip->color_index,(Xv_opaque) TRUE, size.x);
	    break;

	  case MENU_CHOICE:
	  case MENU_TOGGLE:
	    if (menu_class == MENU_CHOICE)
		selected = TRUE;
	    else
		selected = xv_get(default_menu_item, MENU_SELECTED) ? FALSE : TRUE;
	    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
		pw_writebackground(pw, ip->label_rect.r_left, ip->label_rect.r_top,
		  ip->label_rect.r_width, ip->label_rect.r_height, PIX_CLR);
	    choice_rect.r_left = ip->label_rect.r_left + 1;
	    choice_rect.r_top = ip->label_rect.r_top + 1;
	    choice_rect.r_width = ip->label_rect.r_width - 3;
	    choice_rect.r_height = ip->label_rect.r_height - 3;
	    panel_paint_choice_outline(pw, &choice_rect, FALSE, selected,
				       ip->color_index);
	    (void) paint_text(pw, ip->panel->gc, image_font(&image),
			image_string(&image), &ip->label_rect, size.x,
			ip->color_index, &right_margin);
	    PANEL_END_EACH_PAINT_WINDOW
	}
    } else
	/* Invert the button interior */
	panel_paint_button_image(ip, &ip->label, &ip->label_rect,
			  (int) inactive(ip), TRUE, ip->color_index, ip->menu, 0);
}

/* ARGSUSED */
static void
cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    /*
     * If button and event is SELECT-down, then undo the inversion. If menu
     * button, then repaint original menu button.
     */
    if (ip->menu)
	panel_paint_button_image(ip, &ip->label, &ip->label_rect,
			     (int) inactive(ip), FALSE, ip->color_index, ip->menu,
			     ip->label_width);
    else if (action_select_is_down(event))
	panel_paint_button_image(ip, &ip->label, &ip->label_rect,
			      (int) inactive(ip), TRUE, ip->color_index, ip->menu,
			      ip->label_width);
}


static void
accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Menu            default_menu, submenu, topmenu;
    Menu_item       default_menu_item;
    void            (*pin_proc) ();	/* pin procedure for default menu */
    int             menu_depth = 0;
    int             pin_is_default;	/* boolean */
    int             notify_status;	/* XV_OK or XV_ERROR */
    Xv_Window       pw;

    /*
     * If button, then undo the inversion. If button stack, then repaint
     * original button stack.
     */
    panel_paint_button_image(ip, &ip->label, &ip->label_rect,
			 (int) inactive(ip), !ip->menu, ip->color_index, ip->menu,
			 ip->label_width);

    /* mask in the 'working' feedback */
    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	panel_paint_busy_button(pw, ip->label.im_value.t.font,
				&ip->label_rect);
    PANEL_END_EACH_PAINT_WINDOW

    /* notify the client */
	ip->notify_status = XV_OK;
    (*ip->notify) (ITEM_PUBLIC(ip), event);

    if ((ip->menu) && (submenu = topmenu = generate_menu(ip->menu))) {
	/* Select the default entry */
	menu_select_default(submenu);
	do {
	    menu_depth++;
	    default_menu = submenu;
	    default_menu_item = (Menu_item) xv_get(default_menu, MENU_DEFAULT_ITEM);
	} while (submenu = (Menu) xv_get(default_menu_item, MENU_PULLRIGHT));
	pin_is_default = xv_get(default_menu, MENU_PIN) &&
	    xv_get(default_menu_item, MENU_TITLE);
	if (pin_is_default) {
	    pin_proc = (void (*) ()) xv_get(default_menu, MENU_PIN_PROC);
	    pin_proc(default_menu, event_x(event), event_y(event));
	    notify_status = XV_OK;
	} else
	    /*
	     * invoke the appropriate notify procedure(s) for the menu
	     * selection
	     */
	    menu_return_default(topmenu, menu_depth);
	notify_status = (int) xv_get(topmenu, MENU_NOTIFY_STATUS);
    } else
	notify_status = ip->notify_status;

    if (!hidden(ip))
	panel_paint_button_image(ip, &ip->label, &ip->label_rect,
				 (int) inactive(ip), FALSE, ip->color_index, ip->menu,
				 ip->label_width);

    if (notify_status == XV_OK)
	take_down_cmd_frame(PANEL_PUBLIC(ip->panel));
}


static void
accept_menu(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Xv_Window       paint_window = event_window(event);

    if (ip->menu == NULL || paint_window == NULL)
	return;

    /*
     * Notify the client.  This callback allows the client to dynamically
     * generate the menu.
     */
    (*ip->notify) (ITEM_PUBLIC(ip), event);

    /*
     * Save public panel handle and current menu done proc. Switch to
     * button's menu done proc.
     */
    xv_set(ip->menu,
	   XV_KEY_DATA, PANEL_HANDLE, PANEL_PUBLIC(ip->panel),
	   XV_KEY_DATA, MENU_DONE_PROC, xv_get(ip->menu, MENU_DONE_PROC),
	   MENU_DONE_PROC, button_menu_done_proc,
	   0);

    /* Show menu */
    menu_show(ip->menu, paint_window, event,
#ifdef MENU_PREVIEWING
	      MENU_ENABLE_RECT, &ip->label_rect,
#else
	      MENU_POSITION_RECT, &ip->label_rect,
#endif				/* MENU_PREVIEWING */
	      MENU_PULLDOWN, ip->panel->layout == PANEL_HORIZONTAL,
	      0);
}


static void
button_menu_done_proc(menu, result)
    Menu            menu;
    Xv_opaque       result;
{
    void            (*orig_done_proc) ();	/* original menu-done
						 * procedure */
    Panel           panel_public = xv_get(menu, XV_KEY_DATA, PANEL_HANDLE);
    Panel_info     *panel = PANEL_PRIVATE(panel_public);

    if (xv_get(menu, MENU_NOTIFY_STATUS) == XV_OK)
	take_down_cmd_frame(panel_public);

    panel->current = NULL;

    /* Restore original menu done proc. */
    orig_done_proc = (void (*) ()) xv_get(menu, XV_KEY_DATA, MENU_DONE_PROC);
    xv_set(menu, MENU_DONE_PROC, orig_done_proc, 0);

    /* Invoke original menu done proc (if any) */
    if (orig_done_proc)
	(orig_done_proc) (menu, result);
}


static void
panel_paint_button_image(ip, image, rect, grey_out, invert, color_index,
			 menu, width)
    Item_info      *ip;
    Panel_image    *image;
    Rect           *rect;
    int             grey_out;
    int             invert;
    int             color_index;
    Xv_opaque       menu;	/* nil => no menu attached to button */
    int             width;	/* 0 => no truncation necessary; other =
				 * width of string/pixrect */
{
    Panel_info     *panel = ip->panel;
    int             w = rect->r_width;
    int             h = rect->r_height;
    int             default_button;
    
    Xv_Window       pw;
    char           *string;
    Pixrect        *pixrect;
    Pixrect        *menu_mark_pr;

    if (image_type(image) == IM_STRING) {
	string = image_string(image);
	pixrect = 0;
    } else {
	string = 0;
	pixrect = image_pixrect(image);
    }
    if (menu) {
	if (panel->layout == PANEL_VERTICAL)
	    menu_mark_pr = &panel_pullright_pr;
	else
	    menu_mark_pr = &panel_pulldown_pr;
    } else
	menu_mark_pr = 0;
    default_button = ITEM_PUBLIC(ip) == panel->default_item;

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	panel_paint_button(pw, panel->gc, rect, image_font(image), string,
	    pixrect, color_index, invert, menu_mark_pr, default_button, width,
	    (int) is_menu_item(ip));

    if (grey_out) {
	xv_replrop(pw, rect->r_left, rect->r_top, w, h,
		   PIX_SRC & PIX_DST |
		       (color_index < 0 ? 0 : PIX_COLOR(color_index)),
		   &panel_gray_pr, 0, 0);
    }
    PANEL_END_EACH_PAINT_WINDOW
}

Xv_private void
panel_paint_button(pw, gc, rect, font, string, pr, color_index, invert,
		   menu_mark_pr, default_button, width, menu_item)
    Xv_opaque       pw;
    GC		    gc;
    Rect           *rect;
    Pixfont        *font;
    char           *string;
    Pixrect        *pr;
    int    	    color_index;
    int             invert;
    Pixrect        *menu_mark_pr;	/* menu mark pixrect. nil => no menu
					 * attached to button */
    int             default_button;	/* boolean */
    int             width;	/* 0 => no truncation necessary; other =
				 * width of string/pixrect */
    int             menu_item;	/* boolean.  Button is a menu item */
{
    int             x = rect->r_left;
    int             y = rect->r_top;
    int             w = rect->r_width;
    int             h = rect->r_height;
    int             fontsize;
    int		    left;
    int             menu_mark_x;
    int             right_margin;
    register unsigned color_op;

    color_op = color_index < 0 ? PIX_SET : PIX_SRC | PIX_COLOR(color_index);

    if (string)
	fontsize = font->pf_defaultsize.y - 1;
    else if (pr)
	fontsize = pr->pr_size.y;
    else
	xv_error(NULL, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, "No string or pixrect defined for button",
		 XV_ERROR_PKG_NAME, "Panel",
		 0);
    if (invert) {
	/* Invert the inside of the button */
	w++;
	h++;
	if (fontsize <= 12) {
	    xv_invert_polygon(pw, 24, button_interior12, x, y, w, h,
		color_index);

	} else if ((fontsize > 12) && (fontsize < 19)) {
	    xv_invert_polygon(pw, 28, button_interior14, x, y, w, h,
		color_index);

	} else {
	    xv_invert_polygon(pw, 24, button_interior19, x, y, w, h,
		color_index);
	}
    } else {
	pw_writebackground(pw, x, y, w, h, PIX_CLR);
	if (!menu_item || default_button)
	    /* Panel button, or default menu item */
	    paint_outline(pw, rect, fontsize, color_index,
			  menu_item ? FALSE : default_button,	/* draw interior
								 * outline? */
			  !menu_item	/* draw shadow? */
		);
	  if (pr) {
	    left = paint_pr(pw, pr, rect, width, color_op, &right_margin);
	} else
	    left = paint_text(pw, gc, font, string, rect, width, color_index,
			      &right_margin);
	if (menu_mark_pr) {
	    /*
	     * Paint the down or right arrow indicating a pulldown or
	     * pullright menu, respectively.  Note: The arrow is painted even
	     * if it overwrites part of the button text.
	     */
	    menu_mark_x = rect->r_left + rect->r_width - right_margin -
		menu_mark_pr->pr_width;
	    xv_rop(pw, menu_mark_x,
	       rect->r_top + (rect->r_height - menu_mark_pr->pr_height) / 2,
		   menu_mark_pr->pr_width, menu_mark_pr->pr_height,
		   color_op, menu_mark_pr, 0, 0);
	}
    }

}


static int			/* returns the right margin + 1 of the
				 * painted area */
paint_pr(pw, pr, rect, width, color_op, right_margin)
    Xv_opaque       pw;
    Pixrect        *pr;
    Rect           *rect;
    int    	    width;	/* 0 => no truncation necessary; other =
				 * width of string/pixrect */
    unsigned int    color_op;
    int            *right_margin;	/* output parameter */
{
    int             x_offset, y_offset;
    int             x;
    int             y;
    int             bottom_margin;
    unsigned int    height;

    /* Compute top left corner to paint pixrect */
    compute_xy_offset(pr->pr_height, &x_offset, &y_offset,
		      right_margin, &bottom_margin);
    x = rect->r_left + x_offset;
    y = rect->r_top + y_offset;
    if (width == 0)
	width = rect->r_width - x_offset - *right_margin;
    width = MIN(width, pr->pr_width);
    height = MIN(rect->r_height - y_offset - bottom_margin, pr->pr_height);

    /* Paint pixrect */
    xv_rop(pw, x, y, width, height, color_op, pr, 0, 0);

    return (x + width);
}


/*
 * paint_text: returns right margin+1 of the painted area, or 0 if string was
 * truncated.
 */
static int
paint_text(pw, gc, font, string, rect, width, color_index, right_margin)
    Xv_opaque       pw;
    GC		    gc;
    Pixfont        *font;
    char           *string;
    Rect           *rect;
    int             width;	/* 0 => no truncation necessary; other = # of
				 * pixels available in horizontal direction */
    int    	    color_index;
    int            *right_margin;	/* output parameter */
{
    int             result;
    int             truncated = width;	/* boolean value (nonzero = TRUE) */
    int             x_offset, y_offset;
    int             bottom_margin;
    int             text_x;
    int             text_y;
    int             arrow_x;	/* x-coordinate of clip arrow */
    int             arrow_y;	/* y-coordinate of clip arrow */
    char           *scp = string;	/* source character pointer */
    char           *dcp;	/* destination character pointer */
    char           *local_string;
    struct pr_size  label_size;

    /*
     * BUG ALERT: The calloc's in this routine can be eliminated if we copy
     * the user's string to calloc'ed memory in image_set (attr XV_LABEL) in
     * the menu package.  Those calloc's would allocate 3 extra bytes if a
     * window mark was requested.
     */
    if (width)
	local_string = calloc(strlen(string) + 1, 1);
    else
	local_string = string;

    if (width) {
	dcp = local_string;
	arrow_x = 0;
	label_size.y = font->pf_defaultsize.y;
	do {
	    arrow_x += font->pf_char[*scp].pc_pr->pr_width;	/* add char width to
								 * subtotal */
	    label_size.y += font->pf_char[*scp].pc_adv.y;
	    *dcp++ = *scp++;	/* copy char to local_string */
	} while (arrow_x < width && *scp);
	if (truncated = *scp) {
	    width -= panel_button_arrow_pr.pr_width;
	    do {
		arrow_x -= font->pf_char[*--dcp].pc_pr->pr_width;
		*dcp = 0;
	    } while (arrow_x > width);
	}
    } else {
	label_size = xv_pf_textwidth(strlen(local_string), font, local_string);
	arrow_x = label_size.x;
    }

    /* Compute coordinates to print string */
    compute_xy_offset(label_size.y, &x_offset, &y_offset,
		      right_margin, &bottom_margin);
    text_x = rect->r_left + x_offset;
    text_y = rect->r_top + (rect->r_height - label_size.y) / 2
	+ panel_fonthome(font, local_string);

    /* Paint text */
    panel_paint_text(pw, gc, color_index, font, text_x, text_y, local_string);

    /* Paint right clip arrow if string was truncated */
    arrow_x += text_x + 1;
    if (truncated) {
	/* append arrow to string, centered vertically */
	arrow_y = rect->r_top + rect->r_height / 2 -
	    panel_button_arrow_pr.pr_height / 2;
	xv_rop(pw, arrow_x, arrow_y,
	    panel_button_arrow_pr.pr_width, panel_button_arrow_pr.pr_height,
	       color_index < 0 ? PIX_SET : PIX_SRC | PIX_COLOR(color_index),
	       &panel_button_arrow_pr, 0, 0);
	result = 0;
    } else {
	result = arrow_x;
    }
    if (local_string != string)
	free(local_string);

    return (result);
}


static void
compute_xy_offset(image_height, x_offset, y_offset, right_margin, bottom_margin)
    int             image_height;
    int            *x_offset, *y_offset;	/* output parameters */
    int            *right_margin, *bottom_margin;	/* output parameters */
{

    /*
     * Note: These numbers correspond to those in compute_item_size
     * (menu/walkmenu_render.c) and adjust_label_size (panel/item_set.c).
     */
    if (image_height <= 12) {
	*x_offset = 7;
	*right_margin = 8;
	*y_offset = 4;
	*bottom_margin = 7;

    } else if ((image_height > 12) && (image_height < 19)) {
	*x_offset = 8;
	*right_margin = 9;
	*y_offset = 5;
	*bottom_margin = 7;

    } else {
	*x_offset = 9;
	*right_margin = 11;
	*y_offset = 5;
	*bottom_margin = 10;
    }
}


/* laf */
static void
paint_outline(pw, rect, fontsize, color_index, interior_outline, shadow)
    register Xv_opaque pw;
    register Rect  *rect;
    int             fontsize;
    register int    color_index;
    int             interior_outline;	/* draw interior outline? */
    int             shadow;	/* draw button shadow? */
{
    register int    x = rect->r_left;
    register int    y = rect->r_top;
    register int    w = rect->r_width;
    register int    h = rect->r_height;
    register unsigned color_op;
    
    color_op = color_index < 0 ? PIX_SET : PIX_SRC | PIX_COLOR(color_index);

    if (fontsize <= 12) {
	xv_paint_lines(pw, color_op, 24, button_outline12,
		       x, y, w, h);	/* draw outline */
	if (interior_outline)
	    xv_paint_lines(pw, color_op, 24, button_interior12,
			   x, y, w, h);	/* draw interior outline */
	if (shadow)
	    xv_paint_lines(pw, color_op, 10, button_shadow12,
			   x, y, w, h);	/* draw shadow */

    } else if ((fontsize > 12) && (fontsize < 19)) {
	xv_paint_lines(pw, color_op, 24, button_outline14,
		       x, y, w, h);	/* draw outline */
	if (interior_outline)
	    xv_paint_lines(pw, color_op, 28, button_interior14,
			   x, y, w, h);	/* draw interior outline */
	if (shadow)
	    xv_paint_lines(pw, color_op, 8, button_shadow14,
			   x, y, w, h);	/* draw shadow */

    } else {
	xv_paint_lines(pw, color_op, 24, button_outline19,
		       x, y, w, h);	/* draw outline */
	if (interior_outline)
	    xv_paint_lines(pw, color_op, 24, button_interior19,
			   x, y, w, h);	/* draw interior outline */
	if (shadow) {
	    /* Draw shadow */
	    xv_paint_lines(pw, color_op, 6, button_shadow19a,
			   x, y, w, h);
	    xv_paint_lines(pw, color_op, 5, button_shadow19b,
			   x, y, w, h);
	    xv_paint_lines(pw, color_op, 4, button_shadow19c,
			   x, y, w, h);
	}
    }
}


/* laf */
Xv_private void
panel_paint_busy_button(pw, font, rect)
    register Xv_Window pw;
    Pixfont        *font;
    register Rect  *rect;
{
    int             x = rect->r_left;
    int             y = rect->r_top;
    int             w = rect->r_width;
    int             h = rect->r_height;
    int             fontsize = font->pf_defaultsize.y - 1;

    /* Shade the inside of the button */
    w++;
    h++;
    if (fontsize <= 12) {
	xv_gray_polygon(pw, 24, button_interior12, x, y, w, h, &panel_shade_pr);

    } else if ((fontsize > 12) && (fontsize < 19)) {
	xv_gray_polygon(pw, 28, button_interior14, x, y, w, h, &panel_shade_pr);

    } else {
	xv_gray_polygon(pw, 16, button_interior19, x, y, w, h, &panel_shade_pr);
    }
}


static void
take_down_cmd_frame(panel_public)
    Panel           panel_public;
{
    Frame           frame;

    /*
     * If this is a command frame, and the pin is out, then take down the
     * window.  Note: The frame code checks to see if the pin is out.
     */
    frame = xv_get(panel_public, WIN_FRAME);
    if (xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_CMD))
	xv_set(frame, XV_SHOW, FALSE, 0);
}


static          Menu
generate_menu(menu)
    Menu            menu;
{
    Menu            gen_menu, (*gen_proc) ();	/* generated menu and
						 * procedure */

    if (gen_proc = (Menu(*) ()) xv_get(menu, MENU_GEN_PROC)) {
	gen_menu = gen_proc(menu, MENU_DISPLAY);
	if (gen_menu == NULL) {
	    xv_error(menu, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
		 "begin_preview: menu's gen_proc failed to generate a menu",
		     XV_ERROR_PKG_NAME, "Panel",
		     0);
	    return (NULL);
	}
	return (gen_menu);
    } else
	return (menu);
}
