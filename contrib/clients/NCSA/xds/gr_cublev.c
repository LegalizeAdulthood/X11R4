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
 *	File:		gr_cublev.c
 *	Contents:	Cube level window functions for graphics module
 */

#include "gr_com.h"


/*
 *	Create 3D V-Buffer controls
 */
gr_Make3DView(tmp)
A_CubeWind_t *tmp;
{
	tmp->box3D = gr_MakeBulletin("XDSbulletin",tmp->win,2,112,290,120);

	tmp->xptDialog =
		gr_MakeDialog("XDSdialog",tmp->box3D,"X pts","5",4,
				  	2,2,50,50);
	tmp->yptDialog =
		gr_MakeDialog("XDSdialog",tmp->box3D,"Y pts","5",4,
				  	52,2,50,50);
	tmp->zptDialog =
		gr_MakeDialog("XDSdialog",tmp->box3D,"Z pts","5",4,
				  	102,2,50,50);
	tmp->numSubsDialog =
		gr_MakeDialog("XDSdialog",tmp->box3D,"Substances","5",5,
				  	152,2,80,50);

	gr_MakeButton("XDSbutton",tmp->box3D,"VBUFF",
		(XtCallbackProc)gr_CubeOpenSubs,(caddr_t)tmp,
	  	238,2,50,50);
}


/*
 *	Create 2D slicing controls
 */
gr_Make2DView(tmp)
A_CubeWind_t *tmp;
{
	Widget boxWind;

	tmp->box2D = gr_MakeBulletin("XDSbulletin",tmp->win,2,112,290,120);

	tmp->planeDialog =
		gr_MakeDialog("XDSdialog",tmp->box2D,"Plane ID     ","0",5,
				  	2,2,95,50);
	tmp->incrDialog =
		gr_MakeDialog("XDSdialog",tmp->box2D,"Frame Incr   ","1",5,
				  	2,57,95,50);
	tmp->widthDialog =
		gr_MakeDialog("XDSdialog",tmp->box2D,"Width","4",5,
					147,2,47,50);
	tmp->scaleDialog =
		gr_MakeDialog("XDSdialog",tmp->box2D,"Scale","4",5,
					99,2,46,50);
	tmp->numPlanesDialog =
		gr_MakeDialog("XDSdialog",tmp->box2D,"No. of Frames","5",5,
				  	99,57,95,50);
	boxWind = gr_MakeBulletin("XDSbulletin",tmp->box2D,200,2,86,105);
		gr_MakeButton("XDSbutton",boxWind,"Single",
				  	(XtCallbackProc)gr_CubeDisplaySingle,(caddr_t)tmp,
					2,2,80,20);
		gr_MakeButton("XDSbutton",boxWind,"Frame",
				  	(XtCallbackProc)gr_CubeDisplayAuto,(caddr_t)tmp,
					2,26,80,20);
		gr_MakeButton("XDSbutton",boxWind,"Unfolded",
				  	(XtCallbackProc)gr_CubeDisplayUnfolded,(caddr_t)tmp,
					2,50,80,20);
		gr_MakeButton("XDSbutton",boxWind,"Animate",
				  	(XtCallbackProc)gr_CubeAnimate,(caddr_t)tmp,
					2,74,80,20);
}


/*
 *	Return a cartesian Display Mode Window
 */
A_CubeWind_t
*gr_InitCubeLevel(header,parent,tparent)
A_CubeWind_t	*header;
Widget parent;
A_BossWind_t	*tparent;
{
	A_CubeWind_t *tmp;
	Widget		RCWind,boxWind;
	char		label[STRNG40];

	gr_WidgetCursor(tparent->shell,XC_watch);

	if ((tmp = (A_CubeWind_t *)td_Malloc(sizeof(A_CubeWind_t),
			   "A_CubeWind_t")) == NULL)
		return(NULL);

	sprintf(label,"%s: Display Mode",tparent->filename);

	tmp->shell = gr_MakeWindow("XDS Cartesian Display Mode",parent,&(tmp->win),
				(XtCallbackProc)gr_CloseCubeLevel,
				CUBEWIND,label,"Close",(caddr_t)tmp,10,220,404,270);

	tmp->drawWin = gr_MakeWorkSpace("XDSworkspace",tmp->win,
						(XtCallbackProc)gr_CubeDrawExpose,NULL,NULL,
						(caddr_t)tmp,
						2,2,100,100);

	boxWind = gr_MakeBox("XDSbox",tmp->win,102,2,100,100);
	RCWind = gr_MakeRowColumn("XDSrowcolumn",boxWind,ONEOFMANY,1,0,0,
						90,90);
			gr_MakeToggle("XDStoggle",RCWind,"XY Plane",DIAMOND,
					  	TRUE,(XtCallbackProc)gr_CubeSetXY,
						NULL,(caddr_t)tmp,
						2,2,100,27);
			gr_MakeToggle("XDStoggle",RCWind,"XZ Plane",DIAMOND,
					  	FALSE,(XtCallbackProc)gr_CubeSetXZ,
						NULL,(caddr_t)tmp,
						2,33,100,27);
			gr_MakeToggle("XDStoggle",RCWind,"YZ Plane",DIAMOND,
					  	FALSE,(XtCallbackProc)gr_CubeSetYZ,
						NULL,(caddr_t)tmp,
						2,63,100,27);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,202,2,
						195,50);
			gr_MakeButton("XDSbutton",boxWind,"Cntr-Clockwise",
					  	(XtCallbackProc)gr_CubeRotRight,(caddr_t)tmp,
						2,2,90,40);
			gr_MakeButton("XDSbutton",boxWind,"Clockwise",
					  	(XtCallbackProc)gr_CubeRotLeft,(caddr_t)tmp,
						96,2,90,40);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,202,52,
						195,50);
			gr_MakeButton("XDSbutton",boxWind,"Flip Vert",
					  	(XtCallbackProc)gr_CubeFlipVert,(caddr_t)tmp,
						2,2,90,40);
			gr_MakeButton("XDSbutton",boxWind,"Flip Horiz",
					  	(XtCallbackProc)gr_CubeFlipHoriz,(caddr_t)tmp,
						96,2,90,40);
	
	gr_Make2DView(tmp);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,302,112,95,120);
			gr_MakeToggle("XDStoggle",boxWind,"Interpolated",SQUARE,
					  	FALSE,(XtCallbackProc)gr_CubeSetInterp,
						(XtCallbackProc)gr_CubeSetNonInterp,(caddr_t)tmp,
						2,2,100,20);
			gr_MakeToggle("XDStoggle",boxWind,"Use Pixmaps",SQUARE,
					  	TRUE,(XtCallbackProc)gr_CubeUsePixmap,
						(XtCallbackProc)gr_CubeUseXImage,(caddr_t)tmp,
						2,22,100,20);
			gr_MakeToggle("XDStoggle",boxWind,"Use Disk",SQUARE,
					  	FALSE,(XtCallbackProc)gr_CubeUseDisk,
						(XtCallbackProc)gr_CubeUseMemory,(caddr_t)tmp,
						2,42,100,20);
	RCWind = gr_MakeRowColumn("XDSrowcolumn",boxWind,ONEOFMANY,1,0,64,
						95,75);
			gr_MakeToggle("XDStoggle",RCWind,"View 2D",DIAMOND,
					  	TRUE,(XtCallbackProc)gr_CubeSet2D,
						NULL,(caddr_t)tmp,
						2,2,100,20);
			gr_MakeToggle("XDStoggle",RCWind,"View 3D",DIAMOND,
					  	FALSE,(XtCallbackProc)gr_CubeSet3D,
						NULL,(caddr_t)tmp,
						2,24,100,20);

	tmp->interp = FALSE;

/* Cannot use pixmaps on the IRIS */
#ifdef IRIS
	tmp->usePixmap = FALSE;
#else
	tmp->usePixmap = TRUE;
#endif
	tmp->useDisk = FALSE;
	tmp->view2D = TRUE;
	tmp->box3D = NULL;
	tmp->aniWin = NULL;
	tmp->dsplWin = NULL;
	tmp->subsWin = NULL;
	tmp->tileWin = NULL;
	tmp->numAniWins = 0;
	tmp->numDsplWins = 0;
	tmp->numSubsWins = 0;
	tmp->numTileWins = 0;
	tmp->axesOrient.row = 2;
	tmp->axesOrient.col = 1;
	tmp->axesOrient.axis = 3;
	tmp->axesOrient.plane = 0;

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
 *	Close a Cartesian Display Mode Window
 */
void
gr_CloseCubeLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_CubeWind_t	*cubeWin=(A_CubeWind_t *)client_data;

	if (cubeWin != NULL)
	{
        if (cubeWin->prev != NULL)
            cubeWin->prev->next = cubeWin->next;
        else
            cubeWin->parent->cubeWin = cubeWin->next;

        if (cubeWin->next != NULL)
            cubeWin->next->prev = cubeWin->prev;

        cubeWin->parent->numCubeWins--;

		while (cubeWin->numDsplWins > 0)
			gr_CloseDsplLevel(cubeWin->dsplWin->shell,
				(caddr_t)cubeWin->dsplWin,(caddr_t)NULL);

		while (cubeWin->numTileWins > 0)
			gr_CloseTileLevel(cubeWin->tileWin->shell,
				(caddr_t)cubeWin->tileWin,(caddr_t)NULL);

		while (cubeWin->numAniWins > 0)
			gr_CloseAniLevel(cubeWin->aniWin->shell,
				(caddr_t)cubeWin->aniWin,(caddr_t)NULL);

		while (cubeWin->numSubsWins > 0)
			gr_CloseSubsLevel(cubeWin->subsWin->shell,
				(caddr_t)cubeWin->subsWin,(caddr_t)NULL);

		XtDestroyWidget(cubeWin->shell);
		td_Free((char *)cubeWin);
	}
}
