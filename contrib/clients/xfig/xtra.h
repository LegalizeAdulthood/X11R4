/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#define	tool_destroy(tool)	XtDestroyWidget(tool)

pr_size pf_textwidth();

#define	pw_text(w, x, y, op, font, string) \
	XDrawString(tool_d, (w), gccache[(op)], (x), (y), \
		(string), strlen(string))

#define	pw_put(w, x, y, op) \
	XDrawPoint(tool_d, (w), gccache[(op) ? PAINT : ERASE], (x), (y))

#define	pw_vector(w, x1, y1, x2, y2, op, width) \
	XDrawLine(tool_d, (w), gccache[(op)], (x1), (y1), (x2), (y2))

#define	pw_batch_on(w)

#define	pw_batch_off(w)
