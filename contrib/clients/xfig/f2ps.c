/* 
 *	F2ps : Fig-to-PostScript translator
 *
 *	Copyright (c) 1986 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	June 1986.
 *	1st revision : March 1988 - read fig 1.4
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "object.h"

#define		PAGE_WIDTH		612	/* points; 8.5" */
#define		PAGE_HEIGHT		792	/* points; 11" */
#define		TRUE			1
#define		FALSE			0
#define		POINT_PER_INCH		72
#define		DEFAULT_FONT_SIZE	11
#define		DEFAULT_FONT		"Times-Roman"
#define		ULIMIT_FONT_SIZE	300

char		Usage[] = "Usage: %s [-f font][-s size][-e scale][-N][-c] [input [output]]\n";
char		*prog;
char		*font = DEFAULT_FONT;
int		font_size = DEFAULT_FONT_SIZE;
int		cur_thickness;
int		show_page = 1;
int		center = 0;
double		scale = 1.0;
double		reduction;
extern int	num_object;
char		*from = NULL, *to = NULL;
FILE		*ffp = NULL, *tfp = NULL;
char		Err_incomp[] = "Incomplete %s object at line %d.";
char		Err_mem[] = "Running out of memory.";

put_msg(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
char   *format, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8;
{
	fprintf(stderr, format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	fprintf(stderr, "\n");
	}

get_args(argc, argv)
int	 argc;
char	*argv[];
{
	char	*a;
	int	first = 1;

	prog = *argv;
	while (--argc) {
	    a = *++argv;
	    if (*a == '-') {
		if (*++a == 'f') {	/* Font name followed */
		    if (--argc)
			font = *++argv;
		    else
			goto error_exit;
		    }
		else if (*a == 'c') {	/* Centering */
		    center = 1;
		    }
		else if (*a == 's') {	/* Font size followed */
		    if (--argc) {
			font_size = atoi(*++argv);
			if (font_size <= 0 || font_size > ULIMIT_FONT_SIZE)
			    goto error_exit;
			}
		    else
			goto error_exit;
		    }
		else if (*a == 'e') {	/* Enlarging factor followed */
		    if (--argc)
			scale = atof(*++argv);
		    else
			goto error_exit;
		    }
		else if (*a == 'N') {	/* No "showpage" */
		    show_page = 0;
		    }
		else
		    goto error_exit;
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
		goto error_exit;
	    }
	return;

    error_exit:
	fprintf(stderr, Usage, prog);
	exit(0);
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
	    fprintf(stderr, "Couldn't open %s", to);
	    fprintf(stderr, Usage, prog);
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
	genps_objects(&objects);
	if (tfp != stdout) (void)fclose(tfp);
	}

#define		BEGIN_PROLOG	"\
/$F2psDict 32 dict def \
$F2psDict begin\
 $F2psDict /mtrx matrix put\
"
#define		ELLIPSE_PS	" \
/DrawEllipse {\
 /endangle exch def\
 /startangle exch def\
 /yrad exch def\
 /xrad exch def\
 /y exch def\
 /x exch def\
 /savematrix mtrx currentmatrix def\
 x y translate xrad yrad scale 0 0 1 startangle endangle arc\
 savematrix setmatrix\
 } def\
"
#define		SPLINE_PS	" \
/DrawSplineSection {\
 /y3 exch def\
 /x3 exch def\
 /y2 exch def\
 /x2 exch def\
 /y1 exch def\
 /x1 exch def\
 /xa x1 x2 add 2 div def\
 /ya y1 y2 add 2 div def\
 /xb x2 x3 add 2 div def\
 /yb y2 y3 add 2 div def\
 /x2 xa xb add 2 div def\
 /y2 ya yb add 2 div def x1 x2 sub abs 2 lt y1 y2 sub abs 2 lt and\
  { x2 y2 lineto }\
  { x2 y2 xb yb x3 y3 x1 y1 xa ya x2 y2 DrawSplineSection\
  /y3 exch def\
  /x3 exch def\
  /yb exch def\
  /xb exch def\
  /y2 exch def\
  /x2 exch def}\
 ifelse\
 x2 x3 sub abs 2 lt y2 y3 sub abs 2 lt and { x3 y3 lineto }\
 { x2 y2 xb yb x3 y3 DrawSplineSection } ifelse\
 } def\
"
#define		END_PROLOG	"\
 end\
 /$F2psBegin {$F2psDict begin /$F2psEnteredState save def} def\
 /$F2psEnd {$F2psEnteredState restore end} def\
\n%%EndProlog\
"

#define		MAX	32000
#define		MIN	-32000
static int	coord_system;
static int	llx = MAX, lly = MAX, urx = MIN, ury = MIN;

prolog(objects)
F_compound	*objects;
{
	char		host[256];
	struct passwd	*who;
	long		when;
	extern char	*ctime(), *strcpy();
	extern long	time();

	fprintf(tfp, "%%!\n");	/* PostScript magic strings */
	who = getpwuid(getuid());
	if (-1 == gethostname(host, sizeof(host)))
	    (void)strcpy(host, "unknown-host!?!?");
	(void) time(&when);
	fprintf(tfp, "%%%%Title: %s\n", ((from) ? from : "stdin"));
	fprintf(tfp, "%%%%Creator: %s\n", prog);
	fprintf(tfp, "%%%%CreationDate: %s", ctime(&when));
	fprintf(tfp, "%%%%For: %s@%s (%s)\n",
			who->pw_name, host, who->pw_gecos);
	fprintf(tfp, "%%%%Pages: %d\n", show_page);
	fprintf(tfp, "%%%%BoundingBox: %d %d %d %d\n", llx, lly, urx, ury);
	fprintf(tfp, "%%%%EndComments\n");
	fprintf(tfp, "%s\n", BEGIN_PROLOG);
	if (ellipse_exist(objects)) fprintf(tfp, "%s\n", ELLIPSE_PS);
	if (normal_spline_exist(objects)) fprintf(tfp, "%s\n", SPLINE_PS);
	fprintf(tfp, "%s\n", END_PROLOG);
	fprintf(tfp, "$F2psBegin\n");
	}

epilog()
{
	if (show_page) fprintf(tfp, "showpage\n");
	fprintf(tfp, "$F2psEnd\n");
	}

genps_objects(objects)
F_compound	*objects;
{
	double		tx, scalex, scaley;
	double		dx, dy, origx, origy;
	F_arc		*a;
	F_compound	*c;
	F_ellipse	*e;
	F_line		*l;
	F_spline	*s;
	F_text		*t;

	/* Compute bounding box of objects */
	compound_bound(objects, &llx, &lly, &urx, &ury);
	if (llx > urx) {
	    fprintf(stderr, "No object");
	    return;
	    }
	coord_system = objects->nwcorner.y;
	reduction = POINT_PER_INCH / (float)objects->nwcorner.x;
	scalex = scaley = scale * reduction;
	/* convert to point unit */
	llx = (int)ceil(llx * scalex); lly = (int)ceil(lly * scaley);
	urx = (int)ceil(urx * scalex); ury = (int)ceil(ury * scaley);
	if (center) {
	    dx = urx - llx;
	    dy = ury - lly;
	    tx = (PAGE_WIDTH - dx) / 2.0;
	    origx = tx - llx;
	    urx = (llx=tx) + dx;
	    ury = (PAGE_HEIGHT + dy) / 2.0;
	    if (coord_system == 2)
		origy = ury + lly;
	    else
		origy = ury - dy - lly;
	    lly = ury - dy;
	    }
	else {
	    origx = 0.0;
	    origy = PAGE_HEIGHT;
	    }
	if (coord_system == 2) scaley = -scaley;

	prolog(objects);

	fprintf(tfp, "%f %f translate %.3f %.3f scale\n",
		origx, origy, scalex, scaley);

	/* **** Land scape mode ****
	*if (WIDTH > HEIGHT) {	
	*    fprintf(tfp, "%d 0 translate 90 rotate\n", PAGE_WIDTH);
	*    if ((t = PAGE_HEIGHT / SCALE / WIDTH) < 1.0)
	*	fprintf(tfp, "%f %f scale\n", t, t);
	*    }
	*else if ((t = PAGE_HEIGHT / SCALE / HEIGHT) < 1.0)
	*    fprintf(tfp, "%f %f scale\n", t, t);
	*/

	for (a = objects->arcs; a != NULL; a = a->next) genps_arc(a);
	for (c = objects->compounds; c != NULL; c = c->next) genps_compound(c);
	for (e = objects->ellipses; e != NULL; e = e->next) genps_ellipse(e);
	for (l = objects->lines; l != NULL; l = l->next) genps_line(l);
	for (s = objects->splines; s != NULL; s = s->next) genps_spline(s);
	for (t = objects->texts; t != NULL; t = t->next) genps_text(t);
	epilog();
	}

set_style(s, v)
int	s;
float	v;
{
	if (s == DASH_LINE) {
	    if (v > 0.0) fprintf(tfp, "\t[%f] 0 setdash\n", v);
	    }
	else if (s == DOTTED_LINE) {
	    if (v > 0.0) fprintf(tfp, "\t[1 %f] 0 setdash\n", v);
	    }
	}

reset_style(s, v)
int	s;
float	v;
{
	if (s == DASH_LINE) {
	    if (v > 0.0) fprintf(tfp, "\t[] 0 setdash\n");
	    }
	else if (s == DOTTED_LINE) {
	    if (v > 0.0) fprintf(tfp, "\t[] 0 setdash\n");
	    }
	}

set_linewidth(w)
int	w;
{
	extern int	cur_thickness;

	if (w != cur_thickness) {
	    cur_thickness = w;
	    fprintf(tfp, "%.3f setlinewidth\n", 0.7 * cur_thickness);
	    }
	}

genps_compound(com)
F_compound	*com;
{
	F_arc		*a;
	F_compound	*c;
	F_ellipse	*e;
	F_line		*l;
	F_spline	*s;
	F_text		*t;

	for (a = com->arcs; a != NULL; a = a->next) genps_arc(a);
	for (c = com->compounds; c != NULL; c = c->next) genps_compound(c);
	for (e = com->ellipses; e != NULL; e = e->next) genps_ellipse(e);
	for (l = com->lines; l != NULL; l = l->next) genps_line(l);
	for (s = com->splines; s != NULL; s = s->next) genps_spline(s);
	for (t = com->texts; t != NULL; t = t->next) genps_text(t);
	}

genps_line(l)
F_line	*l;
{
	F_point		*p, *q;

	set_linewidth(l->thickness);
	p = l->points;
	q = p->next;
	if (q == NULL) { /* A single point line */
	    fprintf(tfp, "newpath %d %d moveto %d %d lineto stroke\n",
			p->x, p->y, p->x, p->y);
	    return;
	    }
	if (l->back_arrow)
	    draw_arrow_head((float)q->x, (float)q->y, (float)p->x,
			(float)p->y, l->back_arrow->ht, l->back_arrow->wid);
	set_style(l->style, l->style_val);
	fprintf(tfp, "%% Polyline\n");
	fprintf(tfp, "newpath %d %d moveto", p->x, p->y);
	while (q->next != NULL) {
	    p = q;
	    q = q->next;
	    fprintf(tfp, " %d %d lineto", p->x, p->y);
	    }
	if (l->type == T_POLYLINE)
	    fprintf(tfp, " %d %d lineto stroke\n", q->x, q->y);
	else
	    fprintf(tfp, " closepath stroke\n");
	reset_style(l->style, l->style_val);
	if (l->for_arrow)
	    draw_arrow_head((float)p->x, (float)p->y, (float)q->x,
			(float)q->y, l->for_arrow->ht, l->for_arrow->wid);
	}

genps_spline(s)
F_spline	*s;
{
	if (int_spline(s))
	    genps_itp_spline(s);
	else
	    genps_ctl_spline(s);
	}

genps_itp_spline(s)
F_spline	*s;
{
	F_point		*p, *q;
	F_control	*a, *b;

	set_linewidth(s->thickness);
	a = s->controls;
	p = s->points;
	if (s->for_arrow)
	    draw_arrow_head(a->rx, a->ry, (float)p->x,
			(float)p->y, s->for_arrow->ht, s->for_arrow->wid);

	set_style(s->style, s->style_val);
	fprintf(tfp, "%% Interpolated spline\n");
	fprintf(tfp, "newpath %d %d moveto\n", p->x, p->y);
	for (q = p->next; q != NULL; p = q, q = q->next) {
	    b = a->next;
	    fprintf(tfp, "\t%.3f %.3f %.3f %.3f %d %d curveto\n",
			a->rx, a->ry, b->lx, b->ly, q->x, q->y);
	    a = b;
	    b = b->next;
	    }
	if (closed_spline(s)) fprintf(tfp, " closepath");
	fprintf(tfp, " stroke\n");
	reset_style(s->style, s->style_val);

	if (s->for_arrow)
	    draw_arrow_head(a->lx, a->ly, (float)p->x,
			(float)p->y, s->for_arrow->ht, s->for_arrow->wid);
	}

genps_ctl_spline(s)
F_spline	*s;
{
	float		a, b, c, d, x1, y1, x2, y2, x3, y3;
	F_point		*p, *q;

	/*
	if (first) {
	    first = FALSE;
	    fprintf(tfp, "%s\n", SPLINE_PS);
	    }
	*/

	p = s->points;
	x1 = p->x; y1 = p->y;
	p = p->next;
	c = p->x; d = p->y;
	set_linewidth(s->thickness);
	x3 = a = (x1 + c) / 2;
	y3 = b = (y1 + d) / 2;
	if (s->back_arrow) {
	    draw_arrow_head(c, d, x1, y1, s->back_arrow->ht, s->back_arrow->wid);
	    }
	set_style(s->style, s->style_val);
	if (! closed_spline(s)) {
	    fprintf(tfp, "%% Open spline\n");
	    fprintf(tfp, "newpath %.3f %.3f moveto %.3f %.3f lineto\n",
			x1, y1, x3, y3);
	    }
	else {
	    fprintf(tfp, "%% Closed spline\n");
	    fprintf(tfp, "newpath %.3f %.3f moveto\n", a, b);
	    }
	for (q = p->next; q != NULL; p = q, q = q->next) {
	    x1 = x3; y1 = y3;
	    x2 = c;  y2 = d;
	    c = q->x; d = q->y;
	    x3 = (x2 + c) / 2;
	    y3 = (y2 + d) / 2;
	    fprintf(tfp, "\t%.3f %.3f %.3f %.3f %.3f %.3f DrawSplineSection\n",
			x1, y1, x2, y2, x3, y3);
	    }
	/*
	* At this point, (x2,y2) and (c,d) are the position of the 
	* next-to-last and last point respectively, in the point list
	*/
	if (closed_spline(s)) {
	    fprintf(tfp, "\t%.3f %.3f %.3f %.3f %.3f %.3f DrawSplineSection closepath stroke\n",
			x3, y3, c, d, a, b);
	    }
	else {
	    fprintf(tfp, "\t%.3f %.3f lineto stroke\n", c, d);
	    }
	reset_style(s->style, s->style_val);
	if (s->for_arrow) {
	    draw_arrow_head(x2, y2, c, d, s->for_arrow->ht,
				s->for_arrow->wid);
	    }
	}

genps_ellipse(e)
F_ellipse	*e;
{
	set_linewidth(e->thickness);
	set_style(e->style, e->style_val);
	fprintf(tfp, "%% Ellipse\n");
	fprintf(tfp, "newpath %d %d %d %d 0 360 DrawEllipse stroke\n",
		e->center.x, e->center.y, e->radiuses.x, e->radiuses.y);
	reset_style(e->style, e->style_val);
	}

#define	TEXT_PS		"\
/%s findfont %f scalefont setfont\n\
"
genps_text(t)
F_text	*t;
{
	static int	first = TRUE;
	char		*c;

	if (first) {
	    first = FALSE;
	    fprintf(tfp, TEXT_PS, font, font_size/reduction);
	    }
	if (coord_system == 2) {
	    fprintf(tfp, "%d %d moveto 1 -1 scale (", t->base_x, t->base_y);
	    for (c = t->cstring; *c; c++) {
		if (*c == '\\' || *c == '(' || *c == ')') putc('\\', tfp);
		putc(*c, tfp);
		}
	    fprintf(tfp, ") show 1 -1 scale\n");
	    }
	else if (coord_system == 1) {
	    fprintf(tfp, "%d %d moveto (", t->base_x, t->base_y);
	    for (c = t->cstring; c; ) {
		if (*c == '\\' || *c == '(' || *c == ')') putc('\\', tfp);
		putc(*c, tfp);
		}
	    fprintf(tfp, ") show\n");
	    }
	}

genps_arc(a)
F_arc	*a;
{
	double		angle1, angle2, dx, dy, radius, x, y;
	double		cx, cy, sx, sy, ex, ey;
	int		direction;

	cx = a->center.x; cy = a->center.y;
	sx = a->point[0].x; sy = a->point[0].y;
	ex = a->point[2].x; ey = a->point[2].y;

	if (coord_system == 2)
	    direction = !a->direction;
	else
	    direction = a->direction;
	set_linewidth(a->thickness);
	if (a->for_arrow) {
	    arc_tangent(cx, cy, ex, ey, direction, &x, &y);
	    draw_arrow_head(x, y, ex, ey, a->for_arrow->ht, a->for_arrow->wid);
	    }
	if (a->back_arrow) {
	    arc_tangent(cx, cy, sx, sy, !direction, &x, &y);
	    draw_arrow_head(x, y, sx, sy, a->back_arrow->ht, a->back_arrow->wid);
	    }
	dx = cx - sx;
	dy = cy - sy;
	radius = sqrt(dx*dx + dy*dy);
	angle1 = atan2(sy-cy, sx-cx) * 180 / M_PI;
	angle2 = atan2(ey-cy, ex-cx) * 180 / M_PI;
	/* direction = 1 -> Counterclockwise */
	set_style(a->style, a->style_val);
	fprintf(tfp, "newpath %.3f %.3f %.3f %.3f %.3f %s stroke\n",
		cx, cy, radius, angle1, angle2,
		((direction == 1) ? "arc" : "arcn"));
	reset_style(a->style, a->style_val);
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
float	x1, y1, x2, y2, arrowht, arrowwid;
{
	float	x, y, xb, yb, dx, dy, l, sina, cosa;
	float	xc, yc, xd, yd;

	dx = x2 - x1;  dy = y1 - y2;
	l = sqrt((double)(dx*dx + dy*dy));
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
	fprintf(tfp, "newpath %.3f %.3f moveto %.3f %.3f lineto %.3f %.3f lineto stroke\n",
		xc, yc, x2, y2, xd, yd);
	}

ellipse_exist(ob)
F_compound	*ob;
{
	F_compound	*c;

	if (NULL != ob->ellipses) return(1);

	for (c = ob->compounds; c != NULL; c = c->next) {
	    if (ellipse_exist(c)) return(1);
	    }

	return(0);
	}

normal_spline_exist(ob)
F_compound	*ob;
{
	F_spline	*s;
	F_compound	*c;

	for (s = ob->splines; s != NULL; s = s->next) {
	    if (normal_spline(s)) return(1);
	    }

	for (c = ob->compounds; c != NULL; c = c->next) {
	    if (normal_spline_exist(c)) return(1);
	    }

	return(0);
	}
