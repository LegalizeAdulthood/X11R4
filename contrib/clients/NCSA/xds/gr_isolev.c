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
 *	File:		gr_isolev.c
 *	Contents:	Iso level window functions for graphics module
 */

#include "gr_com.h"

#define MINISOXSIZE	280
#define MINISOYSIZE	100
#define MAXISOXSIZE	900
#define MAXISOYSIZE	900
#define	LEEWAY		3
#define SPACING		20
#define ORIGIN		10


/*
 *	Return a iso-surface renderer window
 */
A_IsoWind_t
*gr_InitIsoLevel(header,parent,tparent,scale)
A_IsoWind_t	*header;
Widget parent;
A_BossWind_t	*tparent;
int				scale;
{
	A_IsoWind_t *tmp;
	Widget		boxWind,RCWind;
	int			winxsize, winysize;
	int			i,j;
	Display		*dpy;
	int			scr;
	XGCValues	gcvals;

	gr_WidgetCursor(tparent->shell,XC_watch);

	if ((tmp = (A_IsoWind_t *)td_Malloc(sizeof(A_IsoWind_t),
			   "A_IsoWind_t")) == NULL)
		return(NULL);

	tmp->curCursor = FALSE;
	tmp->scale = scale;
	tmp->xdim = td_HdfgetDim(tparent->data,1);
	tmp->ydim = td_HdfgetDim(tparent->data,2);
	tmp->zdim = td_HdfgetDim(tparent->data,3);
	tmp->xsize = tmp->xdim*scale+SPACING;
	tmp->ysize = tmp->ydim*scale+SPACING;

    if ((tmp->data=td_Malloc2DChar(tmp->xsize,tmp->ysize,"Iso raster"))==NULL)
        return(NULL);
    if ((tmp->fb=td_Malloc2DInt(tmp->xsize,tmp->ysize,"Iso fb data"))==NULL)
        return(NULL);

    if (tmp->xsize > MINISOXSIZE)
        if (tmp->xsize > MAXISOXSIZE)
            winxsize = MAXISOXSIZE;
        else
            winxsize = tmp->xsize;
    else
        winxsize = MINISOXSIZE;

    if (tmp->ysize > MINISOYSIZE)
        if (tmp->ysize > MAXISOYSIZE)
            winysize = MAXISOYSIZE;
        else
            winysize = tmp->ysize;
    else
        winysize = MINISOYSIZE;

	sprintf(msg,"%s: Iso-surface",tparent->filename);

	tmp->shell = gr_MakeWindow("XDS Iso-surface",parent,&(tmp->win),
								(XtCallbackProc)gr_CloseIsoLevel,
								ISOWIND,msg,"Close",(caddr_t)tmp,
								10,10,winxsize+4+LEEWAY,winysize+100+LEEWAY);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,
		2,2,winxsize+LEEWAY,winysize+LEEWAY);

	tmp->imageVPort = gr_MakeVPort("XDSvport",boxWind,
						NOSCROLL, HP, NULL, NULL, 5,(caddr_t)tmp,
						0,0,winxsize+LEEWAY,winysize+LEEWAY);

	tmp->imageWin = gr_MakeWorkSpace("XDSworkspace",tmp->imageVPort,
						(XtCallbackProc)gr_IsoExpose, NULL, NULL,
						(caddr_t)tmp,0,0,winxsize,winysize);

	gr_IsoInit(tmp);
    tmp->image = gr_ImageCreate(tmp->imageVPort,tmp->xsize,tmp->ysize,
        tmp->data[tmp->ysize-1]);

    boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,
			2,winysize+LEEWAY+5,winxsize,55);

    gr_MakeButton("XDSbutton",boxWind,"View",
					(XtCallbackProc)gr_IsoView,(caddr_t)tmp,0,0,50,25);

    gr_MakeButton("XDSbutton",boxWind,"Save",
					(XtCallbackProc)gr_SaveIso,(caddr_t)tmp,52,0,50,25);

    tmp->isoDialog = gr_MakeDialog("XDSdialog",boxWind,
			"Iso-Value","4",4,102,0,85,50);

    RCWind = gr_MakeRowColumn("XDSrowcolumn",boxWind,ONEOFMANY,1,190,0,
                        90,50);
            gr_MakeToggle("XDStoggle",RCWind,"XY Plane",DIAMOND,
                        TRUE,(XtCallbackProc)gr_IsoSetXY,
                        NULL,(caddr_t)tmp,
                        2,0,100,15);
            gr_MakeToggle("XDStoggle",RCWind,"XZ Plane",DIAMOND,
                        FALSE,(XtCallbackProc)gr_IsoSetXZ,
                        NULL,(caddr_t)tmp,
                        2,15,100,15);
            gr_MakeToggle("XDStoggle",RCWind,"YZ Plane",DIAMOND,
                        FALSE,(XtCallbackProc)gr_IsoSetYZ,
                        NULL,(caddr_t)tmp,
                        2,30,100,15);

	dpy = XtDisplay(tmp->imageWin);
	scr = DefaultScreen(dpy);
	gcvals.foreground = BlackPixel(dpy,scr);
	gcvals.background = WhitePixel(dpy,scr);
	gcvals.function   = GXcopy;
	tmp->gc = XtGetGC(tmp->imageWin, 
		GCForeground|GCBackground|GCFunction, &gcvals);

	tmp->axesOrient.row = 1;
	tmp->axesOrient.col = 2;
	tmp->axesOrient.axis = 3;
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
 *	Close a Iso-surface renderer window
 */
void
gr_CloseIsoLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_IsoWind_t	*isoWin=(A_IsoWind_t *)client_data;

	if (isoWin != NULL)
	{
        if (isoWin->prev != NULL)
            isoWin->prev->next = isoWin->next;
        else
            isoWin->parent->isoWin = isoWin->next;

        if (isoWin->next != NULL)
            isoWin->next->prev = isoWin->prev;

        isoWin->parent->numIsoWins--;

		XDestroyImage(isoWin->image);
		td_Free((char *)isoWin->image);
		td_Free((char *)isoWin->data);
		td_Free((int *)isoWin->fb);
		XtDestroyWidget(isoWin->shell);
		td_Free((char *)isoWin);
	}
}
