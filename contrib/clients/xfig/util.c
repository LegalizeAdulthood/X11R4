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
#include "object.h"
#include "paintop.h"

extern F_compound	objects;

/***********  The following extern vars for undo purpose  **************/


extern int		pointmarker_shown;

static u_int	marker_pattern[3] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
mpr_static(pmarker, 5, 5, 1, marker_pattern);

tail(ob, tails)
F_compound	*ob, *tails;
{
	F_arc		*a;
	F_compound	*c;
	F_ellipse	*e;
	F_line		*l;
	F_spline	*s;
	F_text		*t;

	if (NULL != (a = ob->arcs)) for (; a->next != NULL; a = a->next);
	if (NULL != (c = ob->compounds)) for (; c->next != NULL; c = c->next);
	if (NULL != (e = ob->ellipses)) for (; e->next != NULL; e = e->next);
	if (NULL != (l = ob->lines)) for (; l->next != NULL; l = l->next);
	if (NULL != (s = ob->splines)) for (; s->next != NULL; s = s->next);
	if (NULL != (t = ob->texts)) for (; t->next != NULL; t = t->next);

	tails->arcs = a;
	tails->compounds = c;
	tails->ellipses = e;
	tails->lines = l;
	tails->splines = s;
	tails->texts = t;
	}

/*
Make pointers in tails point to the last element of each list of l1
and Append the lists in l2 after those in l1.
The tails pointers must be defined prior to calling append.
*/
append_objects(l1, l2, tails)
F_compound	*l1, *l2, *tails;
{
	if (tails->arcs)
	    tails->arcs->next  = l2->arcs; 
	else
	    l1->arcs  = l2->arcs; 
	if (tails->compounds)
	    tails->compounds->next  = l2->compounds;
	else
	    l1->compounds  = l2->compounds;
	if (tails->ellipses)
	    tails->ellipses->next  = l2->ellipses;
	else
	    l1->ellipses  = l2->ellipses;
	if (tails->lines)
	    tails->lines->next  = l2->lines;
	else
	    l1->lines  = l2->lines;
	if (tails->splines)
	    tails->splines->next  = l2->splines;
	else
	    l1->splines  = l2->splines;
	if (tails->texts)
	    tails->texts->next  = l2->texts;
	else
	    l1->texts  = l2->texts;
	}

/* Cut is the dual of append. */

cut_objects(objects, tails)
F_compound	*objects, *tails;
{
	if (tails->arcs)
	    tails->arcs->next = NULL;
	else
	    objects->arcs = NULL;
	if (tails->compounds)
	    tails->compounds->next = NULL;
	else
	    objects->compounds = NULL;
	if (tails->ellipses)
	    tails->ellipses->next = NULL;
	else
	    objects->ellipses = NULL;
	if (tails->lines)
	    tails->lines->next = NULL;
	else
	    objects->lines = NULL;
	if (tails->splines)
	    tails->splines->next = NULL;
	else
	    objects->splines = NULL;
	if (tails->texts)
	    tails->texts->next = NULL;
	else
	    objects->texts = NULL;
	}

no_object()
{
	if (objects.texts != NULL) return(0);
	if (objects.lines != NULL) return(0);
	if (objects.ellipses != NULL) return(0);
	if (objects.splines != NULL) return(0);
	if (objects.arcs != NULL) return(0);
	if (objects.compounds != NULL) return(0);
	return(1);
	}

show_pointmarker()
{
	if (pointmarker_shown) return;
	pointmarker_shown = 1;
	toggle_pointmarker();
	}

erase_pointmarker()
{
	if (! pointmarker_shown) return;
	pointmarker_shown = 0;
	toggle_pointmarker();
	}

toggle_pointmarker()
{
	F_ellipse	*e;
	F_arc		*a;
	F_line		*l;
	F_spline	*s;

	for (e = objects.ellipses; e != NULL; e = e->next) {
	    toggle_ellipsepointmarker(e);
	    }
	for (a = objects.arcs; a != NULL; a = a->next) {
	    toggle_arcpointmarker(a);
	    }
	for (l = objects.lines; l != NULL; l = l->next) {
	    toggle_linepointmarker(l);
	    }
	for (s = objects.splines; s != NULL; s = s->next) {
	    toggle_splinepointmarker(s);
	    }
	}

toggle_ellipsepointmarker(e)
F_ellipse	*e;
{
	set_marker(canvas_pixwin, e->start.x-2, e->start.y-2, 5, 5, INV_PAINT,
		&pmarker, 0, 0);
	set_marker(canvas_pixwin, e->end.x-2, e->end.y-2, 5, 5, INV_PAINT,
		&pmarker, 0, 0);
	}

toggle_arcpointmarker(a)
F_arc	*a;
{
	set_marker(canvas_pixwin, a->point[0].x-2, a->point[0].y-2, 5, 5,
		INV_PAINT, &pmarker, 0, 0);
	set_marker(canvas_pixwin, a->point[1].x-2, a->point[1].y-2, 5, 5,
		INV_PAINT, &pmarker, 0, 0);
	set_marker(canvas_pixwin, a->point[2].x-2, a->point[2].y-2, 5, 5,
		INV_PAINT, &pmarker, 0, 0);
	}

toggle_linepointmarker(l)
F_line	*l;
{
	F_point	*p;
	int	fx, fy, x, y;

	p = l->points;
	fx = p->x;  fy = p->y;
	for (p = p->next; p!= NULL; p = p->next) {
	    x = p->x;  y = p->y;
	    set_marker(canvas_pixwin, x-2, y-2, 5, 5, INV_PAINT, &pmarker, 0, 0);
	    }
	if (x != fx || y != fy) {
	    set_marker(canvas_pixwin, fx-2, fy-2, 5, 5, INV_PAINT, 
			&pmarker, 0, 0);
	    }
	}

toggle_splinepointmarker(s)
F_spline	*s;
{
	F_point	*p;
	int	fx, fy, x, y;

	p = s->points;
	fx = p->x;  fy = p->y;
	for (p = p->next; p!= NULL; p = p->next) {
	    x = p->x;  y = p->y;
	    set_marker(canvas_pixwin, x-2, y-2, 5, 5, INV_PAINT, &pmarker, 0, 0);
	    }
	if (x != fx || y != fy) {
	    set_marker(canvas_pixwin, fx-2, fy-2, 5, 5, INV_PAINT, 
			&pmarker, 0, 0);
	    }
	}

num_points(points)
F_point	*points;
{
	int	n;
	F_point	*p;

	for (p = points, n = 0; p != NULL; p = p->next, n++);
	return(n);
	}
