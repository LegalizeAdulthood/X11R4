/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Dave Thompson, National Center for Supercomputing Applications
 * davet@ncsa.uiuc.edu
 */
/* Author: Dave Thompson & Dave Whittington*/


#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include <stdio.h>
#include <math.h>

#include "xstuff.h"
#include "i.h"

#define	XLOC	400
#define	YLOC	400
#define	PI	3.1415927

struct	POINT {
	short	int	x,y;
	};

extern	int	white;
extern	Widget	toplevelform;

static	XImage	*pimage;


void	CreatePolarImage();
void	CreateInterPolar();


void	DrawPolar(dataset,type,xdim,ydim)
int	*dataset;
int	type;
int	xdim,ydim;
{
Display	*dpy;
Visual	*vis;
char	*imagebuff;
int	x;
int	y;
static	Widget	polarImageWidget;
static	Widget	topPIWidget;
int	ydim_x_2;

/**/	if (dataset== NULL) {
		printf ("DrawPolar(): (Internal error) dataset == nil\n");
		return;
		}

	dpy = XtDisplay(toplevelform);
	vis = GetVisual(dpy);
	if (vis->class != PseudoColor) {
		PrintMesg("Can't use display. Not a Pseudo Color device\n");
		return;
		}

	ydim_x_2 = ydim << 1;
	if(! (imagebuff = (char *) XIMalloc((ydim_x_2)*(ydim_x_2)))) {
		PrintMesg("Couldn't allocate memory for polar image \n");
		return;
		}

	CreateInterPolar(dataset,xdim,ydim,imagebuff);

	/* Draw image */
	topPIWidget = MakeATopLevel("Image Polar Image",toplevelform,
					XLOC,YLOC,(ydim_x_2)+5,(ydim_x_2)+25);

	polarImageWidget = Make_Window("Polar Image",topPIWidget,
					0,0,(ydim_x_2)+5,(ydim_x_2)+25);

	pimage = XCreateImage(dpy,vis,8,ZPixmap,0,imagebuff,
					(ydim_x_2),(ydim_x_2),8,(2*ydim));

	(void) MakeRasterImage("polar image",polarImageWidget,
					2,22,pimage,NULL,NULL);

	return; 
}/* DrawPolar() */

int DrawPolar2(i)
struct IMAGE *i;
/* return NULL if error */
{
int	ydim_x_2;

	ydim_x_2 = i->ydim << 1;
	if(! (i->polarimage = (char *) XIMalloc((ydim_x_2)*(ydim_x_2)))) {
		PrintMesg("Couldn't allocate memory for polar image \n");
		return(NULL);
		}
	
	/* calculate for non float */
	/*CreateInterPolar(i->ri,i->xdim,i->ydim,i->polarimage);*/
	CreatePolarImage(i->ri,i->xdim,i->ydim,i->polarimage);

	return(1);

}

static void CreatePolarImage(dataset,xdim,ydim,imagebuff)
char	dataset[];
int	xdim;
int	ydim;
char	imagebuff[];
{
long	x,y,rad;		/* for stepping trhough dataset */
long	ys,rs;			/* squared value holders	*/
double	angle,radius;		/* cartesian->polar values	*/
double	dx;			/* dataset x & y		*/
int	dy;

	PrintMesg("Generating polar image\n");

	rs = ydim*ydim;		/* actually radius^2 for speed	*/

	for (y = 0; y < 2*ydim; y++)
	{
		ys = (y - ydim) * (y - ydim);
		for (x = 0; x < 2 * ydim; x++)
		{
			rad = (x - ydim)*(x - ydim) + ys;
			if (rad <= rs)
			{
				radius = sqrt((double) rad);
				if (radius < 1.0) angle = 0.0; else
				angle = acos((double) (x - ydim) / radius);
				if (y > ydim) angle = 2.0*M_PI - angle;
				dx = (angle * xdim) / (2.0 * M_PI);
				dy = radius;
				imagebuff[(long)(x + 2*ydim*y)] =
					dataset[(long)(dx + xdim*dy)];
			}
			else
				imagebuff[x + 2*ydim*y] = 0;
		}
	}
}

static void CreateInterPolar(dataset,xdim,ydim,imagebuff)
char	dataset[];
int	xdim;
int	ydim;
char	imagebuff[];
{
long	x,y,rad;		/* for stepping trhough dataset */
long	ys,rs;			/* squared value holders	*/
double	angle,radius;		/* cartesian->polar values	*/
double	dx,dy;			/* dataset x & y		*/
long	lx,ly;
int	data1,data2;

	PrintMesg("Generating polar image\n");

	rs = ydim*ydim;		/* actually radius^2 for speed	*/

	for (y = 0; y < 2*ydim; y++)
	{
		ys = (y - ydim) * (y - ydim);
		for (x = 0; x < 2 * ydim; x++)
		{
			rad = (x - ydim)*(x - ydim) + ys;
			if (rad <= rs)
			{
				radius = sqrt((double) rad);
				if (radius < 1.0) angle = 0.0; else
				angle = acos((double) (x - ydim) / radius);
				if (y > ydim) angle = 2.0*M_PI - angle;
				dx = (angle * xdim) / (2.0 * M_PI);
				dy = radius;
				lx = (long) dx;
				ly = (long) dy;
				dx = dx - (double) lx;
				dy = dy - (double) ly;
				data1 = (dataset[lx+ly*xdim]*(1.0-dx) +
					dataset[lx+1+ly*xdim]*dx);
				ly++;
				data2 = (dataset[lx+ly*xdim]*(1.0-dx) +
					dataset[lx+1+ly*xdim]*dx);
				data1 = data1*(1.0-dy) + data2*dy;
				imagebuff[(long)(x + 2*ydim*y)] =
					data1;
			}
			else
				imagebuff[x + 2*ydim*y] = 0;
		}
	}
}
