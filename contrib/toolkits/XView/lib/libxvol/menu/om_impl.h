/*	@(#)om_impl.h 20.39 89/10/03		*/

/***********************************************************************/
/*	                      om_impl.h				       */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef _xview_walkmenu_impl_h_already_included
#define _xview_walkmenu_impl_h_already_included

#include <xview/openmenu.h>
#include <xview/rect.h>
#include <xview/svrimage.h>
#include <xview/win_input.h>
#include <xview_private/omi_impl.h>


/***** Definitions *****/
#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif

#define imax(a, b) ((int)(b) > (int)(a) ? (int)(b) : (int)(a))
#define range(v, min, max) ((v) >= (min) && (v) <= (max))

#define menu_attr_next(attr) (Menu_attribute *)attr_next((Xv_opaque *)attr)
#define MENU_FILLER 5

#define MENU_PIN_WIDTH	32
#define MENU_PIN_HEIGHT	16

#define MENU_PRIVATE(menu)  XV_PRIVATE(Xv_menu_info,Xv_menu,menu)
#define MENU_PUBLIC(menu)   XV_PUBLIC(menu)

#define MENU_ITEM_PRIVATE(menu_item) \
	XV_PRIVATE(Xv_menu_item_info, Xv_menu_item, menu_item)
#define MENU_ITEM_PUBLIC(menu_item)  XV_PUBLIC(menu_item)


/***** Procedures *****/
Pkg_private int menu_create_internal(), menu_destroy_internal();


/***** Enumerations *****/
typedef enum {
	MENU_STATUS_PIN = -2,
	MENU_STATUS_ABORT = -1,
	MENU_STATUS_DONE = 0,
	MENU_STATUS_PARENT = 1,
	MENU_STATUS_IGNORE = 2,
	MENU_STATUS_DEFAULT = 3,
	MENU_STATUS_BUSY = 4
} Menu_status;

typedef enum {
	MENU_STATE_INIT,	/* generate and initialize menu */
	MENU_STATE_GET_MODE,	/* Press-Drag-Release or Click-Move-Click mode? */
	MENU_STATE_TRACK_MOUSE	/* track the mouse: process Button Press and Drag */
} Menu_state;


/***********************************************************************/
/*	        	Structures 				       */
/***********************************************************************/

/* Terminology:
 *
 * Menu Group:  a collection of menus.  The first menu shown is referred
 *				to as the top or base menu.  Subsequent menus are referred
 *				to as submenus or pullrights.
 *
 * Menu:	a pair of windows (menu and shadow).  The menu window displays
 *			a list of menu items.
 *
 * Menu item:	one of the entires in a menu.  It's either a command button,
 *				choice rectangle or toggle rectangle.
 */


/***********	Menu Group	**********
 *
 * Menu_group_info data is maintained across the entire menu group.
 * Each menu in the menu group points to the same Menu_group_info
 * structure, which is a static variable in om_public.c
 */
typedef struct menu_group {
    int			depth;
	struct inputevent	first_event;
    struct inputevent	last_event;
    struct inputevent	menu_down_event;  /* action==0 => no MENU-down event */
	struct inputevent	previous_event;
    struct menu	*pinned_menu;
	struct menu *selected_menu;
    Xv_opaque	(*notify_proc)();
} Xv_menu_group_info;


/***********	Menu	**********/
typedef struct menu {
    Menu	public_self;	/* public handle */

    int		cache_index;	/* -1 => menu not cached */
	int		choice_value;
    int		curitem;		/* current item */
    int		default_position;	/* Default item */
	int		drag_right_distance;	/* # pixels continuously dragged right */
    int		font_size;
	int		nitems;			/* Number of items */
	int		notify_status;		/* notify proc status: XV_OK or XV_ERROR */
    int		max_nitems;		/* Size of item list */
    int		ncols;
    int		nrows;
	int		pending_default_position;	/* selected, but not accepted yet */
	int		pullright_delta;	/* # pixels to drag right to show submenu */
    int     pushpin_left, pushpin_top;  /* relative to menu window */
    int		read_inputevent;
    int		selected_position;	/* Last selected item */
	int		type;			/* MENU_MENU: use to verify menu handle */
	struct image	default_image;	/* Font, max size, etc. */
    struct menu_item	**item_list;
    struct menu_item	*parent;	/* Last pullright item */
	struct pr_pos	pin_window_pos;	/* coordinate of top left corner
									 * of pinned window */
    /* An enable rectangle is used to both describe where the menu
	 * is positioned, and the rectangle in which the pointer has
	 * to remain, when outside the menu, in order for the menu to stay up.
	 * A position rect is used only to describe where the menu is
	 * positioned.
	 * If an enable rect is not defined, the menu stays up no matter
	 * where the pointer is positioned.  Such a menu is called a popup menu.
	 */
    Menu	(*gen_proc)();  /* Dynamically generate menu */
    Menu_class	class;		/* command, choice or toggle */
	Menu_state	state;
	Menu_status	status;
	Rect	enable_rect;	/* defines menu position and stay-up area */
	Rect	fs_menurect;	/* in screen coordinates */
	Rect	menurect;		/* relative to input event window */
	Rect	position_rect;	/* defines menu position only.
							 * width==0 && pulldown => flush left menu
							 * instead of centered */
    Rect    pushpin_rect;	/* coordinates relative to input event window */
	void	(*done_proc)();	/* called when menu group is dismissed */
    void	(*pin_proc)();		/* Dynamically generate pinned window */
    Xv_menu_group_info	*group_info;
    Xv_opaque	(*notify_proc)();	/* Handler for item select;
									 * calls menu item proc */
	Xv_opaque	glyph_font;
	Xv_Window	pin_window;		/* window shown when menu is pinned */
	Xv_Window	window;			/* window containing menu items */
	Xv_Window	shadow_window;	/* gray shadow window */

	/* Flags */
    unsigned	active:1;	/* Menu is part of the current menu group.
				 * (It is being displayed on the screen.) */
    unsigned	column_major:1;     /* Layout items in col major */
    unsigned	display_one_level:1; /* Limit initial selection */
	unsigned	gen_items:1;		/* menu has generated menu items */
	unsigned	stay_up:1;
    unsigned	h_line:1;			/* Draw horizontal line after item */
    unsigned	pin:1;				/* Menu contains a pushpin */
	unsigned	popup:1;		/* TRUE: popup, FALSE: pulldown or pullright */ 
	unsigned	ptr_jumped:1;		/* pointer was jumped (warped) */
    unsigned	pulldown:1;			/* TRUE: pulldown, FALSE: pullright */
	unsigned	rendered:1;			/* Menu has been painted */
    unsigned	setting_default:1;
    unsigned	stand_off:1;	    /* Don''t selected item */
    unsigned	valid_result:1;     /* True if m->value is valid */
    unsigned	v_line:1;			/* Draw vertical line after item */

    /*
     * SunView1 compatibility entries
	 */
    Xv_opaque	client_data;
    void		(*extra_destroy_proc)();	/* an extra destroy proc that
											 * is used when client calls 
										     * menu_destroy_with_proc() */
}  Xv_menu_info;


/*********** 	Menu item 	**********/
typedef struct menu_item {
    Menu_item	public_self;	    /* Back pointer */

    Menu		(*gen_pullright)();/* Called before displaying menu */
    Menu_item	(*gen_proc)();	    /* Called before displaying item */
    struct image	image;
    struct menu	*parent;	    /* Current enclosing menu */
    void		(*extra_destroy_proc)();	/* an extra destroy proc that
											 * is used when client calls 
    										 * menu_item_destroy_with_proc() */
    Xv_opaque	(*notify_proc)();  /* Called only when selected */
	Xv_opaque	panel_item_handle;	/* ... for pinned window */
    Xv_opaque	value;		    /* union: value, menu_ptr */

   	/* SunView1 compatibility */
	Xv_opaque	client_data;

	/* Flags  */
    unsigned	inactive:1;
    unsigned	no_feedback:1;
    unsigned	pullright:1;
    unsigned	selected:1;
    unsigned	title:1;			/* This item is the menu's title */
    unsigned	free_item:1;
    unsigned	free_value:1;        /* Not used */
    unsigned	free_client_data:1;  /* Not used */
    unsigned	h_line:1;	      /* Draw horizontal line after item */
    unsigned	v_line:1;	      /* Draw vertical line after item */
    unsigned	toggle_on:1;	      /* laf */
    unsigned	toggle_feedback_on:1;/* laf */

}  Xv_menu_item_info;


#endif _xview_walkmenu_impl_h_already_included
