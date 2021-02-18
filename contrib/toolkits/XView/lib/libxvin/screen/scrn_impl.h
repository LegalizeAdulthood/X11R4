/*	@(#)scrn_impl.h 20.21 89/11/09 SMI	*/

/****************************************************************************/
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/****************************************************************************/

#ifndef _screen_impl_h_already_included
#define _screen_impl_h_already_included

#include <xview/pkg.h>
#include <xview/screen.h>
#include <xview_private/scrn_vis.h>
#include <xview_private/xv_color.h>
#include <xview/xv_xrect.h>
#include <xview/window.h>

#define screen_attr_next(attr) (Screen_attr *)attr_next((caddr_t *)attr)

typedef struct scrn_cache_window {
    Xv_window			window;
    short			busy;
    short			borders;
    int				dynamic;
    struct scrn_cache_window	*next;
} Xv_cached_window;

typedef struct {
    Xv_Screen		public_self;	/* back pointer */
    int			number;		/* screen number on server */
    Xv_opaque		server;		/* always a Server */
    Xv_opaque		root_window;	/* always a Window */
    Screen_visual	*first_visual;	/* list of visual/depth */
    Visual              *static_visual;
    Visual              *dynamic_visual;
    Xv_Colormap_Seg     *default_cms;
    Xv_Colormap         *default_cmap;
    Xv_Colormap         *cmap_list;     /* list of additonal colormaps */
    Xv_xrectlist	clip_xrects;	/* clipping rectangle list */
    Xv_cached_window	*cached_windows;
    short		retain_windows; /* retain leaf windows for perf ? */
} Screen_info;

#define	SCREEN_PRIVATE(screen)	\
	XV_PRIVATE(Screen_info, Xv_screen_struct, screen)
#define	SCREEN_PUBLIC(screen)	XV_PUBLIC(screen)

/* screen_get.c */
Pkg_private Xv_opaque	screen_get_attr();

/* screen.c */
Pkg_private Xv_opaque	screen_set_avlist();

/* screen_layout.c */
Pkg_private int			screen_layout();

#endif
