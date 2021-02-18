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
 *	File:		gr_toplev.c
 *	Contents:	Top level window functions for graphics module
 */

#include "gr_com.h"


/*
 * Open the top level window with the logbook and buttons to open the file
 * window.
 */
void
gr_InitTopLevel(shellName,parent,topWin)
char *shellName;
Widget parent;
A_TopWind_t	*topWin;
{
	Widget		boxWind;

	topWin->shell = 
		gr_MakeWindow("XDS Control",parent,&(topWin->win),
					  (XtCallbackProc)gr_Terminate,CNTRLWIND,
					  td_getToolName(),"Quit",(caddr_t)topWin,0,0,404,255);

	boxWind = gr_MakeBulletin("XDSbulletin",topWin->win,2,2,400,140);
	topWin->msgWin = gr_MakeText("XDStext",boxWind,
					&topWin->msgVPort,VERTONLY,
					SELECTABLE,FILESOURCE, td_getLogFileName(),
					0, 0, 390, 135);

	boxWind = gr_MakeBulletin("XDSbulletin",topWin->win,2,144,400,30);
		gr_MakeButton("XDSbutton",boxWind,"Load 3D SDS",
						(XtCallbackProc)gr_LoadSDS,(caddr_t)topWin,
					  	1, 1, 90, 18);
		gr_MakeButton("XDSbutton",boxWind,"Load 8-bit RASTER",
						(XtCallbackProc)gr_LoadRASTER,(caddr_t)topWin,
					  	100, 1, 190, 18);
		gr_MakeButton("XDSbutton",boxWind,"Process a SDS",
						(XtCallbackProc)gr_SaveProc,(caddr_t)topWin,
					  	300, 1, 86, 18);

	gr_topWin.numAniWins = 0;
	gr_topWin.numBossWins = 0;
	gr_topWin.numFileWins = 0;
	gr_topWin.bossWin = NULL;
	gr_topWin.fileWin = NULL;
	gr_topWin.aniWin = NULL;
}
