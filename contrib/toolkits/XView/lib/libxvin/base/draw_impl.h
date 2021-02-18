/*	@(#)draw_impl.h 20.22 89/10/17 SMI	*/

/***********************************************************************/
/*	                      draw_impl.h			       */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef _xview_private_drawable_impl_h_already_defined
#define _xview_private_drawable_impl_h_already_defined

#include <xview/pkg.h>
#include <xview/drawable.h>
#include <xview_private/scrn_vis.h>
#include <xview_private/xv_color.h>


/* Although this is a private implementation header file, it is included
 * by most of the Drawable sub-pkgs for performance reasons.
 */
#define drawable_attr_next(attr) \
	(Drawable_attribute *)attr_next((Xv_opaque *)attr)

#define	DRAWABLE_PRIVATE(drawable) \
			XV_PRIVATE(Xv_Drawable_info, Xv_drawable_struct, drawable)
/* Note: Xv_Drawable_info has no public_self field, so XV_PUBLIC cannot work
 * and DRAWABLE_PUBLIC is undefined.
 */


/***********************************************************************/
/*	        	Structures 				       */
/***********************************************************************/

typedef struct drawable_info {
    XID			 xid;
    short		 fg;
    short		 bg;
    Xv_Colormap_Seg	*cms;
    unsigned long	 plane_mask;
    Screen_visual	*visual;
	/* Flags */
    unsigned		 private_gc	: 1;	/* Should be gc itself? */
    unsigned		 no_focus	: 1;	/* Don't set focus on click */
    unsigned		 has_focus	: 1;	/* Currently has focus */
    unsigned		 new_clipping	: 1;	/* new clipping has been set*/
    unsigned		 dynamic_color  : 1;    /* created with dynamic cmap*/
    unsigned		 is_bitmap	: 1;	/* use 1 and 0 as only colors */
} Xv_Drawable_info;

#define	xv_xid(info)		((info)->xid)
#define xv_fg(info)		((info)->fg)
#define xv_bg(info)		((info)->bg)
#define	xv_cms(info)		((info)->cms)
#define xv_plane_mask(info)	((info)->plane_mask)
#define xv_visual(info)		((info)->visual)
#define	xv_cmap(info)		((info)->cms->cms_cmap->id)
#define	xv_display(info)	((info)->visual->display)
#define	xv_server(info)		((info)->visual->server)
#define	xv_screen(info)		((info)->visual->screen)
#define	xv_root(info)		((info)->visual->root_window)
#define	xv_depth(info)		((info)->visual->depth)
#define	xv_image_1(info)	((info)->visual->image_1)
#define	xv_image_8(info)	((info)->visual->image_8)
#define xv_dynamic_color(info)  ((info)->dynamic_color)
#define xv_is_bitmap(info)	((info)->is_bitmap)

#define	xv_gc(public, info) \
	((info)->private_gc ? xv_private_gc((public)) : (info)->visual->gc)

#define	xv_set_image(info, im)	(info)->visual->image = im

#define xv_no_focus(info) ((info)->no_focus)
#define xv_set_no_focus(info, f) (info)->no_focus = (f)

#define xv_has_focus(info) ((info)->has_focus)
#define xv_set_has_focus(info, f) (info)->has_focus = (f)

#define DRAWABLE_INFO_MACRO(_win_public, _info)\
{\
   if (_win_public) {\
      Xv_opaque  _object;\
      XV_OBJECT_TO_STANDARD(_win_public, "drawable_info", _object);\
      _info = (_object ? ((Xv_Drawable_info *)(((Xv_drawable_struct *)(_object))->private_data)) : 0);\
   } else _info = 0;\
}
        
extern GC		xv_private_gc();
extern Xv_Drawable_info	*drawable_info();
/* drawable.c */
Pkg_private Xv_opaque        drawable_get_attr();
Pkg_private int              drawable_init();
Pkg_private int              drawable_destroy();

#endif
