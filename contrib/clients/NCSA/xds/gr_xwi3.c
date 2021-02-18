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
 *	File:		gr_xwi3.c
 *	Contents:	Creation functions for X widgets
 *	This is an extention of the file gr_xwi2.c.
 */

#include "gr_com.h"
#include "gr_xwi.h"


/*
 * Return an HP Valuator widget.
 */
Widget
gr_MakeSlider(shellName,parent,min,max,extent,origin,grain,
			  scbp,mcbp,rcbp,orient,client_data,x,y,width,height)
char			*shellName;
Widget			parent;
int				min,max,extent,origin,grain;
XtCallbackProc	scbp,mcbp,rcbp;
A_Scroll_t		orient;
caddr_t			client_data;
int				x,y;
int				width,height;
{
	Widget		shellWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;
	if (orient == VERTONLY)
		XtSetArg(argList[i],XtNslideOrientation,XwVERTICAL);
	else
		XtSetArg(argList[i],XtNslideOrientation,XwHORIZONTAL);
	i++;
	XtSetArg(argList[i],XtNsliderMin,min);	i++;
	XtSetArg(argList[i],XtNsliderMax,max);	i++;
	XtSetArg(argList[i],XtNsliderOrigin,origin);	i++;
	XtSetArg(argList[i],XtNsliderExtent,extent);	i++;
/*	XtSetArg(argList[i],XtNgranularity,grain);	i++;
	XtSetArg(argList[i],XtNinitialDelay,10); i++;
	XtSetArg(argList[i],XtNrepeatRate,10); i++; */

	shellWind = XtCreateManagedWidget(shellName,XwvaluatorWidgetClass,
						parent, argList,i);

	if (mcbp != NULL)
		XtAddCallback(shellWind,"sliderMoved",mcbp,client_data);
	if (rcbp != NULL)
		XtAddCallback(shellWind,"sliderReleased",rcbp,client_data);
	if (scbp != NULL)
		XtAddCallback(shellWind,"areaSelected",scbp,client_data);

	return(shellWind);
}


/*
 * Return an HP Row Column widget.
 */
Widget
gr_MakeRowColumn(shellName,parent,RCmode,ncols,x,y,width,height)
char		*shellName;
Widget		parent;
A_RCmode_t	RCmode;
int			ncols;
int			x,y;
int			width,height;
{
	Widget		shellWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;
	XtSetArg(argList[i],XtNcolumns,ncols); i++;
	if (RCmode == ONEOFMANY)
		XtSetArg(argList[i],XtNmode,XwONE_OF_MANY);
	else
		XtSetArg(argList[i],XtNmode,XwN_OF_MANY);
		
	i++;

	shellWind = XtCreateManagedWidget(shellName,XwrowColWidgetClass,parent,
									  argList,i);

	return(shellWind);
}


/*
 * Return an HP BulletinBoard widget.
 */
Widget
gr_MakeBulletin(shellName,parent,x,y,width,height)
char		*shellName;
Widget		parent;
int			x,y;
int			width,height;
{
	Widget		shellWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;
	shellWind = XtCreateManagedWidget(shellName,XwbulletinWidgetClass,parent,
									  argList,i);

	return(shellWind);
}


/*
 * Return a HP staticImage widget
 */
Widget
gr_MakeImageStatic(shellName,parent,image,scbp,client_data,x,y,width,height)
char		*shellName;
Widget		parent;
XImage		*image;
XtCallbackProc	scbp;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		imageWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;
	XtSetArg(argList[i],XtNheight,height);	i++;
	XtSetArg(argList[i],XtNsRimage,image);	i++;
	XtSetArg(argList[i],XtNinvertOnSelect,FALSE);	i++;

	imageWind = 
		XtCreateManagedWidget(shellName,XwstaticRasterWidgetClass,
							  parent,argList,i);

	if (scbp != NULL)
		XtAddCallback(imageWind,"select",scbp,client_data);

	return(imageWind);
}


/*
 * Return an HP Workspace widget.
 */
Widget
gr_MakeWorkSpace(shellName,parent,exposecbp,resizecbp,keydowncbp,
					client_data,x,y,width,height)
char		*shellName;
Widget		parent;
XtCallbackProc	exposecbp,resizecbp,keydowncbp;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		workSWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;
	XtSetArg(argList[i],XtNheight,height);	i++;

	workSWind = 
		XtCreateManagedWidget(shellName,XwworkSpaceWidgetClass,
							  parent,argList,i);

	if (exposecbp != NULL)
		XtAddCallback(workSWind,"expose",exposecbp,client_data);
	if (resizecbp != NULL)
		XtAddCallback(workSWind,"resize",resizecbp,client_data);
	if (keydowncbp != NULL)
		XtAddCallback(workSWind,"keyDown",keydowncbp,client_data);

	return(workSWind);
}
