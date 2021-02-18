/*	@(#)omi_impl.h 20.20 89/08/18 		*/

/***********************************************************************/
/*	                omi_impl.h			      	       */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef _xview_image_impl_h_already_included
#define _xview_image_impl_h_already_included

#ifndef pixrect_DEFINED
#ifndef _TYPES_
#include <sys/types.h>
#endif _TYPES_
#include <pixrect/pixrect.h>
#endif pixrect_DEFINED
#include <xview_private/omi.h>

#ifndef TRUE
#define	TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif


/***********************************************************************/
/*	        	Structures 				       */
/***********************************************************************/

struct image {

    int			*ops; /* Should be ops vector or unique id */
    struct pixfont	*font;
	struct pixfont	*bold_font;	/* bold version of "font" */
    char 		*string;
    struct pixrect	*pr;
    struct pixrect	left_pr;	/* pr_depth == 0 => pr_data is olglyph index */
	struct pr_pos	left_pos;	/* position of left image (usually pushpin) */
    struct pr_prpos	right_prpos;
    short		 left_margin;
    short		 right_margin;
    short		 margin;
	
/* Flags */
    unsigned		 center:1;
    unsigned		 inactive:1;
    unsigned		 invert:1;
    unsigned		 free_everything:1;	/* Not used */
    unsigned		 free_image:1;
    unsigned		 free_font:1;		/* Not used */
    unsigned		 free_string:1;
    unsigned		 free_pr:1;
    unsigned		 free_right_pr:1;	/* Not used */

/* Auxiliary fields */
    short		 width;
    short		 height;
	struct pr_pos	button_pos;		/* menu button position */
	struct pr_size	button_size;	/* menu button size */
};


#define image_attr_next(attr) \
	(Image_attribute *)attr_next((Xv_opaque *)attr)

#define image_vector(x1,y1,x2,y2) \
	(xv_vector(window,x1,y1,x2,y2,PIX_SET,0))
#endif _xview_image_impl_h_already_included
