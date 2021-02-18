#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)notice.c 20.68 89/11/25";
#endif
#endif


/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */


#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <xview/sun.h>
#include <xview/base.h>
#include <xview/defaults.h>
#include <xview/window_hs.h>
#include <xview/font.h>
#include <xview/fullscreen.h>
#include <xview/openmenu.h>
#include <xview/pixwin.h>
#include <xview/server.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/screen.h>
#include <xview/win_input.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>
#include <xview_private/xv_color.h>
#include <xview/notice.h>
#include <xview_private/noticeimpl.h>

/*
 * --------------------------- Shadow Stuff -------------------------
 */

unsigned short  my_shadow_data[16] = {
    0x8000,
    0x1000,
    0x0200,
    0x4000,
    0x0800,
    0x0100,
    0x2000,
    0x0400,
#if 0
    0x8000,
    0x0200,
    0x2000,
    0x0800,
    0x4000,
    0x0100,
    0x1000,
    0x0400,
#endif
};

mpr_static(my_shadow_patch, 8, 8, 1, my_shadow_data);

/*
 * --------------------------- Cursor Stuff -------------------------
 */

static short    notice_cursor_data[16] = {
    0xC000, 0xF000, 0x7C00, 0x7F00, 0x3FC0, 0x3FF0, 0x1FF0, 0x1F80,
    0x0FC0, 0x0EE0, 0x0670, 0x0638, 0x001C, 0x000E, 0x0007, 0x0003
};
static          mpr_static(notice_cursor_mpr, 16, 16, 1, notice_cursor_data);
Xv_Cursor       notice_cursor;

static short    notice_gray17_data[] = {
#include <images/square_17.pr>
};

mpr_static(notice_gray17_patch, 16, 16, 1, notice_gray17_data);

static short    notice_gray25_data[] = {
#include <images/square_25.pr>
};

mpr_static(notice_gray25_patch, 16, 16, 1, notice_gray25_data);

static short    notice_gray75_data[] = {
#include <images/square_75.pr>
};

mpr_static(notice_gray75_patch, 16, 16, 1, notice_gray75_data);

extern void     xv_help_save_image();

extern struct pr_size xv_pf_textwidth();

static char *notice_default_button_str = "Confirm";

/*
 * --------------------------- DISPLAY PROCS ------------------------
 */

static int      notice_show();
static void	notice_add_default_button();
static void     notice_get_notice_size();
static void     notice_invert_button();
static void     notice_drawbox();
static void     notice_layout();
static void     notice_build_button();
static void     notice_preview_notice_font();
static void     notice_set_avlist();
static void     notice_defaults();
static void     notice_add_button_to_list();
static int      notice_text_width();
static void     notice_paint_button();
static int      notice_button_width();
static void     notice_get_button_pin_points();
static notice_buttons_handle notice_button_for_event();
static int      notice_offset_from_baseline();
static notice_buttons_handle create_button_struct();
static void     free_button_structs();
static void     notice_copy_event();
static void     notice_do_bell();

/*
 * --------------------------- STATICS ------------------------------
 */

static int      left_marg = 5;
static int      right_marg = 5;
static int      top_marg = 5;
static int      bottom_marg = 10;
static int      row_gap = 4;
static int      min_button_gap = 5;

#define BORDER			3	/* white space surounding text */
#define SHADOW			6	/* thickness of shadow         */
#define NOTICE_INVERT_BUTTON	1
#define NOTICE_NORMAL_BUTTON	0
#define NOTICE_BUTTON_GAP      10	/* same as ITEM_X_GAP in item_impl.h */

/*
 * --------------------------- Globals ------------------------------
 */

static notice_handle saved_notice;
static int      default_beeps;
static int      notice_use_audible_bell;
static int      notice_jump_cursor;
static Rect     notice_screen_rect;
static Display *notice_display;
static Xv_Window notice_window;
static int      leftoff, topoff;
static int      max_msg_width;
static Fullscreen fs;
static int      button_gap, left_indent;
static Xv_Font  defpf;

/* box to which cursor is contrained */
int             leftbox, rightbox, bottombox;
int             notice_debug = 0;


/*
 * ----------------------- Public Interface -------------------------
 */

/* VARARGS3 */
Xv_public int
notice_prompt(client_window, event, va_alist)
    Xv_Window       client_window;
    Event          *event;
va_dcl
{
    va_list         valist;
    int             result;
    register Xv_Drawable_info *info;

    if (!client_window) {
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "NULL parent window passed to notice_show(). Not allowed.",
		 XV_ERROR_PKG_NAME, "Notice",
		 0);
	return (NOTICE_FAILED);
    }
    DRAWABLE_INFO_MACRO(client_window, info);
    notice_display = xv_display(info);
#ifndef DISABLE_NOTICES
    for (;;) {
	va_start(valist);
	result = notice_show(client_window, event, valist);
	va_end(valist);
	if (result != NOTICE_HELP)
	    break;
	xv_help_render(client_window, saved_notice->help_data, event);
	XSync(notice_display, 0);
	do {
	    /*
	     * process pending enqueued events
	     */
	    xv_scheduler_internal();
	    XSync(notice_display, 0);
	} while (QLength(notice_display) != 0);
    }

    return (result);
#else
    return (NOTICE_YES);
#endif
}

/*
 * ----------------------PRIVATE PROCS-------------------------------
 */

static int
notice_show(client_window, event, valist)
    Xv_Window       client_window;
    Event          *event;
    va_list         valist;
{
    register notice_handle 	notice;
    register Xv_Drawable_info 	*info;
    Event           		ie;
    Notice_attribute 		avlist[ATTR_STANDARD_SIZE];
    int             		result;
    struct rect     		rect;
    Xv_Colormap_Seg     	*cms = NULL, *screen_find_cms();

    notice_buttons_handle button;
    notice_buttons_handle current_button = NULL;
    int             is_highlighted = FALSE;
    int             old_mousex, old_mousey;
    unsigned short  this_event;
    int             ok_to_toggle_buttons = FALSE;
    int             x, y, quadrant, left_placement, top_placement;
    Xv_opaque       root_window;
    int             font_scale = FONT_NO_SCALE;
    Xv_Font         client_font, my_font;
    Rect           *old_mouse_position;
    unsigned short  trigger, this_id;
    Inputmask       im;

    DRAWABLE_INFO_MACRO(client_window, info);
    notice_display = xv_display(info);

    input_imnull(&im);
    /*
     * Set im to be used in xv_input_readevent
     */
    win_setinputcodebit(&im, MS_LEFT);
    win_setinputcodebit(&im, MS_MIDDLE);
    win_setinputcodebit(&im, MS_RIGHT);
    win_setinputcodebit(&im, (int) WIN_ASCII_EVENTS);
    win_setinputcodebit(&im, (int) WIN_UP_ASCII_EVENTS);
    win_setinputcodebit(&im, LOC_WINENTER);
    win_setinputcodebit(&im, LOC_WINEXIT);
    win_setinputcodebit(&im, LOC_DRAG);
    win_setinputcodebit(&im, LOC_MOVE);
    win_setinputcodebit(&im, WIN_STOP);
    im.im_flags = IM_ASCII | IM_NEGEVENT;

    if (client_window) {
	client_font = xv_get(client_window, XV_FONT);
	font_scale = (int) xv_get(client_font, FONT_SCALE, 0);
    } else {
	/*
	 * need to get own font
	 */
    }
    if ((font_scale == FONT_NO_SCALE)
	|| (font_scale < (int) WIN_SCALE_SMALL)
	|| (font_scale > (int) WIN_SCALE_EXTRALARGE))
	font_scale = (int) WIN_SCALE_MEDIUM;
    /*
     * get new desired scale
     */
    if (font_scale == (int) WIN_SCALE_SMALL) {
	font_scale = (int) WIN_SCALE_MEDIUM;
    } else if (font_scale == (int) WIN_SCALE_MEDIUM) {
	font_scale = (int) WIN_SCALE_LARGE;
    } else if (font_scale == (int) WIN_SCALE_LARGE) {
	font_scale = (int) WIN_SCALE_EXTRALARGE;
    } else
	font_scale = (int) WIN_SCALE_EXTRALARGE;

    if (!saved_notice) {
	saved_notice = (struct notice *) calloc(1, sizeof(struct notice));
	if (!saved_notice) {
	    xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING, "Malloc failed.",
		     XV_ERROR_PKG_NAME, "Notice",
		     0);
	    return (NOTICE_FAILED);
	}
	notice_use_audible_bell = (int) defaults_get_boolean(
			     "alarm.audible", "Alarm.Audible", (Bool) TRUE);
	notice_jump_cursor = (int) defaults_get_boolean(
		     "notice.jumpCursor", "Notice.JumpCursor", (Bool) TRUE);
	default_beeps = defaults_get_integer(
				 "notice.beepCount", "Notice.BeepCount", 1);
    }
    notice_cursor = xv_create(client_window, CURSOR,
			      CURSOR_IMAGE, &notice_cursor_mpr,
			      CURSOR_XHOT, 7,
			      CURSOR_YHOT, 1,
			      CURSOR_OP, PIX_SRC | PIX_DST,
			      0);
    defpf = (Xv_Font) xv_find(client_window, FONT,
			      FONT_FAMILY, FONT_FAMILY_DEFAULT,
			      FONT_STYLE, FONT_STYLE_DEFAULT,
			      FONT_SCALE, FONT_SCALE_DEFAULT,
			      0);
    if (defpf == NULL) {
	defpf = (Xv_Font) xv_get(client_window, XV_FONT);
	if (defpf == NULL) {
	    xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING, "Unable to find default font.",
		     XV_ERROR_PKG_NAME, "Notice",
		     0);
	} else {
	    defpf = (Xv_Font) xv_find(client_window, FONT,
				      FONT_NAME, "fixed",
				      0);
	    if (defpf == NULL) {
		xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			 XV_ERROR_STRING, "Unable to find \"fixed\" font.",
			 XV_ERROR_PKG_NAME, "Notice",
			 0);
		(void) xv_destroy(notice_cursor);
		return (NOTICE_FAILED);
	    }
	}
    }
    notice = saved_notice;
    root_window = (Xv_object) xv_get(client_window, XV_ROOT);

    my_font = (Xv_Font) xv_find(client_window, FONT,
				FONT_RESCALE_OF, client_font, font_scale,
				0);
    if ((my_font == (Xv_Font) XV_ERROR) || (!my_font)) {
	my_font = defpf;
    }
    (void) notice_defaults(notice);
    (void) win_getrect(root_window, &notice_screen_rect);

    notice->notice_font = my_font;

    notice->event = (Event *) event;
    notice->client_window = client_window;

    attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    (void) notice_set_avlist(notice, avlist, FALSE);
    if (notice->number_of_buttons == 0)
        notice_add_default_button(notice);

    /*
     * Get mouse positioning info
     */
    if (notice->focus_specified) {
        int new_x, new_y;
        win_translate_xy(client_window, root_window, 
			 notice->focus_x,
			 notice->focus_y,
			 &new_x,
			 &new_y);
	x = old_mousex = new_x;
	y = old_mousey = new_y;
    } else {
	old_mouse_position = (Rect *) xv_get(root_window, WIN_MOUSE_XY);
	x = old_mousex = notice->focus_x = old_mouse_position->r_left;
	y = old_mousey = notice->focus_y = old_mouse_position->r_top;
    }

    (void) notice_get_notice_size(notice, &rect);	/* just size of
							 * rectangle */
    (void) notice_get_button_pin_points(notice, &rect);

    /*
     * Now offset for shadow
     */
    leftoff = MIN(50, rect.r_width / 3);
    topoff = MIN(50, rect.r_height / 2);
    quadrant = notice_quadrant(x, y);

    switch (quadrant) {
      case 0:
	left_placement = old_mousex;
	top_placement = old_mousey;
	rect.r_left = leftoff;
	rect.r_top = topoff;
	break;
      case 1:
	left_placement = old_mousex - (rect.r_width + leftoff + 2 * BORDER);
	top_placement = old_mousey;
	rect.r_left = 0;
	rect.r_top = topoff;
	break;
      case 2:
	left_placement = old_mousex - (rect.r_width + leftoff + 2 * BORDER);
	top_placement = old_mousey - (rect.r_height + topoff + 2 * BORDER);
	rect.r_left = 0;
	rect.r_top = 0;
	break;
      case 3:
	left_placement = old_mousex;
	top_placement = old_mousey - (rect.r_height + topoff + 2 * BORDER);
	rect.r_left = leftoff;
	rect.r_top = 0;
	break;
    }

    if (xv_depth(info) > 1) {
        cms = (Xv_Colormap_Seg *)
                  screen_find_cms(xv_screen(info), XV_CONTROL_CMS);
    }
    if (cms == NULL) {
        cms = xv_cms(info);
    }

    notice_window = 
	(Xv_Window) xv_create(0, WINDOW, WIN_TRANSPARENT,
			      /* no wmgr decoration */
			      WIN_TOP_LEVEL_NO_DECOR, TRUE,	
			      /* no damage caused */
			      WIN_SAVE_UNDER, TRUE,	
			      WIN_X, left_placement,
			      WIN_Y, top_placement,
			      WIN_WIDTH, rect.r_width + leftoff + 6 * BORDER + 1,
			      WIN_HEIGHT, rect.r_height + topoff + 6 * BORDER + 1,
			      WIN_CMS, cms,
			      0);
    fs = (Fullscreen) xv_create(0, FULLSCREEN,
				FULLSCREEN_INPUT_WINDOW, notice_window,
				WIN_CURSOR, notice_cursor,
				WIN_CONSUME_EVENTS,
				    WIN_MOUSE_BUTTONS,
				    WIN_ASCII_EVENTS,
				    WIN_UP_ASCII_EVENTS,
				    LOC_WINENTER, LOC_WINEXIT,
				    LOC_DRAG, LOC_MOVE,
				    WIN_STOP,
				    ACTION_HELP,
				    0,
				0);
    if (fs) {
	notice->fullscreen = fs;
	notice->fullscreen_window = (Xv_opaque) notice_window;
    } else {
	(void) xv_destroy(notice_window);
	(void) xv_destroy(notice_cursor);
	return (NOTICE_FAILED);	/* out of memory?? */
    }
    if (xv_get(xv_server(info), SERVER_JOURNALLING))
	(void) xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);

    (void) notice_do_bell(notice);

    /*
     * then draw empty box and shadow
     */
    (void) notice_drawbox((Pixwin *)notice->fullscreen_window, &rect, quadrant);
    /*
     * now fill in the box with the text AND buttons
     */
    (void) notice_layout(notice, &rect);

    if (notice_jump_cursor && notice->yes_button_exists) {
	notice_buttons_handle curr;
	for (curr = (notice_buttons_handle) notice->button_info;
	     curr != (notice_buttons_handle) NULL;
	     curr = (notice_buttons_handle) curr->next)
	    if (curr->is_yes) {
		x = left_placement + curr->button_rect.r_left +
		    (curr->button_rect.r_width / 2);
		y = top_placement + bottombox + ((quadrant < 2) ? topoff : 0);
		(void) xv_set(root_window, WIN_MOUSE_XY, x, y, 0);
		break;
	    }
    }
    /*
     * Stay in fullscreen until a button is pressed, or trigger used
     */
    trigger = notice->default_input_code;

    if (xv_get(xv_server(info), SERVER_JOURNALLING))
	(void) xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);

    for (;;) {
	int             is_select_action, is_stop_key;

	if (xv_input_readevent(notice_window, &ie, TRUE, TRUE, &im) == -1) {
	    break;
	}
	x = event_x(&ie);
	y = event_y(&ie);
	this_event = event_action(&ie);	/* get encoded event */
	this_id = event_id(&ie);/* get unencoded event */
	is_select_action = ((this_event == (int) ACTION_SELECT) ||
			    (this_id == (int) MS_LEFT))
	    ? 1 : 0;
	is_stop_key = ((this_event == (int) ACTION_STOP) ||
		       (this_id == (int) WIN_STOP))
	    ? 1 : 0;

	button = notice_button_for_event(notice, x, y);
	if (((this_event == trigger) || (this_id == trigger))
	    && (((trigger == (int) ACTION_SELECT) ||
		 (trigger == (int) MS_LEFT)) ?
		(event_is_up(&ie) && (current_button == NULL))
		: 0)) {
	    /*
	     * catch UP mouse left if missed down below for trigger
	     */
	    notice->result = NOTICE_TRIGGERED;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	} else if (((this_event == trigger) || (this_id == trigger))
		   && (((trigger == (int) ACTION_SELECT) ||
			(trigger == (int) MS_LEFT)) ?
		       (event_is_down(&ie) && (button == NULL))
		       : 0)) {
	    /*
	     * catch down mouse left for trigger, check above against
	     * button rather than current_button since current_button
	     * is NULL on SELECT down, but button may be a real button
	     */
	    notice->result = NOTICE_TRIGGERED;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	} else if (this_event == ACTION_HELP) {
	    if (event_is_down(&ie)) {
		notice->result = NOTICE_HELP;
		(void) notice_copy_event(notice, &ie);
		xv_help_save_image(notice->fullscreen_window,
			    xv_get(notice->fullscreen_window, WIN_WIDTH, 0),
			   xv_get(notice->fullscreen_window, WIN_HEIGHT, 0),
				   x, y);
		goto Done;
	    }
	} else if (is_stop_key
		   && saved_notice->no_button_exists) {
	    notice->result = NOTICE_NO;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	} else if ((this_event == ACTION_DO_IT
		    || this_event == NOTICE_ACTION_DO_IT)
		   && saved_notice->yes_button_exists) {
	    notice->result = NOTICE_YES;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	    /*
	     * NOTE: handle button event processing starting here
	     */
	} else if (is_select_action && notice->button_info) {
	    if (event_is_down(&ie)) {
		if (current_button &&
		    (current_button != button) &&
		    is_highlighted) {
		    (void) notice_paint_button((Pixwin *)notice->fullscreen_window,
					       current_button, NOTICE_NORMAL_BUTTON, (Pixfont *)notice->notice_font);
		    current_button = NULL;
		    is_highlighted = FALSE;
		    ok_to_toggle_buttons = FALSE;
		}
		if (button &&
		    !is_highlighted &&
		    current_button != button) {
		    current_button = button;
		    (void) notice_paint_button((Pixwin *)notice->fullscreen_window,
					       current_button, NOTICE_INVERT_BUTTON, (Pixfont *)notice->notice_font);
		    current_button = button;
		    is_highlighted = TRUE;
		    ok_to_toggle_buttons = TRUE;
		}
	    } else {		/* event_is_up */
		if (button) {
		    if (current_button &&
			(current_button != button) &&
			is_highlighted) {
			(void) notice_paint_button((Pixwin *)notice->fullscreen_window,
						   current_button, NOTICE_NORMAL_BUTTON,(Pixfont *) notice->notice_font);
			current_button = NULL;
			is_highlighted = FALSE;
			ok_to_toggle_buttons = FALSE;
		    }
		    notice->result = button->value;
		    (void) notice_copy_event(notice, &ie);
		    goto Done;
		} else {
		    ok_to_toggle_buttons = FALSE;
		}
	    }
	} else if (this_event == LOC_DRAG) {
	    if (current_button && (current_button != button)) {
		(void) notice_paint_button((Pixwin *)notice->fullscreen_window,
		 current_button, NOTICE_NORMAL_BUTTON,(Pixfont *) notice->notice_font);
		is_highlighted = FALSE;
		current_button = NULL;
		continue;
	    }
	    if (button) {
		if (current_button == button) {
		    continue;	/* already there */
		} else if ((current_button == NULL) && ok_to_toggle_buttons) {
		    (void) notice_paint_button((Pixwin *)notice->fullscreen_window,
			 button, NOTICE_INVERT_BUTTON,(Pixfont *) notice->notice_font);
		    current_button = button;
		    is_highlighted = TRUE;
		    continue;
		}
	    } else if (!button && current_button) {
		(void) notice_paint_button((Pixwin *)notice->fullscreen_window,
		 current_button, NOTICE_NORMAL_BUTTON,(Pixfont *) notice->notice_font);
		current_button = NULL;
		is_highlighted = FALSE;
		continue;
	    }
	} else if (((this_event==trigger)||(this_id==trigger)) 
		&& (!is_select_action)) {
	    /*
	     * catch trigger as a last case, trigger can't be select button
	     * here as that case is dealt with above
	     */
	    notice->result = NOTICE_TRIGGERED;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	}
    }

Done:
    if (xv_get(xv_server(info), SERVER_JOURNALLING))
	(void) xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);
    (void) xv_destroy(fs);
    (void) free_button_structs(notice->button_info);
    result = notice->result;
    if (client_window && (event != (Event *)0)) {
    	int new_x, new_y;
        win_translate_xy(notice_window, client_window, 
			 event_x(notice->event),
			 event_y(notice->event),
			 &new_x,
			 &new_y);
	event_set_x(event, new_x);
	event_set_y(event, new_y);
	event_set_window(event, client_window);
    }
    /* warp mouse back */
    if (notice_jump_cursor && notice->yes_button_exists) {
	if (notice->focus_specified) {
	    (void) xv_set(root_window, WIN_MOUSE_XY,
			  old_mousex, old_mousey,
			  0);
	} else {
	    (void) xv_set(root_window, WIN_MOUSE_XY, old_mousex, old_mousey, 0);
	}
    }
    (void) xv_destroy(notice_window);
    (void) xv_destroy(notice_cursor);
    return (result);
}

static void
notice_copy_event(notice, event)
    register notice_handle notice;
    Event          *event;
{
    if (notice->event == (Event *) 0) {
	return;
    } else
	*notice->event = *event;
}

/*
 * --------------------------- Statics ------------------------------
 */

static int
notice_preview_avlist_for_int_value(attr, avlist, default_if_not_present)
    Notice_attribute attr;
    register Notice_attribute *avlist;
    int             default_if_not_present;
{
    Notice_attribute current_attr;
    int             value = default_if_not_present;

    for (; *avlist; avlist = notice_attr_next(avlist)) {
	current_attr = avlist[0];
	if (attr == current_attr)
	    value = (int) avlist[1];
    }
    return (int) value;
}

static void
notice_preview_notice_font(notice, avlist)
    register notice_handle notice;
    register Notice_attribute *avlist;
{
    Notice_attribute attr;
    caddr_t         value;

    for (; *avlist; avlist = notice_attr_next(avlist)) {
	attr = avlist[0];
	value = (caddr_t) avlist[1];
	switch (attr) {
	  case NOTICE_FONT:
	    notice->notice_font = (Xv_Font) value;
	    break;
	  default:
	    break;
	}
    }
}
static void
notice_add_default_button(notice)
    register notice_handle notice;
{
    notice_buttons_handle button;

    button = (notice_buttons_handle) create_button_struct();
    button->string = (char *) notice_default_button_str;
    button->is_yes = TRUE;
    button->value = NOTICE_YES;
    notice->yes_button_exists = TRUE;
    (void) notice_add_button_to_list(notice, button);

}

static void
notice_set_avlist(notice, avlist, caller_external)
    register notice_handle notice;
    register Notice_attribute *avlist;
    int             caller_external;
{
    int             yes_button_seen = FALSE;
    int             no_button_seen = FALSE;
    int             number_of_buttons_seen = 0;
    int             trigger_set = 0;
    Notice_attribute attr;
    caddr_t         value;

    (void) notice_preview_notice_font(notice, avlist);

    for (; *avlist; avlist = notice_attr_next(avlist)) {
	attr = avlist[0];
	value = (caddr_t) avlist[1];
	switch (attr) {

	  case NOTICE_NO_BEEPING:
	    if ((int) value == 1)
		notice->dont_beep = 1;
	    break;

	  case NOTICE_TRIGGER:
	    notice->default_input_code = (int) avlist[1];
	    trigger_set = 1;
	    break;

	  case NOTICE_MESSAGE_STRINGS_ARRAY_PTR:
	    notice->message_items = (char **) value;
	    break;

	  case NOTICE_MESSAGE_STRINGS:
	    notice->message_items = (char **) &avlist[1];
	    break;

	  case NOTICE_BUTTON_YES:{
		notice_buttons_handle button;

		if (!yes_button_seen) {
		    yes_button_seen = TRUE;
		} else {
		    (void) xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
				    XV_ERROR_STRING,
		    "Only one NOTICE_BUTTON_YES attr allowed. Attr ignored.",
				    XV_ERROR_PKG_NAME, "Notice",
				    0);
		    break;
		}
		button = (notice_buttons_handle) create_button_struct();
		button->string = (char *) avlist[1];
		button->is_yes = TRUE;
		button->value = NOTICE_YES;
		notice->yes_button_exists = TRUE;
		number_of_buttons_seen++;
		(void) notice_add_button_to_list(notice, button);
		break;
	    }

	  case NOTICE_BUTTON_NO:{
		notice_buttons_handle button;

		if (!no_button_seen) {
		    no_button_seen = TRUE;
		} else {
		    xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			     XV_ERROR_STRING,
		    "Only one NOTICE_BUTTON_NO attr allowed. Attr ignored.",
			     XV_ERROR_PKG_NAME, "Notice",
			     0);
		    break;
		}
		button = (notice_buttons_handle) create_button_struct();
		button->string = (char *) avlist[1];
		button->is_no = TRUE;
		button->value = NOTICE_NO;
		notice->no_button_exists = TRUE;
		number_of_buttons_seen++;
		(void) notice_add_button_to_list(notice, button);
		break;
	    }

	  case NOTICE_BUTTON:{
		notice_buttons_handle button;

		button = (notice_buttons_handle) create_button_struct();
		button->string = (char *) avlist[1];
		button->value = (int) avlist[2];
		(void) notice_add_button_to_list(notice, button);
		number_of_buttons_seen++;
		break;
	    }

	  case NOTICE_FONT:
	    /*
	     * already previewed above
	     */
	    break;

	  case NOTICE_FOCUS_XY:
	    /*
	     * needs to be implemented
	     */
	    notice->focus_x = (int) avlist[1];
	    notice->focus_y = (int) avlist[2];
	    notice->focus_specified = TRUE;
	    break;

	  case XV_KEY_DATA:
	    if (avlist[1] == (Notice_attribute) XV_HELP) {
		notice->help_data = (char *) avlist[2];
	    } else
		(void) xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
				XV_ERROR_STRING, "Attribute not allowed.",
				XV_ERROR_PKG_NAME, "Notice",
				0);
	    break;

	  default:
	    (void) xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			    XV_ERROR_STRING, "Attribute not allowed.",
			    XV_ERROR_PKG_NAME, "Notice",
			    0);
	    break;
	}
    }

    if ((number_of_buttons_seen == 0) && (trigger_set == 0))
	notice->default_input_code = (int) ACTION_STOP;

}

/*
 * ------------------------------------------------------------------
 */

static void
notice_get_notice_size(notice, rect)
    register notice_handle notice;
    struct rect    *rect;
{
    int             x = 0, y = 0, i;
    Pixfont        *this_font = (Pixfont *) ((notice->notice_font) ?
					     notice->notice_font : defpf);
    max_msg_width = 0;
    if (notice->message_items) {
	char          **strs;
	int             num_lines = 0, str_width = 0;

	strs = (char **) notice->message_items;
	while (*strs) {
	    str_width = notice_text_width(this_font, (char *) *strs)
		+ 2 * (this_font->pf_defaultsize.x);	/* fudge factor */
	    max_msg_width = MAX(str_width, max_msg_width);	/* keep track of largest */
	    num_lines++;	/* keep line count */
	    strs++;
	}
	x = max_msg_width;
	y += ((num_lines + 2) * this_font->pf_defaultsize.y);
	y += this_font->pf_defaultsize.y / 2;
    }
    y = MAX(y + row_gap, top_marg);	/* top_marg in case no text */

    i = 0;			/* init button count */
    if (notice->button_info) {
	int             max_buttons_width = 0;
	int             this_buttons_width = 0;
	int             total = 0;
	notice_buttons_handle curr;

	for (curr = (notice_buttons_handle) notice->button_info;
	     curr != (notice_buttons_handle) NULL;
	     curr = (notice_buttons_handle) curr->next) {
	    this_buttons_width = notice_button_width(this_font,
					      (notice_buttons_handle) curr);
	    max_buttons_width += this_buttons_width;
	    i++;		/* increment button count */
	}
	y += notice->button_info->button_rect.r_height;
	x = MAX(x, max_buttons_width);
	total = x + left_marg + right_marg + (6 * BORDER) + (4 * SHADOW);
	if (total >= notice_screen_rect.r_width) {
	    /*
	     * if buttons too wide, then they clip just like the text will.
	     * this is not very nice, but TOO BAD!! it's a rough job
	     * attempting to handle their wraping.
	     */
	    button_gap = 0;
	} else {
	    if (i == 1) {	/* center button */
		button_gap = MAX(0, (x - max_buttons_width) / 2);
	    } else {		/* >1 */
		int             leftover = x - max_buttons_width;
		int             desired_gap = (i - 1) * NOTICE_BUTTON_GAP;

		left_indent = MAX(0, (leftover - desired_gap) / 2);
		button_gap = NOTICE_BUTTON_GAP;
	    }
	    if ((i > 1) && (left_indent == 0) && (((i - 1) * min_button_gap) < total)) {
		left_indent = min_button_gap;
		x += (i - 1) * min_button_gap;
	    }
	}
    }
    /*
     * Sum up positions and add in OL FUDGE factors
     */
    x += left_marg + right_marg + (6 * BORDER) + (4 * SHADOW);
    y += top_marg + bottom_marg + (2 * BORDER);

    rect->r_top = 0;
    rect->r_left = 0;
    rect->r_width = x;
    rect->r_height = y;
}

static void
notice_layout(notice, rect)
    register notice_handle notice;
    struct rect    *rect;
{
    int             x, y;
    notice_buttons_handle curr;
    int             x1, y1, wd, ht;
    Pixwin         *pw = (Pixwin *) notice->fullscreen_window;
    Pixfont        *this_font = (Pixfont *) ((notice->notice_font) ?
					     notice->notice_font : defpf);

    /*
     * later, consider centering here
     */
    x = rect->r_left + left_marg + BORDER + this_font->pf_defaultsize.x;
    y = rect->r_top + top_marg + BORDER + this_font->pf_defaultsize.y;

    if (notice->message_items) {
	char          **strs;
	int             offset = notice_offset_from_baseline(this_font);

	strs = (char **) notice->message_items;
	while (*strs) {
	    int             str_width = notice_text_width(this_font, (char *) *strs);
	    pw_text(pw, x + (max_msg_width - str_width) / 2,
		    (offset >= 0) ? y + offset : y - offset,
		    PIX_SRC, this_font, (char *) *strs);

y += this_font->pf_defaultsize.y;
	    strs++;
	}
    }
    y += this_font->pf_defaultsize.y;
    y = MAX(y + row_gap, 0);

    if (notice->button_info) {
	if (notice->number_of_buttons == 1) {
	    x += button_gap;
	    notice_build_button(pw, rect, x, y, notice->button_info, this_font);
	} else if (notice->number_of_buttons > 1) {
	    x += left_indent;
	    for (curr = (notice_buttons_handle) notice->button_info;
		 curr != (notice_buttons_handle) NULL;
		 curr = (notice_buttons_handle) curr->next) {
		notice_build_button(pw, rect, x, y,
				    (notice_buttons_handle) curr, this_font);
		x += curr->button_rect.r_width + button_gap;
	    }
	}
    }
    x1 = rect->r_left + 4;
    y1 = rect->r_top + 4;
    ht = rect->r_height - 8;
    wd = rect->r_width - 8;
    pw_vector(pw, x1, y1, x1 + wd, y1, PIX_SET, 1);
    pw_vector(pw, x1, y1 + ht, x1 + wd, y1 + ht, PIX_SET, 1);
    pw_vector(pw, x1, y1, x1, y1 + ht, PIX_SET, 1);
    pw_vector(pw, x1 + wd, y1, x1 + wd, y1 + ht, PIX_SET, 1);
}

static void
notice_get_button_pin_points(notice, rect)
    register notice_handle notice;
    struct rect    *rect;
{
    int             x, y;
    notice_buttons_handle curr;
    Pixfont        *this_font = (Pixfont *) ((notice->notice_font) ?
					     notice->notice_font : defpf);

    x = left_marg + BORDER + this_font->pf_defaultsize.x;
    y = top_marg + BORDER + this_font->pf_defaultsize.y;

    if (notice->message_items) {
	char          **strs;

	strs = (char **) notice->message_items;
	while (*strs) {
	    y += this_font->pf_defaultsize.y;
	    strs++;
	}
    }
    y += this_font->pf_defaultsize.y;
    y = MAX(y + row_gap, 0);

    if (notice->button_info) {
	if (notice->number_of_buttons == 1) {
	    x += button_gap;
	    leftbox = x;
	    x += notice_button_width(this_font, notice->button_info);
	    rightbox = x;
	} else if (notice->number_of_buttons > 1) {
	    leftbox = x;
	    for (curr = notice->button_info; curr != NULL; curr = curr->next) {
		x += notice_button_width(this_font, curr) + button_gap;
	    }
	    x -= button_gap;
	    rightbox = x;
	}
	y += notice->button_info->button_rect.r_height - (2 * BORDER);
	bottombox = y;
    }
}


/*
 * ----------------------   Misc Utilities   ------------------------
 */

static void
notice_defaults(notice)
    notice_handle   notice;
{
    notice->beeps = default_beeps;
    notice->default_input_code = '\0';	/* ASCII NULL */
    notice->notice_font = (Xv_Font) defpf;
    notice->dont_beep = FALSE;
    notice->yes_button_exists = FALSE;
    notice->no_button_exists = FALSE;
    notice->focus_specified = FALSE;
    notice->event = (Event *) 0;
    notice->button_info = (notice_buttons_handle) 0;
    notice->number_of_buttons = 0;
    notice->help_data = "xview:notice";
    button_gap = 0;
}

/*
 * ------------------------------------------------------------------
 */

static void
notice_add_button_to_list(notice, button)
    register notice_handle notice;
    notice_buttons_handle button;
{
    notice_buttons_handle curr;

    if (notice->button_info) {
	for (curr = notice->button_info; curr; curr = curr->next)
	    if (curr->next == NULL) {
		curr->next = button;
		break;
	    }
    } else
	notice->button_info = button;
    notice->number_of_buttons++;
}

/*
 * ------------------------------------------------------------------
 */

static          notice_buttons_handle
create_button_struct()
{
    notice_buttons_handle pi = NULL;

    pi = (notice_buttons_handle) calloc(1, sizeof(struct notice_buttons));
    if (!pi) {
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
	         XV_ERROR_STRING,
	             "calloc failed in notice[create_button_struct].",
		 XV_ERROR_PKG_NAME, "Notice",
		 0);
    } else {
	pi->is_yes = (int)FALSE;
    }
    return pi;
}


/*
 * ------------------------------------------------------------------
 */

static void
free_button_structs(first)
    notice_buttons_handle first;
{
    notice_buttons_handle current;

    if (!first)
	return;
    for (current = first; current != NULL; current = current->next) {
	free(current);
    }
}

/*
 * font char/pixel conversion routines
 */

static int
notice_offset_from_baseline(font)
    Pixfont        *font;
{
    if (font == NULL)
	return (0);
    return (font->pf_char[110].pc_home.y);	/* lowercase 'n' char */
}

static int
notice_text_width(font, str)
    Pixfont        *font;
    char           *str;
{
    struct pr_size  size;

    size = xv_pf_textwidth(strlen(str), font, str);
    return (size.x);
}

static int
notice_button_width(font, button)
    Pixfont        *font;
    notice_buttons_handle button;
{
    int             text_width = notice_text_width(font, button->string);
    int             fontsize = font->pf_defaultsize.y - 1;
    button->button_rect.r_width = text_width;
    button->button_rect.r_height = font->pf_defaultsize.y;
    if (fontsize <= 12) {
	/*
	 * left text margin = 7, right = width-9 top text margin = 4, bottom
	 * = height-8
	 */
	button->button_rect.r_width += 15;
	button->button_rect.r_height += 11;
    } else if ((fontsize > 12) && (fontsize < 19)) {
	/*
	 * left text margin = 8, right = width-10 top text margin = 5, bottom
	 * = height-8
	 */
	button->button_rect.r_width += 17;
	button->button_rect.r_height += 12;
    } else {
	/*
	 * left text margin = 9; right = width-12 top text margin = 5; bottom
	 * = height-11
	 */
	button->button_rect.r_width += 20;
	button->button_rect.r_height += 15;
    }
    return (button->button_rect.r_width);
}

static int
notice_quadrant(x, y)
    int             x, y;
{
    int             quadrant;

    if ((x <= notice_screen_rect.r_width / 2) && (y <= notice_screen_rect.r_height / 2))
	quadrant = 0;
    else if ((x > notice_screen_rect.r_width / 2) && (y <= notice_screen_rect.r_height / 2))
	quadrant = 1;
    else if ((x > notice_screen_rect.r_width / 2) && (y > notice_screen_rect.r_height / 2))
	quadrant = 2;
    else
	quadrant = 3;

    return (quadrant);
}

static void
notice_drawbox(pw, rectp, quadrant)
    Pixwin         *pw;
    struct rect    *rectp;
    int             quadrant;
{
    struct rect     rect;
    struct pr_pos   vlist[6];
    int             nptarr[1];

    rect = *rectp;
    nptarr[0] = 3;

    /*
     * draw 3D shadow
     */
    switch (quadrant) {
      case 0:			/* break down and to right */
	vlist[0].x = 0;
	vlist[0].y = 0;
	vlist[1].x = rect.r_width + leftoff;
	vlist[1].y = topoff;
	vlist[2].x = leftoff;
	vlist[2].y = topoff;
	pw_polygon_2(pw, 0, 0, 1, nptarr, vlist, PIX_SRC, &notice_gray17_patch, 0, 0);
	vlist[1].x = leftoff;
	vlist[1].y = topoff;
	vlist[2].x = leftoff;
	vlist[2].y = rect.r_height + topoff;
	pw_polygon_2(pw, 0, 0, 1, nptarr, vlist, PIX_SRC, &notice_gray25_patch, 0, 0);
	break;
      case 1:			/* break down and to left */
	vlist[0].x = rect.r_width + leftoff;
	vlist[0].y = 0;
	vlist[1].x = rect.r_width;
	vlist[1].y = rect.r_height + topoff;
	vlist[2].x = rect.r_width;
	vlist[2].y = topoff;
	pw_polygon_2(pw, 0, 0, 1, nptarr, vlist, PIX_SRC, &notice_gray25_patch, 0, 0);
	vlist[1].x = 0;
	vlist[1].y = topoff;
	pw_polygon_2(pw, 0, 0, 1, nptarr, vlist, PIX_SRC, &notice_gray17_patch, 0, 0);
	break;
      case 2:			/* break up and to left */
	vlist[0].x = rect.r_width + leftoff;
	vlist[0].y = rect.r_height + topoff;
	vlist[1].x = rect.r_width;
	vlist[1].y = 0;
	vlist[2].x = rect.r_width;
	vlist[2].y = rect.r_height;
	pw_polygon_2(pw, 0, 0, 1, nptarr, vlist, PIX_SRC, &notice_gray25_patch, 0, 0);
	vlist[1].x = 0;
	vlist[1].y = rect.r_height;
	pw_polygon_2(pw, 0, 0, 1, nptarr, vlist, PIX_SRC, &notice_gray75_patch, 0, 0);
	break;
      case 3:			/* break up and to right */
	vlist[0].x = 0;
	vlist[0].y = rect.r_height + topoff;
	vlist[1].x = rect.r_width + leftoff;
	vlist[1].y = rect.r_height;
	vlist[2].x = leftoff;
	vlist[2].y = rect.r_height;
	pw_polygon_2(pw, 0, 0, 1, nptarr, vlist, PIX_SRC, &notice_gray75_patch, 0, 0);
	vlist[1].x = leftoff;
	vlist[1].y = 0;
	pw_polygon_2(pw, 0, 0, 1, nptarr, vlist, PIX_SRC, &notice_gray25_patch, 0, 0);
	break;
    }

    /*
     * draw box
     */
    pw_writebackground(pw, rect.r_left, rect.r_top,
		       rect.r_width, rect.r_height, PIX_CLR);

    vlist[0].x = rect.r_left;
    vlist[0].y = rect.r_top;
    vlist[1].x = rect.r_left + rect.r_width;
    vlist[1].y = rect.r_top;
    vlist[2].x = rect.r_left + rect.r_width;
    vlist[2].y = rect.r_top + rect.r_height;
    vlist[3].x = rect.r_left;
    vlist[3].y = rect.r_top + rect.r_height;
    pw_vector(pw, vlist[0].x, vlist[0].y, vlist[1].x, vlist[1].y,
	      PIX_SET, 1);
    pw_vector(pw, vlist[0].x, vlist[0].y + 1, vlist[1].x, vlist[1].y + 1,
	      PIX_SET, 1);
    pw_vector(pw, vlist[1].x, vlist[1].y, vlist[2].x, vlist[2].y,
	      PIX_SET, 1);
    pw_vector(pw, vlist[1].x - 1, vlist[1].y, vlist[2].x - 1, vlist[2].y,
	      PIX_SET, 1);
    pw_vector(pw, vlist[2].x, vlist[2].y, vlist[3].x, vlist[3].y,
	      PIX_SET, 1);
    pw_vector(pw, vlist[2].x, vlist[2].y - 1, vlist[3].x, vlist[3].y - 1,
	      PIX_SET, 1);
    pw_vector(pw, vlist[3].x, vlist[3].y, vlist[0].x, vlist[0].y,
	      PIX_SET, 1);
    pw_vector(pw, vlist[3].x + 1, vlist[3].y, vlist[0].x + 1, vlist[0].y,
	      PIX_SET, 1);
}

static void
notice_build_button(pw, rect, x, y, button, font)
    Pixwin         *pw;
    struct rect    *rect;
    int             x, y;
    notice_buttons_handle button;
    Pixfont        *font;
{
    button->button_rect.r_top = y;
    button->button_rect.r_left = x;
    (void) notice_paint_button(pw, button, NOTICE_NORMAL_BUTTON, font);
}

static void
notice_paint_button(pw, button, invert, font)
    struct pixwin  *pw;
    notice_buttons_handle button;
    int             invert;
    Pixfont        *font;
{
    int ringed = button->is_yes;
    (void) panel_paint_button(pw, NULL, &button->button_rect, font,
			      button->string, 0, -1, invert, 0,
			      ringed, 0, 0);
}

static          notice_buttons_handle
notice_button_for_event(notice, x, y)
    register notice_handle notice;
{
    register notice_buttons_handle curr;

    if (notice->button_info == NULL)
	return (NULL);
    for (curr = notice->button_info; curr; curr = curr->next) {
	if ((x >= curr->button_rect.r_left - min_button_gap / 2)
	    && (x <= (curr->button_rect.r_left + min_button_gap / 2
		      + curr->button_rect.r_width))
	    && (y >= curr->button_rect.r_top)
	    && (y <= (curr->button_rect.r_top
		      + curr->button_rect.r_height))) {
	    return (curr);
	}
    }
    return ((notice_buttons_handle) 0);
}

static void
notice_do_bell(notice)
    notice_handle   notice;
{
    struct timeval  wait;

    if (!notice_use_audible_bell)
	return;

    wait.tv_sec = 0;
    wait.tv_usec = 100000;
    if (!notice->dont_beep && (notice->beeps > 0)) {
	int             i = notice->beeps;
	while (i--)
	    (void) win_bell(notice->fullscreen_window, wait, 0);
    }
}
