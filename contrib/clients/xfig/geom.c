/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	March 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "object.h"

#define			PI		3.14159
#define	 		round(x)	((int)((x) + .5))

/*
Input arguments :
	(x1,y1)(x2,y2) : the vector
	direction : direction of the normal vector to (x1,y1)(x2,y2)
Output arguments :
	(*x,*y)(x2,y2) : a normal vector.
Return value : none
*/

compute_normal(x1, y1, x2, y2, direction, x, y)
float	x1, y1;
int	x2, y2,	direction, *x, *y;
{
	if (direction) { /* counter clockwise  */
	    *x = round(x2 - (y2 - y1));
	    *y = round(y2 - (x1 - x2));
	    }
	else {
	    *x = round(x2 + (y2 - y1));
	    *y = round(y2 + (x1 - x2));
	    }
	}

/*
Input arguments:
	(x1,y1)(x2,y2) : the vector
	(xp,yp) : the point
	d : tolerance (max. allowable distance from the point to the vector)
	dd : d * d
Output arguments:
	(*px,*py) : a point on the vector which is not far from (xp,yp)
		by more than d. Normally the vector (*px,*py)(xp,yp)
		is normal to vector (x1,y1)(x2,y2) except when (xp,yp)
		is within d from (x1,y1) or (x2,y2), in which cases,
		(*px,*py) = (x1,y1) or (x2,y2) respectively. 
Return value :
	0 : No point on the vector is within d from (xp, yp)
	1 : (*px, *py) is such a point.
*/

close_to_vector(x1, y1, x2, y2, xp, yp, d, dd, px, py)
int	 x1, y1, x2, y2, xp, yp, d;
float	 dd;
int	*px, *py;
{
	int	xmin, ymin, xmax, ymax;
	float	x, y, slope, D2, dx, dy;

	if (abs(xp - x1) <= d && abs(yp - y1) <= d) {
	    *px = x1;  *py = y1;
	    return(1);
	    }
	if (abs(xp - x2) <= d && abs(yp - y2) <= d) {
	    *px = x2;  *py = y2;
	    return(1);
	    }

	if (x1 < x2) {
	    xmin = x1 - d; xmax = x2 + d;
	    }
	else {
	    xmin = x2 - d; xmax = x1 + d;
	    }
	if (xp < xmin || xmax < xp) return(0);

	if (y1 < y2) {
	    ymin = y1 - d; ymax = y2 + d;
	    }
	else {
	    ymin = y2 - d; ymax = y1 + d;
	    }
	if (yp < ymin || ymax < yp) return(0);

	if (x2 == x1) {
	    x = x1; y = yp;
	    }
	else if (y1 == y2) {  
	    x = xp;
	    y = y1;
	    }
	else {
	    slope = ((float) (x2 - x1)) / ((float) (y2 - y1));
	    y = (slope * (xp - x1 + slope * y1) + yp) / (1 + slope * slope);
	    x = ((float) x1) + slope * (y - y1);
	    }
	dx = ((float) xp) - x;
	dy = ((float) yp) - y;
	D2 = dx * dx + dy * dy;
	if (D2 < dd) {
	    *px = (int)(x + .5); *py = (int)(y +.5);
	    return(1);
	    }
	return(0);
	}

/*
Input arguments :
	p1, p2, p3 : 3 points on the arc
Output arguments :
	(*x,*y) : Center of the arc
Return value :
	0 : if p1, p2, p3 are co-linear.
	1 : if they are not.
*/

int
compute_arccenter(p1, p2, p3, x, y)
F_pos	p1, p2, p3;
float	*x, *y;
{
	float	s12, s13, len1, len2, len3, dx12, dy12, dx13, dy13;

	dx12 = p1.x - p2.x;  dy12 = p1.y - p2.y;
	dx13 = p1.x - p3.x;  dy13 = p1.y - p3.y;
	s12 = asin((double)(dy12 / sqrt((double)(dx12*dx12 + dy12*dy12))));
	s13 = asin((double)(dy13 / sqrt((double)(dx13*dx13 + dy13*dy13))));
	if (fabs(s12 - s13) < .01) return(0);
	
	len1 = p1.x * p1.x + p1.y * p1.y;
	len2 = p2.x * p2.x + p2.y * p2.y;
	len3 = p3.x * p3.x + p3.y * p3.y;
	*y = (dx12 * (len3 - len1) - dx13 * (len2 - len1)) /
		(2 * (dx13 * dy12 - dx12 * dy13));
	if (dx13 != 0) 
	    *x = (len3 + 2 * (*y) * dy13 - len1) / (2 * (-dx13));
	else
	    *x = (len2 + 2 * (*y) * dy12 - len1) / (2 * (-dx12));
	return(1);
	}

/*
Compute_angle
Input arguments :
	(dx,dy) : the vector (0,0)(dx,dy)
Output arguments : none
Return value : the angle of the vector in the range [0, 2PI)
*/

float
compute_angle(dx, dy)  /*  compute the angle between 0 to 2PI  */
float	dx, dy;
{
	float	alpha;

	if (dx == 0) {
	    if (dy > 0)
		alpha = PI / 2;
	    else
		alpha = 3 * PI / 2;
	    }
	else if (dy == 0) {
	    if (dx > 0) 
		alpha = 0;
	    else
		alpha = PI;
	    }
	else {
	    alpha = atan((double)(dy / dx)); /* range = -PI/2 to PI/2 */
	    if (dx < 0) alpha += PI;
	    else if (dy < 0) alpha += 2 * PI;
	    }
	return(alpha);
	}

/*
Input arguments :
	p1, p2, p3 : 3 points of an arc with p1 the first and p3 the last.
Output arguments : none
Return value :
	0 : if the arc passes p1, p2 and p3 (in that order) in
		clockwise direction
	1 : if direction is counterclockwise
*/

int
compute_direction(p1, p2, p3)
F_pos	p1, p2, p3;
{
	float		diff, dx, dy, alpha, theta;

	dx = p2.x - p1.x;
	dy = p1.y - p2.y;	/* because origin of the screen is on 
					the upper left corner */
	
	alpha = compute_angle(dx, dy);

	dx = p3.x - p2.x;
	dy = p2.y - p3.y;
	theta = compute_angle(dx, dy);
	
	diff = theta - alpha;
	if ((0 < diff && diff < PI) || diff < -PI) {
	    return(1);			/* counterclockwise */
	    }
	return(0);			/* clockwise */
	}
