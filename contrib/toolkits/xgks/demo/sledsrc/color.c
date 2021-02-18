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
 *	rgb_to_hsv
 *	hsv_to_rgb
 *	set_color
 *	get_color
 *	init_screen_clrs
 *	alloc_clr_tbl_entries
 */

static char rcsid[] = "$Header: color.c,v 4.0 89/09/01 15:52:51 amy Exp $";

#include <xgks.h>
#include "defs.h"
#include "ws.h"
#include "color.h"


/*
 *  these color tbl entries are reserved for the screen
 *  they may not be altered by the user
 */

IDX black;				/* clr tbl idx for black */
IDX white;				/* clr tbl idx for white */
IDX grey;				/* clr tbl idx for grey */

/*
 *  other important indices
 */

IDX fgdclr;				/* clr tbl idx for fgd_clr */
IDX fillclr;				/* clr tbl idx for fgd_clr */
IDX bgdclr;


/*
 *  rgb_to_hsv
 *
 *  description:	converts color from rgb to hsv color model
 *			NOTE: rgb corresponds to "Gcobundl" model in new GKS.
 *
 *  parameters:		rgb_clr (Gcobundl *) ptr to color in rgb form
 *			hsv_clr (HSV *) ptr to color in hsv form
 */

rgb_to_hsv(rgb_clr,hsv_clr)
Gcobundl *rgb_clr;			/* clr in rgb form */
HSV *hsv_clr;				/* color in hsv form */
{
	Gfloat maxi;			/* max of r,g,b */
	Gfloat mini;			/* min of r,g,b */
	Gfloat rc;			/* dist from red to color */
	Gfloat gc;			/* dist from green to color */
	Gfloat bc;			/* dist from blue to color */

	maxi = max3(rgb_clr->red,rgb_clr->green,rgb_clr->blue);
	mini = min3(rgb_clr->red,rgb_clr->green,rgb_clr->blue);

	/* assign value */

	hsv_clr->val = maxi;

	/* assign saturation */

	if (maxi != 0)
		hsv_clr->sat = (maxi - mini) / maxi;
	else
		hsv_clr->sat = 0;

	/* assign hue */

	if (hsv_clr->sat == 0)
		hsv_clr->hue = UNDEFINED;	/* achromatic case */
	else
	{
		rc = (maxi - rgb_clr->red) / (maxi - mini);
		gc = (maxi - rgb_clr->green) / (maxi - mini);
		bc = (maxi - rgb_clr->blue) / (maxi - mini);

		if (rgb_clr->red == maxi)
			hsv_clr->hue = bc - gc;
		else if (rgb_clr->green == maxi)
			hsv_clr->hue = 2 + rc - bc;
		else /* rgb->blue == maxi */
			hsv_clr->hue = 4 + gc - rc;

		hsv_clr->hue *= 60;
		if (hsv_clr->hue < 0)
			hsv_clr->hue += 360;
	}  /* end chromatic case */

}  /* end rgb_to_hsv */


/*
 *  hsv_to_rgb
 *
 *  description:	converts color from hsv to rgb color model
 *
 *  parameters:		hsv_clr (HSV *) - ptr to color in hsv clr model
 *			rgb_clr (RGB*) - ptr to color in rgb clr model
 */

hsv_to_rgb(hsv_clr,rgb_clr)
HSV *hsv_clr;				/* color in hsv form */
Gcobundl *rgb_clr;			/* color in rgb form */
{
	int integer_part = 0;		/* integer_part of hue */
	Gfloat fractional_part;		/* fractional part of hue */
	Gfloat t1,t2,t3;			/* temps */

	if (hsv_clr->sat == 0)
	{
		/* achromatic case */

		rgb_clr->red = hsv_clr->val;
		rgb_clr->green = hsv_clr->val;
		rgb_clr->blue = hsv_clr->val;
	}
	else
		/* chromatic case */

		integer_part = (int) (hsv_clr->hue / 60);
		fractional_part = (hsv_clr->hue / 60) 
			- (Gfloat) integer_part;
		
		t1 = hsv_clr->val * (1 - hsv_clr->sat);
		t2 = hsv_clr->val * (1 - 
			(hsv_clr->sat * fractional_part));
		t3 = hsv_clr->val * (1 - (hsv_clr->sat *
			(1 - fractional_part)));

		/* integer part = sextent
		   and assignment of t1,t2,t3 is based on 
		   which sextent the color is in */

		switch (integer_part)
		{
			case 0:		rgb_clr->red = hsv_clr->val;
					rgb_clr->green = t3;
					rgb_clr->blue = t1;
					break;

			case 1:		rgb_clr->red = t2;
					rgb_clr->green = hsv_clr->val;
					rgb_clr->blue = t1;
					break;

			case 2:		rgb_clr->red = t1;
					rgb_clr->green = hsv_clr->val;
					rgb_clr->blue = t3;
					break;

			case 3:		rgb_clr->red = t1;
					rgb_clr->green = t2;
					rgb_clr->blue = hsv_clr->val;
					break;

			case 4:		rgb_clr->red = t3;
					rgb_clr->green = t1;
					rgb_clr->blue = hsv_clr->val;
					break;

			case 5:		rgb_clr->red = hsv_clr->val;
					rgb_clr->green = t1;
					rgb_clr->blue = t2;
					break;

		}

}  /* end hsv_to_rgb */


/*
 *  set_color
 *
 *  description:	sets color table entry to desired color
 *
 *  parameters:		idx (IDX) - index into color table
 *			rgb_clr (Gcobundl *) - color in rgb form
 */

set_color(idx,rgb_clr)
IDX idx;				/* color table index */
Gcobundl *rgb_clr;			/* actual color in rgb form */
{

	gsetcolorrep(ws_id,idx,rgb_clr);
}  /* end set_color */


/*
 *  get_color
 *
 *  description:	gets color table entry
 *
 *  parameters:		idx (IDX) - index into color table
 *  			rgb_clr (Gcobundl *) - color in rgb form
 */

get_color(idx,rgb_clr)
IDX idx;				/* color table index */
Gcobundl *rgb_clr;			/* actual color in rgb form */
{

	ginqcolourrep(ws_id,idx,GSET,rgb_clr);

}  /* end get_color */


/*
 *  init_screen_clrs
 *
 *  description:	initialize colors used in drawing screen
 *
 *  parameters:		none
 */

init_screen_clrs()
{
	Gcobundl rgb_clr;

	black = alloc_clr_tbl_entries(3);
	white = black + 1;
	grey = white + 1;

	/* set black */

	rgb_clr.red = 0.0;
	rgb_clr.green = 0.0;
	rgb_clr.blue = 0.0;
	set_color(black,&rgb_clr);

	/* set white */

	rgb_clr.red = 1.0;
	rgb_clr.green = 1.0;
	rgb_clr.blue = 1.0;
	set_color(white,&rgb_clr);

	/* set grey */

	rgb_clr.red = 0.3;
	rgb_clr.green = 0.3;
	rgb_clr.blue = 0.3;
	set_color(grey,&rgb_clr);

}  /* end init_screen_clrs */

static IDX freeclr = 1;			/* first unused entry in clr
					   tbl */

/*
 *  alloc_clr_tbl_entries
 *
 *  description:	allocates next n free entries in clr tbl
 *
 *  parameters:		n (int) - no of entries requested
 *
 *  returns:		(IDX) - index of first of n consecutive
 *				clr tbl entries
 */

IDX
alloc_clr_tbl_entries(n)
int n;					/* number of entries */
{
	Gwstables tbl_sz;

	/* make sure color table limit not exceeded */

	ginqmaxwssttables(ws_type,&tbl_sz);

	if ((freeclr + n) > (tbl_sz.color))
	{
		exit_error("alloc_clr_tbl_enties",5);
	}

	freeclr = freeclr + n;
	return(freeclr - n);
};  /* end alloc_clr_tbl_entries */


/*
 *  get_num_free_clrs
 *
 *  description:	returns number of free entries in color tbl
 *
 *  parameters:		none
 */

get_num_free_clrs()
{
	Gwstables tbl_sz;

	ginqmaxwssttables(ws_type,&tbl_sz);
	return(tbl_sz.color - freeclr);

}  /* end get_num_free_clrs */
