/*	@(#)rect.h 20.8 89/08/18 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Defines the interface to the geometric object
 * called a Rect which is a rectangle.
 */

#ifndef xview_rect_DEFINED
#define xview_rect_DEFINED


/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines
 */

#define	RECT_NULL	((Rect *)0)

#ifndef coord
#define coord	short
#endif

/*
 * Rectangle sort ordering.
 */
#define	RECTS_TOPTOBOTTOM	0
#define	RECTS_BOTTOMTOTOP	1
#define	RECTS_LEFTTORIGHT	2
#define	RECTS_RIGHTTOLEFT	3

#define	RECTS_UNSORTED		-1
#define	RECTS_SORTS		4

/*
 * Rect Geometry macros
 */
#define	rect_right(rect)  	((rect)->r_left+(rect)->r_width-1)
#define	rect_bottom(rect) 	((rect)->r_top+(rect)->r_height-1)

#define rect_print(rect) 							\
        (void)fprintf(stderr,"[left: %d, top: %d, width: %d, height: %d]",	\
            (rect)->r_left, (rect)->r_top, (rect)->r_width, (rect)->r_height)

#define rect_marginadjust(r,m) 				\
	{(r)->r_left-=(m);(r)->r_top-=(m); 		\
	 (r)->r_width+=(m)+(m);(r)->r_height+=(m)+(m);}

#define rect_construct(r,x,y,w,h) \
	{(r)->r_left=(x);(r)->r_top=(y);(r)->r_width=(w);(r)->r_height=(h);}

/*
 * Rect Predicate macros
 */
#define rect_equal(r1,r2) \
	((r1)->r_left==(r2)->r_left && (r1)->r_width==(r2)->r_width && \
	 (r1)->r_top==(r2)->r_top && (r1)->r_height==(r2)->r_height)

#define rect_sizes_differ(r1, r2) \
        ((r1)->r_width != (r2)->r_width || (r1)->r_height != (r2)->r_height)

#define rect_isnull(r)		((r)->r_width == 0 || (r)->r_height == 0)

#define rect_includespoint(r,x,y) \
	((x) >= (r)->r_left && (y) >= (r)->r_top && \
	 (x)<(r)->r_left+(r)->r_width && (y)<(r)->r_top+(r)->r_height)

#define rect_includesrect(r1, r2) \
	((r1)->r_left <= (r2)->r_left && (r1)->r_top <= (r2)->r_top && \
	 (r1)->r_left+(r1)->r_width >= (r2)->r_left+(r2)->r_width && \
	 (r1)->r_top+(r1)->r_height >= (r2)->r_top+(r2)->r_height)

#define rect_intersectsrect(r1,r2) \
	((r1)->r_left<(r2)->r_left+(r2)->r_width && \
	 (r1)->r_top<(r2)->r_top+(r2)->r_height &&  \
	 (r2)->r_left<(r1)->r_left+(r1)->r_width && \
	 (r2)->r_top<(r1)->r_top+(r1)->r_height)

/*
 * Rect Transformation macros used for passing rects up/down embedded
 * coordinate systems.
 */
#define	rect_passtoparent(x,y,rect) \
	{(rect)->r_left=(rect)->r_left+(x); (rect)->r_top=(rect)->r_top+(y);}

#define	rect_passtochild(x,y,rect) \
	{(rect)->r_left=(rect)->r_left-(x); (rect)->r_top=(rect)->r_top-(y);}

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

/*
 * PUBLIC structures 
 */

typedef struct rect {
	coord	r_left, r_top;
	short	r_width, r_height;
} Rect;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PUBLIC variables 
 */
extern	struct rect 	rect_null;

/*
 * PUBLIC functions 
 */
extern	struct rect 	rect_bounding();
extern	unsigned 	rect_clipvector();
extern	unsigned 	rect_order();
extern  int 		rect_right_of();
extern	int 		rect_below();

#ifdef cplus
/*
 * C Library routine specifications related to rect functions.
 */
unsigned	rect_clipvector(struct rect *r, int *x0, *y0, *x1, *y1);
struct	rect 	rect_bounding(struct rect *r1, *r2);
void		rect_intersection(struct rect *r1, *r2, *r);
unsigned	rect_order(struct rect *r1, *r2, int sortorder);
#endif cplus

#endif ~xview_rect_DEFINED
