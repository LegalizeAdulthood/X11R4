/*	@(#)icon.h 20.10 89/08/18 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_icon_DEFINED
#define xview_icon_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/window.h>
#include <xview/attrol.h>


/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */

#define ICON 			&xv_icon_pkg
#define ICON_TYPE  		ATTR_PKG_ICON

#define ICON_WIDTH		XV_WIDTH
#define ICON_HEIGHT		XV_HEIGHT
#define ICON_LABEL		XV_LABEL
#define ICON_FONT		XV_FONT

#define icon_attr_next(attr) 	(Icon_attribute *)attr_next((Xv_opaque *)attr)

#define	ICON_DEFAULT_WIDTH	64
#define	ICON_DEFAULT_HEIGHT	64

/*
 * DEFINE_ICON_FROM_IMAGE macro 
 * -- NOTE -- Currently this is commented out.
 */
/*
#ifdef i386
#define	DEFINE_ICON_FROM_IMAGE(name, image)				\
	static struct mpr_data CAT(name,_data) = {			\
	    mpr_linebytes(ICON_DEFAULT_WIDTH,1), (short *)(image),	\
	    {0, 0}, 0, MP_STATIC};					\
	extern struct pixrectops mem_ops;				\
	static struct pixrect CAT(name,_mpr) = {			\
	    &mem_ops, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT, 1,	\
	    (Xv_opaque)&CAT(name,_data)};				\
	static Xv_icon_info name = {					\
	    ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT,			\
	    (struct pixrect *)0,					\
	    0, 0, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT,		\
	    &CAT(name,_mpr),						\
	    0, 0, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT,		\
	    NULL, (struct pixfont *)0, ICON_BKGRDGRY};

#else 

#define	DEFINE_ICON_FROM_IMAGE(name, image)				\
	static struct mpr_data CAT(name,_data) = {			\
	    mpr_linebytes(ICON_DEFAULT_WIDTH,1), (short *)(image),	\
	    {0, 0}, 0, 0};						\
	extern struct pixrectops mem_ops;				\
	static struct pixrect CAT(name,_mpr) = {			\
	    &mem_ops, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT, 1,	\
	    (Xv_opaque)&CAT(name,_data)};				\
	static Xv_icon_info name = {					\
	    ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT,			\
	    (struct pixrect *)0,					\
	    0, 0, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT,		\
	    &CAT(name,_mpr),						\
	    0, 0, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT,		\
	    NULL, (struct pixfont *)0, ICON_BKGRDGRY};

#endif
*/

/*
 * PRIVATE #defines 
 */

#define ICON_ATTR(type, ordinal)       ATTR(ATTR_PKG_ICON, type, ordinal)
#define ICON_ATTR_LIST(ltype, type, ordinal) \
        ICON_ATTR(ATTR_LIST_INLINE((ltype), (type)), (ordinal))

/*
 ***********************************************************************
 *		Typedefs, enumerations, and structures
 ***********************************************************************
 */

typedef Xv_opaque      Icon;

typedef enum {
   ICON_IMAGE		= ICON_ATTR(ATTR_PIXRECT_PTR, 	5),
   ICON_IMAGE_RECT	= ICON_ATTR(ATTR_RECT_PTR, 	10),
   ICON_LABEL_RECT	= ICON_ATTR(ATTR_RECT_PTR, 	15),
} Icon_attribute;

typedef struct {
	Xv_window_struct	 parent_data;
	Xv_opaque		 private_data;
}   Xv_icon;

/*
 ***********************************************************************
 *			Globals
 ***********************************************************************
 */

/*
 * Variables 
 */
extern Xv_pkg		xv_icon_pkg;

/*
 * PUBLIC Functions 
 * For Sunview 1 Compatibility
 */

extern Icon		icon_create();
extern Xv_opaque	icon_get();
extern int		icon_set();
extern int		icon_destroy();
extern void		icon_display(); /* how are icons undisplayed? */

#endif ~xview_icon_DEFINED
