/*  Tensor field plot -- plots a fluctuation grid over a data set, with */
/*  tendencies toward higher data points.  Concept: David Thompson	*/
/*  Written:  David Whittington.					*/

#include	<X11/Xlib.h>
#include	<X11/Intrinsic.h>

#define	SQR(x)	(x)*(x)
#define	INIT_FIELD_STR	2
#define	INIT_INERTIA	5
#define	INIT_X_DENSITY	20
#define	INIT_Y_DENSITY	20

static	int	x,y;
static	int	lastX,lastY;
static	int	xdim,ydim;
static	char	*ri;
static	Display	*disp;
static	Drawable d;
static	GC	the_gc;

static unsigned char data(x,y)
int	x,y;
{
	return(ri[y*xdim + x]);
}

void ForceX(strength,inertia)
int	strength,inertia;
{
int	delta,average,direction;

	delta = 0;
	average = ((int) data(x+1,y) - (int) data(x-1,y))/2;
	direction = (average < 0) ? -1 : 1;
	average = average * direction;
	while ((average - (delta*direction + 1)/strength +
		direction*(lastX - delta - x)/inertia) > 0)
	{
		delta += direction;
		average -= delta * direction / strength;
		if ((x+delta) < 1)
		{
			delta -= direction;
			average = 0;
		}
		else
		if ((x+delta) > (xdim - 1))
		{
			delta -= direction;
			average = 0;
		}
		else
		average += (((int) data(x+delta+1,y)-
			     (int) data(x+delta-1,y))/2)*direction;
	}
	XDrawLine(disp,d,the_gc,lastX,lastY,x+delta,y);
	lastX = x+delta;
	lastY = y;
}

void ForceY(strength,inertia)
int	strength,inertia;
{
int	delta,average,direction;

	delta = 0;
	average = ((int) data(x,y+1) - (int) data(x,y-1))/2;
	direction = (average < 0) ? -1 : 1;
	average = average * direction;
	while ((average - (delta*direction + 1)/strength +
		direction*(lastY - delta - y)/inertia) > 0)
	{
		delta += direction;
		average -= delta * direction / strength;
		if ((y+delta) < 1)
		{
			delta -= direction;
			average = 0;
		}
		else
		if ((y+delta) > (ydim - 1))
		{
			delta -= direction;
			average = 0;
		}
		else
		average += (((int) data(x,y+delta+1)-
			     (int) data(x,y+delta-1))/2)*direction;
	}
	XDrawLine(disp,d,the_gc,lastX,lastY,x,y+delta);
	lastX = x;
	lastY = y+delta;
}

void Updt_Intensity(w,image,xmax,ymax,fore,xorflag)
Widget	w;
char	*image;
int	xmax,ymax;
int	fore,xorflag;
{
XGCValues values;
int	xDensity,yDensity;
int	xStart,yStart;
int	current;

	disp = XtDisplay(w);
	d = XtWindow(w);
	if (xorflag == 0)
	{
		values.foreground = fore;
		the_gc = XtGetGC(w,(GCForeground),&values);
	}
	else
	{
		values.function = GXxor;
		values.foreground = 255;
		the_gc = XtGetGC(w,(GCFunction | GCForeground),&values);
	}

	xdim = xmax;
	ydim = ymax;
	ri = image;
	xDensity = INIT_X_DENSITY;
	yDensity = INIT_Y_DENSITY;
	xStart = (xdim - (xdim / xDensity)*xDensity)/2;
	yStart = (ydim - (ydim / yDensity)*yDensity)/2;
	xStart = (xStart < 1) ? xDensity / 2 : xStart;
	yStart = (yStart < 1) ? yDensity / 2 : yStart;

	for (x = xStart; x < xdim; x += xDensity)
	{
		lastX = x;
		lastY = 0;
		for (y = 1; y < ydim; y++)
			ForceX(INIT_FIELD_STR,INIT_INERTIA);
	}
	for (y = yStart; y < ydim; y += yDensity)
	{
		lastX = 0;
		lastY = y;
		for (x = 1; x < xdim; x++)
			ForceY(INIT_FIELD_STR,INIT_INERTIA);
	}
}

