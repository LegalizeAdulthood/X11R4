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
#include "object.h"

extern F_compound	objects;

F_arc *
arc_search(x, y, tolerance, px, py)
int	x, y, tolerance, *px, *py;
{	/*	(px, py) is the control point on the circumference of an arc
		which is the closest to (x, y)				*/

	F_arc	*a;
	int	i;

	for (a = objects.arcs; a != NULL; a = a->next) {
	    for (i = 0; i < 3; i++) 
		if ((abs(a->point[i].x - x) <= tolerance) && 
		    (abs(a->point[i].y - y) <= tolerance)) {
		    *px = a->point[i].x;
		    *py = a->point[i].y;
		    return(a);
		    }
	    }
	return(NULL);
	}


F_ellipse *
ellipse_search(x, y, tolerance, px, py)
int	x, y, tolerance, *px, *py;
{	/*	(px, py) is the point on the circumference of an ellipse
		which is the closest to (x, y)				*/

	F_ellipse	*e;
	int		a, b, dx, dy;
	float		dis, r, tol;

	tol = (float) tolerance;
	for (e = objects.ellipses; e != NULL; e = e->next) {
	    dx = x - e->center.x;
	    dy = y - e->center.y;
	    a = e->radiuses.x;
	    b = e->radiuses.y;
	    dis = sqrt((double)(dx*dx + dy*dy));
	    if (dis == 0) {
		    *px = e->center.x;
		    *py = e->center.y;
		    return(e);
	    }
	    r = a * b * dis / sqrt((double) (b*b*dx*dx + a*a*dy*dy));
	    if (fabs(dis - r) <= tol) {
		*px = (int)(r*dx/dis + ((dx < 0) ? -.5 : .5)) + e->center.x;
		*py = (int)(r*dy/dis + ((dy < 0) ? -.5 : .5)) + e->center.y;
		return(e);
		}
	    }
	return(NULL);
	}

F_line *
line_search(x, y, tolerance, px, py)
int	x, y, tolerance, *px, *py;
{	/*	return the pointer to lines object if the search is successful
		otherwise return NULL.  
		The value returned via (px, py) is the closest point on the 
		vector to point (x, y) 					*/
 
	F_line	*lines;
	F_point	*point;
	int	x1, y1, x2, y2;
	float	tol2;

	tol2 = (float) tolerance * tolerance;
	for (lines = objects.lines; lines != NULL; lines = lines->next) {
	    point = lines->points;
	    x1 = point->x;
	    y1 = point->y;
	    if (abs(x - x1) <= tolerance && abs(y - y1) <= tolerance) {
		*px = x1;  *py = y1;
		return(lines);
		}
	    for (point = point->next; point != NULL; point = point->next) {
		x2 = point->x;
		y2 = point->y;
		if (close_to_vector(x1, y1, x2, y2, x, y, tolerance, tol2, 
					px, py)) 
		    return(lines);
		x1 = x2;
		y1 = y2;
		}
	    }
	return(NULL);
	}

F_spline *
spline_search(x, y, tolerance, px, py)
int	x, y, tolerance, *px, *py;
{	/*	return the pointer to lines object if the search is successful
		otherwise return NULL.  */
 
	F_spline	*splines;
	F_point		*point;
	int		x1, y1, x2, y2;
	float		tol2;

	tol2 = (float) tolerance * tolerance;
	for (splines = objects.splines; splines != NULL; splines = splines->next) {
	    point = splines->points;
	    x1 = point->x;
	    y1 = point->y;
	    for (point = point->next; point != NULL; point = point->next) {
		x2 = point->x;
		y2 = point->y;
		if (close_to_vector(x1, y1, x2, y2, x, y, tolerance, tol2,
					px, py)) 
		    return(splines);
		x1 = x2;
		y1 = y2;
		}
	    }
	return(NULL);
	}

F_text *
text_search(x, y)
int	x, y;
{
	F_text	*t;

	for (t = objects.texts; t != NULL; t = t->next) {
	    if (t->base_y - t->height > y) continue;
	    if (t->base_y < y) continue;
	    if (t->base_x > x) continue;
	    if (t->base_x + t->length < x) continue;
	    return(t);
	    }
	return(NULL);
	}
