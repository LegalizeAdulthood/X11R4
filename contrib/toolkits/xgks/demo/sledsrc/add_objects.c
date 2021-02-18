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
 *  this file includes:
 *	add_objects_init
 *	add_line_init
 *	add_line_restart
 *	add_line_exec
 *	add_line_cleanup
 *	add_curve_init
 *	add_curve_restart
 *	add_curve_exec
 *	add_curve_cleanup
 *	add_box_init
 *	add_box_restart
 *	add_box_exec
 *	add_box_cleanup
 *	add_circle_init
 *	add_circle_restart
 *	add_circle_exec
 *	add_circle_cleanup
 *	add_conic_init
 *	add_conic_restart
 *	add_conic_exec
 *	add_conic_cleanup
 *	add_text_init
 *	add_text_restart
 *	add_text_exec
 *	add_text_cleanup
 *	interrupt_handler
 *	set_cbreak_mode
 *	add_objects_undo
 *	init_line_width_popup
 *	switch_line_width_popup
 *	get_line_width
 *	init_line_connected_popup
 *	switch_line_connected_state
 *	get_line_connected
 *	init_line_style_state
 *	switch_line_style_state
 *	get_line_style
 *	switch_curve_connected
 *	init_fill_style_popup
 *	switch_fill_style_popup
 *	get_fill_style_popup
 *	init_char_width_popup
 *	switch_char_width_popup
 *	get_char_expansion_popup
 *	get_char_width_popup
 *	init_char_height_popup
 *	switch_char_height_popup
 *	get_char_height_popup
 *	init_font_style_popup
 *	switch_font_style_popup
 *	get_font_style
 *	init_text_path_popup
 *	switch_text_path_popup
 *	get_text_path
 *	init_horz_just_popup
 *	switch_horz_just_popup
 *	get_horz_just
 *	init_vert_just_popup
 *	switch_vert_just_popup
 *	get_vert_just
 */

static char rcsid[] = "$Header: add_objects.c,v 4.0 89/09/01 15:52:04 amy Exp $";

#include <math.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include "color.h"
#include "functbl.h"
#include "screen_items.h"
#include "key.h"
#include "defs.h"
#include "draw.h"
#include "objects.h"
#include "trans.h"
#include "ws.h"
#include "add_obj_func.h"
#include "object_defs.h"
#include "object_list.h"
#include "popup.h"
#include <xgks.h>
#include "changes.h"

#define	ADD_OBJECTS_UNDO	"add objects undo menu item"
#define	INTERRUPT		'@' 
#define	BACKSPACE		''
#define	END_OF_STR		'\n'

extern char *calloc();

static int pt_needed;		/* FIRST_PT or SECOND_PT */ 
static Gpoint first_pt;		/* first point of line */
static Gpoint *previous_end_pt;	/* previous line endpoint
				   used by add_line menu_item */
static Gpoint control[4];	/* control points
				   used by add_curve menu item */
static Gpoint prev_control[4];	/* previous control points
				   used by add_curve menu item */
static Gpoint past_control[4];	/* 2nd to last set of control points
				   used by add_curve menu item */
static OBJECT *object;		/* current simple line object */
static COMB_OB *comb_ob;	/* current combination
				   line object */
static BOOLEAN can_undo;	/* TRUE if there is something to undo */
static MENU_ITEM *mitem_in_progress;	/* menu item in progress
				   when undo is picked */
static CHAR_OB *curr_char;	/* most recent character read */
static CHAR_OB *head_char;	/* first character in string */
static BOOLEAN is_pt_needed;	/* true if location of next
				   string is needed */
static char ch;			/* char just read in */
Gfloat *char_wd_tbl;
Gfloat *char_ht_tbl;


/*
 *  add_objects_init
 *
 *  description:	initalizes menu, popups and undo variables
 *
 *  parameters:		menu_item
 */

add_objects_init(menu_item)
MENU_ITEM *menu_item;
{
	display_child_menu(menu_item);
	can_undo = FALSE;
	mitem_in_progress = (MENU_ITEM *) NULL;
}  /* add_objects_init */


/*
 *  add_line_init
 *
 *  description:	initializes line menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - line menu item
 */

add_line_init(menu_item)
MENU_ITEM *menu_item;
{
	set_currmitem(menu_item);
	hilite(menu_item->key);
	display_popups(menu_item->poplst);
	comb_ob = (COMB_OB *) NULL;
	object = (OBJECT *) NULL;
	pt_needed = FIRST_PT;
	previous_end_pt = (Gpoint *) NULL;
	push_curr_trans();
	activate(PICT_AREA);
	prompt(4);
}  /* end add_line_init */


/*
 *  add_line_restart
 *
 *  description:	reinitializes line menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - line menu item
 */

add_line_restart(menu_item)
MENU_ITEM *menu_item;
{
	if (eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		set_currmitem(menu_item);
		if (get_line_connected() == CONNECTED)
		{
			/* last object just erased */

			if ((object->visibility == GINVISIBLE) &&
				(previous_end_pt != (Gpoint *) NULL))
			{
				first_pt = *previous_end_pt;
				pt_needed = SECOND_PT;
				prompt(5);
			}
			else if (object->visibility == GINVISIBLE)
			{
				pt_needed = FIRST_PT;
				prompt(4);
			}
			else  /* last object just un-erased */
			{
				first_pt = object->lineob.pts[1];
				pt_needed = SECOND_PT;
				prompt(5);
			}
		}
		else  /* line_connected_state == NOT_CONNECTED */
		{
			pt_needed = FIRST_PT;
			prompt(4);
		}
	}
	else  /* undo not picked */
	{
		comb_ob = (COMB_OB *) NULL;
		object = (OBJECT *) NULL;
		pt_needed = FIRST_PT;
		previous_end_pt = (Gpoint *) NULL;
		prompt(4);
	}
}  /* end add_line_restart */


/*
 *  add_line_exec
 *
 *  description:	puts each line in simple object
 *			and combines all simple objects in
 *			a single combination object.
 *
 *  parameters:		pt (Gpoint) - pt picked
 */

add_line_exec(pt)
Gpoint pt;
{
	if (pt_needed == FIRST_PT)
	{
		first_pt = pt;
		pt_needed = SECOND_PT;
		prompt(5);
	}
	else /* pt_needed == SECOND_PT */
	{
		/* if first line is being drawn, allocate comb_ob
	   	   also note that there is now something to undo */
   
  	 	if (object == (OBJECT *) NULL)
		{
			can_undo = TRUE;
			comb_ob = alloc_comb_ob();
			enq_comb_ob(comb_ob);
		}

		/* else if line is not connected to previous line
		   allocate comb_ob */
	
		else if (!eq_pts(first_pt,object->lineob.pts[1]))
		{
			comb_ob = alloc_comb_ob();
			enq_comb_ob(comb_ob);
		}

		/* otherwise, add to same combination object */

		/* note endpoint of last line segment drawn */

		if (object != (OBJECT *) NULL)
		{
			previous_end_pt = &(object->lineob.pts[1]);
		}

		object = alloc_single_ob(LINEOB);
		object->lineob.nopts = 2;
		object->lineob.pts = (Gpoint *) calloc ((unsigned) 2,
			sizeof(Gpoint));
		object->lineob.pts[0] = first_pt;
		object->lineob.pts[1] = pt;
		object->lineob.style = get_line_style();
		object->lineob.width = get_line_width();
		object->lineob.color = get_fgdclr();
		ident(object->trans);
		add_to_comb_ob(comb_ob,object,TAIL);
		draw_line(object);

		if (get_line_connected() == CONNECTED)
		{
			first_pt = pt;
			prompt(5);
		}
		else
		{
			pt_needed = FIRST_PT;
			prompt(4);
		}
	}
}  /* end add_line_exec */


/*
 *  add_line_cleanup
 *
 *  description:	cleans up line menu item
 *
 *  parameters:		MENU_ITEM * menu_item
 */

add_line_cleanup(menu_item)
MENU_ITEM *menu_item;
{
	if (!eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		erase_popups(menu_item->poplst);
		pop_curr_trans();
		mitem_in_progress = (MENU_ITEM *) NULL;
		unhilite(menu_item->key);
	}
	else
	{
		mitem_in_progress = menu_item;
	}
	set_currmitem((MENU_ITEM *) NULL);
}  /* end add_line_cleanup */


/*
 *  add_curve_init
 *
 *  description:	initializes curve menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - curve menu item
 */

add_curve_init(menu_item)
MENU_ITEM *menu_item;
{
	int i;

	set_currmitem(menu_item);
	hilite(menu_item->key);
	display_popups(menu_item->poplst);
	comb_ob = (COMB_OB *) NULL;
	object = (OBJECT *) NULL;
	pt_needed = FIRST_PT;
	for (i=0; i<4; i++)
	{
		past_control[i].x = -1.0;
		past_control[i].y = -1.0;
		prev_control[i].x = -1.0;
		prev_control[i].y = -1.0;
	}
	push_curr_trans();
	activate(PICT_AREA);
	prompt(36);
}  /* end add_curve_init */


/*
 *  add_curve_restart
 *
 *  description:	reinitializes curve menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - curve menu item
 */

add_curve_restart(menu_item)
MENU_ITEM *menu_item;
{
	int i;

	if (eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		set_currmitem(menu_item);
		if (get_line_connected() == CONNECTED)
		{
			/* last object just erased */

			if ((object->visibility == GINVISIBLE) &&
				(prev_control[0].x != -1.0))
			{
				control[0] = past_control[3];
				control[1].x = control[0].x +
					(control[0].x -
					past_control[2].x);
				control[1].y = control[0].y +
					(control[0].y -
					past_control[2].y);

				pt_needed = THIRD_PT;
				prompt(38);
			}
			else if (object->visibility == GINVISIBLE)
			{
				pt_needed = FIRST_PT;
				prompt(36);
			}
			else  /* last object just un-erased */
			{
				pt_needed = THIRD_PT;
				control[0] = prev_control[3];
				control[1].x = control[0].x +
					(control[0].x -
					prev_control[2].x);
				control[1].y = control[0].y +
					(control[0].y -
					prev_control[2].y);
				prompt(38);
			}
		}
		else  /* line_connected_state == NOT_CONNECTED */
		{
			pt_needed = FIRST_PT;
			prompt(36);
		}
	}
	else  /* undo not picked */
	{
		comb_ob = (COMB_OB *) NULL;
		object = (OBJECT *) NULL;
		pt_needed = FIRST_PT;
		for (i=0; i<4; i++)
		{
			prev_control[i].x = -1.0;
			prev_control[i].y = -1.0;
			past_control[i].x = -1.0;
			past_control[i].y = -1.0;
		}
		prompt(36);
	}
}  /* end add_curve_restart */


/*
 *  add_curve_exec
 *
 *  description:	puts each curve in simple object
 *			and combines all simple objects in
 *			a single combination object.
 *
 *  parameters:		pt (Gpoint) - pt picked
 */

#define	NUM_CURVE_PTS 100

add_curve_exec(pt)
Gpoint pt;
{
	int i;

	if (pt_needed == FIRST_PT)
	{
		control[0] = pt;
		pt_needed = SECOND_PT;
		prompt(37);
	}
	else if (pt_needed == SECOND_PT)
	{
		control[1] = pt;
		pt_needed = THIRD_PT;
		prompt(38);
	}
	else if (pt_needed == THIRD_PT)
	{
		control[2] = pt;
		pt_needed = FOURTH_PT;
		prompt(39);
	}
	else /* pt_needed == FOURTH_PT */
	{
		control[3] = pt;

		/* if first curve is being drawn, allocate comb_ob
	   	   also note that there is now something to undo */
   
  	 	if (object == (OBJECT *) NULL)
		{
			can_undo = TRUE;
			comb_ob = alloc_comb_ob();
			enq_comb_ob(comb_ob);
		}

		/* else if curve is not connected to previous curve
		   allocate comb_ob */
	
		else if (!eq_pts(first_pt,object->curveob.pts[NUM_CURVE_PTS - 1]))
		{
			comb_ob = alloc_comb_ob();
			enq_comb_ob(comb_ob);
		}

		/* otherwise, add to same combination object */

		/* note control points of last curve segment drawn */

		if (object != (OBJECT *) NULL)
		{
			for (i=0; i<4; i++)
			{
				past_control[i] = prev_control[i];
				prev_control[i] = control[i];
			}
		}

		object = alloc_single_ob(CURVEOB);
		object->curveob.nopts = NUM_CURVE_PTS;
		object->curveob.pts = (Gpoint *) calloc ((unsigned)
			NUM_CURVE_PTS, sizeof(Gpoint));
		curve(control,NUM_CURVE_PTS,object->curveob.pts);
		object->curveob.style = get_line_style();
		object->curveob.width = get_line_width();
		object->curveob.color = get_fgdclr();
		ident(object->trans);
		add_to_comb_ob(comb_ob,object,TAIL);
		draw_line(object);

		if (get_line_connected() == CONNECTED)
		{
			pt_needed = THIRD_PT;
			control[0] = control[3];
			control[1].x = control[0].x * 2
				- control[2].x;
			control[1].y = control[0].y * 2
				- control[2].y;
			prompt(38);
		}
		else
		{
			pt_needed = FIRST_PT;
			prompt(36);
		}
	}
}  /* end add_curve_exec */


/*
 *  add_curve_cleanup
 *
 *  description:	cleans up curve menu item
 *
 *  parameters:		MENU_ITEM * menu_item
 */

add_curve_cleanup(menu_item)
MENU_ITEM *menu_item;
{
	if (!eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		erase_popups(menu_item->poplst);
		pop_curr_trans();
		mitem_in_progress = (MENU_ITEM *) NULL;
		unhilite(menu_item->key);
	}
	else
	{
		mitem_in_progress = menu_item;
	}
	set_currmitem((MENU_ITEM *) NULL);
}  /* end add_curve_cleanup */


/*
 *  add_box_init
 *
 *  description:	initializes box menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - box menu item
 */

add_box_init(menu_item)
MENU_ITEM *menu_item;
{
	set_currmitem(menu_item);
	hilite(menu_item->key);
	display_popups(menu_item->poplst);
	comb_ob = (COMB_OB *) NULL;
	object = (OBJECT *) NULL;
	pt_needed = FIRST_PT;
	push_curr_trans();
	activate(PICT_AREA);
	prompt(4);
}  /* end add_box_init */


/*
 *  add_box_restart
 *
 *  description:	initializes box menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - box menu item
 */

add_box_restart(menu_item)
MENU_ITEM *menu_item;
{
	if (eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		set_currmitem(menu_item);
	}
	else
	{
		comb_ob = (COMB_OB *) NULL;
		object = (OBJECT *) NULL;
	}
	pt_needed = FIRST_PT;
	prompt(4);
}  /* end add_box_restart */


/*
 *  add_box_exec
 *
 *  description:	puts each box in single comb_ob
 *
 *  parameters:		pt (Gpoint ) - pt picked
 */

add_box_exec(pt)
Gpoint pt;
{
	Gfloat xmin,xmax,ymin,ymax;

	if (pt_needed == FIRST_PT)
	{
		first_pt = pt;
		pt_needed = SECOND_PT;
		prompt(5);
	}
	else  /* pt_needed == SECOND_PT */
	{
		comb_ob = alloc_comb_ob();
		enq_comb_ob(comb_ob);
		object = alloc_single_ob(BOXOB);
		object->boxob.nopts = 5;
		object->boxob.pts = (Gpoint *) calloc (5,
			sizeof(Gpoint));
		xmin = min2(first_pt.x,pt.x);
		ymin = min2(first_pt.y,pt.y);
		xmax = max2(first_pt.x,pt.x);
		ymax = max2(first_pt.y,pt.y);
		load_box(object->boxob.pts,xmin,xmax,ymin,ymax);
		object->boxob.pts[4] = object->boxob.pts[0];
		object->boxob.fillstyle = get_fill_style();
		if (object->boxob.fillstyle == GSOLID)
			object->boxob.fillcolor = get_fillclr();
		object->boxob.style = get_line_style();
		object->boxob.width = get_line_width();
		object->boxob.linecolor = get_fgdclr();
		ident(object->trans);
		add_to_comb_ob(comb_ob,object,TAIL);
		draw_poly(object);
		can_undo = TRUE;
		pt_needed = FIRST_PT;
		prompt(4);
	}
}  /* end add_box_exec */


/*
 *  add_box_cleanup
 *
 *  description:	cleans up box menu item
 *
 *  parameters:		MENU_ITEM * menu_item
 */

add_box_cleanup(menu_item)
MENU_ITEM *menu_item;
{
	if (!eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		erase_popups(menu_item->poplst);
		pop_curr_trans();
		mitem_in_progress = (MENU_ITEM *) NULL;
		unhilite(menu_item->key);
	}
	else
	{
		mitem_in_progress = menu_item;
	}
	set_currmitem((MENU_ITEM *) NULL);
}  /* end add_box_cleanup */


/*
 *  add_circle_init
 *
 *  description:	initializes circle menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - circle menu item
*/

add_circle_init(menu_item)
MENU_ITEM *menu_item;
{
	set_currmitem(menu_item);
	hilite(menu_item->key);
	display_popups(menu_item->poplst);
	comb_ob = (COMB_OB *) NULL;
	object = (OBJECT *) NULL;
	pt_needed = FIRST_PT;
	push_curr_trans();
	activate(PICT_AREA);
	prompt(8);
}  /* end add_circle_init */


/*
 *  add_circle_restart
 *
 *  description:	initializes circle menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - circle menu item
 */

add_circle_restart(menu_item)
MENU_ITEM *menu_item;
{
	if (eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		set_currmitem(menu_item);
	}
	else
	{
		comb_ob = (COMB_OB *) NULL;
		object = (OBJECT *) NULL;
	}
	pt_needed = FIRST_PT;
	prompt(8);
}  /* end add_circle_restart */


/*
 *  add_circle_exec
 *
 *  description:	puts each circle in single comb_ob
 *
 *  parameters:		pt (Gpoint ) - pt picked
 */

add_circle_exec(pt)
Gpoint pt;
{
	if (pt_needed == FIRST_PT)
	{
		first_pt = pt;
		pt_needed = SECOND_PT;
		prompt(9);
	}
	else  /* pt_needed == SECOND_PT */
	{
		comb_ob = alloc_comb_ob();
		enq_comb_ob(comb_ob);
		object = alloc_single_ob(CIRCLEOB);
		object->circleob.nopts = NO_CONIC_LNS;
		object->circleob.pts = (Gpoint *) calloc ((unsigned)
			NO_CONIC_LNS, sizeof(Gpoint));
		circle(first_pt,(dist(first_pt,pt)), NO_CONIC_LNS,
			object->circleob.pts);
		object->circleob.fillstyle = get_fill_style();
		if (object->circleob.fillstyle == GSOLID)
			object->circleob.fillcolor = get_fillclr();
		object->circleob.style = get_line_style();
		object->circleob.width = get_line_width();
		object->circleob.linecolor = get_fgdclr();
		ident(object->trans);
		add_to_comb_ob(comb_ob,object,TAIL);
		draw_poly(object);
		can_undo = TRUE;
		pt_needed = FIRST_PT;
		prompt(8);
	}
}  /* end add_circle_exec */


/*
 *  add_circle_cleanup
 *
 *  description:	cleans up circle menu item
 *
 *  parameters:		MENU_ITEM * menu_item
 */

add_circle_cleanup(menu_item)
MENU_ITEM *menu_item;
{
	if (!eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		erase_popups(menu_item->poplst);
		pop_curr_trans();
		mitem_in_progress = (MENU_ITEM *) NULL;
		unhilite(menu_item->key);
	}
	else
	{
		mitem_in_progress = menu_item;
	}
	set_currmitem((MENU_ITEM *) NULL);
}  /* end add_circle_cleanup */


/*
 *  add_conic_init
 *
 *  description:	initializes conic menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - conic menu item
 */

add_conic_init(menu_item)
MENU_ITEM *menu_item;
{
	set_currmitem(menu_item);
	hilite(menu_item->key);
	display_popups(menu_item->poplst);
	comb_ob = (COMB_OB *) NULL;
	object = (OBJECT *) NULL;
	pt_needed = FIRST_PT;
	push_curr_trans();
	activate(PICT_AREA);
	prompt(10);
}  /* end add_conic_init */


/*
 *  add_conic_restart
 *
 *  description:	initializes conic menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - conic menu item
 */

add_conic_restart(menu_item)
MENU_ITEM *menu_item;
{
	if (eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		set_currmitem(menu_item);
	}
	else
	{
		comb_ob = (COMB_OB *) NULL;
		object = (OBJECT *) NULL;
	}
	pt_needed = FIRST_PT;
	prompt(10);
}  /* end add_conic_restart */


/*
 *  add_conic_exec
 *
 *  description:	puts each conic in single comb_ob
 *
 *  parameters:		pt (Gpoint ) - pt picked
 */

add_conic_exec(pt)
Gpoint pt;
{
	Gpoint center_pt;

	if (pt_needed == FIRST_PT)
	{
		first_pt = pt;
		pt_needed = SECOND_PT;
		prompt(11);
	}
	else  /* pt_needed == SECOND_PT */
	{
		comb_ob = alloc_comb_ob();
		enq_comb_ob(comb_ob);
		object = alloc_single_ob(CONICOB);
		object->conicob.nopts = NO_CONIC_LNS;
		object->conicob.pts = (Gpoint *) calloc ((unsigned)
			NO_CONIC_LNS, sizeof(Gpoint));
		center_pt.x = (first_pt.x + pt.x) / 2.0;
		center_pt.y = (first_pt.y + pt.y) / 2.0;
		conic(center_pt, abs(first_pt.x - pt.x),
			abs(first_pt.y - pt.y), NO_CONIC_LNS,
			object->conicob.pts);
		object->conicob.fillstyle = get_fill_style();
		if (object->conicob.fillstyle == GSOLID)
			object->conicob.fillcolor = get_fillclr();
		object->conicob.style = get_line_style();
		object->conicob.width = get_line_width();
		object->conicob.linecolor = get_fgdclr();
		ident(object->trans);
		add_to_comb_ob(comb_ob,object,TAIL);
		draw_poly(object);
		can_undo = TRUE;
		pt_needed = FIRST_PT;
		prompt(10);
	}
}  /* end add_conic_exec */


/*
 *  add_conic_cleanup
 *
 *  description:	cleans up conic menu item
 *
 *  parameters:		MENU_ITEM * menu_item
 */

add_conic_cleanup(menu_item)
MENU_ITEM *menu_item;
{
	if (!eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		erase_popups(menu_item->poplst);
		pop_curr_trans();
		mitem_in_progress = (MENU_ITEM *) NULL;
		unhilite(menu_item->key);
	}
	else
	{
		mitem_in_progress = menu_item;
	}
	set_currmitem((MENU_ITEM *) NULL);
}  /* end add_conic_cleanup */


/*
 *  add_text_init
 *
 *  description:	initializes text menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - text menu item
 */

add_text_init(menu_item)
MENU_ITEM *menu_item;
{
	set_currmitem(menu_item);
	hilite(menu_item->key);
	display_popups(menu_item->poplst);
	comb_ob = (COMB_OB *) NULL;
	object = (OBJECT *) NULL;
	is_pt_needed = TRUE;
	ch = (char) NULL;
	head_char = (CHAR_OB *) NULL;
	curr_char = (CHAR_OB *) NULL;
	push_curr_trans();
	activate(PICT_AREA);
	prompt(12);
}  /* end add_text_init */


/*
 *  add_text_restart
 *
 *  description:	reinitializes text menu item
 *
 *  parameters:		menu_item (MENU_ITEM *) - text menu item
 */

add_text_restart(menu_item)
MENU_ITEM *menu_item;
{
	CHAR_OB *temp_char, *temp2_char;

	if (eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		set_currmitem(menu_item);
	}
	else  /* undo not picked */
	{
		comb_ob = (COMB_OB *) NULL;
		object = (OBJECT *) NULL;
	}
	is_pt_needed = TRUE;
	ch = (char) NULL;
	prompt(12);
	temp_char = head_char;
	while (temp_char != (CHAR_OB *) NULL)
	{
		temp2_char = temp_char->next;
		free((char *) temp_char);
		temp_char = temp2_char;
	}
	head_char = (CHAR_OB *) NULL;
	curr_char = (CHAR_OB *) NULL;
}  /* end add_text_restart */


/*
 *  add_text_exec
 *
 *  description:	puts each text in simple object
 *			and combines all simple objects in
 *			a single combination object.
 *
 *  parameters:		pt (Gpoint) - pt picked
 */

add_text_exec(pt)
Gpoint pt;
{
	CHAR_OB *ch_ptr;
	extern Gfloat get_char_width();

	if (is_pt_needed == TRUE)
	{
		first_pt = pt;
		is_pt_needed = FALSE;
		prompt(13);
	}

	/* pt has been picked - just waiting for remainder
   	   of text string */

	printf("Enter text:\n");
	printf("(BACKSPACE is '^H'; ");
	/* printf(" Type '%c' to get back locator\n",INTERRUPT); */
	printf(" Type RETURN at end of string.)\n");

	if (head_char != (CHAR_OB *) NULL)
	{
		printf("Character string thus far is:\n'");
		for (ch_ptr = head_char; ch_ptr != (CHAR_OB *) NULL;
			ch_ptr = ch_ptr->next)
		{
			printf("%c",ch_ptr->ch);
		}
		printf("'\n");
	}

	set_cbreak_mode(ON);
	ch = (char) getchar();
	while ((ch != INTERRUPT) && (ch != END_OF_STR))
	{
		if (ch == BACKSPACE)
		{
			if (head_char == (CHAR_OB *) NULL)
			{
				ch = (char) getchar();
				continue;
			}

			if (head_char == curr_char)
			{
				free((char *) curr_char);
				head_char = (CHAR_OB *) NULL;
				ch = (char) getchar();
				continue;
			}

			for (ch_ptr = head_char;
				ch_ptr->next != curr_char;
				ch_ptr = ch_ptr->next);

			free((char *) curr_char);
			curr_char = ch_ptr;
			curr_char->next = (CHAR_OB *) NULL;
			ch = (char) getchar();
			continue;
		}

		if (head_char == (CHAR_OB *) NULL)
		{
			head_char = curr_char = (CHAR_OB *)
				calloc(1,sizeof(CHAR_OB));
		}
		else
		{
			curr_char->next = (CHAR_OB *)
				calloc(1,sizeof(CHAR_OB));
			curr_char = curr_char->next;
		}

		curr_char->ch = ch;
		curr_char->color = get_fgdclr();
		curr_char->height = get_char_height();
		curr_char->fontstyle = get_font_style();
		curr_char->expansion = get_char_expansion();
		curr_char->width = get_char_width(curr_char);
		curr_char->next = (CHAR_OB *) NULL;

		ch = (char) getchar();
	}  /* end while */

	set_cbreak_mode(OFF);

	if ((ch == END_OF_STR) && 
		(head_char == (CHAR_OB *) NULL))
		prompt(12);
	else if (ch == END_OF_STR)
	{
		/* if first text string is being drawn, 
	      	   note that there is now something to undo */

		if (object == (OBJECT *) NULL)
			can_undo = TRUE;

		comb_ob = alloc_comb_ob();
		enq_comb_ob(comb_ob);
	
		object = alloc_single_ob(TEXTOB);
		object->textob.pos = first_pt;
		object->textob.vert_just = get_vert_just();
		object->textob.horz_just = get_horz_just();
		object->textob.path = get_text_path();
		object->textob.up_vec.x = 0.0;
		object->textob.up_vec.y = 1.0;
		ident(object->trans);
		add_to_comb_ob(comb_ob,object,TAIL);
		object->textob.str = head_char;
		draw_text(object);

		is_pt_needed = TRUE;
		head_char = (CHAR_OB *) NULL;
		curr_char = (CHAR_OB *) NULL;
		prompt(12);
	}  /* end if */
	else
		prompt(13);
}  /* end add_text_exec */


/*
 *  add_text_cleanup
 *
 *  description:	cleans up text menu item
 *
 *  parameters:		MENU_ITEM * menu_item
 */

add_text_cleanup(menu_item)
MENU_ITEM *menu_item;
{
	CHAR_OB *temp_char, *temp2_char;

	if (!eq(get_newmitem(),ADD_OBJECTS_UNDO))
	{
		erase_popups(menu_item->poplst);
		pop_curr_trans();
		mitem_in_progress = (MENU_ITEM *) NULL;
	
		/* free partial string */

		temp_char = head_char;
		while (temp_char != (CHAR_OB *) NULL)
		{
			temp2_char = temp_char->next;
			free((char *) temp_char);
			temp_char = temp2_char;
		}
		unhilite(menu_item->key);
	}
	else
	{
		mitem_in_progress = menu_item;
	}
	set_currmitem((MENU_ITEM *) NULL);
}  /* end add_text_cleanup */


/*
 *  set_cbreak_mode
 *
 *  description:	sets terminal in cbreak mode and also
 *			sets terminal to catch  (i.e. INTERRUPT
 *			char)
 *
 *  parameters:		state (int) - ON | OFF
 */

set_cbreak_mode(state)
int state;
{
	struct sgttyb arg;

	ioctl(0,TIOCGETP,&arg);
	if (state == ON)
	{
		arg.sg_flags = arg.sg_flags | CBREAK;
	}
	else
	{
		arg.sg_flags = arg.sg_flags & ~ CBREAK;
	}
	ioctl(0,TIOCSETP,&arg);
}  /* end set_cbreak_mode */


/*
 *  add_objects_undo
 *
 *  description:	undoes last line segment, conic, box circle
 *			or text string on screen
 *
 *  parameters:		menu_item (MENU_ITEM *) - undo menu item
 */

add_objects_undo(menu_item)
MENU_ITEM *menu_item;
{
	OBJECT *object;

	if (can_undo == TRUE)
	{
		hilite(menu_item->key);
		object = get_last_object();
		if (object->visibility == GVISIBLE)
		{
			object->visibility = GINVISIBLE;
			erase_object(object);
			if (get_update_mode() != QUICK_UPDATE)
			{
				redraw_objects();
			}
		}
		else  /* object == GINVISIBLE */
		{
			object->visibility = GVISIBLE;
			draw_object(object);
		}
		unhilite(menu_item->key);
	}

	/* restart menu item which was in progress when undo was
	   picked */

	if (mitem_in_progress != (MENU_ITEM *) NULL)
	{
		menu_item_func_tbl[find_menu_item_func_tbl_idx(
			mitem_in_progress->key)].restart(
			mitem_in_progress);
	}
}  /* end add_objects_undo */



/* popup routines */

static Gint line_width = 1;
static Gint min_line_width = 1;
static Gint max_line_width = 99;

/*
 *  init_line_width_popup
 *
 *  description:	draws line width popup on screen
 *
 *  parameters:		popup (POPUP *) - line width popup
 */

init_line_width_popup(popup)
POPUP *popup;
{
	display_number_popup(popup,"WIDTH",line_width);
}  /* end init_line_width popup */


/*
 *  switch_line_width_popup
 *
 *  description:	updates line width
 *
 *  parameters:		popup (POPUP *) - line width popup
 */

switch_line_width_popup(popup,pt)
POPUP *popup;
Gpoint pt;
{
	switch_number_popup_state(popup->extent,&line_width,pt,
		min_line_width,max_line_width);
}  /* end switch_line_width popup */


/*
 *  get_line_width
 *
 *  description:	returns current value of line width
 *
 *  parameters:		none
 *
 *  returns:		(int) - line width
 */


Gfloat get_line_width()
{
	Glnfac attr;
	Gfloat val;

	ginqlinefacil(ws_type,&attr);

	if (min_line_width != max_line_width)
		val = attr.min + (attr.max - attr.min)
			* (Gfloat) (line_width - min_line_width) / 
			(Gfloat) (max_line_width - min_line_width);
	else
		val = attr.min;
	val = val / LINE_FAC;
	return(val);
}


static int line_connected_state = CONNECTED;

static char *line_connected_text[] =
{
	"CONNEC-",
	"TED",
	"LINES",
	"UNCON-",
	"NECTED",
	"LINES"
};


/*
 *  init_line_connected_popup
 *
 *  description:	displays popup box and text
 *
 *  parameters:		popup (POPUP *) - line_connected popup
 */

init_line_connected_popup(popup)
POPUP *popup;
{
	char **ln_ptr;
	int i;

	ln_ptr = line_connected_text;
	for (i=0; i<(3 * line_connected_state); i++)
		ln_ptr++;
	display_popup(popup,3,ln_ptr);

}  /* end init_line_connected_popup */


/*
 *  switch_line_connected_state
 *
 *  description:	advances popup state to next state and
 *			redraws popup
 *
 *  parameters:		popup (POPUP *) - line_connected popup
 *			pt (Gpoint) - unused
 */

switch_line_connected_state(popup,pt)
POPUP *popup;
Gpoint pt;
{

	switch_popup_state(&line_connected_state,2,3,
		line_connected_text,popup->extent);

	switch (line_connected_state)
	{
		case CONNECTED:		if ((pt_needed == FIRST_PT) &&
						(object != (OBJECT *) NULL))
					{
						if (object->visibility == GVISIBLE)
							add_line_exec(object->lineob.pts[1]);
						else if (previous_end_pt != (Gpoint *) NULL)
							add_line_exec(*previous_end_pt);
					}
					break;

		case NOT_CONNECTED:	if (pt_needed == SECOND_PT)
					{
						pt_needed = FIRST_PT;
						prompt(4);
					}
					break;
	}

}  /* end switch_line_connected_state */


/*
 *  get_line_connected
 *
 *  description:	finds current value of line_connected_state
 *
 *  parameters:		none
 *
 *  returns:		(int) - line_connected_state
 */

get_line_connected()
{
	return(line_connected_state);
}  /* end get_line_connected */


static int line_style_state = SOLID_LN;

static char *line_style_text[] =
{
	"SOLID",
	"LINE",
	"DASHED",
	"LINE",
	"DASH_DOT",
	"LINE",
	"DOTTED",
	"LINE"
};


/*
 *  init_line_style_popup
 *
 *  description:	displays popup box and text
 *
 *  parameters:		popup (POPUP *) - line_style popup
 */

init_line_style_popup(popup)
POPUP *popup;
{
	char **ln_ptr;
	int i;

	ln_ptr = line_style_text;
	for (i=0; i<(2 * line_style_state); i++)
		ln_ptr++;
	display_popup(popup,2,ln_ptr);

}  /* end init_line_style_popup */


/*
 *  switch_line_style_state
 *
 *  description:	advances popup state to next state and
 *			redraws popup
 *
 *  parameters:		popup (POPUP *) - line_style popup
 *			pt (Gpoint) - unused
 */

switch_line_style_state(popup,pt)
POPUP *popup;
Gpoint pt;
{

	switch_popup_state(&line_style_state,4,2,line_style_text,popup->extent);

}  /* end switch_line_style_state */


/*
 *  get_line_style
 *
 *  description:	finds current value of line_style_state
 *
 *  parameters:		none
 *
 *  returns:		(Gint) - line_style
 */

Gint get_line_style()
{
	Gint line_style;

	switch (line_style_state)
	{
		case SOLID_LN:		line_style = GLN_SOLID;
					break;

		case DASHED_LN:		line_style = GLN_DASH;
					break;

		case DASH_DOT_LN:	line_style = GLN_DOTDASH;
					break;

		case DOTTED_LN:		line_style = GLN_DOT;
					break;

		default:		line_style = GLN_SOLID;
	}

	return(line_style);
}  /* end get_line_style */


/*
 *  switch_curve_connected_state
 *
 *  description:	advances popup state to next state and
 *			redraws popup
 *
 *  parameters:		popup (POPUP *) - curve_connected popup
 *			pt (Gpoint) - unused
 */

switch_curve_connected_state(popup,pt)
POPUP *popup;
Gpoint pt;
{
	Gpoint t;
	switch_popup_state(&line_connected_state,2,3,
		line_connected_text,popup->extent);

	switch (line_connected_state)
	{
		case CONNECTED:		if ((pt_needed == FIRST_PT) &&
						(object != (OBJECT *) NULL))
					{
						if (object->visibility == GVISIBLE)
						{
							add_curve_exec(prev_control[3]);
							t.x = prev_control[3].x * 2 - prev_control[2].x;
							t.y = prev_control[3].y * 2 - prev_control[2].y;
							add_curve_exec(t);
						}
						else if (past_control[0].x != -1.0)
						{
							add_curve_exec(past_control[3]);
							t.x = past_control[3].x * 2 - past_control[2].x;
							t.y = past_control[3].y * 2 - past_control[2].y;
							add_curve_exec(t);
						}
					}
					break;

		case NOT_CONNECTED:	if (object != (OBJECT *) NULL)
					{
						if (pt_needed == THIRD_PT)
						{
							pt_needed = FIRST_PT;
							prompt(36);
						}
					}
					break;
	}

}  /* end switch_curve_connected_state */


static int fill_style_state = SOLID_FILL;

static char *fill_style_text[] =
{
	"SOLID",
	"HOLLOW"
};


/*
 *  init_fill_style_popup
 *
 *  description:	displays popup box and text
 *
 *  parameters:		popup (POPUP *) - fill_style popup
 */

init_fill_style_popup(popup)
POPUP *popup;
{
	char **ln_ptr;
	int i;

	ln_ptr = fill_style_text;
	for (i=0; i<(1 * fill_style_state); i++)
		ln_ptr++;
	display_popup(popup,1,ln_ptr);

}  /* end init_fill_style_popup */


/*
 *  switch_fill_style_state
 *
 *  description:	advances popup state to next state and
 *			redraws popup
 *
 *  parameters:		popup (POPUP *) - fill_style popup
 *			pt (Gpoint) - unused
 */

switch_fill_style_state(popup,pt)
POPUP *popup;
Gpoint pt;
{

	switch_popup_state(&fill_style_state,2,1,
		fill_style_text,popup->extent);
}  /* end switch_fill_style_state */


/*
 *  get_fill_style
 *
 *  description:	finds current value of fill_style_state
 *
 *  parameters:		none
 *
 *  returns:		(Gflinter) - fill_style_state
 */

Gflinter get_fill_style()
{
	Gflinter fill_style;

	switch (fill_style_state)
	{
		case SOLID_FILL:	fill_style = GSOLID;
					break;

		case HOLLOW_FILL:	fill_style = GHOLLOW;
					break;

		default:		fill_style = GSOLID;
	}
	return(fill_style);
}  /* end get_fill_style */


static Gint char_width_state = MEDIUM_WD;
static char *char_width_text[] =
{
	"VERY THIN",
	"THIN",
	"MEDIUM",
	"MED-WIDE",
	"WIDE",
	"VERY WIDE"
};

/*
 *  init_char_width_popup
 *
 *  description:	draws char width popup on screen
 *
 *  parameters:		popup (POPUP *) - char width popup
 */

init_char_width_popup(popup)
POPUP *popup;
{
	char **ln_ptr;
	int i;

	ln_ptr = char_width_text;
	for(i=0; i<(1 * char_width_state); i++)
		ln_ptr++;
	display_header_popup(popup,"CHAR WD",ln_ptr,1);
}  /* end init_char_width popup */


/*
 *  switch_char_width_popup
 *
 *  description:	updates char width
 *
 *  parameters:		popup (POPUP *) - char width popup
 */

switch_char_width_popup(popup,pt)
POPUP *popup;
Gpoint pt;
{
	switch_header_popup_state(&char_width_state,6,1,char_width_text,
		popup->extent);
}  /* end switch_char_width popup */


/*
 *  get_char_expansion
 *
 *  description:	returns current value of char expansion
 *
 *  parameters:		none
 *
 *  returns:		(Gfloat) - char expansion
 */

Gfloat
get_char_expansion()
{
	return(char_wd_tbl[char_width_state]);
}  /* end get_char_expansion */


/*
 *  get_char_width
 *
 *  description:	calculates char width
 *
 *  parameters:		ch_ptr (CHAR_OB *) - char info
 *
 *  returns:		(Gfloat) - char width
 */

Gfloat get_char_width(ch_ptr)
CHAR_OB *ch_ptr;
{
	Gpoint bound_rect[4];
	Gpoint extent[2];
	Gpoint up_vec;

	up_vec.x = 0.0;
	up_vec.y = 1.0;

	get_text_extent(ch_ptr->ch,ch_ptr->center,ch_ptr->fontstyle,
		ch_ptr->height,ch_ptr->expansion,up_vec,bound_rect);
	get_extent(bound_rect,4,extent);
	return((extent[MAX].x - extent[MIN].x));
}  /* end get_char_width */


static Gint char_height_state = MEDIUM_HT;

static char *char_height_text[] =
{
	"V. SHORT",
	"SHORT",
	"MEDIUM",
	"MED-TALL",
	"TALL",
	"VERY TALL"
};

/*
 *  init_char_height_popup
 *
 *  description:	draws char height popup on screen
 *
 *  parameters:		popup (POPUP *) - char height popup
 */

init_char_height_popup(popup)
POPUP *popup;
{
	char **ln_ptr;
	int i;

	ln_ptr = char_height_text;
	for (i=0; i<(1 * char_height_state); i++)
		ln_ptr++;
	display_header_popup(popup,"CHAR HT",ln_ptr,1);
}  /* end init_char_height popup */


/*
 *  switch_char_height_popup
 *
 *  description:	updates char height
 *
 *  parameters:		popup (POPUP *) - char height popup
 */

switch_char_height_popup(popup,pt)
POPUP *popup;
Gpoint pt;
{
	switch_header_popup_state(&char_height_state,6,1,
		char_height_text,popup->extent);
}  /* end switch_char_height popup */


/*
 *  get_char_height
 *
 *  description:	returns current value of char height
 *
 *  parameters:		none
 *
 *  returns:		(Gfloat) - char height
 */

Gfloat get_char_height()
{
	return(char_ht_tbl[char_height_state]);
}  /* end get_char_height */


static int font_index = 0;
static int stroke_font_tbl_sz = 0;
static IDX *stroke_font_tbl = (IDX *) NULL;
static char *font_text[] =
{
	"FONT",
	"STYLE"
};


/*
 *  init_font_style_popup
 *
 *  description:	displays popup box and text
 *
 *  parameters:		popup (POPUP *) - font_style popup
 */

init_font_style_popup(popup)
POPUP *popup;
{
	IDX old_font;
	Gtxprec old_precision;

	/* if not set up, set up font tbl */

	if (stroke_font_tbl_sz == 0)
	{
		alloc_stroke_font_tbl(&stroke_font_tbl,
			&stroke_font_tbl_sz);
		if (stroke_font_tbl_sz == 0)
			exit_error("init_font_style_popup",11);
	}

	/* display_popup */

	get_font(POPUP_TEXT,&old_font,&old_precision);
	set_font(POPUP_TEXT,stroke_font_tbl[font_index],GSTROKE);
	display_popup(popup,2,font_text);
	set_font(POPUP_TEXT,old_font,old_precision);

}  /* end init_font_style_popup */


/*
 *  switch_font_style_popup
 *
 *  description:	advances popup state to next state and
 *			redraws popup
 *
 *  parameters:		popup (POPUP *) - font_style popup
 *			pt (Gpoint) - unused
 */

switch_font_style_popup(popup,pt)
POPUP *popup;
Gpoint pt;
{
	IDX temp_font;
	Gtxprec temp_precision;

	push_curr_trans();
	activate(POPUP_AREA);
	set_aspect_flags(GBUNDLED);

	font_index = (font_index + 1) % stroke_font_tbl_sz;

	get_font(POPUP_TEXT,&temp_font,&temp_precision);
	set_font(POPUP_TEXT,stroke_font_tbl[font_index],GSTROKE);
	display_popup(popup,2,font_text);
	set_font(POPUP_TEXT,temp_font,temp_precision);

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();
}  /* end switch_font_style_popup */


/*
 *  get_font_style
 *
 *  description:	finds current value of font_style_state
 *
 *  parameters:		none
 *
 *  returns:		(int) - font_style_state
 */

get_font_style()
{
	return(stroke_font_tbl[font_index]);
}  /* end get_font_style */


static int text_path_state = RIGHT_DIR;

static char *text_path_text[] =
{
	"RIGHT",
	"LEFT",
	"DOWN",
	"UP"
};


/*
 *  init_text_path_popup
 *
 *  description:	displays popup box and text
 *
 *  parameters:		popup (POPUP *) - text_path popup
 */

init_text_path_popup(popup)
POPUP *popup;
{
	char **ln_ptr;
	int i;

	ln_ptr = text_path_text;
	for (i=0; i < text_path_state; i++)
		ln_ptr++;
	display_header_popup(popup,"TEXT PATH",ln_ptr,1);

}  /* end init_text_path_popup */


/*
 *  switch_text_path_popup
 *
 *  description:	advances popup state to next state and
 *			redraws popup
 *
 *  parameters:		popup (POPUP *) - text_path popup
 *			pt (Gpoint) - unused
 */

switch_text_path_popup(popup,pt)
POPUP *popup;
Gpoint pt;
{

	switch_header_popup_state(&text_path_state,4,1,
		text_path_text,popup->extent);
}  /* end switch_text_path_popup */


/*
 *  get_text_path
 *
 *  description:	finds current value of text_path_state
 *
 *  parameters:		none
 *
 *  returns:		(Gtxpath) - text_path
 */

Gtxpath get_text_path()
{
	Gtxpath text_path;

	switch (text_path_state)
	{
		case RIGHT_DIR:		text_path = GTP_RIGHT;
					break;

		case LEFT_DIR:		text_path = GTP_LEFT;
					break;

		case DOWN_DIR:		text_path = GTP_DOWN;
					break;

		case UP_DIR:		text_path = GTP_UP;
					break;

		default:		text_path = GTP_RIGHT;
	}

	return(text_path);
}  /* end get_text_path */


static int horz_just_state = CENTER_HORZ;

static char *horz_just_text[] =
{
	"CENTER",
	"LEFT",
	"RIGHT"
};


/*
 *  init_horz_just_popup
 *
 *  description:	displays popup box and text
 *
 *  parameters:		popup (POPUP *) - horz_just popup
 */

init_horz_just_popup(popup)
POPUP *popup;
{
	char **ln_ptr;
	int i;

	ln_ptr = horz_just_text;
	for (i=0; i<(2 * horz_just_state); i++)
		ln_ptr++;
	display_header_popup(popup,"HORZ JUST",ln_ptr,1);

}  /* end init_horz_just_popup */


/*
 *  switch_horz_just_popup
 *
 *  description:	advances popup state to next state and
 *			redraws popup
 *
 *  parameters:		popup (POPUP *) - horz_just popup
 *			pt (Gpoint) - unused
 */

switch_horz_just_popup(popup,pt)
POPUP *popup;
Gpoint pt;
{

	switch_header_popup_state(&horz_just_state,3,1,horz_just_text,
		popup->extent);
}  /* end switch_horz_just_popup */


/*
 *  get_horz_just
 *
 *  description:	finds current value of horz_just_state
 *
 *  parameters:		none
 *
 *  returns:		(Gtxhor) - horz_just
 */

Gtxhor get_horz_just()
{
	Gtxhor horz_just;

	switch (horz_just_state)
	{
		case CENTER_HORZ:	horz_just = GTH_CENTER;
					break;

		case LEFT_JUST_HORZ:	horz_just = GTH_LEFT;
					break;

		case RIGHT_JUST_HORZ:	horz_just = GTH_RIGHT;
					break;

		default:		horz_just = GTH_LEFT;
	}

	return(horz_just);
}  /* end get_horz_just */


static int vert_just_state = CENTER_VERT;

static char *vert_just_text[] =
{
	"TOP",
	"CENTER",
	"BOTTOM"
};


/*
 *  init_vert_just_popup
 *
 *  description:	displays popup box and text
 *
 *  parameters:		popup (POPUP *) - vert_just popup
 */

init_vert_just_popup(popup)
POPUP *popup;
{
	char **ln_ptr;
	int i;

	ln_ptr = vert_just_text;
	for (i=0; i<(1 * vert_just_state); i++)
		ln_ptr++;
	display_header_popup(popup,"VERT JUST",ln_ptr,1);

}  /* end init_vert_just_popup */


/*
 *  switch_vert_just_popup
 *
 *  description:	advances popup state to next state and
 *			redraws popup
 *
 *  parameters:		popup (POPUP *) - vert_just popup
 *			pt (Gpoint) - unused
 */

switch_vert_just_popup(popup,pt)
POPUP *popup;
Gpoint pt;
{
	switch_header_popup_state(&vert_just_state,3,1,vert_just_text,
		popup->extent);
}  /* end switch_vert_just_popup */


/*
 *  get_vert_just
 *
 *  description:	finds current value of vert_just_state
 *
 *  parameters:		none
 *
 *  returns:		(Gtxver) - vert_just
 */

Gtxver get_vert_just()
{
	Gtxver vert_just;

	switch (vert_just_state)
	{
		case CENTER_VERT:	vert_just = GTV_HALF;
					break;

		case TOP_JUST_VERT:	vert_just = GTV_CAP;
					break;

		case BOTTOM_JUST_VERT:	vert_just = GTV_BASE;
					break;

		default:		vert_just = GTV_BASE;
	}

	return(vert_just);
}  /* end get_vert_just */
