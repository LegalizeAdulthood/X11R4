/*
**--------------------------------------------------------------------
** Name:        xv_canvas_menu.c
**
** Description:
**
**	Demonstrate the use of an XView menu in a canvas subwindow.
**
**	Menu is brought up with right mouse button and displays
**	choice made in the canvas.
**
**	Allow menu to be pinned.
**--------------------------------------------------------------------
*/

#include	<stdio.h>
#include	<xview/xview.h>
#include	<xview/canvas.h>

Frame	frame;
Canvas	canvas;
Xv_Window canvaspw;
Menu	menu;

/*
**--------------------------------------------------------------------
** main()
**
**	Create a frame, canvas and menu.  
**
**	A canvas receives input in its canvas_paint_window().
**
**	Specify creation of an Open Look Pushpin and transformation of
**	the menu to a pinned command window.
**
**	Each menu item specifies an action proc to be called when the
**	item is chosen, regardless of whether or not menu is pinned.
**
**--------------------------------------------------------------------
*/
main(argc,argv)
int	argc;
char	*argv[];
{
	Xv_opaque	my_event_proc(),my_action_proc();
	
	/*
 	**	Create windows
	*/
	xv_init(XV_INIT_ARGS,	argc,argv,
		0);
	frame = xv_create(NULL,FRAME,
		FRAME_LABEL,	"xv_canvas_menu",
		0);
	canvas = xv_create(frame,CANVAS,
		0);
	canvaspw = canvas_paint_window(canvas);

	/*
	**	Set input mask
	*/
	xv_set(canvaspw,
		WIN_CONSUME_EVENTS,	WIN_MOUSE_BUTTONS,0,
		WIN_EVENT_PROC,		my_event_proc,
		0);

	/*
	**	Create menu
	*/
	menu = xv_create(XV_NULL,MENU_COMMAND_MENU,
		MENU_GEN_PIN_WINDOW,	frame,"Test Menu",
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

	/*
 	**	Start event loop
	*/
	xv_main_loop(frame);
	return 0;
}

/*
**--------------------------------------------------------------------
** my_action_proc() Call xv_text() to display selected string in the canvas.
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
	xv_text(canvaspw,100,100,PIX_SRC,0,my_string,strlen(my_string));
}

/*
**--------------------------------------------------------------------
** my_event_proc() Call menu_show() to display menu on right mouse button push.
**--------------------------------------------------------------------
*/
Xv_opaque
my_event_proc(canvas,event)
Canvas	canvas;
Event *event;
{	
	if((event_id(event) == MS_RIGHT) && event_is_down(event))
		menu_show(menu,canvas,event,0);
}

