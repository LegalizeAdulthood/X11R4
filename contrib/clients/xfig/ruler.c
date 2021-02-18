/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "const.h"
#include "font.h"
#include "paintop.h"

#define			INCH_MARK		12
#define			HALF_MARK		11
#define			QUARTER_MARK		8
#define			SIXTEENTH_MARK		6

#ifndef	X11
#define			MARK_HT			5
#define			TRM_WID			15
#define			TRM_HT			8
#define			SRM_WID			8
#define			SRM_HT			15
#else
#define			MARK_HT			5
#define			TRM_WID			16
#define			TRM_HT			8
#define			SRM_WID			8
#define			SRM_HT			16
#endif

extern int		CANVAS_HEIGHT, CANVAS_WIDTH;
extern int		SIDERULER_WIDTH, SIDERULER_HEIGHT;
extern int		TOPRULER_WIDTH, TOPRULER_HEIGHT;
extern int		SIDERULER_HEIGHT, SIDERULER_START;
extern int		TOPRULER_LEFT, TOPRULER_TOP;
extern			null_proc();
extern int		RHS_PANEL;
extern int		TRACKING;

static			lasty = -100;
static			lastx = -100;
static int	 	start;
static char		*number[] = {"0", "1", "2", "3", "4", "5", 
					"6", "7", "8", "9", "10", "11" };
static int		troffx = -7, troffy = -10;
#ifndef	X11
static short		tr_marker_image[8] = {
				0xFFFE,	/* *************** */
				0x7FFC,	/*  *************  */
				0x3FF8,	/*   ***********   */
				0x1FF0,	/*    *********    */
				0x0FE0,	/*     *******     */
				0x07C0,	/*      *****      */
				0x0380,	/*       ***       */
				0x0100	/*        *        */
				};
#else
static char		tr_marker_image[16] = {
				0xFF, 0xFE,	/* *************** */
				0x7F, 0xFC,	/*  *************  */
				0x3F, 0xF8,	/*   ***********   */
				0x1F, 0xF0,	/*    *********    */
				0x0F, 0xE0,	/*     *******     */
				0x07, 0xC0,	/*      *****      */
				0x03, 0x80,	/*       ***       */
				0x01, 0x00	/*        *        */
				};
#endif
static			mpr_static(trm_pr, TRM_WID, TRM_HT, 1, tr_marker_image);
static int		srroffx = 2, srroffy = -7;
#ifndef	X11
static short		srr_marker_image[15] = {
				0x0100,	/*          *  */
				0x0300,	/*         **  */
				0x0700,	/*        ***  */
				0x0F00,	/*       ****  */
				0x1F00,	/*      *****  */
				0x3F00,	/*     ******  */
				0x7F00,	/*    *******  */
				0xFF00, /*   ********  */
				0x7F00,	/*    *******  */
				0x3F00,	/*     ******  */
				0x1F00,	/*      *****  */
				0x0F00,	/*       ****  */
				0x0700,	/*        ***  */
				0x0300,	/*         **  */
				0x0100	/*          *  */
				};
#else
static char		srr_marker_image[16] = {
				0x01,	/*          *  */
				0x03,	/*         **  */
				0x07,	/*        ***  */
				0x0F,	/*       ****  */
				0x1F,	/*      *****  */
				0x3F,	/*     ******  */
				0x7F,	/*    *******  */
				0xFF,	/*   ********  */
				0x7F,	/*    *******  */
				0x3F,	/*     ******  */
				0x1F,	/*      *****  */
				0x0F,	/*       ****  */
				0x07,	/*        ***  */
				0x03,	/*         **  */
				0x01,	/*          *  */
				0x00
				};
#endif
static		mpr_static(srrm_pr, SRM_WID, SRM_HT, 1, srr_marker_image);

static int		srloffx = -10, srloffy = -7;
#ifndef	X11
static short		srl_marker_image[15] = {
				0x8000,	/*  *          */
				0xC000,	/*  **         */
				0xE000,	/*  ***        */
				0xF000,	/*  ****       */
				0xF800,	/*  *****      */
				0xFC00,	/*  ******     */
				0xFE00,	/*  *******    */
				0xFF00,	/*  ********   */
				0xFE00,	/*  *******    */
				0xFC00,	/*  ******     */
				0xF800,	/*  *****      */
				0xF000,	/*  ****       */
				0xE000,	/*  ***        */
				0xC000,	/*  **         */
				0x8000	/*  *          */
				};
#else
static char		srl_marker_image[16] = {
				0x80,	/*  *          */
				0xC0,	/*  **         */
				0xE0,	/*  ***        */
				0xF0,	/*  ****       */
				0xF8,	/*  *****      */
				0xFC,	/*  ******     */
				0xFE,	/*  *******    */
				0xFF,	/*  ********   */
				0xFE,	/*  *******    */
				0xFC,	/*  ******     */
				0xF8,	/*  *****      */
				0xF0,	/*  ****       */
				0xE0,	/*  ***        */
				0xC0,	/*  **         */
				0x80,	/*  *          */
				0x00
				};
#endif
static		mpr_static(srlm_pr, SRM_WID, SRM_HT, 1, srl_marker_image);

#ifdef	X11
static Pixmap		toparrow_pm, sidearrow_pm;
#endif

set_toprulermark(x)
int	x;
{
#ifndef	X11
	pw_write(topruler_pixwin, lastx + troffx, RULER_WIDTH + troffy, 
		TRM_WID, TRM_HT, INV_PAINT, &trm_pr, 0, 0);
	pw_write(topruler_pixwin, x + troffx, RULER_WIDTH + troffy, 
		TRM_WID, TRM_HT, INV_PAINT, &trm_pr, 0, 0);
#else
	XClearArea(tool_d, topruler_pixwin,lastx + troffx,
		   TOPRULER_HEIGHT + troffy,trm_pr.width,
		   trm_pr.height, False);
	XCopyArea(tool_d, toparrow_pm, topruler_pixwin, topgc,
		0, 0, trm_pr.width, trm_pr.height,
		x + troffx, TOPRULER_HEIGHT + troffy);
#endif	X11
	lastx = x;
	}

#ifndef	X11
static
sideruler_sighandler()
{
	pw_damaged(sideruler_pixwin);
	pw_repairretained(sideruler_pixwin);
	pw_donedamaged(sideruler_pixwin);
	}

static
flush_input(nullsw, ibits, obits, ebits, timer)
caddr_t		nullsw;
int		*ibits, *obits, *ebits;
struct timeval	*timer;
{
	INPUTEVENT		ie;

	(void)input_readevent(srswfd, &ie);
	*ibits = *obits = *ebits = 0;
	}


int
init_sideruler(tool)
TOOL		tool;
{
	INPUTMASK		mask;

	sideruler_sw = tool_createsubwindow(tool, "side ruler", 
				SIDERULER_WIDTH, SIDERULER_HEIGHT);
	if (sideruler_sw == (TOOLSW)0) return(0);
	(int) sideruler_sw->ts_data = srswfd = sideruler_sw->ts_windowfd;

	sideruler_pixwin = pw_open(srswfd);
	sideruler_pixwin->pw_prretained = mem_create(SIDERULER_WIDTH, 
						SIDERULER_HEIGHT, 1);
	input_imnull(&mask);
	win_setinputcodebit(&mask, MS_RIGHT);
	/* Pass input events other than right mouse to parent window */
	win_setinputmask(srswfd, &mask, NULL, WIN_NULLLINK);
	win_setcursor(srswfd, &bull_cursor);
	sideruler_sw->ts_io.tio_selected = flush_input;
	sideruler_sw->ts_io.tio_handlesigwinch = sideruler_sighandler;
	sideruler_sw->ts_destroy = null_proc;

	show_sideruler();
	return(1);
	}
#else
static Arg	ruler_args[] =
{
	{ XtNx, (XtArgVal)0 },
	{ XtNy, (XtArgVal)0 },
	{ XtNwidth, (XtArgVal)0 },
	{ XtNheight, (XtArgVal)0 },
	{ XtNlabel, (XtArgVal)"" },
	{ XtNfromHoriz, (XtArgVal)NULL },
	{ XtNhorizDistance, (XtArgVal)0 },
	{ XtNfromVert, (XtArgVal)NULL },
	{ XtNvertDistance, (XtArgVal)0 },
	{ XtNresizable, (XtArgVal)FALSE },
	{ XtNtop, (XtArgVal)XtRubber },
	{ XtNbottom, (XtArgVal)XtRubber },
	{ XtNleft, (XtArgVal)XtRubber },
	{ XtNright, (XtArgVal)XtRubber },
};

extern int	SIDERULER_WIDTH, SIDERULER_HEIGHT;
extern int	TOPRULER_WIDTH, TOPRULER_HEIGHT;

int init_sideruler(tool)
	TOOL		tool;
{
	ruler_args[2].value = SIDERULER_WIDTH = RULER_WIDTH;
	ruler_args[3].value = SIDERULER_HEIGHT = CANVAS_HEIGHT;
	ruler_args[5].value = (XtArgVal)canvas_sw;
	ruler_args[7].value = (XtArgVal)topruler_sw;
	/* Place the sideruler below the topruler, next to the canvas,
	   but fixed offset from the top of the form */
	ruler_args[10].value = (XtArgVal)XtChainTop;
	ruler_args[11].value = (XtArgVal)XtRubber;
	ruler_args[12].value = (XtArgVal)XtRubber;
	ruler_args[13].value = (XtArgVal)XtRubber;
	
	sideruler_sw = XtCreateWidget("sruler", labelWidgetClass, tool,
		ruler_args, XtNumber(ruler_args));
	return(1);
}

redisplay_sideruler()
{
	XClearWindow(tool_d, sideruler_pixwin);
}

setup_rulers()
{
	register int		i, j;
	register Pixmap		p;
#define	QINCH	(PIX_PER_INCH / 4)
#define	SINCH	(PIX_PER_INCH / 16)
	char			number[3];
	Arg			tmp_arg[3];
	long			bg, fg;
	static Arg		ruler_args[] =
	{
		{ XtNbackgroundPixmap, (XtArgVal)NULL },
	};

	topruler_pixwin = trswfd = XtWindow(topruler_sw);
	sideruler_pixwin = srswfd = XtWindow(sideruler_sw);
	XDefineCursor(tool_d, trswfd, (Cursor)bull_cursor.bitmap);
	XDefineCursor(tool_d, srswfd, (Cursor)bull_cursor.bitmap);

	/* top ruler, adjustments for digits are kludges based on 6x13 char */
	p = XCreatePixmap(tool_d, topruler_pixwin,
			  TOPRULER_WIDTH, TOPRULER_HEIGHT,
			  DefaultDepthOfScreen(tool_s));

	XtSetArg(tmp_arg[0], XtNbackground, &bg);
	XtSetArg(tmp_arg[1], XtNforeground, &fg);
	XtGetValues(topruler_sw, tmp_arg, 2);

	XSetBackground(tool_d, gc, bg);
	XSetForeground(tool_d, gc, bg);
	XFillRectangle(tool_d, p, gc, 0, 0, TOPRULER_WIDTH,
			TOPRULER_HEIGHT);
	XSetForeground(tool_d, gc, fg);
	
	for (i = SINCH - 1; i <= TOPRULER_WIDTH; i += SINCH)
	{
		j = i + 1;
		if (j % PIX_PER_INCH == 0)
		{
			XDrawLine(tool_d, p, gc, i, TOPRULER_HEIGHT - 1, i,
				TOPRULER_HEIGHT - INCH_MARK - 1);
			sprintf(number, "%d", j / PIX_PER_INCH);
			XDrawString(tool_d, p, gc, i - 2,
				TOPRULER_HEIGHT - INCH_MARK - 3, number,
				j < PIX_PER_INCH * 10 ? 1 : 2);
		}
		else if (j % QINCH == 0)
			XDrawLine(tool_d, p, gc, i, TOPRULER_HEIGHT - 1, i,
				TOPRULER_HEIGHT - QUARTER_MARK - 1);
		else if (j % SINCH == 0)
			XDrawLine(tool_d, p, gc, i, TOPRULER_HEIGHT - 1, i,
				TOPRULER_HEIGHT - SIXTEENTH_MARK - 1);
	}
	ruler_args[0].value = (XtArgVal)p;
	XtSetValues(topruler_sw, ruler_args, 1);

	/* The arrows will be XORed into the rulers.
	   We want the foreground color in the arrow to result in
	   the foreground or background color in the display.
	   so if the source pixel is fg^bg, it produces fg when XOR'ed
	   with bg, and bg when XOR'ed with bg.

	   If the source pixel is zero, it produces fg when XOR'ed with
	   fg, and bg when XOR'ed with bg.
	   */
	XSetForeground(tool_d, gc, fg ^ bg);
	XSetBackground(tool_d, gc, 0);

	/* make pixmaps for top ruler arrow */
	toparrow_pm = XCreatePixmap(tool_d, topruler_pixwin, trm_pr.width,
				    trm_pr.height,
				    DefaultDepthOfScreen(tool_s));
	XPutImage(tool_d, toparrow_pm, gc, &trm_pr, 0, 0, 0, 0,
		trm_pr.width, trm_pr.height);
	
	/* side ruler, adjustments for digits are kludges based on 6x13 char */
	p = XCreatePixmap(tool_d, sideruler_pixwin,
			  SIDERULER_WIDTH, SIDERULER_HEIGHT,
			  DefaultDepthOfScreen(tool_s));
	XtSetArg(tmp_arg[0], XtNbackground, &bg);
	XtSetArg(tmp_arg[1], XtNforeground, &fg);
	XtGetValues(sideruler_sw, tmp_arg, 2);

	XSetBackground(tool_d, gc, bg);
	XSetForeground(tool_d, gc, bg);
	XFillRectangle(tool_d, p, gc, 0, 0, SIDERULER_WIDTH,
			SIDERULER_HEIGHT);
	XSetForeground(tool_d, gc, fg);
	
	if( RHS_PANEL )
	{
		for (i = SINCH - 1; i <= SIDERULER_HEIGHT; i += SINCH)
		{
			j = i + 1;
			if (j % PIX_PER_INCH == 0)
			{
				XDrawLine(tool_d, p, gc, RULER_WIDTH-INCH_MARK,
					  i, RULER_WIDTH, i);
				sprintf(number, "%d", j / PIX_PER_INCH);
				XDrawString(tool_d, p, gc,
					    RULER_WIDTH-INCH_MARK - 8, i + 3,
					    number, j < PIX_PER_INCH * 10 ? 1 : 2);
			}
			else if (j % QINCH == 0)
				XDrawLine(tool_d, p, gc,
					  RULER_WIDTH-QUARTER_MARK, i,
					  RULER_WIDTH, i);
			else if (j % SINCH == 0)
				XDrawLine(tool_d, p, gc,
					  RULER_WIDTH-SIXTEENTH_MARK, i,
					  RULER_WIDTH, i);
		}
	}
	else
	{
		for (i = SINCH - 1; i <= SIDERULER_HEIGHT; i += SINCH)
		{
			j = i + 1;
			if (j % PIX_PER_INCH == 0)
			{
				XDrawLine(tool_d, p, gc, 0, i,
					  INCH_MARK - 1, i);
				sprintf(number, "%d", j / PIX_PER_INCH);
				XDrawString(tool_d, p, gc, INCH_MARK + 3,
					    i + 3, number,
					    j < PIX_PER_INCH * 10 ? 1 : 2);
			}	
			else if (j % QINCH == 0)
				XDrawLine(tool_d, p, gc, 0, i,
					  QUARTER_MARK - 1, i);
			else if (j % SINCH == 0)
				XDrawLine(tool_d, p, gc, 0, i,
					  SIXTEENTH_MARK - 1, i);
		}	
	}
	ruler_args[0].value = (XtArgVal)p;
	XtSetValues(sideruler_sw, ruler_args, 1);

	/* Colors set as above */
	XSetForeground(tool_d, gc, fg ^ bg);
	XSetBackground(tool_d, gc, 0);

	/* make pixmaps for side ruler arrow */
	if( RHS_PANEL )
	{
		sidearrow_pm = XCreatePixmap(tool_d, sideruler_pixwin,
					     srlm_pr.width, srlm_pr.height,
					     DefaultDepthOfScreen(tool_s));
		XPutImage(tool_d, sidearrow_pm, gc, &srlm_pr, 0, 0, 0, 0,
			  srlm_pr.width, srlm_pr.height);
	}
	else
	{
		sidearrow_pm = XCreatePixmap(tool_d, sideruler_pixwin,
					     srrm_pr.width, srrm_pr.height,
					     DefaultDepthOfScreen(tool_s));
		XPutImage(tool_d, sidearrow_pm, gc, &srrm_pr, 0, 0, 0, 0,
			  srrm_pr.width, srrm_pr.height);
	}
}
#endif	X11

#ifndef	X11
show_sideruler()
{
	int	 i, qinch, sinch, charht, charx, end;
	char	**d;

	pw_writebackground(sideruler_pixwin, 0, 0, 2048, 2048, PAINT);
	if (RHS_PANEL){
	    start = RULER_WIDTH;
	    charx = RULER_WIDTH - INCH_MARK - char_width(bold_font);
	    }
	else {
	    start = 0;
	    charx = INCH_MARK + 2;
	    }

	charht = char_height(bold_font);
	for (d = number, i = SIDERULER_START + charht / 2 - 3;
		i < SIDERULER_HEIGHT; 
		d++, i += PIX_PER_INCH) {
	    pw_text(sideruler_pixwin, charx, i, PAINT, bold_font, *d);
	    }
	qinch = PIX_PER_INCH / 4;
	sinch = PIX_PER_INCH / 16;
	/*  Draw Scale  */
	end = abs(start - INCH_MARK);
	for (i = SIDERULER_START - 1; i < SIDERULER_HEIGHT; i += PIX_PER_INCH) 
	    pw_vector(sideruler_pixwin, start, i, end, i, PAINT, 1);
	end = abs(start - HALF_MARK);
	for (i = SIDERULER_START - 1 + PIX_PER_INCH / 2; 
		i < SIDERULER_HEIGHT; i += PIX_PER_INCH)
	    pw_vector(sideruler_pixwin, start, i, end, i, PAINT, 1);
	end = abs(start - QUARTER_MARK);
	for (i = SIDERULER_START - 1; i < SIDERULER_HEIGHT; i += qinch)
	    pw_vector(sideruler_pixwin, start, i, end, i, PAINT, 1);
	end = abs(start - SIXTEENTH_MARK);
	for (i = SIDERULER_START - 1; i < SIDERULER_HEIGHT; i += sinch)
	    pw_vector(sideruler_pixwin, start, i, end, i, PAINT, 1);
	/*  Darw marker  */
	if (RHS_PANEL) { /*  side ruler is on the LSH of canvas */
	    pw_write(sideruler_pixwin, RULER_WIDTH+srloffx, lasty+srloffy, 
			SRM_WID, SRM_HT, INV_PAINT, &srlm_pr, 0, 0);
	    }
	else { /*  side ruler is on the RSH of canvas */
	    pw_write(sideruler_pixwin, srroffx, lasty+srroffy, 
			SRM_WID, SRM_HT, INV_PAINT, &srrm_pr, 0, 0);
	    }
	}
#endif	X11

#ifndef	X11
static
topruler_sighandler()
{
	pw_damaged(topruler_pixwin);
	pw_repairretained(topruler_pixwin);
	pw_donedamaged(topruler_pixwin);
	}

int
init_topruler(tool)
TOOL		tool;
{
	INPUTMASK		mask;

	topruler_sw = tool_createsubwindow(tool, "top ruler", 
				TOPRULER_WIDTH, TOPRULER_HEIGHT);
	if (topruler_sw == (TOOLSW)0) return(0);
	(int) topruler_sw->ts_data = trswfd = topruler_sw->ts_windowfd;

	topruler_pixwin = pw_open(trswfd);
	topruler_pixwin->pw_prretained = mem_create(TOPRULER_WIDTH, 
						TOPRULER_HEIGHT, 1);
	topruler_sw->ts_io.tio_selected = flush_input;
	input_imnull(&mask);
	win_setinputcodebit(&mask, MS_RIGHT);
	win_setinputmask(trswfd, &mask, NULL, WIN_NULLLINK);
	win_setcursor(srswfd, &bull_cursor);
	topruler_sw->ts_io.tio_handlesigwinch = topruler_sighandler;
	topruler_sw->ts_destroy = null_proc;

	show_topruler();
	return(1);
	}
#else
int init_topruler(tool)
	TOOL		tool;
{
	ruler_args[2].value = TOPRULER_WIDTH = CANVAS_WIDTH;;
	ruler_args[3].value = TOPRULER_HEIGHT = RULER_WIDTH;
	ruler_args[5].value = (XtArgVal)panel_sw;
	ruler_args[7].value = (XtArgVal)NULL;
	/* fix the top & bottom to the top of the form,
	   the left to the panel, and the right is rubber */
	ruler_args[10].value = (XtArgVal)XtChainTop;
	ruler_args[11].value = (XtArgVal)XtChainTop;
	ruler_args[12].value = (XtArgVal)XtRubber;
	ruler_args[13].value = (XtArgVal)XtRubber;
	
	topruler_sw = XtCreateWidget("truler", labelWidgetClass, tool,
		ruler_args, XtNumber(ruler_args));
	return(1);
}

redisplay_topruler()
{
	XClearWindow(tool_d, topruler_pixwin);
}
#endif	X11

#ifndef	X11
show_topruler()
{
	int	 i, qinch, len, sinch, charwid;
	char	**d;

	pw_writebackground(topruler_pixwin, 0, 0, 2048, 2048, PAINT);

	charwid = char_width(bold_font);
	len = RULER_WIDTH - INCH_MARK;
	for (d = number, d++, i = PIX_PER_INCH - charwid / 2; 
		i < CANVAS_WIDTH; 
		d++, i += PIX_PER_INCH) {
	    pw_text(topruler_pixwin, i, len, PAINT, bold_font, *d);
	    }
	qinch = PIX_PER_INCH / 4;
	sinch = PIX_PER_INCH / 16;
	/*  Draw Scale  */
	len = RULER_WIDTH - INCH_MARK;
	for (i = -1; i < CANVAS_WIDTH; i += PIX_PER_INCH)
	    pw_vector(topruler_pixwin, i, RULER_WIDTH, i, len, PAINT, 1);
	len = RULER_WIDTH - HALF_MARK;
	for (i = -1; i < CANVAS_WIDTH; i += PIX_PER_INCH)
	    pw_vector(topruler_pixwin, i, RULER_WIDTH, i, len, PAINT, 1);
	len = RULER_WIDTH - QUARTER_MARK;
	for (i = -1; i < CANVAS_WIDTH; i += qinch)
	    pw_vector(topruler_pixwin, i, RULER_WIDTH, i, len, PAINT, 1);
	len = RULER_WIDTH - SIXTEENTH_MARK;
	for (i = -1; i < CANVAS_WIDTH; i += sinch)
	    pw_vector(topruler_pixwin, i, RULER_WIDTH, i, len, PAINT, 1);
	/*  Darw marker  */
	pw_write(topruler_pixwin, lastx + troffx, RULER_WIDTH + troffy, 
		TRM_WID, TRM_HT, INV_PAINT, &trm_pr, 0, 0);
	}
#endif	X11

set_rulermark(x, y)
int	x, y;
{
	if( TRACKING )
	{
		set_siderulermark(y);
		set_toprulermark(x);
	}
}

#ifdef	X11
redisplay_rulers()
{
	redisplay_topruler();
	redisplay_sideruler();
}
#endif

set_siderulermark(y)
int	y;
{
	int	sy;

#ifndef	X11
	sy = SIDERULER_START + y;
	if (RHS_PANEL) { /*  side ruler is on the LSH of canvas */
	    pw_write(sideruler_pixwin, RULER_WIDTH+srloffx, lasty+srloffy, 
			SRM_WID, SRM_HT, INV_PAINT, &srlm_pr, 0, 0);
	    pw_write(sideruler_pixwin, RULER_WIDTH+srloffx, sy+srloffy, 
			SRM_WID, SRM_HT, INV_PAINT, &srlm_pr, 0, 0);
	    }
	else { /*  side ruler is on the RSH of canvas */
	    pw_write(sideruler_pixwin, srroffx, lasty+srroffy, 
			SRM_WID, SRM_HT, INV_PAINT, &srrm_pr, 0, 0);
	    pw_write(sideruler_pixwin, srroffx, sy+srroffy, 
			SRM_WID, SRM_HT, INV_PAINT, &srrm_pr, 0, 0);
	    }
	lasty = sy;
#else
	if( RHS_PANEL ) {
		/* Because the ruler uses a background pixmap, we can win
		   here by using XClearArea to erase the old thing. */
		XClearArea(tool_d, sideruler_pixwin,
			   RULER_WIDTH+srloffx, lasty + srloffy,
			   srlm_pr.width, srlm_pr.height, False);
		XCopyArea(tool_d, sidearrow_pm, sideruler_pixwin,
			  sidegc, 0, 0, srlm_pr.width,
			  srlm_pr.height, RULER_WIDTH+srloffx, y + srloffy);
	}
	else
	{
		/* Because the ruler uses a background pixmap, we can win
		   here by using XClearArea to erase the old thing. */
		XClearArea(tool_d, sideruler_pixwin,
			   srroffx, lasty + srroffy,
			   srlm_pr.width, srlm_pr.height, False);
		XCopyArea(tool_d, sidearrow_pm, sideruler_pixwin,
			  sidegc, 0, 0, srrm_pr.width,
			  srrm_pr.height, srroffx, y + srroffy);
	}
	lasty = y;
#endif	X11
}
