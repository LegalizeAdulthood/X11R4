#if defined(sccs) && !defined(lint)
static char     sccsid[] = "@(#)fm_cmdline.c 20.16 89/11/20";
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <xview/cms.h>
#include <xview/rect.h>
#include <xview/screen.h>
#include <xview/server.h>
#include <xview/frame.h>
#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/icon.h>
#include <xview/defaults.h>
#include <xview/win_screen.h>

extern Pixrect *icon_load_mpr();

/*
 * Convert command line frame defaults into attributes. Apply the attributes
 * to the frame with xv_set_avlist().
 */
Pkg_private int
frame_set_cmdline_options(frame_public, avlist)
    Frame           frame_public;
    Frame_attribute avlist[];
{
    Xv_opaque       defaults_array[ATTR_STANDARD_SIZE];
    register Attr_avlist defaults = defaults_array;
    struct xv_singlecolor foreground_color, background_color;
    int             status;
    int             rgb[3];
    char           *string;
    char           *defaults_string[1];

    /* No string malloc's done yet */
    defaults_string[0] = NULL;

    /* Parse frame command line options */
    if (defaults_exists("window.header", "Window.Header")) {
	*defaults++ = (Xv_opaque) FRAME_LABEL;
	string = defaults_get_string("window.header", "Window.Header", "");
	defaults_string[0] = malloc(1, strlen(string) + 1);
	strcpy(defaults_string[0], string);
	*defaults++ = (Xv_opaque) defaults_string[0];
    }
    if (defaults_exists("window.columns", "Window.Columns")) {
	*defaults++ = (Xv_opaque) WIN_COLUMNS;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("window.columns", "Window.Columns", 80);
    }
    if (defaults_exists("window.rows", "Window.Rows")) {
	*defaults++ = (Xv_opaque) WIN_ROWS;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("window.rows", "Window.Rows", 34);
    }
    if (defaults_exists("window.width", "Window.Width")) {
	*defaults++ = (Xv_opaque) WIN_WIDTH;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("window.width", "Window.Width",
				 frame_width_from_columns(80));
    }
    if (defaults_exists("window.height", "Window.Height")) {
	*defaults++ = (Xv_opaque) WIN_HEIGHT;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("window.height", "Window.Height",
				 frame_height_from_lines(34));
    }
    if (defaults_exists("window.x", "Window.X")) {
	*defaults++ = (Xv_opaque) WIN_X;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("window.x", "Window.X", 0);
    }
    if (defaults_exists("window.y", "Window.Y")) {
	*defaults++ = (Xv_opaque) WIN_Y;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("window.y", "Window.Y", 0);
    }
    if (defaults_exists("window.iconic", "Window.Iconic")) {
	*defaults++ = (Xv_opaque) FRAME_CLOSED;
	*defaults++ = (Xv_opaque)
	    defaults_get_boolean("window.iconic", "Window.Iconic", FALSE);
    }
    if (defaults_exists("window.inheritcolor",
			"Window.InheritColor")) {
	*defaults++ = (Xv_opaque) FRAME_INHERIT_COLORS;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("window.inheritcolor",
				 "Window.InheritColor", TRUE);
    }
    if (defaults_exists("window.color.foreground", "Window.Color.Foreground")) {
	*defaults++ = (Xv_opaque) FRAME_FOREGROUND_COLOR;
	string = defaults_get_string("window.color.foreground",
				     "Window.Color.Foreground", "0 0 0");
	sscanf(string, "%d %d %d", &(rgb[0]), &(rgb[1]), &(rgb[2]));
	foreground_color.red = (u_char) rgb[0];
	foreground_color.green = (u_char) rgb[1];
	foreground_color.blue = (u_char) rgb[2];
	*defaults++ = (Xv_opaque) & foreground_color;
    }
    if (defaults_exists("window.color.background", "Window.Color.Background")) {
	*defaults++ = (Xv_opaque) FRAME_BACKGROUND_COLOR;
	string = defaults_get_string("window.color.background",
				     "Window.Color.Background", "0 0 0");
	sscanf(string, "%d %d %d", &(rgb[0]), &(rgb[1]), &(rgb[2]));
	background_color.red = (u_char) rgb[0];
	background_color.green = (u_char) rgb[1];
	background_color.blue = (u_char) rgb[2];
	*defaults++ = (Xv_opaque) & background_color;
    }
    *defaults = 0;

    /* Do a frame set if there are frame attrs */
    if (defaults_array[0])
	status = (int) xv_set_avlist(frame_public, defaults_array);
    else
	status = XV_OK;

    /* Free any malloc'ed strings */
    if (defaults_string[0])
	free(defaults_string[0]);

    return (status);
}


/*
 * Convert command line icon defaults into attributes. Apply the attributes
 * to the frame's icon with xv_set_avlist().
 */
Pkg_private int
frame_set_icon_cmdline_options(frame_public, avlist)
    Frame           frame_public;
    Frame_attribute avlist[];
{
    Xv_opaque       defaults_array[ATTR_STANDARD_SIZE];
    char            errors[100], *string;
    int             status;
    Xv_object       screen, server;
    Pixrect        *image;
    register Attr_avlist defaults = defaults_array;
    char           *defaults_string[1];

    /* No string malloc's done yet */
    defaults_string[0] = NULL;

    if (defaults_exists("icon.font.name", "Icon.Font.Name")) {
	*defaults++ = (Xv_opaque) ICON_FONT;
	string = defaults_get_string("icon.font.name",
				     "Icon.Font.Name", NULL);
	screen = xv_get(frame_public, XV_SCREEN);
	server = xv_get(screen, SCREEN_SERVER);
	*defaults++ = xv_get(server, SERVER_FONT_WITH_NAME, string);
    }
    if (defaults_exists("icon.pixmap", "Icon.Pixmap")) {
	string = defaults_get_string("icon.pixmap", "Icon.Pixmap", NULL);
	image = icon_load_mpr(string, errors);
	if (image) {
	    *defaults++ = (Xv_opaque) ICON_IMAGE;
	    *defaults++ = (Xv_opaque) image;
	} else {
	    /* BUG ALERT!  Show contents of errors to user? */
	}
    }
    if (defaults_exists("icon.footer", "Icon.Footer")) {
	*defaults++ = (Xv_opaque) ICON_LABEL;
	string = defaults_get_string("icon.footer", "Icon.Footer", NULL);
	defaults_string[0] = malloc(1, strlen(string) + 1);
	strcpy(defaults_string[0], string);
	*defaults++ = (Xv_opaque) defaults_string[0];
    }
    if (defaults_exists("icon.x", "Icon.X")) {
	*defaults++ = (Xv_opaque) WIN_X;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("icon.x", "Icon.X", 0);
    }
    if (defaults_exists("icon.y", "Icon.Y")) {
	*defaults++ = (Xv_opaque) WIN_Y;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("icon.y", "Icon.Y", 0);
    }
    *defaults = 0;
    /* Do a frame set if there are frame attrs */
    if (defaults_array[0])
	status = (int) xv_set_avlist(xv_get(frame_public, FRAME_ICON),
				     defaults_array);
    else
	status = XV_OK;

    /* Free any malloc'ed strings */
    if (defaults_string[0])
	free(defaults_string[0]);

    return (status);
}
