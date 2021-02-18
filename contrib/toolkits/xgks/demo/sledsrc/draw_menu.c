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
 *	display_pri_menu
 *	display_sec_menu
 *	display_menu_n_popups
 *	cleanup_menu_n_popups
 *	display_menu
 *	hilite
 *	unhilite
 */

static char rcsid[] = "$Header: draw_menu.c,v 4.0 89/09/01 15:53:12 amy Exp $";

#include "color.h"
#include "defs.h"
#include "key.h"
#include "screen_items.h"
#include "draw.h"
#include "menu.h"
#include "trans.h"
#include "functbl.h"
#include <xgks.h>

int max_num_menu_entries = 0;		/* max num of entries in menu
					   including header */

/*
 *  display_pri_menu
 *
 *  description:	displays menu in pri_menu_area
 *
 *  parameters:		none
 */

display_pri_menu()
{
	MENU *pri_menu;

	pri_menu = get_menu_from_area(PRI_MENU_AREA);
	if (find_trans(PRI_MENU_AREA) == NON_EXISTENT)
		alloc_trans(PRI_MENU_AREA);
	if (pri_menu != (MENU *) NULL)
		display_menu(PRI_MENU_AREA,pri_menu);

}  /* end display_pri_menu */


/*
 *  display_sec_menu
 *
 *  description:	displays menu in sec_menu_area
 *
 *  parameters:		none
 */

display_sec_menu()
{
	MENU *sec_menu;

	sec_menu = get_menu_from_area(SEC_MENU_AREA);
	if (find_trans(SEC_MENU_AREA) == NON_EXISTENT)
		alloc_trans(SEC_MENU_AREA);
	if (sec_menu != (MENU *) NULL)
		display_menu(SEC_MENU_AREA,sec_menu);

}  /* end display_sec_menu */


/*
 *  display_menu_n_popups
 *
 *  description:	displays menu as well as popups
 *			and sets pri_menu or sec_menu to menu
 *
 *  parameters:		area (AREA) - PRI_MENU_AREA or SEC_MENU_AREA
 *			menu (MENU *) - menu to display
 */

display_menu_n_popups(area,menu)
AREA area;
MENU *menu;
{
	POPUP *tbl_ptr;			/* ptr to popup list */
	IDX idx;			/* popup_func_tbl index */

	set_menu(area,menu);
	display_menu(area,menu);

	if (menu->poplst != (POPUP *) NULL)
	{
		add_to_currpoptbl(menu->poplst);
		if (screen_tbl[find_screen_tbl_idx(POPUP_AREA)].is_active
			== TRUE)
			for (tbl_ptr = menu->poplst;
				tbl_ptr != (POPUP *) NULL;
				tbl_ptr = tbl_ptr->next)
			{
				idx = find_popup_func_tbl_idx(tbl_ptr->key);
				(*(popup_func_tbl[idx].init))(tbl_ptr);
			}
	}

}  /* end display_menu_n_popups */


/*
 *  cleanup_menu_n_popups
 *
 *  description:	removes popups from currpoptbl,
 *			sets them to be inactive,
 *			and erases them from screen.
 *			also sets menu ptr (either
 *			pri_menu or sec_menu to NULL.
 *
 *  parameters:		area (AREA) - PRI_MENU_AREA or SEC_MENU_AREA
 *			menu (MENU *) - menu to cleanup
 */

cleanup_menu_n_popups(area,menu)
AREA area;
MENU *menu;
{
	POPUP *tbl_ptr;			/* ptr to popup list */
	IDX idx;			/* popup_func_tbl index */

	set_menu(area,(MENU *) NULL);

	rm_from_currpoptbl(menu->poplst);
	for (tbl_ptr = menu->poplst;
		tbl_ptr != (POPUP *) NULL;
		tbl_ptr = tbl_ptr->next)
	{
		idx = find_popup_func_tbl_idx(tbl_ptr->key);
		(*(popup_func_tbl[idx].cleanup))(tbl_ptr);
	}

}  /* end cleanup_menu_n_popups */
/*
 *  display_menu
 *
 *  description:	displays a menu
 *
 *  parameters:		area (AREA) - menu area (PRI_MENU_AREA or 
 *				SEC_MENU_AREA)
 *			menu_ptr (MENU) - menu to display
 */

display_menu(area,menu_ptr)
AREA area;				/* PRI_MENU_AREA or
					   SEC_MENU_AREA */
MENU *menu_ptr;				/* menu to display */
{
	Gpoint window[2];		/* area window */
	Gpoint box[4];			/* pts of rectangle */
	Gfloat ht;			/* ht of menu item */

	Gfloat top, bottom;		/* ymax and ymin of menu item */
	Gpoint center;			/* center of menu item */
	Gint i;

	/* erase old menu */

	erase_area(area);

	push_curr_trans();
	activate(area);
	set_aspect_flags(GBUNDLED);

	get_window(area,window);

	/* display menu items in white */

	ht = (window[MAX].y - window[MIN].y) /
		max_num_menu_entries;
	bottom = window[MAX].y;
	center.x = (window[MIN].x + window[MAX].x) / 2;

	/* draw menu name in white */

	top = bottom;
	bottom = top - ht;
	load_box(box,window[MIN].x,window[MAX].x,
		bottom,top);
	gsetfillind(WHITE_SOLID);
	gfillarea(4,box);

	/* center and display menu item name */

	center.y = (bottom + top) / 2;
	center_text(menu_ptr->nlns,
		menu_ptr->name,&center,black,
		MENU_TEXT);

	for (i=0; i< menu_ptr->noitems; i++)
	{
		/* outline menu item in white */

		top = bottom;
		bottom = top - ht;
		load_box(box,window[MIN].x,window[MAX].x,
			bottom,top);
		gsetfillind(GREY_SOLID);
		gfillarea(4,box);
		gsetfillind(WHITE_HOLLOW);
		gfillarea(4,box);

		/* center and display menu item name */

		center.y = (bottom + top) / 2;
		center_text(menu_ptr->item[i].nlns,
			menu_ptr->item[i].name,&center,white,
			MENU_TEXT);
	}

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();

}  /* end display_menu */


/*
 *  hilite
 *
 *  description:	hilite menu item identified by key
 *
 *  parameters:		key (KEY) - key to menu item
 */

hilite(key)
KEY key;				/* key of menu item to 
					   hilite */
{
	MENU *menu;			/* pri_menu or sec_menu */
	AREA area;			/* PRI_MENU_AREA or
					   SEC_MENU_AREA */
	Gpoint window[2];		/* booundary of area */
	Gpoint box[4];			/* pts of box */
	Gpoint center;			/* center of menu_item */
	Gfloat boxht;			/* height of menu item */
	Gfloat top;			/* top of menu item */
	Gint i;

	/* determine which menu contains menu_item */
	/* i will be the index of menu item in menu */

	find_menu_item_idx(key,&menu,&i);
	area = get_area_from_menu(menu);

	push_curr_trans();
	activate(area);
	set_aspect_flags(GBUNDLED);

	/* determine boundary of menu item */

	get_window(area,window);
	free(area);

	boxht = (window[MAX].y - window[MIN].y) / 
		max_num_menu_entries;
	top = window[MAX].y - (i + 1) * boxht;
	load_box(box,window[MIN].x,window[MAX].x,
		(top - boxht),top);
	gsetfillind(WHITE_SOLID);
	gfillarea(4,box);

	center.x = (window[MIN].x + window[MAX].x) / 2;
	center.y = top - 0.5 * boxht;
	center_text(menu->item[i].nlns,menu->item[i].name,&center,
		black,MENU_TEXT);

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();
}  /* end hilite */


/*
 *  unhilite
 *
 *  description:	unhilite menu item identified by key
 *
 *  parameters:		key (KEY) - key to menu item
 */

unhilite(key)
KEY key;				/* key of menu item to 
					   unhilite */
{
	MENU *menu;			/* pri_menu or sec_menu */
	AREA area;			/* PRI_MENU_AREA or
					   SEC_MENU_AREA */
	Gpoint window[2];		/* booundary of area */
	Gpoint box[4];			/* pts of box */
	Gpoint center;			/* center of menu_item */
	Gfloat boxht;			/* height of menu item */
	Gfloat top;			/* top of menu item */
	Gint i;

	/* determine which menu contains menu_item */
	/* i will be the index of menu item in menu */

	find_menu_item_idx(key,&menu,&i);
	area = get_area_from_menu(menu);

	push_curr_trans();
	activate(area);
	set_aspect_flags(GBUNDLED);

	/* determine boundary of menu item */

	get_window(area,window);
	free(area);

	boxht = (window[MAX].y - window[MIN].y) / 
		max_num_menu_entries;
	top = window[MAX].y - (i + 1) * boxht;
	load_box(box,window[MIN].x,window[MAX].x,
		(top - boxht),top);
	gsetfillind(GREY_SOLID);
	gfillarea(4,box);
	gsetfillind(WHITE_HOLLOW);
	gfillarea(4,box);

	center.x = (window[MIN].x + window[MAX].x) / 2;
	center.y = top - 0.5 * boxht;
	center_text(menu->item[i].nlns,menu->item[i].name,&center,
		white,MENU_TEXT);

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();
}  /* end unhilite */
