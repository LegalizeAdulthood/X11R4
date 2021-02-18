#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)panel.c 20.30 89/10/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/notify.h>


Pkg_private int panel_init();
Pkg_private int panel_destroy();
Pkg_private void panel_adjust_label_size();
static int      panel_layout();
static void     free_panel();



/* default timer value */
static struct itimerval PANEL_TIMER = {0, 500000, 0, 500000};

Attr_attribute  panel_context_key;


/* Note that this must be modified when the ops structure is modified. */
static struct panel_ops default_panel_ops = {
    panel_default_handle_event,	/* handle_event() */
    (void (*) ()) panel_nullproc,	/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    (void (*) ()) panel_nullproc,	/* cancel_preview() */
    (void (*) ()) panel_nullproc,	/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    (void (*) ()) panel_display,/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc/* layout() */
};


Pkg_private int
panel_init(parent, panel_public, avlist)
    Xv_Window       parent, panel_public;
    Attr_avlist     avlist;
{
    register Panel_info *panel;
    Xv_panel       *panel_object = (Xv_panel *) panel_public;
    extern Notify_value panel_rescale_event();
    Pixfont        *font;
    struct pr_size  size;

    /* initial context key if necessary */
    if (panel_context_key == (Attr_attribute) 0) {
	panel_context_key = xv_unique_key();
    }
    panel = xv_alloc(Panel_info);

    /* link to object */
    panel_object->private_data = (Xv_opaque) panel;
    panel->public_self = panel_public;

    panel->caret_on = FALSE;
    panel->caret_pr = &panel_ghost_caret_pr;
    panel->current_col_x = PANEL_ITEM_X_START;
    panel->extra_height = 1;
    panel->extra_width = 1;
    panel->event_proc = NULL;
    panel->h_margin = 4;
    panel->item_x = PANEL_ITEM_X_START;
    panel->item_x_offset = ITEM_X_GAP;
    panel->item_y = PANEL_ITEM_Y_START;
    panel->item_y_offset = ITEM_Y_GAP;
    panel->flags = IS_PANEL;
    panel->layout = PANEL_HORIZONTAL;
    panel->ops = &default_panel_ops;
    panel->repaint = PANEL_CLEAR;
    panel->status = ADJUST_IS_PENDING_DELETE;
    panel->timer_full = PANEL_TIMER;
    panel->v_margin = 4;

    /*
     * these are zero from the calloc() panel->max_item_y       = 0;
     */

    panel->layout_proc = (int (*) ()) xv_get(panel_public, WIN_LAYOUT_PROC);

    /*
     * Determine the height of a panel button containing a window mark, which
     * is the tallest panel item that can be created (excluding items with
     * pixrect images).  size.y will contain row height.
     */
    font = (Pixfont *) xv_get(panel_public, WIN_FONT);
    size = font->pf_defaultsize;
    panel_adjust_label_size(PANEL_BUTTON_ITEM, &size, TRUE, 0, panel->layout,
			    font);

    (void) xv_set(panel_public,
		  WIN_NOTIFY_SAFE_EVENT_PROC, panel_notify_panel_event,
		  WIN_NOTIFY_IMMEDIATE_EVENT_PROC, panel_notify_panel_event,
		  CANVAS_AUTO_EXPAND, TRUE,
		  CANVAS_AUTO_SHRINK, TRUE,
		  CANVAS_FIXED_IMAGE, FALSE,
		  CANVAS_REPAINT_PROC, panel_redisplay,
		  CANVAS_RETAINED, FALSE,
		  OPENWIN_SHOW_BORDERS, FALSE,
		  WIN_TOP_MARGIN, PANEL_ITEM_Y_START,
		  WIN_LEFT_MARGIN, PANEL_ITEM_X_START,
		  WIN_ROW_HEIGHT, size.y,
		  WIN_ROW_GAP, ITEM_Y_GAP,
		  WIN_LAYOUT_PROC, panel_layout,
		  XV_HELP_DATA, "xview:panel",
		  0);

    (void) win_set_no_focus(panel_public, TRUE);	/* panel sets own focus */
    return XV_OK;
}

Pkg_private int
panel_destroy(panel_public, status)
    Panel           panel_public;
    Destroy_status  status;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    Panel_item      item;

    if (status == DESTROY_PROCESS_DEATH)
	panel->destroying = TRUE;
    else if (status == DESTROY_CLEANUP) {
	/* unlink layout procs */
	xv_set(panel_public,
	       WIN_LAYOUT_PROC, panel->layout_proc,
	       0);
	panel_itimer_set(panel_public, NOTIFY_NO_ITIMER);
	panel->destroying = TRUE;
    }
    PANEL_EACH_ITEM(panel_public, item)
	if (xv_destroy_status(item, status) != XV_OK)
	return XV_ERROR;
    PANEL_END_EACH

	if (status == DESTROY_CLEANUP) {
	free_panel(panel);
    }
    return XV_OK;
}

static
panel_layout(panel_public, child, op, d1, d2, d3, d4, d5)
    Panel           panel_public;
    Xv_Window       child;
    Window_layout_op op;
    int             d1, d2, d3, d4, d5;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);

    switch (op) {
      case WIN_DESTROY:
	panel_unregister_view(panel, child);
	break;
      case WIN_CREATE:
	if ((int) xv_get(child, XV_IS_SUBTYPE_OF, PANEL_VIEW) == TRUE) {
	    panel_register_view(panel, child);
	}
      default:
	break;
    }

    if (panel->layout_proc != NULL) {
	return (panel->layout_proc(panel_public, child, op, d1, d2, d3, d4, d5));
    } else {
	return TRUE;
    }
}

static void
free_panel(panel)
    register Panel_info *panel;
{
    Panel_paint_window *pw;

    /* clean up with selection service */
    panel_seln_destroy(panel);

    /* free the ops vector storage */
    if (ops_set(panel)) {
	free((char *) panel->ops);
    }
    /* free storage for each paint window */
    while (panel->paint_window != NULL) {
	pw = panel->paint_window->next;
	free((char *) panel->paint_window);
	panel->paint_window = pw;
    }

    free((char *) panel);
}

static
panel_register_view(panel, view)
    Panel_info     *panel;
    Xv_Window       view;
{
    Panel_paint_window *paint_window_data;
    Xv_Window       pw;

    pw = xv_get(view, CANVAS_VIEW_PAINT_WINDOW);
    paint_window_data = xv_alloc(Panel_paint_window);
    paint_window_data->pw = pw;
    paint_window_data->view = view;
    paint_window_data->next = panel->paint_window;
    panel->paint_window = paint_window_data;

    (void) xv_set(pw,
		  XV_KEY_DATA, panel_context_key, panel,
		  0);
    (void) win_set_no_focus(pw, TRUE);	/* panel sets own focus */

    return (XV_OK);
}

static
panel_unregister_view(panel, view)
    Panel_info     *panel;
    Xv_Window       view;
{
    Panel_paint_window *pw_data, *lpw_data = NULL;

    for (pw_data = panel->paint_window; pw_data != NULL; pw_data = pw_data->next) {
	if (pw_data->view == view) {
	    if (lpw_data != NULL) {
		lpw_data->next = pw_data->next;
	    } else {
		panel->paint_window = pw_data->next;
	    }
	    xv_free(pw_data);
	}
    }

    return (XV_OK);
}
