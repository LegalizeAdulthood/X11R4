#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)pw_line.c 20.26 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/pw_impl.h>
#include <pixrect/pr_line.h>

extern short    pw_tex_dashed[];

pw_line(pw, x0, y0, x1, y1, brush, tex, op)
    int             op;
    register int    x0, y0, x1, y1;
    register struct pixwin *pw;
    struct pr_brush *brush;
    Pr_texture     *tex;
{
    XGCValues       gc_val;
    unsigned long   gc_mask = GCLineWidth | GCLineStyle;
    char           *dash_set;
    int             p_len;
    char           *pw_short_to_char();
    Xv_Drawable_info *info;
    Display        *display;
    Drawable        xid;
    GC              line_gc;

    DRAWABLE_INFO_MACRO((Xv_opaque) pw, info);
    display = xv_display(info);
    xid = xv_xid(info);
    line_gc = xv_find_proper_gc(display, pw, info, PW_LINE);

    xv_set_gc_op(display, info, line_gc, op, XV_USE_OP_FG,
		 XV_DEFAULT_FG_BG);
    gc_val.line_width = (!brush || !(brush->width)) ? 1 : brush->width;
    /*
     * determine line style according to user specification
     */
    gc_val.line_style = ((!tex || tex->pattern == pw_tex_dashed) ? LineSolid
			 : LineOnOffDash);
    /*
     * No need to save and restore GC values here because we're using
     * dedicated GC's.
     */
    XChangeGC(display, line_gc, gc_mask, &gc_val);
    /* if tex is null, then simply draw a solid line */
    if (tex) {
	/*
	 * BUG ALERT:  We are not going to do "adjust".
	 */
	dash_set = pw_short_to_char(tex->pattern, &p_len);
	XSetDashes(display, line_gc, tex->offset, dash_set, p_len);
    }
    XDrawLine(display, xid, line_gc, x0, y0, x1, y1);
    (void) free((char *) dash_set);
}


/*
 * Transform an array of shorts into an array of chars. If an element in the
 * shorts array is greater than 256, then print msg, and truncate it to 256.
 * As a side effect, the routine returns the length of the array in "len".
 */
/*
 * BUG ALERT: incompatibilities: an element in the shorts array can be
 * greater than 256, which is max for the value of a char array element
 */
Pkg_private char *
pw_short_to_char(pattern, len)
    short          *pattern;
    int            *len;
{
    register short *p = pattern;
    register int    l = 0;
    char           *malloc(), *c;
    register char  *c1;

    while (*p) {
	l++;
	if (*p > 255) {
	    printf("line texture pattern element %d is greater than 255! Shorten to 255\n", l);
	    *p = 255;
	}
	p++;
    }
    *len = l;
    c = c1 = malloc(l);
    p = pattern;
    while (l--) {
	*c1++ = *p++;
    }
    return (c);
}


void
xv_draw_rectangle(pw, x, y, w, h, linestyle, op)
    Xv_opaque       pw;
    int             x, y, w, h;	/* left, top, width and height of rectangle */
    int             linestyle;	/* LineSolid or LineDoubleDash */
    int             op;
{
    Xv_Drawable_info *info;
    Display        *display;
    Drawable        xid;
    GC              rect_gc;
    XGCValues       values;

    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    xid = xv_xid(info);
    rect_gc = xv_find_proper_gc(display, pw, info, PW_LINE);

    xv_set_gc_op(display, info, rect_gc, op, XV_USE_OP_FG,
		 XV_DEFAULT_FG_BG);

    values.line_style = linestyle;
    /*
     * Since the GC is being shared with pw_line, reset the line_width since
     * it might have been changed by an earlier call to pw_line
     */
    values.line_width = 0;
    XChangeGC(display, rect_gc, GCLineWidth | GCLineStyle, &values);

    XDrawRectangle(display, xid, rect_gc, x, y, w, h);
}

void
xv_paint_lines(pw, op, npts, outline, x, y, w, h)
    Xv_opaque       pw;		/* pixwin */
    int             npts;	/* number of points */
    struct pr_pos  *outline;	/* array of points */
    int             x, y;	/* top left corner of enclosing rectangle */
    int             w, h;	/* width and height of enclosing rectangle */
{
    register int    i;
    register XPoint *points;
    Xv_Drawable_info *info;
    Display        *display;
    Drawable        xid;
    GC              gc;

    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    xid = xv_xid(info);
    gc = xv_find_proper_gc(display, pw, info, PW_VECTOR);

    xv_set_gc_op(display, info, gc, op, XV_USE_OP_FG,
		 XV_DEFAULT_FG_BG);
    points = (XPoint *) malloc(npts * sizeof(XPoint));
    /*
     * BUG ALERT: convert int into short, might lose
     */
    for (i = 0; i < npts; i++) {
	points[i].x = x + outline[i].x;
	if (outline[i].x < 0)
	    points[i].x += w;
	points[i].y = y + outline[i].y;
	if (outline[i].y < 0)
	    points[i].y += h;
    }

    XDrawLines(display, xid, gc, points, npts, CoordModeOrigin);
    free(points);
}
