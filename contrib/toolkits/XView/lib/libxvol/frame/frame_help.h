/*	@(#)frame_help.h 1.18 89/11/20 SMI	*/

/***********************************************************************/
/*	                      frame_help.h		               */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef _frame_help_h_already_included
#define _frame_help_h_already_included

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

/* all this for XWMHints */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define	FRAME_HELP_PRIVATE(f)	XV_PRIVATE(Frame_help_info, Xv_frame_help, f)
#define FRAME_HELP_PUBLIC(f)	XV_PUBLIC(f)
#define FRAME_CLASS_FROM_HELP(f) FRAME_PRIVATE(FRAME_HELP_PUBLIC(f))

#define FRAME_HELP_FLAGS  WMDecorationHeader

typedef	struct	{
    Frame	public_self;	/* back pointer to object */
    WM_Win_Type	win_attr;	/* _OL_WIN_ATTR */			

    struct {
	BIT_FIELD(show_label); 		/* show label or not */
    } status_bits;
} Frame_help_info;

/* frame_help_get.c */
Pkg_private int		frame_help_init();

/* frame_help_get.c */
Pkg_private Xv_opaque	frame_help_get_attr();

/* frame_help_set.c */
Pkg_private Xv_opaque	frame_help_set_avlist();

/* frame_help_destroy.c */
Pkg_private int			frame_help_destroy();

#endif
