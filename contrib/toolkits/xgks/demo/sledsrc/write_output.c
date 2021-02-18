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
 * this file includes
 *	write_picture
 *	write_palette_clrs
 *	write_object
 *	write_line
 *	write_poly
 *	write_text
 */

static char rcsid[] = "$Header: write_output.c,v 4.0 89/09/01 15:55:55 amy Exp $";

#include <stdio.h>
#include "io.h"
#include "screen_items.h"
#include "color.h"
#include "palette.h"
#include "objects.h"
#include "object_tbl.h"
#include "object_list.h"
#include <xgks.h>
#include "conversions.h"

extern char *calloc();


/*
 *  write_picture
 *
 *  description:	write in picture description
 *
 *  parameters:		filename (char *) - name of file to write
 *
 *  returns:		(BOOLEAN) - true if input file successfully
 *			write in, false otherwise
 */

BOOLEAN
write_picture(filename)
char *filename;
{
	FILE *fd;
	OBJECT *object;
	COMB_OB *comb_ob;
	int i,j;

	/* open file for writing */

	fd = fopen(filename,"w+");
	if (fd == (FILE *) NULL)
	{
		reprompt(23);
		return(FALSE);
	}

	/* write out version number */

	write_char_str(fd,VERSION_NUM);

	/* write out palette, fgd color, bgd color and fill color */

	i = get_fgdclr();
	write_int(fd,i);
	/* i = get_bgdclr(); */
	i = bgdclr;
	write_int(fd,i);
	i = get_fillclr();
	write_int(fd,i);
	write_palette_clrs(fd);

	/* write in object list */

	i = 0;
	for (comb_ob = head; comb_ob != (COMB_OB *) NULL;
		comb_ob = comb_ob->next)
	{
		if (comb_ob->visibility == GVISIBLE)
			i++;
	}
	write_int(fd,i);

	for (comb_ob = head; comb_ob != (COMB_OB *) NULL;
		comb_ob = comb_ob->next)
	{
		if (comb_ob->visibility == GINVISIBLE)
			continue;

		j = 0;
		for (object = comb_ob->child; object != (OBJECT *) NULL;
			object = object->next)
		{
			if (object->visibility == GVISIBLE)
				j++;
		}
		write_int(fd,j);

		for (object = comb_ob->child; object != (OBJECT *) NULL;
			object = object->next)
		{
			if (object->visibility == GVISIBLE)
				write_object(fd,object);
		}
	}

	/* close file */

	fclose(fd);
	return(TRUE);
}  /* end write_picture */


/*
 *  write_palette_clrs
 *
 *  description:	allocates color table space for palette
 *			colors in color table and initializes colors
 *			in color table as well as initializing fgdclr
 *			to first entry in color table (top row, 
 *			leftmost column)
 *
 *  parameters:		fd (FILE *) - file descriptor
 */

write_palette_clrs(fd)
FILE *fd;
{
	Gcobundl rgb_clr;
	int i;

	/* write in number of rows and columns */

	write_int(fd,nrows);
	write_int(fd,ncols);

	/* write colors */

	for (i = palstartclr; i < (palstartclr + nrows * ncols); i++)
	{
		get_color(i,&rgb_clr);
		write_real3(fd,(rgb_clr.red),(rgb_clr.green),
			(rgb_clr.blue));
	}

}  /* end write_palette_clrs */


/*
 *  write_object
 *
 *  description:	writes out object description
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			object (OBJECT *) - object to write out
 */

write_object(fd,object)
FILE *fd;
OBJECT *object;
{
	int i;

	write_char_str(fd,object->key);

	write_int(fd,vis_to_num(object->visibility));
	for (i=0; i<6; i++)
		write_real(fd,object->trans[i]);

	i = get_object_func_tbl_idx(object->key);
	(*(object_func_tbl[i].write))(fd,object);
}  /* end write_object */


/*
 *  write_line
 *
 *  description:	writes in fields of struct line object
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			object (OBJECT *) - line object
 */

write_line(fd,object)
FILE *fd;
OBJECT *object;
{
	int i;

	write_int(fd,object->lineob.color);
	write_int(fd,object->lineob.style);
	write_int(fd,object->lineob.width);
	write_int(fd,object->lineob.nopts);

	for (i=0; i<object->lineob.nopts; i++)
		write_real2(fd,object->lineob.pts[i].x,
			object->lineob.pts[i].y);
}  /* end write_line */


/*
 *  write_poly
 *
 *  description:	writes in fields of struct poly object
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			object (OBJECT *) - poly object
 */

write_poly(fd,object)
FILE *fd;
OBJECT *object;
{
	int i;

	write_int(fd,style_to_num(object->polyob.fillstyle));
	write_int(fd,object->polyob.fillcolor);
	write_int(fd,object->polyob.linecolor);
	write_int(fd,object->polyob.style);
	write_int(fd,object->polyob.width);
	write_int(fd,object->polyob.nopts);

	for (i=0; i<object->polyob.nopts; i++)
		write_real2(fd,object->polyob.pts[i].x,
			object->polyob.pts[i].y);
}  /* end write_poly */


/*
 *  write_text
 *
 *  description:	writes in fields of struct text object
 *
 *  parameters:		fd (FILE *) - file descriptor
 *			object (OBJECT *) - text object
 */

write_text(fd,object)
FILE *fd;
OBJECT *object;
{
	CHAR_OB *ch_ptr;
	int i;

	write_int(fd,path_to_num(object->textob.path));
	write_int(fd,vert_to_num(object->textob.vert_just));
	write_int(fd,horz_to_num(object->textob.horz_just));
	write_real2(fd,object->textob.pos.x,
		object->textob.pos.y);
	write_real2(fd,object->textob.up_vec.x,
		object->textob.up_vec.y);

	i = 0;
	for (ch_ptr = object->textob.str; ch_ptr != (CHAR_OB *) NULL;
		ch_ptr = ch_ptr->next)
		i++;

	write_int(fd,i);
	for (ch_ptr = object->textob.str; ch_ptr != (CHAR_OB *) NULL;
		ch_ptr = ch_ptr->next)
	{
		write_real2(fd,ch_ptr->center.x,
			ch_ptr->center.y);
		write_int(fd,ch_ptr->color);
		write_real(fd,ch_ptr->height);
		write_int(fd,ch_ptr->fontstyle);
		write_real(fd,ch_ptr->expansion);
		write_real(fd,ch_ptr->width);
		write_char(fd,ch_ptr->ch);
	}
}  /* end write_text */


