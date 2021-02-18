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
 *	File:		gr_arblev.c
 *	Contents:	Arbitrary level window functions for graphics module
 */

#include "gr_com.h"


/*
 * Return an arbitrary display mode window
 */
A_ArbWind_t
*gr_InitArbLevel(header,parent,tparent)
A_ArbWind_t	*header;
Widget parent;
A_BossWind_t	*tparent;
{
	A_ArbWind_t *tmp;
	Widget		boxWind;
	char		label[STRNG40];

	gr_WidgetCursor(tparent->shell,XC_watch);

	if ((tmp = (A_ArbWind_t *)td_Malloc(sizeof(A_ArbWind_t),
			   "A_ArbWind_t")) == NULL)
		return(NULL);

	if ((tmp->xybox = gr_CreateBox(
					td_HdfgetDim(tparent->data,1),
					td_HdfgetDim(tparent->data,2),
					td_HdfgetDim(tparent->data,3))) == NULL)
		return(NULL);

	sprintf(label,"%s Arbitrary Display Mode",tparent->filename);

	tmp->shell = gr_MakeWindow("XDS Arbitrary Display Mode",parent,&(tmp->win),
				(XtCallbackProc)gr_CloseArbLevel,
				ARBWIND,label,"Close",(caddr_t)tmp,10,224,440,410);

	gr_MakeTitleBar("XDSlabel",tmp->win,"Front View",2,2,200,20);
	gr_MakeTitleBar("XDSlabel",tmp->win,"Left Side View",228,2,200,20);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,2,22,200,220);

	tmp->drawCubeWin = gr_MakeWorkSpace("XDSworkspace",boxWind,
						(XtCallbackProc)gr_ArbCubeExpose,NULL,NULL,
						(caddr_t)tmp,
						0,0,200,200);
	tmp->xSlider = gr_MakeSlider("XDSslider",boxWind,0,361,1,0,1,
						(XtCallbackProc)gr_ArbXSliderSel,
						(XtCallbackProc)gr_ArbXSliderMov,
						(XtCallbackProc)gr_ArbXSliderRel,
						HORIZONLY,(caddr_t)tmp,0,200,200,18);
	tmp->ySlider = gr_MakeSlider("XDSslider",tmp->win,0,361,1,0,1,
						(XtCallbackProc)gr_ArbYSliderSel,
						(XtCallbackProc)gr_ArbYSliderMov,
						(XtCallbackProc)gr_ArbYSliderRel,
						VERTONLY,(caddr_t)tmp,206,22,18,200);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,228,22,200,220);

	tmp->drawDepthWin = gr_MakeWorkSpace("XDSworkspace",boxWind,
						(XtCallbackProc)gr_ArbDepthExpose,NULL,NULL,
						(caddr_t)tmp,
						0,0,200,200);

	tmp->zSlider = gr_MakeSlider("XDSslider",boxWind,0,100,1,0,1,
						(XtCallbackProc)gr_ArbZSliderSel,
						(XtCallbackProc)gr_ArbZSliderMov,
						(XtCallbackProc)gr_ArbZSliderRel,
						HORIZONLY,(caddr_t)tmp,0,200,200,18);

    boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,2,252,280,60);
        tmp->scaleDialog =
            gr_MakeDialog("XDSdialog",boxWind,"View Scale  ","1",2,
                        2,2,90,50);
        tmp->numFramesDialog =
            gr_MakeDialog("XDSdialog",boxWind,"Num Frames  ","5",2,
                        92,2,90,50);
        tmp->zpercentDialog =
            gr_MakeDialog("XDSdialog",boxWind,"Percent Incr","10.0",6,
                        182,2,90,50);
    boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,282,252,100,60);
            gr_MakeToggle("XDStoggle",boxWind,"Interpolated",SQUARE,
                        FALSE,(XtCallbackProc)gr_ArbSetInterp,
                        (XtCallbackProc)gr_ArbSetNonInterp,
						(caddr_t)tmp,2,2,100,20);
            gr_MakeToggle("XDStoggle",boxWind,"Use Pixmaps",SQUARE,
                        TRUE,(XtCallbackProc)gr_ArbUsePixmap,
                        (XtCallbackProc)gr_ArbUseXImage,
						(caddr_t)tmp,2,20,100,20);
            gr_MakeToggle("XDStoggle",boxWind,"Use Disk",SQUARE,
                        FALSE,(XtCallbackProc)gr_ArbUseDisk,
                        (XtCallbackProc)gr_ArbUseMemory,
						(caddr_t)tmp,2,40,100,20);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,2,316,400,35);
		gr_MakeButton("XDSbutton",boxWind,"View",
					(XtCallbackProc)gr_ArbViewOpen,(caddr_t)tmp,
					2,2,100,30);
		gr_MakeButton("XDSbutton",boxWind,"Animate",
					(XtCallbackProc)gr_ArbAniOpen,(caddr_t)tmp,
					102,2,100,30);

	tmp->xybox->numCuts = 0;
	tmp->xybox->cutVList = NULL;
	tmp->xybox->cutEList = NULL;
	tmp->viewWin = NULL;
	tmp->aniWin = NULL;
	tmp->numViewWins = 0;
	tmp->numAniWins = 0;
	tmp->interp = FALSE;
	tmp->useDisk = FALSE;

/* Cannot use pixmaps on the Iris */
#ifdef IRIS
	tmp->usePixmap = FALSE;
#else
	tmp->usePixmap = TRUE;
#endif

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
 * Close and remove an Arbitrary Mode window
 */
void
gr_CloseArbLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t	*arbWin=(A_ArbWind_t *)client_data;

	if (arbWin != NULL)
	{
        if (arbWin->prev != NULL)
            arbWin->prev->next = arbWin->next;
        else
            arbWin->parent->arbWin = arbWin->next;

        if (arbWin->next != NULL)
            arbWin->next->prev = arbWin->prev;

        arbWin->parent->numArbWins--;

		while (arbWin->numViewWins > 0)
			gr_CloseViewLevel(w,(caddr_t)arbWin->viewWin,(caddr_t)NULL);

		while (arbWin->numAniWins > 0)
			gr_CloseAniLevel(w,(caddr_t)arbWin->aniWin,(caddr_t)NULL);

		XtDestroyWidget(arbWin->shell);
		td_Free((char *)arbWin);
	}
}
