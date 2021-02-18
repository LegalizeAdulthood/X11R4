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
#include "func.h"
#include "object.h"
#include "paintop.h"

#define			TOLERANCE	7

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();


extern int		foreground_color, background_color;

			init_remove();

remove_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_remove;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&buster_cursor);
	}

init_remove(x, y)
int	x, y;
{
	extern F_line		*line_search();
	extern F_arc		*arc_search();
	extern F_ellipse	*ellipse_search();
	extern F_text		*text_search();
	extern F_spline		*spline_search();
	extern F_compound	*compound_search();
	extern F_compound	objects;
	F_line		*l;
	F_arc		*a;
	F_ellipse	*e;
	F_text		*t;
	F_spline	*s;
	F_compound	*c;
	int		dummy;

	if ((c = compound_search(x, y, TOLERANCE, &dummy, &dummy)) != NULL) {
	    draw_compoundbox(c, INV_PAINT);
	    erase_compound(c);
	    delete_compound(&objects.compounds, c);
	    clean_up();
	    set_action_object(F_REMOVE, O_COMPOUND);
	    set_latestcompound(c);
	    set_modifiedflag();
	    }
	else if ((l = line_search(x, y, TOLERANCE, &dummy, &dummy)) != NULL) {
	    toggle_linepointmarker(l);
	    draw_line(l, ERASE);
	    delete_line(&objects.lines, l);
	    clean_up();
	    set_action_object(F_REMOVE, O_POLYLINE);
	    set_latestline(l);
	    set_modifiedflag();
	    }
	else if ((t = text_search(x, y)) != NULL) {
	    draw_text(t, INV_PAINT);
	    delete_text(&objects.texts, t);
	    clean_up();
	    set_action_object(F_REMOVE, O_TEXT);
	    set_latesttext(t);
	    set_modifiedflag();
	    }
	else if ((e = ellipse_search(x, y, TOLERANCE, &dummy, &dummy)) != NULL){
	    toggle_ellipsepointmarker(e);
	    pw_batch_on(canvas_pixwin);
	    draw_ellipse(e, background_color);
	    pw_batch_off(canvas_pixwin);
	    delete_ellipse(&objects.ellipses, e);
	    clean_up();
	    set_action_object(F_REMOVE, O_ELLIPSE);
	    set_latestellipse(e);
	    set_modifiedflag();
	    }
	else if ((a = arc_search(x, y, TOLERANCE, &dummy, &dummy)) != NULL){
	    toggle_arcpointmarker(a);
	    pw_batch_on(canvas_pixwin);
	    draw_arc(a, background_color);
	    pw_batch_off(canvas_pixwin);
	    delete_arc(&objects.arcs, a);
	    clean_up();
	    set_action_object(F_REMOVE, O_ARC);
	    set_latestarc(a);
	    set_modifiedflag();
	    }
	else if ((s = spline_search(x, y, TOLERANCE, &dummy, &dummy)) != NULL) {
	    toggle_splinepointmarker(s);
	    pw_batch_on(canvas_pixwin);
	    draw_spline(s, ERASE);
	    pw_batch_off(canvas_pixwin);
	    delete_spline(&objects.splines, s);
	    clean_up();
	    set_action_object(F_REMOVE, O_SPLINE);
	    set_latestspline(s);
	    set_modifiedflag();
	    }
	remove_selected();
	}

remove_all()
{
	extern F_compound	objects;
	extern F_compound	saved_objects;
	extern int		last_action;

	clean_up();
	set_action_object(F_REMOVE, O_ALL_OBJECT);

	/* Aggregate assignment between variables is allowed,
	but not from constant (weird!?) */

	saved_objects = objects;

	objects.arcs = NULL;
	objects.compounds = NULL;
	objects.ellipses = NULL;
	objects.lines = NULL;
	objects.splines = NULL;
	objects.texts = NULL;
	}
