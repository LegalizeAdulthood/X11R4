/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	Febuary 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "object.h"

delete_arc(arc_list, arc)
F_arc	**arc_list, *arc;
{
	F_arc	*a, *aa;

	if (*arc_list == NULL) return;
	if (arc == NULL) return;

	for (a = aa = *arc_list; aa != NULL; a = aa, aa = aa->next) {
	    if (aa == arc) {
		if (aa == *arc_list)
		    *arc_list = (*arc_list)->next;
		else
		    a->next = aa->next;
		break;
		}
	    }
	arc->next = NULL;
	}

delete_ellipse(ellipse_list, ellipse)
F_ellipse	**ellipse_list, *ellipse;
{
	F_ellipse	*q, *r;

	if (*ellipse_list == NULL) return;
	if (ellipse == NULL) return;

	for (q = r = *ellipse_list; r != NULL; q = r, r = r->next) {
	    if (r == ellipse) {
		if (r == *ellipse_list)
		    *ellipse_list = (*ellipse_list)->next;
		else
		    q->next = r->next;
		break;
		}
	    }
	ellipse->next = NULL;
	}

delete_line(line_list, line)
F_line	*line, **line_list;
{
	F_line	*q, *r;

	if (*line_list == NULL) return;
	if (line == NULL) return;

	for (q = r = *line_list; r != NULL; q = r, r = r->next) {
	    if (r == line) {
		if (r == *line_list)
		    *line_list = (*line_list)->next;
		else
		    q->next = r->next;
		break;
		}
	    }
	line->next = NULL;
	}

delete_spline(spline_list, spline)
F_spline	**spline_list, *spline;
{
	F_spline	*q, *r;

	if (*spline_list == NULL) return;
	if (spline == NULL) return;

	for (q = r = *spline_list; r != NULL; q = r, r = r->next) {
	    if (r == spline) {
		if (r == *spline_list)
		    *spline_list = (*spline_list)->next;
		else
		    q->next = r->next;
		break;
		}
	    }
	spline->next = NULL;
	}

delete_text(text_list, text)
F_text	**text_list, *text;
{
	F_text	*q, *r;

	if (*text_list == NULL) return;
	if (text == NULL) return;

	for (q = r = *text_list; r != NULL; q = r, r = r->next) 
	    if (r == text) {
		if (r == *text_list)
		    *text_list = text->next;
		else
		    q->next = text->next;
		break;
		}
	text->next = NULL;
	}

delete_compound(list, compound)
F_compound	**list, *compound;
{
	F_compound	*c, *cc;

	if (*list == NULL) return;
	if (compound == NULL) return;

	for (cc = c = *list; c != NULL; cc = c, c = c->next) {
	    if (c == compound) {
		if (c == *list)
		    *list = (*list)->next;
		else
		    cc->next = c->next;
		break;
		}
	    }
	compound->next = NULL;
	}

insert_arc(arc_list, a)
F_arc	**arc_list, *a;
{
	a->next = *arc_list;
	*arc_list = a;
	}

insert_ellipse(ellipse_list, e)
F_ellipse	**ellipse_list, *e;
{
	e->next = *ellipse_list;
	*ellipse_list = e;
	}

insert_line(line_list, l)
F_line	**line_list, *l;
{
	l->next = *line_list;
	*line_list = l;
	}

insert_spline(spline_list, s)
F_spline	**spline_list, *s;
{
	s->next = *spline_list;
	*spline_list = s;
	}

insert_text(text_list, t)
F_text	**text_list, *t;
{
	t->next = *text_list;
	*text_list = t;
	}


insert_compound(list, c)
F_compound	**list, *c;
{
	c->next = *list;
	*list = c;
	}
