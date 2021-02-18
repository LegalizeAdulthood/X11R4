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
 *	File:		gr_dspl.c
 *	Contents:	routines for dspl window
 */

#include "gr_com.h"


/*
 * Sets a 'pixel' box of size scale and color pix at the (x,y) coordinates.
 */
void
gr_ImageSetPixel(w,x,y,scale,pix)
Widget w;
int    x,y,scale;
Pixel  pix;
{
	Display		*dpy=XtDisplay(w);
	Drawable	win=XtWindow(w);
	XGCValues	values;
	GC          drawGC;

	values.foreground = pix;
	drawGC = XtGetGC(w,GCForeground,&values);
	if (scale == 1)
		scale = 2;
	XDrawRectangle(dpy,win,drawGC,x,y,scale-1,scale-1);
}


/*
 * Called by gr_DsplImageSelect to highlight the selected pixel position
 * (when mouse is clicked on image) and highlight the corresponding
 * entry in the data window.
 */
void
gr_DsplImageSetPixel(dsplWin, call_data)
A_DsplWind_t *dsplWin;
caddr_t		 call_data;
{
	A_BossWind_t *bossWin=dsplWin->parent->parent;
	A_Axes_t	 orient;
	Display 	 *dpy=XtDisplay(dsplWin->imageWin);
	XButtonPressedEvent *buttonEvent=(XButtonPressedEvent *)call_data;
	int					x,y,nx,ny;
	int				  	xdim,ydim,scale,index;

	orient = dsplWin->axesOrient;
	x = buttonEvent->x;
	y = buttonEvent->y;

	scale = bossWin->data->scale;
	xdim = dsplWin->xdim;
	ydim = dsplWin->ydim;

	nx = x/scale;
	ny = y/scale;
	x = nx*scale;
	y = ny*scale;
	index = ny*xdim+nx;

	gr_ListHighlight(dsplWin->listWin,index);
	gr_ListHighlight(dsplWin->hScaleWin,nx);
	gr_ListHighlight(dsplWin->vScaleWin,ny);

	if (orient.col < 0) dsplWin->selX = dsplWin->xdim-nx-1;
	else	dsplWin->selX = nx;
	if (orient.row > 0) dsplWin->selY = dsplWin->ydim-ny-1;
	else	dsplWin->selY = ny;

	/* turn off the old pixel highlight box */
	if (dsplWin->oldInd != -1)
		gr_ImageSetPixel(dsplWin->imageWin,
			dsplWin->oldX,dsplWin->oldY,scale,dsplWin->oldPix);

	if (dsplWin->oldInd == index)
	{ 
		/* turn off the old pixel highlight box if previously highlited */
		gr_ImageSetPixel(dsplWin->imageWin,
			dsplWin->oldX,dsplWin->oldY,scale,dsplWin->oldPix);
		dsplWin->oldInd = -1;
	}
	else
	{
		/* turn on the old pixel highlight box */
		dsplWin->oldInd = index;
		dsplWin->oldX = x;
		dsplWin->oldY = y;
		dsplWin->oldPix = XGetPixel(dsplWin->image,x,y);
		gr_ImageSetPixel(dsplWin->imageWin,x,y,scale,
						WhitePixel(dpy,DefaultScreen(dpy)));
	}

	/* scroll the data window and update scales */
	gr_VPortMoveChild(dsplWin->listVPort,&nx,&ny,xdim,ydim);
	gr_DsplListVScroll(dsplWin->listVPort,(caddr_t)dsplWin,(caddr_t)ny);
	gr_DsplListHScroll(dsplWin->listVPort,(caddr_t)dsplWin,(caddr_t)nx);
}


/*
 * Called when mouse is clicked in image window.  Updates all
 * synchronized dspl windows.
 */
void
gr_DsplImageSelect(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t *dsplWin = (A_DsplWind_t *)client_data;
	A_BossWind_t *curboss = gr_topWin.bossWin;
	A_CubeWind_t *curcube;
	A_DsplWind_t *curdspl;

	if (dsplWin->synchronize == FALSE)
		gr_DsplImageSetPixel(dsplWin, call_data);
	else
	while (curboss != NULL)
	{
		curcube = curboss->cubeWin;
		while (curcube != NULL)
		{
			curdspl = curcube->dsplWin;
			while (curdspl != NULL)
			{
				if ((curdspl == dsplWin) || (curdspl->synchronize == TRUE))
					gr_DsplImageSetPixel(curdspl, call_data);

				curdspl = curdspl->next;
			}
			curcube = curcube->next;
		}
		curboss = curboss->next;
	}
}


/*
 * Called by gr_DsplListSelect to highlight and scroll the data window
 * in the dspl window together with the scales and also highlight the
 * corresponding raster image point.
 */
void
gr_DsplListSet(dsplWin, index, highlight)
A_DsplWind_t	*dsplWin;
int				index;
Boolean			highlight;
{
	A_BossWind_t	*bossWin=dsplWin->parent->parent;
	A_Axes_t	 	orient;
	Display	*dpy=XtDisplay(dsplWin->imageWin);
	int		xlines,ylines;
	int		scale;
	int		newx,newy,x,y,xdim,ydim;
	short	tile;

	orient= dsplWin->axesOrient;
	scale = bossWin->data->scale;

	xdim = dsplWin->xdim;
	ydim = dsplWin->ydim;

	ylines = index/xdim;
	xlines = index - (ylines*xdim);

	newx = x = xlines*scale;
	newy = y = ylines*scale;

	gr_VPortMoveChild(dsplWin->listVPort,&newx,&newy,
		xdim*scale,ydim*scale);
	gr_DsplListVScroll(dsplWin->listVPort,(caddr_t)dsplWin,(caddr_t)newy);
	gr_DsplListHScroll(dsplWin->listVPort,(caddr_t)dsplWin,(caddr_t)newx);

	/* turn off previous highlight in raster image */
	if (dsplWin->oldInd != -1)
		gr_ImageSetPixel(dsplWin->imageWin,dsplWin->oldX,
			dsplWin->oldY,scale,dsplWin->oldPix);

	if (dsplWin->oldInd == index)
	{
		gr_ImageSetPixel(dsplWin->imageWin,dsplWin->oldX,
			dsplWin->oldY,scale,dsplWin->oldPix);
		dsplWin->oldInd = -1;
	}
	else
	{
		dsplWin->oldInd = index;
		dsplWin->oldX = x;
		dsplWin->oldY = y;
		dsplWin->oldPix = XGetPixel(dsplWin->image,x,y);
		gr_ImageSetPixel(dsplWin->imageWin,x,y,scale,
						 WhitePixel(dpy,DefaultScreen(dpy)));
	}

	if (highlight == TRUE)
		gr_ListHighlight(dsplWin->listWin,index);

	gr_ListHighlight(dsplWin->hScaleWin,xlines);
	gr_ListHighlight(dsplWin->vScaleWin,ylines);

	if (orient.col < 0) dsplWin->selX = dsplWin->xdim-xlines-1;
	else	dsplWin->selX = xlines;
	if (orient.row > 0) dsplWin->selY = dsplWin->ydim-ylines-1;
	else	dsplWin->selY = ylines;
}


/*
 * Called when a data item is selected in the data window.  Updates
 * all synchronized dspl windows.
 */
void
gr_DsplListSelect(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t *dsplWin = (A_DsplWind_t *)client_data;
	A_BossWind_t *curboss = gr_topWin.bossWin;
	A_CubeWind_t *curcube;
	A_DsplWind_t *curdspl;
	char *strng;
	int	 index;

	strng = gr_ListgetStruct(dsplWin->listWin,&index);

	if (dsplWin->synchronize == FALSE)
		gr_DsplListSet(dsplWin, index, FALSE); /* no need to highlite */
	else
	while (curboss != NULL)
	{
		curcube = curboss->cubeWin;
		while (curcube != NULL)
		{
			curdspl = curcube->dsplWin;
			while (curdspl != NULL)
			{
				/* if same window, then no need to re-highlight */
				if (curdspl == dsplWin)
					gr_DsplListSet(curdspl, index, FALSE);
				else
				if (curdspl->synchronize == TRUE)
					gr_DsplListSet(curdspl, index, TRUE);

				curdspl = curdspl->next;
			}
			curcube = curcube->next;
		}
		curboss = curboss->next;
	}
}


/*
 * Scroll the Vertical scale when either the vertical scroll bar of
 * the data window is clicked or when the data window is moved
 */
void
gr_DsplListVScroll(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t	*dsplWin=(A_DsplWind_t *)client_data;

	gr_VPortVertMove(dsplWin->vScaleVPort,(int)call_data);
}


/*
 * Scroll the Horizontal scale when either the horizontal scroll bar of
 * the data window is clicked or when the data window is moved
 */
void
gr_DsplListHScroll(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t	*dsplWin=(A_DsplWind_t *)client_data;


	gr_VPortHoriMove(dsplWin->hScaleVPort,(int)call_data);
}


/*
 * Turn on synchronization
 */
void
gr_DsplSetSynch(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t	*dsplWin=(A_DsplWind_t *)client_data;

	dsplWin->synchronize = TRUE;
}


/*
 * Turn off synchronization
 */
void
gr_DsplUnSetSynch(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t	*dsplWin=(A_DsplWind_t *)client_data;

	dsplWin->synchronize = FALSE;
}


/*
 * Plot Histogram window
 */
void
gr_DsplHistogram(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t	*dsplWin=(A_DsplWind_t *)client_data;
	A_HistWind_t	*tmp=dsplWin->histWin;

    if (dsplWin->numHistWins < MAX_HISTLEV_WINDS)
    {
        tmp = gr_InitHistLevel(tmp,"XTDwindow",gr_topLevel,dsplWin);

        if (tmp != NULL)
        {
            dsplWin->histWin = tmp;
            dsplWin->numHistWins++;
        }
    }
    else
    {
        sprintf(msg,"Only %d histogram windows are allowed!\n",
            MAX_TILLEV_WINDS);
        gr_TextMsgOut(msg);
    }
    return;
}
