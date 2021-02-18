/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "func.h"
#include "font.h"

/*******************  imported global variables and procedures  **************/

extern int		figure_modified;
extern char		current_file[];

extern			put_msg();
extern			init_msg_receiving();
extern			read_file();
extern			edit_file();
extern			change_directory();
extern			save_current_file();
extern			save_file();
extern			save_and_exit();
extern			write_bitmap();

/********************  local variables  ***************************/

static MENUITEM pumenu_items[] = {
	{MENU_IMAGESTRING, "Undo",		(caddr_t) F_UNDO},
	{MENU_IMAGESTRING, "Redisplay",		(caddr_t) F_REDISPLAY},
	{MENU_IMAGESTRING, "Remove all",	(caddr_t) F_REMOVE_ALL},
	{MENU_IMAGESTRING, "Edit file ...",	(caddr_t) F_EDIT},
	{MENU_IMAGESTRING, "Save",		(caddr_t) F_SAVE},
	{MENU_IMAGESTRING, "Read file ...",	(caddr_t) F_READ},
	{MENU_IMAGESTRING, "Save in ...",	(caddr_t) F_SAVE_IN},
	{MENU_IMAGESTRING, "Status",		(caddr_t) F_STATUS},
	{MENU_IMAGESTRING, "Change Directory",	(caddr_t) F_CHDIR},
	/*
	{MENU_IMAGESTRING, "Print Directory",	(caddr_t) F_PWDIR},
	*/
	{MENU_IMAGESTRING, "Save & Exit",	(caddr_t) F_SAVE_N_EXIT},
	{MENU_IMAGESTRING, "Quit",		(caddr_t) F_QUIT},
	{MENU_IMAGESTRING, "Save as BITMAP ..",	(caddr_t) F_SAVE_BITMAP},
	};
static MENU		menu_body = {
				MENU_IMAGESTRING,
				"commands",
				sizeof(pumenu_items)/sizeof(MENUITEM),
				pumenu_items,
				(MENU *) NULL,
				(caddr_t) NULL
				};
static MENU		*menu_ptr = &menu_body;
static char quit_msg[] = "CONFIRM with LEFT button, CANCEL with RIGHT or MIDDLE button";
static char edit_msg[] = "Figures are modified; please CONFIRM with LEFT button or CANCEL with RIGHT button";

#ifdef	X11
static Arg	menu_args[] =
{
	{ XtNx, (XtArgVal)0 },
	{ XtNy, (XtArgVal)0 },
	{ XtNwidth, (XtArgVal)0 },
	{ XtNheight, (XtArgVal)0 },
};

extern void pane_select();

static XtCallbackRec	pane_callbacks[] =
{
	{ pane_select, NULL },
	{ NULL, NULL },
};

static Arg      pane_args[] =
{
        { XtNx, (XtArgVal)0 },
        { XtNy, (XtArgVal)0 },
        { XtNlabel, (XtArgVal)"        " },
        { XtNwidth, (XtArgVal)0 },
        { XtNheight, (XtArgVal)0 },
        { XtNcallback, (XtArgVal)pane_callbacks },
};

TOOL		menu;

int init_menu(tool)
	TOOL			tool;
{
	TOOL			panes, pane;
	register int		i, tlen, rlen = 0;
	register MENUITEM	*mi;
	XtTranslations		popdown_actions, pane_actions;

	menu = XtCreatePopupShell("popup_menu", overrideShellWidgetClass, tool,
		menu_args, XtNumber(menu_args));
	popdown_actions = XtParseTranslationTable(
		"<Btn3Up>:MenuPopdown()\n\
		<LeaveWindow>:MenuPopdown()\n");
	XtOverrideTranslations(menu, popdown_actions);
	panes = XtCreateManagedWidget("menu", boxWidgetClass, menu, menu_args,
		XtNumber(menu_args));
	pane_actions = XtParseTranslationTable(
		"<EnterWindow>:set()\n\
		<Btn3Up>:notify()unset()\n");
	for (i = 0; i < XtNumber(pumenu_items); i++)
	{
		mi = &pumenu_items[i];
		tlen = strlen(mi->label);
		if( tlen > rlen )
			rlen = tlen;
	}
	
	pane_args[3].value = char_width(canvas_font) * rlen + 10;

	pane_args[2].value = (XtArgVal)"COMMANDS";
	pane = XtCreateManagedWidget("title", labelWidgetClass,
				   panes, pane_args, 4);
	for (i = 0; i < XtNumber(pumenu_items); ++i)
	{
		mi = &pumenu_items[i];
		pane_args[2].value = (XtArgVal)mi->label;
		pane_callbacks[0].closure = (caddr_t)mi;
		pane = XtCreateManagedWidget("pane", commandWidgetClass,
			panes, pane_args, XtNumber(pane_args));
		XtOverrideTranslations(pane, pane_actions);
	}
	return (1);
}

set_popupmenu(event)
	INPUTEVENT	*event;
{
}

void pane_select(widget, mi)
	TOOL			widget;
	MENUITEM		*mi;
{
	int			menu_code = (int)mi->info;
#else

set_popupmenu(ie)
INPUTEVENT	  *ie;
{
	MENUITEM		*mi;
	char			 menu_code;
	int			 x, y;

	x = ie->ie_locx; y = ie->ie_locy;
        if ((mi = menu_display(&menu_ptr, ie, canvas_swfd)) == NULL) return;
	win_setmouseposition(canvas_swfd, x, y);    
        menu_code = (short) mi->mi_data;
#endif	X11
	switch(menu_code) {
	    case F_UNDO : 
		undo();
		break;
	    case F_READ :
		init_msg_receiving(read_file, "Read file : ");
		break;
	    case F_SAVE :
		(void)save_current_file();
		break;
	    case F_SAVE_IN : 
		if (no_object()) {
		    put_msg("No figure to save; ignored");
		    break;
		    }
		init_msg_receiving(save_file, "Save in file : ");
		break;
	    case F_REDISPLAY :
		redisplay_canvas();
		break;
	    case F_EDIT :
		if (!no_object() && figure_modified) {
		    if (wmgr_confirm(canvas_swfd, edit_msg) != -1) break;
		    }
		init_msg_receiving(edit_file, "Edit file : ");
		break;
	    case F_REMOVE_ALL :
		remove_all();
		put_msg("Immediate Undo will restore the figure.");
		redisplay_canvas();
		break;
	    case F_CHDIR :
		init_msg_receiving(change_directory, "Directory : ");
		break;
	    case F_STATUS :
		status();
		break;
	    case F_SAVE_N_EXIT :
		if (! figure_modified) quit();
		if (no_object()) quit();
		if (*current_file != 0) {
		    if (0 == write_file(current_file, 0)) quit();
		    }
		else {
		    blink_msg();
		    init_msg_receiving(save_and_exit, "Save in file : ");
		    }
		break;
	    case F_QUIT : 
		quit();
		break;
	    case F_SAVE_BITMAP : 
		if (no_object()) {
		    put_msg("No figure to save");
		    break;
		    }
		init_msg_receiving(write_bitmap, "Save bitmap to file : ");
		break;
	    }
#ifdef	X11
	XtPopdown(menu);
#endif	X11
	}

quit()
{
	if (no_object()) {
	    tool_destroy(tool);
	    exit(0);
	    }
	if (! figure_modified) {
	    tool_destroy(tool);
	    exit(0);
	    }
	if (wmgr_confirm(canvas_swfd, quit_msg) == -1) {
	    tool_destroy(tool);
	    exit(0);
	    }
	}
