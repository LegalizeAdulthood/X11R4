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
 *	main
 *	init_gks
 *	close_down_gks
 *	redraw_all
 */

static char rcsid[] = "$Header: main.c,v 4.0 89/09/01 15:54:07 amy Exp $";

#include "changes.h"
#include <stdio.h>
#include <strings.h>
#include <xgks.h>
#include "key.h"
#include "screen_items.h"
#include "functbl.h"
#include "ws.h"

char *hostname;
WSTYPE ws_type;
WSID ws_id = 1;				/* ws id no */
WSID pic_wsid = 2;			/* pic ws id no */

extern int redraw_all();


/*
 *  main
 */

main(argc,argv)
int argc;
char *argv[];
{
	BOOLEAN picture_read;
	extern char *calloc();
	Gpoint pt;

	/* get ws_type */

	ws_type = argv[1];
	hostname = argv[1];
	
	/* initialize graphics package */

	init_gks();

	/* initialize all variables, colors and tables */

	init_screen_clrs();

	/* read in picture description from external file
	   if one was specified, otherwise read in standard
	   palette colors */

	/* It seems this feature is not implemented right now... */

	adjust_screen_tbl_viewports();
	init_trans_tbls();

	picture_read = FALSE;

	if (picture_read == FALSE)
		init_palette_clrs("palette_clrs");

	init_clr_area_clrs();
	init_grid();

	init_clr_selector_clrs();
	init_bundle_tbls();

	/* rig to get text to show up with new server */

	set_font("popup text key",1,GSTROKE);
	gsettextind(5);
	pt.x = 10000.0; pt.y = 10000.0;
	gtext(&pt,"text");
	set_font("popup text key",2,GSTROKE);

	/* draw initial screen area */

	draw_screen_background();
	(*(screen_tbl[find_screen_tbl_idx(PAL_AREA)].draw))();
	(*(screen_tbl[find_screen_tbl_idx(FGDCLR_AREA)].draw))();
	(*(screen_tbl[find_screen_tbl_idx(BGDCLR_AREA)].draw))();
	(*(screen_tbl[find_screen_tbl_idx(FILLCLR_AREA)].draw))();
	(*(screen_tbl[find_screen_tbl_idx(POPUP_AREA)].draw))();
	(*(screen_tbl[find_screen_tbl_idx(PICT_AREA)].draw))();
	(*(screen_tbl[find_screen_tbl_idx(MSG_AREA)].draw))();
	(*(screen_tbl[find_screen_tbl_idx(PRI_MENU_AREA)].draw))();
	(*(screen_tbl[find_screen_tbl_idx(SEC_MENU_AREA)].draw))();

	/* read in menu hierarchy */

	read_menu_file("menu");

	/* initialize locator */

	initialize_locator();

	/* all set to go */

	manage_input();

	/* close down */

	close_down_gks();
	exit(0);

}  /* end main */


/*
 *  init_gks
 *
 *  description:	initalizes gks, activates workstation,
 *			sets clipping indicator, and sets deferral
 *			mode to SURPRESSED.
 *
 *  parameters:		none
 */

init_gks()
{
	Gfloat xmax_dc,ymax_dc;
	Glimit win_lim,view_lim;

	/* initialize ws */

	gopengks(stderr,0);
	gopenws(ws_id,hostname,ws_type);
	gactivatews(ws_id);

	set_aspect_flags(GBUNDLED);

	/* set up workstation transformation */

	get_max_dc_coor(&xmax_dc,&ymax_dc);
	if (xmax_dc > ymax_dc)
	{
		win_lim.xmin = 0.0;
		win_lim.xmax = 1.0;
		win_lim.ymin = 0.0;
		win_lim.ymax = (ymax_dc/xmax_dc);
		gsetwswindow(ws_id,&win_lim);
	}
	else
	{
		win_lim.xmin = 0.0;
		win_lim.xmax = (xmax_dc/ymax_dc);
		win_lim.ymin = 0.0;
		win_lim.ymax = 1.0;
		gsetwswindow(ws_id,&win_lim);
	}
	view_lim.xmin = 0.0;
	view_lim.xmax = xmax_dc;
	view_lim.ymin = 0.0;
	view_lim.ymax = ymax_dc;
	gsetwsviewport(ws_id,&view_lim);

	/* turn off automatic redraw */

	gsetdeferst(ws_id,GASAP,GALLOWED);
	gsetclip(GCLIP);

	/* Set up escape features for redrawing window: */

	gescstoreprimi(ws_id,GNO);
	gescredrawnotify(ws_id,redraw_all);


}  /* end init_gks */


/*
 *  close_down_gks
 *
 *  description:	shuts down workstation and gks
 *
 *  parameters:		none
 */

close_down_gks()
{
	gdeactivatews(ws_id);
	gclosews(ws_id);
	gclosegks();
} /* end close_gks */


/*
 * redraw_all
 *
 *	Redraws all the menus and objects after window has been altered.
 *	Set to be the redraw function by "gescredrawnotify" in "init_gks."
 */

redraw_all()
{

	IDX idx;

	draw_screen_background();
        (*(screen_tbl[find_screen_tbl_idx(PAL_AREA)].draw))();
        (*(screen_tbl[find_screen_tbl_idx(FGDCLR_AREA)].draw))();
        (*(screen_tbl[find_screen_tbl_idx(BGDCLR_AREA)].draw))();
        (*(screen_tbl[find_screen_tbl_idx(FILLCLR_AREA)].draw))();
        (*(screen_tbl[find_screen_tbl_idx(POPUP_AREA)].draw))();
        (*(screen_tbl[find_screen_tbl_idx(PICT_AREA)].draw))();
        (*(screen_tbl[find_screen_tbl_idx(MSG_AREA)].draw))();
        (*(screen_tbl[find_screen_tbl_idx(PRI_MENU_AREA)].draw))();
        (*(screen_tbl[find_screen_tbl_idx(SEC_MENU_AREA)].draw))();

	idx = find_screen_tbl_idx(PICT_AREA);
        if (screen_tbl[idx].is_active == FALSE)
        {
                reprompt(1);
                return;
        }
	erase_area(PICT_AREA);
        (*(screen_tbl[idx].draw))();
} /* end redraw_all */

