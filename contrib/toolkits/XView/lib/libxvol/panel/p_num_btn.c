#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_num_btn.c 20.4 89/08/18";
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

Pkg_private int panel_num_up_button_init();
Pkg_private int panel_num_down_button_init();

/*
 * up buttons (10, 12, 14, and 19)
 */
static short up_button_10_image[] = {
#include <images/up_button_10.pr>
};
static mpr_static(up_button_10_pr, 16, 13, 1, up_button_10_image);

static short up_button_12_image[] = {
#include <images/up_button_12.pr>
};
static mpr_static(up_button_12_pr, 16, 15, 1, up_button_12_image);

static short up_button_14_image[] = {
#include <images/up_button_14.pr>
};
static mpr_static(up_button_14_pr, 16, 17, 1, up_button_14_image);

static short up_button_19_image[] = {
#include <images/up_button_19.pr>
};
static mpr_static(up_button_19_pr, 32, 25, 1, up_button_19_image);


/*
 * down buttons (10, 12, 14, and 19)
 */
static short down_button_10_image[] = {
#include <images/down_button_10.pr>
};
static mpr_static(down_button_10_pr, 16, 13, 1, down_button_10_image);

static short down_button_12_image[] = {
#include <images/down_button_12.pr>
};
static mpr_static(down_button_12_pr, 16, 15, 1, down_button_12_image);

static short down_button_14_image[] = {
#include <images/down_button_14.pr>
};
static mpr_static(down_button_14_pr, 16, 17, 1, down_button_14_image);

static short down_button_19_image[] = {
#include <images/down_button_19.pr>
};
static mpr_static(down_button_19_pr, 32, 25, 1, down_button_19_image);

/*
 * ops
 */
static void begin_preview();
static void cancel_preview();
static void accept_preview();
static void paint();

static struct panel_ops ops = {
    (void (*) ()) panel_default_handle_event,	/* handle_event() */
    (void (*) ()) begin_preview,		/* begin_preview() */
    (void (*) ()) panel_nullproc,		/* update_preview() */
    (void (*) ()) cancel_preview,		/* cancel_preview() */
    (void (*) ()) accept_preview,		/* accept_preview() */
    (void (*) ()) panel_nullproc,		/* accept_menu() */
    (void (*) ()) panel_nullproc,		/* accept_key() */
    (void (*) ()) paint,			/* paint() */
    (void (*) ()) panel_nullproc,		/* remove() */
    (void (*) ()) panel_nullproc,		/* restore() */
    (void (*) ()) panel_nullproc		/* layout() */
};

typedef enum {
    NUM_UP, NUM_DOWN
}     Button_Kind;

typedef struct button_info {
    Panel_item public_self;	/* back pointer to object */
    Pixrect *pr;
    Button_Kind kind;
}     Button_info;

#define BUTTON_PRIVATE(item)\
    XV_PRIVATE(Button_info, Xv_panel_button, item)
#define BUTTON_FROM_ITEM(ip)    BUTTON_PRIVATE(ITEM_PUBLIC(ip))


static void
choose_button(ip, dp)
    Item_info *ip;
    Button_info *dp;
{
    Pixfont *font = item_get_panel_font(ip);
    int   fontsize = font->pf_defaultsize.y - 1;
    Button_Kind kind = dp->kind;

    if (fontsize <= 10) {
	dp->pr = (kind == NUM_UP ? &up_button_10_pr : &down_button_10_pr);
	ip->label_rect.r_width = 12;
	ip->label_rect.r_height = 13;
    } else if (fontsize >= 18) {
	dp->pr = (kind == NUM_UP ? &up_button_19_pr : &down_button_19_pr);
	ip->label_rect.r_width = 24;
	ip->label_rect.r_height = 25;
    } else
	switch (fontsize) {
	  case 11:
	  case 12:
	  case 13:
	    dp->pr = (kind == NUM_UP ? &up_button_12_pr : &down_button_12_pr);
	    ip->label_rect.r_width = 14;
	    ip->label_rect.r_height = 15;
	    break;

	  default:
	    dp->pr = (kind == NUM_UP ? &up_button_14_pr : &down_button_14_pr);
	    ip->label_rect.r_width = 16;
	    ip->label_rect.r_height = 17;
	}
}


static void
create_button(panel_public, item_public, kind)
    Panel panel_public;
    Panel_item item_public;
    Button_Kind kind;
{
    Panel_info *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_button *item_object = (Xv_panel_button *) item_public;
    Button_info *dp = xv_alloc(Button_info);

    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;
    dp->kind = kind;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static button ops vector */

    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ())panel->event_proc;

    ip->item_type = PANEL_BUTTON_ITEM;

    choose_button(ip, dp);

    (void) panel_append(ip);
}


Pkg_private int
panel_num_up_button_init(panel_public, item_public, avlist)
    Panel panel_public;
    Panel_item item_public;
    Attr_avlist avlist;
{
    create_button(panel_public, item_public, NUM_UP);

    return XV_OK;
}


Pkg_private int
panel_num_down_button_init(panel_public, item_public, avlist)
    Panel panel_public;
    Panel_item item_public;
    Attr_avlist avlist;
{
    create_button(panel_public, item_public, NUM_DOWN);

    return XV_OK;
}

static void
invert_inside(ip)
    Item_info *ip;
{
    Xv_Window pw;
    register Rect *rect = &(ip->label_rect);
    Button_info *dp = BUTTON_FROM_ITEM(ip);

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	xv_rop(pw,
	       rect->r_left + (dp->kind == NUM_UP ? 2 : 1),
	       rect->r_top + 2,
	       rect->r_width - 4, rect->r_height - 5,
	       ip->color_index < 0 ? PIX_NOT(PIX_DST) :
		   PIX_NOT(PIX_DST) | PIX_COLOR(ip->color_index),
	       (Pixrect *) 0, 0, 0);

    PANEL_END_EACH_PAINT_WINDOW
}

static void
paint(ip)
    Item_info *ip;
{
    Xv_Window pw;
    register Rect *rect = &(ip->label_rect);
    unsigned color = ip->color_index < 0 ? 0 : PIX_COLOR(ip->color_index);
    Button_info *dp = BUTTON_FROM_ITEM(ip);

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	xv_rop(pw,
	       rect->r_left, rect->r_top,
	       rect->r_width, rect->r_height,
	       PIX_SRC | color,
	       dp->pr, 0, 0);

    if (inactive(ip))
	/*
	 * don't grey out the line in the middle between the left and right
	 * buttons or it looks too confusing
	 */
	xv_replrop(pw,
		   rect->r_left + (dp->kind == NUM_UP ? 0 : 1),
		   rect->r_top,
		   rect->r_width - 1, rect->r_height,
		   PIX_SRC & PIX_DST | color,
		   &panel_gray_pr, 0, 0);

    PANEL_END_EACH_PAINT_WINDOW
}


static void
begin_preview(ip, event)
    Item_info *ip;
    Event *event;
{
    invert_inside(ip);
}


static void
cancel_preview(ip, event)
    Item_info *ip;
    Event *event;
{
    /*
     * If button and event is SELECT-down, then undo the inversion.
     */
    if (action_select_is_down(event))
	invert_inside(ip);
}


static void
accept_preview(ip, event)
    Item_info *ip;
    Event *event;
{
    /* undo the inversion */
    invert_inside(ip);

    /* don't mask in the 'working' feedback  it's too quick */

    /* notify the client */
    ip->notify_status = XV_OK;
    (*ip->notify) (ITEM_PUBLIC(ip), event);

    paint(ip);
}
