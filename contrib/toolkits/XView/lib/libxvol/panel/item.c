#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)item.c 20.26 89/10/31";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>


Pkg_private int item_init();
Pkg_private int item_destroy();
static void     item_free();



/*
 * Note that this must be modified when the ops structure is modified.
 */
static Panel_ops null_ops = {
    (void (*) ()) panel_nullproc,	/* handle_event() */
    (void (*) ()) panel_nullproc,	/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    (void (*) ()) panel_nullproc,	/* cancel_preview() */
    (void (*) ()) panel_nullproc,	/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    (void (*) ()) panel_nullproc,	/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc/* layout() */
};


static short    menu_on_image[] = {
#include <images/panel_menu_on.pr>
};
static          mpr_static(panel_menu_on, 16, 16, 1, menu_on_image);

#define         FOREGROUND      -1


Pkg_private int
item_init(parent, item_public, avlist)
    Xv_Window       parent;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    Panel_info     *panel = PANEL_PRIVATE(parent);
    register Item_info *ip;
    Xv_item        *item_object = (Xv_item *) item_public;

    ip = xv_alloc(Item_info);

    /* link to object */
    item_object->private_data = (Xv_opaque) ip;
    ip->public_self = item_public;

    ip->panel = panel;
    ip->next = NULL;
    ip->flags = IS_ITEM | panel->flags;	/* get panel-wide attrs */
    ip->flags &= ~IS_PANEL;	/* this is not a panel */
    ip->flags &= ~OPS_SET;	/* ops vector is not altered */
    ip->layout = PANEL_HORIZONTAL;
    ip->notify = panel_nullproc;
    ip->ops = &null_ops;
    ip->repaint = panel->repaint;	/* default painting behavior */
    ip->color_index = -1;	/* -1 => use foreground color */

    /* determine the next default position */
    (void) panel_find_default_xy(panel);

    image_set_type(&ip->label, IM_STRING);
    image_set_string(&ip->label, panel_strsave(""));
    image_set_font(&ip->label, item_get_panel_font(ip));
    image_set_bold(&ip->label, label_bold_flag(ip));
    image_set_inverted(&ip->label, label_inverted_flag(ip));

    /* nothing is painted yet */
    rect_construct(&ip->painted_rect, 0, 0, 0, 0)
    /*
     * start the item, label and value rects at the default position
     */
	rect_construct(&ip->rect, ip->panel->item_x, ip->panel->item_y, 0, 0);
    ip->label_rect = ip->rect;
    ip->value_rect = ip->rect;

    /* set up the menu */
    ip->menu = NULL;		/* no menu */
#ifdef SUNVIEW1
    ip->menu_title = ip->label;
    image_set_string(&ip->menu_title, panel_strsave(""));
    ip->menu_choices = NULL;	/* no menu choices */
    ip->menu_last = -1;		/* no last slot */
    ip->menu_choices_bold = FALSE;	/* regular choice strings */
    ip->menu_values = NULL;	/* no values */
    ip->menu_type_pr = NULL;	/* no type indication yet */
    ip->menu_max_width = 0;	/* menu is zero wide */
#endif
    ip->menu_mark_on = &panel_menu_on;	/* for PANEL_MARKED menu feedback */
    ip->menu_mark_off = &panel_empty_pr;	/* for PANEL_MARKED menu
						 * feedback */
    ip->menu_mark_width =
	MAX(ip->menu_mark_on->pr_width, ip->menu_mark_off->pr_width);
    ip->menu_mark_height =
	MAX(ip->menu_mark_on->pr_height, ip->menu_mark_off->pr_height);

    if (panel->event_proc) {
	Panel_ops      *new_ops;

	if (!ops_set(ip)) {
	    new_ops = xv_alloc(Panel_ops);
	    *new_ops = *ip->ops;
	    ip->ops = new_ops;
	    ip->flags |= OPS_SET;
	}
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    }
#ifdef SUNVIEW1
    /* done in xv_set() */

    /* set the menu attributes */
    (void) panel_set_menu(ip, avlist);

    /* set the ops vector attributes */
    (void) panel_set_ops(ip, avlist);
#endif

    return XV_OK;
}


Pkg_private int
item_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    Item_info      *item = ITEM_PRIVATE(item_public);
    register Panel_info *panel = item->panel;

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    /* if the panel isn't going away clear the item */
    if (!panel->destroying) {
	if (item == panel->caret) {
	    panel_caret_on(panel, FALSE);
	}
	panel_clear_item(item);
    }
    panel_unlink(item, TRUE);	/* unlink is part of a destroy */

    item_free(item);
    return XV_OK;
}


static void
item_free(ip)
    register Item_info *ip;
{

    /* free the label storage */
    panel_free_image(&ip->label);

    /* decrement the menu ref count, if any */
    if (ip->menu)
	xv_set(ip->menu, XV_DECREMENT_REF_COUNT, 0);

    /* free the menu storage */
    (void) panel_free_menu(ip);

    /* free the ops vector storage */
    if (ops_set(ip))
	free((char *) ip->ops);

    free((char *) ip);
}
