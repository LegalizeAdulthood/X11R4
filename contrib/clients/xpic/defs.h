#ifndef __XPIC_DEFS_H__
#define __XPIC_DEFS_H__
/*
 *  Some basic functions
 */
#ifndef MAX
#define MAX(x,y)	(((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x,y)	(((x) < (y)) ? (x) : (y))
#endif
#ifndef ABS
#define ABS(x)		(((x) >= 0) ? (x) : -(x))
#endif
#ifndef SGN
#define SGN(x)		(((x) > 0) ? 1 : (((x) < 0) ? -1 : 0))
#endif
/*
 *  Modulo function giving non-negative result
 */
#ifndef IMOD
#define IMOD(x,y)	(((x) % (y)) >= 0 ? ((x) % (y)) : ((x) % (y)) + (y))
#endif

/*
 *  Range checking - between lo & hi, inclusive
 */
#define IN_RANGE(x, lo, hi)	((lo) <= (x) && (x) <= (hi))

/*
 *  Two different strings are often different on the first char.
 */
#define STREQ(s1, s2)	((*s1)==(*s2) && strcmp(s1, s2) == 0)
#define STRLT(s1, s2)	(strcmp(s1, s2) < 0)
#define STRGT(s1, s2)	(strcmp(s1, s2) > 0)

#define BOOL		int

#define YES		1
#define NO		0
#define ABORT	-1		/* Returned by the get_input routine */

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define MAXSTR			64

/* 'snap' rounds x to the nearest number divisible by dx */
#define snap(x, dx)	(((int) (((x) + (dx / 2)) / dx)) * dx)

/*
 *  The basic unit of scale is the separation of the grid points on the
 *  screen. Crosses are spaced every five grid points. I'm assuming the
 *  crosses to be 0.5 inches apart, and determine sizes from that by
 *  calculation. This simplifies drawing and re-scaling, I hope. It also
 *  allows pictures displayed on a high-resolution screen to be displayed
 *  on a screen of different resolution without changing the "snap". The
 *  use of that bizarre unit - the inch - is purely because that's how
 *  most of us still think of paper sizes.
 */

#define DEFAULTSPACING		8		/*
									 * For high resolution displays, it is
									 * better to use 10, for low res, try 6
									 */
#define MINSPACING			2

/* Letter size LaserWriter(tm) imaging area size, actually */
#define DEFAULTPAGEHEIGHT	105		/* inches * 10 :-) */
#define DEFAULTPAGEWIDTH	80
#define MINPAGEHEIGHT		60		/* Paper can't get smaller than 6x6" */
#define MINPAGEWIDTH		60

/* 
 *  Defaults almost fill the screen of a Sun-3/50 Gives a nice round scale
 *  factor, with 8 pixel spacing of grid points, 40 pixel spacing of the
 *  crosses. Note that fonts are scaled for this grid spacing.
 */

#define DEFAULTPICHEIGHT	DEFAULTPAGEHEIGHT * DEFAULTSPACING;
#define DEFAULTPICWIDTH		DEFAULTPAGEWIDTH * DEFAULTSPACING;


/*
 *  This font MUST be available, or else xpic won't start up. Doesn't
 *  really matter what it is though
 */

#define DEFAULT_FONT		"fixed"

#define INC_VERTS	128	/* The blocks in which the verts buffer is increased */

/* Increment in which a Buf increments itself */
#define BUF_CHUNK 128

/* Various types of gels */
#define CELL	0
#define LINE	1
#define SPLINE	2
#define BOX		3
#define CIRCLE	4
#define ELLIPSE	5
#define TEXT	6
/* Pseudo-objects - we use them during edit operations */
#define BLOCK	7
#define ELEMENT	8

/* Using one nybble per attribute should leave enough room for expansion */
#define getlinestyle(x)		(x & 0x000f)
#define SOLID	0x0000
#define DOTTED	0x0001
#define SDASH	0x0002
#define LDASH	0x0003
#define DDASH	0x0004
#define NSTYLES	5

#define getlinearrow(x)		((x & 0x00f0) >> 4)
#define NO_ARROW	0x0000
#define ST_ARROW	0x0010
#define EN_ARROW	0x0020

/* !!
 *  Implementing this implies taking care of object ordering - what gets
 *  drawn on top of what. Not yet done.
 */
#define getfillstyle(x)		((x & 0x0f00) >> 8)
#define EMPTY	0x0000
#define GRAY0	0x0000
#define GRAY1	0x0100
#define GRAY2	0x0200
#define GRAY3	0x0300
#define GRAY4	0x0400
#define GRAY5	0x0500
#define GRAY6	0x0600
#define GRAY7	0x0700
#define GRAY8	0x0800
#define GRAY9	0x0900
#define NFILLSTYLES	10

/* Text alignment is in two  bits */
#define HALIGN	0x3000
#define gettext_halign(x)	((x & HALIGN) >> 12)
#define CENTRE	0x0000
#define LJUST	0x1000
#define RJUST	0x2000

#define VALIGN	0xc000
#define gettext_valign(x)	((x & VALIGN) >> 14)
#define MIDLINE	0x0000
#define TOPLINE	0x4000
#define BOTLINE	0x8000

/* Marks the elements in a gel list which have been selected and discarded */
#define SELECTED 0x0001
/* Marks a hilited gel - makes unhilite safer! */
#define HILITED	 0x0002

/* Cell.saved flags */
#define SAVED		0x00
#define MODIFIED	0x01
#define NEWFILE		0x10

/* These are or'ed with the event codes to give the event type */
#define START_MODE	0x100
#define END_MODE	0x200
#define DRAG_MODE	0x300
#define ASK_MODE	0x400

/* Event codes */
#define REDRAW		0x01
#define MOTION		0x02
#define LEFT		0x03
#define RIGHT		0x04
#define MIDDLE		0x05

/* Various editing functions - either BLOCK or ELEMENT */
#define COPY		1
#define MOVE		2
#define DELETE		3
#define	PASTE		4
#define GET			5
#define PUT			6
#define ADJUST		7
#define ROTATE		8
#define SCALE		9
#define CHANGE_ATTRIBUTE	10

/* DrawGel can draw a Gel in one of these modes */
#define DRAW		1
#define ERASE		2
#define INVERT		3
#define HILITE		4

#define MAKEPATTERN(style, dashlist) \
	((style).pattern = (dashlist), (style).len = strlen(dashlist))

#endif /* __XPIC_DEFS_H__ */ /* Do not add anything after this line */
