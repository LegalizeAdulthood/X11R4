/* $Header: draw.h,v 1.5 89/07/02 15:58:04 moraes Exp $ */

#define ellipse(w, x, y, xradius, yradius, gc)	\
	XDrawArc(picDpy, w, gc, (x) - (xradius), (y) - (yradius), \
	 (unsigned int) ((xradius)*2), (unsigned int) ((yradius)*2), 0, 360*64)

#define box(w, x1, y1, x2, y2, gc) \
	draw_box(picDpy, w, gc, x1, y1, x2, y2)

#define line(w, x1, y1, x2, y2, gc) \
	XDrawLine(picDpy, w, gc, x1, y1, x2, y2)

#if 0
/*
 * Enable this if the server ever gets fixed to acknowledge that duplicate
 * points are not an error
 */
#define drawlines(dpy, w, gc, pts, npts, mode) \
	XDrawLines(dpy, w, gc, pts, npts, mode)
#endif
	
#define SETDASHES(gc, style) \
	if ((style) == SOLID && gc->values.line_style != LineSolid) { \
		gcVal.line_style = LineSolid; \
		XChangeGC(picDpy, gc, GCLineStyle, &gcVal); \
	} else if ((style) != SOLID) { \
		if (gc->values.line_style == LineSolid) { \
			gcVal.line_style = LineOnOffDash; \
			XChangeGC(picDpy, gc, GCLineStyle, &gcVal); \
		} \
		XSetDashes(picDpy, gc, 0, LineTypes[(style)].pattern,  \
		 LineTypes[(style)].len); \
	}

#define setwidth(gc, width) \
	(gcVal.line_width = width, XChangeGC(picDpy, gc, GCLineWidth, &gcVal))

#define setfont(gc, fid) \
	(gcVal.font = fid, XChangeGC(picDpy, gc, GCFont, &gcVal))

