/*
 * National Center for SuperComputing Applications, University of Illinois
 *
 * This NCSA software product is public domain software.  Permission
 * is hereby granted to do whatever you like with it. Should you wish
 * to make a contribution towards the production of this software, please
 * send us your comments about your experience with the software,  why
 * you liked or disliked it, how you use it, and most importantly, how it
 * helps your work. We will receive your comments at softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Eng-Whatt Toh, National Center for Supercomputing Applications
 *         ewtoh@ncsa.uiuc.edu
 */

/*
 *	File:		gr_iso1.c
 *	Contents:	Iso-surface level routines for graphics module
 */

#include "gr_com.h"
#include "gr_iso.h"
#include <math.h>
extern double gr_rint();

gr_IsoInit(isoWin)
A_IsoWind_t *isoWin;
{
	int i,j;

    for (i=0;i<isoWin->xsize;i++)
       for (j=0;j<isoWin->ysize;j++)
       {
         isoWin->data[j][i] = (unsigned char)gr_color.white;
         isoWin->fb[j][i] = isoWin->zdim*isoWin->scale;
       }
}


void
gr_IsoCreateImage(isoWin)
A_IsoWind_t	*isoWin;
{
	Widget		wid=isoWin->imageWin;
	Display	    *dpy=XtDisplay(wid);
	Window		win=XtWindow(wid);

    isoWin->image =
		gr_ImageCreate(isoWin->imageVPort,isoWin->xsize,isoWin->ysize,
			isoWin->data[isoWin->ysize-1]);

	XClearWindow(dpy,win);
	XPutImage(dpy,win,isoWin->gc,isoWin->image,0,0,0,0,isoWin->xsize,isoWin->ysize);
}

/*
 *	View iso-surfaces
 */
void
gr_IsoView(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_IsoWind_t *isoWin=(A_IsoWind_t *)client_data;
	A_Data_t	*hdf=isoWin->parent->data;
	float32		***hdfdata=hdf->data;
	float		val;
	float32		values[1];
	char		*strng;

	strng = gr_DialogGetValue(isoWin->isoDialog);
	sscanf(strng,"%f",&val);
	values[0] = (float32)val;

    gr_WidgetCursor(isoWin->shell,XC_watch);
    mcube(isoWin, hdfdata, isoWin->xdim, isoWin->ydim, isoWin->zdim,
			0, isoWin->xdim-1, 0, isoWin->ydim-1, 0, isoWin->zdim-1,
					values, 1);
    gr_WidgetCursor(isoWin->shell,XC_draped_box);

	gr_IsoCreateImage(isoWin);

	return;
}

static float32 A,B,C,K,intensity;
static float32 minX,minY,maxX,maxY,maxZ,startX,endX;
static float32 mb[3],xb[3];
static int	   i,j,k,nb[3],tb[3];

void
gr_IsoScanFill(isoWin,p)
A_IsoWind_t *isoWin;
InterpCoor	p[3];
{
	XGCValues gcvals;
	GC		 gc;
	Widget  wid=isoWin->imageWin;
	XImage  *image=isoWin->image;
	float32 scale=(float32)isoWin->scale;
	char	**data=isoWin->data;
	int		**fb=isoWin->fb;
	int		sx,ex,sy,ey,lineX;
	int		ysize=isoWin->ysize;
	int	    ix,iy;
	Bool	first=TRUE;

	minX = maxX = p[0].x = p[0].x*scale;
	minY = maxY = p[0].y = p[0].y*scale;
	maxZ        = p[0].z = p[0].z*scale;

    for (i=1;i<3;i++) {
		p[i].x = p[i].x * scale;
		p[i].y = p[i].y * scale;
		p[i].z = p[i].z * scale;
		if (p[i].x < minX) minX = p[i].x; else
		if (p[i].x > maxX) maxX = p[i].x;
		if (p[i].y < minY) minY = p[i].y; else
		if (p[i].y > maxY) maxY = p[i].y;
		if (p[i].z > maxZ) maxZ = p[i].z;
	}

    A = p[0].y*(p[1].z-p[2].z)+p[1].y*(p[2].z-p[0].z)+p[2].y*(p[0].z-p[1].z);
    B = p[0].z*(p[1].x-p[2].x)+p[1].z*(p[2].x-p[0].x)+p[2].z*(p[0].x-p[1].x);
    C = p[0].x*(p[1].y-p[2].y)+p[1].x*(p[2].y-p[0].y)+p[2].x*(p[0].y-p[1].y);
    K = sqrt((A*A)+(B*B)+(C*C));
 
    if ( K != 0.0 )
    {
        intensity = (((A+B+C)/(K*sqrt(3.0)))*240.0)+5.0;
        if (intensity < 0.0) intensity = -intensity;
        if ((intensity > 0.0) && (intensity < 250.0))
        {
			if (p[1].y == p[0].y)
				nb[0] = 0;
			else
			{
				nb[0] = 1;
				mb[0] = (p[1].x-p[0].x)/(p[1].y-p[0].y);
			}

			if (p[2].y == p[1].y)
				nb[1] = 0;
			else
			{
				nb[1] = 1;
				mb[1] = (p[2].x-p[1].x)/(p[2].y-p[1].y);
			}

			if (p[0].y == p[2].y)
				nb[2] = 0;
			else
			{
				nb[2] = 1;
				mb[2] = (p[0].x-p[2].x)/(p[0].y-p[2].y);
			}
			for (iy=(int)minY;iy<=(int)maxY;iy++)
			{
				if (nb[0]) xb[0] = mb[0]*((float32)iy-p[0].y)+p[0].x;
				if (nb[1]) xb[1] = mb[1]*((float32)iy-p[1].y)+p[1].x;
				if (nb[2]) xb[2] = mb[2]*((float32)iy-p[2].y)+p[2].x;
				tb[0] = ( (((float32)iy>=p[0].y) && ((float32)iy<=p[1].y)) ||
						  (((float32)iy>=p[1].y) && ((float32)iy<=p[0].y)) );
				tb[1] = ( (((float32)iy>=p[1].y) && ((float32)iy<=p[2].y)) ||
						  (((float32)iy>=p[2].y) && ((float32)iy<=p[1].y)) );
				tb[2] = ( (((float32)iy>=p[2].y) && ((float32)iy<=p[0].y)) ||
						  (((float32)iy>=p[0].y) && ((float32)iy<=p[2].y)) );
				k=0;
				for (j=0;j<3;j++)
					if ((nb[j]) && (tb[j]) &&
						(xb[j] >= minX) && (xb[j] <= maxX))
					{
						if (k==0)
						   startX=endX=xb[j];
						else
						if (xb[j] < startX)
							startX = xb[j];
						else
						if (xb[j] > endX)
							endX = xb[j];
						k++;
					}

				if (k > 1)
				{
					first=TRUE;
					for (ix=(int)startX;ix<=(int)endX;ix++)
						if (fb[iy][ix] > (int)maxZ)
				   		{
					  		fb[iy][ix] = (int)maxZ;
							data[iy][ix] = (int)intensity;
							if (first == TRUE)
							{
								lineX = ix;
								first = FALSE;
							}
						}
						else
						if (first == FALSE)
						{
							gcvals.foreground = (int)intensity;
							gc = XtGetGC(wid,GCForeground,&gcvals);
							XDrawLine(XtDisplay(wid),XtWindow(wid),gc,
						lineX,(int)(ysize-iy),(int)(ix-1),(int)(ysize-iy));
							first = TRUE;
						}

					if (first == FALSE)
					{
						gcvals.foreground = (int)intensity;
						gc = XtGetGC(wid,GCForeground,&gcvals);
						XDrawLine(XtDisplay(wid),XtWindow(wid),gc,
						lineX,(int)(ysize-iy),(int)endX,(int)(ysize-iy));
						first = TRUE;
					}

				}

			}
        }
    }
}


/*
 *	Expose event for iso-surface window
 */
void
gr_IsoExpose(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_IsoWind_t *isoWin=(A_IsoWind_t *)client_data;
	Display		*dpy = XtDisplay(isoWin->imageWin);
	Window	win = XtWindow(isoWin->imageWin);
	GC		gc = isoWin->gc;

	gr_ImageSetCMap(isoWin->shell);
	XPutImage(dpy,win,isoWin->gc,isoWin->image,0,0,
	0,0,(int)isoWin->xsize,(int)isoWin->ysize);

	return;
}


/*
 *	View XY axis
 */
void
gr_IsoSetXY(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_IsoWind_t *isoWin=(A_IsoWind_t *)client_data;

	isoWin->axesOrient.row = 1;
	isoWin->axesOrient.col = 2;
	isoWin->axesOrient.axis = 3;

	gr_IsoInit(isoWin);
	gr_IsoCreateImage(isoWin);

	return;
}

/*
 *	View XZ axis
 */
void
gr_IsoSetXZ(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_IsoWind_t *isoWin=(A_IsoWind_t *)client_data;

	isoWin->axesOrient.row = 1;
	isoWin->axesOrient.col = 3;
	isoWin->axesOrient.axis = 2;

	gr_IsoInit(isoWin);
	gr_IsoCreateImage(isoWin);

	return;
}

/*
 *	View YZ axis
 */
void
gr_IsoSetYZ(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_IsoWind_t *isoWin=(A_IsoWind_t *)client_data;

	isoWin->axesOrient.row = 2;
	isoWin->axesOrient.col = 3;
	isoWin->axesOrient.axis = 1;

	gr_IsoInit(isoWin);
	gr_IsoCreateImage(isoWin);

	return;
}
