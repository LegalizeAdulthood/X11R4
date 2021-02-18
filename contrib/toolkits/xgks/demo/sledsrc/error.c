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
 *	exit_error
 */

static char rcsid[] = "$Header: error.c,v 4.0 89/09/01 15:53:30 amy Exp $";

#include <stdio.h>

/*
 *  error messages
 */

char *err_message[] =
{
	"Ran out of normalization transformations",	/* 0 */
	"Cannot open menu file",			/* 1 */
	"Error reading menu file",			/* 2 */
	"Cannot open text_bndl_tbl file",		/* 3 */
	"Error reading text_bndl_tbl file",		/* 4 */
	"Color tbl limit exceeded",			/* 5 */
	"Cannot open palette_clrs file",		/* 6 */
	"Error reading palette_clrs file",		/* 7 */
	"Cannot find area in menu_tbl",			/* 8 */
	"Cannot find area in object_func_tbl",		/* 9 */
	"Cannot find menu item",			/* 10 */
	"No stroke fonts available",			/* 11 */
	"Error opening input file",			/* 12 */
	"Error reading input file",			/* 13 */
	"Error opening output file",			/* 14 */
	"Error reading output file",			/* 15 */
	"Error opening char_hts_n_wds file",		/* 16 */
	"Error reading char_hts_n_wds file"		/* 17 */
};


/*
 *  exit_error
 *
 *  description:	prints name of routine in which error occurred
 *			and error message and exits
 *
 *  parameters:		routine (char *) - routine in which error
 *			occurred
 *			errnum (int) - error number
 */

exit_error(routine,errnum)
char *routine;
int errnum;
{
	fprintf(stderr,"Error: %s: %s\n",routine,err_message[errnum]);
	abort();

} /* end exit_error */
