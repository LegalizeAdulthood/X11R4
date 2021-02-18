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
#include "font.h"
#include "object.h"
#include "paintop.h"

extern F_pos		last_position, new_position;  /* undo.c   */
extern int		foreground_color, background_color;
extern int		fix_x, fix_y, cur_x, cur_y;

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

static			draw_movingbox();
static int		x1off, y1off, x2off, y2off;

			(*return_proc)();
			 move_line(), place_line();
			 move_arc(), place_arc();
			 move_spline(), place_spline();
			 move_movingbox();
			 place_ellipse();
			 move_text(), place_text();
			 place_compound();

static F_arc		*arc;
static F_compound	*compound;
static F_ellipse	*ellipse;
static F_line		*line;
static F_spline		*spline;
static F_text		*text;

static
draw_movingbox(op)
int	op;
{
	register int	x1, y1, x2, y2;

	x1 = cur_x + x1off;
	x2 = cur_x + x2off;
	y1 = cur_y + y1off;
	y2 = cur_y + y2off;
	pw_vector(canvas_pixwin, x1, y1, x1, y2, op, 1);
	pw_vector(canvas_pixwin, x1, y2, x2, y2, op, 1);
	pw_vector(canvas_pixwin, x2, y2, x2, y1, op, 1);
	pw_vector(canvas_pixwin, x2, y1, x1, y1, op, 1);
	}

move_movingbox(x, y)
int	x, y;
{
	draw_movingbox(INV_PAINT);
	cur_x = x;
	cur_y = y;
	draw_movingbox(INV_PAINT);
	}

/***************************** ellipse section ************************/

init_ellipsedragging(e, x, y)
F_ellipse	*e;
int		x, y;
{
	ellipse = e;
	last_position.x = cur_x = x; 
	last_position.y = cur_y = y;
	x1off = (e->center.x - e->radiuses.x) - cur_x; 
	x2off = (e->center.x + e->radiuses.x) - cur_x; 
	y1off = (e->center.y - e->radiuses.y) - cur_y;
	y2off = (e->center.y + e->radiuses.y) - cur_y;
	canvas_locmove_proc = move_movingbox;
	canvas_middlebut_proc = place_ellipse;
	set_action_on();
	draw_movingbox(INV_PAINT);
	}

place_ellipse(x, y)
int	x, y;
{
	draw_movingbox(INV_PAINT);
	new_position.x = x;
	new_position.y = y;
	translate_ellipse(ellipse, x - last_position.x, y - last_position.y);
	pw_batch_on(canvas_pixwin);
	draw_ellipse(ellipse, foreground_color);
	pw_batch_off(canvas_pixwin);
	show_pointmarker();
	set_modifiedflag();
	(*return_proc)();
	}

/*****************************  arc  section  *******************/

init_arcdragging(a, x, y)
F_arc	*a;
int	x, y;
{
	arc = a;
	fix_x = last_position.x = cur_x = x;
	fix_y = last_position.y = cur_y = y;
	canvas_locmove_proc = move_arc;
	canvas_middlebut_proc = place_arc;
	set_action_on();
	draw_movingarc(arc, INV_PAINT);
	}

move_arc(x, y)
int	x, y;
{
	draw_movingarc(arc, INV_PAINT);
	cur_x = x;  
	cur_y = y;
	draw_movingarc(arc, INV_PAINT);
	}

place_arc(x, y)
int	x, y;
{
	draw_movingarc(arc, INV_PAINT);
	new_position.x = x;
	new_position.y = y;
	translate_arc(arc, x - fix_x, y - fix_y);
	pw_batch_on(canvas_pixwin);
	draw_arc(arc, foreground_color);
	pw_batch_off(canvas_pixwin);
	show_pointmarker();
	set_modifiedflag();
	(*return_proc)();
	}

draw_movingarc(a, op)
F_arc	*a;
int	op;
{
	int	dx, dy;

	dx = cur_x - fix_x;
	dy = cur_y - fix_y;
	pw_vector(canvas_pixwin, a->point[0].x+dx, a->point[0].y+dy,
		a->point[1].x+dx, a->point[1].y+dy, op, 1);
	pw_vector(canvas_pixwin, a->point[1].x+dx, a->point[1].y+dy,
		a->point[2].x+dx, a->point[2].y+dy, op, 1);
	}

/*************************  line  section  **********************/

init_linedragging(l, x, y)
F_line	*l;
int	x, y;
{
	line = l;
	last_position.x = cur_x = fix_x = x;
	last_position.y = cur_y = fix_y = y;
	canvas_locmove_proc = move_line;
	canvas_middlebut_proc = place_line;
	set_action_on();
	draw_movingpoint(line->points, INV_PAINT);
	}

move_line(x, y)
int	x, y;
{
	draw_movingpoint(line->points, INV_PAINT);
	cur_x = x;  
	cur_y = y;
	draw_movingpoint(line->points, INV_PAINT);
	}

place_line(x, y)
int	x, y;
{
	draw_movingpoint(line->points, INV_PAINT);
	new_position.x = x;
	new_position.y = y;
	translate_line(line, x - fix_x, y - fix_y);
	draw_line(line, PAINT);
	show_pointmarker();
	set_modifiedflag();
	(*return_proc)();
	}

draw_movingpoint(ps, op)
F_point	*ps;
int	op;
{
	F_point	*p;
	int	dx, dy, x, y, xx, yy;

	dx = cur_x - fix_x;
	dy = cur_y - fix_y;
	p = ps;
	x = p->x + dx;
	y = p->y + dy;
	for (p = p->next; p != NULL; x = xx, y = yy, p = p->next) {
	    xx = p->x + dx;  yy = p->y +dy;
	    pw_vector(canvas_pixwin, x, y, xx, yy, op, 1);
	    }
	}

/************************  text section  **************************/

init_textdragging(t, x, y)
F_text	*t;
int	x, y;
{
	text = t;
	fix_x = cur_x = x; 
	fix_y = cur_y = y;
	x1off = t->base_x - x;
	y1off = t->base_y - y;
	canvas_locmove_proc = move_text;
	canvas_middlebut_proc = place_text;
	draw_movingtext();
	set_action_on();
	}

move_text(x, y)
int	x, y;
{
	draw_movingtext();
	cur_x = x;
	cur_y = y;
	draw_movingtext();
	}

place_text(x, y)
int	x, y;
{
	draw_movingtext();
	new_position.x = x;
	new_position.y = y;
	last_position.x = fix_x; 
	last_position.y = fix_y;
	translate_text(text, x - fix_x, y - fix_y);
	draw_text(text, PAINT);
	set_modifiedflag();
	(*return_proc)();
	}

draw_movingtext()
{
	pw_text(canvas_pixwin, cur_x+x1off, cur_y + y1off, INV_PAINT,
		canvas_font, text->cstring);
	}

/*************************  spline  section  **********************/

init_splinedragging(s, x, y)
F_spline	*s;
int		x, y;
{
	spline = s;
	last_position.x = cur_x = fix_x = x;
	last_position.y = cur_y = fix_y = y;
	canvas_locmove_proc = move_spline;
	canvas_middlebut_proc = place_spline;
	set_action_on();
	draw_movingpoint(spline->points, INV_PAINT);
	}

move_spline(x, y)
int	x, y;
{
	draw_movingpoint(spline->points, INV_PAINT);
	cur_x = x;  
	cur_y = y;
	draw_movingpoint(spline->points, INV_PAINT);
	}

place_spline(x, y)
int	x, y;
{
	draw_movingpoint(spline->points, INV_PAINT);
	translate_spline(spline, x - fix_x, y - fix_y);
	new_position.x = x;
	new_position.y = y;
	pw_batch_on(canvas_pixwin);
	draw_spline(spline, PAINT);
	pw_batch_off(canvas_pixwin);
	show_pointmarker();
	set_modifiedflag();
	(*return_proc)();
	}

/*****************************  Compound section  *******************/

init_compounddragging(c, x, y)
F_compound	*c;
int		x, y;
{
	compound = c;
	last_position.x = cur_x = x; 
	last_position.y = cur_y = y;
	x1off = c->nwcorner.x - x;
	x2off = c->secorner.x - x;
	y1off = c->nwcorner.y - y;
	y2off = c->secorner.y - y;
	canvas_locmove_proc = move_movingbox;
	canvas_middlebut_proc = place_compound;
	set_action_on();
	draw_movingbox(INV_PAINT);
	}

place_compound(x, y)
int	x, y;
{
	draw_movingbox(INV_PAINT);
	new_position.x = x;
	new_position.y = y;
	translate_compound(compound, x - last_position.x, y - last_position.y);
	draw_compound(compound);
	draw_compoundbox(compound, INV_PAINT);
	set_modifiedflag();
	(*return_proc)();
	}
