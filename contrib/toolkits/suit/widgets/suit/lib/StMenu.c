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

#include <X11/IntrinsicP.h>
#include <X11/XawMisc.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <St/StwP.h>
#include <St/StViewMgrP.h>
#include <St/StMenuItmP.h>
#include <St/StMenuP.h>

#define  PopupFormPointer 	10
#define  DefaultSelection 	"<Btn1Up>"

#define offset(field) XtOffset(StMenuWidget,stmenu.field)
static XtResource resources[] = {
    {StNmenuItems, StCMenuItems, XtRPointer, sizeof(StMenuItemList),
	 offset(menu_items), XtRPointer, NULL},
    {StNmenuFont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	 offset(menu_font), XtRString, "XtDefaultFont"},
    {StNmenuCursor, XtCCursor, XtRCursor, sizeof(Cursor),
	 offset(menu_cursor), XtRString, (caddr_t)None},
    {StNattachTo, XtCString, XtRPointer, sizeof(StParentMenuID *),
	 offset(attach_to), XtRImmediate, (caddr_t)NULL},
    {StNselect, StCSelect, XtRString, sizeof(String),
	 offset(select), XtRString, (caddr_t)DefaultSelection},
    {StNinactivities, StCInactivities, XtRPointer, sizeof(String *),
         offset(inactivities), XtRImmediate, (caddr_t)NULL},
};
#undef offset

static void Initialize(), Destroy(), Realize();
static Boolean SetValues();
static XtGeometryResult PreferredGeometry();
static void Mapped(), Unmapped(), OutsideSet();

#define superclass	(&stViewManagerClassRec)
StMenuClassRec stMenuClassRec = {
  { /* core_class fields */
    /* superclass	  */	(WidgetClass) superclass,
    /* class_name	  */	"StMenu",
    /* widget_size	  */	sizeof(StMenuRec),
    /* class_initialize	  */	NULL,
    /* class_part_init    */    NULL,
    /* class_inited	  */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    NULL,
    /* realize		  */	Realize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	Destroy,
    /* resize		  */	XtInheritResize,
    /* expose		  */	XtInheritExpose,
    /* set_values	  */	SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus	  */	NULL,
    /* version            */    XtVersion,
    /* callback_private	  */	NULL,
    /* tm_table    	  */	NULL,
    /* query_geometry     */    PreferredGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager	  */	XtInheritGeometryManager,
    /* change_managed	  */	XtInheritChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },
  { /* StViewManagerClass fields */
    /* empty		  */	0
  },
  { /* StMenuClass fields */
    /* empty		  */	0
  }
};


  /* for public consumption */
WidgetClass stMenuWidgetClass = (WidgetClass)&stMenuClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static Boolean CheckSensitive(w, name)
	StMenuWidget w;
	String name;
{
	String *inactivities = w->stmenu.inactivities; 

	if (inactivities != (String *)NULL) {
		for (; *inactivities != (String)NULL; inactivities++) {
			if (strcmp(name, *inactivities) == 0) {
				/* Same stirng */
				/* This name is insensitive */
				return False;
			}
		}
	}

	/* This name is sensitive */
	return True;
}

static void AttachPopup_callback();

static Widget CreateMenuItem(parent, no, menu_item, total_height)
	StMenuWidget parent;
	int no;				/* menu item no. */
	StMenuItemList menu_item;	/* menu item info. */ 
	int total_height;		/* total count of menu item's height */
{
	Dimension width = parent->core.width;
	Dimension height = menu_item->height;
	String label = menu_item->label;
	XtCallbackProc callback = menu_item->proc;
	Boolean needs_line = menu_item->line;
	Boolean sensitive = CheckSensitive(parent, label);
	caddr_t closure = (caddr_t)no;
	Widget item;
	Arg menuItem_args[20];
	static XtCallbackRec submenuCallback[] = {
		{AttachPopup_callback, NULL},
		{NULL, NULL},
	};

	int n = 0;
	XtSetArg(menuItem_args[n], XtNx, 0), 				n++;
	XtSetArg(menuItem_args[n], XtNy, total_height), 		n++;
	XtSetArg(menuItem_args[n], XtNwidth, width), 			n++;
	XtSetArg(menuItem_args[n], XtNheight, height), 			n++;
	XtSetArg(menuItem_args[n], StNlineTop, 0), 			n++;
	XtSetArg(menuItem_args[n], StNlineBottom, (needs_line ? 1 : 0)),n++;
	XtSetArg(menuItem_args[n], StNlineRight, 0), 			n++;
	XtSetArg(menuItem_args[n], StNlineLeft, 0), 			n++;
	XtSetArg(menuItem_args[n], StNsubmenu, NULL), 			n++;
	XtSetArg(menuItem_args[n], StNfont, parent->stmenu.menu_font), 	n++;
	XtSetArg(menuItem_args[n], StNcursor, parent->stmenu.menu_cursor),
								 	n++;
	XtSetArg(menuItem_args[n], XtNsensitive, sensitive), 		n++;
	XtSetArg(menuItem_args[n], StNsubmenu, submenuCallback), 	n++;
	XtSetArg(menuItem_args[n], StNlabel, label), 			n++;

	item = XtCreateManagedWidget(label, stMenuItemWidgetClass, 
			(Widget)parent, menuItem_args, n);

	XtAddCallback(item, StNcallback, callback, closure);

	/* Add selection event */
	if (parent->stmenu.select != NULL) 
	{
		char newTranslation[100];
		sprintf(newTranslation, "%s: notify()", parent->stmenu.select); 
		XtOverrideTranslations
			(item, XtParseTranslationTable(newTranslation));
	}
	return item;
}

static Widget GetMyShell(w)
	Widget w;
{
	Widget parent = w;

	while (!XtIsShell(parent)) 
	{
		parent = XtParent(parent);
	}

	return parent;
}

static StMenuWidget GetTopMenu(w)
	StMenuWidget w;
{
	StMenuWidget parent = (StMenuWidget)w;

	for (; parent->stmenu.parent_menu != (Widget)NULL;)
		parent = (StMenuWidget)XtParent(parent->stmenu.parent_menu);
		
	return parent;
}

static Widget SearchWidget(w, name)
	Widget w;
	String name;
{
	Cardinal i;
	Widget target;
	XrmQuark q;

	q = XrmStringToQuark(name);

	if(w->core.xrm_name == q) return w;

	if(XtIsComposite(w)) {
		CompositeWidget cw = (CompositeWidget) w;
		for (i=0; i<cw->composite.num_children; i++) {
			target = SearchWidget(cw->composite.children[i], name);
			if(target != (Widget)NULL) return target;
		}
	}
	
	return (Widget)NULL;
}

static void AttachPopup_callback(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	StMenuItemWidget miw = (StMenuItemWidget)w;
	Widget popup = (Widget)closure;
	XEvent *event = (XEvent *)call_data;
	StMenuWidget parent = (StMenuWidget)XtParent(w);
	Widget parentShell = GetMyShell(parent);
	Widget popuped;

	popuped = parent->stmenu.popuped_menu;
	if (popuped != (Widget)NULL && popuped != popup) {
		XtPopdown(popuped);
		parent->stmenu.popuped_menu = (Widget)NULL;
		XGrabPointer(XtDisplay(parentShell), XtWindow(parentShell), 
			True,
			ButtonPressMask | ButtonReleaseMask | 
			EnterWindowMask | LeaveWindowMask | PointerMotionMask |
			KeyPressMask | KeyReleaseMask,
			GrabModeAsync, GrabModeAsync, NULL, NULL, CurrentTime);
	}
#define  IsInArrow(ev, widget)	(ev->xmotion.x > (widget->core.width * 3)/4)
	if (IsInArrow(event, w)) {
		if (popup != (Widget)NULL && popuped != popup) {
			XtPopup(popup, XtGrabNone);
			parent->stmenu.popuped_menu = popup;
			/*miw->menuitem.submenu_set = TRUE;*/
		}
	}
#undef  IsInArrow
}

static void AttachParentMenu(mw, attach)
	StMenuWidget mw;
	StParentMenuID *attach;
{
	static XtCallbackRec callback[] = {
		{NULL, NULL},
		{NULL, NULL},
	};
	Widget parent = XtNameToWidget(attach->popup_shell, 
				       attach->item_name);
	if (parent != (Widget)NULL) {
		Arg arg[2];
		int n;

		callback[0].callback = AttachPopup_callback;
		callback[0].closure = (caddr_t)mw->stmenu.my_shell;
		n = 0;
		XtSetArg(arg[n], StNsubmenu, (XtArgVal)callback), n++;
		XtSetArg(arg[n], StNarrow, (XtArgVal)TRUE), n++;
		XtSetValues(parent, arg, n);	
	}
	mw->stmenu.parent_menu = parent;	/* save parent menu item */
}

static void DetachParentMenu(w)
	StMenuWidget w;
{
	Widget parent = w->stmenu.parent_menu; 
	static XtCallbackRec callback[] = {
		{ NULL, NULL},
	};

	if (parent != (Widget)NULL) {
		Arg arg[2];
		int n;

		n = 0;
		XtSetArg(arg[n], StNsubmenu, (XtArgVal)callback), n++;
		XtSetArg(arg[n], StNarrow, (XtArgVal)FALSE), n++;
		XtSetValues( parent, arg, n);	
		w->stmenu.parent_menu = (Widget)NULL; 
	}
}

static void CreateAllMenuItems(w)
	StMenuWidget w;
{
	register StMenuItemList menu_items = w->stmenu.menu_items;
	register int i, label_count = 0;

	/* Count menu item */
	for (i = 0; menu_items[i].label != (String)NULL; i++) {
		label_count++;
	}

	/* Create menu items */
	if (label_count > 0) 
	{
		WidgetList items = 
		    (WidgetList)XtMalloc((label_count + 1) * sizeof(Widget));
		int i, height = 0;

		for (i = 0; i < label_count; i++) {
			items[i] = 
			    CreateMenuItem(w, i + 1, &menu_items[i], height);
			height += menu_items[i].height;
		}
		items[i] = (Widget)NULL;
		w->stmenu.item_list = items;
	}

	/* Attach to parent menu */
	if (w->stmenu.attach_to != (StParentMenuID *)NULL) {
		AttachParentMenu(w, w->stmenu.attach_to);
	}
}

static void DestroyAllMenuItems(w)
	StMenuWidget w;
{
	WidgetList items = w->stmenu.item_list;

	/* Destroy menu items */
	if (items != (WidgetList)NULL) 
	{
		register int i;
		for (i = 0; items[i] != (Widget)NULL; i++) {
			XtDestroyWidget(items[i]);
		}

		XtFree(items);
		w->stmenu.item_list = (WidgetList)NULL;
	}
}

#define  IsSubMenu(w)		(w->stmenu.attach_to != (StParentMenuID *)NULL)
static void MenuPopup_callback(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
        StMenuWidget mw = (StMenuWidget)closure;
	Window root, child;
	int root_x, root_y, x, y;
	unsigned int mask;

	if (XQueryPointer(XtDisplay(mw), RootWindowOfScreen(XtScreen(mw)), 
			&root, &child, &root_x, &root_y, &x, &y, &mask))
	{
		XtMoveWidget(mw->stmenu.my_shell, root_x + PopupFormPointer, 
						  root_y);
	}
}

/****************************************************************
 *
 * Class Procedures
 *
 ****************************************************************/


/* ARGSUSED */
static void Initialize(request, new)
	Widget request, new;
{
        StMenuWidget mw = (StMenuWidget)new;
	Widget shell = GetMyShell(mw);
	String select = (mw->stmenu.select == NULL) ? DefaultSelection : 
					       	      mw->stmenu.select; 
	char shellTranslations[100];
	static XtActionsRec actionsList[] = {
		{"mapped",	Mapped},
		{"unmapped",	Unmapped},
		{"outsideSet",	OutsideSet},
	};
	String myName = mw->core.name;

	/* Iinitialize private field */
	mw->stmenu.parent_menu = (Widget)NULL;
    	mw->stmenu.item_list = (WidgetList)NULL;
	mw->stmenu.my_shell = GetMyShell(mw);
	mw->stmenu.popuped_menu = (Widget)NULL;

	/* Create children */
	CreateAllMenuItems(mw);

	/* Move position of menu */
	XtAddCallback(mw->stmenu.my_shell, XtNpopupCallback, 
		      MenuPopup_callback, mw);
	
	/* Set shell's translation */
	sprintf(shellTranslations, "<Map>: mapped(%s) \n\
				    <Unmap>: unmapped(%s)\n\
				    %s: outsideSet(%s)",
				    myName, myName, select, myName);
	XtAddActions(actionsList, XtNumber(actionsList));
	XtOverrideTranslations(shell, 
			XtParseTranslationTable(shellTranslations));
}

/* ARGSUSED */
static void Destroy(w)
	Widget w;
{
        StMenuWidget mw = (StMenuWidget)w;

	if (mw->stmenu.item_list) {
		XtFree(mw->stmenu.item_list);
	}
}

static void Realize(widget, value_mask, attributes)
	Widget widget;
	XtValueMask *value_mask;
	XSetWindowAttributes *attributes;
{
	StMenuWidget mw = (StMenuWidget)widget;
	WidgetList items = mw->stmenu.item_list;

	(*superclass->core_class.realize)(widget, value_mask, attributes);

	if (items != (WidgetList)NULL && *items != (Widget)NULL) {
		register int i;
		for (i = 0; items[i] != (Widget)NULL; i++) 
		{
			XtRealizeWidget(items[i]);
		}
	}
}

/* ARGSUSED */
static Boolean SetValues(current, request, new)
	Widget current, request, new;
{
	StMenuWidget w = (StMenuWidget)new;
	StMenuWidget oldw = (StMenuWidget)current;
	Boolean remake = False;

	if (!remake && oldw->stmenu.menu_items != w->stmenu.menu_items) 
	{ 
		WidgetList items;

		DetachParentMenu(w);
		DestroyAllMenuItems(w);
		CreateAllMenuItems(w);

		items = w->stmenu.item_list;
		if (items != (WidgetList)NULL && *items != (Widget)NULL) {
			register int i;
			for (i = 0; items[i] != (Widget)NULL; i++) 
			{
				XtRealizeWidget(items[i]);
			}
		}

		remake = True;
	}
	if (!remake && oldw->stmenu.menu_font != w->stmenu.menu_font) 
	{
		WidgetList itemP = w->stmenu.item_list;


		for (; *itemP != (Widget)NULL; itemP++) {
			Arg arg[1];
			XtSetArg(arg[0], StNfont, w->stmenu.menu_font);
			XtSetValues(*itemP, arg, 1);	
		}
	}
	if (!remake && oldw->stmenu.menu_cursor != w->stmenu.menu_cursor) 
	{
		WidgetList itemP = w->stmenu.item_list;


		for (; *itemP != (Widget)NULL; itemP++) {
			Arg arg[1];
			XtSetArg(arg[0], StNcursor, w->stmenu.menu_cursor);
			XtSetValues(*itemP, arg, 1);	
		}
	}
	if (oldw->stmenu.select != w->stmenu.select) 
	{
		WidgetList itemP = w->stmenu.item_list;
		String select = (w->stmenu.select == NULL) ? 
				DefaultSelection : w->stmenu.select; 
		String myName = w->core.name;
		Widget shell = GetMyShell(w);
		char shellTranslations[100];

		if (!remake && w->stmenu.select != NULL) 
			for (; *itemP != (Widget)NULL; itemP++) {
			{
				char newTranslation[100];
				sprintf(newTranslation, "%s: notify()", 
				   		w->stmenu.select); 
				XtOverrideTranslations(*itemP, 
				    XtParseTranslationTable(newTranslation));

			}
		}
		sprintf(shellTranslations, "<Map>: mapped(%s) \n\
					    <Unmap>: unmapped(%s)\n\
					    %s: outsideSet(%s)",
					    myName, myName, select, myName);
		XtOverrideTranslations(shell, 
				XtParseTranslationTable(shellTranslations));
	}
	if (!remake && oldw->stmenu.attach_to != w->stmenu.attach_to) 
	{
		DetachParentMenu(w);
		AttachParentMenu(w, w->stmenu.attach_to);
	}
	if (!remake && oldw->stmenu.inactivities != w->stmenu.inactivities)
	{
		WidgetList itemP = w->stmenu.item_list;

		for (; *itemP != (Widget)NULL; itemP++) {
			Arg arg[1];
			Boolean sensitive = CheckSensitive(w, (*itemP)->core.name);
			XtSetArg(arg[0], XtNsensitive, sensitive);
			XtSetValues(*itemP, arg, 1);	
		}
	}

	return False;
}

static XtGeometryResult PreferredGeometry(w, constraints, reply)
    Widget w;
    XtWidgetGeometry *constraints, *reply;
{
	return XtGeometryYes;
}

/************************************************************************
 * 
 * Action Procedures
 *
 ***********************************************************************/

/* ARGSUSED */
static void Mapped(w, event, params, num_params)
	Widget w;
	XEvent *event;
	String *params;
	Cardinal *num_params;
{
	XGrabPointer(XtDisplay(w), XtWindow(w), True, 
			ButtonPressMask | ButtonReleaseMask | 
			EnterWindowMask | LeaveWindowMask | PointerMotionMask |
			KeyPressMask | KeyReleaseMask,
			GrabModeAsync, GrabModeAsync, NULL, NULL, CurrentTime);
}

/* ARGSUSED */
static void Unmapped(w, event, params, num_params)
	Widget w;
	XEvent *event;
	String *params;
	Cardinal *num_params;
{
	StMenuWidget menu;

	if (*num_params != 0) {
		menu = (StMenuWidget)SearchWidget(w, params[0]);
		if (menu->stmenu.popuped_menu != (Widget)NULL) {
			XtPopdown(GetMyShell(menu->stmenu.popuped_menu));
			menu->stmenu.popuped_menu = (Widget)NULL;
		}
	}
}

static void OutsideSet(w, event, params, num_params)
	Widget w;
	XEvent *event;
	String *params;
	Cardinal *num_params;
{
	Widget menu, topMenu;

	if(*num_params != 0) {
		menu = SearchWidget(w, params[0]);
		topMenu = (Widget)GetTopMenu(menu);
		XtPopdown(GetMyShell(topMenu));
	}
}
