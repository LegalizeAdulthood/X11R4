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
 *	alloc_stroke_font_tbl
 */

static char rcsid[] = "$Header: add_objects_subs.c,v 4.0 89/09/01 15:52:18 amy Exp $";

#include <xgks.h>
#include "changes.h"
#include "ws.h"

extern char *calloc();

/*
 *  alloc_stroke_font_tbl
 *
 *  description:	creates list of fonts with stroke precision
 *
 *  parameters:		list (IDX **) - pointer to list of fonts
 *				with stroke precision
 *			n (int *) - no of fonts with stroke precision
 */

alloc_stroke_font_tbl(list,n)
IDX **list;
int *n;
{
	int i,j;
	Gtxfac attr;

	ginqtextfacil(ws_type,&attr);

	/* count number of fonts with stroke precision */

	*n = 0;
	for (i=0; i<(attr.fps); i++)
	{
		if ((attr.fp_list[i]).prec == GSTROKE)
			(*n)++;
	}

	/* assign stroke font list */

	*list = (IDX *) calloc((unsigned) *n, sizeof(IDX));
	j=0;
	for (i=0; i<attr.fps; i++)
	{
		if ((attr.fp_list[i]).prec == GSTROKE)
			(*list)[j++] = attr.fp_list[i].font;
	}
}  /* end alloc_stroke_font_tbl */
