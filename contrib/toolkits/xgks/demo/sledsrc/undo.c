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
 *	initialize_undo
 *	log_last_area
 *	log_last_menu_item
 *	area_undo
 *	menu_item_undo
 *	check_undo_flag
 *	test_undo_flag
 */

static char rcsid[] = "$Header: undo.c,v 4.0 89/09/01 15:55:52 amy Exp $";

#include "defs.h"
#include <strings.h>
#include "functbl.h"
#include "screen_items.h"
#include <xgks.h>
#include "changes.h"


static AREA last_action_area = (AREA) NULL;
static KEY last_action_menu_item = (KEY) NULL;
static IDX last_transno;
static Gpoint last_pt;
static BOOLEAN undo_flag;
static BOOLEAN undo_mode; /* ON = 1, OFF = 0 */


/*
 *  initialize_undo
 *
 *  description:	initializes all variables associated
 *			with undo
 *
 *  parameters:		none
 */

initialize_undo()
{
	if (last_action_area != (AREA) NULL)
	{
		free(last_action_area);
		last_action_area = (AREA) NULL;
	}
	if (last_action_menu_item != (KEY) NULL)
	{
		free(last_action_menu_item);
		last_action_menu_item = (KEY) NULL;
	}
		undo_flag = FALSE;
		undo_mode = OFF;
}  /* end_initialize_undo */


/*
 *  log_last_area
 *
 *  description:	logs last action
 *
 *  parameters:		(AREA) - area
 *			(IDX) - transno
 *			(Gpoint) - pt
 */

log_last_area(area,transno,pt)
AREA area;
IDX transno;
Gpoint pt;
{
	if (!(eq(last_action_area,area) &&
		(last_transno == transno) &&
		(last_pt.x == pt.x)  &&
		(last_pt.y == pt.y)))
	{
		if (!(eq(last_action_area,((AREA) NULL))))
			free(last_action_area);
		last_action_area = (AREA) calloc((unsigned)
			(strlen(area) + 1), sizeof(char));
		(void) strcpy(last_action_area,area);
		last_transno = transno;
		last_pt = pt;
	}
}  /* end log_last_area */


/*
 *  log_last_menu_item
 *
 *  description:	logs last action
 *
 *  parameters:		(MENU_ITEM) - menu item
 *			(Gpoint) - pt
 */

log_last_menu_item(key,pt)
KEY key;
Gpoint pt;
{
	if (!(eq(last_action_menu_item,key) &&
		(last_pt.x == pt.x)  &&
		(last_pt.y == pt.y)))
	{
		if (!(eq(last_action_menu_item,((KEY) NULL))))
			free(last_action_menu_item);
		last_action_menu_item = (KEY) calloc((unsigned)
			(strlen(key) + 1), sizeof(char));
		(void) strcpy(last_action_menu_item,key);
		last_pt = pt;
	}
}  /* end log_last_menu_item */


/*
 *  area_undo
 *
 *  description:	undoes last action by setting undo_mode
 *			to ON and toggling undo_flag
 *			and calls appropriate area
 *
 *  parameters:		menu_item (MENU_ITEM *) - unused
 */

area_undo(menu_item)
MENU_ITEM *menu_item;
{
	undo_mode = ON;
	undo_flag = ((undo_flag == FALSE) ? TRUE : FALSE);
	if (!eq(last_action_area,(AREA) NULL))
	{
		screen_tbl[find_screen_tbl_idx(last_action_area)].exec(
			last_action_area,last_transno,last_pt);
	}
	else
		reprompt(1);
	undo_mode = OFF;
}  /* end area_undo */


/*
 *  menu_item_undo
 *
 *  description:	undoes last action by setting undo_mode
 *			to ON and toggling undo_flag
 *			and calls appropriate menu_item
 *
 *  parameters:		menu_item (MENU_ITEM *) - unused
 */

menu_item_undo(menu_item)
MENU_ITEM *menu_item;
{
	undo_mode = ON;
	undo_flag = ((undo_flag == FALSE) ? TRUE : FALSE);
	if (!eq(last_action_menu_item,(KEY) NULL))
	{
		(*(menu_item_func_tbl[find_menu_item_func_tbl_idx(
			last_action_menu_item)].exec))(last_pt);
	}
	else
		reprompt(1);
	undo_mode = OFF;
}  /* end menu_item_undo */


/*
 *  check_undo_flag
 *
 *  description:	sets undo flag to FALSE if undo
 *			menu option has not been picked
 *
 *  parameters:		none
 */

check_undo_flag()
{
	if (undo_mode == OFF)
	{
		undo_flag = FALSE;
	}
}  /* end check_undo_flag */


/*
 *  test_undo_flag
 *
 *  description:	returns value of undo_flag
 *
 *  parameters:		none
 *
 *  returns:		(BOOLEAN) - undo_flag
 */

BOOLEAN
test_undo_flag()
{
	return(undo_flag);
}  /* end test_undo_flag */
