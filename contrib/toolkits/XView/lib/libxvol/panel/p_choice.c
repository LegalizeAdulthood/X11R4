#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_choice.c 20.65 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <pixrect/pr_line.h>
#include <xview/openmenu.h>
#include <xview/font.h>

#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>

#define CHOICE_PRIVATE(item)	\
	XV_PRIVATE(Choice_info, Xv_panel_choice, item)
#define CHOICE_PUBLIC(item)	XV_PUBLIC(item)

#define	CHOICE_FROM_ITEM(ip)	CHOICE_PRIVATE(ITEM_PUBLIC(ip))

#define DATA_HANDLE	1	/* unique key for private data handle */

#define VERTICAL_WHITE_SPACE 8;
/* as per OPEN LOOK Spec Appendix C */

Xv_private void panel_paint_choice_outline();

Pkg_private int choice_init();
Pkg_private Xv_opaque choice_set_avlist();
Pkg_private Xv_opaque choice_get_attr();
Pkg_private int choice_destroy();

extern struct pr_size xv_pf_textwidth();

/*****************************************************************************/
/* laf images                                                                */
/*****************************************************************************/

static short    choice50[4] = {	/* 50% gray pattern	 */
    0xAAAA, 0x5555, 0xAAAA, 0x5555
};

static          mpr_static(panel_choice_gray_pr, 16, 4, 1, choice50);

static short    panel_choice_off_12_image[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x1F00, 0x60C0, 0x4060, 0x8020,
    0x8030, 0x8030, 0x8030, 0x8030, 0x4070, 0x60E0, 0x3FE0, 0x0F80
};
static          mpr_static(panel_choice_off_12_pr, 16, 16, 1, panel_choice_off_12_image);

static short    panel_choice_on_12_image[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0F80, 0x3060, 0x2020,
    0x4710, 0x4F90, 0x4F90, 0x4F90, 0x4710, 0x2020, 0x3060, 0x0F80
};
static          mpr_static(panel_choice_on_12_pr, 16, 16, 1, panel_choice_on_12_image);

static short    panel_choice_off_14_image[] = {
    0x0000, 0x0000, 0x0F80, 0x3060, 0x4010, 0x4018, 0x8008, 0x800C,
    0x800C, 0x800C, 0x800C, 0x401C, 0x4018, 0x3078, 0x1FF0, 0x07C0
};
static          mpr_static(panel_choice_off_14_pr, 16, 16, 1, panel_choice_off_14_image);

static short    panel_choice_on_14_image[] = {
    0x0000, 0x0000, 0x0000, 0x07C0, 0x1830, 0x2008, 0x2388, 0x47C4,
    0x4FE4, 0x4FE4, 0x4FE4, 0x47C4, 0x2388, 0x2008, 0x1830, 0x07C0
};
static          mpr_static(panel_choice_on_14_pr, 16, 16, 1, panel_choice_on_14_image);

static short    panel_choice_off_20_image[] = {
    0x07C0, 0x1830, 0x2008, 0x4004, 0x4006, 0x8002, 0x8003, 0x8003,
    0x8003, 0x8003, 0x4007, 0x4006, 0x200E, 0x183C, 0x0FF8, 0x03E0
};
static          mpr_static(panel_choice_off_20_pr, 16, 16, 1, panel_choice_off_20_image);

static short    panel_choice_on_20_image[] = {
    0x0000, 0x03E0, 0x0C18, 0x1004, 0x21C2, 0x27F2, 0x47F1, 0x4FF9,
    0x4FF9, 0x4FF9, 0x47F1, 0x27F2, 0x21C2, 0x1004, 0x0C18, 0x03E0
};
static          mpr_static(panel_choice_on_20_pr, 16, 16, 1, panel_choice_on_20_image);

static short    panel_choice_off_24_image[] = {
    0x03F8, 0x0000, 0x0C06, 0x0000, 0x1001, 0x0000, 0x2000, 0x8000,
    0x4000, 0x4000, 0x4000, 0x6000, 0x8000, 0x2000, 0x8000, 0x3000,
    0x8000, 0x3000, 0x8000, 0x3000, 0x8000, 0x3000, 0x8000, 0x3000,
    0x8000, 0x3000, 0x4000, 0x7000, 0x4000, 0x6000, 0x2000, 0xE000,
    0x1001, 0xC000, 0x0C07, 0x8000, 0x07FF, 0x0000, 0x01FC, 0x0000
};
static          mpr_static(panel_choice_off_24_pr, 20, 20, 1, panel_choice_off_24_image);

static short    panel_choice_on_24_image[] = {
    0x0000, 0x0000, 0x01FC, 0x0000, 0x0603, 0x0000, 0x0800, 0x8000,
    0x10F8, 0x4000, 0x23FE, 0x2000, 0x27FF, 0x2000, 0x47FF, 0x1000,
    0x4FFF, 0x9000, 0x4FFF, 0x9000, 0x4FFF, 0x9000, 0x4FFF, 0x9000,
    0x4FFF, 0x9000, 0x47FF, 0x1000, 0x27FF, 0x2000, 0x23FE, 0x2000,
    0x10F8, 0x4000, 0x0800, 0x8000, 0x0603, 0x0000, 0x01FC, 0x0000
};
static          mpr_static(panel_choice_on_24_pr, 20, 20, 1, panel_choice_on_24_image);

static short    panel_choice_off_32_image[] = {
    0x01FC, 0x0000, 0x0603, 0x0000, 0x0800, 0x8000, 0x1000, 0x4000,
    0x2000, 0x2000, 0x4000, 0x1000, 0x4000, 0x1800, 0x8000, 0x0800,
    0x8000, 0x0C00, 0x8000, 0x0C00, 0x8000, 0x0C00, 0x8000, 0x0C00,
    0x8000, 0x0C00, 0x8000, 0x0C00, 0x4000, 0x1C00, 0x4000, 0x1800,
    0x2000, 0x3800, 0x1000, 0x7000, 0x0800, 0xE000, 0x0603, 0xC000,
    0x03FF, 0x8000, 0x00FE, 0x0000
};
static          mpr_static(panel_choice_off_32_pr, 22, 22, 1, panel_choice_off_32_image);

static short    panel_choice_on_32_image[] = {
    0x0000, 0x0000, 0x00FE, 0x0000, 0x0301, 0x8000, 0x0400, 0x4000,
    0x087C, 0x2000, 0x11FF, 0x1000, 0x23FF, 0x8800, 0x27FF, 0xC800,
    0x47FF, 0xC400, 0x4FFF, 0xE400, 0x4FFF, 0xE400, 0x4FFF, 0xE400,
    0x4FFF, 0xE400, 0x4FFF, 0xE400, 0x47FF, 0xC400, 0x27FF, 0xC800,
    0x23FF, 0x8800, 0x11FF, 0x1000, 0x087C, 0x2000, 0x0400, 0x4000,
    0x0301, 0x8000, 0x00FE, 0x0000
};
static          mpr_static(panel_choice_on_32_pr, 22, 22, 1, panel_choice_on_32_image);



static short    panel_toggle_off_10_image[] = {
    0x0000, 0x0000, 0x07FF, 0x0000, 0x0401, 0x0000, 0x0401, 0x0000,
    0x0401, 0x0000, 0x0401, 0x0000, 0x0401, 0x0000, 0x0401, 0x0000,
    0x0401, 0x0000, 0x0401, 0x0000, 0x0401, 0x0000, 0x07FF, 0x0000,
};
static          mpr_static(panel_toggle_off_10_pr, 22, 12, 1, panel_toggle_off_10_image);

static short    panel_toggle_on_10_image[] = {
    0x0000, 0x8000, 0x07F9, 0x8000, 0x0403, 0x0000, 0x0406, 0x0000,
    0x048C, 0x0000, 0x05DD, 0x0000, 0x07F9, 0x0000, 0x05F9, 0x0000,
    0x04F1, 0x0000, 0x0471, 0x0000, 0x0421, 0x0000, 0x07FF, 0x0000,
};
static          mpr_static(panel_toggle_on_10_pr, 22, 12, 1, panel_toggle_on_10_image);

static short    panel_toggle_off_12_image[] = {
    0x0000, 0x0000, 0x07FF, 0xC000, 0x0400, 0x4000, 0x0400, 0x4000,
    0x0400, 0x4000, 0x0400, 0x4000, 0x0400, 0x4000, 0x0400, 0x4000,
    0x0400, 0x4000, 0x0400, 0x4000, 0x0400, 0x4000, 0x0400, 0x4000,
    0x0400, 0x4000, 0x07FF, 0xC000,
};
static          mpr_static(panel_toggle_off_12_pr, 22, 14, 1, panel_toggle_off_12_image);

static short    panel_toggle_on_12_image[] = {
    0x0000, 0x1000, 0x07FE, 0x6000, 0x0400, 0xC000, 0x0401, 0x8000,
    0x0403, 0x0000, 0x0406, 0x4000, 0x04CE, 0x4000, 0x05FC, 0x4000,
    0x07FC, 0x4000, 0x05F8, 0x4000, 0x04F8, 0x4000, 0x0470, 0x4000,
    0x0430, 0x4000, 0x07FF, 0xC000,
};
static          mpr_static(panel_toggle_on_12_pr, 22, 14, 1, panel_toggle_on_12_image);

static short    panel_toggle_off_14_image[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x3FFF, 0x8000, 0x2000, 0x8000,
    0x2000, 0x8000, 0x2000, 0x8000, 0x2000, 0x8000, 0x2000, 0x8000,
    0x2000, 0x8000, 0x2000, 0x8000, 0x2000, 0x8000, 0x2000, 0x8000,
    0x2000, 0x8000, 0x2000, 0x8000, 0x2000, 0x8000, 0x2000, 0x8000,
    0x3FFF, 0x8000,
};
static          mpr_static(panel_toggle_off_14_pr, 22, 17, 1, panel_toggle_off_14_image);

static short    panel_toggle_on_14_image[] = {
    0x0000, 0x0800, 0x0000, 0x3000, 0x3FFC, 0xE000, 0x2001, 0xC000,
    0x2003, 0x8000, 0x2007, 0x0000, 0x220E, 0x0000, 0x271E, 0x8000,
    0x2FBC, 0x8000, 0x3FF8, 0x8000, 0x2FF8, 0x8000, 0x27F0, 0x8000,
    0x23F0, 0x8000, 0x21E0, 0x8000, 0x20E0, 0x8000, 0x2040, 0x8000,
    0x3FFF, 0x8000,
};
static          mpr_static(panel_toggle_on_14_pr, 22, 17, 1, panel_toggle_on_14_image);

static short    panel_toggle_off_19_image[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x3FFF, 0xFC00, 0x2000, 0x0400,
    0x2000, 0x0400, 0x2000, 0x0400, 0x2000, 0x0400, 0x2000, 0x0400,
    0x2000, 0x0400, 0x2000, 0x0400, 0x2000, 0x0400, 0x2000, 0x0400,
    0x2000, 0x0400, 0x2000, 0x0400, 0x2000, 0x0400, 0x2000, 0x0400,
    0x2000, 0x0400, 0x2000, 0x0400, 0x2000, 0x0400, 0x2000, 0x0400,
    0x2000, 0x0400, 0x3FFF, 0xFC00,
};
static          mpr_static(panel_toggle_off_19_pr, 28, 22, 1, panel_toggle_off_19_image);

static short    panel_toggle_on_19_image[] = {
    0x0000, 0x0060, 0x0000, 0x01C0, 0x3FFF, 0xE380, 0x2000, 0x0700,
    0x2000, 0x1E00, 0x2000, 0x3C00, 0x2000, 0x7800, 0x2000, 0xF000,
    0x2101, 0xF400, 0x2383, 0xE400, 0x27C7, 0xC400, 0x2FEF, 0x8400,
    0x3FFF, 0x8400, 0x2FFF, 0x0400, 0x27FE, 0x0400, 0x23FE, 0x0400,
    0x21FC, 0x0400, 0x20FC, 0x0400, 0x2078, 0x0400, 0x2038, 0x0400,
    0x2010, 0x0400, 0x3FFF, 0xFC00,
};
static          mpr_static(panel_toggle_on_19_pr, 28, 22, 1, panel_toggle_on_19_image);


/*****************************************************************************/

#define NULL_CHOICE	-1
#define	MARK_XOFFSET	3	/* # of pixels to leave after a mark */
#define	TOGGLE_X_GAP	11	/* # of x pixels between toggle rectangles */
#define	TOGGLE_Y_GAP	6	/* # of y pixels between toggle rectangles */
#define CHOICE_GAP	-1	/* # of pixels between choice rectangles */
#define ABSTK_WIDTH	16
#define ABSTK_HEIGHT	15


/*
 * assume 8 bits per byte, so byte for nth element is n/8, bit within that
 * byte is defined by the loworder three bits of n.
 */
#define WORD(n)         (n >> 5)/* word for element n */
#define BIT(n)          (n & 0x1F)	/* bit in word for element n */

/*
 * Create a set with n elements. Clear a set with n elements. Copy n elements
 * from one set to another
 */
#define	CREATE_SET(n)		\
    ((unsigned int *) calloc((unsigned int) (WORD(n) + 1), sizeof(unsigned int)))

#define	CLEAR_SET(set, n)	\
    (bzero((char *) (set), (int) (WORD(n) + 1) * sizeof(unsigned int)))

#define	COPY_SET(from_set, to_set, n)	\
    (bcopy((char *) (from_set), (char *) (to_set), (int) ((WORD(n) + 1) * sizeof(unsigned int))))

/*
 * Add a choice by or-ing in the correct bit. Remove a choice by and-ing out
 * the correct bit.
 */
#define ADD_CHOICE(set, n)	((set)[WORD(n)] |= (1 << BIT(n)))
#define REMOVE_CHOICE(set, n)	((set)[WORD(n)] &= ~(1 << BIT(n)))

/* See if the nth bit is on */
#define IN(set, n)		(((set)[WORD(n)] >> BIT(n)) & 01)

#define	EACH_CHOICE(set, last_element, n)	\
   for ((n) = 0; (n) <= (last_element); (n)++) \
      if (IN(set, n))

#define ABSTK_X_GAP	4	/* white space between glyph and text */

static void     begin_preview(), update_preview(), cancel_preview(),
                accept_preview(), accept_menu(), paint(), layout();

static int      choice_number();
static Menu     choice_gen_menu();
static unsigned int choice_value();
static void     choice_create_menu();
static void     choice_menu_done_proc();
static void     layout_choices();
static void     paint_choice();
static void     paint_choice_image();
static void     preview_choice();
static void     update_display();
static void     update_item_rect();
static void     update_value_rect();
static Xv_opaque choice_do_menu_item();

static Panel_ops ops = {
    panel_default_handle_event,	/* handle_event() */
    begin_preview,		/* begin_preview() */
    update_preview,		/* update_preview() */
    cancel_preview,		/* cancel_preview() */
    accept_preview,		/* accept_preview() */
    accept_menu,		/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    paint,			/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    layout			/* layout() */
};

static struct pr_size image_size();

typedef struct {		/* data for a choice item */
    Panel_item      public_self;/* back pointer to object */
    int             current;	/* current choice */
    int             actual;	/* actual value of current */
    unsigned int   *value;	/* bit field of choices */
    unsigned int   *default_value;	/* default settings (bit field of
					 * choices) */
    unsigned int   *saved_value;/* save value set during preview of defaults */
    int             last;	/* last possible choice */
    Panel_setting   display_level;	/* NONE, CURRENT, ALL */
    int             choose_one;	/* only one choice allowed */
    Panel_setting   feedback;	/* MARKED, INVERTED, NONE */
    Panel_image    *choices;	/* each choice */
    Rect           *choice_rects;	/* each choice rect */
    Rect           *mark_rects;	/* each mark rect */
    int             choice_gap;	/* gap between choices */
    Pixrect       **mark_on;	/* selection mark images */
    Pixrect       **mark_off;	/* un-selected mark images */
    int             choices_bold:1;	/* bold/not choices strings (MARKED
					 * feedback only) */
    struct {
	unsigned        gap_set:1;
	unsigned        feedback_set:1;
	unsigned        choice_fixed:1;
    }               status;
}               Choice_info;


/***********************  PROCEDURES  ************************************************/

static int
choice_x_gap(choose_one)
    int             choose_one;
{
    if (choose_one)
	return (CHOICE_GAP);
    else
	return (TOGGLE_X_GAP);
}


static int
choice_y_gap(choose_one)
    int             choose_one;
{
    if (choose_one)
	return (CHOICE_GAP);
    else
	return (TOGGLE_Y_GAP);
}


Pkg_private int
choice_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_choice *item_object = (Xv_panel_choice *) item_public;
    register Choice_info *dp;

    dp = xv_alloc(Choice_info);

    /* link to object */
    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;


    ip->flags |= SHOW_MENU_MARK;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static choice ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_CHOICE_ITEM;
    panel_set_bold_label_font(ip);

    /* initialize */
    dp->value = CREATE_SET(1);	/* set with one choice */
    dp->default_value = CREATE_SET(1);	/* (default) set with one choice */
    ADD_CHOICE(dp->value, 0);	/* initial value is choice 0 selected */
    ADD_CHOICE(dp->default_value, 0);	/* default default is choice 0
					 * selected */
    dp->current = NULL_CHOICE;	/* no current choice */
    dp->actual = FALSE;		/* current choice is off */
    dp->last = 0;		/* last slot is slot zero */
    dp->display_level = PANEL_ALL;	/* default is display all choices */
    dp->choose_one = TRUE;	/* check only one choice */
    dp->feedback = PANEL_INVERTED;	/* inverted feedback */

    /*
     * initialize choices to a single string choice of "". Note that we do
     * not call layout_choices() here. The default layout is now to not have
     * the label and single mark baselines align.  If we lower the label at
     * this point, and the client has fixed its position, the baseline will
     * not be realigned when choices are added. So we settle for a bug in the
     * initial state: label baseline does not line up with single mark
     * baseline.  This restores release 3.0 behavior.
     */
    dp->choices = xv_alloc(struct panel_image);
    panel_make_image(item_get_panel_font(ip), &dp->choices[0], IM_STRING, 
	"", FALSE, FALSE);
    dp->choice_rects = xv_alloc(Rect);
    dp->choice_rects[0] = ip->value_rect;
    dp->choice_gap = 0;		/* space between choices */

    /* set up the default choice marks */
    dp->mark_on = xv_alloc(struct pixrect *);
    dp->mark_off = xv_alloc(struct pixrect *);
    /* laf begin */
    dp->mark_on[0] = &panel_choice_on_14_pr;	/* for PANEL_MARKED feedback */
    dp->mark_off[0] = &panel_choice_off_14_pr;	/* for PANEL_MARKED feedback */
    /* laf end */
    dp->mark_rects = xv_alloc(Rect);
    dp->mark_rects[0] = dp->choice_rects[0];
    dp->mark_rects[0].r_width = dp->mark_on[0]->pr_width;
    dp->mark_rects[0].r_height = dp->mark_on[0]->pr_height;

    /* update the value and item rect */
    update_item_rect(ip);

    (void) panel_append(ip);

    return XV_OK;
}


Pkg_private     Xv_opaque
choice_set_avlist(item_public, avlist)
    Panel_item      item_public;
    register Attr_avlist avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    register Choice_info *dp = CHOICE_PRIVATE(item_public);
    register Panel_attr attr;

    int             choices_type = -1;	/* IM_STRING or IM_PIXRECT */
    Xv_opaque      *choices;	/* choices array */
    struct pixfont **choices_fonts = NULL;	/* choices fonts */
    struct pixrect **mark_images, **nomark_images;
    short           mark_images_set = FALSE;
    short           nomark_images_set = FALSE;
    short           repaint = FALSE;

    Attr_avlist     orig_avlist = avlist;	/* original avlist */
    unsigned int    value;	/* initial value */
    int             which_choice, which_arg;
    short           choices_changed = FALSE;
    short           choices_moved = FALSE;
    Xv_opaque       result;

    /* if a client has called panel_item_parent this item may not */
    /* have a parent -- do nothing in this case */
    if (ip->panel == NULL) {
	return ((Xv_opaque) XV_ERROR);
    }
    /*
     * we need to have the item layed out before doing any of this, so call
     * the generic item set code now.
     */

    /*
     * XV_END_CREATE is set inside-to-out, so no need to call xv_super_set().
     * Also, we don't use it here, so return.
     */
    if ((Xv_generic_attr) * avlist == XV_END_CREATE)
	return XV_OK;

    result = xv_super_set_avlist(item_public, &xv_panel_choice_pkg, avlist);
    if (result != XV_OK)
	return result;

    while (attr = (Panel_attr) * avlist++) {
	switch (attr) {
	  case PANEL_CHOICE_STRINGS:
	  case PANEL_CHOICE_IMAGES:
	    choices_type = attr ==
		PANEL_CHOICE_STRINGS ? IM_STRING : IM_PIXRECT;
	    choices = avlist;	/* base of the array */
	    while (*avlist++);	/* skip past the list */
	    break;

	  case PANEL_CHOICE_STRING:
	  case PANEL_CHOICE_IMAGE:
	    which_choice = (int) *avlist++;
	    if (!modify_choice(ip, attr == PANEL_CHOICE_STRING ?
			   IM_STRING : IM_PIXRECT, which_choice, *avlist++))
		return (Xv_opaque) attr;
	    choices_changed = TRUE;
	    break;

	  case PANEL_CHOICE_FONTS:
	    choices_fonts = (struct pixfont **) avlist;
	    while (*avlist++);	/* skip past the list */
	    break;

	  case PANEL_CHOICES_BOLD:
	    /* Note: dp->choices_bold is only used if feedback is MARKED */
	    dp->choices_bold = (*avlist++ != 0);
	    for (which_choice = 0; which_choice <= dp->last; which_choice++)
		if (is_string(&dp->choices[which_choice]))
		    image_bold(&dp->choices[which_choice]) = dp->choices_bold;
	    choices_changed = TRUE;
	    break;

	  case PANEL_CHOICE_OFFSET:
	    dp->choice_gap = (int) *avlist++;
	    dp->status.gap_set = TRUE;
	    choices_changed = TRUE;
	    break;

	  case PANEL_CHOOSE_ONE:
	    dp->choose_one = (int) *avlist++;
	    CLEAR_SET(dp->value, dp->last);
	    if (dp->choose_one) {
		ip->item_type = PANEL_CHOICE_ITEM;
		ADD_CHOICE(dp->value, 1);	/* ??? */
	    } else {
		ip->item_type = PANEL_TOGGLE_ITEM;
		/*
		 * Note that this depends on the fact that PANEL_CHOOSE_ONE
		 * can only be specified at create time, as part of the
		 * PANEL_TOGGLE macro.  So no choices have been set yet.
		 */
		dp->mark_on[0] = &panel_toggle_on_12_pr;	/* laf */
		dp->mark_off[0] = &panel_toggle_off_12_pr;	/* laf */
		/* force re-layout and resize of rect */
		choices_changed = TRUE;
	    }
	    break;

	  case PANEL_LAYOUT:
	    avlist++;
	    if (!dp->status.gap_set)
		dp->choice_gap = ip->layout == PANEL_HORIZONTAL ?
		    choice_x_gap(dp->choose_one) : choice_y_gap(dp->choose_one);
	    choices_changed = TRUE;
	    break;

	  case PANEL_FEEDBACK:
	    dp->feedback = (Panel_setting) * avlist++;
	    dp->status.feedback_set = TRUE;
	    choices_changed = TRUE;
	    break;

	  case PANEL_MARK_IMAGES:
	    mark_images = (struct pixrect **) avlist;
	    mark_images_set = TRUE;
	    while (*avlist++);
	    break;

	  case PANEL_NOMARK_IMAGES:
	    nomark_images = (struct pixrect **) avlist;
	    nomark_images_set = TRUE;
	    while (*avlist++);
	    break;

	  case PANEL_MARK_IMAGE:
	  case PANEL_NOMARK_IMAGE:
	    avlist++;
	    if (!*avlist)
		*avlist = (Xv_opaque) & panel_empty_pr;
	    avlist++;
	    break;

	  case PANEL_SHOW_MENU:
	    /*
	     * Ignored (menus are always shown where applicable); retained
	     * for compatibility
	     */
	    break;

	  default:
	    /* skip past what we don't care about */
	    avlist = attr_skip(attr, avlist);
	    break;
	}
    }

    if (set(choices_type)) {
	if (!re_alloc_choices(ip, choices_type, choices))
	    return (Xv_opaque) XV_ERROR;
	choices_changed = TRUE;
    }
    if (choices_fonts) {
	for (which_choice = which_arg = 0; which_choice <= dp->last;
	  which_choice++, which_arg += choices_fonts[which_arg + 1] ? 1 : 0)
	    if (is_string(&dp->choices[which_choice]))
		image_font(&dp->choices[which_choice]) = choices_fonts[which_arg];
	choices_changed = TRUE;
    }
    if (mark_images_set) {
	/* reset to empty pixrect if no list */
	if (!mark_images[0])
	    for (which_choice = 0; which_choice <= dp->last; which_choice++)
		dp->mark_on[which_choice] = &panel_empty_pr;
	else
	    for (which_choice = which_arg = 0; which_choice <= dp->last;
	    which_choice++, which_arg += mark_images[which_arg + 1] ? 1 : 0)
		dp->mark_on[which_choice] = mark_images[which_arg];
	choices_changed = TRUE;
    }
    if (nomark_images_set) {
	/* reset to empty pixrect if no list */
	if (!nomark_images[0])
	    for (which_choice = 0; which_choice <= dp->last; which_choice++)
		dp->mark_off[which_choice] = &panel_empty_pr;
	else
	    for (which_choice = which_arg = 0; which_choice <= dp->last;
		 which_choice++, which_arg += nomark_images[which_arg + 1] ? 1 : 0)
		dp->mark_off[which_choice] = nomark_images[which_arg];
	choices_changed = TRUE;
    }
    /*
     * now set things that depend on the new list of choices or the
     * attributes that were set above.
     */
    avlist = orig_avlist;
    while (attr = (Panel_attr) * avlist++) {
	switch (attr) {
	  case PANEL_MARK_IMAGE:
	  case PANEL_NOMARK_IMAGE:
	    which_choice = (int) *avlist++;
	    if (which_choice < 0 || which_choice > dp->last)
		return (Xv_opaque) attr;
	    dp->mark_on[which_choice] = (struct pixrect *) * avlist++;
	    /* may have to relayout the choices */
	    choices_changed = TRUE;
	    break;

	  case PANEL_CHOICE_FONT:
	    which_choice = (int) *avlist++;
	    if (which_choice < 0 || which_choice > dp->last)
		return (Xv_opaque) attr;
	    if (is_string(&dp->choices[which_choice]))
		image_font(&dp->choices[which_choice]) = (Pixfont *) * avlist++;
	    choices_changed = TRUE;
	    break;

	  case PANEL_VALUE:
	    value = (unsigned int) *avlist++;
	    if (dp->choose_one) {
		if (value <= dp->last) {
		    CLEAR_SET(dp->value, dp->last);
		    ADD_CHOICE(dp->value, value);
		}
	    } else
		dp->value[0] = value;
	    repaint = TRUE;
	    break;

	  case PANEL_DEFAULT_VALUE:
	    value = (unsigned int) *avlist++;
	    if (dp->choose_one) {
		/* CHOICE ITEM: value contains a choice (bit) number */
		if (value <= dp->last) {
		    CLEAR_SET(dp->default_value, dp->last);
		    ADD_CHOICE(dp->default_value, value);
		}
	    } else
		/* TOGGLE ITEM: value contains a bit mask */
		dp->default_value[0] = value;	/* Note: Only first 32
						 * choices can be selected as
						 * defaults */
	    repaint = TRUE;
	    break;

	  case PANEL_TOGGLE_VALUE:
	    which_choice = (int) *avlist++;
	    if (which_choice < 0 || which_choice > dp->last)
		return (Xv_opaque) attr;
	    if (*avlist++)
		ADD_CHOICE(dp->value, which_choice);
	    else
		REMOVE_CHOICE(dp->value, which_choice);
	    repaint = TRUE;
	    break;

	  case PANEL_DISPLAY_LEVEL:
	    dp->display_level = (Panel_setting) * avlist++;

	    /* ignore PANEL_CURRENT for toggles */
	    if (dp->display_level == PANEL_CURRENT && !dp->choose_one)
		dp->display_level = PANEL_ALL;

	    /* set the default feedback */
	    if (!dp->status.feedback_set)
		switch (dp->display_level) {
		  case PANEL_NONE:
		  case PANEL_CURRENT:
		    dp->feedback = PANEL_NONE;
		    break;

		  default:
		    dp->feedback = PANEL_INVERTED;
		}
	    choices_changed = TRUE;
	    break;

	  default:
	    /* skip past what we don't care about */
	    avlist = attr_skip(attr, avlist);
	    break;
	}
    }

    /*
     * layout the choices if they have changed and no choice or mark has a
     * fixed position.
     */
    if (choices_changed) {
	if (!dp->status.choice_fixed)
	    layout_choices(ip);
	repaint = TRUE;
    }
    /* move any specified choices */
    choices_moved = move_specified(ip, orig_avlist);

    if (choices_changed || choices_moved)
	update_item_rect(ip);

    if (dp->display_level == PANEL_CURRENT) {
	/*
	 * Create the default menu. laf note:  This was originally at the end
	 * of choice_init.  However, since we want different menus depending
	 * on whether the user specifies choice or toggle (via
	 * PANEL_CHOOSE_ONE), it was moved to here.  We must first make sure
	 * that we destroy any menu that was previously created (in case
	 * choice_set_avlist was called via xv_set instead of xv_create).
	 */
	choice_create_menu(ip);
    }
    if (panel_painted(ip->panel) && repaint & !hidden(ip))
	paint(ip);

    /* The rest of the set has been done */
    return XV_SET_DONE;

}


static void
update_item_rect(ip)
    Item_info      *ip;
{
    update_value_rect(ip);
    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
}


/*
 * re_alloc_choices allocates dp->choices from choices.  The old info is
 * reused and then freed.
 */
static int
re_alloc_choices(ip, type, choices)
    register Item_info *ip;	/* the item */
    int             type;	/* IM_STRING or IM_PIXRECT */
    Xv_opaque       choices[];	/* each choice */
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    Panel_image    *old_choices = dp->choices;
    int             old_last = dp->last;
    unsigned int   *old_value = dp->value;
    unsigned int   *old_default_value = dp->default_value;
    Rect           *old_choice_rects = dp->choice_rects;
    Rect           *old_mark_rects = dp->mark_rects;
    Pixrect       **old_mark_on = dp->mark_on;
    Pixrect       **old_mark_off = dp->mark_off;
    register int    num_choices, i, old_slot;	/* counters */
    struct pr_size  size;
    int             choice_is_bold = FALSE;	/* boolean */

    /* count the # of choices */
    for (num_choices = 0; choices[num_choices]; num_choices++);

    /* allocate the panel_image[] storage for the choices */
    if ((dp->choices = (Panel_image *)
	 calloc((unsigned int) num_choices, sizeof(Panel_image))) == NULL)
	return 0;

    /* allocate the enclosing rectangles for each choice */
    if ((dp->choice_rects = (Rect *)
	 calloc((unsigned int) num_choices, sizeof(Rect))) == NULL)
	return 0;

    /* allocate the enclosing rectangles for each mark */
    if ((dp->mark_rects = (Rect *)
	 calloc((unsigned int) num_choices, sizeof(Rect))) == NULL)
	return 0;

    /* allocate the on mark images */
    if ((dp->mark_on = (Pixrect **)
	 calloc((unsigned int) num_choices, sizeof(Pixrect *))) == NULL)
	return 0;

    /* allocate the off mark images */
    if ((dp->mark_off = (Pixrect **)
	 calloc((unsigned int) num_choices, sizeof(Pixrect *))) == NULL)
	return 0;

    dp->last = num_choices - 1;	/* last slot used in base[] */

    /* allocate the value set */
    if ((dp->value = CREATE_SET(dp->last)) == NULL)
	return 0;

    /* copy the old values */
    COPY_SET(old_value, dp->value, MIN(dp->last, old_last));

    /* allocate the default value set */
    if ((dp->default_value = CREATE_SET(dp->last)) == NULL)
	return 0;

    /* copy the old default values */
    COPY_SET(old_default_value, dp->default_value, MIN(dp->last, old_last));

    /*
     * Copy the choices to the allocated storage. Here we reuse the old mark
     * and font info if it was given.
     */
    for (i = 0; i <= dp->last; i++) {
	old_slot = (i <= old_last) ? i : old_last;
	dp->mark_on[i] = old_mark_on[old_slot];
	dp->mark_off[i] = old_mark_off[old_slot];
	dp->mark_rects[i] = old_mark_rects[old_slot];
	dp->choice_rects[i] = old_choice_rects[old_slot];
	if (dp->feedback == PANEL_MARKED)
	    choice_is_bold = dp->choices_bold;	/* Sunview 1 panel style */
	size = panel_make_image(is_string(&old_choices[old_slot])
	     ? image_font(&old_choices[old_slot]) : item_get_panel_font(ip),
		  &dp->choices[i], type, choices[i], choice_is_bold, FALSE);
	dp->choice_rects[i].r_width = size.x;
	dp->choice_rects[i].r_height = size.y + VERTICAL_WHITE_SPACE;
    }

    /*
     * if there are fewer choices now, and this is not a toggle item, make
     * sure the value is <= the number of the last choice.
     */
    if (dp->choose_one && (dp->last < old_last)) {
	if (choice_number(old_value, old_last) > dp->last) {
	    CLEAR_SET(dp->value, dp->last);
	    ADD_CHOICE(dp->value, dp->last);
	}
	if (choice_number(old_default_value, old_last) > dp->last) {
	    CLEAR_SET(dp->default_value, dp->last);
	    ADD_CHOICE(dp->default_value, dp->last);
	}
    }
    /* now free the old info */
    (void) panel_free_choices(old_choices, 0, old_last);
    free((char *) old_choice_rects);
    free((char *) old_mark_rects);
    free((char *) old_mark_on);
    free((char *) old_mark_off);
    free((char *) old_value);
    free((char *) old_default_value);

    return 1;
}				/* re_alloc_choices */


/*
 * modify_choice modifies the specified choice string or image. If the
 * specified choice does not already exist, the list of choices is extended
 * by adding empty choice strings.
 */
static int
modify_choice(ip, type, which_choice, choice_info)
    register Item_info *ip;	/* the item */
    int             type;	/* IM_STRING or IM_PIXRECT */
    int             which_choice;	/* choice to change */
    Xv_opaque       choice_info;/* new choice string or pixrect */
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    Panel_image    *old_choices = dp->choices;
    int             old_last = dp->last;
    unsigned int   *old_value = dp->value;
    Rect           *old_choice_rects = dp->choice_rects;
    Rect           *old_mark_rects = dp->mark_rects;
    Pixrect       **old_mark_on = dp->mark_on;
    Pixrect       **old_mark_off = dp->mark_off;
    register int    num_choices, i, old_slot;	/* counters */
    struct pr_size  size;
    int             choice_is_bold = FALSE;	/* boolean */

    /* expand the list if not big enough */
    if (which_choice > dp->last) {
	num_choices = which_choice + 1;
	/* allocate the panel_image[] storage for the choices */
	if ((dp->choices = (Panel_image *)
	   calloc((unsigned int) num_choices, sizeof(Panel_image))) == NULL)
	    return 0;

	/* allocate the enclosing rectangles for each choice */
	if ((dp->choice_rects = (Rect *)
	     calloc((unsigned int) num_choices, sizeof(Rect))) == NULL)
	    return 0;

	/* allocate the enclosing rectangles for each mark */
	if ((dp->mark_rects = (Rect *)
	     calloc((unsigned int) num_choices, sizeof(Rect))) == NULL)
	    return 0;

	/* allocate the on mark images */
	if ((dp->mark_on = (Pixrect **)
	     calloc((unsigned int) num_choices, sizeof(Pixrect *))) == NULL)
	    return 0;

	/* allocate the off mark images */
	if ((dp->mark_off = (Pixrect **)
	     calloc((unsigned int) num_choices, sizeof(struct pixrect *))) == NULL)
	    return 0;

	dp->last = num_choices - 1;	/* last slot used in choices[] */

	/* allocate the value set */
	if ((dp->value = CREATE_SET(dp->last)) == NULL)
	    return 0;

	/* copy the old values */
	COPY_SET(old_value, dp->value, MIN(dp->last, old_last));

	/*
	 * Copy the choices to the allocated storage. Here we reuse the old
	 * mark and font info if it was given.
	 */
	for (i = 0; i <= dp->last; i++) {
	    old_slot = (i <= old_last) ? i : old_last;
	    dp->mark_on[i] = old_mark_on[old_slot];
	    dp->mark_off[i] = old_mark_off[old_slot];
	    dp->mark_rects[i] = old_mark_rects[old_slot];
	    dp->choice_rects[i] = old_choice_rects[old_slot];
	    if (i <= old_last)
		dp->choices[i] = old_choices[old_slot];
	    else {
		if (dp->feedback == PANEL_MARKED)
		    choice_is_bold = dp->choices_bold;	/* Sunview 1 panel style */
		size = panel_make_image(is_string(&old_choices[old_slot])
					? image_font(&old_choices[old_slot])
					: item_get_panel_font(ip),
					&dp->choices[i], IM_STRING, "",
					choice_is_bold, FALSE);
		dp->choice_rects[i].r_width = size.x;
		dp->choice_rects[i].r_height = size.y + VERTICAL_WHITE_SPACE;
	    }
	}

    }
    if (dp->feedback == PANEL_MARKED)
	choice_is_bold = dp->choices_bold;	/* Sunview 1 panel style */
    size = panel_make_image(is_string(&dp->choices[which_choice])
	 ? image_font(&dp->choices[which_choice]) : item_get_panel_font(ip),
      &dp->choices[which_choice], type, choice_info, choice_is_bold, FALSE);
    dp->choice_rects[which_choice].r_width = size.x;
    dp->choice_rects[which_choice].r_height = size.y + VERTICAL_WHITE_SPACE;

    if (dp->last != old_last) {
	/* now free the old info */
	/* if new array is smaller, free the unused strings */
	/* in any case, free old_choices */
	(void) panel_free_choices(old_choices, (int) (dp->last + 1), old_last);
	free((char *) old_choice_rects);
	free((char *) old_mark_rects);
	free((char *) old_mark_on);
	free((char *) old_mark_off);
	free((char *) old_value);
    }
    return 1;
}				/* modify_choice */


/*
 * layout_choices lays out the choices in ip.
 */
static void
layout_choices(ip)
    register Item_info *ip;	/* the item */
{
    register Choice_info *dp;	/* choice data */
    register int    i;		/* counter */
    Panel_image    *image;	/* each choice image */
    struct pr_size  size;	/* each choice size */
    Rect           *rect;	/* each choice rect */
    Rect           *mark_rect;	/* each mark rect */
    int             left, top;	/* corner of each choice */
    int             above_baseline;	/* amount above baseline */
    register int    max_above;	/* max. amount above baseline */
    int             max_stringwidth;	/* max width of choice items */

    dp = CHOICE_FROM_ITEM(ip);
    dp->choice_gap = ip->layout == PANEL_HORIZONTAL ?
	choice_x_gap(ip->item_type == PANEL_CHOICE_ITEM) :
	choice_y_gap(ip->item_type == PANEL_CHOICE_ITEM);

    /* figure out longest choice item, if we are laid out vertically */
    max_stringwidth = 0;
    if ((ip->layout == PANEL_VERTICAL) || (dp->display_level == PANEL_CURRENT)) {
	for (i = 0; i <= dp->last; i++) {
	    image = &(dp->choices[i]);
	    if (image->im_type == IM_STRING) {
		size = xv_pf_textwidth(strlen(image->im_value.t.text),
			    image->im_value.t.font, image->im_value.t.text);
		if (size.x > max_stringwidth)
		    max_stringwidth = size.x;
	    }
	}
    }
    /* initialize above baseline offset */
    max_above = 0;

    /*
     * make sure the label is in the right place. This is a hack to account
     * for the baseline label adjustment done below.
     */
    panel_fix_label_position(ip);

    /* account for label size */
    if (ip->label_rect.r_top == ip->value_rect.r_top) {
	size = image_size(&ip->label, &above_baseline, 0);
	max_above = MAX(max_above, above_baseline);
    }
    /* initalize the value width & height */
    ip->value_rect.r_width = 0;
    ip->value_rect.r_height = 0;

    left = ip->value_rect.r_left;

    /* layout each choice & mark */
    for (i = 0; i <= dp->last; i++) {
	image = &(dp->choices[i]);
	rect = &(dp->choice_rects[i]);
	mark_rect = &(dp->mark_rects[i]);
	size = image_size(image, &above_baseline, max_stringwidth);
	if (dp->display_level == PANEL_CURRENT) {
	    size.x += ABSTK_WIDTH + ABSTK_X_GAP;
	    size.y = MAX(size.y, ABSTK_HEIGHT);
	}
	/* compute maximum above baseline */
	max_above = MAX(max_above, above_baseline);

	/* adjust for the mark, if any */
	if (dp->feedback == PANEL_MARKED) {
	    /* construct the mark rect */
	    rect_construct(mark_rect, left, 0, MARK_XOFFSET +
		   MAX(dp->mark_on[i]->pr_width, dp->mark_off[i]->pr_width),
		  MAX(dp->mark_on[i]->pr_height, dp->mark_off[i]->pr_height)
		);
	    /* update maximum above baseline */
	    max_above = MAX(max_above, mark_rect->r_height);
	}
	rect_construct(rect, dp->feedback == PANEL_MARKED ?
		       rect_right(mark_rect) + 1 : left,
		       0, size.x, size.y);

	/* compute the position of the next choice */
	if (dp->display_level == PANEL_ALL && ip->layout == PANEL_HORIZONTAL)
	    left = rect_right(rect) + 1 + dp->choice_gap;
    }

    /* compute the top of each choice rect */
    top = ip->value_rect.r_top;

    /*
     * adjust label rect height only if horizontal with value rect
     */
    if (ip->label_rect.r_top == ip->value_rect.r_top) {
	size = image_size(&ip->label, &above_baseline, 0);
	ip->label_rect.r_top += max_above - above_baseline;
    }
    for (i = 0; i <= dp->last; i++) {
	size = image_size(&dp->choices[i], &above_baseline, max_stringwidth);
	if (dp->display_level == PANEL_CURRENT) {
	    size.x += ABSTK_WIDTH + ABSTK_X_GAP;
	    size.y = MAX(size.y, ABSTK_HEIGHT);
	}
	rect = &dp->choice_rects[i];
	rect->r_top = top + max_above - above_baseline;
	mark_rect = &dp->mark_rects[i];
	mark_rect->r_top = top + max_above - mark_rect->r_height;
	if (dp->display_level == PANEL_ALL && ip->layout == PANEL_VERTICAL)
	    /* move down one row */
	    top = rect_bottom(rect) + 1 + dp->choice_gap;
    }
}				/* layout_choices */


/*
 * move_specified moves the specified choices marks in ip. If any choices are
 * moved, TRUE is returned.
 */
static int
move_specified(ip, avlist)
    register Item_info *ip;
    register Attr_avlist avlist;
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    register Panel_attr attr;
    register int    which_choice;	/* index of current choice */
    register int    i;		/* counter */
    int            *xs;		/* choice x coordinates */
    int            *ys;		/* choice y coordinates */
    int            *mark_xs;	/* mark x coordinates */
    int            *mark_ys;	/* mark y coordinates */
    int             moved = FALSE;	/* TRUE if moved */

    while (attr = (Panel_attr) * avlist++) {
	switch (attr) {
	  case PANEL_CHOICE_X:
	    i = (int) *avlist++;
	    dp->choice_rects[i].r_left = (int) *avlist++;
	    dp->status.choice_fixed = TRUE;
	    ip->flags |= VALUE_X_FIXED;
	    moved = TRUE;
	    break;

	  case PANEL_CHOICE_Y:
	    i = (int) *avlist++;
	    dp->choice_rects[i].r_top = (int) *avlist++;
	    dp->status.choice_fixed = TRUE;
	    ip->flags |= VALUE_Y_FIXED;
	    moved = TRUE;
	    break;

	  case PANEL_MARK_X:
	    i = (int) *avlist++;
	    dp->mark_rects[i].r_left = (int) *avlist++;
	    dp->status.choice_fixed = TRUE;
	    ip->flags |= VALUE_X_FIXED;
	    moved = TRUE;
	    break;

	  case PANEL_MARK_Y:
	    i = (int) *avlist++;
	    dp->mark_rects[i].r_top = (int) *avlist++;
	    dp->status.choice_fixed = TRUE;
	    ip->flags |= VALUE_Y_FIXED;
	    moved = TRUE;
	    break;

	  case PANEL_CHOICE_XS:
	    xs = (int *) avlist;
	    dp->status.choice_fixed = TRUE;
	    ip->flags |= VALUE_X_FIXED;
	    if (xs[0])
		for (which_choice = i = 0; which_choice <= dp->last;
		     which_choice++, i += xs[i + 1] ? 1 : 0)
		    dp->choice_rects[which_choice].r_left = xs[i];
	    while (*avlist++);
	    moved = TRUE;
	    break;

	  case PANEL_CHOICE_YS:
	    ys = (int *) avlist;
	    dp->status.choice_fixed = TRUE;
	    ip->flags |= VALUE_Y_FIXED;
	    if (ys[0])
		for (which_choice = i = 0; which_choice <= dp->last;
		     which_choice++, i += ys[i + 1] ? 1 : 0)
		    dp->choice_rects[which_choice].r_top = ys[i];
	    while (*avlist++);
	    moved = TRUE;
	    break;

	  case PANEL_MARK_XS:
	    mark_xs = (int *) avlist;
	    dp->status.choice_fixed = TRUE;
	    ip->flags |= VALUE_X_FIXED;
	    if (mark_xs[0])
		for (which_choice = i = 0; which_choice <= dp->last;
		     which_choice++, i += mark_xs[i + 1] ? 1 : 0)
		    dp->mark_rects[which_choice].r_left = mark_xs[i];
	    while (*avlist++);
	    moved = TRUE;
	    break;

	  case PANEL_MARK_YS:
	    mark_ys = (int *) avlist;
	    dp->status.choice_fixed = TRUE;
	    ip->flags |= VALUE_Y_FIXED;
	    if (mark_ys[0])
		for (which_choice = i = 0; which_choice <= dp->last;
		     which_choice++, i += mark_ys[i + 1] ? 1 : 0)
		    dp->mark_rects[which_choice].r_top = mark_ys[i];
	    while (*avlist++);
	    moved = TRUE;
	    break;

	  default:
	    /* skip past what we don't care about */
	    avlist = attr_skip(attr, avlist);
	    break;
	}
    }
    return moved;
}				/* move_specified */


/*
 * compute the width & height of the value rect to enclose all of the choices
 * & marks.
 */
static void
update_value_rect(ip)
    Item_info      *ip;
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    Rect            rect;
    register int    i;

    if (dp->display_level != PANEL_NONE) {
	rect = ip->value_rect;
	rect.r_width = rect.r_height = 0;
	for (i = 0; i <= dp->last; i++)
	    rect = panel_enclosing_rect(&rect, &dp->choice_rects[i]);

	if (dp->feedback == PANEL_MARKED)
	    for (i = 0; i <= dp->last; i++)
		rect = panel_enclosing_rect(&rect, &dp->mark_rects[i]);
	ip->value_rect = rect;
    }
}				/* update_value_rect */


/* ops vector routines */

Pkg_private int
choice_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    register Choice_info *dp = CHOICE_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    (void) panel_free_choices(dp->choices, 0, (int) dp->last);
    free((char *) dp->choice_rects);
    free((char *) dp->mark_rects);
    free((char *) dp->mark_on);
    free((char *) dp->mark_off);
    free((char *) dp->value);
    free((char *) dp->default_value);
    free((char *) dp);

    return XV_OK;
}


static void
layout(ip, deltas)
    Item_info      *ip;
    Rect           *deltas;
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    register int    i;

    /* bump each choice & mark rect */
    for (i = 0; i <= dp->last; i++) {
	dp->choice_rects[i].r_left += deltas->r_left;
	dp->choice_rects[i].r_top += deltas->r_top;
	dp->mark_rects[i].r_left += deltas->r_left;
	dp->mark_rects[i].r_top += deltas->r_top;
    }
}				/* layout */


static void
paint(ip)
    register Item_info *ip;
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    register int    i;
    int             which_choice;
    int             y_offset;
    Pixfont        *glyph_font = NULL;
    Rect            rect;
    Rect           *rectp;
    Xv_Window       pw;

    /* paint the label, centered vertically with the choices */
    rect.r_left = ip->label_rect.r_left;
    rect.r_width = ip->label_rect.r_width;
    if (ip->layout == PANEL_HORIZONTAL) {
	rect.r_top = ip->value_rect.r_top;
	rect.r_height = ip->value_rect.r_height;
    } else {
	rect.r_top = ip->label_rect.r_top;
	rect.r_height = ip->label_rect.r_height;
    }
    panel_paint_image(ip->panel, &ip->label, &rect, inactive(ip),
		      ip->color_index);

    /* paint the choices */
    switch (dp->display_level) {
      case PANEL_NONE:		/* don't draw the choices */
	break;

      case PANEL_CURRENT:	/* draw the current choice */
	which_choice = choice_number(dp->value, dp->last);
	rectp = &dp->choice_rects[which_choice];
	y_offset = (rectp->r_height - ABSTK_HEIGHT) / 2;

	/* Draw the abbreviated button stack */
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    if (!glyph_font)
		glyph_font = (Pixfont *) xv_get(pw, WIN_GLYPH_FONT);
	    xv_glyph_char(pw, rectp->r_left, rectp->r_top + y_offset,
		      ABSTK_WIDTH, ABSTK_HEIGHT, glyph_font,
		      OLG_ABBREV_MENU_BUTTON, ip->color_index);
	PANEL_END_EACH_PAINT_WINDOW

	/* Paint the text or image */
	rect = ip->value_rect;
	rect.r_left += ABSTK_WIDTH + ABSTK_X_GAP;
	rect.r_width -= ABSTK_WIDTH + ABSTK_X_GAP;
	panel_clear(ip->panel, &rect); /* clear the current choice */
	paint_choice(ip->panel, ip, dp, which_choice, inactive(ip), TRUE);
	break;

      case PANEL_ALL:		/* draw all the choices */
	/* Draw each choice */
	for (i = 0; i <= dp->last; i++)
	    paint_choice(ip->panel, ip, dp, i, inactive(ip),
			 IN(dp->value, i));
	break;
    }
}				/* paint */


static void
begin_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    preview_choice(ip, find_choice(ip, event), event);
}


static void
preview_choice(ip, new, event)
    Item_info      *ip;
    int             new;	/* new choice # to preview */
    Event          *event;
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    int             new_is_on;


    /* no change */
    if (new == dp->current)
	return;

    /* if no new choice cancel the current choice & restore the value */
    if (new == NULL_CHOICE) {
	cancel_preview(ip, event);
	return;
    }
    new_is_on = IN(dp->value, new);
    switch (dp->choose_one) {
      case TRUE:
	/* if no current, un-mark the actual marked choice */
	if (dp->current == NULL_CHOICE)
	    update_display(ip, choice_number(dp->value, dp->last), FALSE);
	else
	    update_display(ip, dp->current, FALSE);

	/* mark the new choice */
	update_display(ip, new, TRUE);
	break;

      case FALSE:
	/* restore the current choice */
	update_display(ip, dp->current, dp->actual);

	/* toggle the mark for new */
	update_display(ip, new, !new_is_on);
	break;
    }
    dp->current = new;
    dp->actual = new_is_on;
}


static void
update_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    begin_preview(ip, event);
}


/* ARGSUSED */
static void
cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);

    /* restore the current choice */
    update_display(ip, dp->current, dp->actual);

    /* restore the value if modified */
    if (dp->choose_one && dp->current != NULL_CHOICE)
	update_display(ip, choice_number(dp->value, dp->last), TRUE);

    dp->current = NULL_CHOICE;
}


static void
accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    unsigned int    value;

    /* nothing to accept if no current choice */
    if (dp->current == NULL_CHOICE)
	return;

    /*
     * remove current choice if only one choice allowed modify the value if
     * current is non-null
     */
    if (dp->choose_one) {
	CLEAR_SET(dp->value, dp->last);
	ADD_CHOICE(dp->value, dp->current);
    } else if (!dp->actual)
	ADD_CHOICE(dp->value, dp->current);
    else
	REMOVE_CHOICE(dp->value, dp->current);

    /* notify the client */
    value = choice_value(dp->choose_one, dp->value, dp->last);
    (*ip->notify) (ITEM_PUBLIC(ip), value, event);
    dp->current = NULL_CHOICE;
}


static void
accept_menu(ip, event)
    Item_info      *ip;
    Event          *event;
{
    Choice_info    *dp = CHOICE_FROM_ITEM(ip);
    Xv_Window       paint_window = event_window(event);
    Menu_item       menu_item;
    Rect            position_rect;
    int             i;

    if (dp->display_level != PANEL_CURRENT)
	return;

    if (ip->menu == NULL || paint_window == NULL)
	return;

    /* Select the current choice; deselect all others */
    for (i = 0; i <= dp->last; i++) {
	menu_item = menu_get(ip->menu, MENU_NTH_ITEM, i + 1);
	xv_set(menu_item, MENU_SELECTED, IN(dp->value, i), 0);
    }

    /*
     * Save public panel handle and current menu done proc. Switch to
     * button's menu done proc.
     */
    xv_set(ip->menu,
	   XV_KEY_DATA, DATA_HANDLE, dp,
	   XV_KEY_DATA, MENU_DONE_PROC, xv_get(ip->menu, MENU_DONE_PROC),
	   MENU_DONE_PROC, choice_menu_done_proc,
	   0);

    /* Show the menu */
    position_rect = ip->value_rect;
    position_rect.r_top += (position_rect.r_height - ABSTK_HEIGHT) / 2;
    position_rect.r_width = 0;	/* paint menu flush left */
    position_rect.r_height = ABSTK_HEIGHT;
    menu_show(ip->menu, paint_window, event,
	      MENU_POSITION_RECT, &position_rect,
	      MENU_PULLDOWN, TRUE,
	      0);
}


static void
choice_menu_done_proc(menu, result)
    Menu            menu;
    Xv_opaque       result;
{
    int             old_default_choice, new_default_choice;
    void            (*orig_done_proc) ();	/* original menu-done
						 * procedure */
    Choice_info    *dp = (Choice_info *) xv_get(menu, XV_KEY_DATA, DATA_HANDLE);

    /* Update the default, if necessary */
    old_default_choice = choice_number(dp->default_value, dp->last);
    new_default_choice = (int) xv_get(menu, MENU_DEFAULT) - 1;
    if (new_default_choice != old_default_choice) {
	REMOVE_CHOICE(dp->default_value, old_default_choice);
	ADD_CHOICE(dp->default_value, new_default_choice);
    }
    /* Restore original menu done proc. */
    orig_done_proc = (void (*) ()) xv_get(menu, XV_KEY_DATA, MENU_DONE_PROC);
    xv_set(menu, MENU_DONE_PROC, orig_done_proc, 0);

    /* Invoke original menu done proc (if any) */
    if (orig_done_proc)
	(orig_done_proc) (menu, result);
}


/* Create or modify the menu for this choice item. */
static void
choice_create_menu(ip)
    Item_info      *ip;
{
    Choice_info    *dp = CHOICE_FROM_ITEM(ip);
    int             i;
    int             nbr_items;

    if (ip->menu) {
	/*
	 * Destroy and remove all current items. Note: We can't destroy the
	 * menu because PANEL_CHOICE_IMAGES or PANEL_CHOICE_STRINGS can be
	 * set from within a choice stack menu notify proc, in which case the
	 * menu is still in use.
	 */
	nbr_items = (int) xv_get(ip->menu, MENU_NITEMS);
	for (i = nbr_items; i >= 1; i--) {
	    xv_destroy(xv_get(ip->menu, MENU_NTH_ITEM, i));
	    xv_set(ip->menu, MENU_REMOVE, i, 0);
	}
    } else {
	/* Create choice menu */
	ip->menu = xv_create(0, MENU_CHOICE_MENU,
			     MENU_NOTIFY_PROC, choice_do_menu_item,
			     MENU_CLIENT_DATA, ITEM_PUBLIC(ip),
	       MENU_DEFAULT, choice_number(dp->default_value, dp->last) + 1,
			     XV_INCREMENT_REF_COUNT,
			     0);
    }
    panel_images_to_menu_items(ip, dp->choices, dp->last);
}


/*
 * Respond to the selected menu item.
 */
static          Xv_opaque
choice_do_menu_item(menu, menu_item)
    Menu            menu;
    Menu_item       menu_item;
{
    Panel_item      item_public = menu_get(menu, MENU_CLIENT_DATA);
    int             menu_value = (int) menu_get(menu_item, MENU_VALUE);
    Event          *event = (Event *) menu_get(menu, MENU_LAST_EVENT);
    Item_info      *ip;
    Choice_info    *dp;

    if (!item_public)
	return 0;

    ip = ITEM_PRIVATE(item_public);
    dp = CHOICE_FROM_ITEM(ip);

    if (menu_value != choice_number(dp->value, dp->last)) {
	/* preview & accept the new choice */
	preview_choice(ip, menu_value, event);
	accept_preview(ip, event);
    }
    return menu_item;
}

static int
find_choice(ip, event)
    Item_info      *ip;
    Event          *event;
{
    register int    x = event_x(event);	/* locator x */
    register int    y = event_y(event);	/* locator y */
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    register int    i;		/* counter */
    Rect            rect;

    switch (dp->display_level) {
      case PANEL_NONE:
      case PANEL_CURRENT:
	/* Select default if pointing to abbreviated button stack image */
	rect = ip->value_rect;
	rect.r_width = ABSTK_WIDTH;
	if (!rect_includespoint(&rect, x, y))
	    return (NULL_CHOICE);
	return (choice_number(dp->default_value, dp->last));

      case PANEL_ALL:
	for (i = 0; i <= dp->last; i++)
	    if (rect_includespoint(&dp->choice_rects[i], x, y) ||
		((dp->feedback == PANEL_MARKED) &&
		 rect_includespoint(&dp->mark_rects[i], x, y)))
		return (i);

	return (NULL_CHOICE);

      default:			/* invalid display level */
	return (NULL_CHOICE);
    }
}


/*
 * paint the choice which_choice.  The off mark is drawn if dp->feedback is
 * PANEL_MARKED.
 */
static void
paint_choice(panel, ip, dp, which_choice, grey_out, selected)
    Panel_info     *panel;
    Item_info      *ip;
    register Choice_info *dp;
    register int    which_choice;
    int             grey_out;
    int             selected;
{
    Xv_Window       pw;
    Panel_image    *image;
    Rect           *rect;
    int             top;
    int             left;
    int		    color;
    int		    color_op;

    if (dp->display_level == PANEL_NONE)	/* don't draw the choice */
	return;

    if (ip->color_index < 0) {
	color = 0;
	color_op = PIX_SET;
    } else {
	color = PIX_COLOR(ip->color_index);
	color_op = PIX_SRC | color;
    }

    if (dp->feedback == PANEL_MARKED)
	/* draw the off mark */
	panel_paint_pixrect(panel,
			    selected ? dp->mark_on[which_choice] :
				dp->mark_off[which_choice],
			    &dp->mark_rects[which_choice], grey_out,
			    ip->color_index);

    /* draw the choice image */
    image = &dp->choices[which_choice];
    rect = &dp->choice_rects[which_choice];

    switch (image->im_type) {
      case IM_STRING:
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    /* baseline offset */
		top = rect->r_top + panel_fonthome(image_font(image),
						   image_string(image));
	    left = rect->r_left;
	    if (dp->feedback != PANEL_MARKED) {
		top += (rect->r_height -
			image_font(image)->pf_defaultsize.y) / 2;
		left += image_font(image)->pf_defaultsize.x;
	    }
	    if (dp->display_level == PANEL_CURRENT)
		/* Paint text to the right of the abbreviated button stack
		 * image.
		 */
		left += ABSTK_WIDTH + ABSTK_X_GAP;
	    panel_paint_text(pw, panel->gc, ip->color_index, image_font(image),
			     left, top, image_string(image));
	PANEL_END_EACH_PAINT_WINDOW
	break;

      case IM_PIXRECT:
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    left = rect->r_left + 4;
	top = rect->r_top;
	if (dp->display_level == PANEL_CURRENT) {
	    left += ABSTK_WIDTH + ABSTK_X_GAP;
	    top += (rect->r_height - image_pixrect(image)->pr_height) / 2;
	} else
	    top += 4;
	xv_rop(pw, left, top,
	       image_pixrect(image)->pr_width,
	       image_pixrect(image)->pr_height, color_op,
	       image_pixrect(image), 0, 0);
	PANEL_END_EACH_PAINT_WINDOW
	    break;
    }

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	if (dp->display_level != PANEL_CURRENT &&
	    dp->feedback != PANEL_MARKED)
	panel_paint_choice_outline(pw, rect,
	    IN(dp->default_value, which_choice), selected, ip->color_index);
    PANEL_END_EACH_PAINT_WINDOW

    /* laf */
	if (grey_out) {
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    xv_replrop(pw, rect->r_left, rect->r_top, rect->r_width,
		       rect->r_height, PIX_SRC & PIX_DST | color,
		       &panel_gray_pr, 0, 0);
	PANEL_END_EACH_PAINT_WINDOW
    }
}


Xv_private void
panel_paint_choice_outline(pw, rect, default_choice, selected, color_index)
    Xv_Window       pw;
    Rect           *rect;
    int             default_choice;
    int             selected;
    int    	    color_index;
{
    int             x, y;	/* top left coordinate of rectangle */
    int             w, h;	/* width and height of rectangle */
    int		    color_op;

    color_op = color_index < 0 ? PIX_SET : PIX_SRC | PIX_COLOR(color_index);

    x = rect->r_left;
    w = rect->r_width - 1;	/* xv_draw_rectangle uses x+w, not x+w-1 */
    y = rect->r_top;
    h = rect->r_height - 1;	/* xv_draw_rectangle uses y+h, not y+h-1 */

    xv_draw_rectangle(pw, x, y, w, h, LineSolid, PIX_CLR);
    xv_draw_rectangle(pw, x + 1, y + 1, w - 2, h - 2, LineSolid, PIX_CLR);
    xv_draw_rectangle(pw, x + 2, y + 2, w - 4, h - 4, LineSolid, PIX_CLR);

    if (selected) {		/* selected = bold (2 pixel wide) box */
	xv_draw_rectangle(pw, x, y, w, h, LineSolid, color_op);
	xv_draw_rectangle(pw, x + 1, y + 1, w - 2, h - 2, LineSolid, color_op);
    } else if (default_choice) {/* default = hollow box */
	xv_draw_rectangle(pw, x, y, w, h, LineSolid, color_op);
	xv_draw_rectangle(pw, x + 2, y + 2, w - 4, h - 4, LineSolid, color_op);
    } else {			/* not selected, not default = single-line
				 * box */
	xv_draw_rectangle(pw, x, y, w, h, LineSolid, color_op);
    }
}


/*
 * update_display updates the display to suggest or un-suggest which_choice
 * depending on the value of on.
 */
static void
update_display(ip, which_choice, on)
    register Item_info *ip;
    register int    which_choice;
    int             on;
{
    register Choice_info *dp = CHOICE_FROM_ITEM(ip);
    register Rect  *mark_rp;
    Rect            rect;

    if (dp->display_level == PANEL_NONE || which_choice == NULL_CHOICE)
	return;

    if (dp->display_level == PANEL_CURRENT) {	/* clear the old choice */
	rect = ip->value_rect;
	rect.r_left += ABSTK_WIDTH + ABSTK_X_GAP;
	rect.r_width -= ABSTK_WIDTH + ABSTK_X_GAP;
	(void) panel_clear(ip->panel, &rect);
	paint_choice(ip->panel, ip, dp, which_choice, inactive(ip), TRUE);
	return;
    }
    if (on) {
	/* turn the choice on */

	switch (dp->feedback) {
	  case PANEL_INVERTED:
	    paint_choice(ip->panel, ip, dp, which_choice, inactive(ip), TRUE);
	    break;

	  case PANEL_MARKED:
	    mark_rp = &dp->mark_rects[which_choice];
	    /* clear the off mark */
	    panel_clear(ip->panel, mark_rp);
	    /* draw the on mark */
	    panel_paint_pixrect(ip->panel, dp->mark_on[which_choice],
			 	mark_rp, inactive(ip), ip->color_index);
	    break;

	  case PANEL_NONE:
	    break;
	}
    } else {
	/* turn the choice off */

	/* un-mark/invert old */
	switch (dp->feedback) {
	  case PANEL_INVERTED:
	    paint_choice(ip->panel, ip, dp, which_choice, inactive(ip), FALSE);
	    break;

	  case PANEL_MARKED:
	    mark_rp = &dp->mark_rects[which_choice];
	    /* clear the on mark */
	    panel_clear(ip->panel, mark_rp);
	    /* draw the off mark */
	    panel_paint_pixrect(ip->panel, dp->mark_off[which_choice],
			 	mark_rp, inactive(ip), ip->color_index);
	    break;

	  case PANEL_NONE:
	    break;
	}
    }
}				/* update_display */


Pkg_private     Xv_opaque
choice_get_attr(item_public, status, which_attr, valist)
    Panel_item      item_public;
    int            *status;
    register Panel_attr which_attr;
    va_list         valist;
{
    register Choice_info *dp = CHOICE_PRIVATE(item_public);
    int             arg = va_arg(valist, int);
    register int    arg_lousy = (arg < 0 || arg > dp->last);

    switch (which_attr) {
      case PANEL_DEFAULT_VALUE:	/* ordinal value or set of values */
	return (Xv_opaque) choice_value(dp->choose_one, dp->default_value,
	    dp->last);

      case PANEL_VALUE:	/* ordinal value or set of values */
	return (Xv_opaque) choice_value(dp->choose_one, dp->value, dp->last);

      case PANEL_TOGGLE_VALUE:	/* on/off value of arg'th choice */
	if (arg_lousy)
	    return (Xv_opaque) 0;
	return (Xv_opaque) IN(dp->value, arg);

      case PANEL_DISPLAY_LEVEL:
	return (Xv_opaque) dp->display_level;

      case PANEL_FEEDBACK:
	return (Xv_opaque) dp->feedback;

      case PANEL_CHOICE_STRINGS:
      case PANEL_CHOICE_IMAGES:
	return (Xv_opaque) NULL;

      case PANEL_CHOOSE_ONE:
	return (Xv_opaque) dp->choose_one;

      case PANEL_CHOICE_FONT:
	if (arg_lousy || !is_string(&dp->choices[arg]))
	    return (Xv_opaque) 0;
	return (Xv_opaque) image_font(&dp->choices[arg]);

      case PANEL_CHOICE_FONTS:
	return (Xv_opaque) NULL;

      case PANEL_CHOICE_STRING:
	if (arg_lousy || !is_string(&dp->choices[arg]))
	    return (Xv_opaque) 0;
	return (Xv_opaque) image_string(&dp->choices[arg]);

      case PANEL_CHOICE_IMAGE:
	if (arg_lousy || !is_pixrect(&dp->choices[arg]))
	    return (Xv_opaque) 0;
	return (Xv_opaque) image_pixrect(&dp->choices[arg]);

      case PANEL_CHOICE_X:
	if (arg_lousy)
	    return (Xv_opaque) 0;
	return (Xv_opaque) dp->choice_rects[arg].r_left;

      case PANEL_CHOICE_Y:
	if (arg_lousy)
	    return (Xv_opaque) 0;
	return (Xv_opaque) dp->choice_rects[arg].r_top;

      case PANEL_MARK_X:
	if (arg_lousy)
	    return (Xv_opaque) 0;
	return (Xv_opaque) dp->mark_rects[arg].r_left;

      case PANEL_MARK_Y:
	if (arg_lousy)
	    return (Xv_opaque) 0;
	return (Xv_opaque) dp->mark_rects[arg].r_top;

      case PANEL_MARK_IMAGE:
	if (arg_lousy)
	    return (Xv_opaque) 0;
	return (Xv_opaque) dp->mark_on[arg];

      case PANEL_NOMARK_IMAGE:
	if (arg_lousy)
	    return (Xv_opaque) 0;
	return (Xv_opaque) dp->mark_off[arg];

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}


/*
 * image_size returns the size of image.  The amount of image above the
 * baseline is also returned.
 */
static struct pr_size
image_size(image, above_baseline, max_width)
    register Panel_image *image;
    register int   *above_baseline;
{
    struct pr_size  size;	/* full size */
    register char  *sp;		/* string version */
    struct pixfont *font;	/* font for string */

    switch (image->im_type) {
      case IM_STRING:
	sp = image_string(image);
	font = image_font(image);
	size = xv_pf_textwidth(strlen(sp), font, sp);
	if (max_width)
	    size.x = max_width + 2 * font->pf_defaultsize.x;
	else
	    size.x += 2 * font->pf_defaultsize.x;
	/*
	 * Use max height of font (ascent + descent).  Leave some vertical
	 * space between bounding box and string.
	 */
	size.y = font->pf_defaultsize.y + VERTICAL_WHITE_SPACE;

	if (*sp)
	    *above_baseline = -font->pf_char[*sp].pc_home.y + 1;
	else
	    *above_baseline = 0;
	break;

      case IM_PIXRECT:
	size = image_pixrect(image)->pr_size;
	*above_baseline = size.y;
	/* Leave space between bounding box and image */
	size.x += 8;
	size.y += 8;
	break;
    }
    return (size);
}				/* image_size */


/* Return the index of the first set bit in value_set */
static int
choice_number(value_set, last_element)
    register unsigned int *value_set;
    register int    last_element;
{
    register int    i;

    EACH_CHOICE(value_set, last_element, i)
	return i;
    return 0;
}

static unsigned int
choice_value(choose_one, value_set, last_element)
    int             choose_one;
    unsigned int   *value_set;
    int             last_element;
{
    return (choose_one ? (choice_number(value_set, last_element)) : (value_set[0]));
}
