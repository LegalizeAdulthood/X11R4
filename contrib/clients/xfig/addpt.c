/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : August 1985.
 *	2nd revision : March 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "alloc.h"
#include "func.h"
#include "object.h"
#include "paintop.h"

#define			TOLERANCE	3

extern F_line		*line_search();
extern F_spline		*spline_search();
extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

extern int		cur_x, cur_y;
extern int		pointmarker_shown;
extern F_line		*line;
extern F_spline		*spline;

extern F_point		*added_point;
extern F_point		*left_point, *right_point;

			init_point_adding();
			move_addedpoint();
			fix_linepoint_adding();
			fix_splinepoint_adding();

static F_line		*cur_line;
static F_spline		*cur_spline;

point_adding_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_point_adding;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick9_cursor);
	}

init_point_adding(x, y)
int	x, y;
{
	int	px, py;

	if ((cur_line = line_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    if (cur_line->type == T_BOX) {
		put_msg("Adding points to a box is not allowed");
		return;
		}
	    init_linepointadding(px, py);
	    }
	else if ((cur_spline = spline_search(x,y,TOLERANCE,&px,&py)) != NULL){
	    init_splinepointadding(px, py);
	    }
	else {
	    return;
	    }
	canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
	erase_pointmarker();
	}

wrapup_pointadding()
{
	show_pointmarker();
	point_adding_selected();
	}

/**************************  spline  *******************************/

init_splinepointadding(px, py)
int		px, py;
{
	find_endpoints(cur_spline->points, px, py, &left_point, &right_point);
	set_temp_cursor(&null_cursor);
	win_setmouseposition(canvas_swfd, px, py);
	cur_x = px; cur_y = py;
	if (left_point == NULL && closed_spline(cur_spline)) {
	    /* The added_point is between the 1st and 2nd point. */
	    left_point = right_point;
	    right_point = right_point->next;
	    }
	draw_addedlink(INV_PAINT);
	canvas_locmove_proc = move_addedpoint;
	canvas_middlebut_proc = fix_splinepoint_adding;
	}

fix_splinepoint_adding(x, y)
int	x, y;
{
	F_point		*p;

	if (NULL == (Point_malloc(p))) {
	    put_msg(Err_mem);
	    wrapup_pointadding();
	    return;
	    }
	clean_up();
	added_point = p;
	added_point->x = x;
	added_point->y = y;
	draw_addedlink(INV_PAINT);
	if (-1 == splinepoint_adding(cur_spline, added_point))
	    wrapup_pointadding();
	set_action_object(F_ADD_POINT, O_SPLINE);
	set_latestspline(cur_spline);
	wrapup_pointadding();
	}

/*
Warning: Do not change the value of the pointers left_point and
right_point.  Added_point is always inserted between left_point
and right_point, except in two cases. 
	(1) left_point is NULL, the added_point will be prepended
		to the list of points. This case will never
		occur if the spline is closed (periodic).
	(2) right_point is NULL, the added_point will be appended
		to the end of the list.
*/

splinepoint_adding(spline, added_point)
F_spline	*spline;
F_point		*added_point;
{
	F_control	*c;

	set_temp_cursor(&wait_cursor);
	if (int_spline(spline)) {	/* Interpolated spline */
	    if (NULL == (Control_malloc(c))) {
		put_msg(Err_mem);
		return(-1);
		}
	    }
	pw_batch_on(canvas_pixwin);
	if (pointmarker_shown) toggle_splinepointmarker(spline);  
	draw_spline(spline, ERASE); /* erase old spline */
	if (left_point == NULL) {
	    added_point->next = spline->points;
	    spline->points = added_point;
	    }
	else {
	    added_point->next = right_point;
	    left_point->next = added_point;
	    }

	if (int_spline(spline)) {	/* Interpolated spline */
	    c->next = spline->controls;
	    spline->controls = c;
	    remake_control_points(spline);
	    }

	draw_spline(spline, PAINT); /* draw the modified spline */
	if (pointmarker_shown) toggle_splinepointmarker(spline);  
	pw_batch_off(canvas_pixwin);
	reset_cursor();
	set_modifiedflag();
	return(1);
	}

/***************************  line  ********************************/

init_linepointadding(px, py)
int	px, py;
{
	find_endpoints(cur_line->points,px,py,&left_point,&right_point);
	set_temp_cursor(&null_cursor);
	win_setmouseposition(canvas_swfd, px, py);
	cur_x = px; cur_y = py;
	if (left_point == NULL && cur_line->type == T_POLYGON) {
	    left_point = right_point;
	    right_point = right_point->next;
	    }
	if (left_point != NULL && right_point != NULL)
	    draw_line_segment(cur_line->style, cur_line->style_val,
		left_point->x, left_point->y,
		right_point->x, right_point->y, INV_PAINT);
	draw_addedlink(INV_PAINT);
	canvas_locmove_proc = move_addedpoint;
	canvas_middlebut_proc = fix_linepoint_adding;
	}

fix_linepoint_adding(x, y)
int	x, y;
{
	F_point		*p;

	if (NULL == (Point_malloc(p))) {
	    put_msg(Err_mem);
	    wrapup_pointadding();
	    return;
	    }
	clean_up();
	added_point = p;
	added_point->x = x;
	added_point->y = y;
	draw_addedlink(INV_PAINT);
	linepoint_adding(cur_line, added_point);
	set_action_object(F_ADD_POINT, O_POLYLINE);
	set_latestline(cur_line);
	wrapup_pointadding();
	}

linepoint_adding(line, added_point)
F_line	*line;
F_point	*added_point;
{
	if (pointmarker_shown) toggle_linepointmarker(line);
	draw_line(line, ERASE);
	if (left_point == NULL) {
	    added_point->next = line->points;
	    line->points = added_point;
	    }
	else {
	    added_point->next = left_point->next;
	    left_point->next = added_point;
	    }
	draw_line(line, PAINT);
	if (pointmarker_shown) toggle_linepointmarker(line);  
	set_modifiedflag();
	}

/*******************************************************************/

/*
If (x,y) is close to a point, q, fp points to q and sp points to q->next
(right).  However if q is the first point, fp contains NULL and sp points to q.
*/

find_endpoints(p, x, y, fp, sp)
F_point	*p, **fp, **sp;
int	x, y;
{
	int	d;
	F_point	*a = NULL, *b = p;

	if (x == b->x && y == b->y) {
	    *fp = a;
	    *sp = b;
	    return;
	    }

	for (a = p, b = p->next; b != NULL; a = b, b = b->next){
	    if (x == b->x && y == b->y) {
		*fp = b;
		*sp = b->next;
		return;
		}
	    if (close_to_vector(a->x, a->y, b->x, b->y, x, y, 1, 1.0, &d, &d)) {
		*fp = a;
		*sp = b;
		return;
		}
	    }
	*fp = a;
	*sp = b;
	}

draw_addedlink(op)
int	op;
{
	if (left_point != NULL) {
	    pw_vector(canvas_pixwin, left_point->x, left_point->y,
			cur_x, cur_y, op, 1);
	   }
	if (right_point != NULL) {
	    pw_vector(canvas_pixwin, right_point->x, 
			right_point->y, cur_x, cur_y, op, 1);
	    }
	}

move_addedpoint(x, y)
int	x, y;
{
	draw_addedlink(INV_PAINT);
	cur_x = x;
	cur_y = y;
	draw_addedlink(INV_PAINT);
	}
