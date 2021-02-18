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
 *	File:		gr_dicelev.c
 *	Contents:	Dice level window functions for graphics module
 */

#include <math.h>
#include "gr_com.h"

extern void gr_DiceShadeLight();
extern void gr_DiceShadeDark();

#define MINDICEXSIZE	340
#define MINDICEYSIZE	100
#define MAXDICEXSIZE	900
#define MAXDICEYSIZE	900
#define	LEEWAY			3
#define SPACING			20
#define ORIGIN			10


/*
 *	Return a dicer window
 */
A_DiceWind_t
*gr_InitDiceLevel(header,parent,tparent,scale)
A_DiceWind_t	*header;
Widget parent;
A_BossWind_t	*tparent;
int				scale;
{
	XImage		 *palImage;
	A_DiceWind_t *tmp;
	Widget		boxWind,RCWind;
	char		label[STRNG40];
	int			winxsize, winysize;
	int			i,j;
	Display		*dpy;
	int			scr;
	XGCValues	gcvals1,gcvals2,gcvals3;

	gr_WidgetCursor(tparent->shell,XC_watch);

	if ((tmp = (A_DiceWind_t *)td_Malloc(sizeof(A_DiceWind_t),
			   "A_DiceWind_t")) == NULL)
		return(NULL);
	tmp->org = ORIGIN;
	tmp->scale = scale;
	tmp->xdim = td_HdfgetDim(tparent->data,1);
	tmp->ydim = td_HdfgetDim(tparent->data,2);
	tmp->zdim = td_HdfgetDim(tparent->data,3);
	tmp->xdimS = tmp->xdim*tmp->scale;
	tmp->ydimS = tmp->ydim*tmp->scale;
	tmp->zdimS = tmp->zdim*tmp->scale;
	tmp->maxoff   = tmp->zdim*tmp->scale;
	tmp->dimxsize = (tmp->xdim+tmp->zdim)*tmp->scale;
	tmp->dimysize = (tmp->ydim+tmp->zdim)*tmp->scale;
	tmp->xsize = tmp->dimxsize+SPACING;
	tmp->ysize = tmp->dimysize+SPACING;
	tmp->first = TRUE;
	tmp->sliced = FALSE;
	tmp->planeType = 3;

	if ((tmp->data=
		td_Malloc2DChar(tmp->dimxsize,tmp->dimysize,"Dice data"))==NULL)
		return(NULL);

	if ((tmp->fb=
		td_Malloc2DInt(tmp->dimxsize,tmp->dimysize,"Dice fb data"))==NULL)
		return(NULL);

	for (i=0;i<tmp->dimxsize;i++)
		for (j=0;j<tmp->dimysize;j++)
		{
			tmp->data[j][i] = (unsigned char)gr_colorSplit.white;
			tmp->fb[j][i]   = tmp->zdimS+1;
		}

    if (tmp->xsize > MINDICEXSIZE)
        if (tmp->xsize > MAXDICEXSIZE)
            winxsize = MAXDICEXSIZE;
        else
            winxsize = tmp->xsize;
    else
        winxsize = MINDICEXSIZE;

    if (tmp->ysize > MINDICEYSIZE)
        if (tmp->ysize > MAXDICEYSIZE)
            winysize = MAXDICEYSIZE;
        else
            winysize = tmp->ysize;
    else
        winysize = MINDICEYSIZE;

	sprintf(label,"%s: Cartesian Dicer",tparent->filename);

	tmp->shell = gr_MakeWindow("XDS Dicer",parent,&(tmp->win),
								(XtCallbackProc)gr_CloseDiceLevel,
								DICEWIND,label,"Close",(caddr_t)tmp,
								10,10,winxsize+40,winysize+185);

	tmp->vert[0].x = tmp->org;
	tmp->vert[0].y = tmp->ysize-tmp->org;
	tmp->vert[1].x = tmp->vert[0].x+tmp->xdim*tmp->scale;
	tmp->vert[1].y = tmp->vert[0].y;
	tmp->vert[2].x = tmp->vert[1].x;
	tmp->vert[2].y = tmp->vert[1].y-tmp->ydim*tmp->scale;
	tmp->vert[3].x = tmp->vert[0].x;
	tmp->vert[3].y = tmp->vert[2].y;
	tmp->vert[4].x = tmp->org+tmp->zdim*tmp->scale;
	tmp->vert[4].y = tmp->ysize-(tmp->org+tmp->zdim*tmp->scale);
	tmp->vert[5].x = tmp->vert[4].x+tmp->xdim*tmp->scale;
	tmp->vert[5].y = tmp->vert[4].y;
	tmp->vert[6].x = tmp->vert[5].x;
	tmp->vert[6].y = tmp->vert[5].y-tmp->ydim*tmp->scale;
	tmp->vert[7].x = tmp->vert[4].x;
	tmp->vert[7].y = tmp->vert[6].y;

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,
		2,2,winxsize+30,winysize+30);

	tmp->imageVPort = gr_MakeVPort("XDSvport",boxWind,
						NOSCROLL, HP, NULL, NULL, 5,(caddr_t)tmp,
						0,0,winxsize+LEEWAY,winysize+LEEWAY);

	tmp->imageWin = gr_MakeWorkSpace("XDSworkspace",tmp->imageVPort,
						(XtCallbackProc)gr_DiceExpose, NULL, NULL,
						(caddr_t)tmp,
						0,0,winxsize,winysize);

	tmp->image	= gr_ImageCreate(tmp->imageVPort,
				tmp->xsize,tmp->ysize,tmp->data[tmp->dimysize-1]);

	tmp->xSlider =
		gr_MakeSlider("SDSslider",boxWind,0,tmp->xdim,1,0,1,
			(XtCallbackProc)gr_DiceXSliderSel,
			(XtCallbackProc)gr_DiceXSliderMov,
			(XtCallbackProc)gr_DiceXSliderRel,
			HORIZONLY, (caddr_t)tmp,
			ORIGIN+2,winysize+4+LEEWAY,tmp->xdimS,19);

	tmp->ySlider =
		gr_MakeSlider("SDSslider",boxWind,0,tmp->ydim,1,tmp->ydim-1,1,
			(XtCallbackProc)gr_DiceYSliderSel,
			(XtCallbackProc)gr_DiceYSliderMov,
			(XtCallbackProc)gr_DiceYSliderRel,
			VERTONLY, (caddr_t)tmp,
			winxsize+4+LEEWAY,ORIGIN+2,19,tmp->ydimS);

	tmp->zSlider =
		gr_MakeSlider("SDSslider",boxWind,0,tmp->zdim,1,tmp->zdim/2,1,
			(XtCallbackProc)gr_DiceZSliderSel,
			(XtCallbackProc)gr_DiceZSliderMov,
			(XtCallbackProc)gr_DiceZSliderRel,
			HORIZONLY, (caddr_t)tmp,
			ORIGIN+2+tmp->xdimS,winysize+4+LEEWAY,tmp->zdimS,19);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,
		2,winysize+42,winxsize+30,120);

	palImage = gr_ImageCreate(boxWind,gr_colorSplit.maxColors,30,gr_palData);
	gr_MakeImageStatic("XDSstaticimage",boxWind,palImage,NULL,
				(caddr_t)NULL,0,0,256,30);

	gr_MakeButton("XDSbutton",boxWind,"Lighter",
				(XtCallbackProc)gr_DiceShadeLight,(caddr_t)tmp,
				0,30,50,25);

	tmp->shadeSlider =
		gr_MakeSlider("SDSslider",boxWind,0,10001,1,0,1,
			(XtCallbackProc)gr_DiceShadeSel,
			(XtCallbackProc)gr_DiceShadeMov,
			(XtCallbackProc)gr_DiceShadeRel,
			HORIZONLY, (caddr_t)tmp,52,30,152,25);

	gr_MakeButton("XDSbutton",boxWind,"Darker",
				(XtCallbackProc)gr_DiceShadeDark,(caddr_t)tmp,
				206,30,50,25);

	gr_MakeButton("XDSbutton",boxWind,"Palette",
				(XtCallbackProc)gr_LoadPALSplit,(caddr_t)tmp,
				260,0,45,25);

	gr_MakeButton("XDSbutton",boxWind,"Save",
				(XtCallbackProc)gr_SaveDicer,(caddr_t)tmp,
				260,30,45,25);

	gr_MakeButton("XDSbutton",boxWind,"View",
				(XtCallbackProc)gr_DiceView,(caddr_t)tmp,
				310,0,45,25);

	gr_MakeButton("XDSbutton",boxWind,"Clear",
				(XtCallbackProc)gr_DiceErase,(caddr_t)tmp,
				310,30,45,25);

    RCWind = gr_MakeRowColumn("XDSrowcolumn",boxWind,ONEOFMANY,3,
                    0,60,160,20);
    tmp->xToggle=gr_MakeToggle("XDStoggle",RCWind,"X",DIAMOND,
                    FALSE, (XtCallbackProc)gr_DiceSetX,
                    NULL,(caddr_t)tmp, 2,2,50,20);
    tmp->yToggle=gr_MakeToggle("XDStoggle",RCWind,"Y",DIAMOND,
                    FALSE, (XtCallbackProc)gr_DiceSetY,
                    NULL,(caddr_t)tmp, 52,2,50,20);
    tmp->zToggle=gr_MakeToggle("XDStoggle",RCWind,"Z",DIAMOND,
                    TRUE, (XtCallbackProc)gr_DiceSetZ,
                    NULL,(caddr_t)tmp, 102,2,50,20);
	gr_MakeButton("XDSbutton",boxWind,"Set Planes",
				(XtCallbackProc)gr_DiceSetPlanes,(caddr_t)tmp,
				0,80,160,28);

	tmp->xsplane = 0;
	tmp->xfplane = tmp->xdim-1;
	tmp->ysplane = 0;
	tmp->yfplane = tmp->ydim-1;
	tmp->dicer = FALSE;

	sprintf(msg,"%d",tmp->zdim/2);
    tmp->splaneDialog =
        gr_MakeDialog("XDSdialog",boxWind,"Start",msg,4,
					162,60,50,50);
    tmp->fplaneDialog =
        gr_MakeDialog("XDSdialog",boxWind,"End ",msg,4,
					212,60,44,50);
	gr_DiceZCalc(tmp,tmp->zdim/2,tmp->zdim/2);

    gr_MakeToggle("XDStoggle",boxWind,"Dicer",SQUARE,FALSE,
				(XtCallbackProc)gr_DiceSetDicerOn,
                (XtCallbackProc)gr_DiceSetDicerOff,
				(caddr_t)tmp, 260,60,50,20);

	dpy = XtDisplay(tmp->imageWin);
	scr = DefaultScreen(dpy);
	gcvals1.foreground = BlackPixel(dpy,scr);
	gcvals1.function   = GXcopy;
	gcvals2.foreground = BlackPixel(dpy,scr);
	gcvals2.function   = GXinvert;
	gcvals3.foreground = BlackPixel(dpy,scr);
	gcvals3.function   = GXxor;
	tmp->gc1 = XtGetGC(tmp->imageWin, GCForeground|GCFunction, &gcvals1);
	tmp->gc2 = XtGetGC(tmp->imageWin, GCForeground|GCFunction, &gcvals2);
	tmp->gc3 = XtGetGC(tmp->imageWin, GCForeground|GCFunction, &gcvals3);
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
 *	Close a Dicer window
 */
void
gr_CloseDiceLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DiceWind_t	*diceWin=(A_DiceWind_t *)client_data;

	if (diceWin != NULL)
	{
        if (diceWin->prev != NULL)
            diceWin->prev->next = diceWin->next;
        else
            diceWin->parent->diceWin = diceWin->next;

        if (diceWin->next != NULL)
            diceWin->next->prev = diceWin->prev;

        diceWin->parent->numDiceWins--;

		XDestroyImage(diceWin->image);
		td_Free((char *)diceWin->image);
		td_Free2dChar(diceWin->data);
		td_Free2dInt(diceWin->fb);
		XtDestroyWidget(diceWin->shell);
		td_Free((char *)diceWin);
	}
}
