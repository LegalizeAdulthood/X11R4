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
 *	File:		gr_subslev.c
 *	Contents:	Substance level window functions for graphics module
 */

#include "gr_com.h"

#define MINSUBSXSIZE	315
#define MAXSUBSXSIZE	900
#define MINSUBSYSIZE	50
#define MAXSUBSYSIZE	900
#define LEEWAY			3


/*
 *	Return a V-Buffer window
 */
A_SubsWind_t
*gr_InitSubsLevel(header,shellName,parent,tparent)
A_SubsWind_t *header;
char   	 *shellName;
Widget	 parent;
A_CubeWind_t	*tparent;
{
	A_BossWind_t	*bossWin=tparent->parent;
	A_SubsWind_t	*tmp;
	Widget			boxWind;
	XImage			*palImage;
	char			label[STRNG80];
	int				i,xdim,ydim,winxsize,winysize;

	gr_WidgetCursor(tparent->shell,XC_watch);

	sprintf(label,"%s: VBuffer",tparent->parent->filename);

	if ((tmp = (A_SubsWind_t *)td_Malloc(sizeof(A_SubsWind_t),
			   "A_SubsWind_t")) == NULL)
		return(NULL);

	tmp->curCursor = FALSE;
	tmp->axesOrient = tparent->axesOrient;
	tmp->numSubs = atoi(gr_DialogGetValue(tparent->numSubsDialog));
	tmp->xpts = atoi(gr_DialogGetValue(tparent->xptDialog));
	tmp->ypts = atoi(gr_DialogGetValue(tparent->yptDialog));
	tmp->zpts = atoi(gr_DialogGetValue(tparent->zptDialog));
	tmp->hdf = tparent->parent->data;

	if ((tmp->subs = 
		 (A_SubsType_t *)td_Malloc1D(1,tmp->numSubs,sizeof(A_SubsType_t),
		 "Malloc subs array")) == NULL)
		return(NULL);

	for (i=0;i<tmp->numSubs;i++)
		tmp->subs[i].lower = tmp->subs[i].upper = tmp->subs[i].opacity = 0.0;

	xdim = td_HdfgetDim(bossWin->data,tmp->axesOrient.col);
	ydim = td_HdfgetDim(bossWin->data,tmp->axesOrient.row);

	tmp->xsize = (xdim-1)*tmp->xpts;
	tmp->ysize = (ydim-1)*tmp->ypts;

	if (tmp->xsize > MINSUBSXSIZE)
		if (tmp->xsize > MAXSUBSXSIZE)
			winxsize = MAXSUBSXSIZE;
		else
			winxsize = tmp->xsize;
	else
		winxsize = MINSUBSXSIZE;

	if (tmp->ysize > MINSUBSYSIZE)
		if (tmp->ysize > MAXSUBSYSIZE)
			winysize = MAXSUBSYSIZE;
		else
			winysize = tmp->ysize;
	else
		winysize = MINSUBSYSIZE;

	if ((tmp->data = td_Malloc1D(tmp->xsize, tmp->ysize,
		(long)sizeof(unsigned char),"VBuff raster")) == NULL)
		return(NULL);

	if ((tmp->vbuff = td_Malloc2DVRect(xdim,ydim,"VBuff raster")) == NULL)
		return(NULL);

	tmp->shell = gr_MakeWindow("XDSsubstanceWind",parent,&(tmp->win),
					(XtCallbackProc)gr_CloseSubsLevel,
					SUBSWIND,label,"Close",(caddr_t)tmp,
					110,10,winxsize+12,winysize+250+LEEWAY);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,2,2,316,60);
	tmp->IDLabel = gr_MakeTitleBar("XDSlabel",boxWind,"Substance  1",
		2,2,100,25);
	gr_MakeButton("XDSbutton",boxWind,"Set",
         (XtCallbackProc)gr_SubsSetID,(caddr_t)tmp,2,30,100,25);
    tmp->IDSlider =
        gr_MakeSlider("SDSslider",boxWind,0,tmp->numSubs,1,0,1,
            (XtCallbackProc)gr_SubsIDSliderSel,
            (XtCallbackProc)gr_SubsIDSliderMov,
            (XtCallbackProc)gr_SubsIDSliderRel,
            VERTONLY, (caddr_t)tmp,106,2,25,55);
    tmp->lowerDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Lower  ","0.00",5,135,2,60,50);
    tmp->upperDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Upper  ","0.00",5,195,2,60,50);
    tmp->opacityDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Opacity","0.00",5,255,2,60,50);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,2,64,100,108);
	gr_MakeButton("XDSbutton",boxWind,"Palette",
         (XtCallbackProc)gr_LoadPAL,(caddr_t)tmp->shell,2,2,90,20);
	gr_MakeButton("XDSbutton",boxWind,"Save",
         (XtCallbackProc)gr_SaveVbuff,(caddr_t)tmp,2,24,90,20);
	gr_MakeButton("XDSbutton",boxWind,"Reset",
         (XtCallbackProc)gr_SubsReset,(caddr_t)tmp,2,46,90,20);
	tmp->runButton = gr_MakeButton("XDSbutton",boxWind,"Start",
         (XtCallbackProc)gr_SubsStart,(caddr_t)tmp,2,68,90,20);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,106,64,212,108);
    tmp->attDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Atten","0.80",5,2,2,50,50);
    tmp->gammaDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Gamma","1.00",5,54,2,50,50);
    tmp->ambDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Amb  ","0.50",5,106,2,50,50);
    tmp->incrDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Incr ","1",5,158,2,50,50);
    tmp->nearDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Near ","0",5,2,54,50,50);
    tmp->farDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Far  ","0",5,54,54,50,50);
    tmp->maxDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Max. Intensity",
			 "0.0000000000",5,106,54,102,50);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,2,177,316,35);
    palImage = gr_ImageCreate(boxWind,gr_color.maxColors,30,gr_palData);
    gr_MakeImageStatic("XDSstaticimage",boxWind,palImage,NULL,
                (caddr_t)NULL,25,2,256,30);

	tmp->imageVPort = gr_MakeVPort("XDSVPort",tmp->win, NOSCROLL, HP,
		NULL, NULL, 5,(caddr_t)tmp,
		2,217,winxsize+LEEWAY,winysize+LEEWAY);
	tmp->imageWin = gr_MakeWorkSpace("XDSworkspace",tmp->imageVPort,
		(XtCallbackProc)gr_SubsExpose, NULL, NULL,
		(caddr_t)tmp,0,0,winxsize,winysize);

	tmp->image = gr_ImageCreate(tmp->imageVPort,tmp->xsize,tmp->ysize,
		tmp->data);

	tmp->curSubsID = 0;
	tmp->curP	= 0;
	tmp->runMode= 0;	/* reset */
	tmp->parent	= tparent;
	tmp->prev	= NULL;
	tmp->next	= header;
	if (header != NULL)
		header->prev = tmp;

	gr_WidgetCursor(tmp->shell,XC_draped_box);
	gr_WidgetCursor(tparent->shell,XC_draped_box);

	return(tmp);
}


/*
 *	Close a V-Buffer Window
 */
void
gr_CloseSubsLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_SubsWind_t *subsWin=(A_SubsWind_t *)client_data;
	short i;

	if (subsWin != NULL)
	{
		if (subsWin->prev != NULL)
			subsWin->prev->next = subsWin->next;
		else
			subsWin->parent->subsWin = subsWin->next;

		if (subsWin->next != NULL)
			subsWin->next->prev = subsWin->prev;

		subsWin->parent->numSubsWins--;

		XDestroyImage(subsWin->image);
		td_Free((char *)subsWin->image);
		td_Free((char *)subsWin->data);
		td_Free2dVRect(subsWin->vbuff);
		XtDestroyWidget(subsWin->shell);
		td_Free((char *)subsWin);
	}
}
