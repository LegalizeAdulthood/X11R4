 /*	@(#)frame.h 20.41 89/10/03 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_frame_DEFINED
#define xview_frame_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/window.h>
#include <xview/attrol.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */

#define FRAME			FRAME_BASE
#define FRAME_BASE		&xv_frame_base_pkg
#define FRAME_CMD		&xv_frame_cmd_pkg
#define FRAME_PROPS		FRAME_CMD
#define FRAME_HELP		&xv_frame_help_pkg
#define FRAME_CLASS		&xv_frame_class_pkg

#define	ROOT_FRAME		((Frame)0)

#define FRAME_SHOW_HEADER	FRAME_SHOW_LABEL

/*
 * Utility Macros 
 */

#define frame_fit_all(frame) 					\
{ 								\
    Xv_Window win; 						\
    int n = 0; 							\
    while (win = window_get(frame, FRAME_NTH_SUBWINDOW, n++, 0))\
	window_fit(win); 					\
    window_fit(frame); 						\
}

#define frame_done_proc(frame) 					\
   (((void (*)())window_get(frame, FRAME_DONE_PROC))(frame))

#define frame_default_done_proc(frame) 				\
   (((void (*)())window_get(frame, FRAME_DEFAULT_PROC))(frame))

/*
 * PRIVATE #defines 
 */

#define	FRAME_ATTR(type, ordinal)	ATTR(ATTR_PKG_FRAME, type, ordinal)

/*
 * BUG: maybe these should be attributes 
 */


/*
 * width of border around a frame 
 */
#define FRAME_BORDER_WIDTH      (0)
/*
 * width of border around a subwindow 
 */
#define FRAME_SUBWINDOW_SPACING (FRAME_BORDER_WIDTH)


/*
 * PUBLIC #defines 
 * For SunView 1 Compatibility
 */

#define FRAME_TYPE		ATTR_PKG_FRAME

#define	FRAME_ARGS		XV_INIT_ARGS
#define	FRAME_ARGC_PTR_ARGV	XV_INIT_ARGC_PTR_ARGV
#define	FRAME_CMDLINE_HELP_PROC	XV_USAGE_PROC
#define	FRAME_LABEL		XV_LABEL
#define FRAME_OPEN_RECT		WIN_RECT

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures	
 ***********************************************************************
 */

typedef	Xv_opaque 	Frame;
typedef Xv_opaque	Frame_cmd;
typedef Xv_opaque	Frame_props;
typedef Xv_opaque	Frame_help;

typedef struct {
    Xv_window_struct	parent_data;
    Xv_opaque		private_data;
} Xv_frame_class;

typedef struct {
    Xv_frame_class	parent_data;
    Xv_opaque		private_data;
} Xv_frame_base;

typedef Xv_frame_base 	Xv_frame_cmd;
typedef Xv_frame_base 	Xv_frame_props;
typedef Xv_frame_base 	Xv_frame_help;

typedef enum {
	/*
	 * PUBLIC attributes 
	 */
	FRAME_BACKGROUND_COLOR	= FRAME_ATTR(ATTR_SINGLE_COLOR_PTR,	   5),
	FRAME_BUSY		= FRAME_ATTR(ATTR_BOOLEAN,		  10),
	FRAME_CLOSED	= FRAME_ATTR(ATTR_BOOLEAN,		  15),
	FRAME_CLOSED_RECT	= FRAME_ATTR(ATTR_RECT_PTR,		  20),
	FRAME_CMD_PANEL		= FRAME_ATTR(ATTR_OPAQUE,		  25),
	FRAME_CURRENT_RECT	= FRAME_ATTR(ATTR_RECT_PTR,		  35),
	FRAME_OLD_RECT          = FRAME_ATTR(ATTR_RECT_PTR,               36),
	FRAME_DEFAULT_DONE_PROC	= FRAME_ATTR(ATTR_FUNCTION_PTR,		  40),
	FRAME_DONE_PROC		= FRAME_ATTR(ATTR_FUNCTION_PTR,		  45),
	FRAME_FOREGROUND_COLOR	= FRAME_ATTR(ATTR_SINGLE_COLOR_PTR,	  50),
	FRAME_ICON		= FRAME_ATTR(ATTR_OPAQUE,		  55),
	FRAME_INHERIT_COLORS	= FRAME_ATTR(ATTR_BOOLEAN,		  60),
	FRAME_LEFT_FOOTER	= FRAME_ATTR(ATTR_STRING,		  65),
	FRAME_NO_CONFIRM	= FRAME_ATTR(ATTR_BOOLEAN,		  70),
	FRAME_NTH_SUBFRAME	= FRAME_ATTR(ATTR_INT,			  75),
	FRAME_NTH_SUBWINDOW	= FRAME_ATTR(ATTR_INT,			  80),
	FRAME_PROPERTIES_PROC	= FRAME_ATTR(ATTR_FUNCTION_PTR,		  85),
	FRAME_CMD_PUSHPIN_IN	= FRAME_ATTR(ATTR_BOOLEAN,		 105),
	FRAME_RIGHT_FOOTER	= FRAME_ATTR(ATTR_STRING,		 115),
	FRAME_SHOW_FOOTER	= FRAME_ATTR(ATTR_BOOLEAN,		 125),
	FRAME_SHOW_RESIZE_CORNER = FRAME_ATTR(ATTR_BOOLEAN,		 128),
	FRAME_SHOW_LABEL	= FRAME_ATTR(ATTR_BOOLEAN,		 130),
	FRAME_GROUP_LEADER	= FRAME_ATTR(ATTR_BOOLEAN,		 135),
	/*
	 * PRIVATE attributes 
	 */
	FRAME_NEXT_CHILD	= FRAME_ATTR(ATTR_OPAQUE,		 140),
	FRAME_SCALE_STATE	= FRAME_ATTR(ATTR_INT,			 145),
	FRAME_SUBWINDOWS_ADJUSTABLE	
				= FRAME_ATTR(ATTR_BOOLEAN,		 150),
        FRAME_COUNT             = FRAME_ATTR(ATTR_INT,                   160),
} Frame_attribute;

#define	FRAME_PROPS_PUSHPIN_IN	FRAME_CMD_PUSHPIN_IN
#define	FRAME_PROPS_PANEL	FRAME_CMD_PANEL

/*
 *  values for scale state
 */
#define Frame_rescale_state	Window_rescale_state
#define FRAME_SCALE_SMALL	WIN_SCALE_SMALL
#define FRAME_SCALE_MEDIUM	WIN_SCALE_MEDIUM
#define FRAME_SCALE_LARGE	WIN_SCALE_LARGE
#define FRAME_SCALE_EXTRALARGE	WIN_SCALE_EXTRALARGE

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

extern Xv_pkg	xv_frame_class_pkg;
extern Xv_pkg	xv_frame_base_pkg;
extern Xv_pkg	xv_frame_cmd_pkg;
extern Xv_pkg	xv_frame_props_pkg;
extern Xv_pkg	xv_frame_help_pkg;

/*
 * SunView Private functions
 */
extern void		frame_cmdline_help();
extern void		frame_grant_extend_to_edge();
extern void		frame_format_string_to_rect();
extern void		frame_kbd_use();
extern void		frame_kbd_done();

#endif xview_frame_DEFINED

