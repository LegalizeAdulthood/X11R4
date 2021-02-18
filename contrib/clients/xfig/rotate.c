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

#define			TOLERANCE	7

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

extern F_line		*line_search(), *copy_line();
extern F_ellipse	*ellipse_search(), *copy_ellipse();
extern F_ellipse	*ellipse_point_search();
extern F_spline		*spline_search(), *copy_spline();
extern F_arc		*arc_point_search(), *copy_arc();
extern F_compound	*compound_search(), *copy_compound();

extern F_compound	objects;

extern int		rotate_angle;
extern int		pointmarker_shown;
extern int		foreground_color, background_color;

static int		copy;
			init_rotate();
			init_copynrotate();

rotate_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_copynrotate;
	canvas_middlebut_proc = init_rotate;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick15_cursor);
	}

init_rotate(x, y)
int	x, y;
{
	copy = 0;
	rotate_search(x, y);
	}

init_copynrotate(x, y)
int	x, y;
{
	copy = 1;
	rotate_search(x, y);
	}

rotate_search(x, y)
int	x, y;
{
	F_line		*l;
	F_arc		*a;
	F_ellipse	*e;
	F_spline	*s;
	F_compound	*c;
	int		px, py;

	if ((l = line_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    init_rotateline(l, px, py);
	    }
	else if ((e = ellipse_point_search(x, y, TOLERANCE, &px)) != NULL) {
	    if (px == 0)
		init_rotateellipse(e, e->start.x, e->start.y);
	    else
		init_rotateellipse(e, e->end.x, e->end.y);
	    }
	else if ((e = ellipse_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    init_rotateellipse(e, x, y);
	    }
	else if ((a = arc_point_search(x, y, TOLERANCE, &px)) != NULL) {
	    init_rotatearc(a, a->point[px].x, a->point[px].y);
	    }
	else if ((s = spline_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    init_rotatespline(s, px, py);
	    }
	else if ((c = compound_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    init_rotatecompound(c, px, py);
	    }
	else {
	    return;
	    }
	set_modifiedflag();
	}

init_rotateline(l, px, py)
F_line	*l;
int	px, py;
{
	F_line	*line;

	win_setmouseposition(canvas_swfd, px, py);
	if (pointmarker_shown) toggle_linepointmarker(l);
	if (copy) {
	    line = copy_line(l);
	    rotate_line(line, px, py, rotate_angle);
	    insert_line(&objects.lines, line);
	    clean_up();
	    set_action_object(F_CREATE, O_POLYLINE);
	    draw_line(line, PAINT);
	    }
	else {
	    draw_line(l, ERASE);
	    rotate_line(l, px, py, rotate_angle);
	    clean_up();
	    set_action_object(F_ROTATE, O_POLYLINE);
	    set_lastangle(rotate_angle);
	    set_lastposition(px, py);
	    draw_line(l, PAINT);
	    line = l;
	    }
	if (pointmarker_shown) {
	    if (copy) toggle_linepointmarker(line);
	    toggle_linepointmarker(l);
	    }
	set_latestline(line);
	}

init_rotateellipse(e, px, py)
F_ellipse	*e;
int		px, py;
{
	F_ellipse	*ellipse;

	win_setmouseposition(canvas_swfd, px, py);
	if (pointmarker_shown) toggle_ellipsepointmarker(e);
	if (copy) {
	    ellipse = copy_ellipse(e);
	    rotate_ellipse(ellipse, px, py, rotate_angle);
	    insert_ellipse(&objects.ellipses, ellipse);
	    clean_up();
	    set_action_object(F_CREATE, O_ELLIPSE);
	    draw_ellipse(ellipse, PAINT);
	    }
	else {
	    draw_ellipse(e, background_color);
	    rotate_ellipse(e, px, py, rotate_angle);
	    clean_up();
	    set_action_object(F_ROTATE, O_ELLIPSE);
	    set_lastangle(rotate_angle);
	    set_lastposition(px, py);
	    draw_ellipse(e, foreground_color);
	    ellipse = e;
	    }
	if (pointmarker_shown) {
	    if (copy) toggle_ellipsepointmarker(ellipse);
	    toggle_ellipsepointmarker(e);
	    }
	set_latestellipse(ellipse);
	}

init_rotatearc(a, px, py)
F_arc	*a;
int	px, py;
{
	F_arc	*arc;

	win_setmouseposition(canvas_swfd, px, py);
	if (pointmarker_shown) toggle_arcpointmarker(a);
	if (copy) {
	    arc = copy_arc(a);
	    rotate_arc(arc, px, py, rotate_angle);
	    insert_arc(&objects.arcs, arc);
	    clean_up();
	    set_action_object(F_CREATE, O_ARC);
	    draw_arc(arc, foreground_color);
	    }
	else {
	    draw_arc(a, background_color);
	    rotate_arc(a, px, py, rotate_angle);
	    clean_up();
	    set_action_object(F_ROTATE, O_ARC);
	    set_lastangle(rotate_angle);
	    set_lastposition(px, py);
	    draw_arc(a, foreground_color);
	    arc = a;
	    }
	if (pointmarker_shown) {
	    if (copy) toggle_arcpointmarker(arc);
	    toggle_arcpointmarker(a);
	    }
	set_latestarc(arc);
	}

init_rotatespline(s, px, py)
F_spline	*s;
int		px, py;
{
	F_spline	*spline;

	win_setmouseposition(canvas_swfd, px, py);
	if (pointmarker_shown) toggle_splinepointmarker(s);
	if (copy) {
	    spline = copy_spline(s);
	    rotate_spline(spline, px, py, rotate_angle);
	    insert_spline(&objects.splines, spline);
	    clean_up();
	    set_action_object(F_CREATE, O_SPLINE);
	    draw_spline(spline, PAINT);
	    }
	else {
	    draw_spline(s, ERASE);
	    rotate_spline(s, px, py, rotate_angle);
	    clean_up();
	    set_action_object(F_ROTATE, O_SPLINE);
	    set_lastangle(rotate_angle);
	    set_lastposition(px, py);
	    draw_spline(s, PAINT);
	    spline = s;
	    }
	if (pointmarker_shown) {
	    if (copy) toggle_splinepointmarker(spline);
	    toggle_splinepointmarker(s);
	    }
	set_latestspline(spline);
	}

init_rotatecompound(c, px, py)
F_compound	*c;
int		px, py;
{
	F_compound	*compound;

	win_setmouseposition(canvas_swfd, px, py);
	set_temp_cursor(&wait_cursor);
	if (copy) {
	    compound = copy_compound(c);
	    rotate_compound(compound, px, py, rotate_angle);
	    insert_compound(&objects.compounds, compound);
	    clean_up();
	    set_action_object(F_CREATE, O_COMPOUND);
	    draw_compound(compound);
	    draw_compoundbox(compound, INV_PAINT);
	    }
	else {
	    draw_compoundbox(c, INV_PAINT);
	    erase_compound(c);
	    rotate_compound(c, px, py, rotate_angle);
	    clean_up();
	    set_action_object(F_ROTATE, O_COMPOUND);
	    set_lastangle(rotate_angle);
	    set_lastposition(px, py);
	    draw_compound(c);
	    draw_compoundbox(c, INV_PAINT);
	    compound = c;
	    }
	set_latestcompound(compound);
	set_temp_cursor(cur_cursor);
	}

rotate_line(l, x, y, rotate_angle)
F_line	*l;
int	x, y, rotate_angle;
{
	F_point	*p;
	int	dx;

	switch(rotate_angle) {
	    case 270 :
		for (p = l->points; p != NULL; p = p->next) {
		    dx = p->x - x;
		    p->x = x + (y - p->y);
		    p->y = y + dx;
		    }
		break;
	    case 90 :
		for (p = l->points; p != NULL; p = p->next) {
		    dx = p->x - x;
		    p->x = x - (y - p->y);
		    p->y = y - dx;
		    }
		break;
	    }
	}

rotate_spline(s, x, y, rotate_angle)
F_spline	*s;
int		x, y, rotate_angle;
{
	F_point		*p;
	F_control	*cp;
	int		dx;

	switch(rotate_angle) {
	    case 270 :
		for (p = s->points; p != NULL; p = p->next) {
		    dx = p->x - x;
		    p->x = x + (y - p->y);
		    p->y = y + dx;
		    }
		for (cp = s->controls; cp != NULL; cp = cp->next) {
		    dx = cp->lx - x;
		    cp->lx = x + (y - cp->ly);
		    cp->ly = y + dx;
		    dx = cp->rx - x;
		    cp->rx = x + (y - cp->ry);
		    cp->ry = y + dx;
		    }
		break;
	    case 90 :
		for (p = s->points; p != NULL; p = p->next) {
		    dx = p->x - x;
		    p->x = x - (y - p->y);
		    p->y = y - dx;
		    }
		for (cp = s->controls; cp != NULL; cp = cp->next) {
		    dx = cp->lx - x;
		    cp->lx = x - (y - cp->ly);
		    cp->ly = y - dx;
		    dx = cp->rx - x;
		    cp->rx = x - (y - cp->ry);
		    cp->ry = y - dx;
		    }
		break;
	    }
	}

rotate_text(t, x, y, rotate_angle)
F_text	*t;
int	x, y, rotate_angle;
{
	int	dx;

	switch(rotate_angle) {
	    case 270 :
		dx = t->base_x - x;
		t->base_x = x - (t->base_y - y);
		t->base_y = y + dx;
		break;
	    case 90 :	
		dx = t->base_x - x;
		t->base_x = x + (t->base_y - y);
		t->base_y = y - dx;
		break;
	    }
	}

rotate_ellipse(e, x, y, rotate_angle)
F_ellipse	*e;
int		x, y, rotate_angle;
{
	int	dx;

	switch(rotate_angle) {
	    case 270 :
		dx = e->center.x - x;
		e->center.x = x - (e->center.y - y);
		e->center.y = y + dx;
		dx = e->radiuses.x;
		e->radiuses.x = e->radiuses.y;
		e->radiuses.y = dx;
		dx = e->start.x - x;
		e->start.x = x - (e->start.y - y);
		e->start.y = y + dx;
		dx = e->end.x - x;
		e->end.x = x - (e->end.y - y);
		e->end.y = y + dx;
		break;
	    case 90 :	
		dx = e->center.x - x;
		e->center.x = x + (e->center.y - y);
		e->center.y = y - dx;
		dx = e->radiuses.x;
		e->radiuses.x = e->radiuses.y;
		e->radiuses.y = dx;
		dx = e->start.x - x;
		e->start.x = x + (e->start.y - y);
		e->start.y = y - dx;
		dx = e->end.x - x;
		e->end.x = x + (e->end.y - y);
		e->end.y = y - dx;
		break;
	    }
	}

rotate_arc(a, x, y, rotate_angle)
F_arc	*a;
int	x, y, rotate_angle;
{
	int	dx;

	switch(rotate_angle) {
	    case 270 :
		dx = a->center.x - x;
		a->center.x = x - (a->center.y - y);
		a->center.y = y + dx;
		dx = a->point[0].x - x;
		a->point[0].x = x - (a->point[0].y - y);
		a->point[0].y = y + dx;
		dx = a->point[1].x - x;
		a->point[1].x = x - (a->point[1].y - y);
		a->point[1].y = y + dx;
		dx = a->point[2].x - x;
		a->point[2].x = x - (a->point[2].y - y);
		a->point[2].y = y + dx;
		break;
	    case 90 :	
		dx = a->center.x - x;
		a->center.x = x + (a->center.y - y);
		a->center.y = y - dx;
		dx = a->point[0].x - x;
		a->point[0].x = x + (a->point[0].y - y);
		a->point[0].y = y - dx;
		dx = a->point[1].x - x;
		a->point[1].x = x + (a->point[1].y - y);
		a->point[1].y = y - dx;
		dx = a->point[2].x - x;
		a->point[2].x = x + (a->point[2].y - y);
		a->point[2].y = y - dx;
		break;
	    }
	}

#define			min(a, b)	(((a) < (b)) ? (a) : (b))
#define			max(a, b)	(((a) > (b)) ? (a) : (b))

rotate_compound(c, x, y, rotate_angle)
F_compound	*c;
int		x, y, rotate_angle;
{
	F_line		*l;
	F_arc		*a;
	F_ellipse	*e;
	F_spline	*s;
	F_text		*t;
	F_compound	*c1;
	int		x1, y1, x2, y2;

	switch(rotate_angle) {
	    case 270 :
		x1 = x - (c->nwcorner.y - y);
		y1 = y + (c->nwcorner.x - x);
		x2 = x - (c->secorner.y - y);
		y2 = y + (c->secorner.x - x);
		break;
	    case 90 :	
		x1 = x + (c->nwcorner.y - y);
		y1 = y - (c->nwcorner.x - x);
		x2 = x + (c->secorner.y - y);
		y2 = y - (c->secorner.x - x);
		break;
	    }
	c->nwcorner.x = min(x1, x2);
	c->nwcorner.y = min(y1, y2);
	c->secorner.x = max(x1, x2);
	c->secorner.y = max(y1, y2);
	for (l = c->lines; l != NULL; l = l->next)
	    rotate_line(l, x, y, rotate_angle);
	for (a = c->arcs; a != NULL; a = a->next)
	    rotate_arc(a, x, y, rotate_angle);
	for (e = c->ellipses; e != NULL; e = e->next)
	    rotate_ellipse(e, x, y, rotate_angle);
	for (s = c->splines; s != NULL; s = s->next)
	    rotate_spline(s, x, y, rotate_angle);
	for (t = c->texts; t != NULL; t = t->next)
	    rotate_text(t, x, y, rotate_angle);
	for (c1 = c->compounds; c1 != NULL; c1 = c1->next)
	    rotate_compound(c1, x, y, rotate_angle);
	}
