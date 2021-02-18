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
 *	alloc_comb_ob
 *	alloc_single_ob
 *	add_to_comb_ob
 *	enq_comb_ob
 *	enq_after
 *	deq_comb_ob
 *	redraw_objects
 *	get_last_object
 *	update_trans
 *	collect_garbage
 *	cleanup_menu_n_garbage
 *	no_objects
 *	get_object_list
 */

static char rcsid[] = "$Header: object_list.c,v 4.0 89/09/01 15:54:23 amy Exp $";

#include "defs.h"
#include "draw.h"
#include "popup.h"
#include "trans.h"
#include "key.h"
#include "objects.h"
#include "object_tbl.h"
#include "screen_items.h"
#include <xgks.h>

extern char *calloc();

COMB_OB *head = (COMB_OB *) NULL;
COMB_OB *tail = (COMB_OB *) NULL;


/*
 *  alloc_comb_ob
 *
 *  description:	allocates and intializes COMB_OB
 *
 *  parameters:		none
 *
 *  returns:		comb_ob (COMB_OB *)
 */

COMB_OB *
alloc_comb_ob()
{
	COMB_OB *comb_ob;

	comb_ob = (COMB_OB *) calloc(1,sizeof(COMB_OB));
	comb_ob->visibility = GVISIBLE;
	comb_ob->child = (OBJECT *) NULL;
	comb_ob->prev = (COMB_OB *) NULL;
	comb_ob->next = (COMB_OB *) NULL;

	return(comb_ob);
}  /* end alloc_comb_ob */


/*
 *  alloc_single_ob
 *
 *  description:	allocates and intializes OBJECT
 *
 *  parameters:		key (KEY) - object type
 *
 *  returns:		single_ob (OBJECT *)
 */

OBJECT *
alloc_single_ob(key)
KEY key;
{
	OBJECT *single_ob;

	single_ob = (OBJECT *) calloc(1,sizeof(OBJECT));
	single_ob->key = (char *) calloc((unsigned) (strlen(key) + 1),
		sizeof(char));
	(void) strcpy(single_ob->key,key);
	single_ob->visibility = GVISIBLE;
	ident(single_ob->trans);
	single_ob->parent = (COMB_OB *) NULL;
	single_ob->next = (OBJECT *) NULL;

	return(single_ob);
}  /* end alloc_single_ob */


/*
 *  add_to_comb_ob
 *
 *  description:	adds list of simple objects
 *			to combination_object at either head or tail
 *			of list
 *
 *  parameters:		comb_ob (COMB_OB *) - combination object
 *			object_list (OBJECT *) - linked list of simple
 *				objects
 *			end (ENUM) - which end of current list
 *				(HEAD or TAIL) to add new list to
 */

add_to_comb_ob(comb_ob,object_list,end)
COMB_OB *comb_ob;			/* combination object */
OBJECT *object_list;			/* list of simple objects */
int end;				/* HEAD or TAIL */
{
	OBJECT *temp_ob;
	OBJECT *end_of_list;

	if (object_list == (OBJECT *) NULL)
		return;
	
	if ((comb_ob->child == (OBJECT *) NULL) || (end == HEAD))
	{
		temp_ob = comb_ob->child;
		comb_ob->child = object_list;

		/* find end of object_list */

		for (end_of_list = object_list; 
			end_of_list->next != (OBJECT *) NULL;
			end_of_list = end_of_list->next);
		
		end_of_list->next = temp_ob;
	}
	else  /* end == TAIL */
	{
		/* find end of list of simple
		   objects already in comb_ob */

		for (end_of_list = comb_ob->child; 
			end_of_list->next != (OBJECT *) NULL;
			end_of_list = end_of_list->next);
		
		end_of_list->next = object_list;
	}
}  /* end add_to_comb_ob */


/*
 *  enq_comb_ob
 *
 *  description:	enques comb_ob at tail of priority list
 *			(i.e. highest prio => displayed last)
 *
 *  parameters:		comb_ob (COMB_OB) - combination object to enq
 */

enq_comb_ob(comb_ob)
COMB_OB *comb_ob;
{
	comb_ob->prev = tail;
	comb_ob->next = (COMB_OB *) NULL;
	if (tail == (COMB_OB *) NULL)
		head = comb_ob;
	else
		tail->next = comb_ob;
	tail = comb_ob;
}  /* end enq_comb_ob */


/*
 *  enq_after
 *
 *  description:	enques comb_ob after specified comb_ob
 *			(i.e. highest prio => displayed last)
 *
 *  parameters:		comb_ob (COMB_OB *) - combination object to enq
 *			prev_comb_ob (COMB_OB *) - comb ob to enq after
 */

enq_after(comb_ob,prev_comb_ob)
COMB_OB *comb_ob;
COMB_OB *prev_comb_ob;
{
	if (tail == prev_comb_ob)
		tail = comb_ob;
	else
		prev_comb_ob->next->prev = comb_ob;

	comb_ob->next = prev_comb_ob->next;
	comb_ob->prev = prev_comb_ob;
	prev_comb_ob->next = comb_ob;
}  /* end enq_after */


/*
 *  deq_comb_ob
 *
 *  description:	deques comb_ob from priority list
 *
 *  parameters:		comb_ob (COMB_OB) - combination object to deq
 */

deq_comb_ob(comb_ob)
COMB_OB *comb_ob;
{
	if (head == comb_ob)
		head = comb_ob->next;
	if (tail == comb_ob)
		tail = comb_ob->prev;
	if (comb_ob->prev != (COMB_OB *) NULL)
		comb_ob->prev->next = comb_ob->next;
	if (comb_ob->next != (COMB_OB *) NULL)
		comb_ob->next->prev = comb_ob->prev;
	comb_ob->prev = (COMB_OB *) NULL;
	comb_ob->next = (COMB_OB *) NULL;
}  /* end deq_comb_ob */


/*
 *  redraw_objects
 *
 *  description:	redraws objects on screen in proper order
 *
 *  parameters:		none
 */

redraw_objects()
{
	Gpoint window[4];
	Gpoint box[4];

	push_curr_trans();
	activate(PICT_AREA);

	set_aspect_flags(GBUNDLED);

	get_viewport(PICT_AREA,window);
	set_window(PICT_AREA,window);
	load_box(box,window[MIN].x,window[MAX].x,
		window[MIN].y,window[MAX].y);
	gsetfillind(WHITE_HOLLOW);
	gfillarea(4,box);

	set_aspect_flags(GINDIVIDUAL);
	draw_objects();
	pop_curr_trans();

	if (get_grid_state() != GRID_OFF)
		draw_grid();
}  /* end redraw_objects */


/*
 *  draw_objects
 *
 *  descritpion:	traces through list and draws each object
 *			does not set transformations or flags
 *
 *  parameters:		none
 */

 draw_objects()
 {
	COMB_OB *comb_ob_ptr;

	for (comb_ob_ptr = head; comb_ob_ptr != (COMB_OB *) NULL;
		comb_ob_ptr = comb_ob_ptr->next)
	{
		if (comb_ob_ptr->visibility == GVISIBLE)
			draw_comb_ob(comb_ob_ptr);
	}
 }  /* end draw_objects */


/*
 *  get_last_object
 *
 *  description:	returns last single object in list, if any
 *
 *  parameters:	none
 */

OBJECT *
get_last_object()
{
	OBJECT *object;

	if (tail == (COMB_OB *) NULL)
		object = (OBJECT *) NULL;
	else if (tail->child == (OBJECT *) NULL)
		object = (OBJECT *) NULL;
	else
	{
		for (object = tail->child;
			object->next != (OBJECT *) NULL;
			object = object->next);

	}

	return(object);
}


/*
 *  update_trans
 *
 *  description:	updates transformations of all
 *			objects in comb_ob
 *
 *  parameters:		comb_ob (COMB_OB *) - combination object
 *				to transform
 *			trans (Gfloat *) - transformation matrix
 */

update_trans(comb_ob,trans)
COMB_OB *comb_ob;
Gfloat *trans;
{
	OBJECT *object_ptr;

	for (object_ptr = comb_ob->child;
		object_ptr  != (OBJECT *) NULL;
		object_ptr = object_ptr->next)
	{
		mat_mult(object_ptr->trans,trans,object_ptr->trans);
	}
}  /* end update_trans */


/*
 *  collect_garbage
 *
 *  description:	eliminates invisible comb_obs and objects
 *
 *  parameters:		none
 */

collect_garbage()
{
	COMB_OB *comb_ob;
	COMB_OB *temp_comb_ob;
	OBJECT *object;
	OBJECT *prev;
	OBJECT *temp_object;

	comb_ob = head;
	while (comb_ob != (COMB_OB *) NULL)
	{
		/* delete comb_ob if invisible */

		if (comb_ob->visibility == GINVISIBLE)
		{
			temp_comb_ob = comb_ob->next;
			deq_comb_ob(comb_ob);
			free_comb_ob(comb_ob);
			comb_ob = temp_comb_ob;
		}
		else  /* check if parts of comb_ob are invisible */
		{

			prev = (OBJECT *) NULL;
			object = comb_ob->child;
			while (object != (OBJECT *) NULL)
			{
				if (object->visibility == GINVISIBLE)
				{
					if (object == comb_ob->child)
					{
						comb_ob->child = object->next;
					}
					else
					{
						prev->next = object->next;
					
					}
					temp_object = object->next;
					free_object(object);
					object = temp_object;
				}
				else
				{
					prev = object;
					object = object->next;
				}
			}
			/* if all parts of comb_ob were invisible,
			   then delete comb_ob too */

			if (comb_ob->child == (OBJECT *) NULL)
			{
				temp_comb_ob = comb_ob->next;
				deq_comb_ob(comb_ob);
				free_comb_ob(comb_ob);
				comb_ob = temp_comb_ob;
			}
			else
				comb_ob = comb_ob->next;
		}  /* end search for inivisible objects */
	}
}  /* end collect_garbage */


/*
 *  cleanup_menu_n_garbage
 *
 *  description:	cleans up menu, popups and removes invisible
 *			elements from linked list
 *
 *  parameters:		area (AREA) - menu area
 *			menu (MENU *) - ptr to menu to cleanup
 */

cleanup_menu_n_garbage(area,menu)
AREA area;
MENU *menu;
{
	collect_garbage();
	cleanup_menu_n_popups(area,menu);
}  /* end cleanup_menu_n_garbage */


/*
 *  no_objects
 *
 *  description:	tests if linked list is empty
 *			(may not be accurate if list has not been
 *			garbage collected)
 *
 *  parameters:		none
 *
 *  returns:		(BOOLEAN) - true if list is empty
 *				false, otherwise
 */

no_objects()
{
	return(((head == (COMB_OB *) NULL) ? TRUE : FALSE));
}  /* end no_objects */


/*
 *  get_object_list
 *
 *  description:	creates list of all objects completely
 *			within extent
 *
 *  parameters:		extent (Gpoint *) - extent to test objects against
 *
 *  returns:		list (COMB_OB_PTR *) - list of objects
 */

COMB_OB_PTR *
get_object_list(extent)
Gpoint *extent;
{
	COMB_OB_PTR *list;
	COMB_OB_PTR *comb_ob_ptr;
	COMB_OB *comb_ob;
	OBJECT *object;

	list = (COMB_OB_PTR *) NULL;
	
	for (comb_ob = tail; comb_ob != (COMB_OB *) NULL;
		comb_ob = comb_ob->prev)
	{
		if (comb_ob->visibility == GINVISIBLE)
			continue;

		for (object = comb_ob->child; object != (OBJECT *) NULL;
			object = object->next)
		{
			if (object->visibility == GINVISIBLE)
				continue;

			if (is_object_inbounds(object,extent) == TRUE)
			{
				comb_ob_ptr = (COMB_OB_PTR *)
					calloc((unsigned) 1,
					sizeof(COMB_OB_PTR));
				comb_ob_ptr->object = object;
				comb_ob_ptr->comb_ob = comb_ob;
				comb_ob_ptr->next = list;
				list = comb_ob_ptr;
			}
		}  /* end object traversal */
	}  /* end comb_ob traversal */

	return(list);
}  /* end get_object_list */
