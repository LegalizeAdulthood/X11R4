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

#define			TOLERANCE	3

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

extern F_line		*line_point_search();
extern F_spline		*spline_point_search();
extern int		last_object;
extern int		fix_x, fix_y, cur_x, cur_y;
extern int		pointmarker_shown;

extern F_point		*left_point, *right_point;
extern F_point		*deleted_point;
extern F_line		*line;
extern F_spline		*spline;

			init_delete_point();

delete_point_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_delete_point;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick9_cursor);
	reset_action_on();
	}

init_delete_point(x, y)
int	 x, y;
{
	F_spline	*spline;
	F_line		*line;
	F_point		*p, *q;
	int		n;

	/*
	If the attemp to delete point fails, we wouldn't want any important
	variables (left_point, right_point and delted_point) to change. 
	So we used p and q in the search.
	*/

	if ((line = line_point_search(x, y, TOLERANCE, &p, &q)) != NULL) {
	    if (line->type == T_BOX) {
		put_msg("Deleting box corners is not allowed");
		return;
		}
	    n = num_points(line->points);
	    if (line->type == T_POLYGON) {
		if (n <= 4) {
		    put_msg("A polygon cannot have less than 3 points");
		    return;
		    }
		}
	    else if (n <= 1) {
		put_msg("A line cannot have less than 2 points");
		return;
		}
	    clean_up();
	    left_point = p;
	    deleted_point = q;
	    right_point = q->next;
	    linepoint_deleting(line);
	    set_action_object(F_DELETE_POINT, O_POLYLINE);
	    set_latestline(line);
	    }
	else if ((spline=spline_point_search(x, y, TOLERANCE, &p, &q)) != NULL){
	    n = num_points(spline->points);
	    if (closed_spline(spline)) {
		if (n <= 4) {
		    put_msg("A closed spline cannot have less than 3 points");
		    return;
		    }
		}
	    else if (normal_spline(spline)) {
		if (n <= 1) {
		    put_msg("A spline cannot have less than 2 points");
		    return;
		    }
		}
	    else if (n <= 2) { /* it must be an interpolated spline */
		put_msg("An interpolated spline may have less than 3 points");
		return;
		}
	    clean_up();
	    left_point = p;
	    deleted_point = q;
	    right_point = q->next;
	    splinepoint_deleting(spline);
	    set_action_object(F_DELETE_POINT, O_SPLINE);
	    set_latestspline(spline);
	    }
	}

/**************************  spline  *******************************/

splinepoint_deleting(spline)
F_spline	*spline;
{
	F_point		*p;

	set_temp_cursor(&wait_cursor);
	if (closed_spline(spline)) {
	    pw_batch_on(canvas_pixwin);
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    draw_spline(spline, ERASE); /*erase the spline */
	    if (left_point == NULL) { 
		/* The deleted point is the first point */
		spline->points = right_point;
		for (left_point = right_point, p = left_point->next;
			p->next != NULL;
			left_point = p, p = p->next);
		/*
		left_point now points at next to last point
		(the last point is a copy of the first).
		*/
		p->x = spline->points->x;
		p->y = spline->points->y;
		right_point = p;
		/*
		Right_point becomes the last point.  If this operation
		(point deletion) is reversed (undo), the deleted_point
		will not be inserted into it original place, but will
		be between left_point and right_point.
		*/
		}
	    else
		left_point->next = right_point;
	    }
	else { /* open spline */
	    pw_batch_on(canvas_pixwin);
	    if (pointmarker_shown) toggle_splinepointmarker(spline);
	    draw_spline(spline, ERASE); /*erase the spline */
	    if (left_point == NULL) 
		spline->points = right_point;
	    else 
		left_point->next = right_point;
	    }
	if (int_spline(spline)) {
	    F_control	*c;

	    c = spline->controls;
	    spline->controls = c->next;
	    c->next = NULL;
	    free((char*)c);
	    remake_control_points(spline);
	    }
	draw_spline(spline, PAINT);
	if (pointmarker_shown) toggle_splinepointmarker(spline);
	pw_batch_off(canvas_pixwin);
	set_modifiedflag();
	reset_cursor();
	}

/***************************  line  ********************************/

/*
In deleting a point p, linepoint_deleting uses left_point and
right_point of point p.  The relationship of the three points
are : left_point->p->right_point except when p is the first
point in the list, in which case left_point will be NULL.
*/
linepoint_deleting(line)
F_line	*line;
{
	F_point	*p;

	if (line->type == T_POLYGON) {
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    draw_line(line, ERASE); /*erase the line */
	    if (left_point == NULL) {
		/* The deleted point is the first point */
		line->points = right_point;
		for (left_point = right_point, p = left_point->next; 
			p->next != NULL; 
			left_point = p, p = p->next);
		/*
		left_point now points at next to last point
		(the last point is a copy of the first).
		*/
		p->x = right_point->x;
		p->y = right_point->y;
		right_point = p;
		/*
		Right_point becomes the last point.  If this operation
		(point deletion) is reversed (undo), the deleted_point
		will not be inserted into it original place, but will
		be between left_point and right_point.
		*/
		}
	    else
		left_point->next = right_point;
	    }
	else { /* polyline */
	    if (pointmarker_shown) toggle_linepointmarker(line);
	    draw_line(line, ERASE); /*erase the line */
	    if (left_point == NULL) 
		line->points = right_point;
	    else 
		left_point->next = right_point;
	    }
	draw_line(line, PAINT);
	if (pointmarker_shown) toggle_linepointmarker(line);
	set_modifiedflag();
	}
