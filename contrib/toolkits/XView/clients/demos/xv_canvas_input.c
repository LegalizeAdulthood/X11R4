/*
** -----------------------------------------------------------------------
** Name:	xv_canvas_input.c
**
** Description:
**
**	Demonstrates how to get keyboard and mouse events in an
**	canvas window.
**
**	Looks for keyboards, pointer movement and button press events
**	and displays the info in the canvas.
**
** -----------------------------------------------------------------------
**/

#include <stdio.h>
#include <xview/xview.h>
#include <xview/canvas.h>

Frame	frame;
Canvas	canvas;
Xv_Window canvas_pw;
char	kbd_msg[128],
	ptr_msg[128],
	but_msg[128];

/*
** -----------------------------------------------------------------------
** main()
**
**	Create a frame.
**
**	Create a canvas specifying a repaint procedure.
**
**	Get the paint window for the canvas and set the input
**	mask and the event procedure.
**
** -----------------------------------------------------------------------
*/
main(argc,argv)
int	argc;
char	*argv[];
{
	void	cv_event_proc(),cv_repaint();

	/*
 	**	Create windows
	*/
	xv_init(XV_INIT_ARGS,  	argc,argv,
                0);
	frame = xv_create(NULL,FRAME,
		0);
	canvas = xv_create(frame,CANVAS,
		CANVAS_REPAINT_PROC,	cv_repaint,
		0);
	canvas_pw = canvas_paint_window(canvas);

	/*
 	**	Set input mask
	*/
	xv_set(canvas_pw,
		WIN_CONSUME_EVENTS,
			WIN_ASCII_EVENTS, KBD_USE, KBD_DONE,
			LOC_MOVE, LOC_DRAG, LOC_WINENTER, LOC_WINEXIT,
			WIN_MOUSE_BUTTONS, 
			0,
		WIN_EVENT_PROC,	cv_event_proc,
		0);

	/*
 	**	Initial messages
	*/
	sprintf(kbd_msg,"Keyboard: key press events");
	sprintf(ptr_msg,"Pointer: pointer movement events");
	sprintf(but_msg,"Button: button press events");

	/*
 	**	Start event loop
	*/
	xv_main_loop(frame);
	return 0;
}

/*
** -----------------------------------------------------------------------
** cv_event_proc()
**
**	Called when an event is received in the canvas window.
**
**	Updates the keyboard, pointer and button message strings
**	and then calls cv_repaint() to paint them to the window.
**
** -----------------------------------------------------------------------
*/
void
cv_event_proc(window,event)
Xv_Window	window;
Event		*event;
{
	if (event_is_ascii(event)) {
		sprintf(kbd_msg,"Keyboard: key '%c' %d pressed at %d,%d",
			event_action(event),event_action(event),
			event_x(event),event_y(event));
	} 
	else {
		switch (event_action(event)) {
		case KBD_USE:
			sprintf(kbd_msg,"Keyboard: got keyboard focus");
			break;
		case KBD_DONE:
			sprintf(kbd_msg,"Keyboard: lost keyboard focus");
			break;
		case LOC_MOVE:
			sprintf(ptr_msg,"Pointer: moved to %d,%d",
				event_x(event),event_y(event));
			break;
		case LOC_DRAG:
			sprintf(ptr_msg,"Pointer: dragged to %d,%d",
				event_x(event),event_y(event));
			break;
		case LOC_WINENTER:
			sprintf(ptr_msg,"Pointer: entered window at %d,%d",
				event_x(event),event_y(event));
			break;
		case LOC_WINEXIT:
			sprintf(ptr_msg,"Pointer: exited window at %d,%d",
				event_x(event),event_y(event));
			break;
		case ACTION_SELECT:
		case MS_LEFT:
			sprintf(but_msg,"Button: Select (Left) at %d,%d",
				event_x(event),event_y(event));
			break;
		case ACTION_ADJUST:
		case MS_MIDDLE:
			sprintf(but_msg,"Button: Adjust (Middle) at %d,%d",
				event_x(event),event_y(event));
			break;
		case ACTION_MENU:
		case MS_RIGHT:
			sprintf(but_msg,"Button: Menu (Right) at %d,%d",
				event_x(event),event_y(event));
			break;
		default:
			break;
		}
	}
	cv_repaint(canvas,canvas_pw,(Rectlist *)NULL);
}

/*
** -----------------------------------------------------------------------
** cv_repaint()
**
**	Called to repaint the canvas in response to damage events
**	and the initial painting of the canvas window.
**	
**	Displays the keyboard, pointer and button message strings
**	after erasing the previous messages.
**
** -----------------------------------------------------------------------
*/
void
cv_repaint(canvas,paint_window,repaint_area)
Canvas	canvas;
Xv_Window	paint_window;
Rectlist	*repaint_area;
{
	int	cv_width = (int)xv_get(paint_window,XV_WIDTH),
		cv_height = (int)xv_get(paint_window,XV_HEIGHT);

	xv_rop(paint_window,0,0,cv_width,cv_height,PIX_CLR,NULL,0,0);
	xv_text(paint_window,25,25,PIX_SRC,NULL,kbd_msg,strlen(kbd_msg));
	xv_text(paint_window,25,50,PIX_SRC,NULL,ptr_msg,strlen(ptr_msg));
	xv_text(paint_window,25,75,PIX_SRC,NULL,but_msg,strlen(but_msg));
}
