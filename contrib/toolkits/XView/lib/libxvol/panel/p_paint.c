#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_paint.c 20.22 89/10/24 Copyr 1984 Sun Micro";
#endif
#endif

/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */

#include <xview_private/panel_impl.h>
#include <xview/pixwin.h>

static void     panel_repaint_background();
Pkg_private void panel_redisplay_item();


Pkg_private void
panel_redisplay(panel_public, pw, repaint_area)
    Panel           panel_public;
    Pixwin         *pw;
    Rectlist       *repaint_area;
{
    register Panel_info *panel = PANEL_PRIVATE(panel_public);
    register Item_info *ip;

    if (panel->repaint_proc != NULL) {
	panel->repaint_proc(panel_public, pw, repaint_area);
    }
    if ((int) xv_get(panel_public, XV_Y) == 0 &&
	xv_get(panel_public, WIN_VERTICAL_SCROLLBAR) == NULL &&
	xv_get(panel_public, WIN_HORIZONTAL_SCROLLBAR) == NULL) {
	xv_vector(pw, 0, 0,
		  xv_get(panel_public, XV_WIDTH), 0, PIX_SET, 0);
    }
    /*
     * see which items intersect the rect lists rects and paint them
     */
    for (ip = panel->items; ip; ip = ip->next) {
	if (!hidden(ip)) {
	    /* only paint item if intersects with repaint area */
	    if (rl_rectintersects(&ip->rect, repaint_area)) {
		(void) panel_paint_item(ip, FALSE);
	    }
	}
    }

    /* We don't turn the caret off and on here because any  */
    /* painting will be clipped away from the damaged region */
    /* if the caret partially overlaps the damaged area */
    /* this off/on painting would result in a partial ghost */
    /* Instead we only paint the caret on knowing that the */
    /* damaged area is always cleared and we will only be */
    /* painting into the damaged area. We set panel->caret_on */
    /* to FALSE because the panel_caret_on routine has an */
    /* internal check which does nothing if you ask to turn on */
    /* an already turned on caret */
    if (panel->caret_on == TRUE) {
	panel->caret_on = FALSE;
	panel_caret_on(panel, TRUE);
    }
    panel->status |= PANEL_PAINTED;
}


Pkg_private void
panel_display(panel, flag)
    register Panel_info *panel;
    Panel_setting   flag;
{
    register Item_info *ip;
    short           caret_was_on = FALSE;
    Rect            rect, *view_rect;
    Xv_Window       pw;


    if (panel->caret_on) {
	caret_was_on = TRUE;
	panel_caret_on(panel, FALSE);
    }
    /* clear if needed */
    if (flag == PANEL_CLEAR) {
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    view_rect = viewable_rect(panel, pw);
	(void) pw_writebackground(pw,
				  view_rect->r_left, view_rect->r_top,
			  view_rect->r_width, view_rect->r_height, PIX_CLR);
	panel_repaint_background(panel, pw, view_rect);
	PANEL_END_EACH_PAINT_WINDOW

    } else {
	/* paint each hidden item -- only if haven't cleared */
	for (ip = panel->items; ip; ip = ip->next) {
	    if (hidden(ip)) {
		panel_redisplay_item(ip, PANEL_NO_CLEAR, FALSE);
	    }
	}
    }

    /* paint each non-hidden item */

    for (ip = panel->items; ip; ip = ip->next) {
	if (!hidden(ip)) {
	    /*
	     * only paint the item if it intersects with the subwindow
	     * pixwin.
	     */
	    rect = ip->rect;
	    if (rect_intersectsrect(&rect, view_rect)) {
		(void) panel_paint_item(ip, FALSE);
	    }
	}
    }

    if (caret_was_on)
	panel_caret_on(panel, TRUE);
}


/*****************************************************************************/
/* panel_paint()                                                             */
/* calls the painting routine for panels or items, as appropriate.           */
/*****************************************************************************/

Xv_public
panel_paint(client_object, flag)
    Panel           client_object;
    Panel_setting   flag;
{
    Panel_info     *object = PANEL_PRIVATE(client_object);
    short           caret_was_on = FALSE;
    Panel_info     *panel;

    if (!object || (flag != PANEL_CLEAR && flag != PANEL_NO_CLEAR))
	return NULL;

    panel = is_panel(object) ? object : ((Item_info *) object)->panel;

    if (panel->caret_on) {
	caret_was_on = TRUE;
	panel_caret_on(panel, FALSE);
    }
    if (is_panel(object)) {
	(*object->ops->paint) (object, flag);
    } else {
	/*
	 * This is a hack to allow pre & post painting actions for all items.
	 */
	panel_redisplay_item((Item_info *) object, flag, TRUE);
    }

    if (caret_was_on || ((Item_info *) object) == panel->caret)
	panel_caret_on(panel, TRUE);
    return 1;
}


Pkg_private void
panel_redisplay_item(ip, flag, paint_client)
    register Item_info *ip;
    Panel_setting   flag;
    int             paint_client;
{

    if (flag == PANEL_NONE)
	return;

    /* only clear if specified or hidden */
    if (flag == PANEL_CLEAR || hidden(ip)) {
	/* clear the previous painted item */
	panel_clear_item(ip);
    }
    /* call paint */
    panel_paint_item(ip, paint_client);

}


Pkg_private
panel_paint_item(ip, paint_client)
    register Item_info *ip;
    int             paint_client;
{
    register Panel_info *panel = ip->panel;
    Xv_Window       pw;

    if (!hidden(ip)) {
	Rect            rect, *view_rect;

	/* don't lock or paint unless the item is visible */
	rect = ip->rect;
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    view_rect = viewable_rect(panel, pw);
	if (rect_intersectsrect(&rect, view_rect)) {
	    /* paint */
	    (*ip->ops->paint) (ip);
	    ip->painted_rect = ip->rect;
	}
	PANEL_END_EACH_PAINT_WINDOW
    }
    return (XV_OK);
}


Pkg_private void
panel_clear_item(ip)
    register Item_info *ip;
{
    register Panel_info *panel = ip->panel;
    Xv_Window       pw;

    if (!hidden(ip) && !rect_isnull(&ip->painted_rect)) {
	/* Item is visible and was not previously cleared */
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    (void) pw_writebackground(pw,
			    ip->painted_rect.r_left, ip->painted_rect.r_top,
	      ip->painted_rect.r_width, ip->painted_rect.r_height, PIX_CLR);

	/* call client to repaint */
	panel_repaint_background(panel, pw, &ip->painted_rect);
	PANEL_END_EACH_PAINT_WINDOW

	/* nothing is painted */
	    rect_construct(&ip->painted_rect, 0, 0, 0, 0);
    }
}


static void
panel_repaint_background(panel, pw, rect)
    register Panel_info *panel;
    Xv_Window       pw;
    Rect           *rect;
{
    Rectlist        rl, *current_rl;
    Rect            clip_rect;
    extern Rectlist *win_get_damage();

    if (!rect_isnull(rect) && panel->repaint_proc != NULL) {

	clip_rect = *rect;

	rl_initwithrect(&clip_rect, &rl);

	/* get current damage */
	current_rl = win_get_damage(pw);
	/* damage the rectangle */
	win_set_clip(pw, &rl);
	/* call client's repaint proc */
	panel->repaint_proc(PANEL_PUBLIC(panel), pw, &rl);
	win_set_clip(pw, current_rl);

	rl_free(&rl);
    }
}
