/*
**--------------------------------------------------------------------
** Name:	xv_panel_buttons.c
**
** Category:    XView
**
** Subcategory: Panel
**
** Description:
**
**	Demonstrate four different variations of panel buttons:
**
**		Ordinary button
**		Menu Button 
**		Window Button
**		Inactive Button
**
**--------------------------------------------------------------------
*/
#include	<stdio.h>
#include	<xview/xview.h>
#include	<xview/panel.h>
#include	<xview/notice.h>

Frame	frame;
Panel	panel;
Panel_item	panel_message;
Menu	menu;

/*
**--------------------------------------------------------------------
** main -
**
**	Create a frame and panel containing 4 buttons and 1 message items.
**
**	Menu Button employs attribute PANEL_ITEM_MENU.
**
**	Window Button label has three dots (...) appended.
**
**	Inactive Button employs attribute PANEL_INACTIVE.
**
**--------------------------------------------------------------------
*/
main(argc,argv)
int	argc;
char	*argv[];
{
	Xv_opaque	notify_proc1(),my_action_proc(),
			notify_proc2(),notify_proc3();

	/*
 	**	Create the windows
	*/
	xv_init(XV_INIT_ARGS,	argc,argv,
		0);
	frame = xv_create(NULL,FRAME,
		FRAME_LABEL,	"xv_panel_buttons",
		0);
	panel = xv_create(frame,PANEL,
		PANEL_LAYOUT,	PANEL_VERTICAL,
		0);

	/*
 	**	Create the various button types and a menu.
	*/
	(void)xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Ordinary Button",
		PANEL_NOTIFY_PROC,	notify_proc1,
		0);
	menu = xv_create(XV_NULL,MENU_COMMAND_MENU,
		MENU_ITEM,
			MENU_STRING,	"Yes",
			MENU_ACTION_PROC,my_action_proc,
			0,
		MENU_ITEM,
			MENU_STRING,	"No",
			MENU_ACTION_PROC,my_action_proc,
			0,
		MENU_ITEM,
			MENU_STRING,	"Maybe",
			MENU_ACTION_PROC,my_action_proc,
			0,
		0);
	(void)xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Menu Button",
		PANEL_ITEM_MENU,	menu,
		0);
	(void)xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Window Button...",
		PANEL_NOTIFY_PROC,	notify_proc3,
		0);
	(void)xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Inactive Button",
		PANEL_INACTIVE,		TRUE,
		0);
	panel_message = xv_create(panel,PANEL_MESSAGE,
		0);

	/*
	**	Start the event loop
	*/
	xv_main_loop(frame);
	return 0;
}

/*
**--------------------------------------------------------------------
** my_action_proc() Called upon selection of menu item. 
**		    Updates panel message item.
**--------------------------------------------------------------------
*/
Xv_opaque
my_action_proc(menu,menu_item)
Menu	menu;
Menu_item	menu_item;
{
	char	my_string[81];

	sprintf(my_string,"selection is %s      ",
		(char *)menu_get(menu_item,MENU_STRING));
	xv_set(panel_message,PANEL_LABEL_STRING,my_string,0);
}

/*
**--------------------------------------------------------------------
** notify_proc1() Called when clicking on "Ordinary Button".
**		  Updates panel message item.
**--------------------------------------------------------------------
*/
Xv_opaque
notify_proc1(item,event)
Panel_item	item;
Event	*event;
{
	xv_set(panel_message,PANEL_LABEL_STRING,"No Message",0);
}

/*
**--------------------------------------------------------------------
** notify_proc3() Called when clicking on "Window Button".
**
**	The notice appears as a result of notice_prompt().
**	Here, the user must choose YES or NO before processing can continue.
**	The resulting choice is displayed in the panel message item.
**
**--------------------------------------------------------------------
*/
Xv_opaque
notify_proc3(item,event)
Panel_item	item;
Event	*event;
{
	int	result;

	/* 
	**	Bring up the notice and get a response
	*/
	result = notice_prompt(frame,event,
		NOTICE_MESSAGE_STRINGS,
			"This is your last chance to quit without changes.",
			"Press YES to confirm",
			"Press NO to cancel",
			0,
		NOTICE_BUTTON_YES,	"YES",
		NOTICE_BUTTON_NO	,"NO",
		0);

	/*
	**	Handle the result
	*/
	switch(result) {
	    case NOTICE_YES:	xv_set(panel_message,
					PANEL_LABEL_STRING,"Confirmed");
				break;
	    case NOTICE_NO:	xv_set(panel_message,
					PANEL_LABEL_STRING,"Cancelled");
				break;
	    case NOTICE_FAILED: fprintf(stderr,"unable to display pop-up.\n");
				break;
	    default:		fprintf(stderr,"unknown choice in pop-up.\n");
				break;
	};
}
