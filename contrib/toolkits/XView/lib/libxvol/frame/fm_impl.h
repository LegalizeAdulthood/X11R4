/*	@(#)fm_impl.h 20.32 89/11/20 SMI	*/

/***********************************************************************/
/*	                 frame_impl.h/fm_impl.h	       		       */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef _frame_impl_h_already_included
#define _frame_impl_h_already_included

/* standard includes */
#ifndef FILE
#undef NULL
#include <stdio.h>
#endif FILE
#include <sys/time.h>
#include <xview/notify.h>
#include <xview/rect.h>
#include <xview/rectlist.h>

#ifndef	pixrect_hs_DEFINED
#define	pixrect_hs_DEFINED
/* <pixrect/pixrect_hs.h> without frame buffer variable include files */
#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <pixrect/pr_dblbuf.h>
#include <pixrect/pr_line.h>
#include <pixrect/pr_planegroups.h>
#include <pixrect/pr_util.h>
#include <pixrect/traprop.h>
#include <pixrect/memvar.h>
#include <pixrect/pixfont.h>
#include <rasterfile.h>
#include <pixrect/pr_io.h>
#endif	pixrect_hs_DEFINED

#include <xview/win_struct.h>	/* for WL_ links */
#include <xview/win_input.h>

/* all this for wmgr.h */
#include <xview/win_screen.h>
#include <xview/wmgr.h>
#include <xview_private/wmgr_decor.h>

#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/frame.h>
#include <xview/icon.h>
#include <xview/openmenu.h>
#include <xview/cms.h>

/* all this for XWMHints */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xview_private/fm_xutil.h>

#define	FRAME_CLASS_PRIVATE(f)	XV_PRIVATE(Frame_class_info, Xv_frame_class, f)
#define	FRAME_CLASS_PUBLIC(frame)	XV_PUBLIC(frame)
#define FRAME_PUBLIC(frame)	FRAME_CLASS_PUBLIC(frame)
#define FRAME_PRIVATE(f)	FRAME_CLASS_PRIVATE(f)

#define	BIT_FIELD(field)	int field : 1

typedef	struct	{
    Frame	public_self;	/* back pointer to object */
    char	*label;		/* string in name stripe & default icon */
    Icon 	icon;
    Icon 	default_icon;   /* Default Icon			*/
    Frame	first_subframe;	/* first subframe in list of subframes */
    Xv_Window	first_subwindow;/* first subwindow in list of subwindows */
    Rect	rectcache; 	/* rect of frame (tool relative) */
    Rect 	oldrect;  	/* quick check if size has changed */ 
    void	(*done_proc)();
    void	(*default_done_proc)();
    char	*leftfooter;	/* string in left footer */
    char	*rightfooter;	/* string in right footer */
    Xv_Window	focus_subwindow;/* last subwindow that had the input focus */
    Frame_WMHints	wmhints;	/* X window manager hints structure */
    Xv_singlecolor	bg;	/* background color */
    Xv_singlecolor	fg;	/* foreground color */

    struct {
	BIT_FIELD(bndrymgr);		/* movable borders between subwindows */
	BIT_FIELD(full);		/* current state is full */
	BIT_FIELD(no_confirm);		/* don't confirm on destroy */
	BIT_FIELD(icon_at_start);	/* in icon form when starting */
	/* Note that iconic is used only to detect a change in state.
	 * frame_is_iconic() is the function to call to determine
	 * if the frame is iconic or not.
	 */
	BIT_FIELD(busy);		/* whether frame is busy or not */
	BIT_FIELD(iconic);		/* whether frame is iconic or not */
	BIT_FIELD(dismiss);		/* whether frame is being dismissed or not */
    } status_bits;
} Frame_class_info;

#define	status_get(frame, field)	((frame)->status_bits.field)
#define	status_set(frame, field, value)	\
	(frame)->status_bits.field = ((value) != FALSE)


#define frame_attr_next(attr) (Frame_attribute *)attr_next((caddr_t *)attr)


#define	FRAME_EACH_CHILD(first, child)	\
    { Xv_Window	_next; \
    for ((child) = (first), \
	 (child) && \
	 (_next = xv_get((child), XV_KEY_DATA, FRAME_NEXT_CHILD)); \
	 (child); (child) = _next, \
	 (child) && \
	 (_next = xv_get((child), XV_KEY_DATA, FRAME_NEXT_CHILD))) {

#define	FRAME_EACH_SUBWINDOW(frame, child)	\
    FRAME_EACH_CHILD((frame)->first_subwindow, child)

#define	FRAME_EACH_SUBFRAME(frame, child)	\
    FRAME_EACH_CHILD((frame)->first_subframe, child)

#define	FRAME_EACH_SHOWN_SUBWINDOW(frame, child)	\
    FRAME_EACH_SUBWINDOW(frame, child)	\
	if (!xv_get(child, XV_SHOW)) \
	    continue;

#define	FRAME_END_EACH	}}

#define	EXTEND_HEIGHT(child)	\
    ((int) xv_get((child), WIN_DESIRED_HEIGHT) == WIN_EXTEND_TO_EDGE)

#define	EXTEND_WIDTH(child)	\
    ((int) xv_get((child), WIN_DESIRED_WIDTH) == WIN_EXTEND_TO_EDGE)


#define	frame_getnormalrect(frame, rectp) \
	    win_getrect(FRAME_PUBLIC(frame), (rectp))
#define	frame_setnormalrect(frame, rectp) \
	    win_setrect(FRAME_PUBLIC(frame), (rectp))

#define frame_is_iconic(frame)  (status_get(frame, iconic))


#define	FRAME_ICON_WIDTH	64
#define	FRAME_ICON_HEIGHT	64

/* frame.c */
Pkg_private int 	frame_init();
Pkg_private int		frame_notify_count;
Pkg_private void	frame_default_done_func();

/* frame_get.c */
Pkg_private Xv_opaque	frame_get_attr();

/* frame_set.c */
Pkg_private Xv_opaque	frame_set_avlist();

/* frame_layout.c */
Pkg_private int			frame_layout();

/* frame_destroy.c */
Pkg_private int			frame_destroy();

/* frame_menu.c */
Pkg_private void	frame_full();

/* frame_win.c */
Pkg_private int		frame_set_parent();
Pkg_private int		frame_is_exposed();

/* frame_input.c */
Pkg_private Notify_value frame_input();

/* frame_display.c */
Pkg_private void	frame_display_label();
Pkg_private int		frame_set_color();

/* frame_geom.c */
Pkg_private int		frame_height_from_lines();
Pkg_private int		frame_width_from_columns();
Pkg_private void	frame_position_sw();

/* frame_cmdline.c */
Pkg_private int		frame_set_cmdline_options();
Pkg_private int		frame_set_icon_cmdline_options();

/* frame_rescale.c */
Pkg_private void	frame_rescale_subwindows();

/* frame_sw.c */
Pkg_private void	frame_layout_subwindows();

#endif
