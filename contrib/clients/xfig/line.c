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

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

extern int		fix_x, fix_y, cur_x, cur_y;

extern int		cur_command;
extern int		manhattan_mode, mountain_mode;
extern int		latexline_mode, latexarrow_mode;
extern int		magnet_mode, latexarrow_mode;
extern int		autoforwardarrow_mode, autobackwardarrow_mode;
extern int		cur_line_style, line_thickness;
extern float		cur_styleval;
extern int		cur_color;
extern F_compound	objects;
extern int		num_point;
extern int		latex_endpoint();

extern F_point		*first_point, *cur_point; 

/*************************** locally global variables *********************/

static int		firstx, firsty;
			init_line_drawing();

int			create_lineobject();
			create_latexobject();
			determine_angle();
			freehand_elasticline();
			latex_elasticline();
			angle0_elasticline(), angle90_elasticline(); 
			angle45_elasticline(), angle135_elasticline(); 
			get_direction(), get_intermediatepoint(),
			get_latexpoint();

/**********************  polyline and polygon section  **********************/

line_drawing_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_line_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_line_drawing(x, y)
int	x, y;
{
	first_point = Point_malloc(cur_point);
	if (cur_point == NULL) {
	    blink_msg();
	    put_msg(Err_mem);
	    return;
	    }
	canvas_rightbut_proc = null_proc;
	set_action_on();
	cur_point->x = firstx = fix_x = cur_x = x;
	cur_point->y = firsty = fix_y = cur_y = y;
	cur_point->next = NULL;
	num_point = 1;
	if (latexline_mode || latexarrow_mode) {
	    canvas_locmove_proc = latex_elasticline;
	    canvas_leftbut_proc = get_latexpoint;
	    canvas_middlebut_proc = create_latexobject;
	    }
	else if (manhattan_mode || mountain_mode) {
	    canvas_locmove_proc = determine_angle;
	    canvas_leftbut_proc = get_direction;
	    canvas_middlebut_proc = create_lineobject;
	    }
	else {
	    canvas_locmove_proc = freehand_elasticline;
	    canvas_leftbut_proc = get_intermediatepoint;
	    canvas_middlebut_proc = create_lineobject;
	    }
	set_temp_cursor(&null_cursor);
	cur_cursor = &null_cursor;
	draw_elasticline();
	}

draw_elasticline()
{
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	}

freehand_elasticline(x, y)
int	x, y;
{
	draw_elasticline();
	cur_x = x;  
	cur_y = y;
	draw_elasticline();
	}

draw_latexline()
{
	int  x, y;
	CURSOR c;

	latex_endpoint(fix_x, fix_y, cur_x, cur_y, &x, &y, latexarrow_mode, (magnet_mode)? 5: 1);
	pw_vector(canvas_pixwin, fix_x, fix_y, x, y, INV_PAINT,1);
	c = (x == cur_x  &&  y == cur_y)? &null_cursor: &crosshair_cursor;
	if (c != cur_cursor) {
	    set_temp_cursor(c);
	    cur_cursor = c;
	    }
	}

latex_elasticline(x, y)
int	x, y;
{
	draw_latexline();
	cur_x = x;  
	cur_y = y;
	draw_latexline();
	}

angle0_elasticline(x, y)
int	x, y;
{
	if (x == cur_x && y == cur_y) return;
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	cur_x = x;
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	}

angle90_elasticline(x, y)
int	x, y;
{
	if (x == cur_x && y == cur_y) return;
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	cur_y = y;
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	}

angle45_elasticline(x, y)
int	x, y;
{
	if (x == cur_x && y == cur_y) return;
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	if (abs(x - cur_x) < abs(y - cur_y)) {
	    cur_x += cur_y - y;
	    cur_y = y;
	    }
	else {
	    cur_y -= x - cur_x;
	    cur_x = x;
	    }
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	}

angle135_elasticline(x, y)
int	x, y;
{
	if (x == cur_x && y == cur_y) return;
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	if (abs(x - cur_x) < abs(y - cur_y)) {
	    cur_x += y - cur_y;
	    cur_y = y;
	    }
	else {
	    cur_y += x - cur_x;
	    cur_x = x;
	    }
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, INV_PAINT,1);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	}

get_direction(x, y)
int	x, y;
{
	(*canvas_locmove_proc) (x, y);
	canvas_locmove_proc = determine_angle;
	get_intermediatepoint(cur_x, cur_y);
	}

determine_angle(x, y)
int	x, y;
{
	float	angle, dx, dy;

	dx = x - fix_x;  dy = fix_y - y;
	if (sqrt((double)(dx*dx + dy*dy)) < 7) return;
	if (dx == 0)
	    angle = -90;
	else
	    angle = 180 * atan((double)(dy / dx)) / 3.1416;

	if (manhattan_mode) {
	    if (mountain_mode) {
		if (angle < -67.5) canvas_locmove_proc = angle90_elasticline;
		else if (angle < -22.5) canvas_locmove_proc = angle135_elasticline;
		else if (angle <  22.5) canvas_locmove_proc = angle0_elasticline;
		else if (angle <  67.5) canvas_locmove_proc = angle45_elasticline;
		else canvas_locmove_proc = angle90_elasticline;
		}
	    else {
		if (angle < -45) canvas_locmove_proc = angle90_elasticline;
		else if (angle < 45) canvas_locmove_proc = angle0_elasticline;
		else canvas_locmove_proc = angle90_elasticline;
		}
	    }
	else {
	    if (angle < 0) canvas_locmove_proc = angle135_elasticline;
	    else canvas_locmove_proc = angle45_elasticline;
	    }
	    
	(*canvas_locmove_proc)(x, y);
	}

get_latexpoint(x, y)
int	x, y;
{
	draw_latexline();
	latex_endpoint(fix_x, fix_y, x, y, &cur_x, &cur_y, latexarrow_mode, (magnet_mode)? 5: 1);
	if (cur_cursor != &null_cursor) {
	    set_temp_cursor(&null_cursor);
	    cur_cursor = &null_cursor;
	    }
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	get_intermediatepoint(cur_x, cur_y);
	}

get_intermediatepoint(x, y)
int	x, y;
{
	draw_elasticline();  /* erase elastic line */
	cur_x = x;
	cur_y = y;
	if (cur_command == F_POLYLINE || cur_command == F_POLYGON)
	    draw_line_segment(cur_line_style, cur_styleval, fix_x, fix_y, x, y, 
				PAINT);
	else
	    draw_elasticline();
	num_point++;
	fix_x = x;
	fix_y = y;
	draw_elasticline();
	append_point(fix_x, fix_y, &cur_point);
	}

create_latexobject(x, y)
int	x, y;
{
	if (x != fix_x || y != fix_y || num_point == 1) {
	    draw_latexline();
	    latex_endpoint(fix_x, fix_y, x, y, &cur_x, &cur_y, latexarrow_mode, (magnet_mode)? 5: 1);
	    if (cur_cursor != &null_cursor) {
		set_temp_cursor(&null_cursor);
		cur_cursor = &null_cursor;
		}
	    win_setmouseposition(canvas_swfd, cur_x, cur_y);
	    }
	create_lineobject(cur_x, cur_y);
	}

create_lineobject(x, y)
int	x, y;
{
	extern F_arrow	*forward_arrow(), *backward_arrow();
	F_point		*p, *q;
	F_line		*line;
	int		dot;

	dot = 0;
	if (x != fix_x || y != fix_y) {
	    get_intermediatepoint(x, y);
	    draw_elasticline();
	    }
	else if (num_point == 1) {
	    dot = 1;
	    get_intermediatepoint(x, y);
	    draw_elasticline();
	    }
	if (NULL == (Line_malloc(line))) {
	    put_msg(Err_mem);
	    line_drawing_selected();
	    return;
	    }
	line->type = T_POLYLINE;
	line->style = cur_line_style;
	line->thickness = line_thickness;
	line->style_val = cur_styleval;
	line->color = cur_color;
	line->depth = 0;
	line->area_fill = NULL;
	line->pen = NULL;
	line->points = first_point;
	line->next = NULL;
	line->for_arrow = NULL;
	line->back_arrow = NULL;
	if (cur_command == F_POLYGON) {
	    if (num_point >= 3) {
		line->type = T_POLYGON;
		num_point++;
		append_point(firstx, firsty, &cur_point);
		draw_line_segment(cur_line_style, cur_styleval, 
			firstx, firsty, fix_x, fix_y, PAINT);
		}
	    }
	else if (dot) {
	    pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, PAINT, 1);
	    line->style = SOLID_LINE;
	    }
	else if (num_point > 1) {
	    if (autoforwardarrow_mode) {
		line->for_arrow = forward_arrow();
		for (p = line->points; p != cur_point; q = p, p = p->next);
		draw_arrow(q->x, q->y, p->x, p->y, line->for_arrow, PAINT);
		}
	    if (autobackwardarrow_mode) {
		line->back_arrow = backward_arrow();
		p = line->points->next;
		draw_arrow(p->x, p->y, firstx, firsty, line->back_arrow, PAINT);
		}
	    }
	clean_up();
	set_action_object(F_CREATE, O_POLYLINE);
	insert_line(&objects.lines, line);
	set_latestline(line);
	set_modifiedflag();
	line_drawing_selected();
	}

append_point(x, y, point)
int	x, y;
F_point	**point;
{
	F_point	*p;

	if (NULL == (Point_malloc(p))) {
	    put_msg(Err_mem);
	    return;
	    }
	p->x = x;
	p->y = y;
	p->next = NULL;
	(*point)->next = p;
	*point = p;
	}
