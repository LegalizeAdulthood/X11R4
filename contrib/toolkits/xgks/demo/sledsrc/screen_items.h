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
 */

#include <xgks.h>
#include "changes.h"

#ifndef _screen_items_types
#define _screen_items_types

/*
 *  keys
 */

typedef char *AREA;			/* key for area */
typedef char *KEY;			/* screen_item key */

/*
 *  pop ups
 *
 *  each menu item has a list of its pop ups
 *  global popup table contains global popups and local
 *  popups associated with current menu item
 */

typedef struct popup POPUP;

struct popup
{
	KEY key;			/* unique key */	
	Gpoint extent[2];		/* extent[0] = (xmin,ymin)
					   extent[1] = (xmax,ymax) */
	POPUP *next;			/* next popup in tbl */
	BOOLEAN is_active;		/* true when popup is active */
};

extern POPUP *currpoptbl;		/* list of popups associated
					   with current menus and 
					   menu items */


/*
 *  menu structures
 *
 *  menu is stored in tree structure
 */

typedef struct menu MENU;
typedef struct menu_item MENU_ITEM;

struct menu_item
{
	int nlns;			/* nmbr of lns in item name */
	char **name;			/* name of menu item */
	KEY key;			/* unique key */	
	MENU *child;			/* child menu */
	POPUP *poplst;			/* local popup table */
};

struct menu
{
	KEY key;			/* unique key */	
	int nlns;			/* nmbr of lns in menu name */
	char **name;			/* name of menu */
	int noitems;			/* nmbr of menu items */
	struct menu_item *item;		/* list of menu items */
	MENU *parent;			/* parent menu */
	POPUP *poplst;			/* local popup table */
};

extern MENU_ITEM *currmitem;		/* current menu item */


extern POPUP *get_popup_from_pt();
extern KEY get_newmitem();

#endif
