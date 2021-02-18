/*
**--------------------------------------------------------------------
**
** Name:	xv_panel_basic_items.c
**
** Description:
**
** 	This program demonstrates the basic usage of each of the
** 	Panel window items; buttons, choices, toggles, text, sliders,
** 	messages, and lists.  Each item is created with a notify proc.
**
**--------------------------------------------------------------------
**/

#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>

Frame	frame;
Panel	panel;
Panel_item message_item, button_item, choice_item, toggle_item, 
	slider_item, text_item, list_item;
void	button_notify(), choice_notify(), toggle_notify(), 
	slider_notify(), text_notify(), list_notify();
char	message_string[128];

/*
**--------------------------------------------------------------------
** main()
**
**	Create a frame and a panel.  Then create each of the
**	the basic panel item types, each item with a notify proc,
**	positioning each item stacked vertically.
**
**--------------------------------------------------------------------
*/
main(argc,argv)
int	argc;
char	*argv[];
{
	/*
 	**	Create the windows
	*/
	xv_init(XV_INIT_ARGS,	argc,argv,
		0);
	frame = xv_create(NULL,FRAME,
		FRAME_LABEL,	"xv_panel_basic_items",
		0);
	panel = xv_create(frame,PANEL,
		0);

	/*
 	**	Create the various items
	*/
	message_item = xv_create(panel,PANEL_MESSAGE,
		PANEL_ITEM_X,		xv_cols(panel,1),
		PANEL_ITEM_Y,		xv_rows(panel,1),
		PANEL_LABEL_STRING,	"Message: initial message",
		0);
	button_item = xv_create(panel,PANEL_BUTTON,
		PANEL_ITEM_X,		xv_cols(panel,1),
		PANEL_ITEM_Y,		xv_rows(panel,2),
		PANEL_LABEL_STRING,	"Button",
		PANEL_NOTIFY_PROC,	button_notify,
		0);
	choice_item = xv_create(panel,PANEL_CHOICE,
		PANEL_ITEM_X,		xv_cols(panel,1),
		PANEL_ITEM_Y,		xv_rows(panel,3),
		PANEL_LABEL_STRING,	"Choice",
		PANEL_NOTIFY_PROC,	choice_notify,
		PANEL_CHOICE_STRINGS, 	"One","Two","Three","Four","Five",0,
		0);
	toggle_item = xv_create(panel,PANEL_TOGGLE,
		PANEL_ITEM_X,		xv_cols(panel,1),
		PANEL_ITEM_Y,		xv_rows(panel,4),
		PANEL_LABEL_STRING,	"Toggle",
		PANEL_NOTIFY_PROC,	toggle_notify,
		PANEL_CHOICE_STRINGS, 	"One","Two","Three","Four","Five",0,
		0);
	slider_item = xv_create(panel,PANEL_SLIDER,
		PANEL_ITEM_X,		xv_cols(panel,1),
		PANEL_ITEM_Y,		xv_rows(panel,5),
		PANEL_LABEL_STRING,	"Slider",
		PANEL_NOTIFY_PROC,	slider_notify,
		PANEL_MIN_VALUE,	0,
		PANEL_MAX_VALUE,	10,
		0);
	text_item = xv_create(panel,PANEL_TEXT,
		PANEL_ITEM_X,		xv_cols(panel,1),
		PANEL_ITEM_Y,		xv_rows(panel,6),
		PANEL_LABEL_STRING,	"Text",
		PANEL_NOTIFY_PROC,	text_notify,
		PANEL_VALUE_DISPLAY_LENGTH, 20,
		0);
	list_item = xv_create(panel,PANEL_LIST,
		PANEL_ITEM_X,		xv_cols(panel,1),
		PANEL_ITEM_Y,		xv_rows(panel,7),
		PANEL_LABEL_STRING,	"List",
		PANEL_NOTIFY_PROC,	list_notify,
		PANEL_LIST_STRINGS, 	"One","Two","Three","Four","Five",0,
		0);


	/*
	**	Start the event loop
	*/
	window_fit(panel);
	window_fit(frame);
	xv_main_loop(frame);
	return 0;
}

/*
** -----------------------------------------------------------------------
** message()		Updates the panel message string
** -----------------------------------------------------------------------
*/
message(s)
char	*s;
{
	xv_set(message_item,
		PANEL_LABEL_STRING,	s,
		0);
}

/*
** -----------------------------------------------------------------------
** button_notify()	called when a button is pressed
** -----------------------------------------------------------------------
*/
void
button_notify(item,event)
Panel_item	item;
Event		*event;
{
	static int count = 0;

	sprintf(message_string,"Button: clicked %d times",++count);
	message(message_string);
}

/*
** -----------------------------------------------------------------------
** choice_notify()	called when a choice is made
** -----------------------------------------------------------------------
*/
void
choice_notify(item,value,event)
Panel_item	item;
int		value;
Event		*event;
{
	sprintf(message_string,"Choice: choice %d selected",value);
	message(message_string);
}

/*
** -----------------------------------------------------------------------
** toggle_notify()	called when a toggle is changed
** -----------------------------------------------------------------------
*/
void
toggle_notify(item,value,event)
Panel_item	item;
unsigned int	value;
Event		*event;
{
	sprintf(message_string,"Toggle: choice mask %d",value);
	message(message_string);
}

/*
** -----------------------------------------------------------------------
** slider_notify()	called when a slider is slid
** -----------------------------------------------------------------------
*/
void
slider_notify(item,value,event)
Panel_item	item;
int		value;
Event		*event;
{
	sprintf(message_string,"Slider: slid to value %d",value);
	message(message_string);
}

/*
** -----------------------------------------------------------------------
** text_notify()	called when return is pressed
** -----------------------------------------------------------------------
*/
void
text_notify(item,event)
Panel_item	item;
Event		*event;
{
	sprintf(message_string,"Text: entered %s",
		(char *)xv_get(item,PANEL_VALUE));
	message(message_string);
}

/*
** -----------------------------------------------------------------------
** list_notify()	called when a list item is selected
** -----------------------------------------------------------------------
*/
void
list_notify(item, op, event)
Panel_item	item;
Panel_list_op   op;
Event		*event;
{
	if (op == PANEL_LIST_OP_SELECT) {
		sprintf(message_string,"List: chose %s", 
			xv_get(item, PANEL_LABEL_STRING, 0));
		message(message_string);
	}
}
