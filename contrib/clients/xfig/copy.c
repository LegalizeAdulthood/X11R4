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

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			(*return_proc)();
extern			null_proc();
extern			set_popupmenu();
extern F_line		*line_search(), *copy_line();
extern F_arc		*arc_search(), *copy_arc();
extern F_ellipse	*ellipse_search(), *copy_ellipse();
extern F_text		*text_search(), *copy_text();
extern F_spline		*spline_search(), *copy_spline();
extern F_compound	*compound_search(), *copy_compound();

extern F_compound	objects;

extern int		copy_selected();
			init_copy();

copy_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_copy;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	return_proc = copy_selected;
	set_cursor(&pick15_cursor);
	reset_action_on();
	}

init_copy(x, y)
int	x, y;
{
	F_line		*l, *line;
	F_ellipse	*e, *ellipse;
	F_text		*t, *text;
	F_spline	*s, *spline;
	F_arc		*a, *arc;
	F_compound	*c, *compound;
	int		px, py;

	if ((c = compound_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    compound = copy_compound(c);
	    erase_pointmarker();
	    set_temp_cursor(&null_cursor);
	    win_setmouseposition(canvas_swfd, px, py);
	    clean_up();
	    set_action_object(F_CREATE, O_COMPOUND);
	    insert_compound(&objects.compounds, compound);
	    set_latestcompound(compound);
	    init_compounddragging(compound, px, py);
	    }
	else if ((l = line_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    line = copy_line(l);
	    erase_pointmarker();
	    set_temp_cursor(&null_cursor);
	    win_setmouseposition(canvas_swfd, px, py);
	    clean_up();
	    set_action_object(F_CREATE, O_POLYLINE);
	    insert_line(&objects.lines, line);
	    set_latestline(line);
	    init_linedragging(line, px, py);
	    }
	else if ((t = text_search(x, y)) != NULL) {
	    text = copy_text(t);
	    erase_pointmarker();
	    set_temp_cursor(&null_cursor);
	    clean_up();
	    set_action_object(F_CREATE, O_TEXT);
	    insert_text(&objects.texts, text);
	    set_latesttext(text);
	    init_textdragging(text, x, y);
	    }
	else if ((e = ellipse_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    ellipse = copy_ellipse(e);
	    erase_pointmarker();
	    set_temp_cursor(&null_cursor);
	    win_setmouseposition(canvas_swfd, px, py);
	    clean_up();
	    set_action_object(F_CREATE, O_ELLIPSE);
	    insert_ellipse(&objects.ellipses, ellipse);
	    set_latestellipse(ellipse);
	    init_ellipsedragging(ellipse, px, py);
	    }
	else if ((a = arc_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    arc = copy_arc(a);
	    erase_pointmarker();
	    set_temp_cursor(&null_cursor);
	    win_setmouseposition(canvas_swfd, px, py);
	    clean_up();
	    set_action_object(F_CREATE, O_ARC);
	    insert_arc(&objects.arcs, arc);
	    set_latestarc(arc);
	    init_arcdragging(arc, px, py);
	    }
	else if ((s = spline_search(x, y, TOLERANCE, &px, &py)) != NULL) {
	    spline = copy_spline(s);
	    erase_pointmarker();
	    set_temp_cursor(&null_cursor);
	    win_setmouseposition(canvas_swfd, px, py);
	    clean_up();
	    set_action_object(F_CREATE, O_SPLINE);
	    insert_spline(&objects.splines, spline);
	    set_latestspline(spline);
	    init_splinedragging(spline, px, py);
	    }
	else
	    return;
	canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
	}

F_arc *
copy_arc(a)
F_arc	*a;
{
	F_arc	*arc;

	if (NULL == (Arc_malloc(arc))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	*arc = *a;
	arc->next = NULL;
	return(arc);
	}

F_ellipse *
copy_ellipse(e)
F_ellipse	*e;
{
	F_ellipse	*ellipse;

	if (NULL == (Ellipse_malloc(ellipse))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	*ellipse = *e;
	ellipse->next = NULL;
	return(ellipse);
	}

F_line *
copy_line(l)
F_line	*l;
{
	F_line	*line;
	F_point	*p, *point, *last_point;

	if (NULL == (Line_malloc(line))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	*line = *l;
	line->points = Point_malloc(point);
	last_point = point;
	p = l->points;
	*point = *p;
	point->next = NULL;
	for (p = p->next; p != NULL; p = p->next) {
	    last_point->next = Point_malloc(point);
	    if (point == NULL) return(NULL);
	    *point = *p;
	    point->next = NULL;
	    last_point = point;
	    }
	line->next = NULL;
	return(line);
	}

F_spline *
copy_spline(s)
F_spline	*s;
{
	F_spline	*spline;
	F_point		*p, *point, *last_point;
	F_control	*cntrl_pnt, *cp, *last_cntrl_pnt;

	if (NULL == (Spline_malloc(spline))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	*spline = *s;
	spline->next = NULL;

	Point_malloc(point);
	last_point = spline->points = point;
	p = s->points;
	*point = *p;
	for (p = p->next; p != NULL; p = p->next) {
	    last_point->next = Point_malloc(point);
	    if (point == NULL) return(NULL);
	    *point = *p;
	    last_point = point;
	    }
	last_point->next = NULL;

	spline->controls = NULL;
	if (s->controls == NULL) return(spline);

	last_cntrl_pnt = spline->controls = Control_malloc(cntrl_pnt);
	cp = s->controls;
	*cntrl_pnt = *cp;
	for (cp = cp->next; cp != NULL; cp = cp->next) {
	    last_cntrl_pnt->next = Control_malloc(cntrl_pnt);
	    if (cntrl_pnt == NULL) return(NULL);
	    *cntrl_pnt = *cp;
	    last_cntrl_pnt = cntrl_pnt;
	    }
	last_cntrl_pnt->next = NULL;

	return(spline);
	}

F_text *
copy_text(t)
F_text	*t;
{
	F_text		*text;
	extern char	*calloc();

	if (NULL == (Text_malloc(text))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	*text = *t;
	text->cstring = calloc((unsigned)(strlen(t->cstring)+1), sizeof(char));
	if (text->cstring == NULL) {
	    free((char*)text);
	    put_msg(Err_mem);
	    return(NULL);
	    }
	strcpy(text->cstring, t->cstring);
	text->next = NULL;
	return(text);
	}

F_compound *
copy_compound(c)
F_compound	*c;
{
	F_ellipse	*e, *ee;
	F_arc		*a, *aa;
	F_line		*l, *ll;
	F_spline	*s, *ss;
	F_text		*t, *tt;
	F_compound	*cc, *ccc, *compound;

	if (NULL == (Compound_malloc(compound))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	compound->nwcorner = c->nwcorner;
	compound->secorner = c->secorner;
	compound->arcs = NULL;
	compound->ellipses = NULL;
	compound->lines = NULL;
	compound->splines = NULL;
	compound->texts = NULL;
	compound->compounds = NULL;
	compound->next = NULL;
	for (e = c->ellipses; e != NULL; e = e->next) {
	    if (NULL == (ee = copy_ellipse(e))) {
		put_msg(Err_mem);
		return(NULL);
		}
	    insert_ellipse(&compound->ellipses, ee);
	    }
	for (a = c->arcs; a != NULL; a = a->next) {
	    if (NULL == (aa = copy_arc(a))) {
		put_msg(Err_mem);
		return(NULL);
		}
	    insert_arc(&compound->arcs, aa);
	    }
	for (l = c->lines; l != NULL; l = l->next) {
	    if (NULL == (ll = copy_line(l))) {
		put_msg(Err_mem);
		return(NULL);
		}
	    insert_line(&compound->lines, ll);
	    }
	for (s = c->splines; s != NULL; s = s->next) {
	    if (NULL == (ss = copy_spline(s))) {
		put_msg(Err_mem);
		return(NULL);
		}
	    insert_spline(&compound->splines, ss);
	    }
	for (t = c->texts; t != NULL; t = t->next) {
	    if (NULL == (tt = copy_text(t))) {
		put_msg(Err_mem);
		return(NULL);
		}
	    insert_text(&compound->texts, tt);
	    }
	for (cc = c->compounds; cc != NULL; cc = cc->next) {
	    if (NULL == (ccc = copy_compound(cc))) {
		put_msg(Err_mem);
		return(NULL);
		}
	    insert_compound(&compound->compounds, ccc);
	    }
	return(compound);
	}
