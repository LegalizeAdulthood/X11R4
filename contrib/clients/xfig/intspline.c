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

extern int		cur_line_style, line_thickness;
extern float		cur_styleval;
extern int		cur_color;
extern int		fix_x, fix_y, cur_x, cur_y;
extern int		cur_command;
extern int		manhattan_mode, mountain_mode;
extern int		autoforwardarrow_mode;
extern int		autobackwardarrow_mode;
extern F_compound	objects;
extern int		num_point;
extern int		DEBUG;
extern F_point		*first_point, *cur_point;

int			create_intsplineobject();
			init_intspline_drawing();

draw_intspline_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_intspline_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_intspline_drawing(x, y)
int	x, y;
{
	init_line_drawing(x, y);
	canvas_middlebut_proc = create_intsplineobject;
	canvas_rightbut_proc = null_proc;
	}

create_intsplineobject(x, y)
int	x, y;
{
	extern F_arrow	*forward_arrow(), *backward_arrow();
	F_spline	*spline;

	if (x != fix_x || y != fix_y) get_intermediatepoint(x, y);
	draw_elasticline();
	if (num_point <= 2) {
	    pw_vector(canvas_pixwin, first_point->x, first_point->y,
			cur_point->x, cur_point->y, PAINT, 0);
	    if (num_point == 1) free((char*)cur_point);
	    free((char*)first_point);
	    draw_intspline_selected();
	    return;
	    }
	if (NULL == (Spline_malloc(spline))) {
	    if (num_point == 1) free((char*)cur_point);
	    free((char*)first_point);
	    put_msg(Err_mem);
	    return;
	    }
	spline->style = cur_line_style;
	spline->thickness = line_thickness;
	spline->style_val = cur_styleval;
	spline->color = cur_color;
	spline->depth = 0;
	spline->area_fill = NULL;
	spline->pen = NULL;
	spline->points = first_point;
	spline->controls = NULL; 
	spline->next = NULL; 
	cur_x = cur_y = fix_x = fix_y = 0; /* used in draw_movingpoint */
	draw_movingpoint(spline->points, INV_PAINT); /* erase control vector */
	spline->for_arrow = NULL;
	spline->back_arrow  = NULL;
	if (cur_command == F_CLOSED_INTSPLINE) {
	    spline->type = T_CLOSED_INTERPOLATED;
	    num_point++;
	    append_point(first_point->x, first_point->y, &cur_point);
	    }
	else {
	    spline->type = T_OPEN_INTERPOLATED;
	    if (autoforwardarrow_mode) spline->for_arrow = forward_arrow();
	    if (autobackwardarrow_mode) spline->back_arrow = backward_arrow();
	    }
	make_control_points(spline);
	pw_batch_on(canvas_pixwin);
	draw_intspline(spline, PAINT);
	if (DEBUG) {
	    int	xmin, ymin, xmax, ymax;
	    spline_bound(spline, &xmin, &ymin, &xmax, &ymax);
	    draw_rectbox(xmin, ymin, xmax, ymax, PAINT);
	    }
	pw_batch_off(canvas_pixwin);
	clean_up();
	set_action_object(F_CREATE, O_SPLINE);
	insert_spline(&objects.splines, spline);
	set_latestspline(spline);
	set_modifiedflag();
	draw_intspline_selected();
	}

/* Tension : 0 (min) -> 1 (max)	*/

#define		round(x)	((int) (x + .5))

create_control_list(s)
F_spline	*s;
{
	F_point		*p;
	F_control	*cp;

	if (NULL == (Control_malloc(cp))) {
	    put_msg(Err_mem);
	    return(-1);
	    }
	s->controls = cp;
	for (p = s->points->next; p != NULL; p = p->next) {
	    if (NULL == (Control_malloc(cp->next))) {
		put_msg(Err_mem);
		return(-1);
		}
	    cp = cp->next;
	    }
	cp->next = NULL;
	return(1);
	}

make_control_points(s)
F_spline	*s;
{
	if (-1 == create_control_list(s)) return;

	remake_control_points(s);
	}

remake_control_points(s)
F_spline	*s;
{
	if (s->type == T_CLOSED_INTERPOLATED)
	    compute_cp(s->points, s->controls, CLOSED_PATH);
	else
	    compute_cp(s->points, s->controls, OPEN_PATH);
	}

draw_intspline(s, op)
F_spline	*s;
int		op;
{
	F_point		*p1, *p2;
	F_control	*cp1, *cp2;

	p1 = s->points;
	cp1 = s->controls;
	if (s->for_arrow)
	    draw_arrow(round(cp1->rx), round(cp1->ry), p1->x,
			p1->y, s->for_arrow, op);
	for (p2 = p1->next, cp2 = cp1->next; p2 != NULL;
		p1 = p2, cp1 = cp2, p2 = p2->next, cp2 = cp2->next) {
	    bezier_spline((float)p1->x, (float)p1->y, cp1->rx, cp1->ry,
			cp2->lx, cp2->ly, (float)p2->x, (float)p2->y, op);
	    }
	if (s->back_arrow)
	    draw_arrow(round(cp1->lx), round(cp1->ly), p1->x,
			p1->y, s->back_arrow, op);
	}

#define		T		0.45
#define		_2xPI		6.2832
#define		_1dSQR2		0.7071
#define		_SQR2		1.4142

compute_cp(points, controls, path)
F_point		*points;
F_control	*controls;
int		path;
{
	F_control	*cp, *cpn;
	F_point		*p, *p2, *pk;	/* Pk is the next-to-last point. */
	float		dx, dy;
	float		x1, y1, x2, y2, x3, y3;
	float		l1, l2, theta1, theta2;

	x1 = points->x;  y1 = points->y;
	pk = p2 = points->next;
	x2 = p2->x;  y2 = p2->y;
	p = p2->next;
	x3 = p->x;  y3 = p->y;

	dx = x1 - x2;
	dy = y2 - y1;
	l1 = sqrt((double)(dx*dx + dy*dy));
	theta1 = atan2((double)dy, (double)dx);
	dx = x3 - x2;
	dy = y2 - y3;
	l2 = sqrt((double)(dx*dx + dy*dy));
	theta2 = atan2((double)dy, (double)dx);
	/* -PI <= theat1, theta2 <= PI */
	if (theta1 < 0) theta1 += _2xPI;
	if (theta2 < 0) theta2 += _2xPI;
	/* 0 <= theat1, theta2 < 2PI */

	cp = controls->next;
	control_points(x2, y2, l1, l2, theta1, theta2, T, cp);
	/* control points for (x2, y2) */
	if (path == OPEN_PATH) {
	    controls->lx = 0.0; controls->ly = 0.0;
	    controls->rx = (x1 + 3*cp->lx)/4; controls->ry = (y1 + 3*cp->ly)/4;
	    cp->lx = (3*cp->lx + x2)/4; cp->ly = (3*cp->ly + y2)/4;
	    }

	while (1) {
	    x2 = x3; y2 = y3;
	    l1 = l2;
	    if (theta2 >= M_PI)
		theta1 = theta2 - M_PI;
	    else
		theta1 = theta2 + M_PI;
	    if ((p = p->next) == NULL) break;
	    pk = pk->next;
	    x3 = p->x; y3 = p->y;
	    dx = x3 - x2;
	    dy = y2 - y3;
	    l2 = sqrt((double)(dx*dx + dy*dy));
	    theta2 = atan2((double)dy, (double)dx);
	    if (theta2 < 0) theta2 += _2xPI;
	    cp = cp->next;
	    control_points(x2, y2, l1, l2, theta1, theta2, T, cp);
	    };

	if (path == CLOSED_PATH) {
	    dx = p2->x - x2;
	    dy = y2 - p2->y;
	    l2 = sqrt((double)(dx*dx + dy*dy));
	    theta2 = atan2((double)dy, (double)dx);
	    if (theta2 < 0) theta2 += _2xPI;
	    cp = cp->next;
	    control_points(x2, y2, l1, l2, theta1, theta2, T, cp);
	    controls->lx = cp->lx; controls->ly = cp->ly;
	    controls->rx = cp->rx; controls->ry = cp->ry;
	    }
	else {
	    cpn = cp->next;
	    cpn->lx = (3*cp->rx + x2) / 4; cpn->ly = (3*cp->ry + y2) / 4;
	    cpn->rx = 0.0; cpn->ry = 0.0;
	    cp->rx = (pk->x + 3*cp->rx) / 4; cp->ry = (pk->y + 3*cp->ry) / 4;
	    }
	}

/*
The parameter t is the tension.  It must range in [0, 1].
The bigger the value of t, the higher the tension.
*/

control_points(x, y, l1, l2, theta1, theta2, t, cp)
float	x, y, l1, l2, theta1, theta2, t;
F_control	*cp;
{
	float	s, theta, r = 1 - t;

	/* 0 <= theat1, theta2 < 2PI */

	theta = (theta1 + theta2) / 2;

	if (theta1 > theta2) {
	    s = sin((double)(theta-theta2));
	    theta1 = theta + M_PI_2;
	    theta2 = theta - M_PI_2;
	    }
	else {
	    s = sin((double)(theta2-theta));
	    theta1 = theta - M_PI_2;
	    theta2 = theta + M_PI_2;
	    }
	if (s > _1dSQR2) s = _SQR2 - s;
	s *= r;
	l1 *= s; l2 *= s;
	cp->lx = x + l1 * cos((double)theta1);
	cp->ly = y - l1 * sin((double)theta1);
	cp->rx = x + l2 * cos((double)theta2);
	cp->ry = y - l2 * sin((double)theta2);
	}
