/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	March 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "object.h"
#include "paintop.h"

extern int		pointmarker_shown, compoundbox_shown;
extern int		background_color, foreground_color;
extern PIXRECTREC	dot;

/*
erase_objects(objects)
F_compound	*objects;
{
	erase_arcs(objects->arcs);
	erase_ellipses(objects->ellipses);
	erase_lines(objects->lines);
	erase_texts(objects->texts);
	erase_splines(objects->splines);
	erase_compounds(objects->compounds);
	}
*/

erase_splines(splines)
F_spline	*splines;
{
	F_spline	*s;

	pw_batch_on(canvas_pixwin);
	for (s = splines; s != NULL; s = s->next) {
	    if (pointmarker_shown) toggle_splinepointmarker(s);
	    draw_spline(s, ERASE);
	    };
	pw_batch_off(canvas_pixwin);
	}

erase_ellipses(ellipses)
F_ellipse	*ellipses;
{
	F_ellipse	*e;

	pw_batch_on(canvas_pixwin);
	for (e = ellipses; e != NULL; e = e->next) {
	    if (pointmarker_shown) toggle_ellipsepointmarker(e);
	    draw_ellipse(e, background_color);
	    };
	pw_batch_off(canvas_pixwin);
	}

erase_arcs(arcs)
F_arc	*arcs;
{
	F_arc	*a;

	pw_batch_on(canvas_pixwin);
	for (a = arcs; a != NULL; a = a->next) {
	    if (pointmarker_shown) toggle_arcpointmarker(a);
	    draw_arc(a, background_color);
	    };
	pw_batch_off(canvas_pixwin);
	}

erase_compounds(compounds)
F_compound	*compounds;
{
	F_compound	*c;

	for (c = compounds; c != NULL; c = c->next) {
	    if (compoundbox_shown) draw_compoundbox(c, INV_PAINT);
	    erase_compound(c);
	    };
	}

erase_lines(lines)
F_line	*lines;
{
	F_line	*l;

	for (l = lines; l != NULL; l = l->next) {
	    if (pointmarker_shown) toggle_linepointmarker(l);
	    draw_line(l, ERASE);
	    };
	}

erase_texts(texts)
F_text	*texts;
{
	F_text	*t;

	for (t = texts; t != NULL; t = t->next) {
	    draw_text(t, INV_PAINT);
	    };
	}

/*
draw_objects(objects)
F_compound	*objects;
{
	draw_arcs(objects->arcs);
	draw_ellipses(objects->ellipses);
	draw_lines(objects->lines);
	draw_texts(objects->texts);
	draw_splines(objects->splines);
	draw_compounds(objects->compounds);
	}
*/

draw_ellipses(ellipses)
F_ellipse	*ellipses;
{
	F_ellipse	*e;

	pw_batch_on(canvas_pixwin);
	for (e = ellipses; e != NULL; e = e->next) {
	    draw_ellipse(e, foreground_color);
	    if (pointmarker_shown) toggle_ellipsepointmarker(e);
	    };
	pw_batch_off(canvas_pixwin);
	}

draw_arcs(arcs)
F_arc	*arcs;
{
	F_arc	*a;

	pw_batch_on(canvas_pixwin);
	for (a = arcs; a != NULL; a = a->next) {
	    draw_arc(a, foreground_color);
	    if (pointmarker_shown) toggle_arcpointmarker(a);
	    };
	pw_batch_off(canvas_pixwin);
	}

draw_lines(lines)
F_line	*lines;
{
	F_line	*l;

	pw_batch_on(canvas_pixwin);
	for (l = lines; l != NULL; l = l->next) {
	    draw_line(l, PAINT);
	    if (pointmarker_shown) toggle_linepointmarker(l);
	    };
	pw_batch_off(canvas_pixwin);
	}

draw_splines(splines)
F_spline	*splines;
{
	F_spline	*s;

	pw_batch_on(canvas_pixwin);
	for (s = splines; s != NULL; s = s->next) {
	    draw_spline(s, PAINT);
	    if (pointmarker_shown) toggle_splinepointmarker(s);
	    };
	pw_batch_off(canvas_pixwin);
	}

draw_texts(texts)
F_text	*texts;
{
	F_text	*t;

	for (t = texts; t != NULL; t = t->next) {
	    draw_text(t, PAINT);
	    };
	}

draw_compounds(compounds)
F_compound	*compounds;
{
	F_compound	*c;

	for (c = compounds; c != NULL; c = c->next) {
	    draw_compound(c);
	    if (compoundbox_shown) draw_compoundbox(c, INV_PAINT);
	    };
	}

/*	draw arrow heading from (x1, y1) to (x2, y2)	*/

draw_arrow(x1, y1, x2, y2, arrow, op)
int	x1, y1, x2, y2, op;
F_arrow	*arrow;
{
	float	x, y, xb, yb, dx, dy, l, sina, cosa;
	int	xc, yc, xd, yd;
	float	wid = arrow->wid, ht = arrow->ht;

	dx = x2 - x1;  dy = y1 - y2;
	l = sqrt((double)(dx*dx + dy*dy));
	sina = dy / l;  cosa = dx / l;
	xb = x2*cosa - y2*sina;
	yb = x2*sina + y2*cosa;
	x = xb - ht;
	y = yb - wid / 2;
	xc = x*cosa + y*sina + .5;
	yc = -x*sina + y*cosa + .5;
	y = yb + wid / 2;
	xd = x*cosa + y*sina + .5;
	yd = -x*sina + y*cosa + .5;
	pw_vector(canvas_pixwin, xc, yc, x2, y2, op, 1);
	pw_vector(canvas_pixwin, xd, yd, x2, y2, op, 1);
	}

draw_spline(spline, op)
F_spline	*spline;
int		op;
{
	if (int_spline(spline))
	    draw_intspline(spline, op);
	else if (spline->type == T_CLOSED_NORMAL)
	    draw_closed_spline(spline, op);
	else if (spline->type == T_OPEN_NORMAL)
	    draw_open_spline(spline, op);
	}

#define		STACK_DEPTH		32
typedef		struct stack {
			float	x1, y1, x2, y2, x3, y3, x4, y4;
			}
		Stack;
static Stack	stack[20];
static Stack	*stack_top;
static int	stack_count;

clear_stack()
{
	stack_top = stack;
	stack_count = 0;
	}

push(x1, y1, x2, y2, x3, y3, x4, y4)
float	x1, y1, x2, y2, x3, y3, x4, y4;
{
	stack_top->x1 = x1;
	stack_top->y1 = y1;
	stack_top->x2 = x2;
	stack_top->y2 = y2;
	stack_top->x3 = x3;
	stack_top->y3 = y3;
	stack_top->x4 = x4;
	stack_top->y4 = y4;
	stack_top++;
	stack_count++;
	}

int
pop(x1, y1, x2, y2, x3, y3, x4, y4)
float	*x1, *y1, *x2, *y2, *x3, *y3, *x4, *y4;
{
	if (stack_count == 0) return(0);
	stack_top--;
	stack_count--;
	*x1 = stack_top->x1;
	*y1 = stack_top->y1;
	*x2 = stack_top->x2;
	*y2 = stack_top->y2;
	*x3 = stack_top->x3;
	*y3 = stack_top->y3;
	*x4 = stack_top->x4;
	*y4 = stack_top->y4;
	return(1);
	}

draw_line(line, op)
F_line	*line;
int	op;
{
	F_point			*point;
	int			xx, yy, x, y;

	point = line->points;
	x = point->x;
	y = point->y;
	if (line->points->next == NULL) { /* A single point */
#ifndef	X11
	    pw_write(canvas_pixwin, x, y, 1, 1, op, &dot, 0, 0);
#else
		XDrawPoint(tool_d, canvas_pixwin, gccache[op], x, y);
#endif	X11
	    return;
	    }
	if (line->back_arrow) /* backward arrow  */
	    draw_arrow(point->next->x, point->next->y, x, y, 
		line->back_arrow, op);
	for (point = point->next; point != NULL; point = point->next) {
	    draw_line_segment(line->style, line->style_val, 
				x, y, point->x, point->y, op);
	    xx = x; yy = y;
	    x = point->x;
	    y = point->y;
	    }
	if (line->for_arrow) 
	    draw_arrow(xx, yy, x, y, line->for_arrow, op);
	}

#define		round(x)	((int)((x) + .5))
#define		dash_length	style_val
#define		dot_gap		style_val

draw_line_segment(line_style, style_val, x1, y1, x2, y2, op)
int	line_style, x1, y1, x2, y2, op;
float	style_val;
{
	switch (line_style) {
	    case SOLID_LINE :
		pw_vector(canvas_pixwin, x1, y1, x2, y2, op, 1);
		break;
	    case DASH_LINE : {
		float	x, y, leng, dx, dy, dash_x, dash_y;
		float	spacing_leng, space_x, space_y;
		int	num_spacing;

		dx = x2 - x1;
		dy = y2 - y1;
		leng = sqrt((double)(dx*dx + dy*dy));
		if (leng <= dash_length) {
		    pw_vector(canvas_pixwin, x1, y1, x2, y2, op, 1);
		    break;
		    }
		dash_x = dash_length * dx / leng;
		dash_y = dash_length * dy / leng;
		num_spacing = (leng - dash_length) / (2 * dash_length);
		spacing_leng = (leng - dash_length * (num_spacing + 1)) /
				((float)num_spacing);
		space_x = spacing_leng * dx / leng;
		space_y = spacing_leng * dy / leng;
		dx = dash_x + space_x;  dy = dash_y + space_y;
		for (x = x1, y = y1; leng > 0; 
			x += dx, y += dy, leng -= dash_length + spacing_leng) {
		    pw_vector(canvas_pixwin, round(x), round(y), 
				round(x + dash_x), round(y + dash_y), op, 1);
		    }
		break;
		}
	    case DOTTED_LINE : {
		float	x, y, leng, dx, dy, gap_x, gap_y;
		int	n, color;

		dot_gap += 1.0;
		dx = x2 - x1;
		dy = y2 - y1;
		leng = sqrt((double)(dx*dx + dy*dy)) - 1;
		n = round(leng/dot_gap);
		if (op == PAINT)
		    color = foreground_color;
		else
		    color = background_color;
		if (n <= 1) {
		    pw_put(canvas_pixwin, x1, y1, color);
		    pw_put(canvas_pixwin, x2, y2, color);
		    break;
		    }
		dot_gap = leng / n;
		gap_x = dot_gap * dx / leng;
		gap_y = dot_gap * dy / leng;
		for (x = x1, y = y1; n-- >= 0; x += gap_x, y += gap_y)
		    pw_put(canvas_pixwin, round(x), round(y), color);
		break;
		}
	    }
	}
