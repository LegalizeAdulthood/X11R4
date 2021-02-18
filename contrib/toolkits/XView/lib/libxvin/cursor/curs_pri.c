#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)curs_pri.c 20.27 89/11/28";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef	pixrect_hs_DEFINED
#define	pixrect_hs_DEFINED
/* <pixrect/pixrect_hs.h> without frame buffer variable include files */
#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <pixrect/pr_dblbuf.h>
#include <pixrect/pr_line.h>
#include <pixrect/pr_planegroups.h>
#include <pixrect/pr_util.h>
#include <pixrect/traprop.h>
#include <pixrect/memvar.h>
#include <pixrect/pixfont.h>
#include <rasterfile.h>
#include <pixrect/pr_io.h>
#endif	pixrect_hs_DEFINED

#include <stdio.h>
#include <xview/pkg.h>
#include <xview/font.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>
#include <xview_private/scrn_vis.h>

Screen_visual  *screen_add_visual();

Pkg_private void
cursor_free_x(info, old_cursor)
    Xv_Drawable_info *info;
    Cursor          old_cursor;
{
    XFreeCursor(xv_display(info), old_cursor);
}

Pkg_private long unsigned
cursor_make_x(root_info, w, h, d, op, xhot, yhot, xfg, xbg, pr)
    Xv_Drawable_info   *root_info;
    int	    	    	w, h, d, op, xhot, yhot;
    XColor  	       *xfg, *xbg;
    Pixrect 	       *pr;
{
    Window          root = xv_xid(root_info);
    Display        *display = xv_display(root_info);
    GC              gc;
    Pixmap          src, mask, m;
    Cursor          result;
    int             oldw, oldh;
    Screen_visual  *visual;
    Xv_Drawable_info info;

    if ((w <= 0) || (h <= 0) || (d <= 0)) {
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "xv_cursor: cannot create cursor with null image", 0);
	return (long unsigned) None;
    }
    /*
     * handle the case with xhot or yhot bigger than the source pixrect. BUG:
     * does not handle negative xhot or yhot.
     */
    if ((xhot < 0) || (yhot < 0))
	xv_error(NULL, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
	       XV_ERROR_STRING, "cursor_make_x(): bad xhot/yhot parameters",
		 XV_ERROR_PKG, ATTR_PKG_CURSOR,
		 0);

    if (xhot > w) {
	w = xhot;
    }
    if (yhot > h) {
	h = yhot;
    }
    /* if the cursor op is XOR, create a bigger pixmap for outline cursor */
    if ((op & PIX_NOT(0)) == (PIX_SRC ^ PIX_DST)) {
	oldw = w;
	oldh = h;
	w += 2;
	h += 2;
	xhot++;
	yhot++;
    }
    /*
     * BUG: both mask and src pixmaps can only be of depth 1
     */
    src = XCreatePixmap(display, root, w, h, d);
    /* Fake up an info struct to pass to xv_rop_internal */
    info.visual = screen_add_visual(xv_screen(root_info), src, 1);
    info.private_gc = 0;
    m = mask = XCreatePixmap(display, root, w, h, d);
    visual = screen_add_visual(xv_screen(root_info), src, d);
    gc = visual->gc;
    if (!(src && mask && gc)) {
	return (long unsigned) None;
    }
    /* clear the mask since XOR may be used to rop into it */
    XSetFunction(display, gc, GXclear);
    XFillRectangle(display, mask, gc, 0, 0, w, h);
    /* BUG - Clear the src to workaround xnews cursor bug */
    XFillRectangle(display, src, gc, 0, 0, w, h);

    /* PIX_NOT(0) masks out color and PIX_DONTCLIP */
    switch (op & PIX_NOT(0)) {
      case PIX_CLR:
	/* src is already clear, so don't need to touch it. */
	XSetFunction(display, gc, GXclear);
	XFillRectangle(display, src, gc, 0, 0, w, h);
	mask = None;
	break;
      case PIX_SET:
	XSetFunction(display, gc, GXset);
	XFillRectangle(display, src, gc, 0, 0, oldw, oldh);
	mask = None;
	break;
      case PIX_DST:
	XSetFunction(display, gc, GXclear);
	XFillRectangle(display, mask, gc, 0, 0, w, h);
	break;
      case PIX_SRC:
	XSetFunction(display, gc, GXcopy);
	xv_rop_internal(display, src, gc, 0, 0, w, h, pr, 0, 0, &info);
	mask = None;
	break;
      case PIX_NOT(PIX_SRC):
	XSetFunction(display, gc, GXcopyInverted);
	xv_rop_internal(display, src, gc, 0, 0, w, h, pr, 0, 0, &info);
	mask = None;
	break;
      case PIX_SRC & PIX_DST:
	XSetFunction(display, gc, GXcopy);
	xv_rop_internal(display, src, gc, 0, 0, w, h, pr, 0, 0, &info);
	XSetFunction(display, gc, GXcopyInverted);
	xv_rop_internal(display, mask, gc, 0, 0, w, h, pr, 0, 0, &info);
	break;
      case PIX_NOT(PIX_SRC) & PIX_DST:
	XSetFunction(display, gc, GXcopyInverted);
	xv_rop_internal(display, src, gc, 0, 0, w, h, pr, 0, 0, &info);
	XSetFunction(display, gc, GXcopy);
	xv_rop_internal(display, mask, gc, 0, 0, w, h, pr, 0, 0, &info);
	break;
      case PIX_NOT(PIX_SRC) | PIX_DST:
	XSetFunction(display, gc, GXcopyInverted);
	xv_rop_internal(display, src, gc, 0, 0, w, h, pr, 0, 0, &info);
	mask = src;
	break;
      case PIX_SRC ^ PIX_DST:{
	    short           i, j;

	    XSetFunction(display, gc, GXcopy);
	    xv_rop_internal(display, src, gc, 1, 1, oldw, oldh, pr, 0, 0, &info);
	    /* Build a mask that is a stencil around the src. */
	    XSetFunction(display, gc, GXor);
	    for (i = 0; i <= 2; i++) {
		for (j = 0; j <= 2; j++) {
		    xv_rop_internal(display, mask, gc, i, j, oldw, oldh, pr, 0, 0, &info);
		}
	    }
	    break;
	}
      case PIX_SRC | PIX_DST:
	/* BUG: The following cases can't be done w/o CURSOR_OP in X */
	/* We just pretend that it's the same as PIX_SRC | PIX_DST */
      case PIX_SRC & PIX_NOT(PIX_DST):
      case PIX_NOT(PIX_SRC) & PIX_NOT(PIX_DST):
      case PIX_NOT(PIX_SRC) ^ PIX_DST:
      case PIX_SRC | PIX_NOT(PIX_DST):
      case PIX_NOT(PIX_SRC) | PIX_NOT(PIX_DST):
      case PIX_NOT(PIX_DST):
	XSetFunction(display, gc, GXcopy);
	xv_rop_internal(display, src, gc, 0, 0, w, h, pr, 0, 0, &info);
	mask = src;
	break;
      default:
	xv_error(NULL, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
	     XV_ERROR_STRING, "cursor_make_x(): unknown rasterop specified",
		 XV_ERROR_PKG, ATTR_PKG_CURSOR,
		 0);
    }
    /*
     * WARNING: X server interprets "mask==None" as implying src is mask, but
     * we want a completely black mask, so we fill it here if appropriate.
     */
    if (mask == None) {
	/*
	 * PERFORMANCE ALERT!  More complex code could avoid having set the
	 * mask to 0 above when it is going to be unnecessary.
	 */
	mask = m;
	XSetFunction(display, gc, GXset);
	XFillRectangle(display, mask, gc, 0, 0, w, h);
    }
    result = XCreatePixmapCursor(display, src, mask, xfg, xbg, xhot, yhot);
    XFreePixmap(display, src);
    XFreePixmap(display, m);
    return ((long unsigned) result);
}


Pkg_private unsigned long
cursor_make_x_font(root_info, src_char, mask_char, xfg, xbg)
    Xv_Drawable_info *root_info;
    unsigned int    src_char;
    unsigned int    mask_char;
    XColor	    *xfg, *xbg;
{
    Display        *display = xv_display(root_info);
    Font            x_cursor_font;
    Xv_Font         xview_cursor_font;

    xview_cursor_font = (Xv_Font) xv_find(xv_server(root_info), FONT,
					  FONT_FAMILY, FONT_FAMILY_OLCURSOR,
					  FONT_TYPE, FONT_TYPE_CURSOR,
					  0);
    if (!xview_cursor_font)
	xv_error(NULL, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, "Unable to find OPEN LOOK cursor font",
		 XV_ERROR_PKG, ATTR_PKG_CURSOR,
		 0);
    x_cursor_font = (Font) xv_get(xview_cursor_font, XV_XID);
    if (mask_char == 0) {
	mask_char = src_char;
    }
    return (XCreateGlyphCursor(display, x_cursor_font, x_cursor_font,
		src_char, mask_char, xfg, xbg));
}


Pkg_private void
cursor_set_cursor_internal(info, cursor)
    Xv_Drawable_info *info;
    Cursor          cursor;
{
    XDefineCursor(xv_display(info), xv_xid(info), cursor);
}
