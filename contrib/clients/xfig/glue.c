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

#define			TOLERANCE	7
#define			min(a, b)	(((a) < (b)) ? (a) : (b))
#define			max(a, b)	(((a) > (b)) ? (a) : (b))

extern F_pos		last_position, new_position;  /* undo.c   */
extern int		fix_x, fix_y, cur_x, cur_y;

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

extern F_compound	objects;
extern int		compoundbox_shown;
extern int		pointmarker_shown;
extern int		foreground_color, background_color;

			create_compound();
F_compound		*compound_point_search();

			init_create_compound();

compound_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_create_compound;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&arrow_cursor);
	reset_action_on();
	}

init_create_compound(x, y)
int	x, y;
{
	init_box_drawing(x, y);
	canvas_middlebut_proc = create_compound;
	canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
	}

create_compound(x, y)
int	x, y;
{
	F_compound	*c;

	if (NULL == (Compound_malloc(c))) {
	    put_msg(Err_mem);
	    return;
	    }
	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	c->nwcorner.x = min(fix_x, x);
	c->nwcorner.y = min(fix_y, y);
	c->secorner.x = max(fix_x, x);
	c->secorner.y = max(fix_y, y);
	if (compose_compound(c) == 0) {
	    free((char*)c);
	    compound_selected();
	    put_msg("Empty compound, ignore");
	    return;
	    }
	draw_compoundbox(c, INV_PAINT);
	c->next = NULL;
	clean_up();
	set_action(F_GLUE);
	insert_compound(&objects.compounds, c);
	set_latestcompound(c);
	compound_selected();
	}

compose_compound(c)
F_compound	*c;
{
	c->ellipses = NULL;
	c->lines = NULL;
	c->texts = NULL;
	c->splines = NULL;
	c->arcs = NULL;
	c->compounds = NULL;
	get_ellipse(&c->ellipses, c->nwcorner.x, c->nwcorner.y,
		c->secorner.x, c->secorner.y);
	get_lineobj(&c->lines, c->nwcorner.x, c->nwcorner.y,
		c->secorner.x, c->secorner.y);
	get_spline(&c->splines, c->nwcorner.x, c->nwcorner.y,
		c->secorner.x, c->secorner.y);
	get_text(&c->texts, c->nwcorner.x, c->nwcorner.y,
		c->secorner.x, c->secorner.y);
	get_arc(&c->arcs, c->nwcorner.x, c->nwcorner.y,
		c->secorner.x, c->secorner.y);
	get_compound(&c->compounds, c->nwcorner.x, c->nwcorner.y,
		     c->secorner.x, c->secorner.y);
	/*  get rid of point-marker  */
	if (pointmarker_shown) toggle_compoundpointmarker(c);
	if (c->ellipses != NULL) return(1);
	if (c->splines != NULL) return(1);
	if (c->lines != NULL) return(1);
	if (c->texts != NULL) return(1);
	if (c->arcs != NULL) return(1);
	if (c->compounds != NULL) return(1);
	return(0);
	}

draw_compoundbox(c, op)
F_compound	*c;
int		op;
{
	draw_rectbox( c->nwcorner.x-1, c->nwcorner.y-1,
		c->secorner.x+1, c->secorner.y+1, op);
	draw_rectbox( c->nwcorner.x, c->nwcorner.y,
		c->secorner.x, c->secorner.y, op);
	draw_rectbox( c->nwcorner.x+1, c->nwcorner.y+1,
		c->secorner.x-1, c->secorner.y-1, op);
	}

get_ellipse(list, xmin, ymin, xmax, ymax)
F_ellipse	**list;
int		xmin, ymin, xmax, ymax;
{
	F_ellipse	*e, *ee, *ellipse;

	for (e = objects.ellipses; e != NULL;) {
	    if (xmin > e->center.x - e->radiuses.x) { 
		ee = e; e = e->next; continue;
		}
	    if (xmax < e->center.x + e->radiuses.x) {
		ee = e; e = e->next; continue;
		}
	    if (ymin > e->center.y - e->radiuses.y) {
		ee = e; e = e->next; continue;
		}
	    if (ymax < e->center.y + e->radiuses.y) {
		ee = e; e = e->next; continue;
		}
	    ellipse = e;
	    if (e == objects.ellipses) 
		e = objects.ellipses = objects.ellipses->next;
	    else {
		e = ee->next = e->next;
		}
	    ellipse->next = *list;
	    *list = ellipse;
	    }
	}

get_arc(list, xmin, ymin, xmax, ymax)
F_arc	**list;
int		xmin, ymin, xmax, ymax;
{
	F_arc	*a, *arc, *aa;
	int	urx, ury, llx, lly;

	for (a = objects.arcs; a != NULL;) {
	    arc_bound(a, &llx, &lly, &urx, &ury);
	    if (xmin > llx) goto out;
	    if (xmax < urx) goto out;
	    if (ymin > lly) goto out;
	    if (ymax < ury) goto out;
	    arc = a;
	    if (a == objects.arcs) 
		a = objects.arcs = objects.arcs->next;
	    else
		a = aa->next = a->next;
	    arc->next = *list;
	    *list = arc;
	    continue;
	out:
	    aa = a; a = a->next;
	    }
	}

get_lineobj(list, xmin, ymin, xmax, ymax)
F_line	**list;
int	xmin, ymin, xmax, ymax;
{
	F_line	*line, *l, *ll;
	F_point	*p;
	int	inbound;

	for (l = objects.lines; l != NULL;) {
	    for (inbound = 1, p = l->points; p!= NULL && inbound; 
			p = p->next) {
		inbound = 0;
		if (xmin > p->x) continue;
		if (xmax < p->x) continue;
		if (ymin > p->y) continue;
		if (ymax < p->y) continue;
		inbound = 1;
		}
	    if (! inbound) {
		ll = l; l = l->next; continue;
		}
	    line = l;
	    if (l == objects.lines) 
		l = objects.lines = objects.lines->next;
	    else
		l = ll->next = l->next;
	    line->next = *list;
	    *list = line;
	    }
	}

get_spline(list, xmin, ymin, xmax, ymax)
F_spline	**list;
int		xmin, ymin, xmax, ymax;
{
	F_spline	*spline, *s, *ss;
	int		urx, ury, llx, lly;

	for (s = objects.splines; s != NULL;) {
	    spline_bound(s, &llx, &lly, &urx, &ury);
	    if (xmin > llx) goto out;
	    if (xmax < urx) goto out;
	    if (ymin > lly) goto out;
	    if (ymax < ury) goto out;
	    spline = s;
	    if (s == objects.splines) 
		s = objects.splines = objects.splines->next;
	    else
		s = ss->next = s->next;
	    spline->next = *list;
	    *list = spline;
	    continue;
	out:
	    ss = s; s = s->next;
	    }
	}

get_text(list, xmin, ymin, xmax, ymax)
F_text	**list;
int	xmin, ymin, xmax, ymax;
{
	F_text	*text, *t, *tt;
	for (t = objects.texts; t != NULL;) {
	    if (xmin > t->base_x) {
		tt = t; t = t->next; continue;
		}
	    if (xmax < t->base_x + t->length) {
		tt = t; t = t->next; continue;
		}
	    if (ymin > t->base_y - t->height) {
		tt = t; t = t->next; continue;
		}
	    if (ymax < t->base_y) {
		tt = t; t = t->next; continue;
		}
	    text = t;
	    if (t == objects.texts) 
		t = objects.texts = objects.texts->next;
	    else
		t = tt->next = t->next;
	    text->next = *list;
	    *list = text;
	    }
	}

get_compound(list, xmin, ymin, xmax, ymax)
F_compound	**list;
int	xmin, ymin, xmax, ymax;
{
	F_compound	*compd, *c, *cc;
	for (c = objects.compounds; c != NULL;)
	{
		if (xmin > c->nwcorner.x)
		{
			cc = c;
			c = c->next;
			continue;
		}
		if (xmax < c->secorner.x)
		{
			cc = c;
			c = c->next;
			continue;
		}
		if (ymin > c->nwcorner.y)
		{
			cc = c;
			c = c->next;
			continue;
		}
		if (ymax < c->secorner.y)
		{
			cc = c;
			c = c->next;
			continue;
		}
		compd = c;
		if (c == objects.compounds) 
			c = objects.compounds = objects.compounds->next;
		else
			c = cc->next = c->next;
		compd->next = *list;
		*list = compd;
	}
}

F_compound *
compound_point_search(x, y, tol, px, py)
int	x, y, tol, *px, *py;
{
	F_compound	*c;

	for (c = objects.compounds; c != NULL; c = c->next) {
	    if (abs(c->nwcorner.x - x) <= tol && 
		abs(c->nwcorner.y - y) <= tol) {
		*px = c->nwcorner.x;
		*py = c->nwcorner.y;
		return(c);
		}
	    if (abs(c->nwcorner.x - x) <= tol && 
		abs(c->secorner.y - y) <= tol) {
		*px = c->nwcorner.x;
		*py = c->secorner.y;
		return(c);
		}
	    if (abs(c->secorner.x - x) <= tol && 
		abs(c->nwcorner.y - y) <= tol) {
		*px = c->secorner.x;
		*py = c->nwcorner.y;
		return(c);
		}
	    if (abs(c->secorner.x - x) <= tol && 
		abs(c->secorner.y - y) <= tol) {
		*px = c->secorner.x;
		*py = c->secorner.y;
		return(c);
		}
	    }
	return(NULL);
	}

draw_compound(c)
F_compound	*c;
{
	draw_compoundelements(c, foreground_color, foreground_color,
				PAINT, PAINT, PAINT, PAINT);
	}

erase_compound(c)
F_compound	*c;
{
	draw_compoundelements(c, background_color, background_color,
				ERASE, ERASE, INV_PAINT, ERASE);
	}

draw_compoundelements(c, arcop, ellipseop, lineop, splineop, textop, compop)
F_compound	*c;
int		arcop, ellipseop, lineop, splineop, textop, compop;
{
	F_line		*l;
	F_spline	*s;
	F_ellipse	*e;
	F_text		*t;
	F_arc		*a;
	F_compound	*c1;
	
	pw_batch_on(canvas_pixwin);
	for (l = c->lines; l != NULL; l = l->next) {
	    draw_line(l, lineop);
	    }
	for (s = c->splines; s != NULL; s = s->next) {
	    draw_spline(s, splineop);
	   } 
	for (a = c->arcs; a != NULL; a = a->next) {
	    draw_arc(a, arcop);
	   } 
	for (e = c->ellipses; e != NULL; e = e->next) {
	    draw_ellipse(e, ellipseop);
	    }
	for (t = c->texts; t != NULL; t = t->next) {
	    draw_text(t, textop);
	    }
	for (c1 = c->compounds; c1 != NULL; c1 = c1->next) {
	    draw_compoundbox(c1, INV_PAINT);
	    compop == ERASE ? erase_compound(c1) : draw_compound(c1);
	    }
	pw_batch_off(canvas_pixwin);
	}

F_compound *
compound_search(x, y, tolerance, px, py)
int	x, y, tolerance, *px, *py;
{
	F_compound	*c;
	float		tol2;

	tol2 = tolerance * tolerance;
	
	for (c = objects.compounds; c != NULL; c = c->next) {
	    if (close_to_vector(c->nwcorner.x, c->nwcorner.y, c->nwcorner.x,
			c->secorner.y, x, y, tolerance, tol2, px, py)) 
		return(c);
	    if (close_to_vector(c->secorner.x, c->secorner.y, c->nwcorner.x,
			c->secorner.y, x, y, tolerance, tol2, px, py)) 
		return(c);
	    if (close_to_vector(c->secorner.x, c->secorner.y, c->secorner.x,
			c->nwcorner.y, x, y, tolerance, tol2, px, py)) 
		return(c);
	    if (close_to_vector(c->nwcorner.x, c->nwcorner.y, c->secorner.x,
			c->nwcorner.y, x, y, tolerance, tol2, px, py)) 
		return(c);
	    }
	return(NULL);
	}

toggle_compoundpointmarker(c)
F_compound	*c;
{
	F_line		*l;
	F_spline	*s;
	F_ellipse	*e;
	F_arc		*a;

	for (l = c->lines; l != NULL; l = l->next) {
	    toggle_linepointmarker(l);
	    }
	for (s = c->splines; s != NULL; s = s->next) {
	    toggle_splinepointmarker(s);
	   } 
	for (a = c->arcs; a != NULL; a = a->next) {
	    toggle_arcpointmarker(a);
	   } 
	for (e = c->ellipses; e != NULL; e = e->next) {
	    toggle_ellipsepointmarker(e);
	    }
	}

show_compoundbox()
{
	F_compound	*c;

	if (compoundbox_shown) return;
	compoundbox_shown = 1;
	for (c = objects.compounds; c != NULL; c = c->next) 
	    draw_compoundbox(c, INV_PAINT);
	}

erase_compoundbox()
{
	F_compound	*c;

	if (! compoundbox_shown) return;
	compoundbox_shown = 0;
	for (c = objects.compounds; c != NULL; c = c->next) 
	    draw_compoundbox(c, INV_PAINT);
	}



