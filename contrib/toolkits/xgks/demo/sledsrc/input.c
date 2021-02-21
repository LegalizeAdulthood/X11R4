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
 * this file contains:
 *	initialize_locator
 *	select_pt
 */

#include "color.h"
#include "ws.h"
#include "defs.h"
#include "trans.h"
#include <xgks.h>

#define LOCATOR_DEV	1
#define LOCATOR_PROMPT	3

Gfloat xmax,ymax;	/* workstation viewport */

/*
 *  initialize_locator
 *
 *  description:	sets up echo area, echo type, prompt type
 *			and sets locator mode to request
 *
 *  parameters:		none
 */

initialize_locator()
{
	Gpoint pt;

	get_max_dc_coor(&xmax,&ymax);

	/* initialize locator device */

	pt.x = 0.5;
	pt.y = 0.5;

	/* Not needed with new xGKS: */
	/* locdata.ld_attr_flag = GCURRENT;
	locdata.ld_pline_fill = GPOLYLINE;
	locdata.ld_type_interior_asf = GINDIVIDUAL;
	locdata.ld_width_style_asf = GINDIVIDUAL;
	locdata.ld_color_asf = GINDIVIDUAL;
	locdata.ld_idx = 1;
	locdata.ld_type_interior = GSOLID;
	locdata.ld_color_idx = white;
	locdata.ld_style_idx = GSOLID;
	locdata.ld_width = 1.0; */ 


	init_locator(ws_id,LOCATOR_DEV,&pt,DEFAULT_TRANS,LOCATOR_PROMPT,
		0.0,xmax,0.0,ymax);
	/* This function is defined in dummies.c */

	gsetlocmode(ws_id,LOCATOR_DEV,GREQUEST,GNOECHO);
		
}  /* end initialize_locator */


/*
 *  select_pt
 *
 *  description:	returns next Gpoint selected by user as well as 
 *			transformation no used to convert to WC
 *
 *  parameters:		transno (IDX *) - trans used to convert pt to WC
 *			pt (Gpoint *) - pt selected in WC
 */

select_pt(transno,pt)
IDX *transno;				/* trans used to convert pt
					   to WC */
Gpoint *pt;				/* pt selected in WC */
{
	Gistat status;			/* GOK | GNONE */

	/* keep prompting for input until locator input is legit */

	gsetlocmode(ws_id,LOCATOR_DEV,GREQUEST,GECHO);

	req_locator(ws_id,LOCATOR_DEV,&status,transno,pt); /* Defined in dummies.c */
	while (status != GOK)
	{
		reprompt(1);
		req_locator(ws_id,LOCATOR_DEV,&status,transno,pt);
	}

	gsetlocmode(ws_id,LOCATOR_DEV,GREQUEST,GNOECHO);

	init_locator(ws_id,LOCATOR_DEV,pt,*transno,LOCATOR_PROMPT,
		0.0,xmax,0.0,ymax);	/* Defined in dummies.c */

}  /* end select_pt */
