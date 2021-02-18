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
#include "alloc.h"
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

extern int		pointmarker_shown;

extern F_compound	objects;
extern F_line		*line_search();
extern F_spline		*spline_search();

			init_turn();

turn_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_turn;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick15_cursor);
	}

init_turn(x, y)
int	x, y;
{
	F_line		*l;
	F_spline	*s;
	int		dummy;

	if ((l = line_search(x, y, TOLERANCE, &dummy, &dummy)) != NULL) {
	    if (l->type == T_BOX) return;
	    line_2_spline(l);
	    turn_selected();
	    }
	else if ((s = spline_search(x, y, TOLERANCE, &dummy, &dummy)) != NULL) {
	    spline_2_line(s);
	    turn_selected();
	    }
	}

line_2_spline(l)
F_line	*l;
{
	F_spline	*s;

	if (num_points(l->points) < 3) {
	    put_msg("Can't turn this line into spline");
	    return(-1);
	    }
	if (pointmarker_shown) toggle_linepointmarker(l);
	draw_line(l, ERASE);
	delete_line(&objects.lines, l);

	if (NULL == (Spline_malloc(s))) {
	    put_msg(Err_mem);
	    return(-1);
	    }

	if (l->type == T_POLYGON)
	    s->type = T_CLOSED_INTERPOLATED;
	else
	    s->type = T_OPEN_INTERPOLATED;
	s->style = l->style;
	s->thickness = l->thickness;
	s->color = l->color;
	s->depth = l->depth;
	s->style_val = l->style_val;
	s->pen = l->pen;
	s->area_fill = l->area_fill;
	s->for_arrow = l->for_arrow;
	s->back_arrow = l->back_arrow;
	s->points = l->points;
	s->controls = NULL;
	s->next = NULL;

	l->for_arrow = l->back_arrow = NULL;
	l->area_fill = NULL;
	l->pen = NULL;
	l->points = NULL;

	if (-1 == create_control_list(s)) {
	    free(s);
	    return(-1);
	    }

	remake_control_points(s);
	draw_spline(s, PAINT);
	if (pointmarker_shown) toggle_splinepointmarker(s);
	clean_up();
	set_action_object(F_TURN, O_POLYLINE);
	insert_spline(&objects.splines, s);
	set_latestspline(s);
	set_latestline(l);
	return(1);
	}

spline_2_line(s)
F_spline	*s;
{
	F_line	*l;

	if (pointmarker_shown) toggle_splinepointmarker(s);
	draw_spline(s, ERASE);
	delete_spline(&objects.splines, s);

	/* Now we turn s into a line */
	if (NULL == (Line_malloc(l))) {
	    put_msg(Err_mem);
	    return(-1);
	    }

	if (s->type == T_OPEN_INTERPOLATED)
	    l->type = T_POLYLINE;
	else if (s->type == T_CLOSED_INTERPOLATED)
	    l->type = T_POLYGON;
	l->style = s->style;
	l->thickness = s->thickness;
	l->color = s->color;
	l->depth = s->depth;
	l->style_val = s->style_val;
	l->pen = s->pen;
	l->area_fill = s->area_fill;
	l->for_arrow = s->for_arrow;
	l->back_arrow = s->back_arrow;
	l->points = s->points;
	l->next = NULL;

	s->for_arrow = s->back_arrow = NULL;
	s->area_fill = NULL;
	s->pen = NULL;
	s->points = NULL;

	draw_line(l, PAINT);
	if (pointmarker_shown) toggle_linepointmarker(l);
	clean_up();
	set_action_object(F_TURN, O_SPLINE);
	insert_line(&objects.lines, l);
	set_latestspline(s);
	set_latestline(l);
	return(1);
	}
