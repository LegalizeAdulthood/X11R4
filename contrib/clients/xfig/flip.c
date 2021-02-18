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
#define			min(a, b)	(((a) < (b)) ? (a) : (b))
#define			max(a, b)	(((a) > (b)) ? (a) : (b))

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
extern F_text		*copy_text();
extern F_spline		*spline_search(), *copy_spline();
extern F_arc		*arc_point_search(), *copy_arc();
extern F_compound	*compound_search(), *copy_compound();

extern F_compound	objects;

extern int		flip_axis;
extern int		pointmarker_shown;
extern int		foreground_color, background_color;

static int		copy;
			init_flip();
			init_copynflip();

flip_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_copynflip;
	canvas_middlebut_proc = init_flip;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick15_cursor);
	}

init_flip(x, y)
int	x, y;
{
	copy = 0;
	flip_search(x, y);
	flip_selected();
	}

init_copynflip(x, y)
int	x, y;
{
	copy = 1;
	flip_search(x, y);
	flip_selected();
	}

flip_search(x, y)
int	x, y;
{
	F_line		*l;
	F_arc		*a;
	F_ellipse	*e;
	F_spline	*s;
	F_compound	*c;
	int		px, py;

	if ((l = line_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    init_flipline(l, px, py);
	    }
	else if ((a = arc_point_search(x, y, TOLERANCE, &px)) != NULL) {
	    init_fliparc(a, a->point[px].x, a->point[px].y);
	    }
	else if ((e = ellipse_point_search(x, y, TOLERANCE, &px)) != NULL) {
	    if (px == 0) /* start point */
		init_flipellipse(e, e->start.x, e->start.y);
	    else
		init_flipellipse(e, e->end.x, e->end.y);
	    }
	else if ((e = ellipse_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    init_flipellipse(e, px, py);
	    }
	else if ((s = spline_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    init_flipspline(s, px, py);
	    }
	else if ((c = compound_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    init_flipcompound(c, px, py);
	    }
	set_modifiedflag();
	}

init_fliparc(a, px, py)
F_arc	*a;
int	px, py;
{
	F_arc	*arc;

	win_setmouseposition(canvas_swfd, px, py);
	if (pointmarker_shown) toggle_arcpointmarker(a);
	if (copy) {
	    arc = copy_arc(a);
	    flip_arc(arc, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_CREATE, O_ARC);
	    insert_arc(&objects.arcs, arc);
	    }
	else {
	    draw_arc(a, background_color);
	    flip_arc(a, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_FLIP, O_ARC);
	    set_lastaxis(flip_axis);
	    set_lastposition(px, py);
	    arc = a;
	    }
	draw_arc(arc, foreground_color);
	if (pointmarker_shown) {
	    toggle_arcpointmarker(arc);
	    if (copy) toggle_arcpointmarker(a);
	    }
	set_latestarc(arc);
	}

init_flipcompound(c, px, py)
F_compound	*c;
int		px, py;
{
	F_compound	*compound;

	win_setmouseposition(canvas_swfd, px, py);
	set_temp_cursor(&wait_cursor);
	if (copy) {
	    compound = copy_compound(c);
	    flip_compound(compound, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_CREATE, O_COMPOUND);
	    insert_compound(&objects.compounds, compound);
	    }
	else {
	    draw_compoundbox(c, INV_PAINT);
	    erase_compound(c);
	    flip_compound(c, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_FLIP, O_COMPOUND);
	    set_lastaxis(flip_axis);
	    set_lastposition(px, py);
	    compound = c;
	    }
	draw_compoundbox(compound, INV_PAINT);
	draw_compound(compound);
	set_latestcompound(compound);
	set_temp_cursor(cur_cursor);
	}

init_flipellipse(e, px, py)
F_ellipse	*e;
{
	F_ellipse	*ellipse;

	win_setmouseposition(canvas_swfd, px, py);
	if (pointmarker_shown) toggle_ellipsepointmarker(e);
	if (copy) {
	    ellipse = copy_ellipse(e);
	    flip_ellipse(ellipse, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_CREATE, O_ELLIPSE);
	    insert_ellipse(&objects.ellipses, ellipse);
	    }
	else {
	    draw_ellipse(e, background_color);
	    flip_ellipse(e, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_FLIP, O_ELLIPSE);
	    set_lastaxis(flip_axis);
	    set_lastposition(px, py);
	    ellipse = e;
	    }
	draw_ellipse(ellipse, foreground_color);
	if (pointmarker_shown) {
	    toggle_ellipsepointmarker(ellipse);
	    if (copy) toggle_ellipsepointmarker(e);
	    }
	set_latestellipse(ellipse);
	}

init_flipline(l, px, py)
F_line	*l;
int	px, py;
{
	F_line	*line;

	win_setmouseposition(canvas_swfd, px, py);
	if (pointmarker_shown) toggle_linepointmarker(l);
	if (copy) {
	    line = copy_line(l);
	    flip_line(line, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_CREATE, O_POLYLINE);
	    insert_line(&objects.lines, line);
	    }
	else {
	    draw_line(l, ERASE);
	    flip_line(l, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_FLIP, O_POLYLINE);
	    set_lastaxis(flip_axis);
	    set_lastposition(px, py);
	    line = l;
	    }
	draw_line(line, PAINT);
	if (pointmarker_shown) {
	    toggle_linepointmarker(line);
	    if (copy) toggle_linepointmarker(l);
	    }
	set_latestline(line);
	}

init_flipspline(s, px, py)
F_spline	*s;
int		px, py;
{
	F_spline	*spline;

	win_setmouseposition(canvas_swfd, px, py);
	if (pointmarker_shown) toggle_splinepointmarker(s);
	if (copy) {
	    spline = copy_spline(s);
	    flip_spline(spline, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_CREATE, O_SPLINE);
	    insert_spline(&objects.splines, spline);
	    }
	else {  /*  delete the original */
	    draw_spline(s, ERASE);
	    flip_spline(s, px, py, flip_axis);
	    clean_up();
	    set_action_object(F_FLIP, O_SPLINE);
	    set_lastaxis(flip_axis);
	    set_lastposition(px, py);
	    spline = s;
	    }
	draw_spline(spline, PAINT);
	if (pointmarker_shown) {
	    toggle_splinepointmarker(spline);
	    if (copy) toggle_splinepointmarker(s);
	    }
	set_latestspline(spline);
	}

flip_line(l, x, y, flip_axis)
F_line	*l;
int	x, y, flip_axis;
{
	F_point	*p;

	switch(flip_axis) {
	    case 1 :	/*  x axis  */
		for (p = l->points; p != NULL; p = p->next)
		    p->y = y + (y - p->y);
		break;
	    case 2 :	/*  y axis  */
		for (p = l->points; p != NULL; p = p->next)
		    p->x = x + (x - p->x);
		break;
	    }
	}

flip_spline(s, x, y, flip_axis)
F_spline	*s;
int		x, y, flip_axis;
{
	F_point		*p;
	F_control	*cp;

	switch(flip_axis) {
	    case 1 :	/*  x axis  */
		for (p = s->points; p != NULL; p = p->next)
		    p->y = y + (y - p->y);
		for (cp = s->controls; cp != NULL; cp = cp->next) {
		    cp->ly = y + (y - cp->ly);
		    cp->ry = y + (y - cp->ry);
		    }
		break;
	    case 2 :	/*  y axis  */
		for (p = s->points; p != NULL; p = p->next)
		    p->x = x + (x - p->x);
		for (cp = s->controls; cp != NULL; cp = cp->next) {
		    cp->lx = x + (x - cp->lx);
		    cp->rx = x + (x - cp->rx);
		    }
		break;
	    }
	}

flip_text(t, x, y, flip_axis)
F_text	*t;
int	x, y, flip_axis;
{
	switch(flip_axis) {
	    case 1 :	/*  x axis  */
		t->base_y = y + (y - t->base_y);
		break;
	    case 2 :	/*  y axis  */
		t->base_x = x + (x - t->base_x);
		break;
	    }
	}

flip_ellipse(e, x, y, flip_axis)
F_ellipse	*e;
int		x, y, flip_axis;
{
	switch(flip_axis) {
	    case 1 :	/*  x axis  */
		e->direction ^= 1;
		e->center.y = y + (y - e->center.y);
		e->start.y = y + (y - e->start.y);
		e->end.y = y + (y - e->end.y);
		break;
	    case 2 :	/*  y axis  */
		e->direction ^= 1;
		e->center.x = x + (x - e->center.x);
		e->start.x = x + (x - e->start.x);
		e->end.x = x + (x - e->end.x);
		break;
	    }
	}

flip_arc(a, x, y, flip_axis)
F_arc	*a;
int	x, y, flip_axis;
{
	switch(flip_axis) {
	    case 1 :	/*  x axis  */
		a->direction ^= 1;
		a->center.y = y + (y - a->center.y);
		a->point[0].y = y + (y - a->point[0].y);
		a->point[1].y = y + (y - a->point[1].y);
		a->point[2].y = y + (y - a->point[2].y);
		break;
	    case 2 :	/*  y axis  */
		a->direction ^= 1;
		a->center.x = x + (x - a->center.x);
		a->point[0].x = x + (x - a->point[0].x);
		a->point[1].x = x + (x - a->point[1].x);
		a->point[2].x = x + (x - a->point[2].x);
		break;
	    }
	}

flip_compound(c, x, y, flip_axis)
F_compound	*c;
int		x, y, flip_axis;
{
	F_line		*l;
	F_arc		*a;
	F_ellipse	*e;
	F_spline	*s;
	F_text		*t;
	F_compound	*c1;
	int		p, q;
	
	switch(flip_axis) {
	    case 1 :	/*  x axis  */
		p = y + (y - c->nwcorner.y);
		q = y + (y - c->secorner.y);
		c->nwcorner.y = min(p, q);
		c->secorner.y = max(p, q);
		break;
	    case 2 :	/*  y axis  */
		p = x + (x - c->nwcorner.x);
		q = x + (x - c->secorner.x);
		c->nwcorner.x = min(p, q);
		c->secorner.x = max(p, q);
		break;
	    }
	for (l = c->lines; l != NULL; l = l->next)
	    flip_line(l, x, y, flip_axis);
	for (a = c->arcs; a != NULL; a = a->next)
	    flip_arc(a, x, y, flip_axis);
	for (e = c->ellipses; e != NULL; e = e->next)
	    flip_ellipse(e, x, y, flip_axis);
	for (s = c->splines; s != NULL; s = s->next)
	    flip_spline(s, x, y, flip_axis);
	for (t = c->texts; t != NULL; t = t->next)
	    flip_text(t, x, y, flip_axis);
	for (c1 = c->compounds; c1 != NULL; c1 = c1->next)
	    flip_compound(c1, x, y, flip_axis);
	}
