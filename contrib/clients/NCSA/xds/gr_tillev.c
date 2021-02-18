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
 *	File:		gr_tillev.c
 *	Contents:	Tile level window functions for graphics module
 */

#include "gr_com.h"

#define MAX_TILDSP_WIDTH	200
#define MAX_TILDSP_HEIGHT	200


/*
 *	Open a Frame Window, normal or unfolded
 */
A_TileWind_t
*gr_InitTileLevel(header,
	shellName,parent,numplanes,incr,scale,orient,tileType,tparent)
A_TileWind_t 	*header;
char			*shellName;
Widget	 		parent;
int		 		numplanes,incr,scale;
A_Axes_t 		orient;
A_Tile_t 		tileType;
A_CubeWind_t	*tparent;
{
	A_BossWind_t	*bossWin=tparent->parent;
	A_TileWind_t	*tmp;
	Widget		boxWind;
	A_Axes_t	useOrient;
	int			xsize,ysize,atX,atY,usePlane;
	short		i,k;
	char		label[STRNG40];
	char		title[STRNG40];

	gr_WidgetCursor(tparent->shell,XC_watch);

	if ((tmp = (A_TileWind_t *)td_Malloc(sizeof(A_TileWind_t),
			   "A_TileWind_t")) == NULL)
		return(NULL);

	td_HdfsetScale(bossWin->data,scale);

	if (numplanes > MAX_TILLEV_TILES)
		numplanes = MAX_TILLEV_TILES;

	switch(tileType)
	{
		case AUTO:
			sprintf(label,"%s: %d planes along %s-axis",
			bossWin->filename,numplanes, axesLabels[orient.axis-1]);
			break;
		case UNFOLDED:
			sprintf(label,"%s: %d unfolded planes from %d",
			bossWin->filename,numplanes,orient.plane);
			break;
	}

	if (numplanes > 5)
	{
		xsize = (MAX_TILDSP_WIDTH+5)*5;
		if (numplanes > 10)
			ysize = (MAX_TILDSP_HEIGHT+25)*3;
		else
			ysize = (MAX_TILDSP_HEIGHT+25)*2;
	}
	else
	{
		xsize = (MAX_TILDSP_WIDTH+5)*numplanes;
		ysize = (MAX_TILDSP_HEIGHT+25);
	}

	tmp->shell = gr_MakeWindow("XDS Frame",parent,&(tmp->win),
					(XtCallbackProc)gr_CloseTileLevel,
					TILEWIND,label,"Close",(caddr_t)tmp,
					110,10,xsize+4,ysize+80);

	boxWind = gr_MakeBox("XDSbox",tmp->win,2,ysize+5,xsize,40);
		gr_MakeButton("XDSbutton",boxWind,"Palette",
					  (XtCallbackProc)gr_LoadPAL,(caddr_t)tmp->shell,
					  2,2,80,30);
		gr_MakeButton("XDSbutton",boxWind,"Save",
					  (XtCallbackProc)gr_SaveTile,(caddr_t)tmp,
					  82,2,80,30);

	k = 0; atX = 2; atY = 2;
	useOrient = orient;
	if (tileType == UNFOLDED)
	{
		useOrient.row = 2;
		useOrient.col = 1;
		useOrient.axis= 3;
	}
	for (i=0;i<numplanes;i++)
	{
		gr_ImageSetCMap(tmp->shell);

		if (tileType == AUTO)
			usePlane = useOrient.plane+(i*incr);
		else
			usePlane = useOrient.plane+(k*incr);

		if (k > 4)
		{
			atX = 2;
			atY = atY+(MAX_TILDSP_HEIGHT+25);
			k = 0;
			if ((tileType == UNFOLDED) && (useOrient.axis==3))
			{
				useOrient.row = -3;
				useOrient.col = 1;
				useOrient.axis= 2;
				usePlane = useOrient.plane;
			}
			else
			if ((tileType == UNFOLDED) && (useOrient.axis==2))
			{
				useOrient.row = 2;
				useOrient.col = 3;
				useOrient.axis= 1;
				usePlane = useOrient.plane;
			}
		}

		tmp->data[i] = td_HdfgetPixData(bossWin->data,
						useOrient.row,useOrient.col,useOrient.axis,
						usePlane,tparent->interp,
						(unsigned char)gr_color.nColors);

		if (tparent->interp == FALSE)
		{
			tmp->imagexsize[i] =
				td_HdfgetDim(bossWin->data,useOrient.col)*scale;
			tmp->imageysize[i] = 
				td_HdfgetDim(bossWin->data,useOrient.row)*scale;
		}
		else
		{
			tmp->imagexsize[i] = 
				(td_HdfgetDim(bossWin->data,useOrient.col)-1)*scale;
			tmp->imageysize[i] = 
				(td_HdfgetDim(bossWin->data,useOrient.row)-1)*scale;
		}

		sprintf(title,"Plane %d along %s-axis",
				usePlane, axesLabels[useOrient.axis-1]);
		gr_MakeTitleBar("XDSlabel",tmp->win,title,
						atX,atY,MAX_TILDSP_WIDTH,20);
		tmp->imageVPort[i] = gr_MakeVPort("XDSvport",tmp->win, NOSCROLL, HP,
			NULL, NULL,5,(caddr_t)tmp,
			atX,atY+20,MAX_TILDSP_WIDTH,MAX_TILDSP_HEIGHT);
		tmp->image[i] = 
			gr_ImageCreate(tmp->imageVPort[i],tmp->imagexsize[i],
							tmp->imageysize[i],tmp->data[i]);

		tmp->imageWin[i] = gr_MakeImageStatic("XDSimage",
							tmp->imageVPort[i],
							tmp->image[i],
							(XtCallbackProc)gr_TileSelect,(caddr_t)tmp,
							1,1,tmp->imagexsize[i],tmp->imageysize[i]);

		atX = atX + (MAX_TILDSP_WIDTH+5);
		k++;
	}

	tmp->numTiles = numplanes;
	tmp->oldInd = -1;
	tmp->oldX = tmp->oldY = 0;
	for (i=numplanes;i<MAX_TILLEV_TILES;i++)
	{
		tmp->image[i] = NULL;
		tmp->data[i] = NULL;
		tmp->oldPix[i] = (Pixel)0;
	}
	tmp->axesOrient = orient;
	tmp->tileType = tileType;

	tmp->parent = tparent;
	tmp->prev	= NULL;
	tmp->next	= header;
	if (header != NULL)
		header->prev = tmp;

	gr_WidgetCursor(tmp->shell,XC_cross);
	gr_WidgetCursor(tparent->shell,XC_cross);

	return(tmp);
}

void
gr_CloseTileLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_TileWind_t	*tileWin=(A_TileWind_t *)client_data;
	short i;

	if (tileWin != NULL)
	{
        if (tileWin->prev != NULL)
            tileWin->prev->next = tileWin->next;
        else
            tileWin->parent->tileWin = tileWin->next;

        if (tileWin->next != NULL)
            tileWin->next->prev = tileWin->prev;

        tileWin->parent->numTileWins--;

		for (i=0;i<tileWin->numTiles;i++)
		{
				XDestroyImage(tileWin->image[i]);
				td_Free((char *)tileWin->data[i]);
		}

		td_Free((char *)tileWin);

		XtDestroyWidget(tileWin->shell);
	}
}
