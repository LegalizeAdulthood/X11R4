#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_display.c 20.35 89/11/20";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Handle frame displaying and size changes.
 */

#include <X11/Xlib.h>
#include <xview_private/fm_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/server.h>
#include <xview/screen.h>

#include <X11/Xatom.h>

static void     frame_display_icon();
static void     flushline();

static short    _frame_dtgrayimage[16] = {
    0x8888,
    0x8888,
    0x2222,
    0x2222,
    0x8888,
    0x8888,
    0x2222,
    0x2222,
    0x8888,
    0x8888,
    0x2222,
    0x2222,
    0x8888,
    0x8888,
    0x2222,
0x2222};

mpr_static(_frame_pixrectdtgray, 16, 16, 1, _frame_dtgrayimage);
struct pixrect *frame_bkgrd = &_frame_pixrectdtgray;



Pkg_private void
frame_display_label(frame)
    register Frame_class_info *frame;
{
    Xv_Drawable_info *info;

    DRAWABLE_INFO_MACRO(FRAME_PUBLIC(frame), info);
    XStoreName(xv_display(info), xv_xid(info), frame->label);
}

Pkg_private void
frame_display_footer(frame)
    register Frame_class_info *frame;
{
    Frame           frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info *info;
    Xv_object       screen, server;

    DRAWABLE_INFO_MACRO(frame_public, info);
    screen = xv_get(frame_public, XV_SCREEN);
    server = xv_get(screen, SCREEN_SERVER);

    if (frame->leftfooter) {
	XChangeProperty(xv_display(info), xv_xid(info),
			xv_get(server, SERVER_WM_WINMSG_ERROR), XA_STRING,
			8, PropModeReplace, frame->leftfooter,
			strlen(frame->leftfooter));
    }
    if (frame->rightfooter) {
	XChangeProperty(xv_display(info), xv_xid(info),
			xv_get(server, SERVER_WM_WINMSG_STATE), XA_STRING,
			8, PropModeReplace, frame->rightfooter,
			strlen(frame->rightfooter));
    }
    if (frame->leftfooter || frame->rightfooter)
        XFlush(xv_display(info));
}

Pkg_private void
frame_display_busy(frame, status)
    register Frame_class_info *frame;
    int             status;

{
    Frame           frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info *info;
    Xv_object       screen, server;


    DRAWABLE_INFO_MACRO(frame_public, info);
    screen = xv_get(frame_public, XV_SCREEN);
    server = xv_get(screen, SCREEN_SERVER);

    XChangeProperty(xv_display(info), xv_xid(info),
		    xv_get(server, SERVER_WM_WIN_BUSY), XA_INTEGER,
		    32, PropModeReplace, &status,
		    1);
}

static void
frame_display_icon(frame)
    register Frame_class_info *frame;
{
    icon_display(frame->icon, FRAME_PUBLIC(frame), 0, 0);
}

/*
 * highlight subwindow border for sw.
 */
Xv_private void
frame_kbd_use(frame_public, sw)
    Frame           frame_public;
    Xv_Window       sw;
{
    Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);

    frame->focus_subwindow = sw;
    window_set(sw, WIN_KBD_FOCUS, TRUE, 0);
}

/*
 * unhighlight subwindow border for client.
 */
Xv_private void
frame_kbd_done(frame_public, sw)
    Frame           frame_public;
    Xv_Window       sw;
{
    window_set(sw, WIN_KBD_FOCUS, FALSE, 0);
}

/*
 * Display utilities.  Note: Belongs somewhere else.
 */
#define	newline(x, y, w, h, chrht, rect) \
	{ *x = rect->r_left; \
	  *y += chrht; \
	  *h -= chrht; \
	  *w = rect->r_width; \
	}

Xv_private void
frame_format_string_to_rect(pixwin, s, font, rect)
    Xv_Window       pixwin;
    char           *s;
    Pixfont        *font;
    register Rect  *rect;
{
    register char  *charptr, *lineptr, *breakptr, c;
    short           x, y, w, h, chrht, chrwth;
#ifdef lint
    short           dummy;
#endif

    if (s == 0 || font == 0)
	return;

    x = rect->r_left;
#ifdef lint
    dummy = x;
    x = dummy;
#endif
    y = rect->r_top;
    w = rect->r_width;
    h = rect->r_height;
    chrht = font->pf_defaultsize.y;
    breakptr = lineptr = s;
    for (charptr = s; *charptr != '\0'; charptr++) {
	c = (*charptr) & 127;
	/*
	 * Setup to wrap on blanks Note: Need better break test.
	 */
	if (c == ' ') {
	    breakptr = charptr;
	}
	chrwth = font->pf_char[c].pc_adv.x;
	/*
	 * Wrap when not enough room for next char
	 */
	if (w < chrwth) {
	    if (breakptr != lineptr) {
		flushline(pixwin, rect->r_left, y, lineptr, breakptr, font);
		charptr = breakptr;
		lineptr = ++breakptr;
		continue;
	    } else {
		flushline(pixwin, rect->r_left, y, lineptr, charptr, font);
		breakptr = lineptr = charptr;
	    }
	    newline(&x, &y, &w, &h, chrht, rect);
	}
	/*
	 * Punt when run out of vertical space
	 */
	if (h < chrht)
	    break;
	w -= chrwth;
	x += chrwth;
    }
    flushline(pixwin, rect->r_left, y, lineptr, charptr, font);
}

#define	STRBUF_LEN	1000

static void
flushline(pixwin, x, y, lineptr, charptr, font)
    Xv_Window       pixwin;
    int             x, y;
    char           *lineptr, *charptr;
    Pixfont        *font;
{
    char            strbuf[STRBUF_LEN];
    int             len = charptr - lineptr;

    if (charptr == 0 || lineptr == 0 || len > STRBUF_LEN)
	return;

    (void) strncpy(strbuf, lineptr, len);
    *(strbuf + len) = '\0';
    (void) xv_text(pixwin, x - font->pf_char[*strbuf].pc_home.x,
		y - font->pf_char[*strbuf].pc_home.y, PIX_SRC, font, strbuf);
}


/* ARGSUSED */
Pkg_private
frame_set_color(frame, fg, bg)
    Frame_class_info *frame;
    register struct xv_singlecolor *fg;
    register struct xv_singlecolor *bg;
{
    Frame           frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info *info;
    unsigned char   red[2], green[2], blue[2];
    Xv_Colormap_Seg *screen_alloc_cms();
    char            cms_name[CMS_NAMESIZE];
    Xv_cmsdata      cms_data;

    DRAWABLE_INFO_MACRO(frame_public, info);

    if ((xv_depth(info) < 2) || (!fg && !bg)) {
	return;
    }
    /*
     * Note - Assuming that the default cms is set to black/white. If that
     * changes (ie.  some global default mechanism) this will have to change.
     */
    sprintf(cms_name, "xv_frame_%d_%d", xv_xid(info), xv_display(info));
    cms_data.type = XV_STATIC_CMS;
    cms_data.size = 2;
    cms_data.rgb_count = 0;
    if (fg) {
	frame->fg.red = red[1] = fg->red;
	frame->fg.green = green[1] = fg->green;
	frame->fg.blue = blue[1] = fg->blue;
	cms_data.red = &red[1];
	cms_data.green = &green[1];
	cms_data.blue = &blue[1];
	cms_data.index = 1;
	++cms_data.rgb_count;
    }
    if (bg) {
	frame->bg.red = red[0] = bg->red;
	frame->bg.green = green[0] = bg->green;
	frame->bg.blue = blue[0] = bg->blue;
	cms_data.red = &red[0];
	cms_data.green = &green[0];
	cms_data.blue = &blue[0];
	cms_data.index = 0;
	++cms_data.rgb_count;
    }
    xv_set(frame_public, WIN_CMS_NAME, cms_name, WIN_CMS_DATA, &cms_data, 0);
}

/*
 * If the frame cms is not the default cms & either the fg or the bg has not
 * been explicitly specified, initialise to the default values (ie. this is
 * the case where only one of FRAME_FOREGROUND_COLOR & FRAME_BACKGROUND_COLOR
 * has been set.
 */
frame_check_fg_bg(frame_public)
    Frame           frame_public;
{
    Xv_Drawable_info *info;
    Xv_Colormap_Seg *default_cms, *frame_cms;

    DRAWABLE_INFO_MACRO(frame_public, info);
    frame_cms = xv_cms(info);
    default_cms = (Xv_Colormap_Seg *)
	xv_get(xv_screen(info), SCREEN_DEFAULT_CMS);

    if ((frame_cms->cms_type == XV_STATIC_CMS) && (frame_cms != default_cms)) {
	if (XV_STATIC_CMS_BG(frame_cms) == XV_INVALID_PIXVAL) {
	    info->bg = XV_STATIC_CMS_BG(frame_cms) = XV_STATIC_CMS_BG(default_cms);
	}
	if (XV_STATIC_CMS_FG(frame_cms) == XV_INVALID_PIXVAL) {
	    info->fg = XV_STATIC_CMS_FG(frame_cms) = XV_STATIC_CMS_FG(default_cms);
	}
    }
}
