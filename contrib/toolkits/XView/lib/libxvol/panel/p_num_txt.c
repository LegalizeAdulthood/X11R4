#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_num_txt.c 20.7 89/10/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/defaults.h>
#include <xview/screen.h>
#include <xview/pixwin.h>
#include <X11/X.h>
#include <ctype.h>

/*
 * increment/decrement buttons
 */
#define PANEL_NUM_UP_BUTTON    &xv_panel_num_up_button_pkg
extern Xv_pkg xv_panel_num_up_button_pkg;

#define PANEL_NUM_DOWN_BUTTON  &xv_panel_num_down_button_pkg
extern Xv_pkg xv_panel_num_down_button_pkg;


/*
 * offset of increment button from text field's right edge of rect
 */
#define BUTTON_OFFSET		5


/*
 * increment/decrement buttons and text field  point back to numeric text
 * field using slot 0 of XV_KEY_DATA
 */
#define NUM_TEXT_PTR            XV_KEY_DATA, 0


#define NUM_TEXT_PRIVATE(item)	\
	XV_PRIVATE(Num_text_info, Xv_panel_num_text, item)
#define NUM_TEXT_PUBLIC(item)	XV_PUBLIC(item)

#define	NUM_TEXT_FROM_ITEM(ip)	NUM_TEXT_PRIVATE(ITEM_PUBLIC(ip))


Pkg_private int 	panel_num_text_init();
Pkg_private Xv_opaque 	panel_num_text_set_avlist();
Pkg_private Xv_opaque 	panel_num_text_get_attr();
Pkg_private int 	panel_num_text_destroy();

/*
 * BUG: see paint function below for explanation
 */
static void paint();

static struct panel_ops ops = {
    (void (*) ()) panel_nullproc,		/* handle_event() */
    (void (*) ()) panel_nullproc,		/* begin_preview() */
    (void (*) ()) panel_nullproc,		/* update_preview() */
    (void (*) ()) panel_nullproc,		/* cancel_preview() */
    (void (*) ()) panel_nullproc,		/* accept_preview() */
    (void (*) ()) panel_nullproc,		/* accept_menu() */
    (void (*) ()) panel_nullproc,		/* accept_key() */
    (void (*) ()) paint,			/* paint() */
    (void (*) ()) panel_nullproc,		/* remove() */
    (void (*) ()) panel_nullproc,		/* restore() */
    (void (*) ()) panel_nullproc		/* layout() */
};


/***********************************************************************/
/* data area		                                               */
/***********************************************************************/

typedef struct {
    Panel_item public_self;	/* back pointer to object */
    Panel_setting notify_level;	/* NONE, SPECIFIED, NON_PRINTABLE, ALL */
    char *terminators;
    int   min_value;
    int   max_value;
    Panel_item text_field;
    Panel_item up, down;
    Bool  buttons_inactive;	/* PANEL_READ_ONLY || PANEL_INACTIVE   */
}     Num_text_info;


static void
check_dimming(dp)
    register Num_text_info *dp;
{
    int   value;

    if (dp->buttons_inactive)
	return;

    value = (int) xv_get(dp->public_self, PANEL_VALUE);

    if (value == dp->min_value)
	xv_set(dp->down, PANEL_INACTIVE, TRUE, 0);

    else
	xv_set(dp->down, PANEL_INACTIVE, FALSE, 0);

    if (value == dp->max_value)
	xv_set(dp->up, PANEL_INACTIVE, TRUE, 0);

    else
	xv_set(dp->up, PANEL_INACTIVE, FALSE, 0);
}


static void
validate(dp)
    Num_text_info *dp;
{
    int   value = (int) xv_get(dp->public_self, PANEL_VALUE);
    Bool  change = FALSE;

    if (value > dp->max_value) {
	value = dp->max_value;
	change = TRUE;
    }
    if (value < dp->min_value) {
	value = dp->min_value;
	change = TRUE;
    }
    if (change) {
	xv_set(dp->public_self, PANEL_VALUE, value, 0);
	check_dimming(dp);
    }
}



static void
set_value(dp, new_value)
    register Num_text_info *dp;
    int   new_value;
{
    char  buffer[20];

    if (new_value > dp->max_value)
	new_value = dp->max_value;

    if (new_value < dp->min_value)
	new_value = dp->min_value;

    sprintf(buffer, "%d%c", new_value, '\0');
    xv_set(dp->text_field, PANEL_VALUE, buffer, 0);

    check_dimming(dp);
}

static Bool
notify_user(dp, event)
    Num_text_info *dp;
    Event *event;
{
    switch (dp->notify_level) {
      case PANEL_ALL:
      case PANEL_NONE:
      default:
	return TRUE;

      case PANEL_NON_PRINTABLE:
	return isprint(event_action(event));

      case PANEL_SPECIFIED:
	return index(dp->terminators, event_action(event)) != 0;
    }
}


static Panel_setting
text_notify_proc(text_field, event)
    Panel_item text_field;
    Event *event;
{
    Panel_item num_text = (Panel_item) xv_get(text_field,
					      NUM_TEXT_PTR);
    register Item_info *ip = ITEM_PRIVATE(num_text);
    register Num_text_info *dp = NUM_TEXT_PRIVATE(num_text);

    if (isprint(event_action(event)) && !isdigit(event_action(event))) {
	window_bell(xv_get(text_field, XV_OWNER));
	return PANEL_NONE;
    }
    /*
     * validate text field value before calling user
     */
    switch (event_action(event)) {
      case '\n':
      case '\t':
      case '\r':
	validate(dp);
    }

    if (notify_user(dp, event))
	return (Panel_setting) (*ip->notify) (ITEM_PUBLIC(ip), event);

    else
	return panel_text_notify(text_field, event);
}

/*ARGSUSED*/
static void
up_proc(item, event)
    Panel_item item;
    Event *event;
{
    Panel_item num_text = (Panel_item) xv_get(item, NUM_TEXT_PTR);

    (void)xv_set(num_text,
	   PANEL_VALUE, (int) xv_get(num_text, PANEL_VALUE) + 1,
	   0);
}

/*ARGSUSED*/
static void
down_proc(item, event)
    Panel_item item;
    Event *event;
{
    Panel_item num_text = (Panel_item) xv_get(item, NUM_TEXT_PTR);

    (void)xv_set(num_text,
	   PANEL_VALUE, (int) xv_get(num_text, PANEL_VALUE) - 1,
	   0);
}

static void
set_positions(dp)
    Num_text_info *dp;
{
    register Rect *r = (Rect *) xv_get(dp->text_field, PANEL_ITEM_RECT);

    xv_set(dp->up,
	   PANEL_ITEM_X, r->r_left + r->r_width + BUTTON_OFFSET,
	   PANEL_ITEM_Y, r->r_top,
	   0);

    r = (Rect *) xv_get(dp->up, PANEL_ITEM_RECT);

    xv_set(dp->down,
	   PANEL_ITEM_X, r->r_left + r->r_width,
	   PANEL_ITEM_Y, r->r_top,
	   0);
}


/*ARGSUSED*/
Pkg_private int
panel_num_text_init(panel_public, item_public, avlist)
    Panel panel_public;
    Panel_item item_public;
    Attr_avlist avlist;
{
    Panel_info *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip = ITEM_PRIVATE(item_public);
    Xv_panel_num_text *item_object = (Xv_panel_num_text *) item_public;
    Num_text_info *dp = xv_alloc(Num_text_info);

    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static text ops vector */

    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ())panel->event_proc;

    ip->item_type = PANEL_NUMERIC_TEXT_ITEM;

    dp->min_value = 0;
    dp->max_value = 100;
    dp->buttons_inactive = FALSE;

    dp->notify_level = PANEL_SPECIFIED;
    dp->terminators = panel_strsave("\n\r\t");
    if (ip->notify == panel_nullproc)
	ip->notify = (int (*) ()) panel_text_notify;

    dp->text_field = xv_create(panel_public, PANEL_TEXT,
			       PANEL_VALUE, "0",
			       PANEL_NOTIFY_LEVEL, PANEL_ALL,
			       PANEL_NOTIFY_PROC, text_notify_proc,
			       PANEL_VALUE_DISPLAY_LENGTH, 12,
			       PANEL_VALUE_STORED_LENGTH, 12,
			       NUM_TEXT_PTR, item_public,
			       0);

    dp->up = xv_create(panel_public, PANEL_NUM_UP_BUTTON,
		       PANEL_NOTIFY_PROC, up_proc,
		       NUM_TEXT_PTR, item_public,
		       0);

    dp->down = xv_create(panel_public, PANEL_NUM_DOWN_BUTTON,
			 PANEL_NOTIFY_PROC, down_proc,
			 NUM_TEXT_PTR, item_public,
			 0);

    set_positions(dp);
    set_value(dp, 0);

    (void) panel_append(ip);

    return XV_OK;
}

Pkg_private Xv_opaque
panel_num_text_set_avlist(item_public, avlist)
    Panel_item item_public;
    register Attr_avlist avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    register Num_text_info *dp = NUM_TEXT_PRIVATE(item_public);
    register Panel_attr attr;
    Panel_info *panel = ip->panel;
    Bool  pos_change = FALSE;
    Bool  range_change = FALSE;
    Bool  check_for_dimming = FALSE;
    Bool  value_change = FALSE;
    Bool  xv_end_create = FALSE;
    int   new_value;
    int   value;
    Rect *text_rect;
    Rect *up_rect;
    Rect *down_rect;

    /*
     * if a client has called panel_item_parent this item may not have a
     * parent -- do nothing in this case
     */
    if (panel == NULL)
	return ((Xv_opaque) XV_ERROR);

    while (attr = (Panel_attribute) * avlist++) {
	switch (attr) {
	  case PANEL_LABEL_STRING:
	  case XV_X:
	  case XV_Y:
	  case PANEL_LABEL_X:
	  case PANEL_LABEL_Y:
	  case PANEL_VALUE_X:
	  case PANEL_VALUE_Y:
	  case PANEL_ITEM_X:
	  case PANEL_ITEM_Y:
	  case PANEL_NEXT_ROW:
	  case PANEL_NEXT_COL:
	  case PANEL_VALUE_FONT:
	  case PANEL_VALUE_STORED_LENGTH:
	  case PANEL_VALUE_DISPLAY_LENGTH:
	  case PANEL_LABEL_WIDTH:
	  case PANEL_VALUE_UNDERLINED:
	    xv_set(dp->text_field, attr, (char *) *avlist++, 0);
	    pos_change = TRUE;
	    break;

	  case PANEL_ITEM_COLOR:
	    xv_set(dp->text_field, PANEL_ITEM_COLOR, (char *) *avlist, 0);
	    xv_set(dp->up, PANEL_ITEM_COLOR, (char *) *avlist, 0);
	    xv_set(dp->down, PANEL_ITEM_COLOR, (char *) *avlist++, 0);
	    break;

	  case PANEL_READ_ONLY:
	    value = (int) *avlist++;
	    xv_set(dp->text_field, PANEL_READ_ONLY, value, 0);
	    xv_set(dp->up, PANEL_INACTIVE, value, 0);
	    xv_set(dp->down, PANEL_INACTIVE, value, 0);
	    dp->buttons_inactive = value;
	    if (!value)
		check_for_dimming = TRUE;
	    break;

	  case PANEL_INACTIVE:
	    value = (int) *avlist++;
	    xv_set(dp->text_field, PANEL_INACTIVE, value, 0);
	    xv_set(dp->up, PANEL_INACTIVE, value, 0);
	    xv_set(dp->down, PANEL_INACTIVE, value, 0);
	    dp->buttons_inactive = value;
	    if (!value)
		check_for_dimming = TRUE;
	    break;

	  case PANEL_MASK_CHAR:
	    value = (char) *avlist++;
	    xv_set(dp->text_field, PANEL_MASK_CHAR, value, 0);
	    value = (value != '\0');
	    xv_set(dp->up, PANEL_INACTIVE, value, 0);
	    xv_set(dp->down, PANEL_INACTIVE, value, 0);
	    dp->buttons_inactive = value;
	    if (!value)
		check_for_dimming = TRUE;
	    break;

	  case XV_SHOW:
	    value = (Bool) * avlist++;
	    xv_set(dp->text_field, XV_SHOW, value, 0);
	    xv_set(dp->up, XV_SHOW, value, 0);
	    xv_set(dp->down, XV_SHOW, value, 0);
	    break;

	  case PANEL_VALUE:
	    value_change = TRUE;
	    new_value = (int) *avlist++;
	    break;

	  case PANEL_MIN_VALUE:
	    dp->min_value = (int) *avlist++;
	    range_change = TRUE;
	    break;

	  case PANEL_MAX_VALUE:
	    dp->max_value = (int) *avlist++;
	    range_change = TRUE;
	    break;

	  case PANEL_NOTIFY_LEVEL:
	    dp->notify_level = (Panel_setting) * avlist++;
	    break;

	  case PANEL_NOTIFY_STRING:
	    if (dp->terminators)
		free(dp->terminators);
	    dp->terminators = panel_strsave((char *) *avlist++);
	    break;

	  case XV_END_CREATE:
	    xv_end_create = TRUE;
	    break;

	  default:
	    avlist = attr_skip(attr, avlist);
	    break;
	}
    }

    if (pos_change)
	set_positions(dp);

    if (range_change && dp->max_value < dp->min_value)
	dp->max_value = dp->min_value;

    if (value_change)
	set_value(dp, new_value);

    else if (range_change)
	set_value(dp, (int) xv_get(item_public, PANEL_VALUE));

    if (check_for_dimming)
	check_dimming(dp);

    if (xv_end_create || (pos_change & created(ip))) {
	text_rect = (Rect *) xv_get(dp->text_field, PANEL_ITEM_RECT);
	up_rect = (Rect *) xv_get(dp->up, PANEL_ITEM_RECT);
	down_rect = (Rect *) xv_get(dp->down, PANEL_ITEM_RECT);
	ip->rect = panel_enclosing_rect(text_rect, up_rect);
	ip->rect = panel_enclosing_rect(&ip->rect, down_rect);
    }

    return XV_OK;
}


/***********************************************************************/
/* get_attr                                                            */
/* returns the current value of an attribute for the text item.        */
/***********************************************************************/

/*ARGSUSED*/
Pkg_private Xv_opaque
panel_num_text_get_attr(item_public, status, which_attr, valist)
    Panel_item item_public;
    int  *status;
    register Panel_attr which_attr;
    va_list valist;
{
    register Num_text_info *dp = NUM_TEXT_PRIVATE(item_public);
    int   result;
    Bool  changed = FALSE;

    switch (which_attr) {
      case PANEL_VALUE:
	/*
	 * validate text field value before calling user
	 */
	sscanf((char *) xv_get(dp->text_field, PANEL_VALUE), "%d", &result);
	if (result > dp->max_value) {
	    result = dp->max_value;
	    changed = TRUE;
	}
	if (result < dp->min_value) {
	    result = dp->min_value;
	    changed = TRUE;
	}
	if (changed) {
	    xv_set(item_public, PANEL_VALUE, result, 0);
	    check_dimming(dp);
	}
	return (Xv_opaque) result;

      case PANEL_MIN_VALUE:
	return (Xv_opaque) dp->min_value;

      case PANEL_MAX_VALUE:
	return (Xv_opaque) dp->max_value;

      case PANEL_NOTIFY_LEVEL:
	return (Xv_opaque) dp->notify_level;

      case PANEL_NOTIFY_STRING:
	return (Xv_opaque) dp->terminators;

      case PANEL_LABEL_STRING:
      case XV_X:
      case XV_Y:
      case PANEL_LABEL_X:
      case PANEL_LABEL_Y:
      case PANEL_ITEM_X:
      case PANEL_ITEM_Y:
      case PANEL_VALUE_FONT:
      case PANEL_VALUE_STORED_LENGTH:
      case PANEL_VALUE_DISPLAY_LENGTH:
      case PANEL_LABEL_WIDTH:
      case PANEL_VALUE_UNDERLINED:
      case PANEL_READ_ONLY:
      case PANEL_MASK_CHAR:
	return xv_get(dp->text_field, which_attr);

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}


static void
paint(ip)
    Item_info *ip;
{
    Num_text_info *dp = NUM_TEXT_FROM_ITEM(ip);

    panel_paint(dp->text_field, PANEL_CLEAR);
    panel_paint(dp->up, PANEL_CLEAR);
    panel_paint(dp->down, PANEL_CLEAR);
}


/***********************************************************************/
/* destroy                                                             */
/***********************************************************************/

Pkg_private int
panel_num_text_destroy(item_public, status)
    Panel_item item_public;
    Destroy_status status;
{
    Num_text_info *dp = NUM_TEXT_PRIVATE(item_public);

    if (status == DESTROY_CHECKING)
	return XV_OK;

    xv_destroy(dp->text_field);
    xv_destroy(dp->up);
    xv_destroy(dp->down);

    free((char *) dp);

    return XV_OK;
}
