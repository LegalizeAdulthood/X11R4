#ifndef lint
#ifdef sccs
static char sccsid[] = "@(#)xv_color.h 1.10 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_xv_color_h_DEFINED
#define	xview_xv_color_h_DEFINED

#include <X11/Xlib.h>
#include <xview/cms.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */
#define XV_STATIC_CMAP		3
#define XV_DYNAMIC_CMAP		4
#define XV_DEFAULT_CMS  	"xv_default_cms"
#define XV_CONTROL_CMS		"xv_control_cms"
#define XV_INVALID_PIXVAL	-1

#define XV_STATIC_CMS_BG(cms) (cms)->index_table[0]
#define XV_STATIC_CMS_FG(cms) (cms)->index_table[(cms)->cms_size - 1]

#define XV_CMS_BACKGROUND(cms) \
	((cms)->cms_type == XV_STATIC_CMS) ? \
	(cms)->index_table[0] : (cms)->base_pixel
#define XV_CMS_FOREGROUND(cms) \
	((cms)->cms_type == XV_STATIC_CMS) ? \
	(cms)->index_table[(cms)->cms_size - 1] : \
	(cms)->base_pixel + (cms)->cms_size - 1

#define XV_TO_X_PIXEL(index, cms) \
    ((cms)->cms_type == XV_STATIC_CMS) ? \
    (cms)->index_table[((index) >= (cms)->cms_size) ? (cms)->cms_size - 1 : (index)] : \
    (cms)->base_pixel + (((index) >= (cms)->cms_size) ? (cms)->cms_size - 1 : (index))

#define XV_IS_DEFAULT_CMS(cms) \
	(strcmp((cms)->cms_name, XV_DEFAULT_CMS) == 0) ? TRUE : FALSE


/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */
typedef struct xv_colormap_seg {
    struct xv_colormap	    *cms_cmap;
    char		     cms_name[CMS_NAMESIZE + 1];
    enum Xv_cmstype	     cms_type;
    short		     cms_size;
    short		    *index_table;
    short		     base_pixel;
    struct xv_colormap_seg  *next;
} Xv_Colormap_Seg;

typedef struct xv_colormap {
    Colormap	    	id;
    int		    	allocated;
    short		cmap_type;
    Xv_Colormap_Seg 	*cms_list;
    struct xv_colormap  *next
} Xv_Colormap;

typedef struct xv_color_info {
    Xv_Colormap_Seg	*cms;
    short		fg;
    short		bg;
} Xv_Color_info;

#endif	~xview_xv_color_h_DEFINED
