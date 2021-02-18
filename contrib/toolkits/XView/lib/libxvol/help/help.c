#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)help.c 1.40 89/10/17";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/notice.h>
#include <xview/canvas.h>
#include <xview/cursor.h>
#include <xview/notify.h>
#include <xview/panel.h>
#include <xview/svrimage.h>
#include <xview/scrollbar.h>
#include <xview/textsw.h>
#include <xview_private/draw_impl.h>

#define HELPNAMELENGTH 64
/*
 * There is a maximum of 10 lines of text of 50 chars each visible in the
 * help text subwindow.  If the help text exceeds 10 lines, a scrollbar is
 * shown.
 */
#define HELPTEXTCOLS 50
#define HELPTEXTLINES 10
#define HELP_CANVAS_MARGIN 10
#define HELP_IMAGE_X 35
#define HELP_IMAGE_Y 5
#define HELP_IMAGE_WIDTH 80
#define HELP_IMAGE_HEIGHT 73

static short    mglass_image[] = {
#include <images/mglass.icon>
};

mpr_static(mglass_pr, 199, 132, 1, mglass_image);

static short    mglass_mask_image[] = {
#include <images/mglass_mask.icon>
};

mpr_static(mglass_mask_pr, 199, 133, 1, mglass_mask_image);

Pkg_private char *xv_help_get_arg();
Pkg_private char *xv_help_get_text();

static char     image_saved = FALSE;
static Xv_Cursor busy_pointer;
static Frame    help_frame = NULL;
static Panel    help_panel;
static Panel_item help_panel_msg;
static Server_image help_image;
static Scrollbar help_textsw_sb;
static Textsw   help_textsw;


static void
help_request_failed(window, data, str, why, event)
    Xv_Window       window;
    char           *data, *str, *why;
    Event          *event;
{
    char            message[256];
    Xv_Window	    notice_window;

    if (data)
	sprintf(message, "%s for %s.", str, data);
    else
	sprintf(message, "%s.", str);
    notice_window = xv_get(window, WIN_FRAME);
    if (!notice_window)
	notice_window = window;   /* No frame: must be top level window */
    notice_prompt(notice_window, event,
    /* NOTICE_POSITION, NOTICE_SCREEN_CENTERED, */
		  NOTICE_MESSAGE_STRINGS, message, 0,
		  NOTICE_BUTTON_YES, "OK", 0,
		  XV_HELP_DATA, why,
		  0);
}


static          Notify_value
help_frame_destroy_proc(client, status)
    Notify_client   client;
    Destroy_status  status;
{
    if (status != DESTROY_CHECKING && help_image) {
	xv_destroy(help_image);
	help_image = 0;
    }
    return (notify_next_destroy_func(client, status));
}


Xv_private void
xv_help_save_image(pw, client_width, client_height, mouse_x, mouse_y)
    Pixwin         *pw;
    int             client_width, client_height;
    int             mouse_x, mouse_y;	/* offset of mouse pointer in pixwin */
{
    int		    dst_x, dst_y;
    int             src_x, src_y;
    int             image_width, image_height;
    Xv_Drawable_info *info;

    if (mouse_x > client_width || mouse_y > client_height)
	/*
	 * Pointer is outside of event window: probably operating with help
	 * event going to window with input focus.
	 */
	return;			/* image_saved = FALSE */

    if (!help_image) {
	DRAWABLE_INFO_MACRO((Xv_opaque) pw, info);
	/* Create a remote image for magnifying glass with help target image */
	help_image = xv_create(0, SERVER_IMAGE,
			       XV_WIDTH, mglass_pr.pr_width,
			       XV_HEIGHT, mglass_pr.pr_height,
			       SERVER_IMAGE_DEPTH, xv_depth(info),
			       0);
    }
    xv_rop(help_image, HELP_IMAGE_X, HELP_IMAGE_Y, HELP_IMAGE_WIDTH,
	   HELP_IMAGE_HEIGHT, PIX_CLR, 0, 0, 0);
    /* Store copy of Frame Buffer pixels into magnifying glass circle */
    src_x = mouse_x - HELP_IMAGE_WIDTH / 2;
    if (src_x < 0)
	src_x = 0;
    if (src_x + HELP_IMAGE_WIDTH > client_width) {
	image_width = client_width - src_x;
	dst_x = HELP_IMAGE_X + (HELP_IMAGE_WIDTH-image_width)/2;
    } else {
	image_width = HELP_IMAGE_WIDTH;
	dst_x = HELP_IMAGE_X;
    }
    src_y = mouse_y - HELP_IMAGE_HEIGHT / 2;
    if (src_y < 0)
	src_y = 0;
    if (src_y + HELP_IMAGE_HEIGHT > client_height) {
	image_height = client_height - src_y;
	dst_y = HELP_IMAGE_Y + (HELP_IMAGE_HEIGHT-image_height)/2;
    } else {
	image_height = HELP_IMAGE_HEIGHT;
	dst_y = HELP_IMAGE_Y;
    }
    xv_rop(help_image, dst_x, dst_y, image_width, image_height, PIX_SRC,
	   pw, src_x, src_y);
    image_saved = TRUE;
}


Xv_private int
xv_help_render(client_window, client_data, client_event)
    Xv_Window       client_window;
    caddr_t         client_data;
    Event          *client_event;
{
    char           *arg = xv_help_get_arg(client_data);
    char           *text = (arg ? xv_help_get_text() : NULL);
    char            client_name[80];
    int             i;
    int             length;
    int             x, y;
    Frame           base_frame;
    Frame           client_frame;
    Xv_Cursor       current_pointer;
    Xv_object       screen;
    Xv_object       server;

    if (!text) {
	help_request_failed(client_window, client_data, "No help is available",
			    "xview:helpnoinfo", client_event);
	return XV_ERROR;
    }
    if (!image_saved) {
	help_request_failed(client_window, NULL,
			    "No help is available on this window",
			    "xview:helpnoinfo", client_event);
	return XV_ERROR;
    }
    image_saved = FALSE;	/* reset flag */

    /* Change to busy pointer */
    if (!busy_pointer) {
	screen = xv_get(client_window, XV_SCREEN);
	server = xv_get(screen, SCREEN_SERVER);
	busy_pointer = xv_get(server, XV_KEY_DATA, CURSOR_BUSY_PTR);
	if (!busy_pointer) {
	    busy_pointer = xv_create(screen, CURSOR,
				     CURSOR_SRC_CHAR, OLC_BUSY_PTR,
				     CURSOR_MASK_CHAR, 0,
				     0);
	    xv_set(server, XV_KEY_DATA, CURSOR_BUSY_PTR, busy_pointer, 0);
	}
    }
    current_pointer = xv_get(client_window, WIN_CURSOR);
    xv_set(client_window, WIN_CURSOR, busy_pointer, 0);

    /* Draw magnifying glass over help image */
    pw_stencil(help_image, 0, 0, mglass_pr.pr_width, mglass_pr.pr_height,
	       PIX_SRC, &mglass_mask_pr, 0, 0, &mglass_pr, 0, 0);

    if (!help_frame) {
	/* Help frame has not been created yet */
	length = (int) strchr(client_data, ':') - (int) client_data;
	if (length > 73)
	    length = 73;
	strncpy(client_name, client_data, length);
	client_name[length] = 0;
	strcat(client_name, ": Help");
	client_frame = xv_get(client_window, WIN_FRAME);
	if (!client_frame)
	    /* No frame: may be a menu, in which case client_frame
	     * can be found in XV_KEY_DATA WIN_FRAME.
	     */
	    client_frame = xv_get(client_window, XV_KEY_DATA, WIN_FRAME);
	if (!client_frame) {
	    help_request_failed(client_window, client_data,
				"No frame associated with this window",
			    	"xview:helpnoinfo", client_event);
	    return XV_ERROR;
	}
	base_frame = xv_get(client_frame, XV_IS_SUBTYPE_OF, FRAME_BASE) ?
	    client_frame : xv_get(client_frame, WIN_OWNER);
	win_translate_xy(xv_get(client_frame, XV_ROOT), client_frame, 0, 0,
			 &x, &y);
	help_frame = xv_create(base_frame, FRAME_HELP,
			       XV_LABEL, client_name,
			       0);
	notify_interpose_destroy_func(client_frame, help_frame_destroy_proc);
	help_panel = xv_create(help_frame, PANEL,
			       XV_WIDTH, mglass_pr.pr_width,
			       XV_HEIGHT, mglass_pr.pr_height,
			       0);
	help_panel_msg = xv_create(help_panel, PANEL_MESSAGE,
				   0);
	help_textsw = xv_create(help_frame, TEXTSW,
				XV_X, mglass_pr.pr_width,
				XV_Y, 0,
				WIN_ROWS, HELPTEXTLINES,
				WIN_COLUMNS, HELPTEXTCOLS,
				TEXTSW_IGNORE_LIMIT, TEXTSW_INFINITY,
				TEXTSW_LINE_BREAK_ACTION, TEXTSW_WRAP_AT_WORD,
				TEXTSW_LOWER_CONTEXT, -1,	/* disable automatic
								 * scrolling */
				TEXTSW_READ_ONLY, TRUE,
				0);
	help_textsw_sb = xv_get(help_textsw,
				OPENWIN_VERTICAL_SCROLLBAR,
				xv_get(help_textsw, OPENWIN_NTH_VIEW, 0));
	xv_set(help_textsw_sb, SCROLLBAR_SPLITTABLE, FALSE, 0);
	window_fit(help_frame);
    } else {
	/* Help frame already exists: empty text subwindow */
	textsw_reset(help_textsw, 0, 0);
    }

    xv_set(help_panel_msg, PANEL_LABEL_IMAGE, help_image, 0);

    for (i = 0; text; i++) {
	(void) textsw_insert(help_textsw, text, strlen(text));
	text = xv_help_get_text();
    }
    xv_set(help_textsw, TEXTSW_FIRST, 0, 0);

    xv_set(help_textsw_sb, XV_SHOW, i > HELPTEXTLINES, 0);

    /* Show window, in front of all other windows */
    xv_set(help_frame,
	   XV_SHOW, TRUE,
	   WIN_FRONT,
	   0);

    /* Restore pointer */
    xv_set(client_window, WIN_CURSOR, current_pointer, 0);

    return XV_OK;
}


/*
 * Public "show help" routine
 */
Xv_public int
xv_help_show(client_window, client_data, client_event)
    Xv_Window       client_window;
    caddr_t         client_data;
    Event          *client_event;
{
    int             client_width, client_height;

    client_width = (int) xv_get(client_window, XV_WIDTH);
    client_height = (int) xv_get(client_window, XV_HEIGHT);
    xv_help_save_image((Pixwin *)client_window, client_width, client_height,
		       event_x(client_event), event_y(client_event));
    return xv_help_render(client_window, client_data, client_event);
}
