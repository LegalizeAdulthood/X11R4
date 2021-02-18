/*	@(#)panel_impl.h 20.26 89/08/18	*/

/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */

#ifndef panel_impl_defined
#define panel_impl_defined

/* turn on ecd code */
#define	ecd_panel
#define	ecd_slider

#ifndef FILE
#undef NULL
#include <stdio.h>
#endif FILE
#include <sys/types.h>
#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/frame.h>
#include <xview/sel_svc.h>
#include <xview/window_hs.h>
#include <xview/panel.h>
#include <xview_private/item_impl.h>
#include <X11/Xlib.h>


/* panels and panel_items are both of type Xv_panel_or_item so that we
 * can pass them to common routines.
 */
#define PANEL_PRIVATE(p)	XV_PRIVATE(Panel_info, Xv_panel, p)
#define PANEL_PUBLIC(panel)     XV_PUBLIC(panel)

/*			per panel status flags  		         */

#define BLINKING		0x00000001
#define TIMING			0x00000002
#define SHOW_MENU_SET	        0x00000010
#ifdef ecd_panel
#define ADJUST_IS_PENDING_DELETE 0x00000020
#endif ecd_panel
#define PANEL_PAINTED	0x00000040
#define PANEL_HAS_INPUT_FOCUS	0x00000080
#define QUICK_MOVE		0x00000100	/* quick move pending */

#define blinking(panel)			((panel)->status & BLINKING)
#define show_menu_set(panel)		((panel)->status & SHOW_MENU_SET)
#ifdef ecd_panel
#define adjust_is_pending_delete(panel) ((panel)->status & ADJUST_IS_PENDING_DELETE)
#endif ecd_panel
#define panel_painted(panel)	((panel)->status & PANEL_PAINTED)
#define panel_has_input_focus(panel) ((panel)->status & PANEL_HAS_INPUT_FOCUS)
#define quick_move(panel)	((panel)->status & QUICK_MOVE)

#define viewable_rect(panel, pw)		((Rect *) xv_get((panel)->public_self, CANVAS_VIEWABLE_RECT, (pw)))
#define viewable_height(panel, pw)		(((Rect *) xv_get((panel)->public_self, CANVAS_VIEWABLE_RECT, (pw)))->r_height)
#define viewable_width(panel, pw)		(((Rect *) xv_get((panel)->public_self, CANVAS_VIEWABLE_RECT, (pw)))->r_width)

#define panel_height(panel)		((int) xv_get((panel)->public_self, CANVAS_HEIGHT))
#define panel_width(panel)		((int) xv_get((panel)->public_self, CANVAS_WIDTH))

#define rect_copy(to, from)		to.r_left = from.r_left; \
                                to.r_top = from.r_top;   \
                                to.r_width = from.r_width; \
                                to.r_height = from.r_height; 
                                
                                
/* 			structures                                      */

/******************* selection info ****************************************/

typedef struct panel_selection {
   Seln_rank		rank;	/* SELN_{PRIMARY, SECONDARY, CARET} */
   unsigned int		is_null;/* TRUE if the selection has no chars */
   Item_info		*ip;	/* item with the selection */
} Panel_selection;


#define	panel_seln(panel, rank)	(&((panel)->selections[(unsigned int)(rank)]))


/***************************** panel **************************************/
/* *** NOTE: The first three fields of the
 * panel struct must match those of the panel_item
 * struct, since these are used interchangably in 
 * some cases.
 */
typedef struct panel_pw_struct {
	Xv_Window		    pw;
	Xv_Window			view;
	struct panel_pw_struct	*next;
} Panel_paint_window;

typedef struct panel_info {
    /****  DO NOT CHANGE THE ORDER OR PLACEMENT OF THESE THREE FIELDS ****/
    Panel_ops		*ops;		/* panel specific operations */
    unsigned int	flags;		/* default item flags */
    Panel		public_self;	/* back pointer to object */
    /**** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ****/

    Pixfont		*bold_font;
    Item_info		*caret;		/* panel item with the caret */
    int			caret_on;	/* caret is painted */
    Pixrect		*caret_pr;	/* current caret pixrect */
    Xv_opaque		client_data;	/* for client use */
    Item_info		*current;
    int			current_col_x;	/* left position of current column */
    Panel_item		default_item;
    int			destroying;
    char		edit_bk_char;
    char		edit_bk_line;
    char		edit_bk_word;
    int			(*event_proc)();
    int			extra_height;
    int			extra_width;
    GC			gc;		/* Graphics Context */
    int			h_margin;	/* horizontal margin */
    int			item_x;
    int			item_x_offset;
    int			item_y;
    int			item_y_offset;
    Item_info		*items;
    Panel_setting	layout;		/* HORIZONTAL, VERTICAL */
    int			(*layout_proc)(); /* interposed window layout proc */
    int			lowest_bottom;	/* lowest bottom of any item */
    int			max_item_y;	/* lowest item on row ??? */
    Panel_paint_window	*paint_window;
    Panel_setting	repaint;	/* default repaint behavior  */
    int			(*repaint_proc)();
    int			rightmost_right; /* rightmost right of any item */
    Panel_selection	selections[(int) SELN_UNSPECIFIED];
    Xv_opaque		seln_client;	/* selection svc client id */
    char		*shelf;		/* contents of shelf seln */
    int                 SliderActive;  /* TRUE if slider is previewing */
    unsigned int	status;		/* current event state */
    struct itimerval	timer_full;	/* initial secs & usecs */
    int			v_margin;	/* vertical margin */
} Panel_info;


#define	PANEL_EACH_PAINT_WINDOW(panel, pw)	\
   {Panel_paint_window *_next; \
    for (_next = (panel)->paint_window; _next != NULL; _next = _next->next) { \
    	(pw) = _next->pw;

#define	PANEL_END_EACH_PAINT_WINDOW	}}


/***********************************************************************/
/* external declarations of private variables                          */
/***********************************************************************/

Pkg_private Attr_attribute	panel_context_key;
Pkg_private Attr_attribute	panel_pw_context_key;

/***********************************************************************/
/* Pkg_privateal declarations of private functions                          */
/***********************************************************************/

Pkg_private 	Xv_opaque 		panel_set_avlist();
Pkg_private 	Xv_opaque	 	panel_get_attr();

Pkg_private 	Item_info 		*panel_successor();
Pkg_private 	struct pr_size 		panel_make_image();
Pkg_private 	Rect			panel_enclosing_rect();
Pkg_private 	Item_info 		*panel_append();
Pkg_private  int		 	panel_col_to_x();
Pkg_private  int		 	panel_row_to_y();
Pkg_private  char                   *panel_strsave();
Pkg_private int 			panel_nullproc();
Pkg_private int 			panel_fonthome();
Pkg_private				panel_find_default_xy();
Pkg_private void			panel_image_set_font();
Pkg_private void			panel_free_image();
Pkg_private void			panel_set_bold_label_font();

/* paint routines */
Pkg_private void			panel_redisplay();
Pkg_private void			panel_display();
Pkg_private int          		panel_paint_item();
Pkg_private void          		panel_clear_item();
Pkg_private void			panel_paint_text();

Pkg_private Notify_value		panel_use_event();
Pkg_private void			panel_caret_on();
Pkg_private void			panel_caret_invert();

/* selection service routines */
Pkg_private void			panel_seln_init();
Pkg_private void			panel_seln_destroy();
Pkg_private void			panel_seln_inform();
Pkg_private Seln_holder		panel_seln_inquire();
Pkg_private void			panel_seln_acquire();
Pkg_private void			panel_seln_cancel();
Pkg_private void			panel_seln_dehilite();
Pkg_private void			panel_seln_hilite();

/* scrollbar routines */
Pkg_private 		        panel_normalize_scroll();
Pkg_private void			panel_update_extent();
Pkg_private void			panel_update_scrollbars();

Pkg_private int			panel_resize();


/* panel_event.c */
Pkg_private Notify_value	panel_notify_panel_event();
Pkg_private Notify_value	panel_notify_view_event();
Pkg_private Notify_value	panel_notify_event();
Pkg_private void		panel_itimer_set();

/* panel_select.c */
Pkg_private				panel_default_event();


/***********************************************************************/
/* Pkg_private declarations of private data                               */
/***********************************************************************/

Pkg_private	Pixrect			panel_empty_pr;
Pkg_private	Pixrect			panel_caret_pr;
Pkg_private	Pixrect			panel_ghost_caret_pr;
Pkg_private Pixrect			panel_gray_pr;

#define	set(value)	((value) != -1) 

#endif
