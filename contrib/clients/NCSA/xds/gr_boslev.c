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
 *	File:		gr_boslev.c
 *	Contents:	Boss level window functions for graphics module
 */

#include "gr_com.h"

/*
 * Return a Boss Window with statistics on the 3D SDS and toggles to open
 * specialized tools on the SDS.
 */
A_BossWind_t
*gr_InitBossLevel(header,shellName,parent,num,tparent,format)
A_BossWind_t	*header;
char *shellName;
Widget parent;
int	 num;
A_TopWind_t	*tparent;
A_FileFormat_t format;
{
	A_BossWind_t *tmp;
	Widget		RCWind,boxWind;
	char		label[STRNG40],*txtstrng;
	int			ret;

	gr_WidgetCursor(tparent->shell,XC_watch);

	if ((tmp = (A_BossWind_t *)td_Malloc(sizeof(A_BossWind_t),
			   "A_BossWind_t")) == NULL)
		return(NULL);

	if ((tmp->data = (A_Data_t *)td_Malloc(sizeof(A_Data_t),
					 "A_Data_t")) == NULL)
		return(NULL);

	sprintf(tmp->filename,"%s,#%d",shellName,num);
	sprintf(tmp->data->pathName,"%s/%s",td_getDirName(),shellName);

	if ( (format == HDF) && ((ret = td_HdfLoad(tmp->data,num)) == -1) )
	{
		gr_WidgetCursor(tparent->shell,XC_draped_box);
		sprintf(msg,"***HDF ERROR: Cannot load SDS %d.\n",num);
		gr_TextMsgOut(msg);
		td_Free((char *)tmp->data);
		td_Free((char *)tmp);
		return(NULL);
	}
	else
	if ( (format == MIRIAD) && 
		((ret = 
		  td_MiriadLoad(tmp->data,gr_color.nColors,gr_colorSplit.nColors)) 
		  == -1) )
	{
		gr_WidgetCursor(tparent->shell,XC_draped_box);
		sprintf(msg,"***MIRIAD ERROR: Cannot load image.\n");
		gr_TextMsgOut(msg);
		td_Free((char *)tmp->data);
		td_Free((char *)tmp);
		return(NULL);
	};

	sprintf(label,"%s: Attributes",tmp->filename);
	tmp->shell = gr_MakeWindow("XDS File Attributes",parent,&(tmp->win),
								(XtCallbackProc)gr_CloseBossLevel,
								BOSSWIND,label,"Close",(caddr_t)tmp,
								10,10,300,210);

    if (format == HDF)
	tmp->msgWin =
		gr_MakeText("XDStext",tmp->win,
					&(tmp->msgVPort),NOSCROLL,
					SELECTABLE,STRINGSOURCE,
					td_HdfgetStats(tmp->data,TRUE,gr_color.nColors,
					gr_colorSplit.nColors,format),
					2, 2, 290, 70);
	else
	{
		txtstrng = td_Malloc1D(500,1,sizeof(char),"text for statistics");
		sprintf(txtstrng,
"Miriad File\nX dim = %d\nY dim = %d\nZ dim = %d\nMax=%8.5f, Min=%8.5f",
		tmp->data->dims[0],
		tmp->data->dims[1],
		tmp->data->dims[2],
		tmp->data->max,
		tmp->data->min);
	tmp->msgWin =
		gr_MakeText("XDStext",tmp->win,
					&(tmp->msgVPort),NOSCROLL,SELECTABLE,STRINGSOURCE,
					txtstrng, 2, 2, 290, 70);
	}

	RCWind = gr_MakeRowColumn("XDSRC",tmp->win,ONEOFMANY,1,
						2,75,150,100);
			gr_MakeToggle("XDStoggle",RCWind,"Planes Along Axes",DIAMOND,
					  	FALSE,(XtCallbackProc)gr_BossSetAxes,
						NULL,(caddr_t)tmp,
						2,0,150,20);
			gr_MakeToggle("XDStoggle",RCWind,"Arbitrary Planes",DIAMOND,
					  	FALSE,(XtCallbackProc)gr_BossSetArb,
						NULL,(caddr_t)tmp,
						2,20,150,20);
			gr_MakeToggle("XDStoggle",RCWind,"Cartesian Dicer",DIAMOND,
					  	FALSE,(XtCallbackProc)gr_BossSetDicer,
						NULL,(caddr_t)tmp,
						2,40,150,20);
			gr_MakeToggle("XDStogg0e",RCWind,"IsoSurface Renderer",DIAMOND,
					  	FALSE,(XtCallbackProc)gr_BossSetIso,
						NULL,(caddr_t)tmp,
						2,60,150,20);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,150,75,142,100);
	tmp->diceScaleDialog = gr_MakeDialog("XDSdialog",boxWind,
		"Dicer/IsoSurf. Scale","3",4,1,1,135,50);
	sprintf(msg,"%8.5f",tmp->data->min);
	tmp->minDialog = gr_MakeDialog("XDSdialog",boxWind,
		"Min     ",msg,4,1,53,68,45);
	sprintf(msg,"%8.5f",tmp->data->max);
	tmp->maxDialog = gr_MakeDialog("XDSdialog",boxWind,
		"Max     ",msg,4,70,53,66,45);

	tmp->dsplMode = AXES;
	tmp->numCubeWins=tmp->numArbWins=tmp->numDiceWins=tmp->numIsoWins= 0;
	tmp->cubeWin = NULL;
	tmp->arbWin = NULL;
	tmp->diceWin = NULL;
	tmp->isoWin = NULL;

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
 * Close a Boss window and all its children.
 */
void
gr_CloseBossLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_BossWind_t	*bossWin=(A_BossWind_t *)client_data;

	if (bossWin != NULL)
	{
        if (bossWin->prev != NULL)
            bossWin->prev->next = bossWin->next;
        else
            bossWin->parent->bossWin = bossWin->next;

        if (bossWin->next != NULL)
            bossWin->next->prev = bossWin->prev;

        bossWin->parent->numBossWins--;

		while (bossWin->numCubeWins > 0)
			gr_CloseCubeLevel(bossWin->cubeWin->shell,
				(caddr_t)bossWin->cubeWin,(caddr_t)NULL);

		while (bossWin->numArbWins > 0)
			gr_CloseArbLevel(bossWin->arbWin->shell,
				(caddr_t)bossWin->arbWin,(caddr_t)NULL);

		while (bossWin->numDiceWins > 0)
			gr_CloseDiceLevel(bossWin->diceWin->shell,
				(caddr_t)bossWin->diceWin,(caddr_t)NULL);

		while (bossWin->numIsoWins > 0)
			gr_CloseIsoLevel(bossWin->isoWin->shell,
				(caddr_t)bossWin->isoWin,(caddr_t)NULL);


		XtDestroyWidget(bossWin->shell);
		td_Free3d(bossWin->data->data);
		td_Free((char *)bossWin->data);
		td_Free((char *)bossWin);
	}
}
