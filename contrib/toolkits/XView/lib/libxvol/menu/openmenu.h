
/*
 * @(#)openmenu.h 20.33 89/10/03 SMI
 *
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_walkmenu_DEFINED
#define xview_walkmenu_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/generic.h>
#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/window.h>


/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */

#define	MENU				&xv_command_menu_pkg
#define MENU_COMMAND_MENU		&xv_command_menu_pkg
#define MENU_CHOICE_MENU		&xv_choice_menu_pkg
#define MENU_TOGGLE_MENU		&xv_toggle_menu_pkg
#define MENUITEM 			&xv_menu_item_pkg

#define MENUITEM_SPACE			MENUITEM, \
					MENU_STRING, "", \
					MENU_FEEDBACK, FALSE

#define MENU_BUT  			ACTION_MENU
#define	MENU_NULL			((Menu)0)
#define	MENU_ITEM_NULL			((Menu_item)0)
#define MENU_NO_ITEM			MENU_ITEM_NULL
#define MENU_NO_VALUE			0

#define MENU_DEFAULT_NOTIFY_PROC 	menu_return_value

/*
 * PRIVATE #defines 
 */

#define	MENU_ATTR(type, ordinal)	ATTR(ATTR_PKG_MENU, type, ordinal)
#define MENU_ATTR_LIST(ltype, type, ordinal) \
	MENU_ATTR(ATTR_LIST_INLINE((ltype), (type)), (ordinal))

/*
 * Fake types -- This should be resolved someday 
 */
#define ATTR_MENU			ATTR_OPAQUE
#define ATTR_IMAGE			ATTR_OPAQUE
#define ATTR_MENU_ITEM			ATTR_OPAQUE
#define ATTR_MENU_ITEM_PAIR		ATTR_INT_PAIR
#define ATTR_STRING_VALUE_PAIR		ATTR_INT_PAIR
#define ATTR_IMAGE_VALUE_PAIR		ATTR_INT_PAIR
#define ATTR_STRING_MENU_PAIR		ATTR_INT_PAIR
#define ATTR_IMAGE_MENU_PAIR		ATTR_INT_PAIR
#define ATTR_STRING_FUNCTION_PAIR	ATTR_INT_PAIR
#define ATTR_IMAGE_FUNCTION_PAIR	ATTR_INT_PAIR
#define ATTR_INT_MENU_ITEM_PAIR		ATTR_INT_PAIR

/* Reserved for future use */
#define	MENU_ATTR_UNUSED_FIRST		 0
#define	MENU_ATTR_UNUSED_LAST		31

#define MENU_BORDER  			2
#define MENU_PKG 			menu_pkg

/*
 * PUBLIC #defines
 * For SunView 1 compatibility. 
 */

#define MENU_FONT		XV_FONT
#define MENU_HEIGHT		XV_HEIGHT
#define MENU_LEFT_MARGIN	XV_LEFT_MARGIN
#define MENU_MARGIN		XV_MARGIN
#define MENU_PARENT		XV_OWNER
#define MENU_RIGHT_MARGIN	XV_RIGHT_MARGIN
#define MENU_WIDTH		XV_WIDTH
#define menu_destroy(menu_public)	xv_destroy(menu_public)

#define MENU_DEFAULT_SHADOW 	&menu_gray75_pr

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

typedef	Xv_opaque 		Menu;
typedef	Xv_opaque 		Menu_item;

typedef enum {
	MENU_ACTION_IMAGE	= MENU_ATTR(ATTR_IMAGE_FUNCTION_PAIR,	  1),
	MENU_ACTION_ITEM	= MENU_ATTR(ATTR_STRING_FUNCTION_PAIR,	  3),
	MENU_APPEND_ITEM	= MENU_ATTR(ATTR_MENU_ITEM,		  9),
	MENU_CLASS   		= MENU_ATTR(ATTR_ENUM, 			 12),
	MENU_COL_MAJOR	= MENU_ATTR(ATTR_BOOLEAN, 		 18),
	MENU_DEFAULT		= MENU_ATTR(ATTR_INT,			 21),
	MENU_DEFAULT_ITEM	= MENU_ATTR(ATTR_MENU_ITEM,		 24),
	MENU_DESCEND_FIRST	= MENU_ATTR(ATTR_BOOLEAN, 		 27), 
/*NYI*/	MENU_DISABLE_ITEM	= MENU_ATTR(ATTR_ENUM,			 30),
	MENU_DONE_PROC		= MENU_ATTR(ATTR_FUNCTION_PTR,	31),
	MENU_FEEDBACK		= MENU_ATTR(ATTR_BOOLEAN,		 33), 
/*--G*/	MENU_FIRST_EVENT	= MENU_ATTR(ATTR_NO_VALUE,		 36), 
	MENU_GEN_PIN_WINDOW	= MENU_ATTR(ATTR_INT_PAIR, 		 39),
	MENU_GEN_PROC		= MENU_ATTR(ATTR_FUNCTION_PTR,		 42), 
	MENU_GEN_PROC_IMAGE	= MENU_ATTR(ATTR_IMAGE_FUNCTION_PAIR,	 45), 
	MENU_GEN_PROC_ITEM	= MENU_ATTR(ATTR_STRING_FUNCTION_PAIR,	 48), 
	MENU_GEN_PULLRIGHT	= MENU_ATTR(ATTR_FUNCTION_PTR,		 51), 
	MENU_GEN_PULLRIGHT_IMAGE= MENU_ATTR(ATTR_IMAGE_FUNCTION_PAIR,	 54), 
	MENU_GEN_PULLRIGHT_ITEM	= MENU_ATTR(ATTR_STRING_FUNCTION_PAIR,	 57), 
	MENU_IMAGE		= MENU_ATTR(ATTR_IMAGE,			 60),
	MENU_IMAGES		= MENU_ATTR_LIST(ATTR_NULL, ATTR_IMAGE,	 63),
	MENU_IMAGE_ITEM		= MENU_ATTR(ATTR_IMAGE_VALUE_PAIR,	 66),
	MENU_INACTIVE		= MENU_ATTR(ATTR_BOOLEAN,		 69), 
	MENU_INSERT		= MENU_ATTR(ATTR_INT_MENU_ITEM_PAIR,	 81),
	MENU_INSERT_ITEM	= MENU_ATTR(ATTR_MENU_ITEM_PAIR,	 84),
	MENU_INVERT		= MENU_ATTR(ATTR_BOOLEAN,		 87), 
	MENU_ITEM		= MENU_ATTR_LIST(ATTR_RECURSIVE, ATTR_AV, 90),
/*--G*/	MENU_LAST_EVENT		= MENU_ATTR(ATTR_NO_VALUE,		 99), 
	/*
	 * MENU_MENU is a fake attribute returned by MENU_TYPE 
	 */
	MENU_MENU		= MENU_ATTR(ATTR_NO_VALUE,		102), 
	MENU_NCOLS		= MENU_ATTR(ATTR_INT,			105),
/*--G*/	MENU_NITEMS		= MENU_ATTR(ATTR_NO_VALUE,		108), 
	MENU_NOTIFY_PROC	= MENU_ATTR(ATTR_FUNCTION_PTR,		  6),
	MENU_NOTIFY_STATUS	= MENU_ATTR(ATTR_INT,		111),
	/*
	 * The origin for MENU_NTH_ITEM is 1 
	 */
	MENU_NTH_ITEM		= MENU_ATTR(ATTR_INT,			114), 
	MENU_NROWS		= MENU_ATTR(ATTR_INT,			117),
	MENU_PIN		= MENU_ATTR(ATTR_BOOLEAN, 		120),
	MENU_PIN_PROC		= MENU_ATTR(ATTR_FUNCTION_PTR, 		123),
	MENU_PIN_WINDOW		= MENU_ATTR(ATTR_OPAQUE, 		126),
	MENU_PULLRIGHT		= MENU_ATTR(ATTR_MENU,			129),
	MENU_PULLRIGHT_IMAGE	= MENU_ATTR(ATTR_IMAGE_MENU_PAIR,	135),
	MENU_PULLRIGHT_ITEM	= MENU_ATTR(ATTR_STRING_MENU_PAIR,	138),
	MENU_RELEASE		= MENU_ATTR(ATTR_NO_VALUE,		141), 
	MENU_RELEASE_IMAGE	= MENU_ATTR(ATTR_NO_VALUE,		144), 
	MENU_REMOVE		= MENU_ATTR(ATTR_INT,			147),
	MENU_REMOVE_ITEM	= MENU_ATTR(ATTR_MENU_ITEM,		150),
	MENU_REPLACE		= MENU_ATTR(ATTR_INT_MENU_ITEM_PAIR,	153),
	MENU_REPLACE_ITEM	= MENU_ATTR(ATTR_MENU_ITEM_PAIR,	156),
	MENU_SELECTED		= MENU_ATTR(ATTR_INT,			159),
	MENU_SELECTED_ITEM	= MENU_ATTR(ATTR_MENU_ITEM,		162),
	MENU_STRING		= MENU_ATTR(ATTR_STRING,		165),
	MENU_STRINGS		= MENU_ATTR_LIST(ATTR_NULL, ATTR_STRING,168),
	MENU_STRING_ITEM	= MENU_ATTR(ATTR_STRING_VALUE_PAIR,	171),
	MENU_TITLE		= MENU_ATTR(ATTR_NO_VALUE, 		174),
	MENU_TITLE_IMAGE	= MENU_ATTR(ATTR_IMAGE,			177),
	MENU_TITLE_ITEM		= MENU_ATTR(ATTR_STRING,		180),
/*--G*/	MENU_TYPE		= MENU_ATTR(ATTR_NO_VALUE,		183), 
	MENU_VALID_RESULT	= MENU_ATTR(ATTR_INT,			186),
	MENU_VALUE		= MENU_ATTR(ATTR_OPAQUE,		189), 
	/*
	 * Private Attributes 
	 */
	MENU_BUTTON		= MENU_ATTR(ATTR_INT, 			195), 
	MENU_CURSOR		= MENU_ATTR(ATTR_OPAQUE,		198),
	MENU_DEFAULT_IMAGE	= MENU_ATTR(ATTR_IMAGE,			201), 
	MENU_ENABLE_RECT	= MENU_ATTR(ATTR_INT, 			204),
	MENU_FD			= MENU_ATTR(ATTR_INT, 			207), 
	MENU_HORIZONTAL_LINE	= MENU_ATTR(ATTR_INT,			210),
	MENU_IE			= MENU_ATTR(ATTR_INT, 			213), 
	MENU_LINE_AFTER_ITEM	= MENU_ATTR(ATTR_INT,			216),
	MENU_POS		= MENU_ATTR(ATTR_INT_PAIR, 		219), 
	MENU_POSITION_RECT	= MENU_ATTR(ATTR_INT,		220),
	MENU_PULLDOWN		= MENU_ATTR(ATTR_INT, 			222),
	MENU_SHADOW_MENU	= MENU_ATTR(ATTR_OPAQUE,		223),
	MENU_SHADOW_SVRIM	= MENU_ATTR(ATTR_OPAQUE,	225),
	MENU_WINDOW_MENU	= MENU_ATTR(ATTR_OPAQUE,		226),
	MENU_VERTICAL_LINE	= MENU_ATTR(ATTR_INT,			228),
	/*
	 * For SunView 1 Compatibility 
	 */
	MENU_ACTION		= MENU_NOTIFY_PROC,
	MENU_ACTION_PROC	= MENU_NOTIFY_PROC,
	MENU_CENTER		= MENU_ATTR(ATTR_BOOLEAN, 		231),
	MENU_CLIENT_DATA    = MENU_ATTR(ATTR_OPAQUE,         15),
	MENU_DEFAULT_SELECTION	= MENU_ATTR(ATTR_ENUM,			234),
    MENU_INITIAL_SELECTION  = MENU_ATTR(ATTR_ENUM,           72),
    MENU_INITIAL_SELECTION_EXPANDED = MENU_ATTR(ATTR_BOOLEAN,    75),
    MENU_INITIAL_SELECTION_SELECTED = MENU_ATTR(ATTR_BOOLEAN,    78),
    MENU_JUMP_AFTER_NO_SELECTION    = MENU_ATTR(ATTR_BOOLEAN,    93),
    MENU_JUMP_AFTER_SELECTION   = MENU_ATTR(ATTR_BOOLEAN,    96),
    MENU_PULLRIGHT_DELTA    = MENU_ATTR(ATTR_INT,           132),
	MENU_SHADOW		= MENU_ATTR(ATTR_PIXRECT_PTR,		237),
} Menu_attribute;

typedef enum {
	MENU_COMMAND, 
	MENU_CHOICE, 
	MENU_TOGGLE
} Menu_class;

typedef enum {
	MENU_PROVIDE_FEEDBACK, 			/* not default, selected     */
	MENU_REMOVE_FEEDBACK,			/* not default, not selected */
	MENU_SELECTED_DEFAULT_FEEDBACK,		/* default, selected         */
	MENU_DEFAULT_FEEDBACK,			/* default, not selected     */
	MENU_BUSY_FEEDBACK				/* 50% gray */
} Menu_feedback;

/*
 * New generate names intended to be less confusing 
 */
typedef enum {
	MENU_DISPLAY, 
	MENU_DISPLAY_DONE, 
	MENU_NOTIFY, 
	MENU_NOTIFY_DONE
} Menu_generate;

typedef struct {
	Xv_window_struct	parent_data;
	Xv_opaque		private_data;
} Xv_menu;

typedef struct {
	Xv_generic_struct	parent_data;
	Xv_opaque		private_data;
} Xv_menu_item;

/*
 ***********************************************************************	
 *				Globals
 ***********************************************************************
 */

/*
 * PUBLIC variables 
 */

extern Xv_pkg		xv_command_menu_pkg;
extern Xv_pkg		xv_choice_menu_pkg;
extern Xv_pkg		xv_toggle_menu_pkg;
extern Xv_pkg		xv_menu_item_pkg;

/*
 * PUBLIC variables 
 * for SunView 1 compatibility
 */

extern struct pixrect 	menu_gray50_pr;

/*
 * PUBLIC functions 
 */

extern Xv_opaque	menu_return_value();
extern Xv_opaque	menu_return_item();
extern void	menu_show();

/*
 * PRIVATE functions 
 */

extern Xv_opaque	menu_pullright_return_result();
extern Xv_opaque 	menu_return_default();
extern void		menu_select_default();

/* 
 * PUBLIC functions 
 * for SunView 1 compatibility 
 */

extern Menu		menu_create();
extern Menu_item	menu_create_item();
extern Menu_item	menu_find();
extern Xv_opaque	menu_get();
extern Xv_opaque	menu_set();
extern void		menu_destroy_with_proc();
extern Xv_opaque	menu_show_using_fd();

#endif xview_walkmenu_DEFINED
