/*	@(#)panel.h 20.39 89/11/21 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_panel_DEFINED
#define xview_panel_DEFINED

/*
 ***********************************************************************
 *		Include Files
 ***********************************************************************
 */

#include <xview/canvas.h>

/*
 ***********************************************************************
 *		Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */
#define PANEL 		&xv_panel_pkg
#define	PANEL_BUTTON	&xv_panel_button_pkg
#define	PANEL_CHOICE	&xv_panel_choice_pkg
#define	PANEL_ITEM	&xv_panel_item_pkg
#define	PANEL_LIST	&xv_panel_list_pkg
#define	PANEL_MESSAGE	&xv_panel_message_pkg
#define	PANEL_SLIDER	&xv_panel_slider_pkg
#define	PANEL_TEXT	&xv_panel_text_pkg
#define PANEL_NUMERIC_TEXT  &xv_panel_num_text_pkg
#define PANEL_VIEW	&xv_panel_view_pkg

#define SCROLLABLE_PANEL	PANEL, OPENWIN_SHOW_BORDERS,	TRUE

#define PANEL_CHOICE_STACK	\
			PANEL_CHOICE, 	PANEL_DISPLAY_LEVEL,	PANEL_CURRENT

#define PANEL_CYCLE	PANEL_CHOICE,	PANEL_DISPLAY_LEVEL,	PANEL_CURRENT

#define	PANEL_TOGGLE	PANEL_CHOICE, 	PANEL_CHOOSE_ONE, 	FALSE
#define PANEL_CHECK_BOX	PANEL_TOGGLE, 	PANEL_FEEDBACK, 	PANEL_MARKED

#define PANEL_CHECK_BOX_VALUE		PANEL_TOGGLE_VALUE


/*
 * Various utility macros 
 */
#define	panel_get_value(ip) 		xv_get((ip), PANEL_VALUE)
#define	panel_set_value(ip, val)	xv_set((ip), PANEL_VALUE, (val), 0)

#define	PANEL_EACH_ITEM(panel, ip)					\
   { Panel_item _next; 							\
   for ((ip) = (Panel_item) panel_get((panel), PANEL_FIRST_ITEM); 	\
	(ip); (ip) = _next) { 						\
	_next = (Panel_item) panel_get((ip), PANEL_NEXT_ITEM); {

#define	PANEL_END_EACH	}}}

/*
 * Miscellaneous constants 
 */
#define PANEL_ITEM_X_START	4	/* offset from left edge */
#define PANEL_ITEM_Y_START	4	/* offset from top edge */

/* Panel defined events.
 * These are given to the Panel's or Panel_item's event proc
 */
#define	PANEL_EVENT_FIRST	vuid_first(PANEL_DEVID)		/* 32000 */
#define	PANEL_EVENT_CANCEL	(PANEL_EVENT_FIRST + 0)		/* 32000 */
/* PANEL_EVENT_MOVE_IN is obsolete, and is retained
 * for SunView1 compatiblity only.
 */
#define	PANEL_EVENT_MOVE_IN	(PANEL_EVENT_FIRST + 1)		/* 32001 */
#define	PANEL_EVENT_DRAG_IN	(PANEL_EVENT_FIRST + 2)		/* 32002 */

/*
 * PRIVATE #defines 
 */

#define	PANEL_ATTR(type, ordinal)	ATTR(ATTR_PKG_PANEL, type, ordinal)

/*
 * panel specific attribute types 
 */
#define	PANEL_INDEX_STRING		ATTR_TYPE(ATTR_BASE_UNUSED_FIRST,     2)
#define	PANEL_INDEX_PIXRECT_PTR		ATTR_TYPE(ATTR_BASE_UNUSED_FIRST + 1, 2)
#define	PANEL_INDEX_BOOLEAN		ATTR_TYPE(ATTR_BASE_UNUSED_FIRST + 2, 2)
#define	PANEL_INDEX_FONT		ATTR_TYPE(ATTR_BASE_UNUSED_FIRST + 3, 2)
#define	PANEL_INDEX_CLIENT_DATA		ATTR_TYPE(ATTR_BASE_UNUSED_FIRST + 4, 2)


#define PANEL_FONT		WIN_FONT
#define PANEL_TYPE		ATTR_PKG_PANEL

/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */

typedef struct {
    Xv_canvas		parent_data;
    Xv_opaque		private_data;		/* panel or item */
} Xv_panel_or_item;

/*
 * Typedefs 
 */
typedef	Xv_opaque 		Panel;
typedef	Xv_opaque	 	Panel_item;
typedef	Xv_opaque 		Panel_attribute_value;
typedef Xv_panel_or_item	Xv_panel;
typedef Xv_panel_or_item	Xv_item;
typedef Xv_opaque		Panel_view;
typedef Xv_item			Xv_panel_message;

typedef Panel_item		Panel_button_item;
typedef Panel_item              Panel_choice_item;
typedef Panel_item              Panel_list_item;
typedef Panel_item              Panel_message_item; 
typedef Panel_item              Panel_slider_item;
typedef Panel_item              Panel_text_item;
typedef Panel_item              Panel_numeric_text_item;

/*
 * Enumerations 
 */
typedef enum {
	/*
	 * Public Attributes 
	 */
	PANEL_ACCEPT_KEYSTROKE	= PANEL_ATTR(ATTR_BOOLEAN,		   1),
	PANEL_BACKGROUND_PROC	= PANEL_ATTR(ATTR_FUNCTION_PTR,		   2),
	PANEL_BLINK_CARET	= PANEL_ATTR(ATTR_BOOLEAN,		   4),
	PANEL_BOLD_FONT		= PANEL_ATTR(ATTR_PIXFONT_PTR,		   6),
	PANEL_CARET_ITEM	= PANEL_ATTR(ATTR_OPAQUE,		   8),
	PANEL_CHOICES_BOLD	= PANEL_ATTR(ATTR_BOOLEAN,		  10),
	PANEL_CHOICE_FONT	= PANEL_ATTR(PANEL_INDEX_FONT,		  12),
	PANEL_CHOICE_FONTS		=
        	PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_PIXFONT_PTR), 14),
	PANEL_CHOICE_IMAGE	= PANEL_ATTR(PANEL_INDEX_PIXRECT_PTR, 	  16),
	PANEL_CHOICE_IMAGES		= 
        	PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_PIXRECT_PTR), 18),
	PANEL_CHOICE_STRING	= PANEL_ATTR(PANEL_INDEX_STRING,	  20), 
	PANEL_CHOICE_STRINGS	=
       		PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_STRING),	  22),
	PANEL_CHOICE_X		= PANEL_ATTR(ATTR_INDEX_X,		  26),
	PANEL_CHOICE_XS	= PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_X), 28),
	PANEL_CHOICE_Y		= PANEL_ATTR(ATTR_INDEX_Y,		  30),
	PANEL_CHOICE_YS	= PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_Y), 32),
	PANEL_CHOOSE_NONE	= PANEL_ATTR(ATTR_BOOLEAN,		  33),
	PANEL_CHOOSE_ONE	= PANEL_ATTR(ATTR_BOOLEAN,		  34),
	PANEL_CLIENT_DATA	= PANEL_ATTR(ATTR_OPAQUE,		  36),
	PANEL_DEFAULT_ITEM	= PANEL_ATTR(ATTR_OPAQUE,		  38),
	PANEL_DEFAULT_VALUE	= PANEL_ATTR(ATTR_OPAQUE,		  40),
	PANEL_DISPLAY_LEVEL	= PANEL_ATTR(ATTR_ENUM,			  42),
	PANEL_EVENT_PROC	= PANEL_ATTR(ATTR_FUNCTION_PTR,		  44),
	PANEL_EXTRA_PAINT_HEIGHT = PANEL_ATTR(ATTR_Y, 			  46),
	PANEL_EXTRA_PAINT_WIDTH = PANEL_ATTR(ATTR_X,			  48),
	PANEL_FEEDBACK		= PANEL_ATTR(ATTR_ENUM,			  50),
	PANEL_FIRST_ITEM	= PANEL_ATTR(ATTR_OPAQUE,		  52),
	PANEL_INACTIVE		= PANEL_ATTR(ATTR_BOOLEAN,		  54),
	PANEL_ITEM_CLASS	= PANEL_ATTR(ATTR_OPAQUE,		  56),
	PANEL_ITEM_COLOR	= PANEL_ATTR(ATTR_INT,			  58),
	PANEL_ITEM_MENU		= PANEL_ATTR(ATTR_OPAQUE,		  60),
	PANEL_ITEM_RECT		= PANEL_ATTR(ATTR_RECT_PTR,		  62),
	/* these attrs must still be supported in addition to XV_X */
	/* and XV_Y because they can be used on the panel. When used */
	/* on the panel PANEL_ITEM_X != XV_X on the panel */
	PANEL_ITEM_X		= PANEL_ATTR(ATTR_X,			  63),
	PANEL_ITEM_Y		= PANEL_ATTR(ATTR_Y,			  64),
	PANEL_ITEM_X_GAP	= PANEL_ATTR(ATTR_X,			  65),
	PANEL_ITEM_Y_GAP	= PANEL_ATTR(ATTR_Y,			  66),
	PANEL_LABEL_BOLD	= PANEL_ATTR(ATTR_BOOLEAN, 		  68),
	PANEL_LABEL_FONT	= PANEL_ATTR(ATTR_PIXFONT_PTR,		  70),
	PANEL_LABEL_IMAGE	= PANEL_ATTR(ATTR_PIXRECT_PTR,		  72),
	PANEL_LABEL_STRING	= PANEL_ATTR(ATTR_STRING,		  74),
	PANEL_LABEL_WIDTH	= PANEL_ATTR(ATTR_INT,			  76),
	PANEL_LABEL_X		= PANEL_ATTR(ATTR_X,			  78),
	PANEL_LABEL_Y		= PANEL_ATTR(ATTR_Y,			  80),
	PANEL_LAYOUT		= PANEL_ATTR(ATTR_ENUM,			  82),
	PANEL_LIST_CLIENT_DATA	= PANEL_ATTR(PANEL_INDEX_CLIENT_DATA,	  88),
	PANEL_LIST_CLIENT_DATAS	=
       		PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_OPAQUE),	  90),
	PANEL_LIST_DELETE	= PANEL_ATTR(ATTR_INT,			  92),
	PANEL_LIST_DISPLAY_ROWS	= PANEL_ATTR(ATTR_INT,			  94),
	PANEL_LIST_FONT		= PANEL_ATTR(PANEL_INDEX_FONT,		  96),
	PANEL_LIST_FONTS		=
       		PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_PIXFONT_PTR), 98),
	PANEL_LIST_GLYPH	= PANEL_ATTR(PANEL_INDEX_PIXRECT_PTR,	 100),
	PANEL_LIST_GLYPHS	=
       		PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_PIXRECT_PTR),102),
	PANEL_LIST_HEIGHT	= PANEL_ATTR(ATTR_INT,			 104),
	PANEL_LIST_INSERT	= PANEL_ATTR(ATTR_INT,			 106),
	PANEL_LIST_NROWS	= PANEL_ATTR(ATTR_INT,			 108),
	PANEL_LIST_ROW_HEIGHT	= PANEL_ATTR(ATTR_INT,			110),
	PANEL_LIST_SELECT	= PANEL_ATTR(ATTR_INT_PAIR,	 	 112),
	PANEL_LIST_SELECTED	= PANEL_ATTR(ATTR_INT,			 113),
	PANEL_LIST_STRING	= PANEL_ATTR(PANEL_INDEX_STRING,	 114),
	PANEL_LIST_STRINGS	=
       		PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_STRING),	 116),
	PANEL_LIST_WIDTH	= PANEL_ATTR(ATTR_INT,			 122),
	PANEL_MARK_IMAGE	= PANEL_ATTR(PANEL_INDEX_PIXRECT_PTR,	 124),
	PANEL_MARK_IMAGES		= 
        	PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_PIXRECT_PTR),126),
	PANEL_MARK_X		= PANEL_ATTR(ATTR_INDEX_X,		 128),
	PANEL_MARK_XS	= PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_X),130),
	PANEL_MARK_Y		= PANEL_ATTR(ATTR_INDEX_Y,		 132),
	PANEL_MARK_YS	= PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_Y),134),
	PANEL_MASK_CHAR		= PANEL_ATTR(ATTR_CHAR,			 136),
	PANEL_MAX_VALUE		= PANEL_ATTR(ATTR_INT,			 138),
	PANEL_MENU_MARK_IMAGE	= PANEL_ATTR(ATTR_PIXRECT_PTR,		 140),
	PANEL_MENU_NOMARK_IMAGE	= PANEL_ATTR(ATTR_PIXRECT_PTR,		 142),
	PANEL_MIN_VALUE		= PANEL_ATTR(ATTR_INT,			 144),
	PANEL_NEXT_COL		= PANEL_ATTR(ATTR_INT,			 145),
	PANEL_NEXT_ITEM		= PANEL_ATTR(ATTR_OPAQUE,		 146),
	PANEL_NEXT_ROW		= PANEL_ATTR(ATTR_INT,			 147),
	PANEL_NOMARK_IMAGE	= PANEL_ATTR(PANEL_INDEX_PIXRECT_PTR,	 148),
	PANEL_NOMARK_IMAGES		=
        	PANEL_ATTR(ATTR_LIST_INLINE(ATTR_NULL, ATTR_PIXRECT_PTR),150),
	PANEL_NOTIFY_LEVEL	= PANEL_ATTR(ATTR_ENUM,			 152),
	PANEL_NOTIFY_PROC	= PANEL_ATTR(ATTR_FUNCTION_PTR,		 154),
	PANEL_NOTIFY_STATUS	= PANEL_ATTR(ATTR_INT,		156),
	PANEL_NOTIFY_STRING	= PANEL_ATTR(ATTR_STRING,		 158),
	PANEL_PAINT		= PANEL_ATTR(ATTR_ENUM,			 160),
	PANEL_READ_ONLY		= PANEL_ATTR(ATTR_BOOLEAN,		 162),
	PANEL_REPAINT_PROC	= PANEL_ATTR(ATTR_FUNCTION_PTR,		 164),
	PANEL_SHOW_MENU		= PANEL_ATTR(ATTR_BOOLEAN,		 168),
	PANEL_SHOW_MENU_MARK	= PANEL_ATTR(ATTR_BOOLEAN,		 170),
	PANEL_SHOW_RANGE	= PANEL_ATTR(ATTR_BOOLEAN,		 172),
	PANEL_SHOW_VALUE	= PANEL_ATTR(ATTR_BOOLEAN,		 174),
	PANEL_SLIDER_WIDTH	= PANEL_ATTR(ATTR_X,			 176),
	PANEL_TOGGLE_VALUE	= PANEL_ATTR(PANEL_INDEX_BOOLEAN,	 178),
	PANEL_VALUE		= PANEL_ATTR(ATTR_INT, 			 180),
	PANEL_VALUE_DISPLAY_LENGTH	= PANEL_ATTR(ATTR_INT,		 182),
	PANEL_VALUE_FONT	= PANEL_ATTR(ATTR_PIXFONT_PTR,		 184),
	PANEL_VALUE_STORED_LENGTH	= PANEL_ATTR(ATTR_INT,		 186),
	PANEL_VALUE_UNDERLINED	= PANEL_ATTR(ATTR_BOOLEAN,		 188),
	PANEL_VALUE_X		= PANEL_ATTR(ATTR_X,			 190),
	PANEL_VALUE_Y		= PANEL_ATTR(ATTR_Y,			 192),
	/*
	 * Private Attributes 
	 */
	PANEL_CHOICE_OFFSET	= PANEL_ATTR(ATTR_INT,			 196),
	PANEL_LABEL_BOXED	= PANEL_ATTR(ATTR_BOOLEAN,		 198),
	PANEL_LABEL_INVERTED	= PANEL_ATTR(ATTR_BOOLEAN,		 200),
	PANEL_LIST_ROW_HIGHLIGHTED = PANEL_ATTR(ATTR_BOOLEAN,		 202),
	PANEL_MENU_ITEM		= PANEL_ATTR(ATTR_BOOLEAN,		 204),
	PANEL_TEXT_CURSOR	= PANEL_ATTR(ATTR_OPAQUE,		 206),
} Panel_attr;
#define PANEL_PARENT_PANEL	XV_OWNER
#define PANEL_SHOW_ITEM		XV_SHOW

typedef enum {
    PANEL_LIST_OP_DESELECT,
    PANEL_LIST_OP_SELECT,
    PANEL_LIST_OP_VALIDATE,
    PANEL_LIST_OP_DELETE
} Panel_list_op;

/*
 * Values for LEVEL attributes 
 */
typedef enum {
    PANEL_CLEAR,        	/* painting */
    PANEL_NO_CLEAR,     	/* painting */
    PANEL_NONE,			/* text notify, menu, display, painting */
    PANEL_ALL,			/* text notify, slider notify, display */
    PANEL_NON_PRINTABLE,	/* text notify */
    PANEL_SPECIFIED,		/* text notify */
    PANEL_CURRENT,		/* display */
    PANEL_DONE,			/* slider notify */
    PANEL_MARKED,		/* feedback */
    PANEL_VERTICAL,		/* layout */
    PANEL_HORIZONTAL,		/* layout */
    PANEL_INVERTED,		/* feedback */
    /*
     * values returnable by notify routines 
     */
    PANEL_INSERT,
    PANEL_NEXT,
    PANEL_PREVIOUS,
    /*
     * mouse state 
     */
    PANEL_NONE_DOWN,		/* no buttons are down */
    PANEL_LEFT_DOWN,		/* left button is down */
    PANEL_MIDDLE_DOWN,		/* middle button is down */
    PANEL_RIGHT_DOWN,		/* right button is down */
    PANEL_CHORD_DOWN		/* chord of buttons are down */
} Panel_setting;

/*
 * Types of items
 */
typedef enum { 
   PANEL_MESSAGE_ITEM,
   PANEL_BUTTON_ITEM, 
   PANEL_CHOICE_ITEM, 
   PANEL_TOGGLE_ITEM, 
   PANEL_TEXT_ITEM,
   PANEL_NUMERIC_TEXT_ITEM,
   PANEL_SLIDER_ITEM,
   PANEL_LIST_ITEM
} Panel_item_type;
    
typedef struct {
    Xv_canvas_view	parent_data;
    Xv_opaque		private_data;
} Xv_panel_view;

/*
 * Structures 
 */

typedef struct {
    Xv_item		parent_data;
    Xv_opaque		private_data;
} Xv_panel_choice;

typedef struct {
    Xv_item		parent_data;
    Xv_opaque		private_data;
} Xv_panel_pulldown;

typedef struct {
    Xv_item		parent_data;
    Xv_opaque		private_data;
} Xv_panel_slider;

typedef struct {
    Xv_item		parent_data;
    Xv_opaque		private_data;
} Xv_panel_text;

typedef struct {
    Xv_item             parent_data;
    Xv_opaque           private_data;
} Xv_panel_num_text;

typedef struct {
    Xv_item		parent_data;
    Xv_opaque		private_data;
} Xv_panel_button;

typedef struct {
    Xv_item		parent_data;
    Xv_opaque		private_data;
} Xv_panel_list;

/*
 * For SunView 1 compatibility 
 */
typedef Panel_attr	Panel_attribute;

/*
 ***********************************************************************
 *		Globals
 ***********************************************************************
 */

/*
 * 	Variables 
 */
extern Xv_pkg 		xv_panel_pkg;

extern Xv_pkg		xv_panel_view_pkg;
extern Xv_pkg		xv_panel_item_pkg;
extern Xv_pkg		xv_panel_choice_pkg;
extern Xv_pkg		xv_panel_list_pkg;

extern Xv_pkg		xv_panel_button_pkg;
extern Xv_pkg		xv_panel_message_pkg;
extern Xv_pkg		xv_panel_slider_pkg;
extern Xv_pkg		xv_panel_text_pkg;
extern Xv_pkg           xv_panel_num_text_pkg;
/*
 * for PANEL_CYCLE choice item 
 */
extern struct pixrect	panel_cycle_pr;

/*
 * 	Public Functions 
 */

/*
 * Panel routines 
 */
extern Xv_opaque		panel_advance_caret();
extern Xv_opaque		panel_backup_caret();

/*
 * event mapping routines 
 */
extern				panel_handle_event();
extern void			panel_default_handle_event();
extern				panel_cancel();

/*
 * Panel_item action routines 
 */
extern				panel_begin_preview();
extern				panel_update_preview();
extern				panel_accept_preview();
extern				panel_cancel_preview();
extern				panel_accept_menu();
extern				panel_accept_key();

/*
 * utilities 
 */
extern Panel_setting	 	panel_text_notify();
extern struct pixrect  		*panel_button_image();

/* routines to translate event coordinates
 * Note that struct inputevent * is the same as
 * Event *, this is used for compatibility with previous
 * releases.
 */
extern struct inputevent	*panel_window_event();
extern struct inputevent	*panel_event();


/*
 * For SunView 1 Compatibility Only
 */

/*
 * Panel & Panel_item routines 
 * in Sunview 2 xv_ routines should be used.
 */
extern Panel_attribute_value	panel_get();
extern int               	panel_set();
extern                   	panel_paint();
extern			 	panel_free();
extern			 	panel_destroy_item();
extern Panel_item		panel_create_item();

#endif	~xview_panel_DEFINED
