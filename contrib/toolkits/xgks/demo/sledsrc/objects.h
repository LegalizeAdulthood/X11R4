/* 
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 */

#ifndef OBJECTS_H
#define OBJECTS_H

#include <xgks.h>

#include "changes.h"
#include "screen_items.h"

/*
 *  graphics primitives
 */


struct line_ob
{
	IDX color;			/* line color */
	Gint style;			/* line style */
	Gint width;			/* line width */
	Gint nopts;			/* no of pts */
	Gpoint *pts;			/* endpts of lines */
};

struct poly_ob
{
	Gflinter fillstyle;		/* fill style  GHOLLOW | GSOLID */
	IDX fillcolor;			/* polygon fill color */
	IDX linecolor;			/* polygon outline color */
	Gint style;			/* polygon outline style */
	int width;			/* polygon outline width */
	int nopts;			/* no of pts in polygon */
	Gpoint *pts;			/* pts of polygon */
};


/*
 *  text strings
 */

typedef struct char_ob CHAR_OB;
struct char_ob
{
	Gpoint center;			/* center of char */
	IDX color;			/* char color */
	Gfloat height;			/* char height */
	IDX fontstyle;			/* font style of char */
	Gfloat expansion;		/* related to character wd */
	Gfloat width;			/* character wd */
	Gchar ch;			/* char */
	struct char_ob *next;		/* next char in str */
};


struct text_ob
{
	Gtxpath path;			/* GTP_RIGHT | GTP_LEFT | GTP_UP | GTP_DOWN */
	Gtxver vert_just;		/* GTV_CAP | GTV_HALF | GTV_BASE */
	Gtxhor horz_just;		/* GTH_LEFT | GTH_CENTER | GTH_RIGHT */
	Gpoint pos;			/* text string position */
	Gpoint up_vec;			/* up direction */
	struct char_ob *str;		/* char string */
};


/*
 *  unit objects
 */


typedef struct object OBJECT;

typedef struct comb_ob COMB_OB;

#define LINEOB		"line object"
#define BOXOB		"box object"
#define POLYOB		"polygon object"
#define CIRCLEOB	"circle object"
#define CONICOB		"conic object"
#define CURVEOB		"curve object"
#define TEXTOB		"text object"

#define lineob obtype.line_object
#define boxob obtype.poly_object
#define polyob obtype.poly_object
#define circleob obtype.poly_object
#define conicob obtype.poly_object
#define curveob obtype.line_object
#define textob obtype.text_object


struct object
{
	char *key;			/* associated functions */
	Gsegvis visibility;		/* GVISIBLE | GINVISIBLE */
	Gfloat trans[6];		/* transformation */
	COMB_OB *parent;		/* parent */
	OBJECT *next;			/* next OBJECT in COMB_OBJ */
	union 				/* object type specific info */
	{
		struct line_ob line_object;
		struct poly_ob poly_object;
		struct text_ob text_object;
	} obtype;
};


/*
 *  combination object
 */


struct comb_ob
{
	Gsegvis visibility;		/* GVISIBLE | GINVISIBLE */
	COMB_OB *next;			/* next COMB_OB in order of
					   display priority */
	COMB_OB *prev;			/* previous COMB_OB in order
					   of display priority */
	OBJECT *child;			/* list of OBJECTS in COMB_OB */
};

typedef struct comb_ob_ptr COMB_OB_PTR;
struct comb_ob_ptr
{
	COMB_OB *comb_ob;
	OBJECT *object;
	COMB_OB_PTR *next;
};

extern COMB_OB *alloc_comb_ob(void);
extern OBJECT *alloc_single_ob( KEY key);
extern COMB_OB *copy_comb_ob( COMB_OB *comb_ob);
extern OBJECT * copy_object( OBJECT *object);

extern COMB_OB *tail;
extern COMB_OB *head;

#define	HEAD	0
#define	TAIL	1

#endif
