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
 *	File:		gr_image.c
 *	Contents:	Image plotting functions for graphics module
 */

#include "gr_com.h"


/*
 *	Create an image from raw data
 */
XImage
*gr_ImageCreate(wid,xdim,ydim,data)
Widget	wid;
int		xdim,ydim;
char	*data;
{
	int 		scr;
	Display 	*dpy;
	Visual	  	*vis;
	XImage    	*image;
	unsigned char	*tdata;
	int			i,j,offbytes,offset,rem;

	dpy = XtDisplay(wid);
	scr = DefaultScreen(dpy);
	vis = gr_GetVisual(dpy,scr);

	offbytes = 1;

	if (gr_color.depth > 8)
	{
		rem = gr_color.depth % 8;
		offbytes = (int)(gr_color.depth/8);
		if (rem != 0) offbytes++;
		offset = offbytes-1;
    	if ((tdata = (unsigned char *)td_Malloc1D(xdim*ydim*offbytes, 1,
        (long)sizeof(unsigned char),"Image raster")) == NULL)
        return(NULL);

		for (i=0;i<xdim*ydim;i++)
		{
			for (j=0;j<offset;j++)
				tdata[i*offbytes+j]=(unsigned char)0;
			tdata[i*offbytes+offset]=(unsigned char)data[i];
		}
	}
	else
		tdata = (unsigned char *) data;

	image = 
		XCreateImage(dpy,vis,gr_color.depth,ZPixmap,0,tdata,xdim,ydim,
		8*offbytes,(int)(xdim*offbytes));

	return(image);
}


/*
 *	Create an pixmap from raw data
 */
Pixmap
gr_PixmapCreate(wid,gc,xdim,ydim,data)
Widget	wid;
GC		gc;
int		xdim,ydim;
char	*data;
{
	XImage		*pic;
	Pixmap		pix;
	Display 	*dpy=XtDisplay(wid);
	Drawable	draw=(Drawable)XtWindow(wid);

    pic = gr_ImageCreate(wid,xdim,ydim,data);

	pix = XCreatePixmap(dpy,draw,xdim,ydim,gr_color.depth);
	
	XPutImage(dpy,(Drawable)pix,gc,pic,0,0,0,0,xdim,ydim);

	return(pix);
}
