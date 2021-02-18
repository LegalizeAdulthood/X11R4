#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_tool.c 20.15 89/11/02";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Initialization and finalization of text subwindows.
 */

#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>
#include <xview/notice.h>
#include <xview/frame.h>
#include <xview/wmgr.h>
#include <fcntl.h>
#define _NOTIFY_MIN_SYMBOLS
#include <xview/notify.h>
#undef _NOTIFY_MIN_SYMBOLS

#include <xview/win_struct.h>

extern Notify_error win_post_event();
Pkg_private Textsw_view_handle textsw_init_internal();

Pkg_private int
textsw_default_notify(abstract, attrs)
    Textsw          abstract;
    Textsw_attribute *attrs;
{
    register Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    register Frame  frame = xv_get(abstract, WIN_FRAME);

    for (; *attrs; attrs = textsw_attr_next(attrs)) {
	switch ((Textsw_action) (*attrs)) {
	  case TEXTSW_ACTION_TOOL_CLOSE:
	  case TEXTSW_ACTION_TOOL_MGR:
	  case TEXTSW_ACTION_TOOL_DESTROY:
	  case TEXTSW_ACTION_TOOL_QUIT:{
		switch ((Textsw_action) (*attrs)) {
		  case TEXTSW_ACTION_TOOL_CLOSE:
		    if (!xv_get(frame, FRAME_CLOSED))
			xv_set(frame, FRAME_CLOSED, TRUE, 0);
		    break;
		  case TEXTSW_ACTION_TOOL_MGR:{
			(void) win_post_event(frame,
				      (Event *) attrs[1], NOTIFY_IMMEDIATE);
			break;
		    }
		  case TEXTSW_ACTION_TOOL_QUIT:
		    if (textsw_has_been_modified(abstract)) {
			int             result;
			result = notice_prompt(
					       frame,
					       (Event *) 0,
					       NOTICE_MESSAGE_STRINGS,
					       "The text has been edited.",
					       " ",
			    "You may discard edits now and quit, or cancel",
					       "the request to Quit and go back and either save the",
			    "contents or store the contents as a new file.",
					       0,
				   NOTICE_BUTTON_YES, "Cancel, do NOT Quit",
			     NOTICE_BUTTON, "Discard edits, then Quit", 123,
					       NOTICE_TRIGGER, ACTION_STOP,
					       0);
			if ((result == ACTION_STOP) || (result == NOTICE_YES) || (result == NOTICE_FAILED)) {
			    break;
			} else {
			    (void) textsw_reset(abstract, 0, 0);
			    (void) textsw_reset(abstract, 0, 0);
			}
		    }
		    xv_destroy_safe(frame);
		    break;
		  case TEXTSW_ACTION_TOOL_DESTROY:
		    xv_set(frame, FRAME_NO_CONFIRM, TRUE, 0);
		    xv_destroy_safe(frame);
		    break;
		}
		break;
	    }
	  default:
	    break;
	}
    }
}
