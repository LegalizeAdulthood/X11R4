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
 *	File:		gr_xwi2.c
 *	Contents:	Creation functions for X widgets
 *	This file is an extension of the file gr_xwi1.c
 */

#include "gr_com.h"
#include "gr_xwi.h"

/*
 * Return an HP toggle widget with the select and release cbps.
 */
Widget
gr_MakeToggle(shellName,parent,label,toggleShape,setOn,slcbp,rlcbp,
				client_data,x,y,width,height)
char		*shellName;
Widget		parent;
char		*label;
A_ToggleShape_t	toggleShape;
Boolean		setOn;
XtCallbackProc	slcbp,rlcbp;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		toggleWind;
	Arg			argList[10];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;

/* On the certain clients, the toggles are somehow offset by 4 */
#ifdef ADJUST_TOGGLES
	XtSetArg(argList[i],XtNheight,height-4);	i++;
#else
	XtSetArg(argList[i],XtNheight,height);	i++;
#endif
	XtSetArg(argList[i],XtNlabel,label);	i++;

	XtSetArg(argList[i],XtNset,setOn);	i++;
	if (toggleShape == SQUARE)
		XtSetArg(argList[i],XtNsquare,TRUE);
	else
		XtSetArg(argList[i],XtNsquare,FALSE);
		
	i++;

	toggleWind = 
		XtCreateManagedWidget(shellName,XwtoggleWidgetClass,
							  parent,argList,i);
	if (slcbp != NULL)
		XtAddCallback(toggleWind,"select",slcbp,client_data);
	if (rlcbp != NULL)
		XtAddCallback(toggleWind,"release",rlcbp,client_data);

	return(toggleWind);
}


/*
 * Return an Athena Command widget.
 */
Widget
gr_MakeButton(shellName,parent,label,callBackProc,client_data,x,y,width,height)
char		*shellName;
Widget		parent;
char		*label;
XtCallbackProc	callBackProc;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		buttonWind;
	Arg			argList[10];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;
	XtSetArg(argList[i],XtNheight,height);	i++;
	XtSetArg(argList[i],XtNlabel,label);	i++;

	buttonWind = 
		XtCreateManagedWidget(shellName,commandWidgetClass,
							  parent,argList,i);

	if (callBackProc)
		XtAddCallback(buttonWind,"callback",callBackProc,client_data);

	return(buttonWind);
}


void
gr_DialogEventEnter(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
		XSetInputFocus(XtDisplay(w), XtWindow(w),RevertToParent,CurrentTime);
}


/*
 * Return an Athena Dialog widget.
 */
Widget
gr_MakeDialog(shellName,parent,label,defString,maxLen,x,y,width,height)
char		*shellName;
Widget		parent;
char		*label;
char		*defString;
int			maxLen;
int			x,y;
int			width,height;
{
	Widget		dialogWind,textWind;
	Arg			argList[10];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;
	XtSetArg(argList[i],XtNheight,height);	i++;
	XtSetArg(argList[i],XtNlabel,label);	i++;
	if (defString != NULL)
	{
		XtSetArg(argList[i],XtNvalue,defString);	i++;
	}
/*	XtSetArg(argList[i],XtNmaximumLength,maxLen);	i++; */

	dialogWind = 
		XtCreateManagedWidget(shellName,dialogWidgetClass,
							  parent,argList,i);
	textWind = gr_DialogGetTextWind(dialogWind);
	XtTextSetInsertionPoint(textWind,(long)strlen(defString));

#ifndef NO_FOCUS
    XtAddEventHandler(dialogWind,EnterWindowMask,FALSE,
			(XtCallbackProc)gr_DialogEventEnter,NULL);
#endif

	return(dialogWind);
}


/*
 * Return an Athena Label widget.
 */
Widget
gr_MakeTitleBar(shellName,parent,label,x,y,width,height)
char 	*shellName;
Widget 	parent;
char	*label;
int		x,y;
int		width,height;
{
	Widget		labelWind;
	Arg			argList[10];
	Cardinal	i=0;
	XFontStruct	*labelFont;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;
	XtSetArg(argList[i],XtNheight,height);	i++;
	XtSetArg(argList[i],XtNlabel,label);	i++;
/*
	if ((labelFont = XLoadQueryFont(XtDisplay(parent),DEF_LABEL_FONT)) != NULL)
	{
		XtSetArg(argList[i],XtNfont,labelFont);	i++;
	}
*/
	labelWind = XtCreateManagedWidget(shellName,labelWidgetClass,
				parent,argList,i);

	return(labelWind);
}
