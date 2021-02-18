/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#ifndef _StMenuP_h
#define _StMenuP_h

#include <St/StMenu.h>

typedef struct {int empty;} StMenuClassPart;

typedef struct _StMenuClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    StViewManagerClassPart  stview_manager_class;
    StMenuClassPart  stmenu_class;
} StMenuClassRec;

extern StMenuClassRec stMenuClassRec;

typedef struct _StMenuPart {
    /* resources */
    StMenuItemList menu_items;	/* menu item */
    XFontStruct *menu_font;	/* font of menu item name */
    Cursor menu_cursor;		/* cursor of menu item */
    StParentMenuID *attach_to;	/* parent menu item to be attached */
    String select;		/* menu selection event */
    String *inactivities;	/* this menu item is inactive */

    /* private state */
    WidgetList item_list;	/* menu item widgets */ 
    Widget parent_menu;		/* parent menu items to be attached */
    Widget my_shell;		/* my popup shell */
    Widget popuped_menu;	/* submenu poped up just now */
} StMenuPart;

typedef struct _StMenuRec {
    CorePart	core;
    CompositePart	composite;
    StViewmanagerPart	stview_manager;
    StMenuPart	stmenu;
} StMenuRec;

#endif _StMenuP_h
