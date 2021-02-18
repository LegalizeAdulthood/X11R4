/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985, Add closed spline.
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
extern int		arrow_ht, arrow_wid;
extern F_compound	objects;
extern int		num_point;
extern int		DEBUG;
extern F_point		*first_point, *cur_point;

int			create_splineobject();
			init_spline_drawing();

draw_spline_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_spline_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_spline_drawing(x, y)
int	x, y;
{
	init_line_drawing(x, y);
	canvas_middlebut_proc = create_splineobject;
	canvas_rightbut_proc = null_proc;
	}

create_splineobject(x, y)
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
	    draw_spline_selected();
	    return;
	    }
	Spline_malloc(spline);
	spline->style = cur_line_style;
	spline->thickness = line_thickness;
	spline->style_val = cur_styleval;
	spline->color = cur_color;
	spline->depth = 0;
	spline->pen = NULL;
	spline->area_fill = NULL;
	spline->points = first_point;
	spline->controls = NULL; 
	spline->next = NULL; 
	cur_x = cur_y = fix_x = fix_y = 0; /* used in draw_movingpoint */
	draw_movingpoint(spline->points, INV_PAINT); /* erase control vector */
	pw_batch_on(canvas_pixwin);
	if (cur_command == F_CLOSED_SPLINE) {
	    spline->type = T_CLOSED_NORMAL;
	    spline->for_arrow = NULL;
	    spline->back_arrow = NULL;
	    num_point++;
	    append_point(first_point->x, first_point->y, &cur_point);
	    draw_closed_spline(spline, PAINT);
	    }
	else {	/* It must be F_SPLINE */
	    if (autoforwardarrow_mode)
		spline->for_arrow = forward_arrow();
	    else
		spline->for_arrow = NULL;
	    if (autobackwardarrow_mode)
		spline->back_arrow = backward_arrow();
	    else
		spline->back_arrow = NULL;
	    spline->type = T_OPEN_NORMAL;
	    draw_open_spline(spline, PAINT);
	    }
	pw_batch_off(canvas_pixwin);
	if (DEBUG) {
	    int		xmin, ymin, xmax, ymax;
	    spline_bound(spline, &xmin, &ymin, &xmax, &ymax);
	    draw_rectbox(xmin, ymin, xmax, ymax, PAINT);
	    }
	clean_up();
	insert_spline(&objects.splines, spline);
	set_action_object(F_CREATE, O_SPLINE);
	set_latestspline(spline);
	set_modifiedflag();
	draw_spline_selected();
	}

#define			round(x)		((int) (x + .5))

draw_open_spline(spline, op)
F_spline	*spline;
int		op;
{
	F_point	*p;
	float	cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
	float	x1, y1, x2, y2;

	p = spline->points;
	x1 = p->x;  y1 = p->y;
	p = p->next;
	x2 = p->x;  y2 = p->y;
	cx1 = (x1 + x2) / 2;      cy1 = (y1 + y2) / 2;
	cx2 = (cx1 + x2) / 2;  cy2 = (cy1 + y2) / 2;
	if (spline->back_arrow) /*  backward arrow  */
	    draw_arrow((int)x2, (int)y2, (int)x1, (int)y1, 
		spline->back_arrow, op);
	pw_vector(canvas_pixwin, (int)x1, (int)y1, round(cx1), 
		round(cy1), op, 1);

	for (p = p->next; p != NULL; p = p->next) {
	    x1 = x2;  y1 = y2;
	    x2 = p->x;  y2 = p->y;
	    cx4 = (x1 + x2) / 2; cy4 = (y1 + y2) / 2;
	    cx3 = (x1 + cx4) / 2; cy3 = (y1 + cy4) / 2;
	    quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4, op);
	    cx1 = cx4;  cy1 = cy4;
	    cx2 = (cx1 + x2) / 2;  cy2 = (cy1 + y2) / 2;
	    }
	pw_vector(canvas_pixwin, round(cx1), round(cy1), 
		(int)x2, (int)y2, op, 1);
	if (spline->for_arrow) /*  forward arrow  */
	    draw_arrow((int)x1, (int)y1, (int)x2, (int)y2, 
		spline->for_arrow, op);
	}

draw_closed_spline(spline, op)
F_spline	*spline;
int		op;
{
	F_point	*p;
	float	cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
	float	x1, y1, x2, y2;

	p = spline->points;
	x1 = p->x;  y1 = p->y;
	p = p->next;
	x2 = p->x;  y2 = p->y;
	cx1 = (x1 + x2) / 2;      cy1 = (y1 + y2) / 2;
	cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;

	for (p = p->next; p != NULL; p = p->next) {
	    x1 = x2;  y1 = y2;
	    x2 = p->x;  y2 = p->y;
	    cx4 = (x1 + x2) / 2;   cy4 = (y1 + y2) / 2;
	    cx3 = (x1 + cx4) / 2;  cy3 = (y1 + cy4) / 2;
	    quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4, op);
	    cx1 = cx4;  cy1 = cy4;
	    cx2 = (cx1 + x2) / 2;  cy2 = (cy1 + y2) / 2;
	    }
	x1 = x2;  y1 = y2;
	p = spline->points->next;
	x2 = p->x;  y2 = p->y;
	cx4 = (x1 + x2) / 2;   cy4 = (y1 + y2) / 2;
	cx3 = (x1 + cx4) / 2;  cy3 = (y1 + cy4) / 2;
	quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4, op);
	}
