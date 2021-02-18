/*	@(#)rectlist.h 20.7 89/08/18 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_rectlist_DEFINED
#define xview_rectlist_DEFINED

/*
 * Defines the interface to the data structure called 
 * a rectlist which is a list of rectangles.
 *
 * A rectlist has an offset (rl_x, rl_y) assoicated with it that is used
 * to efficiently pass this data structure from one embedded coordinate
 * system to another without changing the offsets of all the rectangles in
 * the list.
 *
 * Rl_bound is the rectangle that bounds all the rectangles in the list
 * and is maintained to facilitate efficient rectangle algebra.
 */


/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */

#define	RECTNODE_NULL	((Rectnode *)0)
#define	RECTLIST_NULL	((Rectlist *)0)

/*
 * Rectlist Transformation macros used for passing rectlists up/down embedded
 * coordinate systems.
 */
#define	rl_passtoparent(x,y,rl) \
	{(rl)->rl_x=(rl)->rl_x+(x); (rl)->rl_y=(rl)->rl_y+(y);}

#define	rl_passtochild(x,y,rl) \
	{(rl)->rl_x=(rl)->rl_x-(x); (rl)->rl_y=(rl)->rl_y-(y);}

/*
 * Rectlist Offset Adjustment macros
 */
#define	rl_rectoffset(rl,r1,r) \
	{*(r)= *(r1); (r)->r_left+=(rl)->rl_x; (r)->r_top+=(rl)->rl_y;}

#define	rl_coordoffset(rl,x,y) {*(x)-=(rl)->rl_x;*(y)-=(rl)->rl_y;}


/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

/*
 * PUBLIC structures 
 */

typedef	struct	rectnode {
	struct	rectnode *rn_next;	/* Pointer to next rectnode */
	struct	rect rn_rect;
} Rectnode;


typedef	struct	rectlist {
	coord	rl_x, rl_y;		/* Offset to apply to each rect
					   in list including bound */
	struct	rectnode *rl_head;	/* Pointer to first rectnode */
	struct	rectnode *rl_tail;	/* Pointer to last rectnode */
	struct	rect rl_bound;		/* Describes bounding rect of 
					   all rects in list */
} Rectlist;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PUBLIC functions 
 */
extern	struct rectlist rl_null;
extern	unsigned 	rl_empty();
extern	unsigned	rl_equal();
extern	unsigned	rl_boundintersectsrect();
extern	unsigned	rl_rectintersects();
extern	unsigned 	rl_equalrect();
extern	unsigned	rl_includespoint();

#ifdef cplus
/*
 * C Library routine specifications related to rectlist functions.
 */

/*
 * Rectlist geometry predicates and functions
 */
unsigned	rl_empty(struct rectlist *rl);
unsigned	rl_equal(struct rectlist *rl1, *rl2);
void		rl_intersection(struct rectlist *rl1, *rl2, *rl);
void		rl_sort(struct rectlist *rl1, *rl, int sortorder);
void		rl_secondarysort(struct rectlist *rl1, *rl, int sortorder);
void		rl_union(struct rectlist *rl1, *rl2, *rl);
void		rl_difference(struct rectlist *rl1, *rl2, *rl);
void		rl_coalesce(struct rectlist rl);
unsigned	rl_includespoint(struct rectlist *rl, coord x, y);

/*
 * Rectlist with Rect geometry predicates and functions
 */
unsigned	rl_equalrect(struct rect *r, struct rectlist *rl);
unsigned	rl_boundintersectsrect(struct rect *r,  struct rectlist *rl);
unsigned	rl_rectintersects(struct rect *r,  struct rectlist *rl);
void		rl_rectintersection(struct rect *r, struct rectlist *rl1, *rl);
void		rl_rectunion(struct rect *r, struct rectlist *rl1, *rl);
void		rl_rectdifference(struct rect *r, struct rectlist *rl1, *rl);

/*
 * Rectlist initialization functions
 */
void		rl_initwithrect(struct rect *r, struct rectlist *rl);

/*
 * Rectlist List Memory Management functions
 */
void		rl_copy(struct rectlist *rl1, *rl);
void		rl_free(struct rectlist *rl);

/*
 * Rectlist Offset Adjustment functions
 */
void		rl_normalize(struct rectlist *rl);

/*
 * Debugging Utilities
 */
void		rl_print(struct rectlist *rl, char *tag);

#endif	cplus

#endif	~xview_rectlist_DEFINED
