/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985, 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : August 1985.
 *	2nd revision : March 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "alloc.h"
#include "object.h"

extern F_arrow		*make_arrow();
extern char		*calloc();
extern int errno;

static F_ellipse	*read_ellipseobject();
static F_line		*read_lineobject();
static F_text		*read_textobject();
static F_spline		*read_splineobject();
static F_arc		*read_arcobject();
static F_compound	*read_compoundobject();

#define			BUF_SIZE		1024

char			buf[BUF_SIZE];
int			line_no = 0;
int			num_object;

read_fail_message(file, err)
char	*file;
int	err;
{
	extern char	*sys_errlist[];

	if (err == 0)		/* Successful read */
	    return;
	else if (err == ENAMETOOLONG)
	    put_msg("File name \"%s\" is too long", file);
	else if (err == ENOENT)
	    put_msg("File \"%s\" does not exist", file);
	else if (err == ENOTDIR)
	    put_msg("A name in the path \"%s\" is not a directory", file);
	else if (err == EACCES)
	    put_msg("Read access to file \"%s\" is blocked", file);
	else if (err == EISDIR)
	    put_msg("File \"%s\" is a directory", file);
	else if (err == -2) {
	    put_msg("File \"%s\" is empty", file);
	    }
	else if (err == -1) {
	    /* Format error; relevant error message is already delivered */
	    }
	else
	    put_msg("File \"%s\" is not accessable; %s", file, sys_errlist[err]);
	}

/**********************************************************
Read_fig returns :

     0 : successful read.
    -1 : File is in incorrect format
    -2 : File is empty
err_no : if file can not be read for various reasons

The resolution (ppi) and the cooridnate system (coord_sys) are
stored in obj->nwcorner.x and obj->nwcorner.x respectively.
**********************************************************/

read_fig(file_name, obj)
char		*file_name;
F_compound	*obj;
{
	FILE	*fp;

	if ((fp = fopen(file_name, "r")) == NULL)
	    return(errno);
	else
	    return(readfp_fig(fp, obj));
	}

readfp_fig(fp, obj)
FILE  *fp;
F_compound    *obj;
{
        char            c;
        int             status;
  
	num_object = 0;
	c = fgetc(fp);
	if (feof(fp)) return(-2);
	ungetc(c, fp);
	bzero((char*)obj, COMOBJ_SIZE);
	if (c == '#')
	    status = read_objects(fp, obj);
	else
	    status = read_1_3_objects(fp, obj);
	fclose(fp);
	return(status);
        }
	
int
read_objects(fp, obj)
FILE		*fp;
F_compound	*obj;
{
	F_ellipse	*e, *le = NULL;
	F_line		*l, *ll = NULL;
	F_text		*t, *lt = NULL;
	F_spline	*s, *ls = NULL;
	F_arc		*a, *la = NULL;
	F_compound	*c, *lc = NULL;
	int		object, ppi, coord_sys;

	fgets(buf, BUF_SIZE, fp);	/* get the line "#FIG 1.4" */
	line_no++;
	if (get_line(fp) < 0) {
	    put_msg("File is truncated");
	    return(-1);
	    }
	if (2 != sscanf(buf,"%d%d\n", &ppi, &coord_sys)) {
	    put_msg("Incomplete data at line %d", line_no);
	    return(-1);
	    }

	obj->nwcorner.x = ppi;
	obj->nwcorner.y = coord_sys;
	while (get_line(fp) > 0) {
	    if (1 != sscanf(buf, "%d", &object)) {
		put_msg("Incorrect format at line %d", line_no);
		return(-1);
		}
	    switch (object) {
		case O_POLYLINE :
		    if ((l = read_lineobject(fp)) == NULL) return(-1);
		    if (ll)
			ll = (ll->next = l);
		    else 
			ll = obj->lines = l;
		    num_object++;
		    break;
		case O_SPLINE :
		    if ((s = read_splineobject(fp)) == NULL) return(-1);
		    if (ls)
			ls = (ls->next = s);
		    else 
			ls = obj->splines = s;
		    num_object++;
		    break;
		case O_ELLIPSE :
		    if ((e = read_ellipseobject()) == NULL) return(-1);
		    if (le)
			le = (le->next = e);
		    else 
			le = obj->ellipses = e;
		    num_object++;
		    break;
		case O_ARC :
		    if ((a = read_arcobject(fp)) == NULL) return(-1);
		    if (la)
			la = (la->next = a);
		    else 
			la = obj->arcs = a;
		    num_object++;
		    break;
		case O_TEXT :
		    if ((t = read_textobject()) == NULL) return(-1);
		    if (lt)
			lt = (lt->next = t);
		    else 
			lt = obj->texts = t;
		    num_object++;
		    break;
		case O_COMPOUND :
		    if ((c = read_compoundobject(fp)) == NULL) return(-1);
		    if (lc)
			lc = (lc->next = c);
		    else 
			lc = obj->compounds = c;
		    num_object++;
		    break;
		default :
		    put_msg("Incorrect object code at line %d", line_no);
		    return(-1);
		} /*  switch */
	    } /*  while */
	if (feof(fp))
	    return(0);
	else
	    return(errno);
	} /*  read_objects */

static F_arc *
read_arcobject(fp)
FILE	*fp;
{
	F_arc	*a;
	int	n, fa, ba;
	int	type, style;
	float	thickness, wid, ht;

	if (NULL == (Arc_malloc(a))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	a->pen = NULL;
	a->area_fill = NULL;
	a->next = NULL;
	n = sscanf(buf, "%*d%d%d%d%d%d%*d%*d%f%d%d%d%f%f%d%d%d%d%d%d\n",
		&a->type, &a->style, &a->thickness, 
		&a->color, &a->depth, /* skip pen and area_fill */
		&a->style_val, &a->direction, &fa, &ba,
		&a->center.x, &a->center.y, 
		&a->point[0].x, &a->point[0].y, 
		&a->point[1].x, &a->point[1].y, 
		&a->point[2].x, &a->point[2].y);
	if (n != 17) {
	    put_msg(Err_incomp, "arc", line_no);
	    free((char*)a);
	    return(NULL);
	    }

	skip_comment(fp);
	if (fa) {
	    line_no++;
	    if (5 != fscanf(fp, "%d%d%f%f%f", &type, &style, &thickness, &wid, &ht)) {
		fprintf(stderr, Err_incomp, "arc", line_no);
		return(NULL);
		}
	    skip_line(fp);
	    a->for_arrow = make_arrow(type, style, thickness, wid, ht);
	    skip_comment(fp);
	    }
	skip_comment(fp);
	if (ba) {
	    line_no++;
	    if (5 != fscanf(fp, "%d%d%f%f%f", &type, &style, &thickness, &wid, &ht)) {
		fprintf(stderr, Err_incomp, "arc", line_no);
		return(NULL);
		}
	    skip_line(fp);
	    a->back_arrow = make_arrow(type, style, thickness, wid, ht);
	    }
	return(a);
	}

static F_compound *
read_compoundobject(fp)
FILE	*fp;
{
	F_arc		*a, *la = NULL;
	F_ellipse	*e, *le = NULL;
	F_line		*l, *ll = NULL;
	F_spline	*s, *ls = NULL;
	F_text		*t, *lt = NULL;
	F_compound	*com, *c, *lc = NULL;
	int		n, object;

	Compound_malloc(com);
	com->arcs = NULL;
	com->ellipses = NULL;
	com->lines = NULL;
	com->splines = NULL;
	com->texts = NULL;
	com->compounds = NULL;
	com->next = NULL;
	n = sscanf(buf, "%*d%d%d%d%d\n", &com->nwcorner.x, &com->nwcorner.y,
		&com->secorner.x, &com->secorner.y);
	if (4 != n) {
	    put_msg(Err_incomp, "compound", line_no);
	    free((char*)com);
	    return(NULL);
	    }
	while (get_line(fp) > 0) {
	    if (1 != sscanf(buf, "%d", &object)) {
		put_msg(Err_incomp, "compound", line_no);
		free_compound(&com);
		return(NULL);
		}
	    switch (object) {
		case O_POLYLINE :
		    if ((l = read_lineobject(fp)) == NULL) { 
			free_line(&l);
			return(NULL);
			}
		    if (ll)
			ll = (ll->next = l);
		    else 
			ll = com->lines = l;
		    break;
		case O_SPLINE :
		    if ((s = read_splineobject(fp)) == NULL) { 
			free_spline(&s);
			return(NULL);
			}
		    if (ls)
			ls = (ls->next = s);
		    else 
			ls = com->splines = s;
		    break;
		case O_ELLIPSE :
		    if ((e = read_ellipseobject()) == NULL) { 
			free_ellipse(&e);
			return(NULL);
			}
		    if (le)
			le = (le->next = e);
		    else 
			le = com->ellipses = e;
		    break;
		case O_ARC :
		    if ((a = read_arcobject(fp)) == NULL) { 
			free_arc(&a);
			return(NULL);
			}
		    if (la)
			la = (la->next = a);
		    else 
			la = com->arcs = a;
		    break;
		case O_TEXT :
		    if ((t = read_textobject()) == NULL) { 
			free_text(&t);
			return(NULL);
			}
		    if (lt)
			lt = (lt->next = t);
		    else 
			lt = com->texts = t;
		    break;
		case O_COMPOUND :
		    if ((c = read_compoundobject(fp)) == NULL) { 
			free_compound(&c);
			return(NULL);
			}
		    if (lc)
			lc = (lc->next = c);
		    else 
			lc = com->compounds = c;
		    break;
		case O_END_COMPOUND :
		    return(com);
		default :
		    put_msg("Wrong object code at line %d", line_no);
		    return(NULL);
		} /*  switch */
	    }
	if (feof(fp))
	    return(com);
	else
	    return(NULL);
	}

static F_ellipse *
read_ellipseobject()
{
	F_ellipse	*e;
	int		n, pen, fill;

	Ellipse_malloc(e);
	e->area_fill = NULL;
	e->pen = NULL;
	e->next = NULL;
	n = sscanf(buf, "%*d%d%d%d%d%d%d%d%f%d%f%d%d%d%d%d%d%d%d\n",
		&e->type, &e->style, &e->thickness,
		&e->color, &e->depth, &pen, &fill,
		&e->style_val, &e->direction, &e->angle,
		&e->center.x, &e->center.y, 
		&e->radiuses.x, &e->radiuses.y, 
		&e->start.x, &e->start.y, 
		&e->end.x, &e->end.y);
	if (n != 18) {
	    put_msg(Err_incomp, "ellipse", line_no);
	    free((char*)e);
	    return(NULL);
	    }
	return(e);
	}

static F_line *
read_lineobject(fp)
FILE	*fp;
{
	F_line	*l;
	F_point	*p, *q;
	int	n, x, y, pen, fill, fa, ba;
	int	type, style;
	float	thickness, wid, ht;

	Line_malloc(l);
	l->points = NULL;
	l->pen = NULL;
	l->area_fill = NULL;
	l->for_arrow = NULL;
	l->back_arrow = NULL;
	l->next = NULL;

	n = sscanf(buf, "%*d%d%d%d%d%d%d%d%f%d%d%d%d",
		&l->type, &l->style, &l->thickness, &l->color,
		&l->depth, &pen, &fill, &l->style_val, &fa, &ba);
	if (n != 10) {
	    put_msg(Err_incomp, "line", line_no);
	    free((char*)l);
	    return(NULL);
	    }
	skip_comment(fp);
	if (fa) {
	    line_no++;
	    if (5 != fscanf(fp, "%d%d%f%f%f", &type, &style, &thickness, &wid, &ht)) {
		fprintf(stderr, Err_incomp, "line", line_no);
		return(NULL);
		}
	    skip_line(fp);
	    l->for_arrow = make_arrow(type, style, thickness, wid, ht);
	    skip_comment(fp);
	    }
	if (ba) {
	    line_no++;
	    if (5 != fscanf(fp, "%d%d%f%f%f", &type, &style, &thickness, &wid, &ht)) {
		fprintf(stderr, Err_incomp, "line", line_no);
		return(NULL);
		}
	    skip_line(fp);
	    l->back_arrow = make_arrow(type, style, thickness, wid, ht);
	    skip_comment(fp);
	    }

	if (NULL == (l->points = Point_malloc(p))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	if (fscanf(fp, "%d%d", &p->x, &p->y) != 2) {
	    put_msg(Err_incomp, "line", line_no);
	    free_linestorage(l);
	    return(NULL);
	    }
	for (;;) {
	    if (fscanf(fp, "%d%d", &x, &y) != 2) {
		put_msg(Err_incomp, "line", line_no);
		free_linestorage(l);
		return(NULL);
		}
	    if (x == 9999) break;
	    if (NULL == (Point_malloc(q))) {
		put_msg(Err_mem);
		free_linestorage(l);
		return(NULL);
		}
	    q->x = x;
	    q->y = y;
	    q->next = NULL;
	    p->next = q;
	    p = q;
	    }
	skip_line(fp);
	return(l);
	}

static F_spline *
read_splineobject(fp)
FILE	*fp;
{
	F_spline	*s;
	F_point		*p, *q;
	F_control	*cp, *cq;
	int		c, n, x, y, pen, fill, fa, ba;
	int		type, style;
	float		thickness, wid, ht;
	float		lx, ly, rx, ry;

	Spline_malloc(s);
	s->points = NULL;
	s->controls = NULL;
	s->pen = NULL;
	s->area_fill = NULL;
	s->for_arrow = NULL;
	s->back_arrow = NULL;
	s->next = NULL;

	n = sscanf(buf, "%*d%d%d%d%d%d%d%d%f%d%d%d%d%d%d", 
	    	&s->type, &s->style, &s->thickness, &s->color,
		&s->depth, &pen, &fill, &s->style_val, &fa, &ba);
	if (n != 10) {
	    put_msg(Err_incomp, "spline", line_no);
	    free((char*)s);
	    return(NULL);
	    }
	skip_comment(fp);
	if (fa) {
	    line_no++;
	    if (5 != fscanf(fp, "%d%d%f%f%f", &type, &style, &thickness, &wid, &ht)) {
		fprintf(stderr, Err_incomp, "spline", line_no);
		return(NULL);
		}
	    skip_line(fp);
	    s->for_arrow = make_arrow(type, style, thickness, wid, ht);
	    skip_comment(fp);
	    }
	if (ba) {
	    line_no++;
	    if (5 != fscanf(fp, "%d%d%f%f%f", &type, &style, &thickness, &wid, &ht)) {
		fprintf(stderr, Err_incomp, "spline", line_no);
		return(NULL);
		}
	    skip_line(fp);
	    s->back_arrow = make_arrow(type, style, thickness, wid, ht);
	    skip_comment(fp);
	    }

	/* Read points */
	if ((n = fscanf(fp, "%d%d", &x, &y)) != 2) {
	    put_msg(Err_incomp, "spline", line_no);
	    free_splinestorage(s);
	    return(NULL);
	    };
	if (NULL == (s->points = Point_malloc(p))) {
	    put_msg(Err_mem);
	    free_splinestorage(s);
	    return(NULL);
	    }
	p->x = x; p->y = y;
	for (c = 1;;) {
	    if (fscanf(fp, "%d%d", &x, &y) != 2) {
		put_msg(Err_incomp, "spline", line_no);
		p->next = NULL;
		free_splinestorage(s);
		return(NULL);
		};
	    if (x == 9999) break;
	    if (NULL == (Point_malloc(q))) {
		put_msg(Err_mem);
		free_splinestorage(s);
		return(NULL);
		}
	    q->x = x;
	    q->y = y;
	    p->next = q;
	    p = q;
	    c++;
	    }
	p->next = NULL;
	skip_line(fp);

	if (normal_spline(s)) return(s);

	skip_comment(fp);
	/* Read controls */
	if ((n = fscanf(fp, "%f%f%f%f", &lx, &ly, &rx, &ry)) != 4) {
	    put_msg(Err_incomp, "spline", line_no);
	    free_splinestorage(s);
	    return(NULL);
	    };
	if (NULL == (s->controls = Control_malloc(cp))) {
	    put_msg(Err_mem);
	    free_splinestorage(s);
	    return(NULL);
	    }
	cp->lx = lx; cp->ly = ly;
	cp->rx = rx; cp->ry = ry;
	while (--c) {
	    if (fscanf(fp, "%f%f%f%f", &lx, &ly, &rx, &ry) != 4) {
		put_msg(Err_incomp, "spline", line_no);
		cp->next = NULL;
		free_splinestorage(s);
		return(NULL);
		};
	    if (NULL == (Control_malloc(cq))) {
		put_msg(Err_mem);
		cp->next = NULL;
		free_splinestorage(s);
		return(NULL);
		}
	    cq->lx = lx; cq->ly = ly;
	    cq->rx = rx; cq->ry = ry;
	    cp->next = cq;
	    cp = cq;
	    }
	cp->next = NULL;

	skip_line(fp);
	return(s);
	}

static F_text *
read_textobject()
{
	F_text	*t;
	int	n, pen;
	char	s[BUF_SIZE];

	Text_malloc(t);
	t->next = NULL;
	n = sscanf(buf,"%*d%d%d%d%d%d%d%f%d%d%d%d%d %[^\1]",
		&t->font, &t->type, &t->size, &pen,
		&t->color, &t->depth, &t->angle,
		&t->style, &t->height, &t->length, 
		&t->base_x, &t->base_y, s);
	if (n != 13) {
	    put_msg(Err_incomp, "text", line_no);
	    free((char*)t);
	    return(NULL);
	    }
	t->cstring = (char*)calloc((unsigned)(strlen(s)+1), sizeof(char));
	if (NULL == t->cstring) {
	    put_msg(Err_mem);
	    free((char*)t);
	    return(NULL);
	    }
	strcpy(t->cstring, s);
	return(t);
	}

get_line(fp)
FILE	*fp;
{
	while (1) {
	    if (NULL == fgets(buf, BUF_SIZE, fp)) {
		return(-1);
		}
	    line_no++;
	    if (*buf != '#') return(1);		/* Skip comment lines */
	    }
	}

skip_comment(fp)
FILE	*fp;
{
	char c;

	while ((c = fgetc(fp)) == '#') skip_line(fp);
	if (c != '#') ungetc(c, fp);
	}

skip_line(fp)
FILE	*fp;
{
	while (fgetc(fp) != '\n') {
	    if (feof(fp)) return;
	    }
	}
