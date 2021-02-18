#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)om_render.c 20.87 89/11/07";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>

#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <pixrect/pr_util.h>
#include <pixrect/memvar.h>

#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/font.h>
#include <xview/notify.h>
#include <xview/pixwin.h>
#include <xview/window.h>
#include <xview/win_input.h>
#include <xview/win_struct.h>

#include <xview/window.h>
#include <xview/server.h>

#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>

#ifdef XV_DEBUG
#include "om_impl.h"
#else
#include <xview_private/om_impl.h>
#endif				/* XV_DEBUG */

#define	PIXRECT_NULL	((struct pixrect *)0)
#define SCREEN_MARGIN   10	/* Minimum number of pixels away from the
				 * edge of the screen a menu's left and top
				 * sides should be. (to enable backing out of
				 * pullright menus */
#define MENU_TITLE_MARGIN 10
#define MOUSE_BUMP 5		/* jiggle damping factor on MENU click */

typedef enum {
    CLEANUP_EXIT,
    CLEANUP_CLEANUP,
    CLEANUP_ABORT
}               Cleanup_mode;

static unsigned short gray50_data[16] = {
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555,
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555
};
static          mpr_static(gray_pr, 16, 16, 1, gray50_data);

extern Pixrect  panel_pullright_pr;

/* ------------------------------------------------------------------------- */

/*
 * XView Private
 */
Xv_private void     panel_paint_button();
Xv_private void     panel_paint_choice_outline();
Xv_private Xv_Window screen_get_cached_window();
Xv_private void	screen_set_cached_window_busy();

/*
 * Package private
 */
Pkg_private Notify_value menu_client_window_event_proc();
Pkg_private struct pixfont *image_get_font();
Pkg_private void image_compute_size();
Pkg_private void menu_cache_release_menu();
Pkg_private void menu_done();
Pkg_private void menu_render();
Pkg_private void menu_shadow_event_proc();
Pkg_private void menu_window_event_proc();

Pkg_private Xv_Window menu_client_window;	/* defined in om_public.c */
Pkg_private Xv_menu_info *menu_active_menu;	/* defined in om_public.c */

/*
 * Private
 */
static Menu_status render_pullright();
static short    compute_show_submenu();
static int      absolute_value();
static int      compute_dimensions();
static int      compute_item_size();
static int      constrainrect();
static void     cleanup();
static void     compute_margin();
static void     compute_menu_item_paint_rect();
static void     compute_rects();
static void     destroy_gen_items();
static void     feedback();
static void     get_mode();
static void     menu_window_paint();
static void     menu_shadow_paint();
static void     paint_menu_item();
static void     process_event();
static void     render_items();
static void     set_mode();
static void     submenu_done();

#define MENU_CACHE_SIZE 4
static Xv_menu_info *menu_cache[MENU_CACHE_SIZE] = {
	NULL, NULL, NULL, NULL
    };
static int menu_cache_index = MENU_CACHE_SIZE-1;
    /* menu_cache_index always points to the last slot filled at
     * the end of each menu_render call.
     */


/*
 * Private defs
 */
#define	MENU_SHADOW	6	/* Width of menu shadow */
#define WALKMENU_BORDER(m)	\
	((m->class == MENU_CHOICE) ? 6 : 2)	/* Width of menu border */

/* ------------------------------------------------------------------------- */

/*
 * Menu_render modifies the inputevent parameter iep. It should contain the
 * last active inputevent read for the fd. Return values: MENU_STATUS_PIN:
 * pushpin was selected MENU_STATUS_ABORT:	no menu item was selected;
 * abort menu chain MENU_STATUS_DONE:	a menu item was selected
 * MENU_STATUS_PARENT:  no menu item was selected; use last event
 * MENU_STATUS_IGNORE:  no menu item was selected; ignore last event
 * MENU_STATUS_DEFAULT: select default item from menu
 */
Pkg_private void
menu_render(menu, group, parent, reenter, font_size)
    Xv_menu_info   *menu;
    Xv_menu_group_info *group;
    Xv_menu_item_info *parent;
    int             font_size;
{
    register Xv_menu_info *m;
    register Event *iep;
    Xv_Color_info	*color_info; 

    /*
     * Extra registers not available on 80386
     */
    register int    i;

    short           check_cmc;	/* boolean: checking for Click-Move-Click
				 * mode */
    int		    cms_changed = FALSE;
    int             item_width, item_height;
    int             n = 0;	/* item number needing to be cleared */
    int		    new_window; /* TRUE or FALSE */
    char	   *old_cms_name;
    int		    original_menu_cache_index;
				/* oldest slot filled in menu_cache */
    struct image   *std_image;
    Menu            gen_menu, (*gen_proc) ();
    int		    menu_window_retained;
    Xv_object       root_window = xv_get(menu_client_window, XV_ROOT);
    Xv_object       screen;
    Rect            shadowrect;
    Xv_Drawable_info *info, *client_window_info;
    Rect	    used_window_rect; /* width = 0 => window not reused */
    Xv_Window	    win;

    /*
     * Initial setup: Pull out group information.
     */
    DRAWABLE_INFO_MACRO(root_window, info);
    screen = xv_screen(info);
    menu->group_info = group;
    menu->parent = parent;
    group->depth++;
    iep = &group->last_event;

    /*
     * From here on any returns from this procedure should go thru exit:
     */

    /*
     * Dynamically create the menu if requested.
     */
    if (gen_proc = menu->gen_proc) {
	gen_menu = gen_proc(MENU_PUBLIC(menu), MENU_DISPLAY);
	if (gen_menu == NULL) {
	    xv_error(menu, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
		   "menu_render: menu's gen_proc failed to generate a menu",
		     XV_ERROR_PKG_NAME, "Menu",
		     0);
	    cleanup(CLEANUP_ABORT);
	    return;
	}
	m = MENU_PRIVATE(gen_menu);
	menu_active_menu = m;
	m->enable_rect = menu->enable_rect;
	m->popup = menu->popup;
	m->position_rect = menu->position_rect;
	m->pulldown = menu->pulldown;
	m->rendered = FALSE;
	m->stay_up = menu->stay_up;
	m->gen_proc = gen_proc;
	m->group_info = group;

	/* Ensure default image fonts valid */
	if (m->default_image.font == 0)
	    m->default_image.font = menu->default_image.font;
	if (m->default_image.bold_font == 0)
	    m->default_image.bold_font = menu->default_image.bold_font;
    } else {
	m = menu;
    }
    m->active = TRUE;
    m->parent = parent;
    m->font_size = font_size;
    m->gen_items = FALSE;
    m->read_inputevent = FALSE;
    if (m->group_info->depth <= 1)
	m->setting_default = event_ctrl_is_down(iep) ? TRUE : FALSE;
    else
	m->setting_default = parent->parent->setting_default;

    /* Get the menu window.  We keep a reserved list of 4 pairs of windows
     * for user-interactive performance.  If the list is full, then the oldest
     * window pair will be reused for the new menu.
     */
    DRAWABLE_INFO_MACRO(menu_client_window, client_window_info);
    rect_construct(&used_window_rect, 0, 0, 0, 0);
    if (!m->window) {
	menu_window_retained = FALSE;
	m->window = screen_get_cached_window(screen, menu_window_event_proc,
	    TRUE, xv_dynamic_color(client_window_info) ? TRUE : FALSE,
	    &new_window);
	if (!m->window) {
	    xv_error(0, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
		     "menu_create: unable to allocate menu window",
		     XV_ERROR_PKG_NAME, "Menu",
		     0);
	    cleanup(CLEANUP_ABORT);
	    return;
	}
	if (!new_window)
	    used_window_rect = *(Rect *)xv_get(m->window, XV_RECT);
	xv_set(m->window,
	       WIN_CONSUME_EVENT, ACTION_HELP,
	       XV_KEY_DATA, MENU_WINDOW_MENU, m,
	       0);
	/* Cache up to MENU_CACHE_SIZE menu window pairs.
	 * Start with the oldest cache slot, which is the slot after
	 * the last filled slot.
	 */
	/* Point to the oldest slot */
	if (++menu_cache_index >= MENU_CACHE_SIZE)
	    menu_cache_index = 0;
	original_menu_cache_index = menu_cache_index;
	while (TRUE) {
	    if (menu_cache[menu_cache_index] == NULL) {
		menu_cache[menu_cache_index] = m;
		m->cache_index = menu_cache_index;
		break;
	    } else if (menu_cache[menu_cache_index]->active) {
		if (++menu_cache_index >= MENU_CACHE_SIZE)
		    menu_cache_index = 0;
		if (menu_cache_index == original_menu_cache_index) {
		    m->cache_index = -1;  /* menu is not cached */
		    /* Point back to last slot filled */
		    if (--menu_cache_index < 0)
			menu_cache_index = MENU_CACHE_SIZE - 1;
		    break;
		}
	    } else {
		/* Free previously reserved menu window */
		win = menu_cache[menu_cache_index]->window;
		screen_set_cached_window_busy(xv_screen(info), win, FALSE);
		menu_cache[menu_cache_index]->rendered = FALSE;
		menu_cache[menu_cache_index]->window = NULL;
		/* Free previously reserved shadow window */
		win = menu_cache[menu_cache_index]->shadow_window;
		screen_set_cached_window_busy(xv_screen(info), win, FALSE);
		menu_cache[menu_cache_index]->shadow_window = NULL;
		/* Reassign menu cache slot to new menu */
		menu_cache[menu_cache_index]->cache_index = -1;
		menu_cache[menu_cache_index] = m;
		m->cache_index = menu_cache_index;
		break;
	    }
	}
    } else
	menu_window_retained = TRUE;

    m->glyph_font = xv_get(m->window, WIN_GLYPH_FONT);

    /* Get the shadow window */
    if (!m->shadow_window) {
	m->shadow_window = screen_get_cached_window(screen,
	    menu_shadow_event_proc, FALSE,
	    xv_dynamic_color(client_window_info) ? TRUE : FALSE, &new_window);
	if (!m->shadow_window) {
	    xv_error(0, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
		     "menu_create: unable to allocate shadow window",
		     XV_ERROR_PKG_NAME, "Menu",
		     0);
	    cleanup(CLEANUP_ABORT);
	    return;
	}
	xv_set(m->shadow_window,
	       WIN_CONSUME_EVENT, ACTION_HELP,
	       XV_KEY_DATA, MENU_SHADOW_MENU, m,
	       0);
    }

    if (xv_depth(client_window_info) > 1) {
	color_info = (Xv_Color_info *)xv_get(menu_client_window, 
					     WIN_COLOR_INFO);
	old_cms_name = (char *) xv_get(m->window, WIN_CMS_NAME);
	xv_set(m->window, WIN_COLOR_INFO, color_info, 0);
	xv_set(m->shadow_window, WIN_COLOR_INFO, color_info, 0);
	cms_changed = strcmp(old_cms_name, (char *) xv_get(m->window,
	    WIN_CMS_NAME));
    }
    /*
     * No items, return to parent menu
     */
    if (m->nitems == 0) {
	m->status = MENU_STATUS_PARENT;
	cleanup(CLEANUP_EXIT);
	return;
    }
    /* Remember initial selections */
    switch (m->class) {
      case MENU_CHOICE:
	for (i = 0; i < m->nitems; i++)
	    if (m->item_list[i]->selected) {
		n = m->selected_position = i + 1;
		break;
	    }
	if (n == 0) {
	    /*
	     * No choice selected by client: force default choice to be
	     * selected
	     */
	    if (m->item_list[m->default_position - 1]->title)
		/* Can't select title */
		++m->default_position;
	    m->item_list[m->default_position - 1]->selected = TRUE;
	    n = m->selected_position = m->default_position;
	}
	break;

      case MENU_TOGGLE:
	/* Set each menu item's toggle_on = selected */
	for (i = 0; i < m->nitems; i++)
	    m->item_list[i]->toggle_on = m->item_list[i]->selected;
	break;
    }

    /*
     * Find the current menu item
     */
    if (m->class == MENU_CHOICE)
	/* Pulldown or pullright choice menu */
	m->curitem = m->setting_default ? m->default_position :
	    m->selected_position;
    else if (m->popup && m->group_info->depth > 1)
	m->curitem = m->default_position;
    else
	m->curitem = 0;

    /*
     * Compute the size of an item.
     */
    std_image = &m->default_image;
    m->gen_items = compute_item_size(m, std_image, font_size);
    item_width = std_image->width;
    item_height = std_image->height;

    /*
     * Compute the dimensions of the menu rectangle
     */
    if (!compute_dimensions(m, item_width, item_height,
			    &m->fs_menurect)) {
	cleanup(CLEANUP_ABORT);
	return;
    }
    /*
     * Compute the rects:
     *   m->fs_menurect - represents the area of the menu including its borders.
     *   shadowrect - 	shadow rectangle.
     * Both rects are in screen coordinates.
     */
    compute_rects(m, iep, item_width, item_height, &m->fs_menurect,
		  &shadowrect);

    /* Compute pushpin coordinates */
    if (m->pin) {
	m->pushpin_left = WALKMENU_BORDER(m) + 2*m->default_image.margin;
	m->pushpin_rect.r_left = m->menurect.r_left + m->pushpin_left;
	m->pushpin_top = WALKMENU_BORDER(m) + m->default_image.margin +
	    (m->default_image.height - MENU_PIN_HEIGHT)/2;
	m->pushpin_rect.r_top = m->menurect.r_top + m->pushpin_top;
	m->pushpin_rect.r_width = MENU_PIN_WIDTH;
	m->pushpin_rect.r_height = MENU_PIN_HEIGHT;
    }

    /*
     * Define the menu and shadow window position.
     */
    xv_set(m->window,
	   XV_X, m->fs_menurect.r_left,
	   XV_Y, m->fs_menurect.r_top,
	   0);
    xv_set(m->shadow_window,
	   XV_X, shadowrect.r_left,
	   XV_Y, shadowrect.r_top,
	   0);

    /*
     * Define the menu and shadow window dimensions Note: shadow rect width &
     * height = menu rect width & height
     */
    xv_set(m->window,
	   XV_WIDTH, m->fs_menurect.r_width,
	   XV_HEIGHT, m->fs_menurect.r_height,
	   0);
    xv_set(m->shadow_window,
	   XV_WIDTH, shadowrect.r_width,
	   XV_HEIGHT, shadowrect.r_height,
	   0);
    xv_set(XV_SERVER_FROM_WINDOW(m->window), SERVER_SYNC, FALSE, 0);

    /*
     * Display the menu (i.e., map the menu and shadow windows). Note:
     * Windows must be mapped after the width and height have been set.
     * Changing the size of a save-under window will invalidate the
     * save-under, resulting in damage-repaint events.
     */
    xv_set(m->shadow_window,
	   XV_SHOW, TRUE,	/* MUST BE FIRST! */
	   WIN_RETAINED, TRUE,
	   0);
    xv_set(m->window,
	   XV_SHOW, TRUE,
	   WIN_RETAINED, TRUE,
	   0);
    if ((m->rendered && cms_changed) ||
	(m->fs_menurect.r_width <= used_window_rect.r_width &&
	 m->fs_menurect.r_height <= used_window_rect.r_height) ||
	(menu_window_retained && !m->rendered)) {
	/* 1. The menu has been shown before, but with different colors.
	 * 2. Retained windows that don't expose any new area don't
	 * generate WIN_REPAINT events.
	 * 3. When the MENU button is released faster than the menu and shadow
	 * windows are mapped, the Expose events may come in after the window
	 * is unmapped and the menu group dismissed.  (An xscope trace will
	 * show the XEvents sequence to be UnmapNotify, UnmapNotify, Expose,
	 * Expose.)  menu_window_event_proc will not call menu_window_paint
	 * when menu_client_window is NULL, and m->rendered will not be set.
	 * In these case, we must manually paint the menu and shadow windows.
	 */
	m->rendered = FALSE;
	menu_window_paint(m, m->window);
	menu_shadow_paint(m->shadow_window);
    }
    if (xv_get(xv_server(info), SERVER_JOURNALLING))
	xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);

    if (m->group_info->depth <= 1) {
	/*
	 * We're bringing up the base menu: determine if this is a stay-up
	 * menu.
	 */
	if (event_action(&m->group_info->first_event) == ACTION_MENU &&
	    event_is_up(&m->group_info->first_event))
	    m->stay_up = TRUE;
	else {
	    m->state = MENU_STATE_GET_MODE;
	    m->status = MENU_STATUS_BUSY;
	    return;
	}
    }
    check_cmc = m->group_info->depth <= 1 || parent->parent->stay_up;
    set_mode(check_cmc, TRUE);
    process_event(iep);
    return;
}


static void
get_mode(event)
    Event          *event;
{
    Xv_menu_info   *m = menu_active_menu;
    short           mouse_dragged;
    /*
     * boolean: mouse was dragged at least 5 pixels in any direction.
     */
    short           menu_button_up;	/* boolean */

    menu_button_up = event_action(event) == ACTION_MENU && event_is_up(event);
    if (!m->ptr_jumped) {
	/*
	 * Pointer was not jumped: determine stay-up mode based on mouse drag
	 * and MENU click.  Mouse is said to have been dragged if it was
	 * moved more than the MENU-click jiggle damping factor (MOUSE_BUMP).
	 */
	mouse_dragged =
	    (event_action(event) == LOC_DRAG ||
	     event_action(event) == ACTION_MENU) &&
	    ((absolute_value(event_x(event) -
		     event_x(&m->group_info->first_event)) >= MOUSE_BUMP) ||
	     (absolute_value(event_y(event) -
		      event_y(&m->group_info->first_event)) >= MOUSE_BUMP));
	if (!mouse_dragged && !menu_button_up)
	    /* Menu state is still MENU_STATE_GET_MODE */
	    return;		/* go back to notifer; wait for next mouse
				 * event */
	/* Mouse was dragged and/or MENU button is up */
	m->stay_up = !mouse_dragged && menu_button_up;
    } else {
	/*
	 * Pointer was jumped: we can't determine if the mouse was dragged
	 * because jumping the pointer is asynchronous to reporting mouse
	 * events.  Declare Click-Move-Click (i.e., stay-up) if MENU-up.
	 */
	m->stay_up = menu_button_up;
    }
    set_mode(TRUE, menu_button_up);
}


static void
set_mode(check_cmc, menu_button_up)
    short           check_cmc;	/* boolean: checking for Click-Move-Click
				 * mode */
    short           menu_button_up;	/* boolean */
{
    register Xv_menu_info *m = menu_active_menu;

    if (check_cmc && !m->stay_up) {
	/*
	 * Press-drag-release: process a MENU-up
	 */
	m->read_inputevent = menu_button_up;
    }
    m->state = MENU_STATE_TRACK_MOUSE;
}


static int
find_item_nbr(m, event, row, column)
    register Xv_menu_info *m;
    Event *event;
    int *row;
    int *column;
{
    int newitem;

    *column = event->ie_locx - m->menurect.r_left - WALKMENU_BORDER(m);
    if (*column < 0)
	*column = m->nitems;	/* Outside menu proper */
    else {
	*column /=  m->default_image.width;
	if (*column >= m->ncols)
	    *column = m->ncols - 1;
    }

    *row = (event->ie_locy - m->menurect.r_top - WALKMENU_BORDER(m))
	/ m->default_image.height;
    if (*row < 0)
	*row = 0;
    else if (*row >= m->nrows)
	*row = m->nrows - 1;

    newitem = m->column_major ?
	*column * m->nrows + *row + 1 :
	*row * m->ncols + *column + 1;

    return newitem;
}


static void
process_event(event)
    Event          *event;
{
    register Xv_menu_info *m = menu_active_menu;
    register Xv_menu_item_info *mi;
    int		    color_index = -1;	/* use default foreground color */
    int             column, row;
    int             default_position = m->default_position;
    int             font_height;
    int             left_bdry;	/* left boundary of menu button stack */
    int             mi_top;	/* menu item top */
    int             newitem;
    int             newitem_tracks_pointer;
    /*
     * TRUE: newitem is derived from pointer position; FALSE: newitem is
     * derived from default position (pointer is off the menu or over the
     * menu title)
     */
    int             right_margin;
    int             submenu_stay_up;
    Menu_feedback   feedback_state;
    Rect            itemrect;
    Rect            menu_mark_rect;
    Rect            mi_paint_rect;	/* rect in menu item containing oval
					 * or box */

    /*
     * Track the mouse.
     */
    newitem_tracks_pointer = TRUE;
    m->group_info->previous_event = m->group_info->last_event;
    m->group_info->last_event = *event;
    if (rect_includespoint(&m->menurect, event->ie_locx, event->ie_locy)) {
	/*
	 * Pointer is in the current menu
	 */
	newitem = find_item_nbr(m, event, &row, &column);
	if (newitem == 1 && m->pin) {
	    /*
	     * Determine if pointer is over pushpin
	     */
	    if (rect_includespoint(&m->pushpin_rect, event->ie_locx,
				   event->ie_locy)) {
		/*
		 * Pointer is over pushpin: Change image from pin-out to
		 * pin-in or default-pin-out
		 */
		if (m->rendered)
		    xv_glyph_char(m->window, m->pushpin_left, m->pushpin_top,
				  MENU_PIN_WIDTH, MENU_PIN_HEIGHT,
				  m->glyph_font, m->setting_default ?
				  OLG_MENU_DEFAULT_PIN_OUT : OLG_MENU_PIN_IN,
				  color_index);
	    } else {
		newitem = 0;	/* pointer is not over any item or pushpin */
		newitem_tracks_pointer = FALSE;
	    }
	}
	if (newitem) {
	    if (newitem > m->nitems) {
		newitem = 0;
		newitem_tracks_pointer = FALSE;
	    } else {
		/* Get menu window rect containing menu item oval/box */
		compute_menu_item_paint_rect(m, newitem, &mi_paint_rect,
					     &mi_top);
		/*
		 * Translate to client window space and account for oval/box
		 * whitespace-border width.  Use full menuitem rect height to
		 * avoid selecting default when dragging up or down items.
		 */
		mi_paint_rect.r_left += m->menurect.r_left + 1;
		mi_paint_rect.r_top = m->menurect.r_top + mi_top;
		mi_paint_rect.r_height = m->default_image.height;
		/* If we're not over the oval/box, then there's no new item */
		if (!rect_includespoint(&mi_paint_rect,
					event->ie_locx, event->ie_locy)) {
		    newitem = 0;
		    newitem_tracks_pointer = FALSE;
		}
	    }
	}
    } else {			/* Pointer is not in the current menu */
	if (!m->stay_up
	    && m->group_info->depth > 1
	    && event->ie_locx <= m->menurect.r_left) {
	    /* Pointer is to the left of the submenu: return to parent */
	    m->status = MENU_STATUS_PARENT;
	    cleanup(CLEANUP_CLEANUP);
	    return;
	}
	newitem = 0;
	newitem_tracks_pointer = FALSE;
    }

    if (m->pin			/* there's a pushpin */
	&& !rect_includespoint(&m->pushpin_rect, event->ie_locx, event->ie_locy)) {
	/*
	 * Pointer is not over pushpin
	 */
	if (newitem != m->curitem && m->rendered)
	    /* Change pushpin glyph */
	    xv_glyph_char(m->window, m->pushpin_left, m->pushpin_top,
			  MENU_PIN_WIDTH, MENU_PIN_HEIGHT, m->glyph_font,
			  (!newitem && m->default_position == 1) ?
			      OLG_MENU_DEFAULT_PIN_OUT : OLG_MENU_PIN_OUT,
			  color_index);
    }
    /*
     * Provide feedback for new item.
     */
    /* clear old item */
    if (newitem != m->curitem) {
	m->drag_right_distance = 0;
	switch (m->class) {
	  case MENU_COMMAND:
	    if (!m->curitem)
		m->curitem = m->default_position;
	    feedback(m, m->curitem, MENU_REMOVE_FEEDBACK);
	    break;

	  case MENU_CHOICE:
	    if (newitem == 0) {
		/*
		 * Pointer has been moved off of menu. Note that this is only
		 * true when m->setting_default == FALSE.
		 */
		if (m->selected_position > 0)
		    newitem = m->selected_position;
		else if (default_position > 0)
		    newitem = default_position;
		else
		    newitem = 1;
		newitem_tracks_pointer = FALSE;
	    }
	    if (newitem != m->curitem) {
		feedback_state = MENU_REMOVE_FEEDBACK;
		if (m->setting_default) {
		    if (m->curitem == m->selected_position)
			feedback_state = MENU_PROVIDE_FEEDBACK;
		} else if (m->curitem == default_position)
		    feedback_state = MENU_DEFAULT_FEEDBACK;
		feedback(m, m->curitem, feedback_state);
	    }
	    break;

	  case MENU_TOGGLE:
	    if (!m->curitem && m->setting_default)
		m->curitem = m->default_position;
	    if (m->curitem) {
		if (!m->setting_default)
		    m->item_list[m->curitem - 1]->toggle_on =
			(!m->item_list[m->curitem - 1]->toggle_on);
		feedback(m, m->curitem,
			 m->item_list[m->curitem - 1]->toggle_on ?
			 MENU_PROVIDE_FEEDBACK :
		   (default_position == m->curitem && !m->setting_default) ?
			 MENU_DEFAULT_FEEDBACK : MENU_REMOVE_FEEDBACK);
	    }
	    break;
	}

	/* Update new item, if any */
	if (!newitem && (m->class != MENU_TOGGLE || m->setting_default))
	    newitem = m->default_position;
	if (newitem) {
	    feedback_state = m->setting_default ? MENU_DEFAULT_FEEDBACK :
		(default_position != newitem) ?
		MENU_PROVIDE_FEEDBACK :
		(newitem_tracks_pointer || m->class == MENU_CHOICE) ?
		MENU_SELECTED_DEFAULT_FEEDBACK : MENU_DEFAULT_FEEDBACK;
	    switch (m->class) {

	      case MENU_COMMAND:
		feedback(m, newitem, feedback_state);
		break;

	      case MENU_CHOICE:
		if (newitem != m->curitem) {
		    feedback(m, newitem, feedback_state);
		    m->curitem = newitem;
		}
		break;

	      case MENU_TOGGLE:
		if (m->setting_default == 0) {
		    if (m->item_list[newitem - 1]->toggle_on =
			(!m->item_list[newitem - 1]->toggle_on))
			feedback_state = MENU_PROVIDE_FEEDBACK;
		    else if (default_position == newitem)
			feedback_state = MENU_DEFAULT_FEEDBACK;
		    else
			feedback_state = MENU_REMOVE_FEEDBACK;
		} else
		    feedback_state = MENU_DEFAULT_FEEDBACK;
		feedback(m, newitem, feedback_state);
		break;
	    }
	    if (m->setting_default)
		m->pending_default_position = default_position = newitem;
	    if (!newitem_tracks_pointer)
		newitem = 0;
	}
	if (m->class != MENU_CHOICE)
	    m->curitem = newitem;
    }
    if (newitem) {
	/*
	 * If item is a menu, recurse.
	 */
	mi = m->item_list[newitem - 1];
	if (mi->pullright) {
	    left_bdry = m->menurect.r_left + WALKMENU_BORDER(m)
		+ (column * m->default_image.width);
	    /*
	     * Note: Right margin computation mimics that employed by
	     * panel_paint_button.
	     */
	    font_height =
		image_get_font(&mi->image, &m->default_image)->pf_defaultsize.y;
	    if (font_height <= 12)
		right_margin = 8;
	    else if ((font_height > 12) && (font_height < 19))
		right_margin = 9;
	    else
		right_margin = 11;
	    menu_mark_rect.r_left = left_bdry + mi->image.button_pos.x +
		m->default_image.button_size.x - right_margin -
		panel_pullright_pr.pr_width;
	    menu_mark_rect.r_top = m->menurect.r_top + WALKMENU_BORDER(m) +
		row * m->default_image.height + mi->image.button_pos.y;
	    menu_mark_rect.r_width = panel_pullright_pr.pr_width;
	    menu_mark_rect.r_height = mi->image.button_size.y;
	    itemrect.r_left = left_bdry + mi->image.button_pos.x;
	    itemrect.r_top = m->menurect.r_top + WALKMENU_BORDER(m) +
		row * m->default_image.height + mi->image.button_pos.y;
	    itemrect.r_width = m->default_image.button_size.x
		- 1;		/* decrement width because there's no button
				 * shadow */
	    itemrect.r_height = mi->image.button_size.y;
	    if (!mi->inactive	/* if active item... */
		&& (mi->value || mi->gen_pullright)	/* has submenu... */
		&&(action_menu_is_down(event)
		   || event_action(event) == ACTION_MENU)	/* MENU is down... */
		&&compute_show_submenu(m, event, &menu_mark_rect,
				       &submenu_stay_up)
		) {		/* ... then render submenu */
		/*
		 * If showing submenu in stay up mode, then gray-out menu
		 * item
		 */
		if (m->class == MENU_COMMAND && submenu_stay_up &&
		    !m->setting_default)
		    feedback(m, m->curitem, MENU_BUSY_FEEDBACK);
		/* Recurse */
		m->group_info->first_event = m->group_info->last_event = *event;

		render_pullright(m, mi, &m->menurect, &itemrect, m->font_size,
				 submenu_stay_up);

		goto get_next_input_event;
	    }
	}
    }
    /*
     * If button up is the menu button, then exit this menu. If SELECT-up in
     * Click-Move-Click (m->stay_up == TRUE), then select menu item and exit
     * menu.
     */
    if (m->read_inputevent &&
	event_is_up(event) &&
	(event_action(event) == ACTION_MENU ||
	 (m->stay_up && event_action(event) == ACTION_SELECT))) {
	if (m->curitem == 0 || m->item_list[m->curitem - 1]->inactive) {
	    if (!m->parent || !m->stay_up) {
		/*
		 * MENU-up on base menu, or MENU-up on Press-Drag-Release
		 * submenu: abort menu chain.
		 */
		cleanup(CLEANUP_ABORT);
		return;
	    } else {
		/* MENU-up on Click-Move-Click submenu: reuse event */
		m->status = MENU_STATUS_PARENT;
		cleanup(CLEANUP_CLEANUP);
		return;
	    }
	}
	mi = m->item_list[m->curitem - 1];
	if (mi->pullright) {
	    if ((!m->stay_up && event_action(event) == ACTION_MENU) ||
		event_action(event) == ACTION_SELECT)
		menu_select_default(mi->value);
	    else
		/*
		 * MENU or SELECT up over row containing pullright, but not
		 * over the active area of the pullright: abort menu group
		 */
		newitem_tracks_pointer = FALSE;
	}
	if (newitem_tracks_pointer) {
	    if (m->pin && newitem == 1 && !m->setting_default) {
		m->pin_window_pos.x = m->fs_menurect.r_left;
		m->pin_window_pos.y = m->fs_menurect.r_top;
		m->group_info->pinned_menu = m;
		m->status = MENU_STATUS_PIN;
	    } else
		m->status = MENU_STATUS_DONE;
	} else
	    m->status = MENU_STATUS_ABORT;
	cleanup(CLEANUP_EXIT);
	return;
    }
    /*
     * Get next input event.
     */
get_next_input_event:
    m->status = MENU_STATUS_BUSY;
    return;
}


static void
cleanup(cleanup_mode)
    Cleanup_mode    cleanup_mode;
{
    register Xv_menu_info *m = menu_active_menu;
    Xv_object       root_window = xv_get(menu_client_window, XV_ROOT);
    Xv_Drawable_info *info;

    /*
     * m->status 		Actions at the next higher level
     * ---------		--------------------------------
     * MENU_STATUS_{PIN,ABORT}	abort menu chain.
     * MENU_STATUS_DONE		valid selection, save selected item.
     * MENU_STATUS_PARENT	cursor has entered a parent menu.
     * MENU_STATUS_IGNORE	no menu item was selected; ignore last event
     * MENU_STATUS_DEFAULT	select default item.
     */

    switch (cleanup_mode) {
      case CLEANUP_EXIT:
	if (m->setting_default) {
	    m->default_position = m->pending_default_position;
	    menu_update_pin_image(m);	/* if applicable */
	    if (m->group_info->depth == 1)
		goto abort;
	}
      case CLEANUP_CLEANUP:
cleanup:
	if (!m->setting_default &&
	    (m->status == MENU_STATUS_DONE ||
	     m->status == MENU_STATUS_PARENT ||
	     m->status == MENU_STATUS_PIN ||
	     m->status == MENU_STATUS_ABORT) &&
	    m->curitem) {
	    m->selected_position = m->curitem;
	    if (m->class == MENU_COMMAND)
		feedback(m, m->curitem,
			 m->pending_default_position == m->curitem ?
			 MENU_DEFAULT_FEEDBACK : MENU_REMOVE_FEEDBACK);
	} else if (m->status == MENU_STATUS_DEFAULT)
	    m->selected_position = m->default_position;
	DRAWABLE_INFO_MACRO(root_window, info);
	if (m->window) {
	    xv_set(m->window, XV_SHOW, FALSE, 0);	/* DO THIS FIRST! */
	    m->active = FALSE;
	    if (m->cache_index < 0) {
		screen_set_cached_window_busy(xv_screen(info), m->window,
					      FALSE);
		m->rendered = FALSE;
		m->window = NULL;
	    }
	}
	if (m->shadow_window) {
	    xv_set(m->shadow_window, XV_SHOW, FALSE, 0);
	    if (m->cache_index < 0) {
		screen_set_cached_window_busy(xv_screen(info), m->shadow_window,
					      FALSE);
		m->shadow_window = NULL;
	    }
	}
	if (xv_get(xv_server(info), SERVER_JOURNALLING))
	    xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);
	--m->group_info->depth;
	if (m->gen_items)
	    destroy_gen_items(m);
	if (m->gen_proc) {
	    (m->gen_proc) (MENU_PUBLIC(m), MENU_DISPLAY_DONE);
	}
	if (m->group_info->depth)
	    submenu_done(m);
	else
	    menu_done(m->status);
	return;

      case CLEANUP_ABORT:
abort:
	/* No selection has been made. */
	m->status = MENU_STATUS_ABORT;
	goto cleanup;
    }
}


static void
compute_margin(font_size, margin)
    int             font_size;
    int            *margin;
{
    switch (font_size) {
      case 8:
	*margin = 4;
	break;
      case 10:
	*margin = 4;
	break;
      case 12:
	*margin = 4;
	break;
      case 14:
	*margin = 6;
	break;
      case 16:
	*margin = 6;
	break;
      case 19:
	*margin = 7;
	break;
      case 24:
	*margin = 10;
	break;
      case 32:
	*margin = 12;
	break;
      default:
	*margin = 4;
	break;
    }
}


/*
 * Compute max item size.  Only zero sized items have to be recomputed
 */
static int
compute_item_size(menu, std_image, font_size)
    Xv_menu_info   *menu;
    struct image   *std_image;
    int             font_size;
{
    register int    width, height, nitems, recompute;
    register Xv_menu_item_info *mi, **mip;
    int             gen_items = FALSE;
    int             margin;
    int             fontsize;
    struct image   *im;
    struct pr_size  max_button_size;

    nitems = menu->nitems;
    width = height = max_button_size.x = max_button_size.y = 0;
    recompute = std_image->width == 0;

    /*
     * This causes the menu to shrink around the items. When the std_image is
     * available at the client interface zeroing the size of std_image should
     * be rethought.
     */
    std_image->width = std_image->height = 0;

    switch (menu->class) {
      case MENU_COMMAND:
	margin = 0;
	break;
      case MENU_TOGGLE:
	compute_margin(font_size, &margin);
	break;
      case MENU_CHOICE:
	margin = 0;
	break;
    }
    std_image->margin = margin;

    /* Compute max size if any of the items have changed */
    for (mip = menu->item_list; mi = *mip, nitems--; mip++) {

	mi->parent = menu;

	im = &mi->image;
	im->margin = margin;

	if (mi->gen_proc) {
	    *mip = mi = MENU_ITEM_PRIVATE(
		       (mi->gen_proc) (MENU_ITEM_PUBLIC(mi), MENU_DISPLAY));
	    gen_items = TRUE;
	}
	if (recompute)
	    /* Force the image size to be recomputed */
	    im->width = 0;
	if (im->width == 0) {
	    /* Compute standard image size, including left and right pixrects */
	    image_compute_size(im, std_image);

	    /*
	     * Adjust MENU_COMMAND menu item image sizes to account for
	     * button outlines and vertically centered text.
	     */
	    if (menu->class == MENU_COMMAND) {
		fontsize = im->height - 2 * margin;
		if (fontsize < 12) {
		    im->width += 15;	/* left text margin = 7, right =
					 * width-9 */
		    im->button_size.x += 15;
		    if (im->string) {
			im->height = 17;
			im->button_size.y = 16;
		    } else {
			/* top text margin = 4, bottom = height-8 */
			im->height += 11;
			im->button_size.y += 11;
		    }
		} else if ((fontsize >= 12) && (fontsize <= 19)) {
		    im->width += 18;	/* left text margin = 9, right =
					 * width-9 */
		    im->button_size.x += 17;
		    if (im->string) {
			if (fontsize >= 14) {
			    im->height = 21;
			    im->button_size.y = 20;
			} else {
			    im->height = 19;
			    im->button_size.y = 18;
			}
		    } else {
			/* top text margin = 5, bottom = height-8 */
			im->height += 12;
			im->button_size.y += 12;
		    }
		} else {
		    im->width += 20;	/* left text margin = 9; right =
					 * width-12 */
		    im->button_size.x += 20;
		    if (im->string) {
			im->height = 25;
			im->button_size.y = 23;
		    } else {
			/* top text margin = 5; bottom = height-11 */
			im->height += 15;
			im->button_size.y += 15;
		    }
		}
		if (mi->pullright) {
		    im->width += panel_pullright_pr.pr_width;
		    im->button_size.x += panel_pullright_pr.pr_width;
		    if (im->height < panel_pullright_pr.pr_height)
			im->height = panel_pullright_pr.pr_height;
		    if (im->button_size.y < panel_pullright_pr.pr_height)
			im->button_size.y = panel_pullright_pr.pr_height;
		}
	    } else {
		/*
		 * Adjust height for 3 pixel-wide border, which extends in 1
		 * pixel on each side.
		 */
		im->height += 6;
		im->width += 6;
	    }
	}
	width = imax(im->width, width);
	height = imax(im->height, height);
	max_button_size.x = imax(im->button_size.x, max_button_size.x);
	max_button_size.y = imax(im->button_size.y, max_button_size.y);

    }				/* end of for(each menu item) loop */

    if (menu->class == MENU_COMMAND)
	std_image->width = std_image->margin * 2 + std_image->left_margin
	    + width + std_image->right_margin;
    else
	std_image->width = width;
    std_image->height = height;
    std_image->button_size = max_button_size;

    return gen_items;
}


/*
 * Compute the dimensions of the menu.
 */
static int
compute_dimensions(menu, iwidth, iheight, rect)
    register Xv_menu_info *menu;
    int             iwidth, iheight;
    register Rect  *rect;
{
    register int    ncols, nrows;
    Xv_object       root_window = xv_get(menu_client_window, XV_ROOT);
    Rect           *fs_screenrect = (Rect *) xv_get(root_window, XV_RECT);

    ncols = menu->ncols;
    nrows = menu->nrows;

    if (ncols * nrows < menu->nitems)
	nrows = 0;		/* not enough space: add more rows */
    if (!(ncols && nrows)) {
	if (ncols) {		/* case: ncols=n, nrows=to fit */
	    rect->r_width = (ncols * iwidth) + 2 * WALKMENU_BORDER(menu);
	    if (rect->r_width > fs_screenrect->r_width) {
		ncols = (fs_screenrect->r_width - 2 * WALKMENU_BORDER(menu))
		    / iwidth;
	    }
	    nrows = (menu->nitems - 1) / ncols + 1;
	} else {		/* case: nrows=n, ncols=to fit */
	    if (!nrows)
		nrows = menu->nitems;	/* Both zero, fit cols */
	    rect->r_height = (nrows * iheight) + 2 * WALKMENU_BORDER(menu);
	    if (rect->r_height > fs_screenrect->r_height) {
		nrows = (fs_screenrect->r_height
			 - 2 * WALKMENU_BORDER(menu)) / iheight;
		ncols = (menu->nitems - 1) / nrows + 1;
		nrows = (menu->nitems - 1) / ncols + 1;
	    } else {
		ncols = (menu->nitems - 1) / nrows + 1;
	    }
	}
    }
    rect->r_width = (ncols * iwidth) + 2 * WALKMENU_BORDER(menu);
    rect->r_height = (nrows * iheight) + 2 * WALKMENU_BORDER(menu);
    if (rect->r_width > fs_screenrect->r_width - SCREEN_MARGIN
	|| rect->r_height > fs_screenrect->r_height - SCREEN_MARGIN) {
	xv_error(0, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, "menu_show: Menu too large for screen",
		 XV_ERROR_PKG_NAME, "Menu",
		 0);
	return FALSE;
    }
    menu->ncols = ncols;
    menu->nrows = nrows;

    return TRUE;
}


/*
 * Compute 3 rects: menu->menurect = menu rectangle relative to input event
 * window mrect = menu rectangle in screen coordinates srect = shadow
 * rectangle in screen coordinates
 */
static void
compute_rects(menu, iep, item_width, item_height, mrect, srect)
    Xv_menu_info   *menu;
    struct inputevent *iep;
    int             item_width, item_height;
    Rect           *mrect, *srect;	/* menu and shadow rect, in screen
					 * coordinates */
{
    int             constrained;
    int             left, top;
    int             mouse_x, mouse_y;
    Rect           *enable_rect = &menu->enable_rect;
    Rect           *position_rect = &menu->position_rect;
    Xv_object       rootwindow = xv_get(iep->ie_win, XV_ROOT);
    Rect           *rootrect = (Rect *) xv_get(rootwindow, WIN_RECT);

    if (menu->popup) {
	/* Popup menu */
	menu->menurect.r_left = iep->ie_locx;
	if (menu->group_info->depth > 1) {
	    /*
	     * Submenu: position pointer over default item.  Center of
	     * submenu default item equals center of parent menu item.
	     */
	    menu->menurect.r_left -= WALKMENU_BORDER(menu) +
		menu->item_list[menu->default_position - 1]->image.button_pos.x;
	    menu->menurect.r_top = position_rect->r_top +
		position_rect->r_height / 2;
	} else {
	    /*
	     * Base menu: position pointer just outside of menu.  Center of
	     * submenu default item equals pointer y-coordinate.
	     */
	    menu->menurect.r_left++;
	    menu->menurect.r_top = iep->ie_locy;
	}
	menu->menurect.r_top -= (menu->default_position - 1) * item_height +
	    item_height / 2;
    } else {
	if (enable_rect->r_width)	/* if an enable rect has been defined */
	    position_rect = enable_rect;
	if (menu->pulldown) {
	    /* Pulldown menu */
	    menu->menurect.r_left = position_rect->r_left +
		(position_rect->r_width ?
	    /* Centered pulldown */
		 (position_rect->r_width / 2 - mrect->r_width / 2) :
	    /* Flush left: account for menu border width */
		 1);
	    menu->menurect.r_top = position_rect->r_top +
		position_rect->r_height;
	} else {
	    /* Pullright menu */
	    menu->menurect.r_left = position_rect->r_left +
		position_rect->r_width;
	    menu->menurect.r_top = position_rect->r_top +
		position_rect->r_height / 2 -
		(menu->default_position - 1) * item_height - item_height / 2;
	}
    }
    menu->menurect.r_width = mrect->r_width;
    menu->menurect.r_height = mrect->r_height;

    /* Convert to screen coordinates */
    win_translate_xy(iep->ie_win, rootwindow,
		     menu->menurect.r_left, menu->menurect.r_top,
		     &left, &top);
    mrect->r_left = left;
    mrect->r_top = top;

    /*
     * Make sure menu and shadow windows are entirely visible on the screen
     */
    mrect->r_width += MENU_SHADOW;
    mrect->r_height += MENU_SHADOW;
    /* adjust fullscreen menu rect */
    constrained = constrainrect(mrect, rootrect);
    mrect->r_width -= MENU_SHADOW;
    mrect->r_height -= MENU_SHADOW;
    /* adjust input-event window menu rect */
    menu->menurect.r_left += mrect->r_left - left;
    menu->menurect.r_top += mrect->r_top - top;

    /* Shadow rect is menu rect offset by MENU_SHADOW */
    *srect = *mrect;
    srect->r_left += MENU_SHADOW;
    srect->r_top += MENU_SHADOW;

    /*
     * If menu was moved to make it fit in the screen, then jump pointer to
     * the left of or over the default item.
     */
    if (constrained) {
	mouse_x = mrect->r_left;
	if (menu->group_info->depth > 1) {
	    /* Submenu: position pointer over default item. */
	    mouse_x += WALKMENU_BORDER(menu) +
		menu->item_list[menu->default_position - 1]->image.button_pos.x;
	} else {
	    /* Base menu: position pointer just outside of menu. */
	    mouse_x--;
	}
	mouse_y = mrect->r_top + (menu->default_position) * item_height -
	    item_height / 2;
	win_setmouseposition(rootwindow, mouse_x, mouse_y);
	menu->ptr_jumped = TRUE;
    }
}


/*
 * Paint items into menu window
 */
static void
render_items(menu, std_image, window)
    Xv_menu_info   *menu;
    struct image   *std_image;
    Xv_opaque       window;
{
    register int    top, left, n, i, j;
    register Xv_menu_item_info **mip;
    int             item_width, item_height;
    register        total_height, total_width;

    item_width = std_image->width;
    item_height = std_image->height;
    mip = menu->item_list;

    n = 0;
    total_height = menu->menurect.r_height;
    total_width = menu->menurect.r_width;
    if (menu->column_major) {
	left = WALKMENU_BORDER(menu);
	for (j = 0; j < menu->ncols; j++) {
	    top = WALKMENU_BORDER(menu);
	    for (i = 0; i < menu->nrows; i++) {
		if (++n > menu->nitems)
		    break;
		if (menu->class != MENU_COMMAND || (*mip)->title) {
		    if ((*mip)->inactive) {
			image_render(&(*mip)->image, std_image,
				     IMAGE_REGION, window, left, top,
				     IMAGE_INACTIVE,
				     IMAGE_SELECTED, (*mip)->selected,
				 IMAGE_DEFAULT, n == menu->default_position,
				     IMAGE_TITLE, (*mip)->title,
				     0);
		    } else {
			image_render(&(*mip)->image, std_image,
				     IMAGE_REGION, window, left, top,
				     IMAGE_SELECTED, (*mip)->selected,
				 IMAGE_DEFAULT, n == menu->default_position,
				     IMAGE_TITLE, (*mip)->title,
				     0);
		    }
		}
		if (menu->h_line || (*mip)->h_line) {
		    if ((*mip)->title)
			image_vector(MENU_TITLE_MARGIN, top + item_height - 4,
				     total_width - 1 - MENU_TITLE_MARGIN, top + item_height - 4);
		    else
			image_vector(0, top + item_height, total_width - 1, top + item_height);
		}
		if (menu->v_line || (*mip)->v_line) {
		    image_vector(left + item_width, 0, left + item_width, total_height - 1);
		}
		top += item_height;
		mip++;
	    }
	    left += item_width;
	}
    } else {
	top = WALKMENU_BORDER(menu);
	for (i = n = 0; i < menu->nrows; i++) {
	    left = WALKMENU_BORDER(menu);
	    for (j = 0; j < menu->ncols; j++) {
		if (++n > menu->nitems)
		    break;
		if (menu->class != MENU_COMMAND || (*mip)->title) {
		    if ((*mip)->inactive)
			image_render(&(*mip)->image, std_image,
				     IMAGE_REGION, window, left, top,
				     IMAGE_INACTIVE,
				     IMAGE_SELECTED, (*mip)->selected,
				 IMAGE_DEFAULT, n == menu->default_position,
				     IMAGE_TITLE, (*mip)->title,
				     0);
		    else {
			image_render(&(*mip)->image, std_image,
				     IMAGE_REGION, window, left, top,
				     IMAGE_SELECTED, (*mip)->selected,
				 IMAGE_DEFAULT, n == menu->default_position,
				     IMAGE_TITLE, (*mip)->title,
				     0);
		    }
		}
		if (menu->h_line || (*mip)->h_line) {
		    if ((*mip)->title)
			image_vector(MENU_TITLE_MARGIN, top + item_height - 2,
				     total_width - 1 - MENU_TITLE_MARGIN, top + item_height - 2);
		    else
			image_vector(0, top + item_height, total_width - 1, top + item_height);
		}
		if (menu->v_line || (*mip)->v_line) {
		    image_vector(left + item_width, 0, left + item_width, total_height - 1);
		}
		left += item_width;
		mip++;
	    }
	    top += item_height;
	}
    }
}


/*
 * Handle recursive calls for pullright items
 */
static          Menu_status
render_pullright(parent_menu, mi, menurect, position_rect, font_size, stay_up)
    register Xv_menu_info *parent_menu;
    register Xv_menu_item_info *mi;
    Rect           *menurect;
    Rect           *position_rect;
    int             font_size;
    int             stay_up;	/* TRUE or FALSE */
{
    register Xv_menu_info *m;
    register Menu   gen_menu, (*gen_proc) ();

    if (gen_proc = mi->gen_pullright) {
	gen_menu = gen_proc(MENU_ITEM_PUBLIC(mi), MENU_DISPLAY);
	if (!gen_menu) {
	    xv_error(mi, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
	    "render_pullright: gen_proc failed to generate a pullright menu",
		     XV_ERROR_PKG_NAME, "Menu",
		     0);
	    return MENU_STATUS_ABORT;
	}
	m = MENU_PRIVATE(gen_menu);
	mi->value = (Xv_opaque) m;
    } else {
	m = MENU_PRIVATE(mi->value);
    }

    /* Active menu is now the submenu */
    menu_active_menu = m;
    mi->parent->group_info->selected_menu = m;

    /* Ensure default image fonts valid */
    if (m->default_image.font == 0)
	m->default_image.font = parent_menu->default_image.font;
    if (m->default_image.bold_font == 0)
	m->default_image.bold_font = parent_menu->default_image.bold_font;

    /* Render submenu */
    m->enable_rect.r_width = 0;	/* submenus do not have an enable rect */
    m->popup = !stay_up;
    m->position_rect = *position_rect;
    m->ptr_jumped = FALSE;
    m->pulldown = FALSE;
    m->state = MENU_STATE_INIT;
    m->stay_up = stay_up;
    menu_render(m, mi->parent->group_info, mi, TRUE, font_size);
}


static void
submenu_done(m)
    register Xv_menu_info *m;
{
    register Xv_menu_item_info *mi = m->parent;
    register        Menu(*gen_proc) ();
    Xv_menu_info   *parent_menu = mi->parent;

    if (gen_proc = mi->gen_pullright)
	mi->value = (Xv_opaque) (gen_proc) (MENU_ITEM_PUBLIC(mi), MENU_DISPLAY_DONE);

    parent_menu->status = m->status;

    /* Active menu is now the parent of this submenu */
    menu_active_menu = parent_menu;

    /* Clean up the parent menu */
    switch (m->status) {
      case MENU_STATUS_PIN:
      case MENU_STATUS_DONE:
	cleanup(CLEANUP_EXIT);
	break;
      case MENU_STATUS_ABORT:
	cleanup(CLEANUP_ABORT);
	break;
      case MENU_STATUS_PARENT:
	m->group_info->selected_menu = parent_menu;
	break;
      case MENU_STATUS_IGNORE:
	break;
      case MENU_STATUS_DEFAULT:
	m->status = MENU_STATUS_DONE;
	cleanup(CLEANUP_EXIT);
	break;
    }
}


/*
 * Provide feedback directly to the pixwin. Someday this should be a client
 * settable option.
 */
static void
feedback(m, n, state)
    register Xv_menu_info *m;	/* ptr to menu information structure */
    int             n;		/* index of current menu item */
    Menu_feedback   state;
{
    Xv_menu_item_info *mi = m->item_list[n - 1];

    if (!mi->no_feedback && !mi->inactive)
	paint_menu_item(m, n, state);
}


static void
paint_menu_item(m, n, state)
    register Xv_menu_info *m;	/* ptr to menu information structure */
    int             n;		/* current menu item number */
    Menu_feedback   state;
{
    Xv_menu_item_info *mi = m->item_list[n - 1];
    Rect            rect;
    int		    color_index = -1;	/* use default foreground color */
    short           busy = FALSE;
    short           selected;
    short           default_item;
    int             mi_top;	/* placeholder for
				 * compute_menu_item_paint_rect output
				 * parameter */

    if (m->group_info->depth > 1 && state == MENU_DEFAULT_FEEDBACK &&
	!m->setting_default)
	state = MENU_REMOVE_FEEDBACK;

    switch (state) {
      case MENU_BUSY_FEEDBACK:
	busy = TRUE;		/* shade the interior gray */
	selected = FALSE;
	default_item = TRUE;	/* draw outer ring */
	break;
      case MENU_REMOVE_FEEDBACK:
	selected = FALSE;
	default_item = FALSE;
	break;
      case MENU_PROVIDE_FEEDBACK:
	selected = TRUE;
	default_item = FALSE;
	break;
      case MENU_SELECTED_DEFAULT_FEEDBACK:
	selected = TRUE;
	default_item = TRUE;
	break;
      case MENU_DEFAULT_FEEDBACK:
	selected = FALSE;
	default_item = TRUE;
	break;
    }

    /* Determine dimensions of menu item paint rectangle */
    compute_menu_item_paint_rect(m, n, &rect, &mi_top);

    /* Paint menu item */
    switch (m->class) {
      case MENU_COMMAND:
	panel_paint_button(
			   m->window,	/* paint window */
			   NULL,	/* Graphics Context */
			   &rect,	/* position and dimensions of button */
			   image_get_font(&mi->image, &m->default_image),	/* font */
			   mi->image.string,	/* text string, or ... */
			   mi->image.pr,	/* ... pixrect image */
			   color_index,
			   selected,	/* invert the button */
			   mi->pullright ? &panel_pullright_pr : 0,
	/* menu mark pixrect.  nil => no menu attached to button */
			   default_item,
			   0,	/* width (0= no trunction necessary) */
			   TRUE	/* button is a menu item */
	    );
	if (busy)
	    panel_paint_busy_button(
				    m->window,	/* paint window */
			      image_get_font(&mi->image, &m->default_image),	/* font */
				    &rect,	/* position and dimensions of
						 * button */
				    color_index);
	break;

      case MENU_TOGGLE:
      case MENU_CHOICE:
	panel_paint_choice_outline(m->window, &rect,
				   default_item, selected, color_index);
	break;


    }

    /* Gray out an inactive item */
    if (mi->inactive)
	xv_replrop(m->window, rect.r_left, rect.r_top,
		   rect.r_width, rect.r_height,
		   color_index < 0 ? PIX_SRC & PIX_DST :
		   PIX_SRC & PIX_DST | PIX_COLOR(color_index),
		   &gray_pr, 0, 0);
}


static void
compute_menu_item_paint_rect(m, item_nbr, rect, item_top)
    register Xv_menu_info *m;	/* ptr to menu information structure */
    register int    item_nbr;	/* current menu item number */
    register Rect  *rect;	/* paint rect (output parameter) */
    int            *item_top;	/* menu item top (output parameter) */
{
    int             margin = m->default_image.margin;
    Xv_menu_item_info *mi = m->item_list[item_nbr - 1];
    int             item_left;

    item_nbr--;
    item_left = WALKMENU_BORDER(m)
	+ (m->column_major ? item_nbr / m->nrows : item_nbr % m->ncols)
	* m->default_image.width;
    *item_top = WALKMENU_BORDER(m)
	+ (m->column_major ? item_nbr % m->nrows : item_nbr / m->ncols)
	* m->default_image.height;

    switch (m->class) {

      case MENU_COMMAND:
	rect->r_left = item_left + mi->image.button_pos.x;
	rect->r_top = *item_top + mi->image.button_pos.y;
	rect->r_width = m->default_image.button_size.x;
	rect->r_height = mi->image.button_size.y;
	break;

      case MENU_TOGGLE:
      case MENU_CHOICE:
	rect->r_left = item_left + margin - 2;
	rect->r_top = *item_top + margin - 2;
	rect->r_width = m->default_image.width - 2 * margin + 1;
	rect->r_height = m->default_image.height - 2 * margin + 1;
	break;
    }

}


/*
 * Menu must be completely on the screen.
 */
static int
constrainrect(rconstrain, rbound)
    register struct rect *rconstrain, *rbound;
{
    int             constrained = FALSE;

    /*
     * Bias algorithm to have too big rconstrain fall off right/bottom.
     */
    if (rect_right(rconstrain) > rect_right(rbound)) {
	rconstrain->r_left = rbound->r_left + rbound->r_width
	    - rconstrain->r_width;
	constrained = TRUE;
    }
    if (rconstrain->r_left < rbound->r_left) {
	rconstrain->r_left = rbound->r_left + SCREEN_MARGIN;
	constrained = TRUE;
    }
    if (rect_bottom(rconstrain) > rect_bottom(rbound)) {
	rconstrain->r_top = rbound->r_top + rbound->r_height
	    - rconstrain->r_height;
	constrained = TRUE;
    }
    if (rconstrain->r_top < rbound->r_top) {
	rconstrain->r_top = rbound->r_top + SCREEN_MARGIN;
	constrained = TRUE;
    }
    return (constrained);
}


/*
 * Clean up any client generated items
 */
static void
destroy_gen_items(menu)
    Xv_menu_info   *menu;
{
    register int    nitems;
    register Xv_menu_item_info *mi, **mip;

    nitems = menu->nitems;
    /* Give client a chance to clean up any generated items */
    for (mip = menu->item_list; mi = *mip, nitems--; mip++)
	if (mi->gen_proc)
	    *mip = MENU_ITEM_PRIVATE(
		  (mi->gen_proc) (MENU_ITEM_PUBLIC(mi), MENU_DISPLAY_DONE));
}


static int
absolute_value(x)
    int             x;
{
    return (x < 0 ? -x : x);
}


/* Return: TRUE= show submenu; FALSE= don't show submenu */
static short
compute_show_submenu(m, event, submenu_region_rect, submenu_stay_up)
    Xv_menu_info   *m;
    Event          *event;
    Rect           *submenu_region_rect;
    int            *submenu_stay_up;
{
    register short  x_delta;

    *submenu_stay_up = FALSE;	/* assume Press-Drag-Release */

    /* In Click-Move-Click mode, if event is MENU-up, then show submenu. */
    if (m->stay_up
	&& event_action(event) == ACTION_MENU
	&& event_is_up(event)
	&& m->group_info->menu_down_event.action) {
	*submenu_stay_up = TRUE;
	return (TRUE);
    }
    /*
     * If event is LOC_DRAG, and we've continuously moved to the right the
     * specified drag-right distance, or we're over the submenu region, then
     * show the submenu. FYI, event->ie_locx == m->group_info->last_event.
     */
    if (event_action(event) == LOC_DRAG) {
	if (rect_includespoint(submenu_region_rect,
			       event->ie_locx, event->ie_locy)) {
	    /* Over the submenu region: bring up submenu */
	    m->drag_right_distance = 0;
	    return (TRUE);
	}
	x_delta = event->ie_locx - m->group_info->previous_event.ie_locx;
	if (x_delta <= 0) {
	    /* User has moved left: cancel the drag right */
	    m->drag_right_distance = 0;
	    return (FALSE);
	} else {
	    m->drag_right_distance += x_delta;
	    if (m->drag_right_distance > m->pullright_delta) {
		/*
		 * User has moved the pullright-delta distance right: bring
		 * up the submenu
		 */
		m->drag_right_distance = 0;
		return (TRUE);
	    }
	}
    }
    return (FALSE);
}


void
menu_window_event_proc(window, event)
    Xv_Window       window;
    Event          *event;
{
    register Xv_menu_info *m = menu_active_menu;
    char *help_data;
    int help_item;
    int row;
    int column;

    if (menu_client_window && event_id(event) == WIN_REPAINT)
	menu_window_paint(xv_get(window, XV_KEY_DATA, MENU_WINDOW_MENU),
			  window);
    else if (m && event_action(event) == ACTION_HELP && event_is_down(event)) {
	/* Note: find_item_nbr expects event coordinates to be in reference
	 * to client window.  menurect describes menu window position with
	 * respect to the client window.
	 */
	event->ie_locx += m->menurect.r_left;
	event->ie_locy += m->menurect.r_top;
	help_item = find_item_nbr(m, event, &row, &column);
	event->ie_locx -= m->menurect.r_left;
	event->ie_locy -= m->menurect.r_top;
	if (help_item)
	    help_data = (char *) xv_get(
		MENU_ITEM_PUBLIC(m->item_list[help_item-1]), XV_HELP_DATA);
	if (help_item == 0 || !help_data)
	    help_data = "xview:menu";
	xv_set(window,
	    XV_KEY_DATA, WIN_FRAME, xv_get(menu_client_window, WIN_FRAME),
	    0);
	if (xv_help_show(window, help_data, event) == XV_ERROR) {
	    /* Notice was shown.  Fullscreen notice affects GrabPointer,
	     * so we must abort menu.
	     */
	    m->curitem = 0;
	    cleanup(CLEANUP_ABORT);
	}
    }
}


void
menu_shadow_event_proc(window, event)
    Xv_Window       window;
    Event          *event;
{
    if (menu_client_window && event_id(event) == WIN_REPAINT)
	menu_shadow_paint(window);
    else if (event_action(event) == ACTION_HELP && event_is_down(event)) {
	xv_set(window,
	    XV_KEY_DATA, WIN_FRAME, xv_get(menu_client_window, WIN_FRAME),
	    0);
	if (xv_help_show(window, "xview:menu", event) == XV_ERROR) {
	    /* Notice was shown.  Fullscreen notice affects GrabPointer,
	     * so we must abort menu.
	     */
	    menu_active_menu->curitem = 0;
	    cleanup(CLEANUP_ABORT);
	}
    }
}


static void
menu_window_paint(m, window)
    register Xv_menu_info *m;
    Xv_Window       window;
{
    register int    i;
    Menu_feedback   feedback_state;
    Xv_menu_item_info *mi;

    if (m->rendered)
	return;

    /*
     * Draw the menu title and items
     */
    render_items(m, &m->default_image, window);
    i = 1;
    for (; i <= m->nitems; i++) {
	mi = m->item_list[i - 1];
	if (!mi->no_feedback) {
	    if (!m->setting_default
		&& ((m->curitem == i && m->class == MENU_CHOICE)
		    || (mi->toggle_on && m->class == MENU_TOGGLE)))
		feedback_state = MENU_PROVIDE_FEEDBACK;
	    else
		feedback_state = (i == m->default_position) ?
		    MENU_DEFAULT_FEEDBACK : MENU_REMOVE_FEEDBACK;
	    paint_menu_item(m, i, feedback_state);
	    if (i == m->curitem && m->class == MENU_COMMAND)
		/* Invert current item */
		paint_menu_item(m, i, MENU_PROVIDE_FEEDBACK);
	}
    }

    m->rendered = TRUE;
}


static void
menu_shadow_paint(window)
    Xv_Window       window;
{
    Rect            rect;
    Server_image    shadow_svrim;
    Xv_object       screen = xv_get(window, XV_SCREEN);
    Xv_object       server = xv_get(screen, SCREEN_SERVER);

    /* Fill in shadow window with shadow pattern */
    rect = *(Rect *) xv_get(window, WIN_RECT);
    shadow_svrim = xv_get(server, XV_KEY_DATA, MENU_SHADOW_SVRIM);
    xv_replrop(window, rect.r_width - MENU_SHADOW, 0,
	       MENU_SHADOW, rect.r_height - MENU_SHADOW,
	       PIX_SRC, shadow_svrim, 0, 0);
    xv_replrop(window, 0, rect.r_height - MENU_SHADOW,
	       rect.r_width, MENU_SHADOW,
	       PIX_SRC, shadow_svrim, 0, 0);
}

Pkg_private     Notify_value
menu_client_window_event_proc(win, event, arg, type)
    Xv_Window       win;	/* menu client window */
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    register Xv_menu_info *m = menu_active_menu;

    if (!m)
	return (notify_next_event_func(win, event, arg, type));

    switch (event_action(event)) {
      case ACTION_MENU:
	if (event_is_down(event))
	    menu_active_menu->group_info->menu_down_event = *event;
	/* fall through */
      case ACTION_SELECT:
      case LOC_DRAG:
	break;
      default:
	return (NOTIFY_IGNORED);
    }

    if (menu_active_menu->state == MENU_STATE_TRACK_MOUSE)
	m->read_inputevent = TRUE;
    else if (menu_active_menu->state == MENU_STATE_GET_MODE)
	get_mode(event);	/* may change menu state and
				 * m->read_inputevent */

    if (menu_active_menu->state == MENU_STATE_TRACK_MOUSE)
	do {
	    process_event(event);
	} while (menu_active_menu &&
		 menu_active_menu->status == MENU_STATUS_PARENT);

    if (menu_active_menu
	&& event_action(event) == ACTION_MENU
	&& event_is_up(event))
	/* MENU-up processed: corresponding MENU-down event no longer valid */
	menu_active_menu->group_info->menu_down_event.action = 0;

    return (NOTIFY_DONE);
}


Pkg_private void
menu_cache_release_menu(m)
    register Xv_menu_info *m;
{
    register int i;

    for (i=0; i<MENU_CACHE_SIZE; i++) {
        if (menu_cache[i] == m) {
            menu_cache[i] = NULL;
        }
    }    
}
