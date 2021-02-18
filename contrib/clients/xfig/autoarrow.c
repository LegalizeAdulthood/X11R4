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
extern F_line		*line_point_search();
extern F_spline		*spline_point_search();
extern F_arc		*arc_point_search();
extern F_arrow		*forward_arrow(), *backward_arrow();

extern int		foreground_color, background_color;
extern int		pointmarker_shown;

F_point			*selected_point, *left_point;
int			arcpoint_num;
			add_arrow_head();
			delete_arrow_head();

#define			round(x)	((int) (x + .5))

arrow_head_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = add_arrow_head;
	canvas_middlebut_proc = delete_arrow_head;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick9_cursor);
	}

add_arrow_head(x, y)
int	x, y;
{
	F_line		*line;
	F_spline	*spline;
	F_arc		*arc;

	if ((line = line_point_search(x, y, TOLERANCE,
		&left_point, &selected_point)) != NULL) {
		add_linearrow(line);
	    }
	else if ((spline = spline_point_search(x, y, 
		TOLERANCE, &left_point, &selected_point)) != NULL){
		add_splinearrow(spline);
	    }
	else if ((arc = arc_point_search(x, y, TOLERANCE, 
		&arcpoint_num)) != NULL) {
		add_arcarrow(arc);
	    }
	else
	    return;

	set_modifiedflag();
	}

delete_arrow_head(x, y)
int	x, y;
{
	F_line		*line;
	F_spline	*spline;
	F_arc		*arc;

	if ((line = line_point_search(x, y, TOLERANCE,
		&left_point, &selected_point)) != NULL) {
		delete_linearrow(line);
	    }
	else if ((spline = spline_point_search(x, y, 
		TOLERANCE, &left_point, &selected_point)) != NULL){
		delete_splinearrow(spline);
	    }
	else if ((arc = arc_point_search(x, y, TOLERANCE, 
		&arcpoint_num)) != NULL) {
		delete_arcarrow(arc);
	    }
	else
	    return;

	set_modifiedflag();
	}

add_linearrow(line)
F_line	*line;
{
	F_point	*p;

	if (line->type == T_POLYGON || line->type == T_BOX) return;
	if (line->points->next == NULL) return;	/* A single point line */

	if (left_point == NULL) { /*  selected_point is the first point */
	    if (line->back_arrow) return;
	    p = selected_point->next;
	    line->back_arrow = backward_arrow();
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    draw_arrow(p->x, p->y, selected_point->x, selected_point->y, 
			line->back_arrow, PAINT);
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    }
	else if (selected_point->next == NULL) { /* forward arrow */
	    if (line->for_arrow) return;
	    line->for_arrow = forward_arrow();
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    draw_arrow(left_point->x, left_point->y, 
			selected_point->x, selected_point->y, 
			line->for_arrow, PAINT);
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    }
	clean_up();
	set_action_object(F_ADD_ARROW_HEAD, O_POLYLINE);
	}

add_arcarrow(arc)
F_arc	*arc;
{

	if (arcpoint_num == 0) { /*  backward arrow  */
	    if (arc->back_arrow) return;
	    arc->back_arrow = backward_arrow();
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    draw_arcarrow(arc, foreground_color);
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    }
	else if (arcpoint_num == 2) { /*  for_arrow  */
	    if (arc->for_arrow) return;
	    arc->for_arrow = forward_arrow();
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    draw_arcarrow(arc, foreground_color);
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    }
	clean_up();
	set_action_object(F_ADD_ARROW_HEAD, O_ARC);
	}

add_splinearrow(spline)
F_spline	*spline;
{
	F_point		*p;
	F_control	*c;

	if (closed_spline(spline)) return;
	if (left_point == NULL) { /* add backward arrow */
	    if (spline->back_arrow) return;
	    p = selected_point->next;
	    spline->back_arrow = backward_arrow();
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    if (normal_spline(spline)) {
		draw_arrow(p->x, p->y, selected_point->x,
			selected_point->y, spline->back_arrow, PAINT);
		}
	    else {
		c = spline->controls;
		draw_arrow(round(c->rx), round(c->ry), selected_point->x,
			selected_point->y, spline->back_arrow, PAINT);
		}
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    }
	else if (selected_point->next == NULL) { /* add forward arrow */
	    if (spline->for_arrow) return;
	    spline->for_arrow = forward_arrow();
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    if (normal_spline(spline)) {
		draw_arrow(left_point->x, left_point->y,
			selected_point->x, selected_point->y,
			spline->for_arrow, PAINT);
		}
	    else {
		for (c = spline->controls; c->next != NULL; c = c->next);
		draw_arrow(round(c->lx), round(c->ly), selected_point->x,
			selected_point->y, spline->for_arrow, PAINT);
		}
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    }
	clean_up();
	set_action_object(F_ADD_ARROW_HEAD, O_SPLINE);
	}

delete_linearrow(line)
F_line	*line;
{
	if (line->type == T_POLYGON || line->type == T_BOX) return;

	if (left_point == NULL) { /*  selected_point is the first point */
	    if (! line->back_arrow) return;
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    draw_line(line, INV_PAINT);
	    line->back_arrow = 0;
	    free((char*)line->back_arrow);
	    line->for_arrow = NULL;
	    draw_line(line, PAINT);
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    }
	else if (selected_point->next == NULL) { /* forward arrow */
	    if (! line->for_arrow) return;
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    draw_line(line, INV_PAINT);
	    free((char*)line->for_arrow);
	    line->for_arrow = NULL;
	    draw_line(line, PAINT);
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    }
	clean_up();
	set_action_object(F_DELETE_ARROW_HEAD, O_POLYLINE);
	}

delete_arcarrow(arc)
F_arc	*arc;
{
	if (arcpoint_num == 0) { /*  backward arrow  */
	    if (! arc->back_arrow) return;
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    draw_arc(arc, background_color);
	    free((char*)arc->back_arrow);
	    arc->back_arrow = NULL;
	    draw_arc(arc, foreground_color);
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    }
	else if (arcpoint_num == 2) { /*  for_arrow  */
	    if (! arc->for_arrow) return;
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    draw_arc(arc, background_color);
	    free((char*)arc->for_arrow);
	    arc->for_arrow = NULL;
	    draw_arc(arc, foreground_color);
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    }
	clean_up();
	set_action_object(F_DELETE_ARROW_HEAD, O_ARC);
	}

delete_splinearrow(spline)
F_spline	*spline;
{
	F_point	*p;

	if (closed_spline(spline)) return;
	if (left_point == NULL) { /*  selected_point is the first point */
	    if (! spline->back_arrow) return;
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    p = selected_point->next;
	    if (normal_spline(spline)) {
		draw_arrow(p->x, p->y, selected_point->x,
			selected_point->y, spline->back_arrow, ERASE);
		pw_vector(canvas_pixwin, selected_point->x, selected_point->y,
			round((p->x+selected_point->x)/2.0),
			round((p->y+selected_point->y)/2.0), PAINT, 1);
		}
	    else {
		F_control	*a, *b;

		a = spline->controls;
		b = a->next;
		draw_arrow(round(a->rx), round(a->ry), selected_point->x,
			selected_point->y, spline->back_arrow, ERASE);
		bezier_spline(
			(float)selected_point->x, (float)selected_point->y,
			a->rx, a->ry, b->lx, b->ly, (float)p->x, (float)p->y,
			PAINT);
		}
	    free((char*)spline->back_arrow);
	    spline->back_arrow = NULL;
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    }
	else if (selected_point->next == NULL) { /* forward arrow */
	    if (! spline->for_arrow) return;
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    if (normal_spline(spline)) {
		draw_arrow(left_point->x, left_point->y,
			selected_point->x, selected_point->y,
			spline->for_arrow, ERASE);
		pw_vector(canvas_pixwin, selected_point->x, selected_point->y,
			round((selected_point->x+left_point->x)/2.0),
			round((selected_point->y+left_point->y)/2.0),
			PAINT, 1);
		}
	    else {
		F_control	*a, *b;

		a = spline->controls;
		for (b = a->next; b->next != NULL; a = b, b = b->next);
		draw_arrow(round(b->lx), round(b->ly), selected_point->x,
			selected_point->y, spline->for_arrow, ERASE);
		bezier_spline(
			(float)left_point->x, (float)left_point->y,
			a->rx, a->ry, b->lx, b->ly,
			(float)selected_point->x, (float)selected_point->y,
			PAINT);
		}
	    free((char*)spline->for_arrow);
	    spline->for_arrow = NULL;
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    }
	clean_up();
	set_action_object(F_DELETE_ARROW_HEAD, O_SPLINE);
	}
