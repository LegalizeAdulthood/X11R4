/*
**--------------------------------------------------------------------
** Name:	xv_notice_button.c
**
** Description:
**
**	Demonstrate the use of notices.
**
**	Click on the panel button to bring up a notice; resulting
**	choice is displayed in a panel message item.
**
**--------------------------------------------------------------------
*/
#include	<stdio.h>
#include	<xview/xview.h>
#include	<xview/panel.h>
#include	<xview/notice.h>

Frame	frame;
Panel	panel;
Panel_item	panel_button,panel_message;

/*
**--------------------------------------------------------------------
** main()
**
**	Create a frame and panel containing button and message items.
**
**	Pushing the button calls my_notify_proc().
**	
**--------------------------------------------------------------------
*/

main(argc,argv)
int	argc;
char	*argv[];
{
	Xv_opaque	my_notify_proc();

	/*
 	**	Create the windows
	*/
	xv_init(XV_INIT_ARGS,	argc,argv,
		0);
	frame = xv_create(NULL,FRAME,
		FRAME_LABEL,	"xv_notice_button",
		0);
	panel = xv_create(frame,PANEL,
		PANEL_LAYOUT,	PANEL_VERTICAL,
		0);
	panel_button = xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Commit...",
		PANEL_NOTIFY_PROC,	my_notify_proc,
		0);
	panel_message = xv_create(panel,PANEL_MESSAGE,0);

	/*
	**	Start the event loop
	*/
	xv_main_loop(frame);
	return 0;
}

/*
**--------------------------------------------------------------------
** my_notify_proc()
**
**	The notice appears as a result of notice_prompt().
**
**	Here, the user must chooses YES or NO before processing can continue.
**
**	The resulting choice is displayed in the panel message item.
**
**--------------------------------------------------------------------
*/
Xv_opaque
my_notify_proc(item,event)
Panel_item	item;
Event	*event;
{
	int	result;

	/*
 	**	Create the notice and get a response
	*/
	result = notice_prompt(panel,NULL,
		NOTICE_MESSAGE_STRINGS,
			"This is your last chance to quit without changes.",
			"Press YES to confirm",
			"Press NO to cancel",
			0,
		NOTICE_BUTTON_YES,	"YES",
		NOTICE_BUTTON_NO,	"NO",
		0);

	/* 
	**	Handle the result
	*/
	switch(result) {
	    case NOTICE_YES:	xv_set(panel_message,
					PANEL_LABEL_STRING,"Confirmed", 0);
				break;
	    case NOTICE_NO:	xv_set(panel_message,
					PANEL_LABEL_STRING,"Cancelled", 0);
				break;
	    case NOTICE_FAILED: fprintf(stderr,"unable to display pop-up.\n");
				break;
	    default:		fprintf(stderr,"unknown choice in pop-up.\n");
				break;
	};
}
