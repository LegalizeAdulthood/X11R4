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

#define			PI		3.14159

extern int		foreground_color, background_color;
extern int		fix_x, fix_y, cur_x, cur_y;

extern int		autoforwardarrow_mode, autobackwardarrow_mode;
extern int		cur_line_style, line_thickness;
extern float		cur_styleval;
extern int		cur_color;
extern float		cur_dashlength;
extern int		num_point;
extern int		DEBUG;
extern F_compound	objects;

extern			freehand_elasticline();
extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

F_pos			point[3];

			create_arcobject();
			get_arcpoint();
			init_arc_drawing();

arc_drawing_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_arc_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_arc_drawing(x, y)
int	x, y;
{
	num_point = 0;
	point[num_point].x = fix_x = cur_x = x;
	point[num_point++].y = fix_y = cur_y = y;
	canvas_locmove_proc = freehand_elasticline;	/* in line.c  */
	canvas_leftbut_proc = get_arcpoint;
	canvas_middlebut_proc = create_arcobject;
	draw_elasticline();		/* in line.c  */
	set_temp_cursor(&null_cursor);
	set_action_on();
	}

get_arcpoint(x, y)
int	x, y;
{
	if (x == fix_x && y == fix_y) return;

	if (num_point == 2) create_arcobject(x, y);
	draw_elasticline();		/* in line.c  */
	cur_x = x;  cur_y = y;
	draw_elasticline();		/* in line.c  */
	point[num_point].x = fix_x = x;
	point[num_point++].y = fix_y = y;
	draw_elasticline();		/* in line.c  */
	}

create_arcobject(lx, ly)
int	lx, ly;
{
	extern F_arrow	*forward_arrow(), *backward_arrow();
	F_arc		*arc;
	int		x, y, i;
	float		xx, yy;

	draw_elasticline();
	cur_x = lx;  cur_y = ly;
	draw_elasticline();		/* in line.c  */
	if (num_point == 1) {
	    arc_drawing_selected();
	    return;
	    }
	else if (num_point == 2) {
	    point[num_point].x = lx;
	    point[num_point++].y = ly;
	    }

	x = point[0].x;  y = point[0].y;
	for (i = 1; i < num_point; i++) {
	    pw_vector(canvas_pixwin, x, y, point[i].x, point[i].y, INV_PAINT,
			1);
	    x = point[i].x;  y = point[i].y;
	    }
	if (num_point < 3) {
	    arc_drawing_selected();
	    return;
	    }
	if (! compute_arccenter(point[0], point[1], point[2], &xx, &yy)) {
	    arc_drawing_selected();
	    return;
	    }
	Arc_malloc(arc);
	if (arc == NULL) {
	    blink_msg();
	    put_msg(Err_mem);
	    arc_drawing_selected();
	    return;
	    }
	arc->type = T_3_POINTS_ARC;
	arc->style = cur_line_style;
	arc->thickness = line_thickness;
	arc->style_val = cur_styleval;
	arc->pen = NULL;
	arc->area_fill = NULL;
	arc->color = cur_color;
	arc->depth = 0;
	arc->direction = compute_direction(point[0], point[1], point[2]);
	if (autoforwardarrow_mode)
	    arc->for_arrow = forward_arrow();
	else
	    arc->for_arrow = NULL;
	if (autobackwardarrow_mode)
	    arc->back_arrow = backward_arrow();
	else
	    arc->back_arrow = NULL;
	arc->center.x = xx;
	arc->center.y = yy;
	arc->point[0].x = point[0].x;
	arc->point[0].y = point[0].y;
	arc->point[1].x = point[1].x;
	arc->point[1].y = point[1].y;
	arc->point[2].x = point[2].x;
	arc->point[2].y = point[2].y;
	arc->next = NULL;
	pw_batch_on(canvas_pixwin);
	draw_arc(arc, foreground_color);
	if (DEBUG) {
	    int		xmin, ymin, xmax, ymax;
	    arc_bound(arc, &xmin, &ymin, &xmax, &ymax);
	    draw_rectbox(xmin, ymin, xmax, ymax, PAINT);
	    }
	pw_batch_off(canvas_pixwin);
	clean_up();
	set_action_object(F_CREATE, O_ARC);
	insert_arc(&objects.arcs, arc);
	set_latestarc(arc);
	set_modifiedflag();
	arc_drawing_selected();
	}

#define	 		round(x)	((int)((x) + .5))

draw_arc(a, op)
F_arc	*a;
int	op;
{
	extern int	foreground_color, background_color;

	curve(round(a->point[0].x - a->center.x),
		round(a->center.y - a->point[0].y),
		round(a->point[2].x - a->center.x),
		round(a->center.y - a->point[2].y),
		a->direction, 1, 1,
		round(a->center.x), round(a->center.y), op);
	draw_arcarrow(a, op);
	}

draw_arcarrow(a, op)
F_arc	*a;
int	op;
{
	int	x, y;

	if (a->for_arrow) {
	    compute_normal(a->center.x, a->center.y, a->point[2].x, 
			a->point[2].y, a->direction, &x, &y);
	    if (op == foreground_color)
		draw_arrow(x, y, a->point[2].x, a->point[2].y,
				a->for_arrow, PAINT);
	    else
		draw_arrow(x, y, a->point[2].x, a->point[2].y,
				a->for_arrow, ERASE);
	    }
	if (a->back_arrow) {
	    compute_normal(a->center.x, a->center.y, a->point[0].x, 
			a->point[0].y, a->direction ^ 1, &x, &y);
	    if (op == foreground_color)
		draw_arrow(x, y, a->point[0].x, a->point[0].y,
				a->back_arrow, PAINT);
	    else
		draw_arrow(x, y, a->point[0].x, a->point[0].y,
				a->back_arrow, ERASE);
	    }
	}
