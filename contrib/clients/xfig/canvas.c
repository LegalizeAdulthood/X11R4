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
#include "paintop.h"

/***************** import global variables and routines *******************/

extern int		cur_command;
extern int		errno;
extern int		action_on;     /* tracker.c */
extern int		magnet_mode;

extern int		receiving_msg;
extern int		borderwid, stripeht, windowspacing;
extern int		CANVAS_WIDTH, CANVAS_HEIGHT;
extern			null_proc();
extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();


/*********************** local variables and routines ************************/

static			canvas_selected();
static			canvas_sighandler();

#ifndef	X11
int
init_canvas(tool)
TOOL	tool;
{
	INPUTMASK	 mask;

	canvas_sw = tool_createsubwindow(tool, "", CANVAS_WIDTH, CANVAS_HEIGHT);
	if (canvas_sw == (TOOLSW)0) return(0);
	canvas_swfd = canvas_sw->ts_windowfd;

	canvas_pixwin = pw_open(canvas_swfd);
	canvas_pixwin->pw_prretained = mem_create(CANVAS_WIDTH, 
						CANVAS_HEIGHT, 1);
	canvas_sw->ts_io.tio_selected = canvas_selected;
	canvas_sw->ts_io.tio_handlesigwinch = canvas_sighandler;
	canvas_sw->ts_destroy = null_proc;

	input_imnull(&mask);
	win_setinputcodebit(&mask, MS_LEFT);
	win_setinputcodebit(&mask, MS_MIDDLE);
	win_setinputcodebit(&mask, MS_RIGHT);
	win_setinputcodebit(&mask, LOC_MOVE);
	mask.im_flags |= IM_ASCII | IM_INTRANSIT;
	win_setinputmask(canvas_swfd, &mask, NULL, WIN_NULLLINK);
	set_temp_cursor(cur_cursor);
	(void)fcntl(canvas_swfd, F_SETFL, fcntl(canvas_swfd, F_GETFL, 0) | O_NDELAY);
	init_grid();
	canvas_leftbut_proc = null_proc;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = null_proc;
	canvas_kbd_proc = canvas_locmove_proc = null_proc;
	return(1);
	}

static
canvas_selected(nullsw, ibits, obits, ebits, timer)
caddr_t		*nullsw;
int		*ibits, *obits, *ebits;
struct timeval	*timer;
{
	static INPUTEVENT		ie;
	static int			x, y, t;

	*ibits = *obits = *ebits = 0;
	if (input_readevent(canvas_swfd, &ie) == -1) {
	    printf ("error %d in canvas_selected", errno);
	    return;
	    }

	x = ie.ie_locx;
	y = ie.ie_locy;
	while (ie.ie_code == LOC_MOVE) {
	    if (input_readevent(canvas_swfd, &ie) == -1) {
		if (magnet_mode) {
		    x = ((t = x % 5) < 3) ? x - t - 1 : x + 5 - t - 1;
		    y = ((t = y % 5) < 3) ? y - t - 1 : y + 5 - t - 1;
/*
		    win_setmouseposition(canvas_swfd, x, y);
*/
		    }
		set_rulermark(x, y);
		(*canvas_locmove_proc)(x, y);
		return;
		}
	    x = ie.ie_locx;
	    y = ie.ie_locy;
	    }
	if (magnet_mode) {
	    x = ((t = x % 5) < 3) ? x - t - 1 : x + 5 - t - 1;
	    y = ((t = y % 5) < 3) ? y - t - 1 : y + 5 - t - 1;
/*
	    win_setmouseposition(canvas_swfd, x, y);
*/
	    }
	if (ie.ie_code == MS_LEFT) {
	    canvas_leftbut_proc(x, y);
	    }
	else if (ie.ie_code == MS_RIGHT) {
	    canvas_rightbut_proc(&ie);
	    }
	else if (ie.ie_code == MS_MIDDLE) {
	    canvas_middlebut_proc(x, y);
	    }
	else if (ie.ie_code <= ASCII_LAST) {
	    canvas_kbd_proc(ie.ie_code);
	    }
	}

static
canvas_sighandler()
{
	pw_damaged(canvas_pixwin);
	pw_repairretained(canvas_pixwin);
	pw_donedamaged(canvas_pixwin);
	}

clear_canvas()
{
	pw_writebackground(canvas_pixwin, 0, 0, 2048, 2048, PAINT);
	}
#else
static Arg      canvas_args[] =
{
	{ XtNx, (XtArgVal)0 },
	{ XtNy, (XtArgVal)0 },
	{ XtNlabel, (XtArgVal)"" },
	{ XtNwidth, (XtArgVal)0 },
	{ XtNheight, (XtArgVal)0 },
	{ XtNfromHoriz, (XtArgVal)NULL },
	{ XtNhorizDistance, (XtArgVal)0 },
	{ XtNfromVert, (XtArgVal)NULL },
	{ XtNvertDistance, (XtArgVal)0 },
	{ XtNtop, (XtArgVal)XtChainTop },
};

static void canvas_exposed(tool, event, params, nparams)
	TOOL		tool;
	INPUTEVENT	*event;
	String		*params;
	Cardinal	*nparams;
{
	if (((XExposeEvent *)event)->count > 0)
		return;
	redisplay_canvas();
	redisplay_rulers();
}

static void set_pos(tool, event, params, nparams)
	TOOL		tool;
	XEvent		*event;
	String		*params;
	Cardinal	*nparams;
{
	int		x, y;
	Window		w;
	extern TOOL	menu;
	
	XTranslateCoordinates(tool_d, canvas_swfd, XDefaultRootWindow(tool_d),
			      event->xbutton.x, event->xbutton.y, &x, &y, &w);
	XtMoveWidget(menu, x-10, y-10);
}

XtActionsRec canvas_actions[] =
{
	{ "Event", (XtActionProc)canvas_selected },
	{ "Expose", (XtActionProc)canvas_exposed },
	{ "set_pos", (XtActionProc)set_pos },
};

static String canvas_translations =
	"<Motion>:Event()\n\
	<Btn1Down>:Event()\n\
	<Btn2Down>:Event()\n\
	<Btn3Down>:set_pos(popup_menu)MenuPopup(popup_menu)\n\
	<Key>:Event()\n\
	<Expose>:Expose()\n";

static	Arg	canvas_color_args[] = {
	{ XtNforeground, (XtArgVal) &x_fg_color.pixel },
	{ XtNbackground, (XtArgVal) &x_bg_color.pixel },
};

int init_canvas(tool)
	TOOL		tool;
{
	XColor	fixcolors[2];
	
	canvas_args[3].value = CANVAS_WIDTH;
	canvas_args[4].value = CANVAS_HEIGHT;
	canvas_args[5].value = (XtArgVal)panel_sw;
	canvas_args[7].value = (XtArgVal)topruler_sw;
	canvas_sw = XtCreateWidget("canvas", labelWidgetClass, tool,
		canvas_args, XtNumber(canvas_args));
	XtGetValues(canvas_sw, canvas_color_args, XtNumber(canvas_color_args));

	/* get the RGB values for recolor cursor use -- may want to have
	   cursor color resource */
	fixcolors[0] = x_fg_color;
	fixcolors[1] = x_bg_color;
	XQueryColors(tool_d, DefaultColormapOfScreen(tool_s), fixcolors, 2);
	x_fg_color = fixcolors[0];
	x_bg_color = fixcolors[1];
	
	/* now fix the global GC */
	XSetState(tool_d, gc, x_fg_color.pixel, x_bg_color.pixel, GXcopy,
		  AllPlanes);
	
	/* and recolor the cursors */
	recolor_cursors();
	
	canvas_leftbut_proc = null_proc;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = null_proc;
	canvas_kbd_proc = canvas_locmove_proc = null_proc;
	XtAddActions(canvas_actions, XtNumber(canvas_actions));
	XtOverrideTranslations(canvas_sw,
		XtParseTranslationTable(canvas_translations));
	return (1);
}

setup_canvas()
{
	canvas_rightbut_proc = set_popupmenu;
	canvas_pixwin = canvas_swfd = XtWindow(canvas_sw);
	init_grid();
}

static canvas_selected(tool, event, params, nparams)
	TOOL		tool;
	INPUTEVENT	*event;
	String		*params;
	Cardinal	*nparams;
{
	register int	x, y, t;
	char		buf[1];
	XButtonPressedEvent	*be = (XButtonPressedEvent *)event;
	XKeyPressedEvent	*ke = (XKeyPressedEvent *)event;

	x = event->x;
	y = event->y;
	if (magnet_mode)
	{
		x = ((t = x % 5) < 3) ? x - t - 1 : x + 5 - t - 1;
		y = ((t = y % 5) < 3) ? y - t - 1 : y + 5 - t - 1;
	}
	switch (event->type)
	{
	case MotionNotify:
		{
			Window rw, cw;
			static int sx = -10000, sy = -10000;
			int rx, ry, cx, cy;
			unsigned int mask;

			XQueryPointer(event->display, event->window,
				      &rw, &cw,
				      &rx, &ry,
				      &cx, &cy,
				      &mask);
			if(cx == sx && cy == sy) break;
			x = sx = cx;
			y = sy = cy;
		}
		set_rulermark(x, y);
		(*canvas_locmove_proc)(x, y);
		break;
	case ButtonPress:
		if (be->button & Button1)
			(*canvas_leftbut_proc)(x, y);
		else if (be->button & Button3)
			(*canvas_middlebut_proc)(x, y);
		break;
	case KeyPress:
		if (XLookupString(ke, buf, sizeof(buf), NULL, NULL) > 0)
			(*canvas_kbd_proc)(buf[0]);
		break;
	}
}

clear_canvas()
{
	XClearArea(tool_d, canvas_pixwin, 0, 0, 0, 0, False);
}
#endif	X11
