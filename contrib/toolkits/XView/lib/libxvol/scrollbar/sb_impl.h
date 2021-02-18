/*      @(#)sb_impl.h 1.27 89/08/18 */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef	__scrollbar_impl_h 
#define	__scrollbar_impl_h 

/*
 * Module:	scrollbar_impl.h
 *
 * Level:	private
 *
 * Description:
 *
 *	Declarations for data structures internal to the scrollbar 
 *
 */


/*
 * Include files: <only those required by THIS file>
 */

#include <xview/xview.h>
#include <xview/font.h>
#include <xview/scrollbar.h>
#include <xview/openmenu.h>
#include <xview/rect.h>
#include <xview/notify.h>

/*
 * Global Defines:
 */
 
/* macros to convert variable from/to public/private form */
#define SCROLLBAR_PRIVATE(scrollbar)  \
	XV_PRIVATE(Xv_scrollbar_info, Xv_scrollbar, scrollbar)
#define SCROLLBAR_PUBLIC(scrollbar)   XV_PUBLIC(scrollbar)

#define Max_offset(sb)	\
	((sb)->object_length * (1 + (sb)->overscroll/100) - (sb)->view_length)

/* uninitialized lengths */
#define	SCROLLBAR_DEFAULT_LENGTH 0
#define VSB_ROW_IMAGE_WIDTH		16
#define VSB_ROW_IMAGE_HEIGHT	8
#define HSB_ROW_IMAGE_WIDTH		16
#define HSB_ROW_IMAGE_HEIGHT	12
#define SCROLLBAR_CABLE_GAP		2
#define SCROLLBAR_CABLE_THICKNESS	3

/* time in msec between repeats of single line scroll */
#define SCROLLBAR_REPEAT_DELAY 			500
#define SCROLLBAR_REPEAT_PAGE_INTERVAL	        100
#define SCROLLBAR_REPEAT_LINE_INTERVAL	          1

#define SCROLLBAR_POSITION_UNCHANGED -1

#define SB_ELEVATOR_INNER_TOP 			 1
#define SB_ELEVATOR_INNER_LEFT 			 1

#define SB_BLINE_IMAGE_LEFT			 2
#define SB_BLINE_IMAGE_TOP			 0
#define SB_FLINE_IMAGE_LEFT			 2
#define SB_FLINE_IMAGE_TOP			 2

/* dimensions for small size scrollbar */
#define SCROLLBAR_SMALL_THICKNESS		17
#define	SB_SMALL_MARGIN				 2
#define	SB_SMALL_MARKER_HEIGHT			 5
#define SB_SMALL_ELEVATOR_BOX_HEIGHT	 	13
#define SB_SMALL_FLINE_IMAGE_WIDTH		 6
#define SB_SMALL_FLINE_IMAGE_HEIGHT		 4
#define SB_SMALL_LINE_IMAGE_LEFT_MARGIN		 1
#define SB_SMALL_LINE_IMAGE_TOP_MARGIN		 3
#define SB_REDUCED_SMALL_LINE_IMAGE_TOP_MARGIN	 2

/* dimensions for regular size scrollbar */
/* #define SCROLLBAR_MEDIUM_THICKNESS		19 (defined in scrollbar.h) */
#define	SB_MEDIUM_MARGIN			 2
#define	SB_MEDIUM_MARKER_HEIGHT			 6
#define SB_MEDIUM_ELEVATOR_BOX_HEIGHT	 	15
#define SB_MEDIUM_FLINE_IMAGE_WIDTH		 8
#define SB_MEDIUM_FLINE_IMAGE_HEIGHT		 5
#define SB_MEDIUM_LINE_IMAGE_LEFT_MARGIN	 2
#define SB_MEDIUM_LINE_IMAGE_TOP_MARGIN		 4
#define SB_REDUCED_MEDIUM_LINE_IMAGE_TOP_MARGIN	 2

/* dimensions for large size scrollbar */
#define SCROLLBAR_LARGE_THICKNESS		23
#define	SB_LARGE_MARGIN				 3
#define	SB_LARGE_MARKER_HEIGHT			 7
#define SB_LARGE_ELEVATOR_BOX_HEIGHT	 	17
#define SB_LARGE_FLINE_IMAGE_WIDTH		 8
#define SB_LARGE_FLINE_IMAGE_HEIGHT		 5
#define SB_LARGE_LINE_IMAGE_LEFT_MARGIN		 2
#define SB_LARGE_LINE_IMAGE_TOP_MARGIN		 5
#define SB_REDUCED_LARGE_LINE_IMAGE_TOP_MARGIN	 3

/* dimensions for extralarge size scrollbar */
#define SCROLLBAR_XLARGE_THICKNESS		29
#define	SB_XLARGE_MARGIN			 4
#define	SB_XLARGE_MARKER_HEIGHT			 9
#define SB_XLARGE_ELEVATOR_BOX_HEIGHT	 	21
#define SB_XLARGE_FLINE_IMAGE_WIDTH		12
#define SB_XLARGE_FLINE_IMAGE_HEIGHT		 7
#define SB_XLARGE_LINE_IMAGE_LEFT_MARGIN	 3
#define SB_XLARGE_LINE_IMAGE_TOP_MARGIN		 7
#define SB_REDUCED_XLARGE_LINE_IMAGE_TOP_MARGIN	 5

/*
 * Typedefs:
 */
 
typedef struct scrollbar			Xv_scrollbar_info;
 
struct scrollbar {
    Scrollbar		public_self;		/* Back pointer */

	char			bottom_anchor_glyph; /* current bottom anchor char in
										  * olgyph font. */
    int				cable_height;
    int				cable_start;
    int				can_split;
    void			(*compute_scroll_proc)();
    int				creating;
    Scrollbar_setting	direction;
	char			elevator_glyph;	/* current scrollbar elevator character
									 * in olglyph font */
	int				elevator_top;	/* top of elevator */
	Xv_Font			glyph_font;
	struct timeval	last_select_time;	/* timestamp of last ACTION_SELECT */
    unsigned long	last_view_start;
    Xv_opaque		managee;
    int				marker_height;
    Menu			menu;
    int				moved_mouse_pointer;
    int				moved_mouse_pointer_x;
    int				moved_mouse_pointer_y;
	int				multiclick_timeout;
    int				(*normalize_proc)();
    unsigned long	object_length;
    int				overscroll;		/* 0 - 100% */
    unsigned int	page_length;
	int				painted;
    unsigned int	pixels_per_unit;
    int			 	prev_page;		/* last page displayed */
    int				reduced;
    Frame_rescale_state	scale;
	char			top_anchor_glyph; /* current top anchor char in
									   * olgyph font. */
    Event			transit_event;
    Scroll_motion	transit_motion;
    int				transit_occurred;
    unsigned int	view_length;
    unsigned long	view_start;
};


/*
 * Global Variable Declarations:
 */

extern	Attr_attribute	sb_context_key;
extern	Attr_attribute	sb_split_view_menu_item_key;
extern	Attr_attribute	sb_join_view_menu_item_key;


/*
 * Package-private Function Declarations:
 */

/* from sb_destroy.c */
Pkg_private	int scrollbar_destroy_internal();

/* from sb_event.c */
Pkg_private	Notify_value	scrollbar_handle_event();
Pkg_private	void				scrollbar_line_to_top();
Pkg_private	void				scrollbar_top_to_line();

/* from sb_get.c */
Pkg_private Xv_opaque	scrollbar_get_internal();

/* from sb_init.c */
Pkg_private	void scrollbar_init_common();
Pkg_private void scrollbar_create_standard_menu();

/* from sb_paint.c */
Pkg_private void scrollbar_clear_damage();
Pkg_private void scrollbar_paint_elevator_move();
Pkg_private void scrollbar_invert_region();
Pkg_private void scrollbar_paint_page_number();

/* from sb_pos.c */
Pkg_private	void scrollbar_init_positions();
Pkg_private	void scrollbar_position_elevator();
Pkg_private void scrollbar_absolute_position_elevator();
Pkg_private	void scrollbar_to_start_marker_rect();
Pkg_private	void scrollbar_to_end_marker_rect();
Pkg_private	void scrollbar_to_start_marker_innerrect();
Pkg_private	void scrollbar_to_end_marker_innerrect();
Pkg_private	void sb_normalize_rect();
Pkg_private void scrollbar_elevator_rect();
Pkg_private void scrollbar_available_cable();
Pkg_private void sb_line_backward_region();
Pkg_private void sb_absolute_region();
Pkg_private void sb_reduced_elevator();
Pkg_private void sb_fline_place_rect();
Pkg_private void sb_rescale();
Pkg_private void sb_line_backward_region();
Pkg_private void sb_line_forward_region();
Pkg_private void sb_absolute_region();
Pkg_private int sb_margin();
Pkg_private int sb_marker_height();
Pkg_private int sb_elevator_height();

/* from sb_scroll.c */
Pkg_private 	int scrollbar_scroll();
Pkg_private 	int scrollbar_scroll_to_offset();

/* from sb_set.c */
Pkg_private Xv_opaque	scrollbar_set_internal();

/* from sb.c */
Pkg_private int	scrollbar_create_internal();

/* from sb_event.c */
Pkg_private Menu scrollbar_gen_menu();
Pkg_private Notify_value scrollbar_handle_event();
Pkg_private void scrollbar_line_to_top();
Pkg_private void scrollbar_top_to_line();
Pkg_private void scrollbar_last_position();
Pkg_private void scrollbar_split_view_from_menu();
Pkg_private void scrollbar_join_view_from_menu();

#endif	__scrollbar_impl_h
