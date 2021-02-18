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
 *	draw_comb_ob
 *	erase_comb_ob
 *	draw_object
 *	erase_object
 *	draw_line
 *	erase_line
 *	draw_poly
 *	erase_poly
 *	draw_text
 *	erase_text
 *	display_text
 */

static char rcsid[] = "$Header: objects_draw.c,v 4.0 89/09/01 15:54:46 amy Exp $";

#include "popup.h"
#include "color.h"
#include "objects.h"
#include "object_tbl.h"

#include "key.h"
#include "trans.h"
#include "draw.h"
#include "ws.h"


#define	DRAW	0
#define ERASE	1


/*
 *  draw_comb_ob
 *
 *  description:	draws combination object
 *			only draws visible objects
 *			but does not check if comb_ob is visible
 *
 *  parameters:		comb_ob (COMB_OB *) - comb_ob to draw
 */

draw_comb_ob(comb_ob)
COMB_OB *comb_ob;
{
	OBJECT *object_ptr;

	for (object_ptr = comb_ob->child;
		object_ptr != (OBJECT *) NULL;
		object_ptr = object_ptr->next)
	{
		if (object_ptr->visibility == GVISIBLE)
			draw_object(object_ptr);
	}
}  /* end draw_comb_ob */


/*
 *  erase_comb_ob
 *
 *  description:	erases combination object
 *			only erases visible objects
 *			but does not check if comb_ob is visible
 *
 *  parameters:		comb_ob (COMB_OB *) - comb_ob to erase
 */

erase_comb_ob(comb_ob)
COMB_OB *comb_ob;
{
	OBJECT *object_ptr;

	for (object_ptr = comb_ob->child;
		object_ptr != (OBJECT *) NULL;
		object_ptr = object_ptr->next)
	{
		erase_object(object_ptr);
	}
}  /* end erase_comb_ob */


/*
 *  draw_object
 *
 *  description:	draws object
 *
 *  parameters:		object (OBJECT *) - object to draw
 */

draw_object(object)
OBJECT *object;
{
	(*(object_func_tbl[get_object_func_tbl_idx(object->key)].draw))
		(object);
}  /* end draw_object */


/*
 *  erase_object
 *
 *  description:	erases object
 *
 *  parameters:		object (OBJECT *) - object to erase
 */

erase_object(object)
OBJECT *object;
{
	(*(object_func_tbl[get_object_func_tbl_idx(object->key)].erase))
		(object);
}  /* end erase_object */


/*
 *  draw_line
 *
 *  description:	draw routine for lineob
 *
 *  parameters:		object (OBJECT *) - line object
 */

draw_line(object)
OBJECT *object;
{
	/* update pts by multiplying them by transformation
	   matrix and then set trans to identity matrix */
	
	if (!is_ident(object->trans))
	{
		pt_mult(object->lineob.pts,object->lineob.nopts,
			object->trans);
		ident(object->trans);
	}

	gsetlinecolorind(object->lineob.color);
	gsetlinetype(object->lineob.style);
	gsetlinewidth((Gfloat) (object->lineob.width));
	gpolyline(object->lineob.nopts,object->lineob.pts);
}  /* end draw_line */


/*
 *  erase_line
 *
 *  description:	erase routine for lineob
 *
 *  parameters:		object (OBJECT *) - line object
 */

erase_line(object)
OBJECT *object;
{
	gsetlinecolorind(get_bgdclr());
	gsetlinetype(object->lineob.style);
	gsetlinewidth((Gfloat) object->lineob.width);
	gpolyline(object->lineob.nopts,object->lineob.pts);
}  /* end erase_line */


/*
 *  draw_poly
 *
 *  description:	draw routine for polyob
 *
 *  parameters:		object (OBJECT *) - poly (or box or circle
 *				or conic) object
 */

draw_poly(object)
OBJECT *object;
{
	/* update pts by multiplying them by transformation
	   matrix and then set trans to identity matrix */

	if (!is_ident(object->trans))
	{
		pt_mult(object->polyob.pts,object->polyob.nopts,
			object->trans);
		ident(object->trans);
	}

	gsetfillintstyle(object->polyob.fillstyle);
	gsetfillcolorind(object->polyob.fillcolor);
	gfillarea(object->polyob.nopts,object->polyob.pts);

	gsetlinecolorind(object->polyob.linecolor);
	gsetlinetype(object->polyob.style);
	gsetlinewidth((Gfloat) (object->polyob.width));
	gpolyline(object->polyob.nopts,object->polyob.pts);
}  /* end draw_poly */


/*
 *  erase_poly
 *
 *  description:	erase routine for polyob
 *
 *  parameters:		none
 */

erase_poly(object)
OBJECT *object;
{
	if (object->polyob.fillstyle == GSOLID)
	{
		gsetfillintstyle(object->polyob.fillstyle);
		gsetfillcolorind(get_bgdclr());
		gfillarea(object->polyob.nopts,object->polyob.pts);
	}

	gsetlinecolorind(get_bgdclr());
	gsetlinetype(object->polyob.style);
	gsetlinewidth((Gfloat) object->polyob.width);
	gpolyline(object->polyob.nopts,object->polyob.pts);
}  /* end erase_poly */


/*
 *  draw_text
 *
 *  description:	draws text string
 *
 *  parameters:		object (OBJECT *) - text object
 */

draw_text(object)
OBJECT *object;
{
	display_text(object,DRAW);
}  /* end draw_text */


/*
 *  erase_text
 *
 *  description:	draws text string
 *
 *  parameters:		object (OBJECT *) - text object
 */

erase_text(object)
OBJECT *object;
{
	display_text(object,ERASE);
}  /* end erase_text */


/*
 *  display_text
 *
 *  description:	draws text string
 *
 *  parameters:		object (OBJECT *) - text object
 *			draw_erase (int) - DRAW | ERASE
 */

display_text(object,draw_erase)
OBJECT *object;
int draw_erase;
{
	Gtxpath path;
	Gtxhor horz;
	Gtxver vert;
	Gpoint pos;
	Gfloat dir;
	Gpoint pt;
	Gfloat ht = 0;
	Gfloat wd = 0;
	CHAR_OB* ch_ptr;
	Gpoint window[2];
	char str[2];
	Gtxfp tfp;
	Gtxalign align;


	get_window(PICT_AREA,window);
	path = object->textob.path;
	horz = object->textob.horz_just;
	vert = object->textob.vert_just;
	pos = object->textob.pos;

	/* find width and height of string (ignoring transformations) */

	switch (path)
	{
		case GTP_RIGHT:
		case GTP_LEFT:	wd = 0;
				ht = 0;
				for (ch_ptr = object->textob.str;
					ch_ptr->next != 
					(struct char_ob *) NULL;
					ch_ptr = ch_ptr->next)
				{
					/* add in wd of char */
					wd += ch_ptr->width;

					/* add in spacing */
					wd += (ch_ptr->expansion *
						ch_ptr-> height +
						ch_ptr->next->expansion
						* ch_ptr->next->height)
						/ 5.0;
					
					/* find max char ht */
					if (ht < ch_ptr->height)
						ht = ch_ptr->height;
				}
				wd += ch_ptr->width;
				if (ht < ch_ptr->height)
					ht = ch_ptr->height;
				break;

		case GTP_UP:
		case GTP_DOWN:	wd = 0;
				ht = 0;
				for (ch_ptr = object->textob.str;
					ch_ptr->next != 
					(struct char_ob *) NULL;
					ch_ptr = ch_ptr->next)
				{
					/* add in ht of char */
					ht += ch_ptr->height;

					/* add in spacing */
					ht += (ch_ptr->height +
						ch_ptr->next->height)
						/ 40.0;
					
					/* find max char wd */
					if (wd < ch_ptr->width)
						wd = ch_ptr->width;
				}
				ht += ch_ptr->height;
				if (wd < ch_ptr->width)
					wd = ch_ptr->width;
				break;
	}  /* end switch */

	/* find starting x position (ignoring transformation) */

	switch (path)
	{
		case GTP_RIGHT:	if (horz == GTH_LEFT)
					pt.x = pos.x;
				else if (horz == GTH_RIGHT)
					pt.x = pos.x - wd;
				else  /* horz == GTH_CENTER */
					pt.x = pos.x - (wd / 2.0);
				break;

		case GTP_LEFT:	if (horz == GTH_LEFT)
					pt.x = pos.x + wd;
				else if (horz == GTH_RIGHT)
					pt.x = pos.x;
				else  /* horz == GTH_CENTER */
					pt.x = pos.x + (wd / 2.0);
				break;

		case GTP_UP:
		case GTP_DOWN:	if (horz == GTH_LEFT)
					pt.x = pos.x + (wd / 2.0);
				else if (horz == GTH_RIGHT)
					pt.x = pos.x - (wd / 2.0);
				else  /* horz == GTH_CENTER */
					pt.x = pos.x;
				break;

	}  /* end switch */

	/* find starting y position (ignoring transformation) */

	switch (path)
	{
		case GTP_RIGHT:
		case GTP_LEFT:	if (vert == GTV_CAP)
					pt.y = pos.y - (ht / 2.0);
				else if (vert == GTV_HALF)
					pt.y = pos.y;
				else  /* vert == GTV_BASE */
					pt.y = pos.y + (ht / 2.0);
				break;

		case GTP_UP:	if (vert == GTV_CAP)
					pt.y = pos.y - ht;
				else if (vert == GTV_HALF)
					pt.y = pos.y - (ht / 2.0);
				else  /* vert == GTV_BASE */
					pt.y = pos.y;
				break;

		case GTP_DOWN:	if (vert == GTV_CAP)
					pt.y = pos.y;
				else if (vert == GTV_HALF)
					pt.y = pos.y + (ht / 2.0);
				else  /* vert == GTV_BASE */
					pt.y = pos.y + ht;
				break;

	}  /* end switch */

	/* calculate center of each char (ignoring transformations) */

	switch (path)
	{
		case GTP_RIGHT:	dir = 1.0;
				break;

		case GTP_LEFT:	dir = -1.0;
				break;

		case GTP_UP:	dir = 1.0;
				break;

		case GTP_DOWN:	dir = -1.0;
				break;

		default:	dir = 1.0;
	}  /* end switch */

	switch (path)
	{
		case GTP_RIGHT:	
		case GTP_LEFT:	for (ch_ptr = object->textob.str;
					ch_ptr != (struct char_ob *) NULL;
					ch_ptr = ch_ptr->next)
				{
					ch_ptr->center.x = pt.x
						+ dir * ch_ptr->width
						/ 2.0;
					ch_ptr->center.y = pt.y;

					if (ch_ptr->next != (struct char_ob *) NULL)
					{
						pt.x += (dir * ch_ptr->width);
						pt.x += dir *
							(ch_ptr->expansion
							* ch_ptr-> height +
							ch_ptr->next->expansion * ch_ptr->next->height) / 5.0;
					}
				}
				break;

		case GTP_UP:	
		case GTP_DOWN:	for (ch_ptr = object->textob.str;
					ch_ptr != (struct char_ob *) NULL;
					ch_ptr = ch_ptr->next)
				{
					ch_ptr->center.y = pt.y
						+ dir * ch_ptr->height
						/ 2.0;
					ch_ptr->center.x = pt.x;

					if (ch_ptr->next != (struct char_ob *) NULL)
					{
						pt.y += (dir * ch_ptr->height);
						pt.y += dir * (ch_ptr->height +
							ch_ptr->next->height) / 40.0;
					}
				}
				break;
	}  /* end switch */

	/* multiply pts by transformation matrix */

	for (ch_ptr = object->textob.str;
		ch_ptr != (struct char_ob *) NULL;
		ch_ptr = ch_ptr->next)
	{
		pt_mult(&(ch_ptr->center),1,object->trans);
	}

	/* finally - display text */

	align.hor = GTH_CENTER;
	align.ver = GTV_HALF;
	gsettextalign(&align);

	gsettextpath(path);
	gsetcharup(&(object->textob.up_vec));

	str[1] = (char) NULL;
	for (ch_ptr = object->textob.str;
		ch_ptr != (CHAR_OB *) NULL;
		ch_ptr = ch_ptr->next)
	{
		gsettextcolorind((draw_erase == DRAW) ?
			ch_ptr->color : get_bgdclr());
		gsetcharheight(ch_ptr->height);

		tfp.font = ch_ptr->fontstyle;
		tfp.prec = GSTROKE;
		gsettextfontprec(&tfp);

		gsetcharexpan(ch_ptr->expansion);

		str[0] = ch_ptr->ch;

		gtext(&(ch_ptr->center),str);
	}
}  /* end display_text */
