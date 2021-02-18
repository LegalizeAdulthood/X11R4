#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_slider.c 20.32 89/11/14 Copyr 1984 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/pixwin.h>
#include <xview_private/draw_impl.h>
#include <X11/Xlib.h>

extern char    *sprintf();

#define SLIDER_PRIVATE(item)	\
	XV_PRIVATE(Slider_info, Xv_panel_slider, item)
#define SLIDER_PUBLIC(item)	XV_PUBLIC(item)

#define	SLIDER_FROM_ITEM(ip)	SLIDER_PRIVATE(ITEM_PUBLIC(ip))

#define SLIDER_BOX_WIDTH(font_width)	(font_width)

#define SLIDER_BOX_HEIGHT(font_height)	(font_height)

#define SAVE_ITEM_XY	0
#define RESTORE_ITEM_XY	1

Pkg_private int slider_init();
Pkg_private Xv_opaque slider_set_avlist();
Pkg_private Xv_opaque slider_get_attr();
Pkg_private int slider_destroy();

static int      round();

static Panel_setting     get_value();

static void     change_item_order();
static void     do_item_xy();
static void     draw_slider_box();
static void     paint_slider();
static void     update_rects();

static void     begin_preview(), update_preview(), cancel_preview(),
                accept_preview(), paint(), layout();

static Panel_ops ops = {
    panel_default_handle_event,	/* handle_event() */
    begin_preview,		/* begin_preview() */
    update_preview,		/* update_preview() */
    cancel_preview,		/* cancel_preview() */
    accept_preview,		/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    paint,			/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    layout			/* layout() */
};

#define	BAR_WIDTH	5
#define	LABEL_XOFFSET	5	/* offset to value rect after label */

typedef struct Slider_info {	/* data for a slider */
    Panel_item      public_self;/* back pointer to object */
    Panel_item      value_textitem;	/* for displaying value as a text
					 * item */
    int             actual;
    int             apparent;
    u_int           flags;
    int             width;
    int             min_value;
    int             max_value;
    int             valuesize;
    Pixfont        *font;
    Rect            sliderrect;	/* rect containing slider */
    Rect            valuerect;	/* rect containing value of slider */
    Rect            leftrect;	/* rect containing info to the left */
    Rect            rightrect;	/* rect containing info to the right */
    /* rect containing the position of last slider box */
    Rect            sliderbox;
    /*
     * When a user drags the slider box, the value should actually be just to
     * the left of the slider box. Value_offset is the distance between
     * event_x(event) and where the real value should be
     */
    int             value_offset;
    int             print_value;/* value from PANEL_VALUE */
    int             use_print_value:1;
    int             restore_print_value:1;
}               Slider_info;


/* flags */
#define	OS_SHOWRANGE	1	/* show high and low scale numbers */
#define	OS_SHOWVALUE	2	/* show current value */
#define	OS_CONTINUOUS	4	/* notify on any change */


Pkg_private int
slider_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_slider *item_object = (Xv_panel_slider *) item_public;
    register Slider_info *dp;

    dp = xv_alloc(Slider_info);

    /* link to object */
    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static slider ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_SLIDER_ITEM;
    panel_set_bold_label_font(ip);

    dp->actual = 0;
    dp->apparent = 0;
    dp->flags = OS_SHOWRANGE | OS_SHOWVALUE;
    dp->width = 100;
    dp->min_value = 0;
    dp->max_value = 100;
    dp->valuesize = 3;		/* three characters wide */
    dp->font = item_get_panel_font(ip);
    if (dp->font) {
	(void) xv_set(dp->font, XV_INCREMENT_REF_COUNT, 0);
    }
    /* left rect starts at value_rect position */
    dp->leftrect = ip->value_rect;

    /* update the various rects */
    update_rects(ip);

    /*
     * BUG: since set is not called yet, append will not have the right size
     * for the item.
     */
    (void) panel_append(ip);

    return XV_OK;
}


Pkg_private     Xv_opaque
slider_set_avlist(item_public, avlist)
    Panel_item      item_public;
    register Attr_avlist avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    register Slider_info *dp = SLIDER_PRIVATE(item_public);
    register Panel_attr attr;
    int             value;
    short           value_set = FALSE;
    int             min_value = dp->min_value;
    int             max_value = dp->max_value;
    int             char_width;	/* laf */
    int             width = dp->width;
    short           range_set = FALSE;
    short           size_changed = FALSE;
    short           show_item = -1;

    /* if a client has called panel_item_parent this item may not */
    /* have a parent -- do nothing in this case */
    if (ip->panel == NULL) {
	return ((Xv_opaque) XV_ERROR);
    }
    while (attr = (Panel_attribute) * avlist++) {
	switch (attr) {
	  case PANEL_ITEM_COLOR:
	    if (dp->value_textitem)
		xv_set(dp->value_textitem,
		       PANEL_ITEM_COLOR, (char *) *avlist++,
		       0);
	    else
		(char *) avlist++;
	    break;

	  case PANEL_VALUE_FONT:
	    if (dp->font) {
		xv_set(dp->font, XV_DECREMENT_REF_COUNT, 0);
	    }
	    dp->font = (Pixfont *) * avlist++;
	    if (dp->font) {
		(void) xv_set(dp->font, XV_INCREMENT_REF_COUNT, 0);
		if (dp->value_textitem) {
		    xv_set(dp->value_textitem, PANEL_VALUE_FONT, dp->font, 0);
		}
	    }
	    size_changed = TRUE;
	    break;

	  case PANEL_NOTIFY_LEVEL:
	    if ((Panel_setting) * avlist++ == PANEL_ALL)
		dp->flags |= OS_CONTINUOUS;
	    else
		dp->flags &= ~OS_CONTINUOUS;
	    break;

	  case PANEL_VALUE:
	    value = (int) *avlist++;
	    value_set = TRUE;
	    dp->use_print_value = FALSE;
	    break;

	  case PANEL_MIN_VALUE:
	    min_value = (int) *avlist++;
	    range_set = TRUE;
	    break;

	  case PANEL_MAX_VALUE:
	    max_value = (int) *avlist++;
	    range_set = TRUE;
	    break;

	  case PANEL_SLIDER_WIDTH:
	    width = (int) *avlist++;
	    range_set = TRUE;
	    break;

	    /* laf begin */
	  case PANEL_VALUE_DISPLAY_LENGTH:
	    char_width = (int) *avlist++;
	    width = panel_col_to_x(dp->font, char_width);
	    range_set = TRUE;
	    break;
	    /* laf end */

	  case PANEL_SHOW_ITEM:
	    show_item = (short) *avlist++;
	    break;

	  case PANEL_SHOW_VALUE:
	    if (*avlist++) {
		if (!dp->value_textitem) {
		    do_item_xy(ip->panel, SAVE_ITEM_XY);
		    dp->value_textitem =
			xv_create(PANEL_PUBLIC(ip->panel), PANEL_TEXT,
				  PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
				  PANEL_NOTIFY_STRING, "\r\n",
				  PANEL_NOTIFY_PROC, get_value,
			          PANEL_ITEM_X, rect_right(&dp->rightrect) + 1,
				  PANEL_ITEM_Y, dp->leftrect.r_top,
				  PANEL_VALUE_DISPLAY_LENGTH, dp->valuesize,
				  PANEL_ITEM_COLOR, ip->color_index,
				  PANEL_CLIENT_DATA, ip,
				  0);
		    change_item_order(ip->panel, ip, ITEM_PRIVATE(dp->value_textitem));
		    do_item_xy(ip->panel, RESTORE_ITEM_XY);
		}
		dp->flags |= OS_SHOWVALUE;
	    } else
		dp->flags &= ~OS_SHOWVALUE;
	    size_changed = TRUE;
	    break;

	  case PANEL_SHOW_RANGE:
	    if ((int) *avlist++)
		dp->flags |= OS_SHOWRANGE;
	    else
		dp->flags &= ~OS_SHOWRANGE;
	    size_changed = TRUE;
	    break;

	  case XV_END_CREATE:
	    if (dp->flags & OS_SHOWVALUE) {
		if (dp->value_textitem) {
		    xv_set(dp->value_textitem,
			   PANEL_ITEM_COLOR, ip->color_index,
			   0);
		} else {
		    do_item_xy(ip->panel, SAVE_ITEM_XY);
		    dp->value_textitem =
			xv_create(PANEL_PUBLIC(ip->panel), PANEL_TEXT,
				  PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
				  PANEL_NOTIFY_STRING, "\r\n",
				  PANEL_NOTIFY_PROC, get_value,
				  PANEL_ITEM_X, rect_right(&dp->rightrect) + 1,
				  PANEL_ITEM_Y, dp->rightrect.r_top,
				  PANEL_VALUE_DISPLAY_LENGTH, dp->valuesize,
				  PANEL_ITEM_COLOR, ip->color_index,
				  PANEL_CLIENT_DATA, ip,
				  0);
		    change_item_order(ip->panel, ip,
				      ITEM_PRIVATE(dp->value_textitem));
		    do_item_xy(ip->panel, RESTORE_ITEM_XY);
		    size_changed = TRUE;
		}
	    }
	  default:
	    /* skip past what we don't care about */
	    avlist = attr_skip(attr, avlist);
	    break;
	}
    }

    if (dp->value_textitem) {
	if (show_item == -1)
	    show_item = hidden(ip) ? FALSE : TRUE;
	xv_set(dp->value_textitem,
	       PANEL_SHOW_ITEM, show_item,
	       0);
    }
    if (range_set) {
	char            buf[16];/* string version of max. value */
	char            buf2[16];

	/* get the current value */
	if (!value_set) {
	    value = itoe(dp, dp->actual);
	    value_set = TRUE;
	}
	dp->min_value = min_value;
	/* don't let the max value be <= the min value */
	dp->max_value =
	    (max_value <= dp->min_value) ? dp->min_value + 1 : max_value;
	(void) sprintf(buf, "%d", dp->max_value);
	(void) sprintf(buf2, "%d", dp->min_value);
	dp->valuesize = (strlen(buf) > strlen(buf2)) ? strlen(buf) : strlen(buf2);
	if (dp->value_textitem)
	    xv_set(dp->value_textitem, PANEL_VALUE_DISPLAY_LENGTH, dp->valuesize, 0);
	dp->width = width;
	size_changed = TRUE;
	/* print_value is no longer valid */
	dp->use_print_value = FALSE;
    }
    /* set apparent & actual value */
    if (value_set) {
	char            buf[16];

	if (value < dp->min_value)
	    value = dp->min_value;
	else if (value > dp->max_value)
	    value = dp->max_value;
	dp->apparent = dp->actual = etoi(dp, value);
	(void) sprintf(buf, "%d", value);
	if (dp->value_textitem) {
	    xv_set(dp->value_textitem,
		PANEL_VALUE_DISPLAY_LENGTH, dp->valuesize,
		PANEL_VALUE, buf,
		0);
	}
	dp->print_value = value;
	dp->use_print_value = TRUE;
    }
    if (size_changed) {
	update_rects(ip);
    }
    return XV_OK;
}


Pkg_private     Xv_opaque
slider_get_attr(item_public, status, which_attr, valist)
    Panel_item      item_public;
    int            *status;
    Panel_attr      which_attr;
    va_list         valist;
{
    register Slider_info *dp = SLIDER_PRIVATE(item_public);

    switch (which_attr) {
      case PANEL_VALUE_FONT:
	return (Xv_opaque) dp->font;

      case PANEL_NOTIFY_LEVEL:
	return (Xv_opaque)
	    (dp->flags & OS_CONTINUOUS ? PANEL_ALL : PANEL_DONE);

      case PANEL_VALUE:
	return (Xv_opaque) itoe(dp, dp->actual);

      case PANEL_MIN_VALUE:
	return (Xv_opaque) dp->min_value;

      case PANEL_MAX_VALUE:
	return (Xv_opaque) dp->max_value;

      case PANEL_SLIDER_WIDTH:
	return (Xv_opaque) dp->width;

      case PANEL_SHOW_VALUE:
	return (Xv_opaque) (dp->flags & OS_SHOWVALUE);

      case PANEL_SHOW_RANGE:
	return (Xv_opaque) (dp->flags & OS_SHOWRANGE);

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}


Pkg_private int
slider_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    Slider_info    *dp = SLIDER_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    if (dp->font)
	(void) xv_set(dp->font, XV_DECREMENT_REF_COUNT, 0);

    if (dp->value_textitem) 
        xv_destroy(dp->value_textitem);

    free((char *) dp);

    return XV_OK;
}


static void
update_rects(ip)
    register Item_info *ip;
{
    register Slider_info *dp = SLIDER_FROM_ITEM(ip);
    int             width, height, top;

    /*
     * Adjust the top of the leftrect, so the slider box isn't drawn outside
     * of the bounding box
     */
    dp->leftrect.r_height = dp->font->pf_defaultsize.y;

    /* create the left range rect */
    if (dp->flags & OS_SHOWRANGE)
	width = dp->font->pf_defaultsize.x * (dp->valuesize + 1);	/* " " */
    else
	width = 0;
    dp->leftrect.r_width = width;

    /* account for box lines on left and right and extra slop for bar */
    /* the width of the slider box depends on the size of the font    */
    width = dp->width + 4 + SLIDER_BOX_WIDTH(dp->font->pf_defaultsize.x);
    height = dp->leftrect.r_height / 2;
    /*
     * the y of sliderrect should be centered on the y of the left rect
     */
    top = dp->leftrect.r_top + (dp->leftrect.r_height - height) / 2;
    rect_construct(&dp->sliderrect, rect_right(&dp->leftrect) + 1,
		   top, width, height);

    /* create the right range rect */
    rect_construct(&dp->rightrect, rect_right(&dp->sliderrect) + 1,
	   dp->leftrect.r_top, dp->leftrect.r_width, dp->leftrect.r_height);

    /* create the value rect, which is a text item */
    if (dp->value_textitem) {
	xv_set(dp->value_textitem,
	       PANEL_ITEM_X, rect_right(&dp->rightrect) + 1,
	       PANEL_ITEM_Y, dp->rightrect.r_top,
	       PANEL_VALUE_DISPLAY_LENGTH, dp->valuesize,
	       0);
	dp->valuerect = *((Rect *) xv_get(dp->value_textitem, PANEL_ITEM_RECT));
	ip->value_rect.r_width = rect_right(&dp->valuerect) + 1 - dp->leftrect.r_left;
    } else {
	/* here dp->valuerect has not been initialized yet */
	ip->value_rect.r_width = rect_right(&dp->rightrect) + 1 - dp->leftrect.r_left;
    }
    ip->value_rect.r_height = MAX(dp->leftrect.r_height, dp->valuerect.r_height);

    /* update the item's enclosing rect */
    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
}


static void
layout(ip, deltas)
    Item_info      *ip;
    Rect           *deltas;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);

    dp->valuerect.r_left += deltas->r_left;
    dp->valuerect.r_top += deltas->r_top;
    dp->sliderrect.r_left += deltas->r_left;
    dp->sliderrect.r_top += deltas->r_top;
    dp->leftrect.r_left += deltas->r_left;
    dp->leftrect.r_top += deltas->r_top;
    dp->rightrect.r_left += deltas->r_left;
    dp->rightrect.r_top += deltas->r_top;
    update_rects(ip);
}


static void
paint(ip)
    Item_info      *ip;
{
    Rect           *r;
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    char            buf[10];
    Xv_Window       pw;

    r = &(ip->label_rect);

    /* paint the label */
    (void) panel_paint_image(ip->panel, &ip->label, r, inactive(ip),
			     ip->color_index);

    /* paint the range */
    if (dp->flags & OS_SHOWRANGE) {
	(void) sprintf(buf, "%*d ", dp->valuesize, dp->min_value);
	r = &dp->leftrect;
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    panel_paint_text(pw, ip->panel->gc, ip->color_index, dp->font,
	    		     r->r_left,
			     r->r_top + panel_fonthome(dp->font, buf),
		             buf);
	PANEL_END_EACH_PAINT_WINDOW

	(void) sprintf(buf, " %d", dp->max_value);
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    r = &dp->rightrect;
	    panel_paint_text(pw, ip->panel->gc, ip->color_index, dp->font,
			     r->r_left,
		             r->r_top + panel_fonthome(dp->font, buf),
		             buf);
	PANEL_END_EACH_PAINT_WINDOW
    }

    /* paint the slider */
    paint_slider(ip, 1);

    /* paint the text item */
    if (dp->value_textitem) {
	Item_info      *tp = ITEM_PRIVATE(dp->value_textitem);

	(*tp->ops->paint) (tp);
    }
}


static void
paint_slider(ip, refresh)
    Item_info      *ip;
    int             refresh;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    short           box_height = SLIDER_BOX_HEIGHT(dp->font->pf_defaultsize.y);
    short           box_width = SLIDER_BOX_WIDTH(dp->font->pf_defaultsize.x);
    int		    color_op;
    Display	   *display;
    int             empty_width = 0;
    Xv_Drawable_info *info;
    Xv_Window       pw;
    Rect           *r = &dp->sliderrect;
    Rect	    old_sliderbox;
    int		    solid_width;
    int             value;
    int		    top_left;
    Drawable	    xid;

    color_op = ip->color_index < 0 ? PIX_SET :
	PIX_SRC | PIX_COLOR(ip->color_index);

    DRAWABLE_INFO_MACRO(ip->panel->paint_window->pw, info);
    display = xv_display(info);

    /* Update the sliderbox location.  Determine empty and solid widths. */
    old_sliderbox = dp->sliderbox;
    value = dp->apparent;
    if (value < 0)
	value = 0;
    else if (value > dp->width)
	value = dp->width;
    dp->sliderbox.r_left = r->r_left + value + 2;
    dp->sliderbox.r_top = dp->leftrect.r_top;
    dp->sliderbox.r_width = box_width;
    dp->sliderbox.r_height = box_height;
    top_left = r->r_left + value + 4 + box_width;
    if ((empty_width = r->r_width - 4 - box_width - value - 1) < 0)
	empty_width = 0;
    if ((solid_width = dp->sliderbox.r_left - 2 - r->r_left) < 0)
	solid_width = 0;

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	xid = xv_xid(info);

	xv_set_gc_op(display, info, ip->panel->gc, PIX_CLR, XV_USE_OP_FG,
		     XV_DEFAULT_FG_BG);

	/* Clear old slider box */
	draw_slider_box(display, xid, ip->panel->gc,
			old_sliderbox.r_left, old_sliderbox.r_top,
			old_sliderbox.r_width, old_sliderbox.r_height);

	/* Clear the slider rectangle */
	XFillRectangle(display, xid, ip->panel->gc, r->r_left, r->r_top,
		       r->r_width+1, r->r_height+1);

	xv_set_gc_op(display, info, ip->panel->gc, color_op, XV_USE_OP_FG,
		     XV_DEFAULT_FG_BG);

        /* Draw the solid part of the slider */
	if (solid_width)
	    XFillRectangle(display, xid, ip->panel->gc, r->r_left, r->r_top,
			   solid_width, r->r_height+1);

	/* Draw new slider box */
	draw_slider_box(display, xid, ip->panel->gc,
			r->r_left + value + 2, dp->leftrect.r_top,
			box_width, box_height);

	/* Draw the empty part of the slider */
	if (empty_width)
	    XDrawRectangle(display, xid, ip->panel->gc, top_left, r->r_top,
			   empty_width, r->r_height);
    PANEL_END_EACH_PAINT_WINDOW
}


static void
begin_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);

    /*
     * We only begin preview if the user action clicks on the slider, not
     * when he/she drags in the slider w/ a button down
     */
    if (event_action(event) != ACTION_SELECT) {
	return;
    }
    if (rect_includespoint(&dp->sliderbox, event_x(event), event_y(event))) {
	(ip->panel)->SliderActive = TRUE;
	/*
	 * BUG: this assumes horizontal slider only
	 */
	dp->value_offset = event_x(event) - dp->sliderbox.r_left + 1;
    }
    /* save status of print value */
    dp->restore_print_value = dp->use_print_value;

    /* update the preview */
    update_preview(ip, event);
}

static void
update_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    register int    new;
    Rect            r;
    char            buf[20];

    r = dp->sliderrect;
    rect_marginadjust(&r, -1);

    if (!(ip->panel)->SliderActive) {
	return;
    }
    new = event_x(event) - r.r_left - dp->value_offset;
    if (new == dp->apparent)
	return;			/* state and display both correct */

    dp->apparent = new;
    dp->use_print_value = FALSE;
    paint_slider(ip, 0);
    if (dp->flags & OS_SHOWVALUE) {
	(void) sprintf(buf, "%d", itoe(dp, dp->apparent));
	xv_set(dp->value_textitem, PANEL_VALUE, buf, 0);
    }
    /* ONLY NOTIFY IF EXTERNAL VALUE HAS CHANGED? */
    if (dp->flags & OS_CONTINUOUS)
	(*ip->notify) (ITEM_PUBLIC(ip), itoe(dp, dp->apparent), event);
}


static void
cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    char            buf[20];

    (ip->panel)->SliderActive = FALSE;

    if (dp->apparent != dp->actual) {
	dp->apparent = dp->actual;
	dp->use_print_value = dp->restore_print_value;
	if (dp->flags & OS_CONTINUOUS)
	    (*ip->notify) (ip, itoe(dp, dp->actual), event);
	paint_slider(ip, 0);
	if (dp->flags & OS_SHOWVALUE) {
	    sprintf(buf, "%d", itoe(dp, dp->apparent));
	    xv_set(dp->value_textitem, PANEL_VALUE, buf, 0);
	}
    }
}


static void
accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);

    (ip->panel)->SliderActive = FALSE;

    if (dp->apparent != dp->actual) {
	dp->actual = dp->apparent;
	/* print_value is no longer valid */
	dp->use_print_value = FALSE;
	(*ip->notify) (ITEM_PUBLIC(ip), itoe(dp, dp->actual), event);
    }
}


/*
 * Convert external value to internal value.
 */
static int
etoi(dp, value)
    Slider_info    *dp;
    int             value;
{
    if (value <= dp->min_value)
	return (0);

    if (value >= dp->max_value)
	return (dp->width);

    return (round(((value - dp->min_value) * dp->width),
		  (dp->max_value - dp->min_value + 1)));
}

/*
 * Convert internal value to external value.
 */
static int
itoe(dp, value)
    Slider_info    *dp;
    int             value;
{
    /* use the print value if valid */
    if (dp->use_print_value)
	return dp->print_value;

    if (value <= 0)
	return (dp->min_value);

    if (value >= dp->width - 1)
	return (dp->max_value);

    return (dp->min_value +
	    (value * (dp->max_value - dp->min_value + 1)) / dp->width);
}

static int
round(x, y)
    register int    x, y;
{
    register int    z, rem;
    register short  is_neg = FALSE;

    if (x < 0) {
	x = -x;
	if (y < 0)
	    y = -y;
	else
	    is_neg = TRUE;
    } else if (y < 0) {
	y = -y;
	is_neg = TRUE;
    }
    z = x / y;
    rem = x % y;
    /* round up if needed */
    if (2 * rem >= y)
	if (is_neg)
	    z--;
	else
	    z++;

    return (is_neg ? -z : z);
}

static void
draw_slider_box(display, xid, gc, left, top, width, height)
    Display	   *display;
    Drawable	    xid;
    GC		    gc;
    int             left, top, width, height;
{
    if (!width || !height)
	return;

    /* Draw the new slider box */
    XDrawRectangle(display, xid, gc, left, top, width-1, height-1);

    /* Draw the shadow */
    XDrawLine(display, xid, gc, left+width, top+1, left+width, top+height-1);
    XDrawLine(display, xid, gc, left+1, top+height, left+width-1, top+height);
}

static Panel_setting
get_value(item_public, event)
    Panel_item     *item_public;
    Event          *event;
{
    Item_info      *ip = (Item_info *) xv_get(item_public, PANEL_CLIENT_DATA);
    Slider_info    *dp = SLIDER_FROM_ITEM(ip);
    int             value;

    value = atoi((char *) xv_get(item_public, PANEL_VALUE));
    if (value < dp->min_value) {
	value = dp->min_value;
    } else if (value > dp->max_value) {
	value = dp->max_value;
    }
    dp->apparent = dp->actual = etoi(dp, value);
    dp->use_print_value = TRUE;
    dp->print_value = value;
    paint_slider(ip, 0);

    (*ip->notify) (ITEM_PUBLIC(ip), value, event);

    return panel_text_notify(item_public, event);
}

static void
do_item_xy(panel, op)
    Panel_info     *panel;
    unsigned        op;
{
    static int      item_x, item_y, max_item_y;

    if (op == SAVE_ITEM_XY) {
	item_x = panel->item_x;
	item_y = panel->item_y;
	max_item_y = panel->max_item_y;
    } else {
	panel->item_x = item_x;
	panel->item_y = item_y;
	panel->max_item_y = max_item_y;
    }
}


/*
 * ALERT: GROSS HACK!! This routine changes the order of the items.  "ip"
 * should be the slider item, and the routine puts the text item created by
 * slider item in front of the slider item.  Because the text item and the
 * slider item overlap, putting text item before slider item ensures that
 * proper events get delivered to the text item(such as ACTION_SELECT and key
 * events.)
 */
static void
change_item_order(panel, slider, text)
    Panel_info     *panel;
    Item_info      *slider, *text;
{
    Item_info      *i = panel->items;
    Item_info      *before_text = NULL, *before_slider = NULL;
    short           slider_found = FALSE, text_found = FALSE;

    while (i && !slider_found) {
	if (i == slider) {
	    slider_found = TRUE;
	    break;
	}
	before_slider = i;
	i = i->next;
    }
    while (i && !text_found) {
	if (i == text) {
	    text_found = TRUE;
	    break;
	}
	before_text = i;
	i = i->next;
    }
    if (slider_found == FALSE || text_found == FALSE || !before_text) {
	/*
	 * both slider and text have to be in the item list, otherwise, there
	 * is serious internal bookkeeping error.  Similarly text can't
	 * possibly be the first item in the list; at least the slider is
	 * before it
	 */
	xv_error(PANEL_PUBLIC(panel), XV_ERROR_NON_RECOVERABLE,
	XV_ERROR_INTERNAL, XV_ERROR_STRING, "Panel item list corrupted", 0);
    }
    if (!before_slider) {
	panel->items = text;
    } else {
	before_slider->next = text;
    }
    before_text->next = text->next;
    text->next = slider;
}
