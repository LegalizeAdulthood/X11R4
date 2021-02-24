/* 
 *              Copyright IBM Corporation 1989
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
 * photodummy.c gives dummy definitions of the functions in photo.c when
 * they are not to be used - that is, when the photograph features are not
 * implemented.
 *
 * These functions are:
 *      photo
 *      four_by_five
 *      eight_by_ten
 *      konica_ft1
 *      one_to_one
 */

#include "photo_func.h"

#include "draw_menu.h"
#include "prompt.h"
#include "screen_items.h"

void photo(menu_item)
    MENU_ITEM *menu_item;
{
    hilite(menu_item->key);
    reprompt(0);
    unhilite(menu_item->key);
}

void four_by_five(menu_item)
    MENU_ITEM *menu_item;
{
}

void eight_by_ten(menu_item)
    MENU_ITEM *menu_item;
{
}

void konica_ft1(menu_item)
    MENU_ITEM *menu_item;
{
}

void one_to_one(MENU_ITEM *menu_item)
{
}
