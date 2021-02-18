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
 *	File:		gr_tile.c
 *	Contents:	routines for tile window
 */

#include "gr_com.h"

void
gr_TileSelect(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_TileWind_t		*tileWin=(A_TileWind_t *)client_data;
	A_Axes_t			orient;
	A_BossWind_t		*bossWin=tileWin->parent->parent;
	Display				*dpy;
	XButtonPressedEvent *buttonEvent;
	int					i,x,y,nx,ny,newx,newy;
	int				  	xdim,ydim,scale,numtiles,index;

	orient=tileWin->axesOrient;
	buttonEvent = (XButtonPressedEvent *)call_data;
	x = buttonEvent->x;
	y = buttonEvent->y;

	numtiles = tileWin->numTiles;
	scale = bossWin->data->scale;
	xdim = td_HdfgetDim(bossWin->data,orient.col);
	ydim = td_HdfgetDim(bossWin->data,orient.row);

	nx = x/scale;
	ny = y/scale;
	x = nx*scale;
	y = ny*scale;
	index = ny*xdim+nx;

	if (tileWin->oldInd != -1)
		for (i=0;i<numtiles;i++)
			gr_ImageSetPixel(tileWin->imageWin[i],tileWin->oldX,
						 tileWin->oldY,scale,tileWin->oldPix[i]);
	if (tileWin->oldInd == index)
	{
		for (i=0;i<numtiles;i++)
			gr_ImageSetPixel(tileWin->imageWin[i],tileWin->oldX,
						 tileWin->oldY,scale,tileWin->oldPix[i]);
		tileWin->oldInd = -1;
	}
	else
	{
		tileWin->oldInd = index;
		tileWin->oldX = x;
		tileWin->oldY = y;
		for (i=0;i<numtiles;i++)
		{
			newx = x; newy = y;
			gr_VPortMoveChild(tileWin->imageVPort[i],
				&newx,&newy,xdim*scale,ydim*scale);
			dpy = XtDisplay(tileWin->imageWin[i]);
			tileWin->oldPix[i] = 
				XGetPixel(tileWin->image[i],x,y);
			gr_ImageSetPixel(tileWin->imageWin[i],x,y,scale,
					 WhitePixel(dpy,DefaultScreen(dpy)));
		}
	}
}
