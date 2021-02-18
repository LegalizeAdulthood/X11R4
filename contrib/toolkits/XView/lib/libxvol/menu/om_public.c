#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)om_public.c 20.80 89/10/17";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview/win_input.h>	/* includes types & time */
#ifndef FILE
#undef NULL
#include <stdio.h>
#endif				/* FILE */
#include <fcntl.h>

#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <pixrect/pr_util.h>
#include <pixrect/memvar.h>

#include <xview/xview.h>

#include <xview/defaults.h>
#include <xview/font.h>
#include <xview/panel.h>
#include <xview/win_struct.h>
#include <xview/rectlist.h>

#include <xview/screen.h>
#include <xview/server.h>
#include <xview/fullscreen.h>
#include <xview/notify.h>

#include <xview_private/draw_impl.h>
#include <xview_private/om_impl.h>

#include <X11/Xlib.h>

/* ------------------------------------------------------------------------- */

/*
 * Public
 */
Xv_public Xv_opaque menu_return_value();
Xv_public void  menu_show();
Xv_public Xv_opaque menu_return_item();
Xv_public void  menu_default_pin_proc();

Sv1_public Xv_opaque menu_get();
Sv1_public Xv_opaque menu_set();
Sv1_public void menu_destroy_with_proc();
Sv1_public Menu_item menu_find();

/*
 * Toolkit private
 */
Xv_private Xv_opaque menu_return_default();
Xv_private void menu_select_default();

/*
 * Package private
 */
Pkg_private int menu_create_internal();
Pkg_private int menu_create_item_internal();
Pkg_private int menu_item_destroy_internal();
Pkg_private Xv_opaque menu_pullright_return_result();
Pkg_private Xv_opaque menu_return_no_value();
Pkg_private Xv_opaque menu_sets();
Pkg_private Xv_opaque menu_gets();
Pkg_private Xv_opaque menu_pkg_find();
Pkg_private Xv_opaque menu_item_sets();
Pkg_private Xv_opaque menu_item_gets();
Pkg_private Xv_opaque menu_return_no_value();
Pkg_private void menu_create_pin_window();
Pkg_private void menu_done();
Pkg_private void menu_render();
Pkg_private void menu_window_event_proc();
Pkg_private void menu_shadow_event_proc();
Pkg_private Notify_value menu_client_window_event_proc();

Pkg_private short menu_gray75_data[];
Pkg_private struct pixrect menu_gray75_pr;

/* destroy routines for menus and menu_items */
Pkg_private int menu_destroys(), menu_item_destroys();

/* Pkg_private */
Xv_menu_info   *menu_active_menu;	/* 0 => no menu active */
Xv_Window       menu_client_window;	/* 0 => pointer not grabbed yet */

/*
 * Private
 */
static void     menu_show_internal();
static Xv_menu_group_info group_info;
static Xv_opaque menu_return_result();

/* Cache the standard menu data obtained from the defaults database */
static Xv_menu_info *m_cache;

/*
 * Private defs
 */
#define	MENU_DEFAULT_MARGIN		1
#define	MENU_DEFAULT_LEFT_MARGIN	16
#define	MENU_DEFAULT_RIGHT_MARGIN	6
#define	Null_status	(int *)0
#define MENU_KEY 1
#define MENU_ITEM_KEY 2
#define ACTION_PROC_KEY 3

/* ------------------------------------------------------------------------- */


/*
 * Display the menu, get the menu item, and call notify proc. Default proc
 * returns a pointer to the item selected or NULL.
 */
/* VARARGS3 */
Xv_public void
menu_show(menu_public, win, iep, va_alist)
    Menu            menu_public;
    Xv_Window       win;
    struct inputevent *iep;
va_dcl				/*** WARNING: menu_show does not support ATTR_LIST. ***/
{
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);
#ifdef ATTR_MAKE
    Menu_attribute  avlist[ATTR_STANDARD_SIZE];
    va_list         valist;
#endif				/* ATTR_MAKE */
    Menu_attribute *attrs;
    Rect           *enable_rectp = 0;	/* rectangle in which menu stays up.
					 * Also describes position. 0=> popup
					 * menu */
    Rect           *position_rectp = 0;	/* describes menu position only */

#ifdef ATTR_MAKE
    va_start(valist);
    (void) attr_make((char **) avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    attrs = avlist;
#else
    attrs = (Menu_attribute *) & va_alist;
#endif				/* ATTR_MAKE */
    for (; *attrs; attrs = menu_attr_next(attrs)) {
	switch (attrs[0]) {

	  case MENU_ENABLE_RECT:
	    enable_rectp = (Rect *) attrs[1];
	    break;

	  case MENU_BUTTON:
	    event_set_id(iep, (int) attrs[1]);
	    break;

	  case MENU_POS:
	    iep->ie_locx = (int) attrs[1], iep->ie_locy = (int) attrs[2];
	    break;

	  case MENU_POSITION_RECT:
	    position_rectp = (Rect *) attrs[1];
	    break;

	  case MENU_PULLDOWN:
	    menu->pulldown = (int) attrs[1];
	    break;

	  default:
	    (void) xv_check_bad_attr(MENU, attrs);
	    break;
	}
    }

    menu_show_internal(menu_public, win, iep, enable_rectp, position_rectp);
}


static void
menu_show_internal(menu_public, win, iep, enable_rectp, position_rectp)
    Menu            menu_public;
    Xv_object       win;
    Event          *iep;
    Rect           *enable_rectp;	/* rectangle in which menu stays up.
					 * Also describes position. 0=> popup
					 * menu */
    Rect           *position_rectp;	/* describes menu position only */
{
    int             font_size;
    Xv_menu_info   *m = MENU_PRIVATE(menu_public);
    Xv_Drawable_info *info;
    Display        *display;

    if (!m) {
	xv_error(0, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "menu_show: no menu specified",
		 XV_ERROR_PKG_NAME, "Menu",
		 0);
	return;
    }
    DRAWABLE_INFO_MACRO(win, info);
    display = xv_display(info);

    /*
     * Interpose an event function on the client window to catch the MENU
     * button events.
     */
    notify_interpose_event_func(win, menu_client_window_event_proc, NOTIFY_SAFE);

    /*
     * Grab all input and disable anybody but us from writing to screen while
     * we are violating window overlapping.
     */
    if (XGrabPointer(display, xv_get(win, XV_XID),
		     False,	/* owner events: report events relative to
				 * grab window */
		     ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,	/* event mask */
		     GrabModeAsync,	/* pointer mode */
		     GrabModeAsync,	/* keyboard mode */
		     None,	/* confine to: don't confine pointer */
		     0,		/* cursor: use default window cursor */
		     CurrentTime)	/* time */
	!=GrabSuccess) {
	xv_error(0, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "menu_show: unable to grab pointer",
		 XV_ERROR_PKG_NAME, "Menu",
		 0);
	return;
    }
    m->state = MENU_STATE_INIT;
    menu_active_menu = m;
    menu_client_window = win;

    group_info.depth = 0;
    group_info.first_event = *iep;
    group_info.last_event = *iep;
    group_info.menu_down_event.action = 0;	/* no MENU-down event
						 * received yet */
    group_info.selected_menu = m;

    if (enable_rectp)
	m->enable_rect = *enable_rectp;
    else
	/* tell process_event and compute_rects that there's no enable rect */
	m->enable_rect.r_width = 0;
    if (position_rectp)
	m->position_rect = *position_rectp;
    m->popup = (enable_rectp || position_rectp) ?
	FALSE			/* menu is a pulldown or pullright */
	: TRUE;			/* menu is a popup */

    m->ptr_jumped = FALSE;
    m->stay_up = FALSE;		/* assume non-stay-up mode */

    /* Insure default image fonts valid */
    if (m->default_image.font == 0)
	m->default_image.font = (Pixfont *) xv_get(win, XV_FONT);
    if (m->default_image.bold_font == 0) {
	char           *family = (char *) xv_get(m->default_image.font, FONT_FAMILY);
	int             size = (int) xv_get(m->default_image.font, FONT_SIZE);
	if (family && *family)
	    m->default_image.bold_font = (Pixfont *) xv_find(win, FONT,
							FONT_FAMILY, family,
						FONT_STYLE, FONT_STYLE_BOLD,
			       FONT_SIZE, (size) ? size : FONT_SIZE_DEFAULT,
							     0);
	if (!m->default_image.bold_font)
	    m->default_image.bold_font = m->default_image.font;
    }
    font_size = (int) xv_get(m->default_image.font, FONT_SIZE);

    menu_render(m, &group_info, (Xv_menu_item_info *) 0, RECTNODE_NULL, FALSE,
		font_size);
}


Pkg_private void
menu_done(status)
    Menu_status     status;
{
    register Xv_menu_info *m = menu_active_menu;
    Display        *display;
    Xv_Drawable_info *info;
    Xv_opaque       result;
    Xv_Server       server;

    DRAWABLE_INFO_MACRO(menu_client_window, info);
    display = xv_display(info);
    server = xv_server(info);

    XUngrabPointer(display, CurrentTime);

    if (status == MENU_STATUS_PIN)
	(group_info.pinned_menu->pin_proc) (MENU_PUBLIC(group_info.pinned_menu),
				   group_info.pinned_menu->pin_window_pos.x,
				  group_info.pinned_menu->pin_window_pos.y);

    m->notify_status = XV_OK;	/* assume notify procedure succeeds */
    if (status == MENU_STATUS_DONE) {
	/* Sync the server */
	xv_set(server, SERVER_SYNC, FALSE, 0);
	/*
	 * Call notify_proc.  Should handle special case of selection = 0
	 */
	group_info.notify_proc = m->notify_proc;
	if (!group_info.notify_proc)
	    group_info.notify_proc = MENU_DEFAULT_NOTIFY_PROC;
	result = m->status == MENU_STATUS_DONE ?
	    menu_return_result(group_info.selected_menu, &group_info, 0) :
	    menu_return_no_value(MENU_PUBLIC(group_info.selected_menu), 0);
    } else
	result = MENU_NO_VALUE;

    /*
     * Call menu-done procedure, if any.
     */
    if (m->done_proc)
	(m->done_proc) (MENU_PUBLIC(m), result);

    notify_remove_event_func(menu_client_window, menu_client_window_event_proc,
			     NOTIFY_SAFE);
    m->group_info = NULL;
    menu_active_menu = 0;
    menu_client_window = 0;
}


/* VARARGS */
Pkg_private int
menu_create_internal(parent, object, avlist)
    Xv_opaque       parent;
    Xv_opaque       object;
    Xv_opaque      *avlist;
{
    register Xv_menu_info *m;
    register Xv_pkg *menu_type;
    Server_image    shadow_svrim;
    /*
     * BUG ALERT:  We need to pass a root window to the menu package in order
     * to use multiple screens.
     */
    Xv_object       screen = xv_default_screen;
    Xv_object       server = xv_get(screen, SCREEN_SERVER);

    ((Xv_menu *) (object))->private_data = (Xv_opaque) xv_alloc(Xv_menu_info);
    if (!(m = MENU_PRIVATE(object))) {
	xv_error(object, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "menu_create: unable to allocate menu structure",
		 XV_ERROR_PKG_NAME, "Menu",
		 0);
	return (int) XV_ERROR;
    }
    if (!m_cache) {
	if (!(m_cache = xv_alloc(Xv_menu_info))) {
	    xv_error(object, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
		     "menu_create: unable to allocate menu structure",
		     XV_ERROR_PKG_NAME, "Menu",
		     0);
	    return (int) XV_ERROR;
	}
	m_cache->column_major = TRUE;
	m_cache->default_image.center = FALSE;
	m_cache->default_image.bold_font = (Pixfont *) 0;
	m_cache->default_image.font = (Pixfont *) 0;
	m_cache->default_image.left_margin = 1;
	m_cache->default_image.margin = 1;
	m_cache->default_image.right_margin = 1;
	m_cache->default_position = 1;	/* default item is always the first
					 * if not specified */
	m_cache->display_one_level = (unsigned) FALSE;
	m_cache->extra_destroy_proc = 0;
	m_cache->notify_proc = MENU_DEFAULT_NOTIFY_PROC;
	m_cache->pin_proc = menu_default_pin_proc;
	m_cache->pullright_delta =
	    defaults_get_integer("openWindows.dragRightDistance",
				 "OpenWindows.DragRightDistance", 5);
	m_cache->stand_off = TRUE;
    }
    bcopy((Xv_opaque) m_cache, (Xv_opaque) m, sizeof(Xv_menu_info));

    /* Above bcopy() zaps all of m, so must set individual fields after it.  */
    m->public_self = object;
    m->type = (int) MENU_MENU;	/* for verifying menu handle in menu_destroys */

    /* Create the shadow's gray pattern server image */
    shadow_svrim = xv_get(server, XV_KEY_DATA, MENU_SHADOW_SVRIM);
    if (!shadow_svrim) {
	shadow_svrim = xv_create(0, SERVER_IMAGE,
				 XV_WIDTH, menu_gray75_pr.pr_width,
				 XV_HEIGHT, menu_gray75_pr.pr_height,
				 SERVER_IMAGE_DEPTH, menu_gray75_pr.pr_depth,
				 SERVER_IMAGE_BITS, menu_gray75_data,
				 0);
	if (!shadow_svrim) {
	    xv_error(object, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
		     "menu_create: unable to create shadow server image",
		     XV_ERROR_PKG_NAME, "Menu",
		     0);
	    return (int) XV_ERROR;
	}
	xv_set(server, XV_KEY_DATA, MENU_SHADOW_SVRIM, shadow_svrim, 0);
    }
    /*
     * Malloc the menu storage and create the item list
     */
    m->nitems = 0, m->max_nitems = 2 * MENU_FILLER;
    m->item_list = (Xv_menu_item_info **)
	calloc(2 * MENU_FILLER, sizeof(Xv_menu_item_info *));
    if (!m->item_list) {
	xv_error(object, XV_ERROR_RECOVERABLE, XV_ERROR_SYSTEM,
		 XV_ERROR_STRING,
		 "menu_create: unable to allocate an item list",
		 XV_ERROR_PKG_NAME, "Menu",
		 0);
	return (int) XV_ERROR;
    }
    /*
     * set the class field depending on what type of menu the client created
     */
    menu_type = (Xv_pkg *) xv_get(object, XV_TYPE);
    if (menu_type == MENU_COMMAND_MENU) {
	m->class = MENU_COMMAND;
    } else if (menu_type == MENU_CHOICE_MENU) {
	m->class = MENU_CHOICE;
	m->default_image.left_margin = 3;
	m->default_image.right_margin = 3;
    } else if (menu_type == MENU_TOGGLE_MENU) {
	m->class = MENU_TOGGLE;
    } else {
	xv_error(object, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, "Unknown menu type",
		 XV_ERROR_PKG_NAME, "Menu",
		 0);
    }

    (void) xv_set(object, XV_RESET_REF_COUNT, 0);	/* Mark as ref counted. */

    return (int) XV_OK;
}

/* VARARGS */
Pkg_private int
menu_create_item_internal(parent, object, avlist)
    Xv_opaque       parent;
    Xv_opaque       object;
    Xv_opaque      *avlist;
{
    Xv_menu_item_info *mi;

    ((Xv_menu_item *) (object))->private_data =
	(Xv_opaque) xv_alloc(Xv_menu_item_info);
    if (!(mi = MENU_ITEM_PRIVATE(object))) {
	xv_error(object, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "Menu_create_item: unable to allocate menu_item",
		 XV_ERROR_PKG_NAME, "Menu",
		 0);
	return (int) XV_ERROR;
    }
    mi->public_self = object;
    mi->image.free_image = TRUE;
    mi->image.left_pr.pr_data = 0;	/* no left image (e.g., pushpin) */

    return (int) XV_OK;
}


/*
 * destroy the menu data.
 */
Pkg_private int
menu_destroy_internal(menu_public, status)
    Menu            menu_public;
    Destroy_status  status;
{
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);

    if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF))
	menu_destroys(menu, menu->extra_destroy_proc);

    return XV_OK;
}


/*
 * destroy the menu item data.
 */
Pkg_private int
menu_item_destroy_internal(menu_item_public, status)
    Menu_item       menu_item_public;
    Destroy_status  status;
{
    Xv_menu_item_info *menu_item = MENU_ITEM_PRIVATE(menu_item_public);

    if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF))
	menu_item_destroys(menu_item, menu_item->extra_destroy_proc);

    return XV_OK;
}


static          Xv_opaque
menu_return_result(menu, group, parent)
    Xv_menu_info   *menu;
    Xv_menu_group_info *group;
    Xv_menu_item_info *parent;
{
    register Xv_menu_info *m;
    register Xv_menu_item_info *mi;
    Menu(*m_gen_proc) ();
    Menu_item(*mi_gen_proc) ();
    Xv_opaque       v, (*notify_proc) ();
    int             i;

    if (m_gen_proc = menu->gen_proc) {
	m = MENU_PRIVATE((m_gen_proc) (MENU_PUBLIC(menu), MENU_NOTIFY));
	if (m == NULL)
	    return 0;
	m->group_info = group;
	m->parent = parent;
    } else
	m = menu;

    if (m->status != MENU_STATUS_DONE ||
	!range(m->selected_position, 1, m->nitems))
	m->selected_position = m->default_position;
    mi = m->item_list[m->selected_position - 1];

    switch (m->class) {
      case MENU_CHOICE:	/* exclusive choice */
	for (i = 0; i < m->nitems; i++)
	    m->item_list[i]->selected = FALSE;
	mi->selected = TRUE;
	break;

      case MENU_TOGGLE:	/* nonexclusive choice */
	mi->selected = mi->selected ? FALSE : TRUE;
	break;

      case MENU_COMMAND:
      default:
	mi->selected = m->status == MENU_STATUS_DONE;
	break;
    }

    mi->parent = m;

    if (mi->inactive) {
	v = menu_return_no_value(MENU_PUBLIC(m), MENU_ITEM_PUBLIC(mi));
	goto cleanup;
    }
    if (mi_gen_proc = mi->gen_proc) {
	mi = MENU_ITEM_PRIVATE((mi_gen_proc) (MENU_ITEM_PUBLIC(mi), MENU_NOTIFY));
	if (mi == NULL)
	    return menu_return_no_value(MENU_PUBLIC(menu), MENU_ITEM_PUBLIC(mi));
	mi->parent = m;
    }
    notify_proc = mi->notify_proc ? mi->notify_proc
	: m->notify_proc ? m->notify_proc : m->group_info->notify_proc;

    v = (notify_proc) (MENU_PUBLIC(m), MENU_ITEM_PUBLIC(mi));

    if (mi_gen_proc)
	(mi_gen_proc) (MENU_ITEM_PUBLIC(mi), MENU_NOTIFY_DONE);

cleanup:
    if (m_gen_proc) {
	m->group_info = NULL;
	(m_gen_proc) (MENU_PUBLIC(m), MENU_NOTIFY_DONE);
    }
    menu->group_info = NULL;

    return v;
}


Pkg_private     Xv_opaque
menu_pullright_return_result(menu_item_public)
    Menu_item       menu_item_public;
{
    register Xv_menu_info *m;
    Menu(*gen_proc) ();
    register Xv_menu_info *mn;
    Xv_opaque       v;
    register Xv_menu_item_info *mi;

    if (!menu_item_public)
	return NULL;
    mi = MENU_ITEM_PRIVATE(menu_item_public);
    if (!mi->pullright)
	return NULL;


    m = mi->parent;

    if (gen_proc = mi->gen_pullright) {
	mn = MENU_PRIVATE((gen_proc) (menu_item_public, MENU_NOTIFY));
	if (mn == NULL)
	    return menu_return_no_value(MENU_PUBLIC(m), menu_item_public);
    } else {
	mn = MENU_PRIVATE(mi->value);
    }

    v = menu_return_result(mn, m->group_info, mi);
    m->valid_result = mn->valid_result;

    if (gen_proc)
	(gen_proc) (menu_item_public, MENU_NOTIFY_DONE);

    return v;
}


Xv_public       Xv_opaque
menu_return_value(menu_public, menu_item_public)
    Menu            menu_public;
    Menu_item       menu_item_public;
{
    register Xv_menu_info *m;
    register Xv_menu_item_info *mi;

    if (!menu_public || !menu_item_public) {	/* No menu or item */
	if (menu_public) {
	    (MENU_PRIVATE(menu_public))->valid_result = FALSE;
	}
	return (Xv_opaque) MENU_NO_VALUE;
    }
    m = MENU_PRIVATE(menu_public);
    mi = MENU_ITEM_PRIVATE(menu_item_public);
    if (mi->pullright)
	return menu_pullright_return_result(menu_item_public);

    m->valid_result = TRUE;
    return (Xv_opaque) mi->value;	/* Return value */
}


Xv_public       Xv_opaque
menu_return_item(menu_public, menu_item_public)
    Menu            menu_public;
    Menu_item       menu_item_public;
{
    register Xv_menu_info *m;
    register Xv_menu_item_info *mi;

    if (!menu_public || !menu_item_public) {	/* No menu or item */
	if (menu_public) {
	    (MENU_PRIVATE(menu_public))->valid_result = FALSE;
	}
	return (Xv_opaque) MENU_NO_ITEM;
    }
    m = MENU_PRIVATE(menu_public);
    mi = MENU_ITEM_PRIVATE(menu_item_public);
    if (mi->pullright)
	return menu_pullright_return_result(menu_item_public);

    m->valid_result = TRUE;
    return MENU_ITEM_PUBLIC(mi);/* Return pointer */
}


/* ARGSUSED */
Pkg_private     Xv_opaque
menu_return_no_value(menu_public, menu_item_public)
    Menu            menu_public;
    Menu_item       menu_item_public;
{
    register Xv_menu_info *m;

    if (menu_public) {
	m = MENU_PRIVATE(menu_public);
	m->valid_result = FALSE;
	if (m->gen_proc) {
	    (m->gen_proc) (menu_public, MENU_NOTIFY);
	    (m->gen_proc) (menu_public, MENU_NOTIFY_DONE);
	}
    }
    return (Xv_opaque) MENU_NO_VALUE;
}


/* VARARGS1 */
Sv1_public      Menu_item
menu_find(menu_public, va_alist)
    Menu            menu_public;
va_dcl				/*** WARNING: menu_find does not support ATTR_LIST. ***/
{
#ifdef ATTR_MAKE
    Menu_attribute  avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make((char **) avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    return menu_pkg_find(menu_public, MENUITEM, avlist);
#else
    return menu_pkg_find(menu_public, MENUITEM, &va_alist);
#endif				/* ATTR_MAKE */
}


/*
 * Find the menu_item specified by the avlist. menu_pkg_find is called from
 * xv_find via the menu package ops vector.
 */
Pkg_private     Xv_opaque
menu_pkg_find(menu_public, pkg, avlist)
    Menu            menu_public;
    Xv_pkg         *pkg;	/* Must be MENUITEM */
    Menu_attribute  avlist[ATTR_STANDARD_SIZE];
{
    register Menu_attribute *attrs;
    register Xv_menu_item_info *mi, **mip;
    register int    nitems, valid;
    int             submenus = FALSE, descend_first = FALSE;
    Xv_menu_info   *m;
    Menu(*gen_proc) ();
    Menu_item       mi_public;
    Menu_item(*gen_item_proc) ();
    Xv_menu_info   *m_base;

    if (!menu_public)
	return NULL;

    m_base = MENU_PRIVATE(menu_public);

    nitems = m_base->nitems;
    for (attrs = avlist; *attrs; attrs = menu_attr_next(attrs))
	if (attrs[0] == MENU_DESCEND_FIRST)
	    descend_first = (int) attrs[1];

    if (gen_proc = m_base->gen_proc) {
	m = MENU_PRIVATE((gen_proc) (menu_public, MENU_DISPLAY));
	if (m == NULL) {
	    xv_error(menu_public, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
		     "menu_find: menu's gen_proc failed to generate a menu",
		     XV_ERROR_PKG_NAME, "Menu",
		     0);
	    return NULL;
	}
    } else {
	m = m_base;
    }

    nitems = m->nitems;
    for (mip = m->item_list; valid = TRUE, mi = *mip, nitems--; mip++) {

	if (gen_item_proc = mi->gen_proc) {
	    mi = MENU_ITEM_PRIVATE((gen_item_proc) (MENU_ITEM_PUBLIC(mi),
						    MENU_DISPLAY));
	    if (mi == NULL) {
		xv_error(menu_public, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			 XV_ERROR_STRING,
			 "menu_find: menu item's gen_proc failed to generate a menu item",
			 XV_ERROR_PKG_NAME, "Menu",
			 0);
		goto exit;
	    }
	}
	for (attrs = avlist; *attrs; attrs = menu_attr_next(attrs)) {
	    switch (attrs[0]) {

	      case MENU_ACTION:/* & case MENU_NOTIFY_PROC: */
		valid &= mi->notify_proc == (Xv_opaque(*) ()) attrs[1];
		break;

	      case MENU_CLIENT_DATA:
		valid &= mi->client_data == (Xv_opaque) attrs[1];
		break;

	      case MENU_FEEDBACK:
		valid &= !mi->no_feedback == (unsigned) attrs[1];
		break;

	      case MENU_FONT:
		valid &= (mi->image.font ? mi->image.font : 0) == (Pixfont *) attrs[1];
		break;

	      case MENU_GEN_PROC:
		valid &= mi->gen_proc == (Menu_item(*) ()) attrs[1];
		break;

	      case MENU_GEN_PULLRIGHT:
		valid &= mi->pullright &&
		    mi->gen_pullright == (Menu(*) ()) attrs[1];
		break;

	      case MENU_IMAGE:
		valid &= mi->image.pr == (struct pixrect *) attrs[1];
		break;

	      case MENU_INACTIVE:
		valid &= mi->inactive == (unsigned) attrs[1];
		break;

	      case MENU_INVERT:
		valid &= mi->image.invert == (unsigned) attrs[1];
		break;

	      case MENU_LEFT_MARGIN:
		valid &= mi->image.left_margin == (int) attrs[1];
		break;

	      case MENU_MARGIN:
		valid &= mi->image.margin == (int) attrs[1];
		break;

	      case MENU_PARENT:
		valid &= mi->parent == MENU_PRIVATE(attrs[1]);
		break;

	      case MENU_PULLRIGHT:
		valid &= mi->pullright && mi->value == (Xv_opaque) attrs[1];
		break;

	      case MENU_RIGHT_MARGIN:
		valid &= mi->image.right_margin == (int) attrs[1];
		break;

	      case MENU_STRING:
		valid &= mi->image.string && strcmp(mi->image.string, (Xv_opaque) attrs[1]) == 0;
		break;

	      case MENU_VALUE:
		valid &= mi->value == (Xv_opaque) attrs[1];
		break;

	    }
	    if (!valid)
		break;
	}

	if (gen_item_proc)
	    (gen_item_proc) (MENU_ITEM_PUBLIC(mi), MENU_DISPLAY_DONE);

	if (valid)
	    goto exit;

	if (mi->pullright)
	    if (descend_first) {
		mi_public = menu_pkg_find(mi->value, MENUITEM, avlist);
		if (mi_public) {
		    mi = MENU_ITEM_PRIVATE(mi_public);
		    goto exit;
		}
	    } else {
		submenus = TRUE;
	    }
    }

    if (submenus) {
	nitems = m->nitems;
	for (mip = m->item_list; mi = *mip, nitems--; mip++)
	    if (mi->pullright) {
		mi_public = menu_pkg_find(mi->value, MENUITEM, avlist);
		if (mi_public) {
		    mi = MENU_ITEM_PRIVATE(mi_public);
		    goto exit;
		}
	    }
    }
    mi = NULL;

exit:
    if (gen_proc)
	(gen_proc) (menu_public, MENU_DISPLAY_DONE);

    return mi ? MENU_ITEM_PUBLIC(mi) : NULL;
}


Xv_private void
menu_select_default(menu_public)
    Menu            menu_public;
{
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);
    Xv_menu_item_info *mi;

    menu->selected_position = menu->default_position;
    mi = menu->item_list[menu->selected_position - 1];
    if (mi && mi->pullright && mi->value)
	menu_select_default(mi->value);
}


Xv_private      Xv_opaque
menu_return_default(menu_public, depth)
    Menu            menu_public;
    int             depth;
{
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);
    Xv_menu_group_info *group;
    Xv_opaque       result;

    group = xv_alloc(Xv_menu_group_info);
    group->depth = depth;
    group->notify_proc = menu->notify_proc;
    if (!group->notify_proc)
	group->notify_proc = MENU_DEFAULT_NOTIFY_PROC;
    menu->notify_status = XV_OK;
    result = menu_return_result(menu, group, (Xv_menu_info *) 0);
    xv_free(group);
    return (result);
}


Xv_public void
menu_default_pin_proc(menu_public, x, y)
    Menu            menu_public;
    int             x, y;	/* fullscreen coordinate of top left corner
				 * of pinned window */
{
    Panel           panel;
    Panel_item      default_panel_item;
    Rect            frame_rect;
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);

    /* Set the pin window panel default item */
    default_panel_item =
	menu->item_list[menu->default_position - 1]->panel_item_handle;
    panel = xv_get(default_panel_item, XV_OWNER);
    xv_set(panel, PANEL_DEFAULT_ITEM, default_panel_item, 0);

    /* Set the pin window's position */
    frame_get_rect(menu->pin_window, &frame_rect);
    frame_rect.r_left = x;
    frame_rect.r_top = y;
    frame_set_rect(menu->pin_window, &frame_rect);

    /* Show the pin window */
    xv_set(menu->pin_window,
	   XV_SHOW, TRUE,
	   0);
}


static void
pin_button_notify_proc(item, event)
    Panel_item      item;
    Event          *event;
{
    Menu            menu;
    register Xv_menu_info *m;
    Menu_item       menu_item;
    void            (*action_proc) ();

    menu = (Menu) xv_get(item, XV_KEY_DATA, MENU_KEY);
    menu_item = (Menu_item) xv_get(item, XV_KEY_DATA, MENU_ITEM_KEY);
    action_proc = (void (*) ()) xv_get(item, XV_KEY_DATA, ACTION_PROC_KEY);

    /* Fake a MENU_FIRST_EVENT */
    m = MENU_PRIVATE(menu);
    m->group_info = xv_alloc(Xv_menu_group_info);
    m->group_info->first_event = *event;

    /* Invoke the menu item action proc */
    m->notify_status = XV_OK;
    action_proc(menu, menu_item);
    xv_set(item, PANEL_NOTIFY_STATUS, m->notify_status, 0);

    xv_free(m->group_info);
}


Pkg_private void
menu_create_pin_window(menu_public, parent_frame, frame_label)
    Menu            menu_public;
    Frame           parent_frame;
    char           *frame_label;
{
    int             i;
    Frame           frame, base_frame;
    Panel           panel;
    Xv_menu_info   *menu = MENU_PRIVATE(menu_public);
    Xv_menu_item_info *mi;

    base_frame = (xv_get(parent_frame, XV_IS_SUBTYPE_OF, FRAME_BASE) ?
		  parent_frame : xv_get(parent_frame, WIN_OWNER));

    frame = xv_create(base_frame, FRAME_CMD,
		      FRAME_LABEL, frame_label,
		      FRAME_CMD_PUSHPIN_IN, TRUE,
		      XV_SHOW, FALSE,
		      0);
    panel = xv_get(frame, FRAME_CMD_PANEL);
    xv_set(panel,
	   PANEL_LAYOUT, PANEL_VERTICAL,
	   PANEL_ITEM_Y_GAP, menu->default_image.margin,
	   0);

    for (i = 0; i < menu->nitems; i++) {
	mi = menu->item_list[i];
	mi->panel_item_handle = xv_create(panel, PANEL_BUTTON,
					  PANEL_INACTIVE, mi->inactive,
					  PANEL_MENU_ITEM, TRUE,
					  0);
	if (mi->title)
	    continue;
	if (mi->image.string)
	    xv_set(mi->panel_item_handle,
		   PANEL_LABEL_STRING, mi->image.string,
		   0);
	else if (mi->image.pr)
	    xv_set(mi->panel_item_handle,
		   PANEL_LABEL_IMAGE, mi->image.pr,
		   0);
	else
	    xv_error(0, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
		     "menu_create_pin_window: menu item does not have a string or image",
		     XV_ERROR_PKG_NAME, "Menu",
		     0);
	if (mi->pullright)
	    xv_set(mi->panel_item_handle,
		   PANEL_ITEM_MENU, mi->value,
		   0);
	else {
	    if (!mi->notify_proc)
		xv_error(0, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
			 XV_ERROR_STRING,
			 "menu_create_pin_window: menu item does not have an action proc",
			 XV_ERROR_PKG_NAME, "Menu",
			 0);
	    xv_set(mi->panel_item_handle,
		   PANEL_NOTIFY_PROC, pin_button_notify_proc,
		   XV_KEY_DATA, MENU_KEY, menu_public,
		   XV_KEY_DATA, MENU_ITEM_KEY, MENU_ITEM_PUBLIC(mi),
		   XV_KEY_DATA, ACTION_PROC_KEY, mi->notify_proc,
		   0);
	}
    }

    window_fit(panel);
    window_fit(frame);

    xv_set(menu_public,
	   MENU_PIN, TRUE,
	   MENU_PIN_WINDOW, frame,
	   0);
}
