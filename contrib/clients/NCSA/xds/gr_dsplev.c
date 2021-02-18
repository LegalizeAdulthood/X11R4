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
 *	File:		gr_dsplev.c
 *	Contents:	Dspl level window functions for graphics module
 */

#include "gr_com.h"

#define	MINDSPLXSIZE	110
#define	MINDSPLYSIZE	350
#define	MAXDSPLXSIZE	600
#define	MAXDSPLYSIZE	900
#define	LEEWAY			3


/*
 *	Return a Cartesian Single plane window with data spreadsheet
 */
A_DsplWind_t
*gr_InitDsplLevel(header,shellName,parent,orient,scale,width,tparent)
A_DsplWind_t	*header;
char   			*shellName;
Widget 			parent;
A_Axes_t		orient;
int				scale,width;
A_CubeWind_t	*tparent;
{
	A_BossWind_t	*bossWin=tparent->parent;
	A_Data_t		*hdf=bossWin->data;
	A_DsplWind_t	*tmp;
	Widget		boxWind;
	char		label[STRNG40],axes[STRNG40];
	int			numCols;
	int			winxsize,winysize,xsize,ysize,xaxis,yaxis;
	XImage 		*palImage;

	gr_WidgetCursor(tparent->shell,XC_watch);

	if ((tmp = (A_DsplWind_t *)td_Malloc(sizeof(A_DsplWind_t),
			   "A_DsplWind_t")) == NULL)
		return(NULL);

	td_HdfsetScale(hdf,scale);
	tmp->width = width;
    tmp->xdim = td_HdfgetDim(hdf,orient.col);
	tmp->ydim = td_HdfgetDim(hdf,orient.row);
	tmp->zdim = td_HdfgetDim(hdf,orient.axis);


	sprintf(label,"%s: Plane %d along %s-axis",bossWin->filename,
			orient.plane,axesLabels[orient.axis -1]);

	if (orient.col < 0) xaxis = -orient.col;
	else xaxis = orient.col;
	if (orient.row < 0) yaxis = -orient.row;
	else yaxis = orient.row;

	sprintf(axes,"%s %s",axesLabels[yaxis-1],axesLabels[xaxis-1]);

	numCols	= tmp->xdim;

	gr_WidgetCursor(tparent->shell,XC_watch);
	if ((tmp->hScaleData = td_HdfgetHScale(hdf,orient.col)) == NULL)
		return(NULL);
	if ((tmp->vScaleData = td_HdfgetVScale(hdf,orient.row)) == NULL)
		return(NULL);
	if ((tmp->listData = td_HdfgetPlaneData(hdf,
				orient.row,orient.col,orient.axis,orient.plane)) == NULL)
		return(NULL);

	if ((tmp->data = td_HdfgetPixData(hdf,
					orient.row,orient.col,orient.axis,orient.plane,
					tparent->interp,
					(unsigned char)gr_color.nColors )) == NULL)
		return(NULL);

	if (tparent->interp == FALSE)
	{
		xsize = tmp->imagexsize	= tmp->xdim*scale;
		ysize = tmp->imageysize	= tmp->ydim*scale;
	}
	else
	{
		xsize = tmp->imagexsize	= 
				(tmp->xdim-1)*scale;
		ysize = tmp->imageysize	= 
				(tmp->ydim-1)*scale;
	}

    if (xsize > MINDSPLXSIZE)
        if (xsize > MAXDSPLXSIZE)
            winxsize = MAXDSPLXSIZE;
        else
            winxsize = xsize+LEEWAY;
    else
        winxsize = MINDSPLXSIZE;

    if (ysize > MINDSPLYSIZE)
        if (ysize > MAXDSPLYSIZE)
            winysize = MAXDSPLYSIZE;
        else
            winysize = ysize+LEEWAY;
    else
        winysize = MINDSPLYSIZE;

	tmp->shell = gr_MakeWindow("XDS Cartesian Single Plane",parent,&(tmp->win),
					(XtCallbackProc)gr_CloseDsplLevel,
					DSPLWIND,label,"Close",(caddr_t)tmp,
					110,10,winxsize+414,winysize+40);

	gr_MakeTitleBar("XDSlabel",tmp->win,axes,1,1,47,20);
	tmp->hScaleWin = 
		gr_MakeListStatic("XDSstaticlist",tmp->win,&(tmp->hScaleVPort),
					numCols,NULL,tmp->hScaleData,(caddr_t)tmp,
					51,1,319,20);
	tmp->vScaleWin = 
		gr_MakeListStatic("XDSstaticlist",tmp->win,&(tmp->vScaleVPort),
					1,NULL,tmp->vScaleData,(caddr_t)tmp,
					1,26,45,245);
	tmp->listWin =
		gr_MakeList("XDSlist",tmp->win,&(tmp->listVPort),
					VERTHORIZ,numCols,5,
					(XtCallbackProc)gr_DsplListVScroll,
					(XtCallbackProc)gr_DsplListHScroll,
					(XtCallbackProc)gr_DsplListSelect,
					tmp->listData,(caddr_t)tmp,
					51, 26, 345, 272);

	gr_ImageSetCMap(tmp->shell);

	tmp->imageVPort = gr_MakeVPort("XDSvport",tmp->win,NOSCROLL,HP,
						NULL,NULL,5,(caddr_t)tmp,
						400,1,winxsize,winysize);

	tmp->image = gr_ImageCreate(tmp->imageVPort,xsize,ysize,
						tmp->data);

	tmp->imageWin = gr_MakeImageStatic("XDSstaticimage",
							tmp->imageVPort,tmp->image,
							(XtCallbackProc)gr_DsplImageSelect,(caddr_t)tmp,
							0,0,xsize,ysize);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,2,304,265,50);
	palImage = gr_ImageCreate(boxWind,gr_color.maxColors,30,gr_palData);
	gr_MakeImageStatic("XDSstaticimage",boxWind,palImage,
						NULL,(caddr_t)NULL,
					   1,10,256,30);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,270,304,120,50);
			gr_MakeToggle("XDStoggle",boxWind,"Sync",SQUARE,
						FALSE,(XtCallbackProc)gr_DsplSetSynch,
						(XtCallbackProc)gr_DsplUnSetSynch,(caddr_t)tmp,
						2,2,50,20);
			gr_MakeButton("XDSbutton",boxWind,"Profile",
					  	(XtCallbackProc)gr_DsplHistogram,(caddr_t)tmp,
						60,2,50,20);
			gr_MakeButton("XDSbutton",boxWind,"Palette",
					  	(XtCallbackProc)gr_LoadPAL,(caddr_t)tmp->shell,
						2,25,50,20);
			gr_MakeButton("XDSbutton",boxWind,"Save",
					  	(XtCallbackProc)gr_SaveDspl,(caddr_t)tmp,
						60,25,50,20);

	tmp->numHistWins = 0;
	tmp->histWin = NULL;
	tmp->oldInd = -1;
	tmp->selX = tmp->selY = 0;
	tmp->oldX = tmp->oldY = 0;
	tmp->oldPix = (Pixel)0;
	tmp->axesOrient = orient;
	tmp->synchronize = FALSE;

	tmp->parent = tparent;
	tmp->prev = NULL;
	tmp->next = header;
	if (header != NULL)
		header->prev = tmp;

	gr_WidgetCursor(tmp->shell,XC_draped_box);
	gr_WidgetCursor(tparent->shell,XC_draped_box);

	return(tmp);
}


/*
 * Close a Cartesian Single plane window
 */
void
gr_CloseDsplLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t	*dsplWin=(A_DsplWind_t *)client_data;

	if (dsplWin != NULL)
	{
		while (dsplWin->numHistWins > 0)
			gr_CloseHistLevel(dsplWin->histWin->shell,
				(caddr_t)dsplWin->histWin,(caddr_t)NULL);

		if (dsplWin->prev != NULL)
			dsplWin->prev->next = dsplWin->next;
		else
			dsplWin->parent->dsplWin = dsplWin->next;
		
		if (dsplWin->next != NULL)
			dsplWin->next->prev = dsplWin->prev;

		dsplWin->parent->numDsplWins--;

		XDestroyImage(dsplWin->image);
		td_Free((char *)dsplWin->image);
		td_Free((char *)dsplWin->data);
		td_Free2d((char **)dsplWin->listData);
		td_Free2d((char **)dsplWin->vScaleData);
		td_Free2d((char **)dsplWin->hScaleData);
		td_Free((char *)dsplWin);

		XtDestroyWidget(dsplWin->shell);
	}
}
