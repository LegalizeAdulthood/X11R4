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
#include "font.h"
#include "func.h"
#include "object.h"
#include "paintop.h"

#define			TOLERANCE	3

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			(*return_proc)();
extern			null_proc();
extern			set_popupmenu();

extern F_line		*line_search();
extern F_arc		*arc_search();
extern F_ellipse	*ellipse_search();
extern F_text		*text_search();
extern F_spline		*spline_search();
extern F_compound	*compound_search();

extern int		foreground_color, background_color;
extern int		pointmarker_shown, compoundbox_shown;
extern int		last_object;
extern int		last_action;

			init_move();

move_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_move;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	return_proc = move_selected;
	set_cursor(&pick9_cursor);
	reset_action_on();
	}

init_move(x, y)
int	x, y;
{
	F_line		*l;
	F_arc		*a;
	F_ellipse	*e;
	F_text		*t;
	F_spline	*s;
	F_compound	*c;
	int		px, py;

	if ((c = compound_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    set_temp_cursor(&wait_cursor);
	    win_setmouseposition(canvas_swfd, px, py);
	    if (compoundbox_shown) draw_compoundbox(c, INV_PAINT);
	    erase_compound(c);
	    init_compounddragging(c, px, py);
	    set_temp_cursor(&null_cursor);
	    clean_up();
	    set_action_object(F_MOVE, O_COMPOUND);
	    set_latestcompound(c);
	    }
	else if ((l = line_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    set_temp_cursor(&wait_cursor);
	    erase_pointmarker();
	    win_setmouseposition(canvas_swfd, px, py);
	    draw_line(l, ERASE);
	    init_linedragging(l, px, py);
	    set_temp_cursor(&null_cursor);
	    clean_up();
	    set_action_object(F_MOVE, O_POLYLINE);
	    set_latestline(l);
	    }
	else if ((t = text_search(x, y)) != NULL) {
	    set_temp_cursor(&wait_cursor);
	    erase_pointmarker();
	    pw_text(canvas_pixwin, t->base_x, t->base_y, 
		INV_PAINT, canvas_font, t->cstring);
	    init_textdragging(t, x, y);
	    set_temp_cursor(&null_cursor);
	    clean_up();
	    set_action_object(F_MOVE, O_TEXT);
	    set_latesttext(t);
	    }
	else if ((e = ellipse_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    set_temp_cursor(&wait_cursor);
	    erase_pointmarker();
	    set_temp_cursor(&null_cursor);
	    win_setmouseposition(canvas_swfd, px, py);
	    pw_batch_on(canvas_pixwin);
	    draw_ellipse(e, background_color);
	    pw_batch_off(canvas_pixwin);
	    init_ellipsedragging(e, px, py);
	    set_temp_cursor(&null_cursor);
	    clean_up();
	    set_action_object(F_MOVE, O_ELLIPSE);
	    set_latestellipse(e);
	    }
	else if ((a = arc_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    set_temp_cursor(&wait_cursor);
	    erase_pointmarker();
	    win_setmouseposition(canvas_swfd, px, py);
	    pw_batch_on(canvas_pixwin);
	    draw_arc(a, background_color);
	    pw_batch_off(canvas_pixwin);
	    init_arcdragging(a, px, py);
	    set_temp_cursor(&null_cursor);
	    clean_up();
	    set_action_object(F_MOVE, O_ARC);
	    set_latestarc(a);
	    }
	else if ((s = spline_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    set_temp_cursor(&wait_cursor);
	    win_setmouseposition(canvas_swfd, px, py);
	    erase_pointmarker();
	    pw_batch_on(canvas_pixwin);
	    draw_spline(s, ERASE);
	    pw_batch_off(canvas_pixwin);
	    init_splinedragging(s, px, py);
	    set_temp_cursor(&null_cursor);
	    clean_up();
	    set_action_object(F_MOVE, O_SPLINE);
	    set_latestspline(s);
	    }
	else
	    return;
	canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
	}
