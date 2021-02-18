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
#include "func.h"
#include "object.h"
#include "paintop.h"

extern			null_proc();

/**************     local variables and routines   **************/

#ifdef	X11
typedef struct
{
	TOOL	widget;
	Pixmap	normal, reverse;
} button_info;
#endif	X11

#include "panel.h"

static			panel_selected();
static			panel_sighandler();
static			init_switch();
static F_switch		*switch_selected();
static			panel_selected();
static			switch_handler();
static			switch_action();
static			mode_on();
static			mode_off();
static			set_command();
static			set_geometry();
static			set_grid();
static			set_style();

#define			on_action(z)	(z->on_func)(z)
#define			off_action(z)	(z->off_func)(z)

static F_switch		switches[] = { 
	{ 0, 0, 0, 0, &cirrad_ic, F_CIRCLE_BY_RAD, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 0, &cirdia_ic, F_CIRCLE_BY_DIA, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 1, &ellrad_ic, F_ELLIPSE_BY_RAD, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 1, &elldia_ic, F_ELLIPSE_BY_DIA, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 2, &c_spl_ic, F_CLOSED_SPLINE, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 2, &spl_ic, F_SPLINE, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 3, &c_intspl_ic, F_CLOSED_INTSPLINE, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 3, &intspl_ic, F_INTSPLINE, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 4, &box_ic, F_BOX, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 4, &polygon_ic, F_POLYGON, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 5, &line_ic, F_POLYLINE, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 5, &text_ic, F_TEXT, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 6, &arc_ic, F_CIRCULAR_ARC, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 6, &turn_ic, F_TURN, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 8, &glue_ic, F_GLUE, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 8, &break_ic, F_BREAK, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 9, &scale_ic, F_SCALE, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 9, &autoarrow_ic, F_AUTOARROW, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 10, &addpt_ic, F_ADD_POINT, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 10, &deletept_ic, F_DELETE_POINT, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 11, &move_ic, F_MOVE, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 11, &movept_ic, F_MOVE_POINT, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 12, &copy_ic, F_COPY, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 12, &remove_ic, F_REMOVE, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 13, &flip_x_ic, F_FLIP_XAXIS, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 13, &flip_y_ic, F_FLIP_YAXIS, set_command, null_proc, S_ON, },
	{ 0, 0, 0, 14, &rot90_ic, F_ROTATE90, set_command, null_proc, S_ON, },
	{ 0, 0, 1, 14, &rot270_ic, F_ROTATE270, set_command, null_proc, S_ON, },
	{ 0, 1, 0, 16, &grid1_ic, F_GRID1, set_grid, set_grid, S_TOG, },
	{ 0, 1, 1, 16, &grid2_ic, F_GRID2, set_grid, set_grid, S_TOG, },
	{ 1, 2, 0, 17, &unconstraint_ic, F_UNCONSTRAINT, set_geometry, null_proc, S_ON, },
	{ 0, 2, 1, 17, &mounthattan_ic, F_MOUNTHATTAN, set_geometry, null_proc, S_ON, },
	{ 0, 2, 0, 18, &manhattan_ic, F_MANHATTAN, set_geometry, null_proc, S_ON, },
	{ 0, 2, 1, 18, &mountain_ic, F_MOUNTAIN, set_geometry, null_proc, S_ON, },
	{ 0, 2, 0, 19, &latexline_ic, F_LATEX_LINE, set_geometry, null_proc, S_ON, },
	{ 0, 2, 1, 19, &latexarrow_ic, F_LATEX_ARROW, set_geometry, null_proc, S_ON, },
	{ 0, -1, 0, 20, &backarrow_ic, F_AUTOB_ARROW, mode_on, mode_off, S_TOG, },
	{ 0, -1, 1, 20, &forarrow_ic, F_AUTOF_ARROW, mode_on, mode_off, S_TOG, },
	{ 1, 3, 0, 21, &solidline_ic, F_SET_SOLID_LINE, set_style, null_proc, S_ON, },
	{ 0, 3, 1, 21, &dashline_ic, F_SET_DASH_LINE, set_style, null_proc, S_ON, },
	{ 0, 3, 1, 22, &dottedline_ic, F_SET_DOTTED_LINE, set_style, null_proc, S_ON, },
	{ 1, -1, 1, 23, &magnet_ic, F_MAGNET, mode_on, mode_off, S_TOG, },
	};

#define			NUM_GROUP	4
#define			NUM_SWITCH	(sizeof(switches) / sizeof(F_switch))
static F_switch		*group[NUM_GROUP];

/*
 *	Panel subwindow section
 */

#ifndef	X11
init_panel(tool)
TOOL		tool;
{
	INPUTMASK		mask;

	panel_sw = tool_createsubwindow(tool, "", -1, -1);
	if (panel_sw == (TOOLSW) 0) return(0);
	panel_swfd = panel_sw->ts_windowfd;
	panel_pixwin = pw_open(panel_swfd);

	panel_sw->ts_io.tio_selected = panel_selected;
	panel_sw->ts_io.tio_handlesigwinch = panel_sighandler;
	panel_sw->ts_destroy = null_proc;

	input_imnull(&mask);
	win_setinputcodebit(&mask, MS_LEFT);
	win_setinputcodebit(&mask, MS_MIDDLE);
	win_setinputcodebit(&mask, MS_RIGHT);
	win_setinputmask(panel_swfd, &mask, NULL, WIN_NULLLINK);
	win_setcursor(panel_swfd, &arrow_cursor);
	(void)fcntl(panel_swfd, F_SETFL, fcntl(panel_swfd, F_GETFL, 0) | O_NDELAY);
	init_switch();
	return(1);
	}
#else
static Arg	panel_args[] =
{
	{ XtNx, (XtArgVal)0 },
	{ XtNy, (XtArgVal)0 },
	{ XtNwidth, (XtArgVal)0 },
	{ XtNheight, (XtArgVal)0 },
	{ XtNhSpace, (XtArgVal)SWITCH_ICON_SPACING },
	{ XtNvSpace, (XtArgVal)SWITCH_ICON_SPACING },
	/* Fix the size of the panel window by chaining both top & bottom
	   to the top, and both left & right to the right */
	{ XtNtop, (XtArgVal)XtChainTop},
	{ XtNbottom, (XtArgVal)XtChainTop},
	{ XtNleft, (XtArgVal)XtChainLeft },
	{ XtNright, (XtArgVal)XtChainLeft },
	{ XtNresizable, (XtArgVal) FALSE },
};

extern void button_select();

static XtCallbackRec    button_callbacks[] =
{
	{ button_select, NULL },
	{ NULL, NULL },
};

static Arg	button_args[] =
{
	{ XtNx, (XtArgVal)0 },
	{ XtNy, (XtArgVal)0 },
	{ XtNlabel, (XtArgVal)"        " },
	{ XtNwidth, (XtArgVal)0 },
	{ XtNheight, (XtArgVal)0 },
	{ XtNbackgroundPixmap, (XtArgVal)NULL },
	{ XtNcallback, (XtArgVal)button_callbacks },
	{ XtNresizable, (XtArgVal) FALSE },
};

extern int	PANEL_LEFT, PANEL_TOP, PANEL_HEIGHT, PANEL_WID;
extern int	CANVAS_HEIGHT, ICON_COLUMN;

#define	N_SWITCHES	(sizeof(switches)/sizeof(switches[0]))

int init_panel(tool)
	TOOL		tool;
{
	register int		i;
	register F_switch	*sw;

	panel_args[2].value = PANEL_WID =
		(SWITCH_ICON_WIDTH + 2 + SWITCH_ICON_SPACING) * ICON_COLUMN
/* 2 for borders on switches */
		+ SWITCH_ICON_SPACING;
	panel_args[3].value = PANEL_HEIGHT = CANVAS_HEIGHT + RULER_WIDTH + MSG_HEIGHT;
	panel_sw = XtCreateWidget("panel", boxWidgetClass, tool, panel_args,
		XtNumber(panel_args));
	for (i = 0; i < N_SWITCHES; ++i)
	{
		sw = &switches[i];
		button_args[0].value = sw->x * sw->icon->width;
		button_args[1].value = sw->y * sw->icon->height;
		button_args[3].value = sw->icon->width;
		button_args[4].value = sw->icon->height;
		button_callbacks[0].closure = (caddr_t)sw;
		sw->but.widget = XtCreateManagedWidget(
			"button", commandWidgetClass,
			panel_sw, button_args, XtNumber(button_args));
	}
	return (1);
}

setup_panel()
{
	register int		i;
	register F_switch	*sw;
	register Display	*d = tool_d;
	register Screen		*s = tool_s;
	register Pixmap		p;
	register GC		cgc;
	XGCValues		gcv;
	Arg			tmp_arg[3];
	long			bg, fg;
	
	cgc = XCreateGC(d, XtWindow(panel_sw), 0, &gcv);
	XtSetArg(tmp_arg[0], XtNbackground, &bg);
	XtSetArg(tmp_arg[1], XtNforeground, &fg);
	XtGetValues(switches[0].but.widget, tmp_arg, 2);
	XSetForeground(d, cgc, fg);
	XSetBackground(d, cgc, bg);
	
	for (i = 0; i < N_SWITCHES; ++i)
	{
		sw = &switches[i];
/*
**	You'd think XCreateBitmapFromBitmapData would work but
**	unfortunately it wants LSB first data.
*/
		p = XCreatePixmap(d, XtWindow(sw->but.widget),
				  sw->icon->width, sw->icon->height,
				  DefaultDepthOfScreen(s));
		XPutImage(d, p, cgc, sw->icon, 0, 0, 0, 0, sw->icon->width,
			sw->icon->height);
		sw->but.normal = button_args[5].value = (XtArgVal)p;
		XtSetValues(sw->but.widget, &button_args[5], 1);
	}
/*
**	Create reversed bitmaps for displaying activated state
*/
	XSetForeground(d, cgc, bg);
	XSetBackground(d, cgc, fg);
	for (i = 0; i < N_SWITCHES; ++i)
	{
		sw = &switches[i];
		p = XCreatePixmap(d, XtWindow(sw->but.widget),
				  sw->icon->width, sw->icon->height,
				  DefaultDepthOfScreen(s));
		XPutImage(d, p, cgc, sw->icon, 0, 0, 0, 0, sw->icon->width,
			sw->icon->height);
		sw->but.reverse = (XtArgVal)p;
	}
	XFreeGC(d, cgc);
	init_switch();
	panel_pixwin = panel_swfd = XtWindow(panel_sw);
	XDefineCursor(d, panel_swfd, (Cursor)arrow_cursor.bitmap);
}

static void button_select(widget, s)
	TOOL		widget;
	F_switch	*s;
{
	switch_action(s);
}
#endif	X11

#ifndef	X11
static
panel_selected(nullsw, ibits, obits, ebits, timer)
caddr_t		*nullsw;
int		*ibits, *obits, *ebits;
struct timeval	*timer;
{
	extern int		action_on;
	extern int		receiving_msg;
	INPUTEVENT		ie;
	int			x, y;

	x = -1;
	while (input_readevent(panel_swfd, &ie) != -1) {
	    if (ie.ie_code == MS_LEFT) {
		x = ie.ie_locx;
		y = ie.ie_locy;
		}
	    }
	*ibits = *obits = *ebits = 0;
	if (action_on) return;
	if (receiving_msg) return;  /* ignore mouse input when msg is being
					accepted	*/
	if (x != -1) switch_handler(x, y);
	}

static
panel_sighandler()
{
	pw_damaged(panel_pixwin);
	pw_writebackground(panel_pixwin, 0, 0, 2048, 2048, PAINT);
	redisplay_panel();
	pw_donedamaged(panel_pixwin);
	}

redisplay_panel()
{
	F_switch	*s, *t;

	t = &switches[NUM_SWITCH-1];
	for (s = switches; s <= t; s++) {
	    if (s->on == 1)
		turn_on(s);
	    else 
		turn_off(s);
	    }
	}

static F_switch *
switch_selected(x, y)
int	x, y;
{
	F_switch	*sw;
	int		i;

	for (sw = switches, i = 0; i < NUM_SWITCH; sw++, i++) {
	    if (sw->x > x || sw->x + SWITCH_ICON_WIDTH < x) continue;
	    if (sw->y <= y && y <= sw->y + SWITCH_ICON_HEIGHT) return(sw);
	    }
	return(NULL); /* no item is selected */
	}
#endif	X11

static
init_switch()
{
	extern int	manhattan_mode;
	extern int	mountain_mode;
	extern int	autoforwardarrow_mode;
	extern int	autobackwardarrow_mode;
	extern int	latexline_mode;
	extern int	latexarrow_mode;
	extern int	magnet_mode;
	extern int	cur_line_style;
	extern float	cur_styleval;
	extern float	cur_dashlength;
	extern float	cur_dotgap;
	int		i;
	F_switch	*sw;

	for (sw = switches, i = 0; i < NUM_SWITCH; sw++, i++) {
	    sw->x *= SWITCH_ICON_WIDTH;
	    sw->y *= SWITCH_ICON_HEIGHT;
	    }
	for (i = 0; i < NUM_GROUP; ) group[i++] = NULL;

	for (sw = switches, i = 0; i < NUM_SWITCH; sw++, i++) {
	    if (sw->on) {
		/* Only the following switch can be preset */
		switch (sw->value) {
		    case F_UNCONSTRAINT :
			manhattan_mode = 0;
			mountain_mode = 0;
			latexline_mode = 0;
			latexarrow_mode = 0;
			group[sw->group] = sw;
			break;
		    case F_MOUNTHATTAN :
			manhattan_mode = 1;
			mountain_mode = 1;
			group[sw->group] = sw;
			break;
		    case F_MANHATTAN :
			manhattan_mode = 1;
			group[sw->group] = sw;
			break;
		    case F_MOUNTAIN :
			mountain_mode = 1;
			group[sw->group] = sw;
			break;
		    case F_LATEX_LINE :
			latexline_mode = 1;
			group[sw->group] = sw;
			break;
		    case F_LATEX_ARROW :
			latexarrow_mode = 1;
			group[sw->group] = sw;
			break;
		    case F_AUTOF_ARROW :
			autoforwardarrow_mode = 1;
			break;
		    case F_AUTOB_ARROW :
			autobackwardarrow_mode = 1;
			break;
		    case F_SET_SOLID_LINE :
			cur_line_style = SOLID_LINE;
			cur_styleval = 0.0;
			break;
		    case F_SET_DASH_LINE :
			cur_line_style = DASH_LINE;
			cur_styleval = cur_dashlength;
			break;
		    case F_SET_DOTTED_LINE :
			cur_line_style = DOTTED_LINE;
			cur_styleval = cur_dotgap;
			break;
		    case F_MAGNET :
			magnet_mode = 1;
			break;
		    default :
			continue;
		    }
		turn_on(sw);
		if (sw->group != -1) group[sw->group] = sw;
		}
	    }
	}

#ifndef	X11
static
switch_handler(x, y)
int	x, y;
{
	F_switch *s;

	if (NULL == (s = switch_selected(x, y))) return;
	switch_action(s);
	}
#endif	X11

static
set_command(sw)
F_switch	*sw;
{
	extern int	cur_command;
	extern int	rotate_angle;
	extern int	flip_axis;

	switch (sw->value) {
	    case F_CIRCLE_BY_RAD :
		circlebyradius_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("CIRCLE drawing: specify RADIUS");
		break;
	    case F_CIRCLE_BY_DIA :
		circlebydiameter_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("CIRCLE drawing: specify DIAMETER");
		break;
	    case F_ELLIPSE_BY_RAD :
		ellipsebyradius_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("ELLIPSE drawing: specify RADIUSES");
		break;
	    case F_ELLIPSE_BY_DIA :
		ellipsebydiameter_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("ELLIPSE drawing: specify DIAMETERS");
		break;
	    case F_BOX :
		box_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("Rectangular BOX drawing");
		break;
	    case F_POLYGON :
		line_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("POLYGON drawing");
		break;
	    case F_POLYLINE :
		line_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("POLYLINE drawing");
		break;
	    case F_TEXT :
		text_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("TEXT input (from keyboard)");
		break;
	    case F_CIRCULAR_ARC :
		arc_drawing_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("ARC drawing: specify three points on the arc");
		break;
	    case F_SPLINE :
		draw_spline_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("SPLINE drawing: specify control points");
		break;
	    case F_CLOSED_SPLINE :
		draw_spline_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("CLOSED SPLINE drawing: specify control points");
		break;
	    case F_INTSPLINE :
		draw_intspline_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("INTERPOLATED SPLINE drawing");
		break;
	    case F_CLOSED_INTSPLINE :
		draw_intspline_selected();
		erase_pointmarker();
		erase_compoundbox();
		put_msg("CLOSED INTERPOLATED SPLINE drawing");
		break;
	    case F_GLUE :
		compound_selected();
		erase_pointmarker();
		show_compoundbox();
		put_msg("GLUE objects into COMPOUND object with bounding box");
		break;
	    case F_BREAK :
		break_selected();
		erase_pointmarker();
		show_compoundbox();
		put_msg("BREAK COMPOUND object");
		break;
	    case F_SCALE :
		scale_compound_selected();
		erase_pointmarker();
		show_compoundbox();
		put_msg("SCALE COMPOUND object");
		break;
	    case F_ADD_POINT :
		point_adding_selected();
		show_pointmarker();
		erase_compoundbox();
		put_msg("ADD POINTs (to POLYLINE, POLYGON, CLOSED-SPLINE and SPLINE)");
		break;
	    case F_DELETE_POINT :
		delete_point_selected();
		show_pointmarker();
		erase_compoundbox();
		put_msg("DELETE POINTs (from POLYLINE, POLYGON, CLOSED-SPLINE and SPLINE)");
		break;
	    case F_MOVE :
		move_selected();
		show_pointmarker();
		show_compoundbox();
		put_msg("MOVE objects");
		break;
	    case F_MOVE_POINT :
		move_point_selected();
		show_pointmarker();
		erase_compoundbox();
		put_msg("MOVE POINTs (of POLYLINE, POLYGON, CLOSED-SPLINE, SPLINE and BOX)");
		break;
	    case F_REMOVE :
		remove_selected();
		show_pointmarker();
		show_compoundbox();
		put_msg("REMOVE objects");
		break;
	    case F_COPY :
		copy_selected();
		show_pointmarker();
		show_compoundbox();
		put_msg("COPY objects");
		break;
	    case F_ROTATE270 :
		rotate_selected();
		show_pointmarker();
		show_compoundbox();
		rotate_angle = 270;
		put_msg("ROTATE objects (middle button) or COPY & ROTATE (left button) -90 degree");
		break;
	    case F_ROTATE90 :
		rotate_selected();
		show_pointmarker();
		show_compoundbox();
		rotate_angle = 90;
		put_msg("ROTATE objects (middle button) or COPY & ROTATE (left button) 90 degree");
		break;
	    case F_FLIP_XAXIS :
		flip_selected();
		show_pointmarker();
		show_compoundbox();
		flip_axis = 1;
		put_msg("FLIP objects (middle button) or COPY & FLIP (left button) up or down");
		break;
	    case F_FLIP_YAXIS :
		flip_selected();
		show_pointmarker();
		show_compoundbox();
		flip_axis = 2;
		put_msg("FLIP objects (middle button) or COPY & FLIP (left button) left or right");
		break;
	    case F_TURN :
		turn_selected();
		show_pointmarker();
		erase_compoundbox();
		put_msg("Turn POLYGON (POLYLINE) into CLOSED-SPLINE (SPLINE) or vice versa");
		break;
	    case F_AUTOARROW :
		arrow_head_selected();
		show_pointmarker();
		erase_compoundbox();
		put_msg("ADD arrow head (left button); DELETE arrow head (middle button)");
		break;
	    }
	cur_command = sw->value;
	}

static
set_geometry(sw)
F_switch	*sw;
{
	extern int	manhattan_mode;
	extern int	mountain_mode;
	extern int	latexline_mode;
	extern int	latexarrow_mode;

	switch (sw->value) {
	    case F_UNCONSTRAINT :
		manhattan_mode = 0;
		mountain_mode = 0;
		latexline_mode = 0;
		latexarrow_mode = 0;
		put_msg("UNCONSTRAINT geometry (for POLYLINE and SPLINE)");
		break;
	    case F_MOUNTHATTAN :
		mountain_mode = 1;
		manhattan_mode = 1;
		latexline_mode = 0;
		latexarrow_mode = 0;
		put_msg("MOUNT-HATTAN geometry (for POLYLINE and SPLINE)");
		break;
	    case F_MANHATTAN :
		manhattan_mode = 1;
		mountain_mode = 0;
		latexline_mode = 0;
		latexarrow_mode = 0;
		put_msg("MANHATTAN geometry (for POLYLINE and SPLINE)");
		break;
	    case F_MOUNTAIN :
		mountain_mode = 1;
		manhattan_mode = 0;
		latexline_mode = 0;
		latexarrow_mode = 0;
		put_msg("MOUNTAIN geometry (for POLYLINE and SPLINE)");
		break;
	    case F_LATEX_LINE :
		latexline_mode = 1;
		manhattan_mode = 0;
		mountain_mode = 0;
		latexarrow_mode = 0;
		put_msg("LATEX LINE geometry: allow only LaTeX line slopes");
		break;
	    case F_LATEX_ARROW :
		latexarrow_mode = 1;
		manhattan_mode = 0;
		mountain_mode = 0;
		latexline_mode = 0;
		put_msg("LATEX ARROW geometry: allow only LaTeX arrow slopes");
		break;
	    }
	}

static
set_grid(sw)
F_switch	*sw;
{
	setup_grid(sw->value);
	}

static
mode_on(sw)
F_switch	*sw;
{
	extern int	autoforwardarrow_mode;
	extern int	autobackwardarrow_mode;
	extern int	magnet_mode;

	switch (sw->value) {
	    case F_AUTOF_ARROW :
		autoforwardarrow_mode = 1;
		put_msg("AUTO FORWARD ARROW (for ARC, POLYLINE and SPLINE)");
		break;
	    case F_AUTOB_ARROW :
		autobackwardarrow_mode = 1;
		put_msg("AUTO BACKWARD ARROW (for ARC, POLYLINE and SPLINE)");
		break;
	    case F_MAGNET :
		magnet_mode = 1;
		put_msg("MAGNET: round entered points to the nearest 1/16\" increment");
		break;
	    }
	}

static
mode_off(sw)
F_switch	*sw;
{
	extern int	autoforwardarrow_mode;
	extern int	autobackwardarrow_mode;
	extern int	magnet_mode;

	switch (sw->value) {
	    case F_AUTOF_ARROW :
		autoforwardarrow_mode = 0;
		break;
	    case F_AUTOB_ARROW :
		autobackwardarrow_mode = 0;
		break;
	    case F_MAGNET :
		magnet_mode = 0;
		break;
	    }
	}

static
set_style(sw)
F_switch	*sw;
{
	extern int	cur_line_style;
	extern float	cur_styleval;
	extern float	cur_dashlength;
	extern float	cur_dotgap;

	switch (sw->value) {
	    case F_SET_SOLID_LINE :
		cur_line_style = SOLID_LINE;
		cur_styleval = 0.0;
		put_msg("SOLID LINE STYLE (for BOX, POLYGON and POLYLINE)");
		break;
	    case F_SET_DASH_LINE :
		cur_line_style = DASH_LINE;
		cur_styleval = cur_dashlength;
		put_msg("DASH LINE STYLE (for BOX, POLYGON and POLYLINE)");
		break;
	    case F_SET_DOTTED_LINE :
		cur_line_style = DOTTED_LINE;
		cur_styleval = cur_dotgap;
		put_msg("DOTTED LINE STYLE (for BOX, POLYGON and POLYLINE)");
		break;
	    }
	}

#ifndef	X11
static
turn_on(s)
F_switch	*s;
{
	s->on = 1;
	pw_write(panel_pixwin, s->x, s->y,
		SWITCH_ICON_WIDTH, SWITCH_ICON_HEIGHT, ERASE, s->icon, 0, 0);
	}

static
turn_off(s)
F_switch	*s;
{
	s->on = 0;
	pw_write(panel_pixwin, s->x, s->y,
		SWITCH_ICON_WIDTH, SWITCH_ICON_HEIGHT, PAINT,
		s->icon, 0, 0);
	}
#else
static turn_on(s)
	F_switch	*s;
{
	s->on = 1;
	button_args[5].value = (XtArgVal)s->but.reverse;
	XtSetValues(s->but.widget, &button_args[5], 1);
}

static turn_off(s)
	F_switch	*s;
{
	s->on = 0;
	button_args[5].value = (XtArgVal)s->but.normal;
	XtSetValues(s->but.widget, &button_args[5], 1);
}
#endif	X11

static
switch_action(sw)
F_switch	*sw;
{
	F_switch	*old;

	if (sw == NULL) return;

	if (sw->group == -1) {
	    if (sw->on) {
		turn_off(sw);
		off_action(sw);
		}
	    else {
		turn_on(sw);
		on_action(sw);
		}
	    return;
	    }

	old = group[sw->group];
	if (old == sw) {
	    if (old->type != S_ON) {
		turn_off(old);
		off_action(old);
		group[old->group] = NULL;
		}
	    return;
	    }
	else if (old) {
	    turn_off(old);
	    off_action(old);
	    }

	turn_on(sw);
	on_action(sw);
	group[sw->group] = sw;
	}
