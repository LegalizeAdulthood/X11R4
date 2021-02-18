/*      @(#)font_impl.h 20.19 89/11/21 SMI      */

/***********************************************************************/
/*	                      font_impl.h			       */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef font_impl_h_already_defined
#define font_impl_h_already_defined

#include <sys/types.h>
#include <sys/time.h>

#include <xview/pkg.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/font.h>


#define font_attr_next(attr) (Font_attribute *)attr_next((caddr_t *)attr)

#define	FONT_PRIVATE(font)	XV_PRIVATE(Font_info, Xv_font_struct, font)
#define	FONT_PUBLIC(font)	XV_PUBLIC(font)
#define	FONT_STANDARD(font, object)	XV_OBJECT_TO_STANDARD(XV_PUBLIC(font), \
							 "font", object)
#define	FONT_PIX(font)		(Pixfont *)XV_PUBLIC(font)

/***********************************************************************/
/*	        	Structures 				       */
/***********************************************************************/

typedef struct font_info {
    Xv_Font	 	 public_self;	/* back pointer to public struct */
    Attr_pkg		 pkg;
    Xv_opaque	 	 parent;	/* back pointer to screen */
    Xv_opaque	 	 display;
    Xv_opaque	 	 server;
    struct font_info	*next;
    
    /* family, style and point-size */
    char		*name;

    char		*foundry;
    char		*family;
    char		*style;
    /*
     * Isa
     * font weight, slant, setwidth, addstyle
     */
    char		*weight;
    char		*slant;
    char		*setwidthname;
    char		*addstylename;
    
    int			 scale;
    
    int			 size; /* for this scale */
    int			 small_size;
    int			 medium_size;
    int			 large_size;
    int			 extra_large_size;
    
    int			 ref_count;

    Font_type	 	 type;	/* text, glyph or cursor */

    /* interface to Xlib */
    long unsigned	 xid;
    Xv_opaque 		 x_font_info;

    /* flags */
    unsigned		 has_glyph_prs:1;
    unsigned		 overlapping_chars:1;
} Font_info;

/* from font.c */
Pkg_private Xv_opaque    font_set_avlist();
Pkg_private Xv_opaque    font_get_attr();
Pkg_private int 	 font_init();
Pkg_private int 	 font_destroy_struct();
Pkg_private Xv_object 	 font_find_font();

#endif
