#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_load.c 1.17 89/10/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Text load popup frame creation and support.
 */

#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>
#include <xview_private/ev_impl.h>
#include <sys/time.h>
#include <signal.h>
#include <xview/notice.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/openmenu.h>
#include <xview/wmgr.h>
#include <xview/pixwin.h>
#include <xview/win_struct.h>
#include <xview/win_screen.h>

#define		MAX_DISPLAY_LENGTH	50
#define   	MAX_STR_LENGTH		1024

#define HELP_INFO(s) XV_HELP_DATA, s,

typedef enum {
    FILE_CMD_ITEM = 0,
    DIR_STRING_ITEM = 1,
    FILE_STRING_ITEM = 2,
}               File_panel_item_enum;

extern Panel_item load_panel_items[];

Pkg_private Textsw_view_handle text_view_frm_p_itm();
Pkg_private Xv_Window frame_from_panel_item();

static int
do_load_proc(folio, ie)
    Textsw_folio    folio;
    Event          *ie;
{
    Textsw          textsw = FOLIO_REP_TO_ABS(folio);
    char           *dir_str, *file_str;
    Event           event;
    int             result;
    register int    locx, locy;
    Frame           popup_frame;
    char            curr_dir[MAX_STR_LENGTH];


    if (textsw_has_been_modified(textsw)) {
	result = notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
			       (Event *) 0,
			NOTICE_MESSAGE_STRINGS, "The text has been edited.",
		      "Load File will discard these edits. Please confirm.",
			       0,
			       NOTICE_BUTTON_YES, "Confirm, discard edits",
			       NOTICE_BUTTON_NO, "Cancel",
			       0);
	if (result == NOTICE_NO || result == NOTICE_FAILED)
	    return TRUE;
    }
    dir_str = (char *) panel_get_value(load_panel_items[(int) DIR_STRING_ITEM]);
    file_str = (char *) panel_get_value(
				      load_panel_items[(int) FILE_STRING_ITEM]);

    if AN_ERROR
	(ie == 0)
	    locx = locy = 0;
    else {
	locx = ie->ie_locx;
	locy = ie->ie_locy;
    }
    if (textsw_expand_filename(folio, dir_str, locx, locy)) {
	/* error handled inside routine */
	return TRUE;
    }
    if (textsw_expand_filename(folio, file_str, locx, locy)) {
	/* error handled inside routine */
	return TRUE;
    }
    /* if "cd" is not disabled */
    (void) getwd(curr_dir);
    if (strcmp(curr_dir, dir_str) != 0) {
	if (!(folio->state & TXTSW_NO_CD)) {
	    if (textsw_change_directory(folio, dir_str, FALSE, locx, locy) != 0) {
		/* error or directory does not exist */
		return TRUE;
	    }
	} else {
	    (void) notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
				 (Event *) 0,
			 NOTICE_MESSAGE_STRINGS, "Cannot change directory.",
				 "Change Directory Has Been Disabled.",
				 0,
				 NOTICE_BUTTON_YES, "Continue",
				 NOTICE_TRIGGER, ACTION_STOP,
				 0);
	    return TRUE;
	}
    }
    if (strlen(file_str) > 0) {
	result = textsw_load_file(textsw, file_str, TRUE, event.ie_locx, event.ie_locy);
	if (result == 0) {
	    (void) textsw_set_insert(folio, 0L);
	    popup_frame =
		frame_from_panel_item(load_panel_items[(int) FILE_CMD_ITEM]);
	    (void) xv_set(popup_frame, XV_SHOW, FALSE, 0);
	    return FALSE;
	}
	/* error */
	return TRUE;
    }
    (void) notice_prompt(FRAME_FROM_FOLIO_OR_VIEW(folio),
			 (Event *) 0,
		      NOTICE_MESSAGE_STRINGS, "No file name was specified.",
			 "Specify a file name to Load.",
			 0,
			 NOTICE_BUTTON_YES, "Continue",
			 NOTICE_TRIGGER, ACTION_STOP,
			 0);
    /* if we made it here, there was an error or notice */
    return TRUE;
}

static          Panel_setting
load_cmd_proc(item, event)
    Panel_item      item;
    Event          *event;
{
    Textsw_view_handle view = text_view_frm_p_itm(item);
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    int             error;

    if (item == load_panel_items[(int) FILE_CMD_ITEM]) {
	error = do_load_proc(folio, event);
	if (error) {
	    xv_set(item, PANEL_NOTIFY_STATUS, XV_ERROR, 0);
	}
	return PANEL_NONE;
    }
    return PANEL_NEXT;
}

static          Panel_setting
load_cmd_proc_accel(item, event)
    Panel_item      item;
    Event          *event;
{
    Textsw_view_handle view = text_view_frm_p_itm(item);
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    int             error;

    if (item == load_panel_items[(int) FILE_STRING_ITEM]) {
	error = do_load_proc(folio, event);
	if (error) {
	    xv_set(item, PANEL_NOTIFY_STATUS, XV_ERROR, 0);
	}
	return PANEL_NONE;
    }
    return PANEL_NEXT;
}

static void
create_load_items(panel, view)
    Panel           panel;
    Textsw_view_handle view;
{

    static char    *load_file = "Load File";
    char            load_string[MAX_STR_LENGTH];
    char            current_dir_load_string[MAX_STR_LENGTH];
    int             dummy;

    load_string[0] = NULL;
    (void) textsw_get_selection(view, &dummy, &dummy, load_string, MAX_STR_LENGTH);
    (void) getwd(current_dir_load_string);
    load_panel_items[(int) DIR_STRING_ITEM] = panel_create_item(panel, PANEL_TEXT,
						 PANEL_LABEL_X, ATTR_COL(0),
						 PANEL_LABEL_Y, ATTR_ROW(0),
			     PANEL_VALUE_DISPLAY_LENGTH, MAX_DISPLAY_LENGTH,
				  PANEL_VALUE_STORED_LENGTH, MAX_STR_LENGTH,
				       PANEL_VALUE, current_dir_load_string,
					   PANEL_LABEL_STRING, "Directory:",
					       HELP_INFO("textsw:dirstring")
								0);
    load_panel_items[(int) FILE_STRING_ITEM] = panel_create_item(panel, PANEL_TEXT,
						 PANEL_LABEL_X, ATTR_COL(5),
						 PANEL_LABEL_Y, ATTR_ROW(1),
			     PANEL_VALUE_DISPLAY_LENGTH, MAX_DISPLAY_LENGTH,
				  PANEL_VALUE_STORED_LENGTH, MAX_STR_LENGTH,
						   PANEL_VALUE, load_string,
						PANEL_LABEL_STRING, "File:",
					PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
						PANEL_NOTIFY_STRING, "\n\r",
				     PANEL_NOTIFY_PROC, load_cmd_proc_accel,
					      HELP_INFO("textsw:loadstring")
								 0);
    xv_set(panel, PANEL_CARET_ITEM,
	   load_panel_items[(int) FILE_STRING_ITEM], 0);

    load_panel_items[(int) FILE_CMD_ITEM] = panel_create_item(panel,
							      PANEL_BUTTON,
						PANEL_LABEL_X, ATTR_COL(26),
						 PANEL_LABEL_Y, ATTR_ROW(2),
					      PANEL_LABEL_STRING, load_file,
					   PANEL_NOTIFY_PROC, load_cmd_proc,
						HELP_INFO("textsw:loadfile")
							      0);
    (void) xv_set(panel,
		  PANEL_DEFAULT_ITEM, load_panel_items[(int) FILE_CMD_ITEM],
		  0);
}

extern          Panel
textsw_create_load_panel(frame, view)
    Frame           frame;
    Textsw_view_handle view;
{
    Panel           panel;

    panel = (Panel) xv_get(frame, FRAME_CMD_PANEL,
			   HELP_INFO("textsw:loadpanel")
			   0);
    (void) create_load_items(panel, view);

    return (panel);
}
