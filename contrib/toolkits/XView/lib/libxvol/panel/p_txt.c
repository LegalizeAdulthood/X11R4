#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_txt.c 20.65 89/10/24";
#endif
#endif


/***********************************************************************/
/* panel_text.c                             */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#include <xview_private/panel_impl.h>
#include <xview/defaults.h>
#include <xview/screen.h>
#include <xview/pixwin.h>
#include <X11/X.h>

#define TEXT_PRIVATE(item)	\
	XV_PRIVATE(Text_info, Xv_panel_text, item)
#define TEXT_PUBLIC(item)	XV_PUBLIC(item)

#define	TEXT_FROM_ITEM(ip)	TEXT_PRIVATE(ITEM_PUBLIC(ip))

Pkg_private int text_init();
Pkg_private Xv_opaque text_set_avlist();
Pkg_private Xv_opaque text_get_attr();
Pkg_private int text_destroy();
Pkg_private void panel_seln_delete();



static Xv_opaque panel_setcaret();

char           *malloc();

/*
 * laf (look and feel) begin -- this is the same as the one defined in
 * panel_slider.c
 */


/***********************************************************************/
/* field-overflow pointer                                    		 */
/***********************************************************************/

static short    left_arrow_image[] = {
#include <images/panel_left_arrow.pr>
};
static          mpr_static(panel_left_arrow_pr, 16, 14, 1, left_arrow_image);

static short    right_arrow_image[] = {
#include <images/panel_right_arrow.pr>
};
static          mpr_static(panel_right_arrow_pr, 16, 14, 1, right_arrow_image);

#define PRIMARY_SELN 0
#define SECONDARY_SELN 1
static int      select_click_cnt[2] = {0, 0};	/* nbr of select mouse clicks

						 * 
						 * pending (primary, secondary) */
static struct timeval last_click_time;
static int      multi_click_timeout = -1;
static u_char   delete_pending = FALSE;	/* primary selection is
					 * pending-delete */
static Panel_info *primary_seln_panel, *secondary_seln_panel;
static Rect     primary_seln_rect, secondary_seln_rect;
static int      primary_seln_first, primary_seln_last;
static int      secondary_seln_first, secondary_seln_last;

typedef enum {
    HL_NONE,
    HL_UNDERLINE,
    HL_STRIKE_THRU,
    HL_INVERT
} highlight_t;

static int erase_go_actions[] = {
    ACTION_ERASE_CHAR_BACKWARD,
    ACTION_ERASE_CHAR_FORWARD,
    ACTION_ERASE_WORD_BACKWARD,
    ACTION_ERASE_WORD_FORWARD,
    ACTION_ERASE_LINE_BACKWARD,
    ACTION_ERASE_LINE_END,
    ACTION_GO_CHAR_BACKWARD,
    ACTION_GO_CHAR_FORWARD,
    ACTION_GO_WORD_BACKWARD,
    ACTION_GO_WORD_END,
    ACTION_GO_WORD_FORWARD,
    ACTION_GO_LINE_BACKWARD,
    ACTION_GO_LINE_END,
    ACTION_GO_LINE_FORWARD,
    ACTION_GO_COLUMN_BACKWARD,
    ACTION_GO_COLUMN_FORWARD,
    0	/* list terminator */
};

static highlight_t seln_highlight = HL_NONE;

#define PV_HIGHLIGHT TRUE
#define PV_NO_HIGHLIGHT FALSE

static void     accept_key();
static void     accept_preview();
static void     begin_preview();
static void     blink_value();
static void     cancel_preview();
static void     deselect();
static void     horizontal_scroll();
static void     insert();
static void     paint();
static void     paint_caret();
static void     paint_text();
static void     paint_value();
static void     panel_find_word();
static void     panel_multiclick_handler();
static void     panel_select_line();
static void     panel_text_handle_event();
static void     remove();
static void     restore();
static void     text_caret_invert();
static void     text_caret_on();
static void     text_seln_hilite();
static void     update_caret_offset();
static void     update_text_rect();
static void     update_value();
static void     update_value_offset();

extern void     (*panel_caret_on_proc) ();
extern void     (*panel_caret_invert_proc) ();
extern void     (*panel_seln_hilite_proc) ();
extern struct pr_size xv_pf_textwidth();

extern struct pixrect textedit_cursor_pr;


static struct panel_ops ops = {
    panel_text_handle_event,	/* handle_event() */
    begin_preview,		/* begin_preview() */
    begin_preview,		/* update_preview() */
    cancel_preview,		/* cancel_preview() */
    accept_preview,		/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    accept_key,			/* accept_key() */
    paint,			/* paint() */
    remove,			/* remove() */
    restore,			/* restore() */
    (void (*) ()) panel_nullproc/* layout() */
};


/***********************************************************************/
/* data area		                       */
/***********************************************************************/

typedef struct {
    Panel_item      public_self;/* back pointer to object */
    Panel_setting   notify_level;	/* NONE, SPECIFIED, NON_PRINTABLE,
					 * ALL */
    Item_info      *orig_caret;	/* original item with the caret */
    Rect            text_rect;	/* rect containing text (i.e., not arrows) */
    int             flags;
    char           *value;
    Pixfont        *font;
    int             font_home;
    char            mask;
    int             caret_offset;	/* caret's x offset from right margin
					 * of left arrow (which may be
					 * blank). -1 = invalid. */
    int             caret_position;	/* caret's character position */
    int             value_offset;	/* right margin of last displayed
					 * char (x offset from right margin
					 * of left arrow) */
    int             last_char;	/* last displayed character */
    int             seln_first[2];	/* index of first char selected
					 * (primary, secondary) */
    int             seln_last[2];	/* index of last char selected
					 * (primary, secondary) */
    int             ext_first;	/* first char of extended word */
    int             ext_last;	/* last char of extended word */
    int             first_char;	/* first displayed character */
    char           *terminators;
    int             stored_length;
    int             display_length;	/* in characters */
    char            edit_bk_char;
    char            edit_bk_word;
    char            edit_bk_line;
    Item_info      *next;
}               Text_info;


#define SELECTING_ITEM	0x00000001
#define HASCARET		0x00000002
#define UNDERLINED		0x00000008
#define READ_ONLY		0x00000010
#define SELECTING_TEXT	0x00000020

#define BOX_X   4		/* laf */
#define BOX_Y   2		/* laf */

#define LINE_Y   1		/* laf */

#define UP_CURSOR_KEY           (KEY_RIGHT(8))
#define DOWN_CURSOR_KEY         (KEY_RIGHT(14))
#define RIGHT_CURSOR_KEY        (KEY_RIGHT(12))
#define LEFT_CURSOR_KEY         (KEY_RIGHT(10))

Pkg_private int
text_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_text  *item_object = (Xv_panel_text *) item_public;
    register Text_info *dp;
    char           *def_str;
    Xv_object       screen = xv_get(panel_public, XV_SCREEN);
    int             i;

    dp = xv_alloc(Text_info);

    /* link to object */
    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    /* set the caret functions for panel_public.c and panel_select.c */
    panel_caret_on_proc = text_caret_on;
    panel_caret_invert_proc = text_caret_invert;
    panel_seln_hilite_proc = text_seln_hilite;

    if (!ip->panel->seln_client)
	panel_seln_init(ip->panel);

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static text ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_TEXT_ITEM;
    if (ip->notify == panel_nullproc)
	ip->notify = (int (*) ()) panel_text_notify;
    panel_set_bold_label_font(ip);

    dp->notify_level = PANEL_SPECIFIED;
    dp->font = item_get_panel_font(ip);
    (void) xv_set(dp->font, XV_INCREMENT_REF_COUNT, 0);
    for (i = 0; i <= 127; i++) {
	if (dp->font->pf_char[i].pc_home.y < dp->font_home)
	    dp->font_home = dp->font->pf_char[i].pc_home.y;
    }
    dp->font_home = -dp->font_home;
    dp->mask = '\0';
    dp->terminators = panel_strsave("\n\r\t");
    dp->stored_length = 80;
    dp->display_length = 80;
    dp->flags |= UNDERLINED;

    /*
     * Keymapping has made the following edit character settings obsolete.
     * (see update_value() below)
     */
    if (ip->panel->edit_bk_char == NULL) {
	def_str = defaults_get_string("keyboard.deleteChar",
				      "Keyboard.DeleteChar", "\177");	/* ??? Keymapping
									 * strategy? */
	ip->panel->edit_bk_char = def_str[0];
	def_str = defaults_get_string("keyboard.deleteWord",
				      "Keyboard.DeleteWord", "\027");	/* ??? Keymapping
									 * strategy? */
	ip->panel->edit_bk_word = def_str[0];
	def_str = defaults_get_string("keyboard.deleteLine",
				      "Keyboard.DeleteLine", "\025");	/* ??? Keymapping
									 * strategy? */
	ip->panel->edit_bk_line = def_str[0];
    }
    dp->edit_bk_char = ip->panel->edit_bk_char;
    dp->edit_bk_word = ip->panel->edit_bk_word;
    dp->edit_bk_line = ip->panel->edit_bk_line;

    if (multi_click_timeout == -1) {
	multi_click_timeout = 100 *
	    defaults_get_integer_check("openWindows.multiClickTimeout",
				 "OpenWindows.MultiClickTimeout", 4, 1, 10);
    }
    dp->value = (char *) calloc(1, (u_int) (dp->stored_length + 1));
    if (!dp->value)
	return (NULL);

    ip->value_rect.r_width = panel_col_to_x(dp->font, dp->display_length);
    ip->value_rect.r_height = dp->font->pf_defaultsize.y + BOX_Y;
    if (ip->value_rect.r_height < panel_left_arrow_pr.pr_height)
	ip->value_rect.r_height = panel_left_arrow_pr.pr_height;

    /*
     * BUG: since set is not called yet, append will not have the right size
     * for the item.
     */
    (void) panel_append(ip);

    /* only insert in caret list if not hidden already */
    if (!hidden(ip))
	insert(ip);

    return XV_OK;
}


Pkg_private     Xv_opaque
text_set_avlist(item_public, avlist)
    Panel_item      item_public;
    register Attr_avlist avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    register Text_info *dp = TEXT_PRIVATE(item_public);
    register Panel_attr attr;
    char           *new_value = NULL;
    short           value_rect_changed = FALSE;
    extern char    *realloc();
    extern char    *strncpy();
    Panel_info     *panel = ip->panel;

    /* if a client has called panel_item_parent this item may not */
    /* have a parent -- do nothing in this case */
    if (panel == NULL) {
	return ((Xv_opaque) XV_ERROR);
    }
    while (attr = (Panel_attribute) * avlist++) {
	switch (attr) {
	  case PANEL_VALUE:
	    new_value = (char *) *avlist++;
	    break;

	  case PANEL_VALUE_FONT:
	    if (dp->font) {
		(void) xv_set(dp->font, XV_DECREMENT_REF_COUNT, 0);
	    }
	    dp->font = (Pixfont *) * avlist++;
	    if (dp->font) {
		(void) xv_set(dp->font, XV_INCREMENT_REF_COUNT, 0);
	    }
	    value_rect_changed = TRUE;
	    break;

	    /* laf begin */
	  case PANEL_VALUE_UNDERLINED:
	    if (*avlist++) {
		dp->flags |= UNDERLINED;
	    } else {
		dp->flags &= ~UNDERLINED;
	    }
	    break;
	    /* laf end */

	  case PANEL_NOTIFY_LEVEL:
	    dp->notify_level = (Panel_setting) * avlist++;
	    break;

	  case PANEL_NOTIFY_STRING:
	    if (dp->terminators)
		free(dp->terminators);
	    dp->terminators = panel_strsave((char *) *avlist++);
	    break;

	  case PANEL_VALUE_STORED_LENGTH:
	    dp->stored_length = (int) *avlist++;
	    dp->value = (char *) realloc(dp->value, (u_int) (dp->stored_length + 1));
	    break;

	  case PANEL_VALUE_DISPLAY_LENGTH:
	    dp->display_length = (int) *avlist++;
	    value_rect_changed = TRUE;
	    break;

	  case PANEL_MASK_CHAR:
	    dp->mask = (char) *avlist++;
	    break;


	  case PANEL_INACTIVE:
	    if (*avlist++ && panel->caret == ip) {
		/* Text item had caret: move caret to next text item, if any */
		panel_caret_on(panel, FALSE);
		panel->caret = panel->caret == dp->next ? NULL : dp->next;
		if (panel->caret)
		    panel->caret->flags |= HASCARET;
		if (panel_seln(panel, SELN_CARET)->ip == ip) {
		    panel_seln_cancel(panel, SELN_CARET);
		    panel_seln(panel, SELN_CARET)->ip = panel->caret;
		}
		panel_caret_on(panel, TRUE);
		/* Remove primary selection from item, if any */
		if (panel_seln(panel, SELN_PRIMARY)->ip == ip)
		    panel_seln_cancel(panel, SELN_PRIMARY);
	    }
	    break;

	  case PANEL_READ_ONLY:
	    if (*avlist++) {
		dp->flags |= READ_ONLY;
		if (panel->caret == ip) {
		    /*
		     * Text item had caret: move caret to next text item, if
		     * any
		     */
		    panel_caret_on(panel, FALSE);
		    panel->caret = panel->caret == dp->next ? NULL : dp->next;
		    if (panel->caret)
			panel->caret->flags |= HASCARET;
		    if (panel_seln(panel, SELN_CARET)->ip == ip) {
			panel_seln_cancel(panel, SELN_CARET);
			panel_seln(panel, SELN_CARET)->ip = panel->caret;
		    }
		    panel_caret_on(panel, TRUE);
		    /* Remove primary selection from item, if any */
		    if (panel_seln(panel, SELN_PRIMARY)->ip == ip)
			panel_seln_cancel(panel, SELN_PRIMARY);
		}
	    } else
		dp->flags &= ~READ_ONLY;
	    break;

	  default:
	    avlist = attr_skip(attr, avlist);
	    break;

	}
    }

    if (new_value) {
	char           *old_value = dp->value;

	dp->value = (char *) calloc(1, (u_int) (dp->stored_length + 1));
	(void) strncpy(dp->value, new_value, dp->stored_length);

	free(old_value);
    }
    /*
     * update the value & items rect if the width or height of the value has
     * changed.
     */
    if (value_rect_changed) {
	ip->value_rect.r_width = panel_col_to_x(dp->font, dp->display_length);
	if (ip->value_rect.r_width <
	    panel_left_arrow_pr.pr_width + panel_right_arrow_pr.pr_width)
	    ip->value_rect.r_width = panel_left_arrow_pr.pr_width + 
		panel_right_arrow_pr.pr_width;
	ip->value_rect.r_height = dp->font->pf_defaultsize.y + BOX_Y;
	ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
    }
    return XV_OK;
}


/***********************************************************************/
/* get_attr                                                            */
/* returns the current value of an attribute for the text item.        */
/***********************************************************************/
Pkg_private     Xv_opaque
text_get_attr(item_public, status, which_attr, valist)
    Panel_item      item_public;
    int            *status;
    register Panel_attr which_attr;
    va_list         valist;
{
    register Text_info *dp = TEXT_PRIVATE(item_public);

    switch (which_attr) {
      case PANEL_VALUE:
	return (Xv_opaque) dp->value;

      case PANEL_VALUE_FONT:
	return (Xv_opaque) dp->font;

      case PANEL_VALUE_STORED_LENGTH:
	return (Xv_opaque) dp->stored_length;

      case PANEL_VALUE_DISPLAY_LENGTH:
	return (Xv_opaque) dp->display_length;

      case PANEL_NOTIFY_LEVEL:
	return (Xv_opaque) dp->notify_level;

      case PANEL_NOTIFY_STRING:
	return (Xv_opaque) dp->terminators;

      case PANEL_READ_ONLY:
	if (dp->flags & READ_ONLY)
	    return (Xv_opaque) TRUE;
	else
	    return (Xv_opaque) FALSE;

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}

/***********************************************************************/
/* remove                                                              */
/***********************************************************************/

static void
remove(ip)
    register Item_info *ip;
{
    register Panel_info *panel = ip->panel;
    register Item_info *prev_item, *next_item;
    short           had_caret_seln = FALSE;
    Panel_selection	*selection;

    if (TEXT_FROM_ITEM(ip)->flags & READ_ONLY)
	return;

    next_item = TEXT_FROM_ITEM(ip)->next;

    if (next_item) {

	/*
	 * cancel the selection if this item owns it.
	 */
	if (panel->seln_client) {
            selection = panel_seln(panel, SELN_PRIMARY);
            if (selection->ip == ip) {
		selection->ip = (Item_info *) 0;
                seln_done(panel->seln_client, SELN_PRIMARY);
	    }
 
            selection = panel_seln(panel, SELN_SECONDARY);
            if (selection->ip == ip) {
                selection->ip = (Item_info *) 0;
                seln_done(panel->seln_client, SELN_SECONDARY);
	    }
	}

	if (panel_seln(panel, SELN_CARET)->ip == ip) {
	    had_caret_seln = TRUE;
	    panel_seln_cancel(panel, SELN_CARET);
	}
	/* if ip is the only text item, then remove caret for panel */
	if (next_item == ip) {
	    TEXT_FROM_ITEM(ip)->next = NULL;
	    TEXT_FROM_ITEM(ip)->flags &= ~HASCARET;
	    panel->caret = NULL;
	    /* the caret is cleared so don't paint caret off */
	    /* however mark that no caret is on */
	    panel->caret_on = FALSE;
	    return;
	}
	/* find the next item */
	for (prev_item = next_item;
	     TEXT_FROM_ITEM(prev_item)->next != ip;
	     prev_item = TEXT_FROM_ITEM(prev_item)->next
	    );

	/* unlink ip from the list */
	TEXT_FROM_ITEM(prev_item)->next = next_item;
	TEXT_FROM_ITEM(ip)->next = NULL;
    }
    /* Reset the caret */
    if (panel->caret == ip) {
	/*
	 * The caret is cleared, so don't paint the caret off. However, mark
	 * that no caret is on.
	 */
	panel->caret_on = FALSE;
	TEXT_FROM_ITEM(ip)->flags &= ~HASCARET;
	/*
	 * Only reassign the caret to another item if the panel isn't being
	 * destroyed.
	 */
	if (!panel->destroying) {
	    if (next_item->panel) {
		panel->caret = next_item;
		TEXT_FROM_ITEM(next_item)->flags |= HASCARET;
		if (had_caret_seln)
		    panel_seln(panel, SELN_CARET)->ip = panel->caret;
		panel_caret_on(panel, TRUE);
	    } else
		panel->caret = NULL;
	}
    }
    return;
}


/*************************************************************************/
/* restore                                                               */
/* this code assumes that the caller has already set ip to be not hidden */
/*************************************************************************/

static void
restore(ip)
    register Item_info *ip;
{
    register Item_info *prev_item, *next_item;
    register Text_info *dp, *prev_dp;

    if (TEXT_FROM_ITEM(ip)->flags & READ_ONLY)
	return;

    /* if not removed then ignore restore() */
    if (TEXT_FROM_ITEM(ip)->next || hidden(ip))
	return;

    /* see whether selection client is initialized */
    /* may not be if restore is called because of reparent */
    if (!ip->panel->seln_client)
	panel_seln_init(ip->panel);

    /* find next non_hidden text item following ip in the circular list */
    /* of caret items (could be ip itself ) */
    next_item = ip;
    do {
	next_item = panel_successor(next_item);	/* find next unhidden item */
	if (!next_item)
	    next_item = ip->panel->items;	/* wrap to start of list   */
    } while ((next_item->item_type != PANEL_TEXT_ITEM) || hidden(next_item));

    /* now find the previous text item of next_item in the caret list */
    prev_item = next_item;
    prev_dp = TEXT_FROM_ITEM(prev_item);
    /* if next_item is different from ip, then find the previous item */
    if (next_item != ip)
	while (prev_dp->next != next_item) {
	    prev_item = prev_dp->next;
	    prev_dp = TEXT_FROM_ITEM(prev_item);
	}
    /* prev_item now points to the (circular) 	 */
    /* predecessor of ip; if ip is sole text item, */
    /* prev_item == next_item == ip.		 */

    /* link ip into the caret list */
    dp = TEXT_FROM_ITEM(ip);
    dp->next = next_item;
    prev_dp->next = ip;

    /* If this is the only item, give the caret to this item. */
    if (prev_item == next_item) {
	ip->panel->caret = ip;
	dp->flags |= HASCARET;
	/* if we have the caret selection, change the item */
	if (panel_seln(ip->panel, SELN_CARET)->ip)
	    panel_seln(ip->panel, SELN_CARET)->ip = ip;
	/* note that the caret will be drawn when the item is painted */
    }
    return;
}

/***********************************************************************/
/* insert                                                              */
/***********************************************************************/

/*
 * insert inserts ip into the caret list for ip->panel.
 */
static void
insert(ip)
    register Item_info *ip;
{
    register Item_info *head;
    register Text_info *dp;

    if (TEXT_FROM_ITEM(ip)->flags & READ_ONLY)
	return;

    head = ip->panel->caret;
    if (head == NULL) {
	ip->panel->caret = ip;
	dp = TEXT_FROM_ITEM(ip);
	dp->flags |= HASCARET;
	head = ip;
    } else {
	/* find the last caret item */
	for (dp = TEXT_FROM_ITEM(head); dp->next != head; dp = TEXT_FROM_ITEM(dp->next));

	/* link after the last */
	dp->next = ip;
    }
    /* point back to the head of the list */
    dp = TEXT_FROM_ITEM(ip);
    dp->next = head;
}

/***********************************************************************/
/* destroy                                                             */
/***********************************************************************/

Pkg_private int
text_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    register Text_info *dp = TEXT_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    remove(ITEM_PRIVATE(item_public));

    if (dp->font)
	(void) xv_set(dp->font, XV_DECREMENT_REF_COUNT, 0);

    free(dp->value);
    free(dp->terminators);
    free((char *) dp);

    return XV_OK;
}



/***********************************************************************/
/* paint                                                               */
/***********************************************************************/

static void
paint(ip)
    register Item_info *ip;
{
    Rect            text_label_rect;
    struct pr_size  image_size;
    int             image_width;

    text_label_rect = ip->label_rect;
    if (ip->label_width) {
	if (is_string(&ip->label)) {
	    image_size = xv_pf_textwidth(strlen(image_string(&ip->label)),
			  image_font(&ip->label), image_string(&ip->label));
	    image_width = image_size.x;
	} else
	    image_width = image_pixrect(&ip->label)->pr_width;
	text_label_rect.r_left += ip->label_rect.r_width - image_width;
    }
    panel_paint_image(ip->panel, &ip->label, &text_label_rect, inactive(ip),
		      ip->color_index);
    paint_text(ip);
}


/***********************************************************************/
/* paint_text                                                          */
/***********************************************************************/

static void
paint_text(ip)
    Item_info      *ip;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);

    /* compute the caret position */
    update_value_offset(ip, 0, 0);
    update_caret_offset(ip, 0);

    /* don't paint the text if masked */
    if (dp->mask != ' ')
	paint_value(ip, PV_HIGHLIGHT);
}


/*
 * paint_value clears the value rect for ip and paints the string value
 * clipped to the left of the rect.
 */
static void
paint_value(ip, highlight)
    register Item_info *ip;
    u_char          highlight;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    register Panel_info *panel = ip->panel;
    register int    x = ip->value_rect.r_left;
    register int    y = ip->value_rect.r_top;
    int		    color;
    int		    color_op;
    int             i, j, len;
    char           *str;
    Xv_Window       pw;

    if (ip->color_index < 0) {
	color = 0;
	color_op = PIX_SET;
    } else {
	color = PIX_COLOR(ip->color_index);
	color_op = PIX_SRC | color;
    }

    /* Clear the value rect */
    (void) panel_clear(panel, &ip->value_rect);

    /* Get the actual characters which will be displayed */
    len = dp->last_char - dp->first_char + 2;
    if (len) {
	str = (char *) malloc(len);
	for (j = 0, i = dp->first_char; i <= dp->last_char; i++, j++)
	    str[j] = dp->value[i];
	str[len - 1] = '\0';

	/* Draw the left clip arrow if needed */
	if (dp->first_char) {
	    /* center the arrow vertically in the value rect */
	    PANEL_EACH_PAINT_WINDOW(panel, pw)
		xv_rop(pw, x,
		       y + (ip->value_rect.r_height -
		           panel_left_arrow_pr.pr_height) / 2,
		       panel_left_arrow_pr.pr_width,
		       panel_left_arrow_pr.pr_height,
		       color_op, &panel_left_arrow_pr, 0, 0);
	    PANEL_END_EACH_PAINT_WINDOW
	}
	/* Draw the text */
	if (dp->first_char)
	    x += panel_left_arrow_pr.pr_width;
	if (dp->mask == '\0') {	/* not masked */
	    PANEL_EACH_PAINT_WINDOW(panel, pw)
		panel_paint_text(pw, ip->panel->gc, ip->color_index, dp->font,
				 x, y+dp->font_home, str);
	    PANEL_END_EACH_PAINT_WINDOW
	} else {		/* masked */
	    char           *buf;
	    int             length, i;
	    length = dp->last_char - dp->first_char + 2;
	    buf = (char *) malloc(length);
	    for (j = 0, i = dp->first_char; i <= dp->last_char; i++, j++)
		buf[j] = dp->mask; buf[length - 1] = '\0';

	    PANEL_EACH_PAINT_WINDOW(panel, pw)
		panel_paint_text(pw, ip->panel->gc, ip->color_index, dp->font,
				 x, y+dp->font_home, buf);
	    PANEL_END_EACH_PAINT_WINDOW
	    free(buf);
	}

	/* Draw the right clip arrow if needed */
	if (dp->last_char < (strlen(dp->value) - 1)) {
	    PANEL_EACH_PAINT_WINDOW(panel, pw)
		xv_rop(pw,
		       rect_right(&ip->value_rect) -
			   panel_right_arrow_pr.pr_width,
		       y + (ip->value_rect.r_height -
			   panel_right_arrow_pr.pr_height) / 2,
		       panel_right_arrow_pr.pr_width,
		       panel_right_arrow_pr.pr_height,
		       color_op, &panel_right_arrow_pr, 0, 0);
	    PANEL_END_EACH_PAINT_WINDOW
	}
	free(str);

    }
    /* Underline the text (optional) */
    if (dp->flags & UNDERLINED) {
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    xv_vector(pw, ip->value_rect.r_left, rect_bottom(&ip->value_rect),
		      ip->value_rect.r_left + ip->value_rect.r_width - 1,
		      rect_bottom(&ip->value_rect), color_op, 0);
	PANEL_END_EACH_PAINT_WINDOW
    }
    if (highlight) {
	/* re-hilite if this is a selection item */
	if (ip == panel_seln(panel, SELN_PRIMARY)->ip &&
	    !panel_seln(panel, SELN_PRIMARY)->is_null)
	    panel_seln_hilite(panel_seln(panel, SELN_PRIMARY));
	if (ip == panel_seln(panel, SELN_SECONDARY)->ip &&
	    !panel_seln(panel, SELN_SECONDARY)->is_null)
	    panel_seln_hilite(panel_seln(panel, SELN_SECONDARY));
    }
#ifdef PAINT_BOX
    /*
     * The following is used to debug font positioning problems. The painted
     * characters should not go outside of the value rectangle.
     */
    xv_draw_rectangle(pw, ip->value_rect.r_left, ip->value_rect.r_top,
		      ip->value_rect.r_width, ip->value_rect.r_height,
		      LineSolid, color_op);
#endif				/* PAINT_BOX */

    if (inactive(ip)) {
	xv_replrop(pw, ip->value_rect.r_left, ip->value_rect.r_top,
		   ip->value_rect.r_width, ip->value_rect.r_height,
		   PIX_SRC & PIX_DST | color, &panel_gray_pr, 0, 0);
    }
}


/***********************************************************************/
/* paint_caret                                                         */
/***********************************************************************/

static void
paint_caret(ip, op)
    Item_info      *ip;
    int             op;
{
    register Panel_info *panel = ip->panel;
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    register int    x, max_x;
    Rect           *r;
    int             painted_caret_offset;
    Xv_Window       pw;

    if ((op == PIX_SET && (panel->caret_on || (dp->flags & SELECTING_TEXT))) ||
	(op == PIX_CLR && !panel->caret_on))
	return;

    panel->caret_on = panel->caret_on ? FALSE : TRUE;
    op = PIX_SRC ^ PIX_DST;
    if (ip->color_index >= 0)
	op |= PIX_COLOR(ip->color_index);

    /* paint the caret after the offset & above descender */
    painted_caret_offset = dp->mask ? 0 : dp->caret_offset;
    x = ip->value_rect.r_left + painted_caret_offset - 4;
    if (dp->first_char)
	x += panel_left_arrow_pr.pr_width;

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	r = viewable_rect(panel, pw);
	max_x = r->r_left + r->r_width;
	if (x <= max_x - 3)
	    xv_rop(pw, x,
		   ip->value_rect.r_top + dp->font->pf_defaultsize.y - 4,
		   7, 7, op, panel->caret_pr, 0, 0);
    PANEL_END_EACH_PAINT_WINDOW
}

/*
 * This is to fix the flashing problem during dragging in primary selection
 * of panel text item. Instead of dehiliting the old selection and then
 * hiliting the new one (which is a continuation of the old one), just invert
 * the difference in selection.  That is either hiliting more if selection
 * has grown, or dehiliting if selection has shrinked.
 */
Pkg_private void
panel_add_selection(panel, ip, rank, event)
    register Panel_info *panel;
    Seln_rank       rank;
    Item_info      *ip;
    Event          *event;

{
    register Panel_selection *selection;
    Text_info      *dp = TEXT_FROM_ITEM(ip);
    Rect            rect;
    int             seln_rank;	/* PRIMARY_SELN or SECONDARY_SELN */
    struct pr_size  size;
    /* left and right pos of the text to invert */
    int             diff_first, diff_last;

    if (!panel->seln_client)
	return;

    selection = panel_seln(panel, rank);
    /*
     * if we already own the selection, don't ask the service for it.
     */
    if (!(ip && selection->ip == ip)) {
	rank = seln_acquire(panel->seln_client, rank);
    }
    selection = panel_seln(panel, rank);

    /* record the selection */
    selection->ip = ip;

    rect = ip->value_rect;
    seln_rank = (selection->rank == SELN_PRIMARY) ? PRIMARY_SELN : SECONDARY_SELN;

    if (select_click_cnt[seln_rank] == 0)
	return;
    if (selection->rank != SELN_PRIMARY)
	return;
    if ((dp->seln_first[seln_rank] == primary_seln_first) &&
	(dp->seln_last[seln_rank] == primary_seln_last))
	return;			/* no change */

    if (dp->seln_first[seln_rank] == primary_seln_first) {
	if (dp->seln_last[seln_rank] > primary_seln_last) {
	    /* more at the end */
	    diff_first = primary_seln_last + 1;
	    diff_last = dp->seln_last[seln_rank];
	} else if (dp->seln_last[seln_rank] < primary_seln_last) {
	    /* less at the end */
	    diff_first = dp->seln_last[seln_rank] + 1;
	    diff_last = primary_seln_last;
	}
    } else if (dp->seln_last[seln_rank] == primary_seln_last) {
	if (dp->seln_first[seln_rank] > primary_seln_first) {
	    /* less at the beg */
	    diff_first = primary_seln_first;
	    diff_last = dp->seln_first[seln_rank] - 1;
	} else if (dp->seln_first[seln_rank] < primary_seln_first) {
	    /* more at the beg */
	    diff_first = dp->seln_first[seln_rank];
	    diff_last = primary_seln_first - 1;
	}
    }
    /* Highlight characters bounded by diff_first and diff_last */
    if (diff_first >= dp->first_char) {
	size = xv_pf_textwidth((diff_first - dp->first_char),
			       dp->font, &dp->value[dp->first_char]);
	rect.r_left += size.x;
    }
    if (dp->first_char)
	rect.r_left += panel_left_arrow_pr.pr_width;
    size = xv_pf_textwidth(diff_last - diff_first + 1,
			   dp->font, &dp->value[diff_first]);
    rect.r_width = size.x;
    if (rect.r_width > dp->text_rect.r_width)
	rect.r_width = dp->text_rect.r_width;
    rect.r_height--;		/* don't disturb underlining */

    seln_highlight = HL_INVERT;
    panel_invert(ip->panel, &rect, ip->color_index);

    /* restore the rect */
    rect = ip->value_rect;
    /*
     * Update rect to be bounded by seln_first and seln_last this is
     * necessary so that primary_seln_rect is always the entire selection.
     */
    if (dp->seln_first[seln_rank] >= dp->first_char) {
	size = xv_pf_textwidth((dp->seln_first[seln_rank] - dp->first_char),
			       dp->font, &dp->value[dp->first_char]);
	rect.r_left += size.x;
    }
    if (dp->first_char)
	rect.r_left += panel_left_arrow_pr.pr_width;
    size = xv_pf_textwidth(dp->seln_last[seln_rank] - dp->seln_first[seln_rank] + 1,
			   dp->font, &dp->value[dp->seln_first[seln_rank]]);
    rect.r_width = size.x;
    if (rect.r_width > dp->text_rect.r_width)
	rect.r_width = dp->text_rect.r_width;
    rect.r_height--;		/* don't disturb underlining */

    /* update globals */
    primary_seln_panel = ip->panel;	/* save panel */
    primary_seln_rect = rect;	/* save rectangle coordinates */
    primary_seln_first = dp->seln_first[seln_rank];
    primary_seln_last = dp->seln_last[seln_rank];

}

/***********************************************************************/
/* ops vector routines                                                 */
/***********************************************************************/

/* ARGSUSED */
static void
begin_preview(ip, event)
    register Item_info *ip;
    Event          *event;
{
    Text_info      *dp = TEXT_FROM_ITEM(ip);
    register Panel_info *panel = ip->panel;
    Seln_holder     holder;
    u_int           is_null;
    int             ext_caret_offset;	/* new caret offset when adjusting to
					 * the right */
    int             save_caret_offset;	/* caret doesn't move in secondary
					 * selections */
    u_char          adjust_right;
    u_char          save_delete_pending;
    char            save_char;
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
    struct pr_size  size;
    int             dragging = 0;	/* to fix flashing bug */

    holder = panel_seln_inquire(panel, SELN_UNSPECIFIED);
    seln_rank = (holder.rank == SELN_PRIMARY) ? PRIMARY_SELN : SECONDARY_SELN;

    /*
     * Kludge to fix flashing bug when wiping thru. If SELN_PRIMARY and
     * wiping thru text item, call panel_add_selection() to invert the
     * difference in the growing or shrinking selection.
     */
    if (strlen(dp->value) && (select_click_cnt[seln_rank] > 0) &&
	(holder.rank == SELN_PRIMARY) &&
	(event_action(event) == LOC_DRAG) &&
	(event_left_is_down(event)) &&
	(dp->flags & SELECTING_TEXT))
	/*
	 * dragging SELECT: select text, do panel_add_selection()
	 */
	dragging = 1;


    /* Ask for kbd focus if this is a primary selection */
    holder = panel_seln_inquire(panel, SELN_UNSPECIFIED);
    if (holder.rank == SELN_PRIMARY)
	win_set_kbd_focus(PANEL_PUBLIC(panel),
			  xv_get(event_window(event), XV_XID));

    /*
     * If nothing is selected on the line being pointed to, then allow the
     * middle button to pick (i.e., make it look like the left button was
     * clicked).
     */
    save_delete_pending = delete_pending;
    if (((select_click_cnt[seln_rank] == 0) && (event_action(event) == ACTION_ADJUST))
	|| ((panel_seln(panel, holder.rank)->ip != ip)
	    && (event_action(event) == ACTION_ADJUST))) {
	event_set_action(event, ACTION_SELECT);
    }
    /*
     * Ignore the middle mouse button when the mouse is dragged to a line not
     * containing the caret.
     */
    if ((panel->caret != ip) &&
	((event_action(event) == LOC_DRAG) && event_middle_is_down(event))) {
	return;
    }
    /* Caret position can be set by mouse */
    save_caret_offset = dp->caret_offset;	/* in case of secondary
						 * select */
    dp->caret_offset = event->ie_locx - ip->value_rect.r_left;

    /* Check if one of the horizontal scroll buttons was selected */
    if (event_action(event) == ACTION_SELECT) {
	if (dp->first_char &&
	    dp->caret_offset >= 0 &&
	    dp->caret_offset < panel_left_arrow_pr.pr_width) {
	    /* User clicked on left arrow */
	    dp->caret_offset = save_caret_offset;
	    horizontal_scroll(ip, event, -1);	/* scroll left */
	    return;
	} else if (
		   dp->caret_offset > ip->value_rect.r_width - panel_right_arrow_pr.pr_width &&
		   dp->caret_offset < ip->value_rect.r_width &&
		   dp->last_char < strlen(dp->value) - 1) {
	    /* User clicked on right arrow */
	    dp->caret_offset = save_caret_offset;
	    horizontal_scroll(ip, event, 1);	/* scroll right */
	    return;
	}
    }
    /*
     * Define rectangle containing the text (i.e., value rect less arrows),
     * and caret offset within that rectangle.
     */
    update_text_rect(ip);

    /* Define caret_offset as offset within text rectangle */
    dp->caret_offset = event->ie_locx - dp->text_rect.r_left;
    if (dp->caret_offset < 0)
	dp->caret_offset = 0;	/* => leftmost pixel */
    if (dp->caret_offset > dp->text_rect.r_width)
	dp->caret_offset = dp->text_rect.r_width;	/* => rightmost pixel +
							 * 1 */

    if (event_action(event) == ACTION_SELECT) {
	/*
	 * This is a mouse left click event. Set seln_first and seln_last to
	 * the character pointed to by the caret.
	 */
	if (strlen(dp->value) == 0) {
	    dp->seln_first[seln_rank] = 0;
	    dp->seln_last[seln_rank] = -1;
	} else {
	    save_char = dp->value[dp->last_char + 1];
	    dp->value[dp->last_char + 1] = 0;
	    dp->seln_first[seln_rank] = dp->first_char;
	    dp->seln_first[seln_rank] += char_position(dp->caret_offset,
				      dp->font, &dp->value[dp->first_char]);
	    dp->value[dp->last_char + 1] = save_char;
	    if (dp->seln_first[seln_rank] >= strlen(dp->value))
		if ((dp->seln_first[seln_rank] = strlen(dp->value) - 1) < 0)
		    dp->seln_first[seln_rank] = 0;
	    dp->seln_last[seln_rank] = dp->seln_first[seln_rank];
	}
	if (event_action(event) == ACTION_SELECT) {
	    select_click_cnt[seln_rank]++;
	    /*
	     * If this is not a double click, or we've moved to another panel
	     * item, then reset mouse left click count to 1.
	     */
	    if ((!panel_dclick_is(&event_time(event))) ||
		(panel_seln(panel, holder.rank)->ip &&
		 panel_seln(panel, holder.rank)->ip != ip))
		select_click_cnt[seln_rank] = 1;
	}
	dp->flags &= ~SELECTING_TEXT;	/* assume we're not selecting text */
	if (select_click_cnt[seln_rank] > 1) {
	    /* Double or triple click */
	    if (!event_ctrl_is_down(event))
		dp->flags |= SELECTING_TEXT;
	    (void) panel_multiclick_handler(ip, event, seln_rank);
	} else if (event_ctrl_is_down(event) || event_action(event) == LOC_DRAG)
	    /* Single click with ctrl or drag */
	    dp->flags |= SELECTING_TEXT;

    } else if (strlen(dp->value) && (select_click_cnt[seln_rank] > 0) &&
	       ((event_action(event) == ACTION_ADJUST) ||
		((event_action(event) == LOC_DRAG) &&
	     (event_left_is_down(event) || event_middle_is_down(event))))) {
	/*
	 * ADJUST or dragging SELECT: select text
	 */
	dp->flags |= SELECTING_TEXT;

	/* Define extended selection point */
	dp->ext_first = char_position(dp->caret_offset,
				      dp->font, &dp->value[dp->first_char]);
	if (dp->first_char)
	    dp->ext_first += dp->first_char;
	if (dp->ext_first >= strlen(dp->value))
	    if ((dp->ext_first = strlen(dp->value) - 1) < 0)
		dp->ext_first = 0;
	dp->ext_last = dp->ext_first;

	if (select_click_cnt[seln_rank] >= 3) {
	    dp->ext_first = dp->seln_first[seln_rank];
	    dp->ext_last = dp->seln_last[seln_rank];
	    panel_select_line(ip, event, seln_rank);	/* update caret offset */
	} else {
	    if (select_click_cnt[seln_rank] == 2) {
		panel_find_word(dp, &dp->ext_first, &dp->ext_last);
		size = xv_pf_textwidth(dp->ext_last - dp->first_char + 1, dp->font,
				       &dp->value[dp->first_char]);
		ext_caret_offset = size.x;
	    } else
		/*
		 * Note: the following is used only when adjusting single
		 * characters to the right. This assignment forces
		 * ext_caret_offset to be to the right of the last selected
		 * character.
		 */
		ext_caret_offset = dp->caret_offset +
		    dp->font->pf_char[dp->value[dp->ext_last]].pc_pr->pr_size.x;

	    /* Adjust first or last character selected */
	    if (dp->ext_last > dp->seln_last[seln_rank])
		adjust_right = TRUE;
	    else if (dp->ext_first < dp->seln_first[seln_rank])
		adjust_right = FALSE;	/* adjust left */
	    else if ((dp->ext_first - dp->seln_first[seln_rank]) <
		     (dp->seln_last[seln_rank] - dp->ext_first))
		adjust_right = FALSE;	/* adjust left */
	    else
		adjust_right = TRUE;
	    if (adjust_right) {	/* Note: caret must be to the right of the
				 * last selected character, due to a check
				 * made on this assumption in
				 * panel_seln_delete(). */
		dp->seln_last[seln_rank] = dp->ext_last;
		dp->caret_offset = ext_caret_offset;
	    } else {		/* adjust left */
		dp->seln_first[seln_rank] = dp->ext_first;
		size = xv_pf_textwidth(dp->ext_first - dp->first_char, dp->font,
				       &dp->value[dp->first_char]);
		dp->caret_offset = size.x;
	    }
	}
    }
    delete_pending = (seln_rank == PRIMARY_SELN) && (dp->flags & SELECTING_TEXT);
    update_caret_offset(ip, 0);

    if (dragging)
	panel_add_selection(panel, ip, holder.rank, event);
    else {

	/*
	 * Make this item the selection. Dehilite any current selection and
	 * acquire the {PRIMARY,SECONDARY} selection for this ip.  If we're
	 * selecting text, and there is text to be selected, then highlight
	 * the selected text.
	 */
	is_null = !(dp->flags & SELECTING_TEXT) || strlen(dp->value) == 0;
	panel_seln_acquire(panel, ip, holder.rank, is_null);
    }

    /*
     * If we are getting the PRIMARY selection, make sure we have the CARET
     * selection also. If we are getting the SECONDARY selection, restore
     * dp->caret_offset since the insertion point (represented by the caret)
     * doesn't move during secondary selections, and restore delete_pending,
     * since it only applies to primary selections.
     */
    if (holder.rank == SELN_PRIMARY) {
	if (!(dp->flags & READ_ONLY))
	    panel_seln_acquire(panel, ip, SELN_CARET, TRUE);
    } else if (holder.rank == SELN_SECONDARY) {
	dp->caret_offset = save_caret_offset;
	delete_pending = save_delete_pending;
    }
    dp->flags |= SELECTING_ITEM;
    dp->orig_caret = ip->panel->caret;
    if (!(dp->flags & READ_ONLY))
	panel_setcaret(panel, (holder.rank == SELN_PRIMARY) ? ip : ip->panel->caret,
		       delete_pending ? PIX_CLR : PIX_SET);
}


static void
horizontal_scroll(ip, event, shift)
    register Item_info *ip;
    Event          *event;
    int             shift;	/* number of characters to shift value and
				 * caret: <0 for shift left, >0 for shift
				 * right. */
{

    cancel_preview(ip, event);
    update_value_offset(ip, 0, shift);
    update_caret_offset(ip, -shift);
    paint_value(ip, PV_HIGHLIGHT);
}



/***********************************************************************/
/* cancel_preview                                                      */
/***********************************************************************/

/* ARGSUSED */
static void
cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    register Panel_info *panel = ip->panel;

    if (dp->flags & SELECTING_ITEM) {
	deselect(panel);
	panel->caret = dp->orig_caret;
	TEXT_FROM_ITEM(dp->orig_caret)->flags |= HASCARET;
	dp->orig_caret = (Item_info *) NULL;
	dp->flags &= ~SELECTING_ITEM;
    }
    if (ip == panel_seln(panel, SELN_PRIMARY)->ip)
	panel_seln_cancel(panel, SELN_PRIMARY);
    if (ip == panel_seln(panel, SELN_SECONDARY)->ip)
	panel_seln_cancel(panel, SELN_SECONDARY);
}


/***********************************************************************/
/* accept_preview                                                      */
/***********************************************************************/

/* ARGSUSED */
static void
accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);

    if (!(dp->flags & SELECTING_ITEM))
	return;

    dp->orig_caret = (Item_info *) NULL;
    dp->flags &= ~SELECTING_ITEM;
    dp->flags &= ~SELECTING_TEXT;
    update_caret_offset(ip, 0);
}


#define CTRL_D_KEY	'\004'
#define CTRL_G_KEY	'\007'

/***********************************************************************/
/* accept_key                                                          */
/***********************************************************************/

static void
accept_key(ip, event)
    register Item_info *ip;
    register Event *event;
{
    Panel_info     *panel = ip->panel;
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    short           action = event_action(event);
    int             has_caret;
    int		    i;
    int             notify_desired = FALSE;
    Panel_setting   notify_rtn_code;
    int             ok_to_insert;
    Panel_selection *selection = panel_seln(panel, SELN_CARET);
    extern char    *index();
    char           *selection_string;
    int             selection_length;
    char            save_char;

    if (event_is_ascii(event) ||/* ASCII key */
#ifdef ecd_input
	event_is_iso(event) ||	/* ISO key */
#endif
	panel_event_is_panel_semantic(event) ||	/* erase or go char, word or
						 * line */
	(event_is_up(event) &&
	 ((action == ACTION_PASTE && select_click_cnt[SECONDARY_SELN]) ||
	  action == ACTION_CUT))) {
	if (delete_pending) {
	    for (i=0; erase_go_actions[i]; i++)
		if (erase_go_actions[i] == action)
		    break;
	    if (erase_go_actions[i]) {
		panel_seln_dehilite(ip, SELN_PRIMARY);
		delete_pending = FALSE;
	    } else {
		/* Copy the selection to the shelf */
		if (panel->shelf) {
		    free(panel->shelf);
		    panel->shelf = 0;
		}
		panel_get_text_selection(ip, &selection_string,
					 &selection_length, SELN_PRIMARY);
		if (selection_string) {
		    save_char = *(selection_string + selection_length);
		    *(selection_string + selection_length) = 0;
		    panel->shelf = panel_strsave(selection_string);
		    *(selection_string + selection_length) = save_char;
		    panel_seln_acquire(panel, (Item_info *) 0,
				       SELN_SHELF, TRUE);

		    /* Delete the selected characters */
		    panel_seln_delete(panel_seln(panel, SELN_PRIMARY));
		}
	    }
	}
	select_click_cnt[PRIMARY_SELN] = 0;
    }
    if (dp->flags & READ_ONLY)
	return;

    dp->flags &= ~SELECTING_TEXT;

    /* get the caret selection */
    if (panel->seln_client && !selection->ip)
	if (event_is_down(event) &&
	    (event_is_ascii(event) ||
#ifdef ecd_input
	     event_is_iso(event) ||
#endif
	     action == ACTION_PASTE))
	    panel_seln_acquire(panel, ip, SELN_CARET, TRUE);

    /*
     * not interested in function keys, except for acquiring the caret
     * selection.
     */
    /*
     * also, never pass ctrl-D or ctrl-G, which have been mapped to the
     * Delete and Get function keys, to the notify proc
     */
#ifdef OBSOLETE
    if (panel_event_is_xview_semantic(event) ||
	event_id(event) == CTRL_D_KEY ||
	event_id(event) == CTRL_G_KEY)
	return;
#else
    if (panel_event_is_xview_semantic(event))
	return;
#endif				/* OBSOLETE */


    /* de-hilite */
    if (ip == panel_seln(panel, SELN_PRIMARY)->ip)
	panel_seln_dehilite(ip, SELN_PRIMARY);
    else if (ip == panel_seln(panel, SELN_SECONDARY)->ip)
	panel_seln_dehilite(ip, SELN_SECONDARY);

    switch (dp->notify_level) {
      case PANEL_ALL:
	notify_desired = TRUE;
	break;
      case PANEL_SPECIFIED:
	notify_desired = ((index(dp->terminators, action) != 0) &&
			  event_is_down(event));
	break;
      case PANEL_NON_PRINTABLE:
	notify_desired = !panel_printable_char(action);
	break;
      case PANEL_NONE:
	notify_desired = FALSE;
	break;
    }
    if (notify_desired) {
	notify_rtn_code = (Panel_setting)
	    (*ip->notify) (ITEM_PUBLIC(ip), event);
	ok_to_insert = notify_rtn_code == PANEL_INSERT;
    } else {
	/* editting characters are always ok to insert */
	ok_to_insert = (panel_printable_char(action) ||
			(action == ACTION_ERASE_CHAR_BACKWARD) ||
			(action == ACTION_ERASE_CHAR_FORWARD) ||
			(action == ACTION_ERASE_WORD_BACKWARD) ||
			(action == ACTION_ERASE_WORD_FORWARD) ||
			(action == ACTION_ERASE_LINE_BACKWARD) ||
			(action == ACTION_ERASE_LINE_END));
    }

    /* Process movement actions that could change lines */
    if (event_is_down(event)) {
	switch (action) {
	  case ACTION_GO_LINE_BACKWARD:
	    /* Go to the start of this or the previous line */
	    if (dp->first_char == 0 && dp->caret_offset == 0) {
		notify_desired = FALSE;
		ok_to_insert = FALSE;
		ip = ITEM_PRIVATE(panel_backup_caret(PANEL_PUBLIC(panel)));
	    }
	    break;
	  case ACTION_GO_LINE_END:
	    /* Go to the end of this or the next line */
	    if (dp->caret_offset < dp->value_offset)
		break;
	    /* else fall through to ACTION_GO_LINE_FORWARD */
	  case ACTION_GO_LINE_FORWARD:  
	    /* Go to the start of the next line */
	    notify_desired = FALSE;
	    ok_to_insert = FALSE;
	    ip = ITEM_PRIVATE(panel_advance_caret(PANEL_PUBLIC(panel)));
	    break;
	  case ACTION_GO_COLUMN_BACKWARD: /* up arrow */
	    notify_rtn_code = PANEL_PREVIOUS;
	    notify_desired = TRUE;
	    ok_to_insert = FALSE;
	    break;
	  case ACTION_GO_COLUMN_FORWARD: /* down arrow */
	    notify_rtn_code = PANEL_NEXT;
	    notify_desired = TRUE;
	    ok_to_insert = FALSE;
	    break;
	  case ACTION_GO_CHAR_FORWARD:   /* right arrow */
	  case ACTION_GO_CHAR_BACKWARD:  /* left arrow */
	    notify_desired = FALSE;
	    ok_to_insert = FALSE;
	    break;
	  default:
	    break;
	}
    }

    /* If this item has the caret, then turn off the caret.
     * Note: has_caret is calculated after the text item's notify proc is
     *       called since the notify proc may change who has the caret.
     */
    has_caret = panel->caret == ip;
    if (has_caret)
	paint_caret(ip, PIX_CLR);

    /*
     * Do something with the character.
     */
    if (event_is_down(event))
	update_value(ip, action, ok_to_insert);

    if (has_caret)
	paint_caret(ip, PIX_SET);

    if (notify_desired && has_caret && event_is_down(event))
	switch (notify_rtn_code) {
	  case PANEL_NEXT:
	    (void) panel_advance_caret(PANEL_PUBLIC(panel));
	    break;

	  case PANEL_PREVIOUS:
	    (void) panel_backup_caret(PANEL_PUBLIC(panel));
	    break;

	  default:
	    break;
	}
}


/*
 * update_value updates the value of ip according to code.  If code is an
 * edit character, the appropriate edit action is taken. Otherwise, if
 * ok_to_insert is true, code is appended to ip->value.
 */
static void
update_value(ip, action, ok_to_insert)
    Item_info      *ip;
    register int    action;	/* event action code */
    int             ok_to_insert;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    register char  *sp;		/* string value */
    int             orig_len, new_len;	/* before & after lengths */
    register int    i;		/* counter */
    register int    x;		/* left of insert/del point */
    int             was_clipped;/* TRUE if value was clipped */
    int             orig_offset;/* before caret offset */
    int             orig_text_rect_width;	/* original text rectangle
						 * width */
    int             caret_shift = 0;	/* number of positions to move caret */
    int             insert_pos;	/* position for character add/delete */
    int             j;
    int             val_shift = 0;	/* number of characters to shift
					 * value display */
    int             val_change = 0;	/* number of characters added (+) or
					 * deleted (-) */
    int             char_code;
    struct pr_size  size;
    Xv_Window       pw;

    /* Define rectangle containing the text (i.e., value rect less arrows) */
    update_text_rect(ip);

    /* Get the insert position for character add/delete */
    if (dp->caret_offset == 0)
	insert_pos = dp->first_char;
    else
	insert_pos = dp->caret_position;

    sp = dp->value;
    orig_len = strlen(sp);

    if ((action == dp->edit_bk_char) ||
	(action == ACTION_ERASE_CHAR_BACKWARD)) {
	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* Nothing to backspace if caret is at left boundary. */
	if (dp->caret_offset == 0)
	    return;

	/* Can't show result of backspace if display length exceeded and */
	/* caret is to the right of the panel left arrow.  The moral here */
	/* is that you can't delete what you can't see. */
	if ((orig_len > dp->display_length) && (dp->first_char) && (dp->caret_offset == 0))
	    return;

	if ((*sp) && (insert_pos > 0)) {
	    for (i = insert_pos; i < orig_len; i++)
		sp[i - 1] = sp[i];
	    sp[orig_len - 1] = '\0';
	    insert_pos--;
	    caret_shift = -1;
	    val_change = -1;

	    /*
	     * If clipped at left boundary, leave caret alone. Characters
	     * will shift in from the left.
	     */
	    if (dp->first_char) {
		caret_shift = 0;
		dp->caret_position--;
	    }
	}
    } else if (action == ACTION_ERASE_CHAR_FORWARD) {

	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* Can't show result of forespace if display length exceeded and */
	/* caret is to the left of the panel right arrow.  The moral here */
	/* is that you can't delete what you can't see. */
	if (dp->caret_offset == dp->text_rect.r_width)
	    return;
	size = xv_pf_textwidth(dp->last_char - dp->first_char, dp->font,
			       &dp->value[dp->first_char]);
	if (dp->caret_offset > size.x)
	    return;

	if ((*sp) && (insert_pos >= 0)) {
	    for (i = insert_pos; i < orig_len; i++)
		sp[i] = sp[i + 1];
	    sp[orig_len - 1] = '\0';
	    caret_shift = 0;
	    val_change = 0;
	    if ((dp->last_char >= (strlen(sp) - 1)) && (dp->last_char > 1)) {
		val_change = -1;
		/*
		 * ???  Why was the following line put in here?  This causes
		 * a bug in deleting the next character when the last char is
		 * displayed and the first character is not.  ???
		 */
		/* if (dp->first_char > 2) caret_shift = 1; */
	    }
	}
    } else if ((action == dp->edit_bk_word) ||
	       (action == ACTION_ERASE_WORD_BACKWARD)) {

	/*
	 * for(i = orig_len - 1; i >= 0 && sp[i] == ' '; i--); for(; i >= 0
	 * && sp[i] != ' '; i--); sp[i + 1] = '\0';
	 */
	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* skip back past blanks */
	if (insert_pos > orig_len)
	    insert_pos -= (dp->first_char - 1);
	for (i = insert_pos - 1; (i >= 0) && (sp[i] == ' '); i--);
	for (; (i >= 0) && (sp[i] != ' '); i--);
	if (i < 0)
	    i = 0;
	if (i > 0)
	    i++;
	caret_shift = i - insert_pos;
	val_change = i - insert_pos;
	for (j = insert_pos; j <= orig_len; j++, i++)
	    sp[i] = sp[j];
	insert_pos += caret_shift;
    } else if (action == ACTION_ERASE_WORD_FORWARD) {

	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* skip back past blanks */
	for (i = insert_pos; (i < orig_len) && (sp[i] == ' '); i++);
	for (; (i < orig_len) && (sp[i] != ' '); i++);
	if (i >= orig_len)
	    i = orig_len - 1;
	if (i < (orig_len - 1))
	    i--;
	caret_shift = 0;
	val_change = 0;
	for (j = insert_pos; i < orig_len; j++, i++)
	    sp[j] = sp[i + 1];
	if ((dp->last_char >= (strlen(sp) - 1)) && (dp->first_char > 1)) {
	    val_change = strlen(sp) - 1 - dp->last_char;
	    if (dp->last_char < (orig_len - 1))
		val_change--;
	    caret_shift = -val_change;
	}
    } else if ((action == dp->edit_bk_line) ||
	       (action == ACTION_ERASE_LINE_BACKWARD)) {
	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* sp[0] = '\0'; */
	caret_shift = -insert_pos;
	val_change = -insert_pos;
	for (i = 0, j = insert_pos; j <= orig_len; i++, j++)
	    sp[i] = sp[j];
	insert_pos = 0;
    } else if (action == ACTION_ERASE_LINE_END) {

	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	caret_shift = 0;
	val_change = 0;
	sp[insert_pos] = '\0';
	if (dp->first_char > 1) {
	    val_change = strlen(sp) - 1 - dp->last_char;
	    if (dp->last_char < (orig_len - 1))
		val_change--;
	    caret_shift = -val_change;
	}
    } else if (action == ACTION_GO_CHAR_FORWARD) {
	caret_shift = 1;
	if (dp->last_char < orig_len - 1 &&
	    dp->caret_offset == dp->value_offset)
	    val_shift = 1;	/* display will include next char to right */
    } else if (action == ACTION_GO_CHAR_BACKWARD) {
	caret_shift = -1;
	if ((dp->first_char) && (dp->caret_offset == 0))
	    val_shift = -1;	/* display will include next char to left */
    } else if (action == ACTION_GO_WORD_BACKWARD) {
	/* Skip back to start of current or previous word */
	if (insert_pos > orig_len)
	    insert_pos -= (dp->first_char - 1);
	for (i = insert_pos - 1; (i >= 0) && (sp[i] == ' '); i--);
	for (; (i >= 0) && (sp[i] != ' '); i--);
	if (i < 0)
	    i = 0;
	if (i > 0)
	    i++;
	caret_shift = i - insert_pos;
	if (i < dp->first_char)
	    /* Shift value so that caret is at correct character */
	    val_shift = caret_shift;
    } else if (action == ACTION_GO_WORD_END && insert_pos < orig_len) {
	/* Skip forward to end of current or next word */
	for (i = insert_pos; (i < orig_len) && (sp[i] == ' '); i++);
	for (; (i < orig_len) && (sp[i] != ' '); i++);
	caret_shift = i - insert_pos;
	if (i > dp->last_char)
	    /* Shift value so that caret is visible */
	    val_shift = i - dp->last_char;
    } else if (action == ACTION_GO_WORD_FORWARD && insert_pos < orig_len) {
	/* Skip forward to start of next word */
	for (i = insert_pos; (i < orig_len) && (sp[i] != ' '); i++);
	for (; (i < orig_len) && (sp[i] == ' '); i++);
	caret_shift = i - insert_pos;
	if (i > dp->last_char)
	    /* Shift value so that caret is visible */
	    val_shift = i - dp->last_char;
    } else if (action == ACTION_GO_LINE_BACKWARD ||
	       action == ACTION_GO_LINE_FORWARD) {
	/* Note: For ACTION_GO_LINE_FORWARD, the caret has already been
	 * advanced to the next line.  We now want to position it at
	 * the beginning of this line.
	 */
	caret_shift = -insert_pos;
	if (dp->first_char)
	    val_shift = -dp->first_char;
    } else if (action == ACTION_GO_LINE_END) {
	caret_shift = orig_len - insert_pos;
	if (dp->last_char < orig_len - 1)
	    val_shift = orig_len - dp->last_char;
    }
	
    /* Since all events are now allowed to pass thru, need to explicitly */
    /* check for printable characters.  */
    else if (panel_printable_char(action)) {
	if (ok_to_insert) {	/* insert */
	    if (orig_len < dp->stored_length) {	/* there is room */
#ifdef ecd_input
		if (action >= ISO_FIRST && action <= ISO_LAST)
		    char_code = action - ISO_FIRST;
		else
#endif
		    char_code = action;
		for (i = orig_len; i > insert_pos; i--)
		    sp[i] = sp[i - 1];
		sp[insert_pos] = (char) char_code;
		caret_shift = 1;
		val_change = 1;
		sp[orig_len + 1] = '\0';

	    } else		/* no more room */
		blink_value(ip);

	} else			/* must be read-only */
	    blink_value(ip);
    }
    /* determine the new caret offset */
    orig_offset = dp->value_offset;
    orig_text_rect_width = dp->text_rect.r_width;
    update_value_offset(ip, val_change, val_shift);
    dp->caret_offset = -1;	/* caret offset is no longer valid */
    update_caret_offset(ip, caret_shift);
    update_text_rect(ip);

    /* update the display if not masked */
    if (dp->mask != ' ') {
	/* compute the position of the caret */
	x = dp->text_rect.r_left + dp->caret_offset;
	new_len = strlen(sp);
	was_clipped = dp->first_char || dp->last_char < new_len - 1;
	/* erase deleted characters that were displayed */
	if (new_len < orig_len) {
	    /* repaint the whole value if needed */
	    if (was_clipped || dp->text_rect.r_width != orig_text_rect_width)
		paint_value(ip, PV_HIGHLIGHT);
	    else {
		/* clear the deleted characters and everything to the right */
		PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
		    pw_writebackground(pw, x, ip->value_rect.r_top,
			orig_offset - dp->caret_offset,
			ip->value_rect.r_height - 2 * LINE_Y, PIX_CLR);
		    panel_paint_text(pw, ip->panel->gc, ip->color_index,
			dp->font, x, ip->value_rect.r_top + dp->font_home,
			&sp[insert_pos]);
		PANEL_END_EACH_PAINT_WINDOW
	    }

	} else if (new_len > orig_len) {
	    /* repaint the whole value if it doesn't fit */
	    if (was_clipped)
		paint_value(ip, PV_HIGHLIGHT);
	    else
		/* write the new character to the left of the caret */
	    if (dp->mask == '\0')	/* not masked */
		PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
		    panel_paint_text(pw, ip->panel->gc, ip->color_index,
			dp->font, 
			x - dp->font->pf_char[sp[insert_pos]].pc_adv.x,
			ip->value_rect.r_top + dp->font_home,
			&sp[insert_pos]);
	    PANEL_END_EACH_PAINT_WINDOW
		else {		/* masked */
		char            buf[2];
		buf[0] = dp->mask;
		buf[1] = '\0';
		PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
		    panel_paint_text(pw, ip->panel->gc, ip->color_index,
			dp->font, 
			x - dp->font->pf_char[dp->mask].pc_adv.x,
			ip->value_rect.r_top + dp->font_home,
			buf);
		PANEL_END_EACH_PAINT_WINDOW
	    }
	} else
	    /* Cursor key causes display shift */
	if (val_shift)
	    paint_value(ip, PV_HIGHLIGHT);
    }
}


static void
update_value_offset(ip, val_change, val_shift)
    Item_info      *ip;
    int             val_change;	/* number of characters added (+) or deleted
				 * (-) */
    int             val_shift;	/* number of characters to shift value
				 * display */
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int             full_len;
    struct pr_size  size;
    int             max_caret = ip->value_rect.r_width;
    int             i, x;
    int             max_width;

    full_len = strlen(dp->value);
    size = xv_pf_textwidth(full_len, dp->font, dp->value);

    if (size.x <= max_caret) {
	size = xv_pf_textwidth(full_len, dp->font, dp->value);
	dp->first_char = 0;
	dp->last_char = full_len - 1;
	dp->value_offset = size.x;

    } else {			/* there are more characters than can be
				 * displayed */

	if (val_change > 0) {
	    /* Add a character */

	    /*
	     * Inserted characters will always be visible and the caret is
	     * always positioned after the inserted character, unless the
	     * caret is already positioned after the last displayable
	     * character, in which case all the characters to the left of the
	     * inserted character are shifted to the left on the display.
	     */
	    if (dp->caret_position > dp->last_char) {
		/*
		 * We are appending characters to the end of the string.
		 * Compute first_char = first character that can be fully
		 * displayed when the current (just-typed) character is the
		 * last character displayed.
		 */
		x = 0;
		max_width = max_caret - panel_left_arrow_pr.pr_width;
		if (dp->caret_position < strlen(dp->value) - 1)
		    max_width -= panel_right_arrow_pr.pr_width;
		for (i = dp->caret_position; x < max_width; i--)
		    x += dp->font->pf_char[dp->value[i]].pc_adv.x;
		dp->first_char = i + 2;
	    }
	} else if (val_change < 0) {
	    /* Delete 1 or more characters */
	    dp->first_char += val_change;
	    if (dp->first_char < 0)
		dp->first_char = 0;	/* no more clip at left */
	} else {
	    /* Shift the display */
	    dp->first_char += val_shift;
	}

	/*
	 * dp->last_char = the last character in the string, starting from
	 * first_char, that can be fully displayed within the rectangle
	 * reserved for the text value string.  If the last character in the
	 * string cannot be displayed, then recompute dp->last_char to
	 * accomodate the right arrow scroll button.
	 */
	if (dp->first_char)
	    max_caret -= panel_left_arrow_pr.pr_width;
	dp->last_char = char_position(max_caret, dp->font,
			   &dp->value[dp->first_char]) - 1 + dp->first_char;
	if (dp->last_char < strlen(dp->value) - 1)
	    /*
	     * Decrement dp->last_char until enough space has been made to
	     * draw the right arrow scroll button.
	     */
	    for (x = 0; x < panel_right_arrow_pr.pr_width;)
		x += dp->font->pf_char[dp->value[dp->last_char--]].pc_adv.x;

	/* Compute value offset */
	size = xv_pf_textwidth(dp->last_char - dp->first_char + 1, dp->font,
			       &dp->value[dp->first_char]);
	dp->value_offset = size.x;
    }
}


/*
 * update_caret_offset computes the caret x offset for ip.
 */
static void
update_caret_offset(ip, caret_shift)
    Item_info      *ip;
    int             caret_shift;/* char position delta from caret_position */
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int             max_caret_pos;
    struct pr_size  size;

    if (caret_shift) {
	dp->caret_position += caret_shift;
	if (dp->caret_position < dp->first_char)
	    dp->caret_position = dp->first_char;
    } else if (dp->caret_offset >= 0)
	dp->caret_position = char_position(dp->caret_offset,
		     dp->font, &dp->value[dp->first_char]) + dp->first_char;
    max_caret_pos = strlen(dp->value);
    if (dp->caret_position > max_caret_pos)
	dp->caret_position = max_caret_pos;
    size = xv_pf_textwidth(dp->caret_position - dp->first_char, dp->font,
			   &dp->value[dp->first_char]);
    dp->caret_offset = size.x;

    /* Caret cannot exceed last character of value */
    if (dp->caret_offset > dp->value_offset)
	dp->caret_offset = dp->value_offset;
}


/*
 * blink_value blinks the value rect of ip.
 */
static void
blink_value(ip)
    Item_info      *ip;
{
    int             i;		/* counter */
    Rect            r;		/* laf */

    /* laf begin */
    r = ip->rect;
    r.r_height += BOX_Y;

    /* invert the value rect */
    (void) panel_invert(ip->panel, &ip->value_rect, ip->color_index);

    /* wait a while */
    for (i = 1; i < 5000; i++);

    /* un-invert the value rect */
    (void) panel_invert(ip->panel, &ip->value_rect, ip->color_index);
}


/***********************************************************************/
/* caret routines                              */
/***********************************************************************/


static void
deselect(panel)
    Panel_info     *panel;
{
    Item_info      *old_item = panel->caret;
    Text_info      *old_dp;

    if (old_item != NULL) {
	old_dp = TEXT_FROM_ITEM(old_item);
	old_dp->flags &= ~HASCARET;
	paint_caret(old_item, PIX_CLR);
    }
}



static          Panel_item
panel_setcaret(panel, ip, op)
    Panel_info     *panel;
    Item_info      *ip;
    int             op;		/* PIX_SET or PIX_CLR */
{
    Text_info      *dp = TEXT_FROM_ITEM(ip);

    if (ip == NULL || hidden(ip))
	return NULL;

    deselect(panel);
    panel->caret = ip;
    if (panel_seln(panel, SELN_CARET)->ip)
	panel_seln(panel, SELN_CARET)->ip = ip;
    dp->flags |= HASCARET;
    paint_caret(ip, op);
    return ITEM_PUBLIC(ip);
}


Xv_public       Panel_item
panel_advance_caret(client_panel)
    Panel           client_panel;
{
    Panel_info     *panel = PANEL_PRIVATE(client_panel);
    Item_info      *old_item;
    Text_info      *old_dp;

    old_item = panel->caret;

    if (!old_item)
	return (NULL);

    old_dp = TEXT_FROM_ITEM(old_item);
    (void) panel_setcaret(panel, old_dp->next, PIX_SET);

    return ITEM_PUBLIC(panel->caret);
}


Xv_public       Panel_item
panel_backup_caret(client_panel)
    Panel           client_panel;
{
    Panel_info     *panel = PANEL_PRIVATE(client_panel);
    register Item_info *old_ip;
    register Item_info *new_ip, *pre_ip;

    old_ip = panel->caret;

    if (!old_ip)
	return (NULL);

    /* find previous item by going forward in linked list of text items... */
    pre_ip = old_ip;
    new_ip = TEXT_FROM_ITEM(old_ip)->next;
    while (new_ip != old_ip) {
	pre_ip = new_ip;
	new_ip = TEXT_FROM_ITEM(new_ip)->next;
    }

    (void) panel_setcaret(panel, pre_ip, PIX_SET);

    return ITEM_PUBLIC(panel->caret);
}


/*
 * text_caret_invert inverts the type-in caret.
 */
static void
text_caret_invert(panel)
    register Panel_info *panel;
{
    if (!panel->caret)
	return;

    paint_caret(panel->caret, panel->caret_on ? PIX_CLR : PIX_SET);
}


/*
 * text_caret_on paints the type-in caret if on is true; otherwise xor's it.
 */
static void
text_caret_on(panel, on)
    Panel_info     *panel;
    int             on;
{
    if (!panel->caret)
	return;

    paint_caret(panel->caret, on ? PIX_SET : PIX_CLR);
}


/***********************************************************************/
/* panel_text_notify                                                   */
/***********************************************************************/

/* ARGSUSED */
Xv_public       Panel_setting
panel_text_notify(client_item, event)
    Panel_item      client_item;
    register Event *event;
{
    switch (event_action(event)) {
      case '\n':
      case '\r':
      case '\t':
	return (event_shift_is_down(event)) ? PANEL_PREVIOUS : PANEL_NEXT;

	/* always insert editting characters */
      case ACTION_ERASE_CHAR_BACKWARD:
      case ACTION_ERASE_CHAR_FORWARD:
      case ACTION_ERASE_WORD_BACKWARD:
      case ACTION_ERASE_WORD_FORWARD:
      case ACTION_ERASE_LINE_BACKWARD:
      case ACTION_ERASE_LINE_END:
	return (PANEL_INSERT);

      default:
#ifdef ecd_input
	if (event_is_iso(event))
	    return ((event_id(event) >= ISO_FIRST + 0xA0) ?
		    PANEL_INSERT : PANEL_NONE);
#endif
	return ((event_action(event) >= ' ' && event_action(event) <= '~') ?
		PANEL_INSERT : PANEL_NONE);
    }
}


/*
 * Hilite selection according to its rank.
 */
static void
text_seln_hilite(selection)
    register Panel_selection *selection;
{
    register Item_info *ip = selection->ip;
    Text_info      *dp = TEXT_FROM_ITEM(ip);
    Rect            rect;
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
    int             y;
    struct pr_size  size;
    Xv_Window       pw;

    rect = ip->value_rect;
    seln_rank = (selection->rank == SELN_PRIMARY) ? PRIMARY_SELN :
	SECONDARY_SELN;

    if (select_click_cnt[seln_rank] == 0)
	return;

    /* Highlight characters bounded by seln_first and seln_last */
    if (dp->seln_first[seln_rank] >= dp->first_char) {
	size = xv_pf_textwidth((dp->seln_first[seln_rank] - dp->first_char),
			       dp->font, &dp->value[dp->first_char]);
	rect.r_left += size.x;
    }
    if (dp->first_char)
	rect.r_left += panel_left_arrow_pr.pr_width;
    size = xv_pf_textwidth(
		   dp->seln_last[seln_rank] - dp->seln_first[seln_rank] + 1,
			   dp->font, &dp->value[dp->seln_first[seln_rank]]);
    rect.r_width = size.x;
    if (rect.r_width > dp->text_rect.r_width)
	rect.r_width = dp->text_rect.r_width;
    rect.r_height--;		/* don't disturb underlining */

    switch (selection->rank) {
      case SELN_PRIMARY:
	primary_seln_panel = ip->panel;	/* save panel */
	primary_seln_rect = rect;	/* save rectangle coordinates */
	primary_seln_first = dp->seln_first[seln_rank];
	primary_seln_last = dp->seln_last[seln_rank];
	seln_highlight = HL_INVERT;
	panel_invert(ip->panel, &rect, ip->color_index);
	break;

      case SELN_SECONDARY:
	secondary_seln_panel = ip->panel;	/* save panel */
	secondary_seln_rect = rect;	/* save rectangle coordinates */
	secondary_seln_first = dp->seln_first[seln_rank];
	secondary_seln_last = dp->seln_last[seln_rank];
	if (quick_move(ip->panel)) {
	    seln_highlight = HL_STRIKE_THRU;
	    y = rect.r_top + (rect.r_height / 2);
	} else {
	    seln_highlight = HL_UNDERLINE;
	    y = rect_bottom(&rect);
	}
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    xv_vector(pw, rect.r_left, y, rect.r_left + rect.r_width - 1, y,
		      ip->color_index < 0 ? PIX_SET :
			  PIX_SRC | PIX_COLOR(ip->color_index),
		      0);
	PANEL_END_EACH_PAINT_WINDOW
	    break;
    }
}


/* Dehighlight whatever was last highlighted */
Pkg_private void
panel_seln_dehilite(ip, rank)
    Item_info      *ip;
    Seln_rank       rank;
{
    Panel_info     *seln_panel = (Panel_info *) 0;
    Rect           *seln_rect_handle;
    Text_info      *dp = TEXT_FROM_ITEM(ip);
    int             seln_first, seln_last;
    char            save_seln_last_char;
    Xv_Window       pw;

    switch (rank) {
      case SELN_PRIMARY:
	seln_panel = primary_seln_panel;
	seln_rect_handle = &primary_seln_rect;
	seln_first = primary_seln_first;
	seln_last = primary_seln_last;
	primary_seln_panel = 0;	/* no longer valid */
	break;
      case SELN_SECONDARY:
	seln_panel = secondary_seln_panel;
	seln_rect_handle = &secondary_seln_rect;
	seln_first = secondary_seln_first;
	seln_last = secondary_seln_last;
	secondary_seln_panel = 0;	/* no longer valid */
	break;
    }
    if (seln_panel) {
	if (seln_highlight == HL_INVERT)
	    panel_invert(seln_panel, seln_rect_handle, ip->color_index);
	else if (ip && (seln_highlight == HL_UNDERLINE ||
			seln_highlight == HL_STRIKE_THRU)) {
	    if ((seln_first >= dp->first_char) &&
		(seln_last - seln_first + 1 <= dp->display_length)) {	/* ??? variable-width
		 * ??? *//* Entire selection is visible */
		panel_clear(ip->panel, seln_rect_handle);
		save_seln_last_char = dp->value[seln_last + 1];
		dp->value[seln_last + 1] = 0;	/* terminate substring */
		PANEL_EACH_PAINT_WINDOW(seln_panel, pw)
		    panel_paint_text(pw, ip->panel->gc, ip->color_index,
			dp->font,
			seln_rect_handle->r_left,
			seln_rect_handle->r_top + dp->font_home,
			dp->value + seln_first);
		PANEL_END_EACH_PAINT_WINDOW
		    dp->value[seln_last + 1] = save_seln_last_char;
	    } else
		paint_value(ip, PV_NO_HIGHLIGHT);
	}
	if (seln_highlight != HL_NONE)
	    seln_highlight = HL_NONE;
    }
}


static void
panel_multiclick_handler(ip, event, seln_rank)
    Item_info      *ip;
    Event          *event;
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int             left, right;/* left and right pixel coordinates of
				 * selection */
    struct pr_size  size;

    if (select_click_cnt[seln_rank] == 2) {
	/* Double click received: select displayed word containing caret */
	panel_find_word(dp, &dp->seln_first[seln_rank], &dp->seln_last[seln_rank]);
	/*
	 * Set caret offset to end of selected word or line closest to mouse
	 * position.
	 */
	size = xv_pf_textwidth(dp->seln_first[seln_rank] - dp->first_char, dp->font,
			       &dp->value[dp->first_char]);
	left = ip->value_rect.r_left + size.x;
	if (dp->first_char)
	    left += panel_left_arrow_pr.pr_width;
	size = xv_pf_textwidth(dp->seln_last[seln_rank] - dp->seln_first[seln_rank] + 1,
			   dp->font, &dp->value[dp->seln_first[seln_rank]]);
	right = left + size.x;
	if ((event->ie_locx - left) < (right - event->ie_locx))
	    event->ie_locx = left;	/* caret will move to left side */
	else
	    event->ie_locx = right;	/* caret will move to right side */
	dp->caret_offset = event->ie_locx - ip->value_rect.r_left;
	if (dp->first_char)
	    dp->caret_offset -= panel_left_arrow_pr.pr_width;
    } else {
	/*
	 * Triple click received: select entire line. Repaint the value, with
	 * the first or last character displayed, if the mouse is closer to
	 * the left or right margin, respectively.
	 */
	panel_select_line(ip, event, seln_rank);
	paint_value(ip, PV_HIGHLIGHT);
    }

}


static void
panel_select_line(ip, event, seln_rank)
    Item_info      *ip;
    Event          *event;
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int             max_caret = dp->text_rect.r_width;
    int             right;	/* horizontal offset of right margin of
				 * selection */
    int             left_offset;/* horizontal offset of cursor from left
				 * margin of text value */
    int             right_offset;	/* horizontal offset of cursor from
					 * right margin of text value */
    int             x, i;
    struct pr_size  size;

    dp->first_char = dp->seln_first[seln_rank] = 0;
    dp->last_char = dp->seln_last[seln_rank] = strlen(dp->value) - 1;
    right = dp->text_rect.r_width;
    size = xv_pf_textwidth(strlen(dp->value), dp->font, dp->value);
    if (size.x < right)
	right = size.x;
    left_offset = event_x(event) - dp->text_rect.r_left;
    right_offset = right + dp->text_rect.r_left - event_x(event);
    if (left_offset < right_offset) {
	/*
	 * Repaint (later) with first character displayed. dp->last_char =
	 * the last character in the string, starting from first_char, that
	 * can be fully displayed within the rectangle reserved for the text
	 * value string.
	 */
	dp->last_char = char_position(right, dp->font, dp->value) - 1;
	dp->caret_offset = 0;
    } else {
	/* Repaint (later) with last character displayed */
	x = 0;
	for (i = dp->last_char; (i >= 0) && (x < max_caret); i--)
	    x += dp->font->pf_char[dp->value[i]].pc_adv.x;
	if (i >= 0)
	    dp->first_char = i + 2;
	dp->caret_offset = ip->value_rect.r_width;
	if (dp->first_char)
	    dp->caret_offset -= panel_left_arrow_pr.pr_width;
    }
}


static void
panel_find_word(dp, first, last)
    register Text_info *dp;
    int            *first, *last;
{
    register int    index;

    /* Find beginning of word */
    index = *first;
    while ((index > dp->first_char) &&
	   (dp->value[index] != ' ') &&
	   (dp->value[index] != '/') &&
	   (dp->value[index] != '\t'))
	index--;
    if ((index != *first) &&
	((dp->value[index] == ' ') ||
	 (dp->value[index] == '/') ||
	 (dp->value[index] == '\t')))
	index++;		/* don't include word delimiter */
    *first = index;

    /*
     * Find end of word.  Note that on a single click, seln_last is set equal
     * to seln_first.
     */
    index = *last;
    while ((index < dp->last_char) &&
	   (dp->value[index] != ' ') &&
	   (dp->value[index] != '/') &&
	   (dp->value[index] != '\t'))
	index++;
    if ((index != *last) &&
	((dp->value[index] == ' ') ||
	 (dp->value[index] == '/') ||
	 (dp->value[index] == '\t')))
	index--;		/* don't include word delimiter */
    *last = index;
}


Pkg_private
panel_get_text_selection(ip, selection_string, selection_length, rank)
    Item_info      *ip;
    char          **selection_string;
    int            *selection_length;
    Seln_rank       rank;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */

    seln_rank = (rank == SELN_PRIMARY) ? PRIMARY_SELN : SECONDARY_SELN;
    if ((strlen(dp->value) == 0) || (select_click_cnt[seln_rank] == 0)) {
	*selection_length = 0;
	*selection_string = 0;
	return;
    }
    *selection_length = dp->seln_last[seln_rank] - dp->seln_first[seln_rank] + 1;
    *selection_string = dp->value + dp->seln_first[seln_rank];
}


Pkg_private void
panel_seln_delete(selection)
    Panel_selection *selection;
{
    Item_info      *ip = selection->ip;
    register Text_info *dp;
    register int    new;	/* new position of char to be moved */
    register int    old;	/* old position of char to be moved */
    int             last;	/* position of last valid char in value */
    int             val_change = 0;
    int             val_shift = 0;
    int             caret_shift = 0;
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
    struct pr_size  size;

    if (!ip)
	return;
    dp = TEXT_FROM_ITEM(ip);

    seln_rank = (selection->rank == SELN_PRIMARY) ?
	PRIMARY_SELN : SECONDARY_SELN;
    if (seln_rank == PRIMARY_SELN)
	delete_pending = FALSE;

    if (dp->flags & READ_ONLY)
	return;

    /*
     * Calculate number of character positions to move displayed value
     * (val_change) and number of character positions to move caret
     * (caret_shift).
     */
    val_change = dp->seln_first[seln_rank] - dp->seln_last[seln_rank] - 1;
    size = xv_pf_textwidth(dp->seln_last[seln_rank] - dp->first_char + 1, dp->font,
			   &dp->value[dp->first_char]);
    if (dp->caret_offset == size.x)	/* Is caret at right margin of
					 * selection? */
	caret_shift = val_change;	/* Yes: shift caret to left margin of
					 * selection */
    if (dp->first_char)
	caret_shift += ((dp->first_char - 1) > -val_change) ?
	    (-val_change) : (dp->first_char - 1);

    /* Delete the selected characters from the value buffer */
    new = dp->seln_first[seln_rank];
    old = dp->seln_last[seln_rank] + 1;
    last = strlen(dp->value);
    for (; new <= dp->stored_length - 1; new++, old++) {
	if (old > last)
	    dp->value[new] = 0;
	else
	    dp->value[new] = dp->value[old];
    }

    /*
     * Selection has been "used up": no mouse-left clicks or primary
     * selection pending.
     */
    select_click_cnt[seln_rank] = 0;
    if (selection->rank == SELN_PRIMARY)
	primary_seln_panel = 0;
    else
	secondary_seln_panel = 0;

    /* Repaint the value */
    if (ip->panel->caret == ip)
	paint_caret(ip, PIX_CLR);
    update_value_offset(ip, val_change, val_shift);
    paint_value(ip, PV_HIGHLIGHT);

    if (ip->panel->caret == ip) {
	/* Repaint the caret */
	update_caret_offset(ip, caret_shift);
	paint_caret(ip, PIX_SET);
    }
}

static int
panel_printable_char(code)
    int             code;	/* event action code */
{
    return ((code >= ' ' && code <= '~')
#ifdef ecd_input
	    || (code >= ISO_FIRST + 0xA0 && code <= ISO_FIRST + 0xFF)
#endif
	);
}


/*
 * return TRUE if tv is within multi-click timeout from global
 * last_click_time.
 */
static int
panel_dclick_is(tv)
    struct timeval *tv;
{
    int             delta;

    delta = (tv->tv_sec - last_click_time.tv_sec) * 1000;
    delta += tv->tv_usec / 1000;
    delta -= last_click_time.tv_usec / 1000;
    last_click_time = *tv;
    return (delta <= multi_click_timeout);
}


/* Find the position of the character to the left of caret_offset */
static int
char_position(caret_offset, font, str)
    int             caret_offset;
    Pixfont        *font;
    char           *str;
{
    int             i;		/* character string index */
    int             x = 0;	/* desired horizontal position */
    int             x_new = 0;	/* next horizontal position */

    for (i = 0; x_new <= caret_offset && str[i]; i++) {
	x = x_new;
	x_new += font->pf_char[str[i]].pc_adv.x;
    }

    /* Return character string index */
    if (x_new <= caret_offset)
	return (i);		/* cursor points to the right of the last
				 * character */
    else if (--i < 0)
	return (0);		/* cursor points to the left of the first
				 * character */
    else if ((caret_offset - x) > (x_new - x) / 2)
	return (i + 1);		/* cursor is in right half of char; point to
				 * next char */
    else
	return (i);		/* cursor is in left half of char; point to
				 * this char */
}


static void
panel_text_handle_event(client_object, event)
    Panel_item      client_object;	/* could be a Panel */
    register Event *event;
{
    Item_info      *ip = ITEM_PRIVATE(client_object);

    Xv_Window       window = event_window(event);
    update_text_rect(ip);
    panel_default_handle_event(client_object, event);
}

/*
 * Define rectangle containing the text (i.e., value rect less arrows)
 */
static void
update_text_rect(ip)
    Item_info      *ip;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);

    dp->text_rect = ip->value_rect;
    if (dp->first_char) {
	dp->text_rect.r_left += panel_left_arrow_pr.pr_width;
	dp->text_rect.r_width -= panel_left_arrow_pr.pr_width;
    }
    if (dp->last_char < strlen(dp->value) - 1)
	dp->text_rect.r_width -= panel_right_arrow_pr.pr_width;

}
