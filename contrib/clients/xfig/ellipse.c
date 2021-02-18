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

extern int		cur_line_style, line_thickness;
extern float		cur_styleval;
extern float		cur_angle;
extern int		cur_color;
extern int		fix_x, fix_y, cur_x, cur_y;

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

extern F_compound	objects;

/*************************  local procedures  ********************/

			init_ellipsebyradius_drawing();
			init_ellipsebydiameter_drawing();
			init_circlebyradius_drawing();
			init_circlebydiameter_drawing();
			move_ebrbox(), move_ebdbox();
			move_cbrbox(), move_cbdbox();
int			create_ellipsebydia();
int			create_ellipsebyrad();
int			create_circlebyrad();
int			create_circlebydia();

#define		round(z)	(int)((z)+.5)

center_marker(x, y)
int	x, y;
{
	pw_vector(canvas_pixwin, x, y-2, x, y+2, INV_PAINT, 1);
	pw_vector(canvas_pixwin, x-2, y, x+2, y, INV_PAINT, 1);
	}

ellipsebyradius_drawing_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_ellipsebyradius_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_ellipsebyradius_drawing(x, y)
int	x, y;
{
	cur_x = fix_x = x; 
	cur_y = fix_y = y;
	center_marker(fix_x, fix_y);
	canvas_locmove_proc = move_ebrbox;
	canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
	canvas_middlebut_proc = create_ellipsebyrad;
	set_temp_cursor(&null_cursor);
	ellipsebyrad_box(INV_PAINT);
	set_action_on();
	}

move_ebrbox(x, y)
int	x, y;
{
	ellipsebyrad_box(INV_PAINT);
	cur_x = x;
	cur_y = y;
	ellipsebyrad_box(INV_PAINT);
	}

ellipsebyrad_box(op)
int	op;
{
	register int	x1, y1, x2, y2;
	int		rx, ry;

	rx = cur_x - fix_x;
	ry = cur_y - fix_y;
	x1 = fix_x + rx;
	x2 = fix_x - rx;
	y1 = fix_y + ry;
	y2 = fix_y - ry;
	draw_rectbox(x1, y1, x2, y2, op);
	}

create_ellipsebyrad(x, y)
int	x, y;
{
	F_ellipse	*ellipse;

	ellipsebyrad_box(INV_PAINT);
	center_marker(fix_x, fix_y);
	if (NULL == (Ellipse_malloc(ellipse))) {
	    blink_msg();
	    put_msg(Err_mem);
	    return;
	    }
	ellipse->type = T_ELLIPSE_BY_RAD;
	ellipse->style = cur_line_style;
	ellipse->thickness = line_thickness;
	ellipse->style_val = cur_styleval;
	ellipse->angle = cur_angle;
	ellipse->color = cur_color;
	ellipse->depth = 0;
	ellipse->pen = NULL;
	ellipse->area_fill = NULL;
	ellipse->direction = 1;
	ellipse->center.x = fix_x;
	ellipse->center.y = fix_y;
	ellipse->radiuses.x = abs(x - fix_x) +1;
	ellipse->radiuses.y = abs(y - fix_y) +1;
	ellipse->start.x = fix_x;
	ellipse->start.y = fix_y;
	ellipse->end.x = x;
	ellipse->end.y = y;
	ellipse->next = NULL;
	pw_batch_on(canvas_pixwin);
	draw_ellipse(ellipse, DRAW);
	pw_batch_off(canvas_pixwin);
	clean_up();
	set_action_object(F_CREATE, O_ELLIPSE);
	insert_ellipse(&objects.ellipses, ellipse);
	set_latestellipse(ellipse);
	set_modifiedflag();
	ellipsebyradius_drawing_selected();
	}

ellipsebydiameter_drawing_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_ellipsebydiameter_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_ellipsebydiameter_drawing(x, y)
int	x, y;
{
	cur_x = fix_x = x; 
	cur_y = fix_y = y;
	center_marker(fix_x, fix_y);
	canvas_locmove_proc = move_ebdbox;
	canvas_leftbut_proc = null_proc;
	canvas_middlebut_proc = create_ellipsebydia;
	set_temp_cursor(&null_cursor);
	ellipsebydia_box(INV_PAINT);
	set_action_on();
	}

move_ebdbox(x, y)
int	x, y;
{
	ellipsebydia_box(INV_PAINT);
	cur_x = x;
	cur_y = y;
	ellipsebydia_box(INV_PAINT);
	}

ellipsebydia_box(op)
int	op;
{
	draw_rectbox(fix_x, fix_y, cur_x, cur_y, op);
	}

create_ellipsebydia(x, y)
int	x, y;
{
	F_ellipse	*ellipse;

	ellipsebydia_box(INV_PAINT);
	center_marker(fix_x, fix_y);
	if (NULL == (Ellipse_malloc(ellipse))) {
	    blink_msg();
	    put_msg(Err_mem);
	    return;
	    }
	ellipse->type = T_ELLIPSE_BY_DIA;
	ellipse->style = cur_line_style;
	ellipse->thickness = line_thickness;
	ellipse->style_val = cur_styleval;
	ellipse->angle = cur_angle;
	ellipse->color = cur_color;
	ellipse->depth = 0;
	ellipse->pen = NULL;
	ellipse->area_fill = NULL;
	ellipse->direction = 1;
	ellipse->center.x = (fix_x + x) / 2;
	ellipse->center.y = (fix_y + y) / 2;
	ellipse->radiuses.x = abs(ellipse->center.x - fix_x);
	ellipse->radiuses.y = abs(ellipse->center.y - fix_y);
	ellipse->start.x = fix_x;
	ellipse->start.y = fix_y;
	ellipse->end.x = x;
	ellipse->end.y = y;
	ellipse->next = NULL;
	pw_batch_on(canvas_pixwin);
	draw_ellipse(ellipse, DRAW);
	pw_batch_off(canvas_pixwin);
	clean_up();
	set_action_object(F_CREATE, O_ELLIPSE);
	insert_ellipse(&objects.ellipses, ellipse);
	set_latestellipse(ellipse);
	set_modifiedflag();
	ellipsebydiameter_drawing_selected();
	}

/***************************  circle  section  ************************/

circlebyradius_drawing_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_circlebyradius_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_circlebyradius_drawing(x, y)
int	x, y;
{
	cur_x = fix_x = x; 
	cur_y = fix_y = y;
	center_marker(fix_x, fix_y);
	canvas_locmove_proc = move_cbrbox;
	canvas_leftbut_proc = null_proc;
	canvas_middlebut_proc = create_circlebyrad;
	set_temp_cursor(&null_cursor);
	circlebyrad_box(INV_PAINT);
	set_action_on();
	}

move_cbrbox(x, y)
int	x, y;
{
	circlebyrad_box(INV_PAINT);
	cur_x = x;
	cur_y = y;
	circlebyrad_box(INV_PAINT);
	}

circlebyrad_box(op)
int	op;
{
	register int	x1, y1, x2, y2;
	int		radius, rx, ry;

	rx = cur_x - fix_x;
	ry = cur_y - fix_y;
	radius = round(sqrt((double) (rx*rx + ry*ry)));
	x1 = fix_x + radius;
	x2 = fix_x - radius;
	y1 = fix_y + radius;
	y2 = fix_y - radius;
	draw_rectbox(x1, y1, x2, y2, op);
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, op, 1);
	}

create_circlebyrad(x, y)
int	x, y;
{
	F_ellipse	*c;
	int		rx, ry;

	circlebyrad_box(INV_PAINT);
	center_marker(fix_x, fix_y);
	if (NULL == (Ellipse_malloc(c))) {
	    blink_msg();
	    put_msg(Err_mem);
	    return;
	    }
	c->type = T_CIRCLE_BY_RAD;
	c->style = cur_line_style;
	c->thickness = line_thickness;
	c->style_val = cur_styleval;
	c->angle = 0.0;
	c->color = cur_color;
	c->depth = 0;
	c->pen = NULL;
	c->area_fill = NULL;
	c->direction = 1;
	c->center.x = fix_x;
	c->center.y = fix_y;
	rx = fix_x - x;  ry = fix_y - y;
	c->radiuses.x = c->radiuses.y = round(sqrt((double)(rx*rx + ry*ry)));
	c->start.x = fix_x;
	c->start.y = fix_y;
	c->end.x = x;
	c->end.y = y;
	c->next = NULL;
	pw_batch_on(canvas_pixwin);
	draw_ellipse(c, DRAW);
	pw_batch_off(canvas_pixwin);
	clean_up();
	set_action_object(F_CREATE, O_ELLIPSE);
	insert_ellipse(&objects.ellipses, c);
	set_latestellipse(c);
	set_modifiedflag();
	circlebyradius_drawing_selected();
	}

circlebydiameter_drawing_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_circlebydiameter_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_circlebydiameter_drawing(x, y)
int	x, y;
{
	cur_x = fix_x = x; 
	cur_y = fix_y = y;
	center_marker(fix_x, fix_y);
	canvas_locmove_proc = move_cbdbox;
	canvas_leftbut_proc = null_proc;
	canvas_middlebut_proc = create_circlebydia;
	set_temp_cursor(&null_cursor);
	circlebydia_box(INV_PAINT);
	set_action_on();
	}

move_cbdbox(x, y)
int	x, y;
{
	circlebydia_box(INV_PAINT);
	cur_x = x;
	cur_y = y;
	circlebydia_box(INV_PAINT);
	}

circlebydia_box(op)
int	op;
{
	register int	x1, y1, x2, y2;
	int		radius, rx, ry;

	rx = (cur_x - fix_x) / 2;
	ry = (cur_y - fix_y) / 2;
	radius = round(sqrt((double) (rx*rx + ry*ry)));
	x1 = fix_x + rx + radius;
	x2 = fix_x + rx - radius;
	y1 = fix_y + ry + radius;
	y2 = fix_y + ry - radius;
	draw_rectbox(x1, y1, x2, y2, op);
	pw_vector(canvas_pixwin, fix_x, fix_y, cur_x, cur_y, op, 1);
	}

create_circlebydia(x, y)
int	x, y;
{
	F_ellipse	*c;
	int		rx, ry;

	circlebydia_box(INV_PAINT);
	center_marker(fix_x, fix_y);
	if (NULL == (Ellipse_malloc(c))) {
	    blink_msg();
	    put_msg(Err_mem);
	    return;
	    }
	c->type = T_CIRCLE_BY_DIA;
	c->style = cur_line_style;
	c->thickness = line_thickness;
	c->style_val = cur_styleval;
	c->angle = 0.0;
	c->color = cur_color;
	c->depth = 0;
	c->pen = NULL;
	c->area_fill = NULL;
	c->direction = 1;
	c->center.x = (fix_x + x) / 2 + .5;
	c->center.y = (fix_y + y) / 2 + .5;
	rx = x - c->center.x;  ry = y - c->center.y;
	c->radiuses.x = c->radiuses.y = round(sqrt((double)(rx*rx + ry*ry)));
	c->start.x = fix_x;
	c->start.y = fix_y;
	c->end.x = x;
	c->end.y = y;
	c->next = NULL;
	pw_batch_on(canvas_pixwin);
	draw_ellipse(c, DRAW);
	pw_batch_off(canvas_pixwin);
	clean_up();
	set_action_object(F_CREATE, O_ELLIPSE);
	insert_ellipse(&objects.ellipses, c);
	set_latestellipse(c);
	set_modifiedflag();
	circlebydiameter_drawing_selected();
	}

draw_ellipse(e, op)
F_ellipse	*e;
int		op;
{
	int	a, b;

	a = e->radiuses.x;
	b = e->radiuses.y;
	curve(a, 0, a, 0, e->direction, (b*b), (a*a), e->center.x, 
		e->center.y, op);
	}
