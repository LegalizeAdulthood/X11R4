/* 
 *	F2p : Fig-to-pic translator
 *
 *	Copyright (c) 1985, 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : October 1985.
 *	2nd revision : March 1988 - read fig 1.4
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "object.h"

char		Usage[] = "Usage: f2p [ -s font_size ] [ input_file [ output_file ] ]\n";
char		*from = NULL, *to = NULL;
FILE		*ffp = NULL, *tfp = NULL;
int		font_size = 11;		/* points */
char		Err_incomp[] = "Incomplete %s object at line %d.";
char		Err_mem[] = "Running out of memory.";

put_msg(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
char   *format, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8;
{
	fprintf(stderr, format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}

get_args(argc, argv)
int	 argc;
char	*argv[];
{
	char	*a;
	int	first = 1;

	while (--argc) {
	    a = *++argv;
	    if (*a == '-') {
		if (*++a == 's') {	/* Font size followed */
		    if (--argc) {
			font_size = atoi(*++argv);
			if (font_size <= 0 || font_size > 30) {
			    fprintf(stderr, Usage);
			    exit(0);
			    }
			}
		    else {
			fprintf(stderr, Usage);
			exit(0);
			}
		    }
		else {
		    fprintf(stderr, Usage);
		    exit(0);
		    }
		}
	    else if (first) {
		from = a;	/*  from file  */
		first = 0;
		}
	    else if (first == 0) {
		to = a;		/*  to file  */
		first = -1;
		}
	    else
		fprintf(stderr, Usage);
	    }
	}

main(argc, argv)
int	 argc;
char	*argv[];
{
	F_compound	objects;
	int		status;
	char		c;

	get_args(argc, argv);
	if (to == NULL)
	    tfp = stdout;
	else if ((tfp = fopen(to, "w")) == NULL) {
	    fprintf(stderr, "Couldn't open %s\n", to);
	    fprintf(stderr, Usage);
	    exit(0);
	    }

	if (from)
	    status = read_fig(from, &objects);
	else 	/* read from stdin */
	    status = readfp_fig(stdin, &objects);

	if (status != 0) {
	    if (from) read_fail_message(from, status);
	    exit(0);
	    }
	genpic_objects(&objects);
	if (tfp != stdout) fclose(tfp);
	}

#define			TOP	10.5	/* top of page is 10.5 inch */
static double		ppi;
static int		CONV = 0;

double
convy(a)
double	a;
{
	return((double)(CONV ? TOP-a : a));
	}

genpic_objects(objects)
F_compound	*objects;
{
	int		coord_system;
	F_arc		*a;
	F_compound	*c;
	F_ellipse	*e;
	F_line		*l;
	F_spline	*s;
	F_text		*t;

	if (0 == (ppi = (double)objects->nwcorner.x)) {
	    fprintf(stderr, "Resolution is zero!! default to 80 ppi\n");
	    ppi = 80.0;
	    }
	coord_system = objects->nwcorner.y;
	if (coord_system != 1 && coord_system != 2) {
	    fprintf(stderr, "Wrong coordinate system; cannot continue\n");
	    return;
	    }
	if (coord_system == 2) CONV = 1;

	fprintf(tfp, ".PS\n");	/* PostScript magic strings */

	for (a = objects->arcs; a != NULL; a = a->next) genpic_arc(a);
	for (c = objects->compounds; c != NULL; c = c->next) genpic_compound(c);
	for (e = objects->ellipses; e != NULL; e = e->next) genpic_ellipse(e);
	for (l = objects->lines; l != NULL; l = l->next) genpic_line(l);
	for (s = objects->splines; s != NULL; s = s->next) genpic_spline(s);
	for (t = objects->texts; t != NULL; t = t->next) genpic_text(t);
	fprintf(tfp, ".PE\n");
	}

/*
The line thickness is, unfortunately, multiple of pixel.
One pixel thickness is a little too thick on the hard copy
so I scale it with 0.7; i.e., it's a kludge.  The best way is
to allow thickness in fraction of pixel.

Note that the current version of psdit (a ditroff to postcript filter)
won't take the legitimate line thickness command.
*/
set_linewidth(w)
int	w;
{
	static int	cur_thickness = -1;

	/*
	if (w == 0) return;
	if (w != cur_thickness) {
	    cur_thickness = w;
	    fprintf(tfp, "\"\\D't %.5fi'\"\n", 0.7 * cur_thickness / ppi);
	    }
	*/
	}

set_style(s, v)
int	s;
float	v;
{
	static float	style_val = -1;

	if (s == DASH_LINE || s == DOTTED_LINE) {
	    if (v == style_val) return;
	    if (v == 0.0) return;
	    style_val = v;
	    fprintf(tfp, "dashwid = %.3fi\n", style_val/ppi);
	    }
	}

genpic_compound(com)
F_compound	*com;
{
	F_arc		*a;
	F_compound	*c;
	F_ellipse	*e;
	F_line		*l;
	F_spline	*s;
	F_text		*t;

	for (a = com->arcs; a != NULL; a = a->next) genpic_arc(a);
	for (c = com->compounds; c != NULL; c = c->next) genpic_compound(c);
	for (e = com->ellipses; e != NULL; e = e->next) genpic_ellipse(e);
	for (l = com->lines; l != NULL; l = l->next) genpic_line(l);
	for (s = com->splines; s != NULL; s = s->next) genpic_spline(s);
	for (t = com->texts; t != NULL; t = t->next) genpic_text(t);
	}

genpic_line(l)
F_line	*l;
{
	F_point		*p, *q;

	set_linewidth(l->thickness);
	set_style(l->style, l->style_val);
	p = l->points;
	q = p->next;
	if (q == NULL) { /* A single point line */
	    fprintf(tfp, "line from %.3f,%.3f to %.3f,%.3f\n",
			p->x/ppi, convy(p->y/ppi), p->x/ppi, convy(p->y/ppi));
	    return;
	    }
	if (l->back_arrow)
	    draw_arrow_head(q->x/ppi, convy(q->y/ppi), p->x/ppi,
		convy(p->y/ppi), l->back_arrow->ht/ppi, l->back_arrow->wid/ppi);
	if (l->style == DASH_LINE && l->style_val > 0.0)
	    fprintf(tfp, "line dashed from");
	else if (l->style == DOTTED_LINE && l->style_val > 0.0)
	    fprintf(tfp, "line dotted from");
	else
	    fprintf(tfp, "line from");
	fprintf(tfp, " %.3f,%.3f to", p->x/ppi, convy(p->y/ppi));
	while (q->next != NULL) {
	    p = q;
	    q = q->next;
	    fprintf(tfp, " %.3f,%.3f to", p->x/ppi, convy(p->y/ppi));
	    }
	fprintf(tfp, " %.3f,%.3f\n", q->x/ppi, convy(q->y/ppi));
	if (l->for_arrow)
	    draw_arrow_head(p->x/ppi, convy(p->y/ppi), q->x/ppi,
		convy(q->y/ppi), l->for_arrow->ht/ppi, l->for_arrow->wid/ppi);
	}

genpic_spline(s)
F_spline	*s;
{
	if (int_spline(s))
	    genpic_itp_spline(s);
	else
	    genpic_ctl_spline(s);
	}

genpic_ctl_spline(s)
F_spline	*s;
{
	if (closed_spline(s))
	    genpic_closed_spline(s);
	else
	    genpic_open_spline(s);
	}

genpic_open_spline(s)
F_spline	*s;
{
	double		x1, y1, x2, y2;
	F_point		*p, *q;

	p = s->points;
	x1 = p->x/ppi; y1 = convy(p->y/ppi);
	p = p->next;
	x2 = p->x/ppi; y2 = convy(p->y/ppi);

	if (s->back_arrow)
	    draw_arrow_head(x2, y2, x1, y1,
		s->back_arrow->ht/ppi, s->back_arrow->wid/ppi);

	/* Pic's spline supports only solid line style */
	/* set_linewidth(s->thickness); */

	if (p->next == NULL) {
	    fprintf(tfp, "line from %.3f,%.3f to %.3f,%.3f\n", x1, y1, x2, y2);
	    if (s->for_arrow)
		draw_arrow_head(x1, y1, x2, y2, s->for_arrow->ht/ppi,
				s->for_arrow->wid/ppi);
	    return;
	    }

	fprintf(tfp, "spline from %.3f,%.3f to %.3f,%.3f", x1, y1, x2, y2);

	for (q = p->next; q->next != NULL; p = q, q = q->next)
	    fprintf(tfp, " to %.3f,%.3f", q->x/ppi, convy(q->y/ppi));
	fprintf(tfp, " to %.3f,%.3f\n", (x2=q->x/ppi), (y2=convy(q->y/ppi)));

	if (s->for_arrow)
	    draw_arrow_head(p->x/ppi, convy(p->y/ppi), x2, y2,
		s->for_arrow->ht/ppi, s->for_arrow->wid/ppi);
	}

genpic_ellipse(e)
F_ellipse	*e;
{
	set_linewidth(e->thickness);
	fprintf(tfp, "ellipse at %.3f,%.3f wid %.3f ht %.3f\n",
		e->center.x/ppi, convy(e->center.y/ppi),
		2 * e->radiuses.x/ppi, 2 * e->radiuses.y/ppi);
	}

/*
Text is display on the screen with the base line starting at
(base_x, base_y); some characters extend below this line.
Pic displays the center of the height of text at the given
coordinate. HT_OFFSET is use to compensate all the above factors
so text position in fig 1.4 should be at the same position on
the screen as on the hard copy.
*/
#define			HT_OFFSET	(0.2 / 72.0)

genpic_text(t)
F_text	*t;
{
	float	y;
	int	first = 1;

	if (first) {
	    first = 0;
	    fprintf(tfp, ".ps %d\n", font_size);
	    }

	y = convy(t->base_y/ppi) + font_size * HT_OFFSET;
	fprintf(tfp, "\"%s\" at %.3f,%.3f ljust\n",
		t->cstring, t->base_x/ppi, y);
	}

genpic_arc(a)
F_arc	*a;
{
	double		x, y;
	double		cx, cy, sx, sy, ex, ey;

	cx = a->center.x/ppi; cy = convy(a->center.y/ppi);
	sx = a->point[0].x/ppi; sy = convy(a->point[0].y/ppi);
	ex = a->point[2].x/ppi; ey = convy(a->point[2].y/ppi);

	set_linewidth(a->thickness);

	if (a->for_arrow) {
	    arc_tangent(cx, cy, ex, ey, a->direction, &x, &y);
	    draw_arrow_head(x, y, ex, ey,
			a->for_arrow->ht/ppi, a->for_arrow->wid/ppi);
	    }
	if (a->back_arrow) {
	    arc_tangent(cx, cy, sx, sy, !a->direction, &x, &y);
	    draw_arrow_head(x, y, sx, sy,
			a->back_arrow->ht/ppi, a->back_arrow->wid/ppi);
	    }

	if (a->direction)
	    fprintf(tfp, "arc at %.3f,%.3f from %.3f,%.3f to %.3f,%.3f\n",
			cx, cy, sx, sy, ex, ey);
	else
	    fprintf(tfp, "arc at %.3f,%.3f from %.3f,%.3f to %.3f,%.3f cw\n",
			cx, cy, sx, sy, ex, ey);

	}

arc_tangent(x1, y1, x2, y2, direction, x, y)
double	x1, y1, x2, y2, *x, *y;
int	direction;
{
	if (direction) { /* counter clockwise  */
	    *x = x2 + (y2 - y1);
	    *y = y2 - (x2 - x1);
	    }
	else {
	    *x = x2 - (y2 - y1);
	    *y = y2 + (x2 - x1);
	    }
	}

/*	draw arrow heading from (x1, y1) to (x2, y2)	*/

draw_arrow_head(x1, y1, x2, y2, arrowht, arrowwid)
double	x1, y1, x2, y2, arrowht, arrowwid;
{
	double	x, y, xb, yb, dx, dy, l, sina, cosa;
	double	xc, yc, xd, yd;

	dx = x2 - x1;  dy = y1 - y2;
	l = sqrt((dx*dx + dy*dy));
	sina = dy / l;  cosa = dx / l;
	xb = x2*cosa - y2*sina;
	yb = x2*sina + y2*cosa;
	x = xb - arrowht;
	y = yb - arrowwid / 2;
	xc = x*cosa + y*sina;
	yc = -x*sina + y*cosa;
	y = yb + arrowwid / 2;
	xd = x*cosa + y*sina;
	yd = -x*sina + y*cosa;
	fprintf(tfp, "line from %.3f,%.3f to %.3f,%.3f to %.3f,%.3f\n",
		xc, yc, x2, y2, xd, yd);
	}

#define		THRESHOLD	.05	/* inch */

quadratic_spline(a1, b1, a2, b2, a3, b3, a4, b4)
double	a1, b1, a2, b2, a3, b3, a4, b4;
{
	double	x1, y1, x4, y4;
	double	xmid, ymid;

	x1 = a1; y1 = b1;
	x4 = a4; y4 = b4;

	xmid = (a2 + a3) / 2;
	ymid = (b2 + b3) / 2;
	if (fabs(x1 - xmid) < THRESHOLD && fabs(y1 - ymid) < THRESHOLD) {
	    fprintf(tfp, "\tto %.3f,%.3f\\\n", xmid, ymid);
	    }
	else {
	    quadratic_spline(x1, y1, ((x1+a2)/2), ((y1+b2)/2),
			((3*a2+a3)/4), ((3*b2+b3)/4), xmid, ymid);
	    }

	if (fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
	    fprintf(tfp, "\tto %.3f,%.3f\\\n", x4, y4);
	    }
	else {
	    quadratic_spline(xmid, ymid, ((a2+3*a3)/4), ((b2+3*b3)/4),
			((a3+x4)/2), ((b3+y4)/2), x4, y4);
	    }
	}

genpic_closed_spline(s)
F_spline	*s;
{
	F_point	*p;
	double	cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
	double	x1, y1, x2, y2;

	p = s->points;
	x1 = p->x/ppi;  y1 = convy(p->y/ppi);
	p = p->next;
	x2 = p->x/ppi;  y2 = convy(p->y/ppi);
	cx1 = (x1 + x2) / 2;      cy1 = (y1 + y2) / 2;
	cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;

	for (p = p->next; p != NULL; p = p->next) {
	    fprintf(tfp, "line from %.3f,%.3f ", cx1, cy1);
	    x1 = x2;  y1 = y2;
	    x2 = p->x/ppi;  y2 = convy(p->y/ppi);
	    cx3 = (3 * x1 + x2) / 4;  cy3 = (3 * y1 + y2) / 4;
	    cx4 = (x1 + x2) / 2;      cy4 = (y1 + y2) / 2;
	    quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);
	    fprintf(tfp, "\n");
	    cx1 = cx4;  cy1 = cy4;
	    cx2 = (x1 + 3 * x2) / 4;  cy2 = (y1 + 3 * y2) / 4;
	    }
	x1 = x2;  y1 = y2;
	p = s->points->next;
	x2 = p->x/ppi;  y2 = convy(p->y/ppi);
	cx3 = (3 * x1 + x2) / 4;  cy3 = (3 * y1 + y2) / 4;
	cx4 = (x1 + x2) / 2;      cy4 = (y1 + y2) / 2;
	fprintf(tfp, "line from %.3f,%.3f ", cx1, cy1);
	quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);
	fprintf(tfp, "\n");
	}

genpic_itp_spline(s)
F_spline	*s;
{
	F_point		*p1, *p2;
	F_control	*cp1, *cp2;
	double		x1, x2, y1, y2;

	p1 = s->points;
	cp1 = s->controls;
	x2 = p1->x/ppi; y2 = convy(p1->y/ppi);

	if (s->for_arrow)
	    draw_arrow_head(cp1->rx/ppi, convy(cp1->ry/ppi), x2, y2,
		s->for_arrow->ht/ppi, s->for_arrow->wid/ppi);

	for (p2 = p1->next, cp2 = cp1->next; p2 != NULL;
		p1 = p2, cp1 = cp2, p2 = p2->next, cp2 = cp2->next) {
	    fprintf(tfp, "line from %.3f,%.3f ", x2, y2);
	    x1 = x2; y1 = y2;
	    x2 = p2->x/ppi; y2 = convy(p2->y/ppi);
	    bezier_spline(x1, y1, (double)cp1->rx/ppi, convy(cp1->ry/ppi),
		(double)cp2->lx/ppi, convy(cp2->ly/ppi), x2, y2);
	    fprintf(tfp, "\n");
	    }

	if (s->back_arrow)
	    draw_arrow_head(cp1->lx/ppi, convy(cp1->ly/ppi), x2, y2,
		s->back_arrow->ht/ppi, s->back_arrow->wid/ppi);
	}

bezier_spline(a0, b0, a1, b1, a2, b2, a3, b3)
double	a0, b0, a1, b1, a2, b2, a3, b3;
{
	double	x0, y0, x3, y3;
	double	sx1, sy1, sx2, sy2, tx, ty, tx1, ty1, tx2, ty2, xmid, ymid;

	x0 = a0; y0 = b0;
	x3 = a3; y3 = b3;
	if (fabs(x0 - x3) < THRESHOLD && fabs(y0 - y3) < THRESHOLD) {
	    fprintf(tfp, "\tto %.3f,%.3f\\\n", x3, y3);
	    }
	else {
	    tx = (a1 + a2) / 2;		ty = (b1 + b2) / 2;
	    sx1 = (x0 + a1) / 2;	sy1 = (y0 + b1) / 2;
	    sx2 = (sx1 + tx) / 2;	sy2 = (sy1 + ty) / 2;
	    tx2 = (a2 + x3) / 2;	ty2 = (b2 + y3) / 2;
	    tx1 = (tx2 + tx) / 2;	ty1 = (ty2 + ty) / 2;
	    xmid = (sx2 + tx1) / 2;	ymid = (sy2 + ty1) / 2;

	    bezier_spline(x0, y0, sx1, sy1, sx2, sy2, xmid, ymid);
	    bezier_spline(xmid, ymid, tx1, ty1, tx2, ty2, x3, y3);
	    }
	}
