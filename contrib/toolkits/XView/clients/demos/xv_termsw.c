/*
** -----------------------------------------------------------------------
** Name:	xv_termsw.c
**
** Category:	XView
**
** Subcategory:	Term
**
** Description:
**
**	Demonstrate incorporation of a Term subwindow in an application;
**	keyboard input to the termsw can come either directly to the
**	termsw or from an adjoining panel text item.
**
** -----------------------------------------------------------------------
*/
#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/tty.h>
#include <xview/termsw.h>

Frame	frame;
Tty	tty;
Panel	panel;
Panel_item	text_item;
Xv_opaque	notify_proc();

/*
**--------------------------------------------------------------------
** main -
**
**	Create a frame.
**
**	Create a panel containing a panel text item and panel button.
**	The intent is that the user enter a command to the text item;
**	pressing the button or hitting RETURN will call notify_proc(),
**	causing the string to be fed to the Term subwindow.
**
**	Create a Term subwindow.  
**
**--------------------------------------------------------------------
*/
main(argc,argv)
int	argc;
char	*argv[];
{

	xv_init(XV_INIT_ARGS,argc,argv,0);
	frame=xv_create(0,FRAME,0);
	panel=xv_create(frame,PANEL,0);
        text_item=xv_create(panel,PANEL_TEXT,
                PANEL_LABEL_STRING,"Command:",
                PANEL_NOTIFY_PROC,notify_proc,
                PANEL_VALUE_DISPLAY_LENGTH, 20,
                0);
	(void)xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,"Apply",
		PANEL_NOTIFY_PROC,notify_proc,
		0);
	window_fit_height(panel);
	tty=xv_create(frame,TERMSW,0);
	window_fit(frame);
	xv_main_loop(frame);
}


/*
**--------------------------------------------------------------------
** notify_proc - 
**
**	This procedure is called upon user hitting return on the
**	panel text item or clicking on the panel button.  Steps are:
**
**	Use xv_get() to ask the panel text item for its string value;
**	this should be cast to a char *.
**
**	After a newline has been appended to the string, feed it to
**	the Term subwindow with ttysw_input().
**
**	Clear the panel text item by resetting PANEL_VALUE with xv_set().
**--------------------------------------------------------------------
*/
Xv_opaque
notify_proc(item,event)
Panel_item	item;
Event	*event;
{
	char	str[81];
	
	sprintf(str,"%s\n",(char *)xv_get(text_item,PANEL_VALUE));
	ttysw_input(tty,str,strlen(str));
	xv_set(text_item,PANEL_VALUE,"",0);
}
