/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef	__openwin_impl_h 
#define	__openwin_impl_h 

/*
 * Module:	openwin_impl.h
 * Product:	SunView 2.0
 *
 * Level:	Private
 *
 * Description:
 *
 *	Defines internal data structures for managing openlook window
 *
 */


/*
 * Include files: <only those required by THIS file>
 */
#include <xview/xview.h>
#include <xview/openwin.h>
#include <xview/scrollbar.h>
#include <xview/rect.h>



/*
 * Global Defines:
 */
 
/* macros to convert variable from/to public/private form */
#define OPENWIN_PRIVATE(win)  \
	XV_PRIVATE(Xv_openwin_info, Xv_openwin, win)
#define OPENWIN_PUBLIC(win)   XV_PUBLIC(win)

#define OPENWIN_REGULAR_VIEW_MARGIN	2
#define OPENWIN_VIEW_BORDER_WIDTH 2

#define OPENWIN_SPLIT_VERTICAL_MINIMUM  50
#define OPENWIN_SPLIT_HORIZONTAL_MINIMUM  50

#define openwin_sb(view, direction)    \
   ((view)->sb[((direction) == SCROLLBAR_VERTICAL) ? 0 : 1])
   
#define openwin_set_sb(view, direction, sb) \
   openwin_sb((view), (direction)) = sb
   

/*
 * Typedefs:
 */
 
typedef struct	openwin_view_struct		Openwin_view_info;
typedef struct	openwin_info_struct		Xv_openwin_info;

struct openwin_view_struct {
	Xv_opaque	view;
	Scrollbar	sb[2]; /* 0 -> vertical 1 -> horizontal */
	Rect		enclosing_rect; /* full area the view takes up --
	               includes margins, borders and scrollabrs */
	
	int			right_edge; /* view against openwin's right edge */
	int			bottom_edge; /* view against bottom edge */
	Openwin_view_info	*next_view;
    Xv_openwin_info       *owin;
};

typedef struct {
	int auto_clear : 1;
	int adjust_vertical : 1;
	int adjust_horizontal : 1;
	int no_margin : 1;
	int created : 1;
	int show_borders: 1;
	int	removing_scrollbars : 1;
} OWStatus;


typedef struct openwin_info_struct {
   	Openwin		public_self;		/* Back pointer */
   	
   	Xv_pkg		*view_class;
	Openwin_view_info	*views;
    int			margin;
    Rect		cached_rect;
    Scrollbar	vsb_on_create;	/* cached scrollbar until view is */
    Scrollbar	hsb_on_create;	/* created */
    Attr_avlist	view_avlist; 	/* cached view avlist on create */
    Attr_avlist	view_end_avlist;
    Xv_opaque	seln_client;		/* selection svc client id */
    Openwin_view_info	*seln_view;	/* selected view, if any */
    OWStatus		status_bits;
    int			nbr_cols;		/* WIN_COLUMNS specified by client */
    int			nbr_rows;		/* WIN_ROWS specified by client */
    int			(*layout_proc)();
    void		(*split_init_proc)();
    void		(*split_destroy_proc)();
};

#define	status_is(openwin, field)		((openwin)->status_bits.field)
#define	status_set(openwin, field)	status_is(openwin, field) = TRUE
#define	status_reset(openwin, field)	status_is(openwin, field) = FALSE


/*
 * Structures and Enumerations:
 */

/*
 * Global Variable Declarations:
 */
extern	Attr_attribute openwin_view_context_key;

/*
 * Global Function Declarations:
 */
 /* openwin.c */
 Pkg_private int openwin_init();
 Pkg_private int openwin_destroy();
 
/* openwin_evt.c */
extern	Notify_value	openwin_event();
extern	Notify_value	openwin_view_event();


/* openwin_get.c */
extern	Xv_opaque		openwin_get();

/* openwin_pnt.c */
extern	void				openwin_clear_damage();
extern	void				openwin_paint_borders();
extern	void				openwin_hilite_view();

/* openwin_rsze.c */
extern	int				openwin_adjust_views();
extern	void				openwin_adjust_view();
extern	void				openwin_place_scrollbar();


/* openwin_seln.c */
extern void 			openwin_seln_function();
extern Xv_opaque		openwin_seln_reply();
extern void				openwin_select();
extern void				openwin_select_view();

/* openwin_set.c */
extern	Xv_opaque		openwin_set();

/* openwin_view.c */
extern	void				openwin_create_initial_view();
extern	void				openwin_destroy_views();
extern	int				openwin_count_views();
extern Openwin_view_info *openwin_nth_view();
extern	int				openwin_viewdata_for_view();
extern	void				openwin_split_view();
extern	int				openwin_fill_view_gap();
extern	void				openwin_copy_scrollbar();




#endif	__openwin_impl_h
