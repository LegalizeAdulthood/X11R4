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

/*************************** locally global procedures *********************/

			elastic_box();
int			create_boxobject();
			init_box_drawing();

box_drawing_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_box_drawing;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_box_drawing(x, y)
int	x, y;
{
	cur_x = fix_x = x; 
	cur_y = fix_y = y;
	canvas_locmove_proc = elastic_box;
	canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
	canvas_middlebut_proc = create_boxobject;
	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	set_temp_cursor(&null_cursor);
	set_action_on();
	}

draw_rectbox(x1, y1, x2, y2, op)
int	x1, y1, x2, y2, op;
{
	pw_vector(canvas_pixwin, x1, y1, x1, y2, op, 1);
	pw_vector(canvas_pixwin, x1, y2, x2, y2, op, 1);
	pw_vector(canvas_pixwin, x2, y2, x2, y1, op, 1);
	pw_vector(canvas_pixwin, x2, y1, x1, y1, op, 1);
	}

elastic_box(x, y)
int	x, y;
{
	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	cur_x = x;  
	cur_y = y;
	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	}

create_boxobject(x, y)
int	x, y;
{
	F_line	*box;
	F_point	*point;

	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);

	
	if ((Point_malloc(point)) == NULL) {
	    blink_msg();
	    put_msg(Err_mem);
	    return;
	    }
	point->x = fix_x;
	point->y = fix_y;
	point->next = NULL;

	Line_malloc(box);
	box->type = T_BOX;
	box->style = cur_line_style;
	box->thickness = line_thickness;
	box->color = cur_color;
	box->pen = NULL;
	box->area_fill = NULL;
	box->style_val = cur_styleval;
	box->for_arrow = NULL;
	box->back_arrow = NULL;
	box->points = point;
	box->next  = NULL;
	append_point(x, fix_y, &point);
	append_point(x, y, &point);
	append_point(fix_x, y, &point);
	append_point(fix_x, fix_y, &point);
	draw_line(box, PAINT);
	clean_up();
	set_action_object(F_CREATE, O_POLYLINE);
	insert_line(&objects.lines, box);
	set_latestline(box);
	set_modifiedflag();
	box_drawing_selected();
	}
