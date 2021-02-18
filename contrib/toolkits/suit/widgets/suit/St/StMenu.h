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

#ifndef _StMenu_h
#define _StMenu_h

#include <St/StViewMgr.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 horizentalScrollbar Boolean		Boolean		False
 verticalScrollbar   Boolean		Boolean		False
 scrollBarOnTop	     Boolean		Boolean		False
 scrollBarOnRight    Boolean		Boolean		False
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 height		     Height		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0
 menuItems           MenuItems		Pointer		NULL
 font 		     Font 		String          XtDefaultFont
 attachTo 	     String 		Pointer		NULL
 select 	     Select		String 		<Btn1Up>
 inactivities	     Inactivities	Pointer 	NULL

*/

/* fields added to Box */
#define	StNmenuItems    	"stmenu_items"
#define StNmenuFont		"stmenu_font"
#define StNmenuCursor		"stmenu_cursor"
#define	StNattachTo    		"stattach_to"
#define	StNselect    		"stselect"
#define	StNinactivities    	"stinactivities"

#define	StCMenuItems    	"stmenu_items"
#define	StCAttachTo    		"stattach_to"
#define	StCSelect		"stselect"
#define	StCInactivities		"stinactivities"

extern WidgetClass stMenuWidgetClass;

typedef struct _StMenuClassRec	*StMenuWidgetClass;
typedef struct _StMenuRec	*StMenuWidget;

typedef struct {
	String label;
	XtCallbackProc proc;
	Boolean	line;
	Dimension height;
} StMenuItem, *StMenuItemList;

typedef struct {
	String item_name;
	Widget popup_shell;
} StParentMenuID;

#endif _StMenu_h
