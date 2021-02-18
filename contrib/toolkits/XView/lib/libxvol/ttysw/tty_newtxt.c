#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)tty_newtxt.c 20.15 89/03/11";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 * 
 * tty_newtxt.c	-- faster routines for text output
 * 
 * Experimental routines for faster text thruput. Jim Becker 4/3/89
 * 
 * These routines are being added as needed to speed up the text output. Note
 * that, since I am not changing the main body of code, any initialization of
 * the routines is done the `firsttime' they are called.
 * 
 * This logic is being augmented to perform text clipping on the line level for
 * the rendering of text to the screen. The current tty and text logic always
 * repaints all of the screen. This prevents lines that are out of the
 * clipping regions from being displayed.
 * 
 * There are three local GCs that are allocated in this file for all the text
 * needs.
 */


#include <xview/window.h>
#include <xview_private/pw_impl.h>
#include <pixrect/pixfont.h>
#include <xview_private/xv_color.h>
#include <xview/xv_xrect.h>

#define	BADVAL		-1
#define	ALL		AllPlanes
#define	MAX_LINES	128
/* #define JCB_DEBUG	 */

#define	convert_op(op)	XlatOp[(op) >> PIX_OP_SHIFT]

extern int      XDrawImageString(), XDrawString();
extern Xv_xrectlist *screen_get_clip_rects();

static GC       inverted_gc, default_gc, back_gc;
static int      XlatOp[16];
static int      fore = BADVAL;
static int      back = BADVAL;
static short    line_visible[MAX_LINES];
static short    filter_lines = FALSE;
static short    firsttime = TRUE;
static short	fontloaded = FALSE;
static int      font_height = 0;


/*
 * create a local GC for the user. This is used the first time that the text
 * routines are used to make the three GC used herein.
 */

static          GC
create_GC(display, drawable, foreground, background, function)
    Display        *display;
    Drawable        drawable;
    int             foreground;
    int             background;
    int             function;
{
    unsigned long   gcvaluemask;
    XGCValues       gcvalues;
    GC              gc;

    gcvalues.function = function;
    gcvalues.plane_mask = AllPlanes;
    gcvalues.foreground = foreground;
    gcvalues.background = background;

    gcvaluemask = GCFunction | GCPlaneMask |
	GCForeground | GCBackground;

    gc = XCreateGC(display, drawable, gcvaluemask, &gcvalues);

    return gc;
}

/*
 * set the fonts in the GCs to the Pixfont specified. called once only!
 */
static void
setup_font(window, pixfont)
    Xv_opaque       window;
    Xv_opaque       pixfont;
{
    Xv_Drawable_info *info;
    Display        *display;
    Font            font;
    XFontStruct    *fontinfo;

    DRAWABLE_INFO_MACRO(window, info);
    display = xv_display(info);
    font = (Font) xv_get(pixfont, XV_XID);

    /* it should always be valid, but be careful */
    if (font != NULL) {
	XSetFont(display, default_gc, font);
	XSetFont(display, inverted_gc, font);

	/* determine font height -- don't trust globals!! */
	fontinfo = XQueryFont(display, font);

	font_height = fontinfo->ascent + fontinfo->descent;
	fontloaded = TRUE;
    }

}

/*
 * setup the correct pens and such in the graphics context
 */
static void
setup_GC(display, info, gc, pix_op)
    Display        *display;
    Xv_Drawable_info *info;
    GC              gc;
    int             pix_op;
{
    int             lfore = xv_fg(info);
    int             lback = xv_bg(info);
    int             lplanes = AllPlanes;
    int             lfunc = convert_op(pix_op);

    /* convert functions from SunView expectation to X understanding */
    switch (lfunc) {
      case GXclear:
	lfore = lback;
	lfunc = GXcopy;
	break;
      case GXset:
	lfunc = GXcopy;
	break;
      case GXxor:
	lfore = lfore ^ lback;
	break;
      case GXinvert:
	lplanes = lfore ^ lback;
	break;
      case GXcopyInverted: /* jcb 11/15/89  to fix lack of highlight */
	lfore = lback;
	lback = xv_fg(info);
	lfunc = GXcopy;
    }

    XSetState(display, gc, lfore, lback, lfunc, lplanes);
}


/*
 * called the firsttime we have some access to this module to perform
 * self-initialization.. This keeps this code self-contained to some degree,
 * as it can be called from anywhere.
 */
static void
firsttime_init(window)
    Xv_opaque       window;
{
    Xv_Drawable_info *info;
    Display        *display;
    Drawable        drawable;

    DRAWABLE_INFO_MACRO(window, info);
    display = xv_display(info);
    drawable = xv_xid(info);
    fore = xv_fg(info);
    back = xv_bg(info);

    inverted_gc = create_GC(display, drawable, fore ^ back, back, GXxor);
    default_gc = create_GC(display, drawable, fore, back, GXcopy);
    back_gc = create_GC(display, drawable, fore, back, GXcopy);

    /* init the opcode translation table */
    /* this table was stolen from xv_rop.c	 */
    XlatOp[PIX_CLR >> PIX_OP_SHIFT] = GXclear;
    XlatOp[PIX_SET >> PIX_OP_SHIFT] = GXcopy;
    XlatOp[PIX_DST >> PIX_OP_SHIFT] = GXnoop;
    XlatOp[PIX_NOT(PIX_DST) >> PIX_OP_SHIFT] = GXinvert;
    XlatOp[PIX_SRC >> PIX_OP_SHIFT] = GXcopy;
    XlatOp[PIX_NOT(PIX_SRC) >> PIX_OP_SHIFT] = GXcopyInverted;
    XlatOp[(PIX_SRC & PIX_DST) >> PIX_OP_SHIFT] = GXand;
    XlatOp[(PIX_SRC & PIX_NOT(PIX_DST)) >> PIX_OP_SHIFT] = GXandReverse;
    XlatOp[(PIX_NOT(PIX_SRC) & PIX_DST) >> PIX_OP_SHIFT] = GXandInverted;
    XlatOp[(PIX_SRC ^ PIX_DST) >> PIX_OP_SHIFT] = GXxor;
    XlatOp[(PIX_SRC | PIX_DST) >> PIX_OP_SHIFT] = GXor;
    XlatOp[(PIX_NOT(PIX_SRC) & PIX_NOT(PIX_DST)) >> PIX_OP_SHIFT] = GXnor;
    XlatOp[(PIX_NOT(PIX_SRC) ^ PIX_DST) >> PIX_OP_SHIFT] = GXequiv;
    XlatOp[(PIX_SRC | PIX_NOT(PIX_DST)) >> PIX_OP_SHIFT] = GXorReverse;
    XlatOp[(PIX_NOT(PIX_SRC) | PIX_DST) >> PIX_OP_SHIFT] = GXorInverted;
    XlatOp[(PIX_NOT(PIX_SRC) | PIX_NOT(PIX_DST)) >> PIX_OP_SHIFT] = GXnand;

    firsttime = FALSE;
}

/*
 * basic mechanism is to cache the information between use of this routine.
 * this assumes that certain of the attributes are not going to change.
 * 
 * Note that this also assumes that all will be the same dependent on the
 * "window" pointer. [There would still be too much work to look at the even
 * the window XID for me to be happy...]
 */
Xv_private void
tty_newtext(window, xbasew, ybasew, op, pixfont, string, len)
    Xv_opaque       window;
    int             op;
    register int    xbasew, ybasew;
    Xv_opaque       pixfont;
    char           *string;
    int             len;
{
    static Xv_opaque old_window = (Xv_opaque) BADVAL;
    static Xv_opaque old_pixfont = NULL;
    static int      old_op = BADVAL;
    static Xv_Drawable_info *info;
    static Display *display;
    static Drawable drawable;
    static GC      *gc;
    static int      (*routine) ();
    XGCValues      *gv;
    int             new_fg, new_bg;
    Font            font;

    if (len == 0)
	return;

    if (firsttime)
	firsttime_init(window);

    /* determine if this line is clipped from sight */
    if (filter_lines && !line_visible[ybasew / font_height])
	return;

#ifdef	JCB_DEBUG
    {
	char            de[200];
	strncpy(de, string, len);
	de[len] = '\0';
	printf("%s\n", de);
    }
#endif

    if (window != old_window) {
	DRAWABLE_INFO_MACRO(window, info);
	display = xv_display(info);
	drawable = xv_xid(info);
	old_window = window;
    }

    /* determine if there is a need to change the font */
    if( pixfont != old_pixfont ) {
	    setup_font(window, pixfont);

	    /* don't change unless it was valid */
	    if( fontloaded )
		    old_pixfont = pixfont;
    }

    if ((op = PIX_OP(op)) != old_op) {

	if (op == PIX_NOT(PIX_DST))
	    gc = &inverted_gc;
	else {
	    gc = &default_gc;

	    setup_GC(display, info, *gc, op);
	}

	if (op == PIX_SRC || op == PIX_NOT(PIX_SRC))
	    routine = XDrawImageString;
	else
	    routine = XDrawString;

	old_op = op;
    }
    gv = &(*gc)->values;
    new_fg = xv_fg(info);
    new_bg = xv_bg(info);
    if (((new_fg != gv->foreground) || 
	 (new_bg != gv->background)) && 
	(op != PIX_NOT(PIX_SRC)/* jcb -- in this case pens switched */)) {
	XGCValues       gc_values;

	if (gc == &inverted_gc) {
	    gc_values.foreground = new_fg ^ new_bg;
	} else {
	    gc_values.foreground = new_fg;
	}
	gc_values.background = new_bg;

	XChangeGC(display, *gc, GCForeground | GCBackground, &gc_values);
    }

    (void) (*routine) (display, drawable, *gc, xbasew, ybasew, string, len);
}




/*
 * routine to set, clear or invert the background.
 * 
 * takes the same parameters as pw_writebackground()
 */

Xv_private void
tty_background(window, X, Y, W, H, op)
    Xv_opaque       window;
    int             X, Y, W, H;
    int             op;
{
    static Xv_opaque old_window = NULL;
    static Xv_Drawable_info *info;
    static Display *display;
    static Drawable drawable;

    if (firsttime)
	firsttime_init(window);

    if (old_window != window) {
	DRAWABLE_INFO_MACRO(window, info);
	display = xv_display(info);
	drawable = xv_xid(info);
	old_window = window;
    }
    setup_GC(display, info, back_gc, op);

    XFillRectangle(display, drawable, back_gc, X, Y, W, H);
}



/*
 * copy bits from one place to another on same window.
 */
Xv_private void
tty_copyarea(window, sX, sY, W, H, dX, dY)
    Xv_opaque       window;
    int             sX, sY, W, H, dX, dY;
{
    Xv_Drawable_info *info;
    Display        *display;
    Drawable        drawable;

    if (firsttime)
	firsttime_init(window);

    DRAWABLE_INFO_MACRO(window, info);
    display = xv_display(info);
    drawable = xv_xid(info);

    /* reset to normal mode for copying */
    XSetState(display, back_gc, xv_fg(info), xv_bg(info), GXcopy, AllPlanes);

    XCopyArea(display, drawable, drawable, back_gc,
	      sX, sY, W, H, dX, dY);
}



/*
 * The following routines are used to interact with the cliping regions that
 * have been setup by the XEvent handling logic. This is done because the
 * current implementation of the text logic renders the entire rectangle of
 * the text during repaint events. These clipping rectangles minimize the
 * visible flash to the user.
 */

Xv_private void
tty_setup_clip_rectangles(window)
    Xv_opaque       window;
{
    Xv_Drawable_info *info;
    Display        *display;
    Xv_Screen       screen;
    Xv_xrectlist   *clip_xrects;
    int             count;
    XRectangle     *rects;
    XRectangle     *rptr;
    int             i, j;
    int             nlines, fline;

    if (firsttime)
	firsttime_init(window);

    DRAWABLE_INFO_MACRO(window, info);
    display = xv_display(info);
    screen = xv_screen(info);
    clip_xrects = screen_get_clip_rects(screen);
    count = clip_xrects->count;
    rects = clip_xrects->rect_array;
    rptr = rects;

#ifdef	JCB_DEBUG
    printf("--there are %2d clip rects--\n", count);
#endif

    /* we have some cliprects and the char height is setup.. */
    if (count > 0 && font_height > 0) {

	/* make all the clipping regions bigger */
/*	for (i = 0, rptr = rects; i < count; i++, rptr++) {

	    if (rptr->y > 0) {
 		rptr->height += rptr->y;
		rptr->y = 0;
	    }
	}
*/
	/* set on all the possible gc's we have */
	XSetClipRectangles(display, default_gc, 0, 0,
			   rects, count, Unsorted);

	XSetClipRectangles(display, inverted_gc, 0, 0,
			   rects, count, Unsorted);

	XSetClipRectangles(display, back_gc, 0, 0,
			   rects, count, Unsorted);

	/* setup internal line clipping logic array */
	for (i = 0; i < MAX_LINES; i++)
	    line_visible[i] = (short) FALSE;

	for (i = 0, rptr = rects; i < count; i++, rptr++) {
#ifdef	JCB_DEBUG
	    printf("%2d: %3d %3d %3d %3d\n", i,
		   rptr->x, rptr->y, rptr->width, rptr->height);
#endif

	    /* mark lines in the region as being paintable */
	    fline = rptr->y / font_height;
	    nlines = (rptr->height + font_height) / font_height;

	    /* set visible, include next line because of baseline calc */
	    for (j = 0; j <= nlines; j++)
		line_visible[j + fline] = (short) TRUE;
	}

	filter_lines = TRUE;
    }
}

Xv_private void
tty_clear_clip_rectangles(window)
    Xv_opaque       window;
{
    Display        *display;
    Xv_Drawable_info *info;

    if (firsttime)
	firsttime_init(window);

    DRAWABLE_INFO_MACRO(window, info);
    display = xv_display(info);

#ifdef	JCB_DEBUG
    printf("--clearing clip rects--\n");
#endif

    XSetClipMask(display, inverted_gc, None);
    XSetClipMask(display, default_gc, None);
    XSetClipMask(display, back_gc, None);

    filter_lines = FALSE;
}
