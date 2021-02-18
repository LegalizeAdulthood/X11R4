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
extern int		latexline_mode, latexarrow_mode;
extern int		magnet_mode;

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();
F_line			*line_point_search();
F_spline		*spline_point_search();
F_ellipse		*ellipse_point_search();
F_arc			*arc_point_search();

extern F_compound	objects;

extern F_point		*moved_point, *left_point;
extern F_pos		last_position, new_position;
extern int		movedpoint_num;
extern int		last_object;
extern int		fix_x, fix_y, cur_x, cur_y;
extern int		pointmarker_shown;
extern int		foreground_color, background_color;

extern			elastic_box(), move_ebrbox(), move_ebdbox();
extern			move_cbrbox(), move_cbdbox();

			init_move_point();
			move_linepoint(), fix_movedlinepoint();
			move_latexlinepoint(), fix_movedlatexlinepoint();
			fix_box();
			fix_movedsplinepoint();
			move_arcpoint(), fix_movedarcpoint();
			fix_movedellipsepoint();

static F_line		*line;
static F_spline		*spline;
static F_ellipse	*ellipse;
static F_arc		*arc;

static int		latex_fix_x, latex_fix_y;
CURSOR			cur_latexcursor;

move_point_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_move_point;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick9_cursor);
	reset_action_on();
	}

init_move_point(x, y)
int	x, y;
{
	if ((line = line_point_search(x, y, TOLERANCE,
		&left_point, &moved_point)) != NULL) {
	    init_linepointmoving(line);
	    }
	else if ((spline = spline_point_search(x, y, 
		TOLERANCE, &left_point, &moved_point)) != NULL){
	    init_splinepointmoving(spline);
	    }
	else if ((ellipse = ellipse_point_search(x, y, TOLERANCE, 
		&movedpoint_num)) != NULL) {
	    init_ellipsepointmoving(ellipse);
	    }
	else if ((arc = arc_point_search(x, y, TOLERANCE, 
		&movedpoint_num)) != NULL) {
	    init_arcpointmoving(arc);
	    }
	else {
	    return;
	    }
	canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
	erase_pointmarker();
	}

wrapup_movepoint()
{
	show_pointmarker();
	move_point_selected();
	}

/*************************  ellipse  *******************************/

F_ellipse *
ellipse_point_search(x, y, tol, point_num)
int	x, y, tol, *point_num;
{
	F_ellipse	*e;

	for (e = objects.ellipses; e != NULL; e = e->next) {
	    if (abs(e->start.x - x) <= tol && abs(e->start.y - y) <= tol) {
		*point_num = 0;
		return(e);
		}
	    if (abs(e->end.x - x) <= tol && abs(e->end.y - y) <= tol) {
		*point_num = 1;
		return(e);
		}
	    }
	return(NULL);
	}

static F_ellipse	*cur_e;

init_ellipsepointmoving(ellipse)
F_ellipse	*ellipse;
{
	if (movedpoint_num == 0) {  /*  starting point is selected  */
	    if (ellipse->type == T_ELLIPSE_BY_RAD || 
		ellipse->type == T_CIRCLE_BY_RAD) {
		return;
		}
	    last_position.x = cur_x = ellipse->start.x;
	    last_position.y = cur_y = ellipse->start.y;
	    fix_x = ellipse->end.x;  fix_y = ellipse->end.y;
	    switch (ellipse->type) {
		case T_ELLIPSE_BY_DIA :
		    canvas_locmove_proc = move_ebdbox;
		    ellipsebydia_box(INV_PAINT);
		    break;
		case T_CIRCLE_BY_DIA :
		    canvas_locmove_proc = move_cbdbox;
		    circlebydia_box(INV_PAINT);
		    break;
		}
	    }
	else {
	    last_position.x = cur_x = ellipse->end.x;
	    last_position.y = cur_y = ellipse->end.y;
	    fix_x = ellipse->start.x;  fix_y = ellipse->start.y;
	    switch (ellipse->type) {
		case T_ELLIPSE_BY_RAD :
		    canvas_locmove_proc = move_ebrbox;
		    ellipsebyrad_box(INV_PAINT);
		    break;
		case T_CIRCLE_BY_RAD :
		    canvas_locmove_proc = move_cbrbox;
		    circlebyrad_box(INV_PAINT);
		    break;
		case T_ELLIPSE_BY_DIA :
		    canvas_locmove_proc = move_ebdbox;
		    ellipsebydia_box(INV_PAINT);
		    break;
		case T_CIRCLE_BY_DIA :
		    canvas_locmove_proc = move_cbdbox;
		    circlebydia_box(INV_PAINT);
		    break;
		}
	    }
	cur_e = ellipse;
	set_temp_cursor(&crosshair_cursor);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	canvas_middlebut_proc = fix_movedellipsepoint;
	canvas_leftbut_proc = null_proc;
	}

fix_movedellipsepoint(x, y)
int	x, y;
{
	switch (cur_e->type) {
	    case T_ELLIPSE_BY_RAD :
		ellipsebyrad_box(INV_PAINT);
		break;
	    case T_CIRCLE_BY_RAD :
		circlebyrad_box(INV_PAINT);
		break;
	    case T_ELLIPSE_BY_DIA :
		ellipsebydia_box(INV_PAINT);
		break;
	    case T_CIRCLE_BY_DIA :
		circlebydia_box(INV_PAINT);
		break;
	    }
	new_position.x = x;
	new_position.y = y;
	clean_up();
	set_action_object(F_MOVE_POINT, O_ELLIPSE);
	set_latestellipse(cur_e);
	relocate_ellipsepoint(cur_e, x, y, movedpoint_num);
	wrapup_movepoint();
	}

relocate_ellipsepoint(ellipse, x, y, point_num)
F_ellipse	*ellipse;
int		x, y, point_num;
{
	int	dx, dy;

	set_temp_cursor(&wait_cursor);
	pw_batch_on(canvas_pixwin);
	if (pointmarker_shown) toggle_ellipsepointmarker(ellipse);
	draw_ellipse(ellipse, background_color);
	if (point_num == 0) {  /*  starting point is selected  */
	    fix_x = ellipse->end.x;  fix_y = ellipse->end.y;
	    ellipse->start.x = x;  ellipse->start.y = y;
	    }
	else {
	    fix_x = ellipse->start.x;  fix_y = ellipse->start.y;
	    ellipse->end.x = x;  ellipse->end.y = y;
	    }
	switch (ellipse->type) {
	    case T_ELLIPSE_BY_RAD :
		ellipse->radiuses.x = abs(x - fix_x) + 1;
		ellipse->radiuses.y = abs(y - fix_y) + 1;
		break;
	    case T_CIRCLE_BY_RAD :
		dx = fix_x - x;  dy = fix_y - y;
		ellipse->radiuses.x = sqrt((double)(dx*dx + dy*dy)) + .5;
		ellipse->radiuses.y = ellipse->radiuses.x;
		break;
	    case T_ELLIPSE_BY_DIA :
		ellipse->center.x = (fix_x + x) / 2;
		ellipse->center.y = (fix_y + y) / 2;
		ellipse->radiuses.x = abs(ellipse->center.x - fix_x);
		ellipse->radiuses.y = abs(ellipse->center.y - fix_y);
		break;
	    case T_CIRCLE_BY_DIA :
		dx = ellipse->center.x = (fix_x + x) / 2 +.5;
		dy = ellipse->center.y = (fix_y + y) / 2 +.5;
		dx -= x;  dy -= y; 
		ellipse->radiuses.x = sqrt((double)(dx*dx + dy*dy)) + .5;
		ellipse->radiuses.y = ellipse->radiuses.x;
		break;
	    }
	draw_ellipse(ellipse, foreground_color);
	if (pointmarker_shown) toggle_ellipsepointmarker(ellipse);
	pw_batch_off(canvas_pixwin);
	reset_cursor();
	set_modifiedflag();
	}

/***************************  arc  *********************************/

static F_arc		*cur_a;

F_arc *
arc_point_search(x, y, tol, point_num)
int	x, y, tol, *point_num;
{
	F_arc	*a;
	int	i;

	for(a = objects.arcs; a != NULL; a = a->next) {
	    for (i = 0; i < 3; i++) {
		if (abs(a->point[i].x - x) <= tol && 
			abs(a->point[i].y - y) <= tol) {
		    *point_num = i;
		    return(a);
		    }
		}
	    }
	return(NULL);
	}

init_arcpointmoving(arc)
F_arc	*arc;
{
	cur_a = arc;
	last_position.x = cur_x = arc->point[movedpoint_num].x;
	last_position.y = cur_y = arc->point[movedpoint_num].y;
	set_temp_cursor(&crosshair_cursor);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	draw_arclink(INV_PAINT);
	canvas_locmove_proc = move_arcpoint;
	canvas_middlebut_proc = fix_movedarcpoint;
	canvas_leftbut_proc = null_proc;
	}

move_arcpoint(x, y)
int	x, y;
{
	draw_arclink(INV_PAINT);
	cur_x = x;  cur_y = y;
	draw_arclink(INV_PAINT);
	}

draw_arclink(op)
int	op;
{
	switch (movedpoint_num) {
	    case 0 :
		pw_vector(canvas_pixwin, cur_x, cur_y,
			arc->point[1].x, arc->point[1].y, op, 1);
		break;
	    case 1 :
		pw_vector(canvas_pixwin, arc->point[0].x, arc->point[0].y,
			cur_x, cur_y, op, 1);
		pw_vector(canvas_pixwin, arc->point[2].x, arc->point[2].y,
			cur_x, cur_y, op, 1);
		break;
	    default :
		pw_vector(canvas_pixwin, arc->point[2].x, arc->point[2].y,
			cur_x, cur_y, op, 1);
	    }
	}

fix_movedarcpoint(x, y)
int	x, y;
{
	draw_arclink(INV_PAINT);
	new_position.x = x;
	new_position.y = y;
	clean_up();
	set_action_object(F_MOVE_POINT, O_ARC);
	set_latestarc(cur_a);
	relocate_arcpoint(cur_a, x, y, movedpoint_num);
	wrapup_movepoint();
	}

relocate_arcpoint(arc, x, y, movedpoint_num)
F_arc	*arc;
int			 x, y, movedpoint_num;
{
	float	xx, yy;
	F_pos	p[3];

	p[0] = arc->point[0];
	p[1] = arc->point[1];
	p[2] = arc->point[2];
	p[movedpoint_num].x = x;
	p[movedpoint_num].y = y;
	if (compute_arccenter(p[0], p[1], p[2], &xx, &yy)) {
	    set_temp_cursor(&wait_cursor);
	    pw_batch_on(canvas_pixwin);
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    draw_arc(arc, background_color);	/* erase old arc */
	    arc->point[movedpoint_num].x = x;
	    arc->point[movedpoint_num].y = y;
	    arc->center.x = xx;
	    arc->center.y = yy;
	    arc->direction = compute_direction(p[0], p[1], p[2]);
	    draw_arc(arc, foreground_color);	/* draw new arc */
	    if (pointmarker_shown) toggle_arcpointmarker(arc);
	    pw_batch_off(canvas_pixwin);
	    reset_cursor();
	    set_modifiedflag();
	    }
	}

/**************************  spline  *******************************/

static F_spline		*cur_s;

init_splinepointmoving(s)
F_spline	*s;
{
	F_point	*p;

	cur_s = s;
	last_position.x = cur_x = moved_point->x;
	last_position.y = cur_y = moved_point->y;
	set_temp_cursor(&crosshair_cursor);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	if (closed_spline(s) && left_point == NULL) {
	    for (left_point = moved_point->next, 
		p = left_point->next; 
		p->next != NULL; 
		left_point = p, p = p->next);
	    }
	draw_pointlink(INV_PAINT);
	canvas_locmove_proc = move_linepoint;
	canvas_middlebut_proc = fix_movedsplinepoint;
	canvas_leftbut_proc = null_proc;
	}

F_spline *
spline_point_search(x, y, tol, p, q)
int	x, y, tol;
F_point	**p, **q;
{
	F_spline	*s;

	for (s = objects.splines; s != NULL; s= s->next) {
	    *p = NULL;
	    for (*q = s->points; *q != NULL; *p = *q, *q = (*q)->next) {
		if (abs((*q)->x - x) <= tol && abs((*q)->y - y) <= tol)
		    return(s);
		}
	    }
	return(NULL);
	}

fix_movedsplinepoint(x, y)
int	x, y;
{
	draw_pointlink(INV_PAINT);
	new_position.x = x;
	new_position.y = y;
	clean_up();
	set_action_object(F_MOVE_POINT, O_SPLINE);
	set_latestspline(cur_s);
	relocate_splinepoint(cur_s, x, y, moved_point);
	wrapup_movepoint();
	}

relocate_splinepoint(s, x, y, moved_point)
F_spline	*s;
int		x, y;
F_point		*moved_point;
{
	set_temp_cursor(&wait_cursor);
	pw_batch_on(canvas_pixwin);
	if (pointmarker_shown) toggle_splinepointmarker(s);  
	draw_spline(s, ERASE); /* erase old spline */
	moved_point->x = x;
	moved_point->y = y;
	if (closed_spline(s)) {
	    left_point->next->x = x;
	    left_point->next->y = y;
	    }
	if (int_spline(s)) remake_control_points(s);
	draw_spline(s, PAINT); /* draw spline with moved point */
	if (pointmarker_shown) toggle_splinepointmarker(s);  
	pw_batch_off(canvas_pixwin);
	reset_cursor();
	set_modifiedflag();
	}

/***************************  line  ********************************/

static F_line		*cur_l;

init_linepointmoving(line)
F_line	*line;
{
	int	box_case;
	int	latex_case;
	F_point	*p;

	cur_l = line;
	box_case = 0;
	latex_case = 0;
	last_position.x = cur_x = moved_point->x;
	last_position.y = cur_y = moved_point->y;
	set_temp_cursor(&crosshair_cursor);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	switch (line->type) {
	    case T_POLYGON :
		if (left_point == NULL)
		    for (left_point = moved_point->next, 
			p = left_point->next; 
			p->next != NULL; 
			left_point = p, p = p->next);
		break;
	    case T_BOX :
		if (moved_point->next->next == NULL) { /* point 4 */
		    fix_x = line->points->next->x;
		    fix_y = line->points->next->y;
		    }
		else {
		    fix_x = moved_point->next->next->x;
		    fix_y = moved_point->next->next->y;
		    }
		draw_line(line, ERASE);
		box_case = 1;
		break;
	    case T_POLYLINE :
		if (left_point != NULL) {
		    if (left_point == line->points) {
			if (line->back_arrow) /*  backward arrow  */
			    draw_arrow(cur_x, cur_y,
				left_point->x, left_point->y,
				line->back_arrow, ERASE);
			}
		    }
		else if (line->back_arrow) /*  backward arrow  */
		    draw_arrow(moved_point->next->x, moved_point->next->y,
			cur_x, cur_y, line->back_arrow, ERASE);
		p = moved_point->next;
		if (p != NULL) {
		    if (line->for_arrow && p->next == NULL) 
			draw_arrow(cur_x, cur_y, p->x, p->y, 
			    line->for_arrow, ERASE);
			}
		else if (line->for_arrow)/* f arrow */
		    draw_arrow(left_point->x, left_point->y, 
			cur_x, cur_y, line->for_arrow, ERASE);
		if (latexline_mode || latexarrow_mode) {
		    if (left_point != NULL) {
			latex_fix_x = left_point->x;
			latex_fix_y = left_point->y;
			latex_case = 1;
			}
		    else if (p != NULL) {
			latex_fix_x = p->x;
			latex_fix_y = p->y;
			latex_case = 1;
			}
		    }
	    }
	if (box_case) {
	    draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	    canvas_locmove_proc = elastic_box;
	    canvas_middlebut_proc = fix_box;
	    }
	else if (latex_case) {
	    draw_pointlink(INV_PAINT);
	    canvas_locmove_proc = move_latexlinepoint;
	    canvas_middlebut_proc = fix_movedlatexlinepoint;
	    cur_latexcursor = &crosshair_cursor;
	    }
	else {
	    draw_pointlink(INV_PAINT);
	    canvas_locmove_proc = move_linepoint;
	    canvas_middlebut_proc = fix_movedlinepoint;
	    }
	canvas_leftbut_proc = null_proc;
	}

move_linepoint(x, y)
int	x, y;
{
	draw_pointlink(INV_PAINT);
	cur_x = x;
	cur_y = y;
	draw_pointlink(INV_PAINT);
	}

move_latexlinepoint(x, y)
int	x, y;
{
	CURSOR	 c;

	draw_pointlink(INV_PAINT);
	latex_endpoint(latex_fix_x, latex_fix_y, x, y, &cur_x, &cur_y,
	    latexarrow_mode, (magnet_mode)? 5: 1);
	draw_pointlink(INV_PAINT);
	c = (x == cur_x  &&  y == cur_y)? &null_cursor: &crosshair_cursor;
	if (c != cur_latexcursor) {
	    set_temp_cursor(c);
	    cur_latexcursor = c;
	    }
	}

fix_box(x, y)
int	x, y;
{
	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	new_position.x = x;
	new_position.y = y;
	clean_up();
	set_action_object(F_MOVE_POINT, O_POLYLINE);
	set_latestline(line);
	relocate_linepoint(line, x, y, fix_x, fix_y, moved_point, 
		left_point);
	wrapup_movepoint();
	}

assign_newboxpoint(b, x1, y1, x2, y2)
F_line	*b;
int	x1, y1, x2, y2;
{
	F_point	*p;

	p = b->points;
	p->x = x1;	p->y = y1;	p = p->next;
	p->x = x1;	p->y = y2;	p = p->next;
	p->x = x2;	p->y = y2;	p = p->next;
	p->x = x2;	p->y = y1;	p = p->next;
	p->x = x1;	p->y = y1;	p = p->next;
	}

fix_movedlinepoint(x, y)
int	x, y;
{
	draw_pointlink(INV_PAINT);
	new_position.x = x;
	new_position.y = y;
	clean_up();
	set_action_object(F_MOVE_POINT, O_POLYLINE);
	set_latestline(cur_l);
	relocate_linepoint(cur_l, x, y, fix_x, fix_y, moved_point, left_point);
	wrapup_movepoint();
	}

fix_movedlatexlinepoint(x, y)
int	x, y;
{
	draw_pointlink(INV_PAINT);
	latex_endpoint(latex_fix_x, latex_fix_y, x, y, &x, &y,
	    latexarrow_mode, (magnet_mode)? 5: 1);
	if (cur_latexcursor != &crosshair_cursor)
	    set_temp_cursor(&crosshair_cursor);
	win_setmouseposition(canvas_swfd, x, y);
	new_position.x = x;
	new_position.y = y;
	clean_up();
	set_action_object(F_MOVE_POINT, O_POLYLINE);
	set_latestline(cur_l);
	relocate_linepoint(cur_l, x, y, fix_x, fix_y, moved_point, left_point);
	wrapup_movepoint();
	}

relocate_linepoint(line, x, y, fix_x, fix_y, moved_point, left_point)
F_line	*line;
int	x, y;
F_point	*moved_point, *left_point;
{
	if (pointmarker_shown) toggle_linepointmarker(line);
	draw_line(line, ERASE);
	switch (line->type) {
	    case T_BOX :
		assign_newboxpoint(line, fix_x, fix_y, x, y);
		break;
	    case T_POLYGON :
		if (line->points == moved_point) {
		    left_point->next->x = x;
		    left_point->next->y = y;
		    }
	    default :
		moved_point->x = x;
		moved_point->y = y;
	    }
	draw_line(line, PAINT);
	if (pointmarker_shown) toggle_linepointmarker(line);  
	set_modifiedflag();
	}

draw_pointlink(op)
int	op;
{
	F_point	*p;

	if (left_point != NULL) {
	    pw_vector(canvas_pixwin, left_point->x, left_point->y,
			cur_x, cur_y, op, 1);
	    }
	if ((p = moved_point->next) != NULL) {
	    pw_vector(canvas_pixwin, p->x, p->y, cur_x, cur_y, op, 1);
	    }
	}

F_line *
line_point_search(x, y, tol, p, q)
int	x, y, tol;
F_point	**p, **q;
{
	F_line	*l;
	F_point	*a, *b;

	for (l = objects.lines; l != NULL; l= l->next) {
	    for (a = NULL, b = l->points; b != NULL; a = b, b = b->next) {
		if (abs(b->x - x) <= tol && abs(b->y - y) <= tol) {
		    *p = a;
		    *q = b;
		    return(l);
		    }
		}
	    }
	return(NULL);
	}
