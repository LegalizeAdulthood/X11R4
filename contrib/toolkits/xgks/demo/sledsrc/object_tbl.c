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
 *  this file contains:
 *	get_object_func_tbl_idx
 */

static char rcsid[] = "$Header: object_tbl.c,v 4.0 89/09/01 15:54:30 amy Exp $";

#include "defs.h"
#include "objects.h"
#include "object_tbl.h"
#include "objects_func.h"

int object_func_tbl_sz = 7;
struct object_func_tbl_entry object_func_tbl[] =
{
	{
		LINEOB,			/* object type key */
		draw_line,		/* draw function */
		erase_line,		/* erase function */
		detect_line,		/* detect function */
		get_line_extent,	/* get_extent function */
		copy_line,		/* copy function */
		rotate_gen,		/* rotate function */
		free_line,		/* free function */
		read_line,		/* read funtion */
		write_line		/* write funtion */
	},

	{
		BOXOB,			/* object type key */
		draw_poly,		/* draw function */
		erase_poly,		/* erase function */
		detect_poly,		/* detect function */
		get_poly_extent,	/* get_extent function */
		copy_poly,		/* copy function */
		rotate_gen,		/* rotate function */
		free_poly,		/* free function */
		read_poly,		/* read funtion */
		write_poly		/* write funtion */
	},

	{
		POLYOB,			/* object type key */
		draw_poly,		/* draw function */
		erase_poly,		/* erase function */
		detect_poly,		/* detect function */
		get_poly_extent,	/* get_extent function */
		copy_poly,		/* copy function */
		rotate_gen,		/* rotate function */
		free_poly,		/* free function */
		read_poly,		/* read funtion */
		write_poly		/* write funtion */
	},

	{
		CIRCLEOB,		/* object type key */
		draw_poly,		/* draw function */
		erase_poly,		/* erase function */
		detect_poly,		/* detect function */
		get_poly_extent,	/* get_extent function */
		copy_poly,		/* copy function */
		rotate_gen,		/* rotate function */
		free_poly,		/* free function */
		read_poly,		/* read funtion */
		write_poly		/* write funtion */
	},

	{
		CONICOB,		/* object type key */
		draw_poly,		/* draw function */
		erase_poly,		/* erase function */
		detect_poly,		/* detect function */
		get_poly_extent,	/* get_extent function */
		copy_poly,		/* copy function */
		rotate_gen,		/* rotate function */
		free_poly,		/* free function */
		read_poly,		/* read funtion */
		write_poly		/* write funtion */
	},

	{
		CURVEOB,		/* object type key */
		draw_line,		/* draw function */
		erase_line,		/* erase function */
		detect_line,		/* detect function */
		get_line_extent,	/* get_extent function */
		copy_line,		/* copy function */
		rotate_gen,		/* rotate function */
		free_line,		/* free function */
		read_line,		/* read funtion */
		write_line		/* write funtion */
	},

	{
		TEXTOB,			/* object type key */
		draw_text,		/* draw function */
		erase_text,		/* erase function */
		detect_text,		/* detect function */
		get_string_extent,	/* get_extent function */
		copy_text,		/* copy function */
		rotate_text,		/* rotate function */
		free_text,		/* free function */
		read_text,		/* read funtion */
		write_text		/* write funtion */
	}
};


/*
 *  get_object_func_tbl_idx
 *
 *  description:	finds index in object_func_tbl of
 *			object type identified by key
 *
 *  parameters:		key (KEY) - key identifying object type
 */

IDX
get_object_func_tbl_idx(key)
KEY key;
{
	IDX idx;

	for (idx = 0; ((idx < object_func_tbl_sz) &&
		!eq(key,object_func_tbl[idx].key)); idx++);

	if (!eq(key,object_func_tbl[idx].key))
		exit_error("get_object_func_tbl_idx",9);
	return(idx);
}  /* end get_object_func_tbl_idx */


