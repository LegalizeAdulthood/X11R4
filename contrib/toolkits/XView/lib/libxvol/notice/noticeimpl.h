/*	@(#)noticeimpl.h 20.19 89/08/18	*/

/* ------------------------------------------------------------------ */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/* ------------------------------------------------------------------ */

#include <xview/notice.h>
#include <xview/svrimage.h>

#define NOTICE_HELP		(NOTICE_TRIGGERED-1)
#define NOTICE_ACTION_DO_IT	'\015'

/* ------------------------------------------------------------------ */
/* -------------- opaque types and useful typedefs  ----------------- */
/* ------------------------------------------------------------------ */

struct notice {
    Xv_Window		client_window;
    Fullscreen		fullscreen;
    Xv_object		fullscreen_window;
    int			result;
    int			default_input_code;
    Event		*event;
    Xv_Font		notice_font;

    int			beeps;
    int			dont_beep;
    
    int			focus_x;
    int			focus_y;
    int			focus_specified;

    char		**message_items;

    int			number_of_buttons;
    struct notice_buttons *button_info;
    int			yes_button_exists;
    int			no_button_exists;
    char 		*help_data;
};

typedef struct notice	*notice_handle;

struct notice_buttons {
    char			*string;
    int				 value;
    int				 is_yes;
    int				 is_no;
    struct rect			 button_rect;
    struct notice_buttons	*next;
};

typedef struct notice_buttons	*notice_buttons_handle;
