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
 *	File:		gr_xwi1.c
 *	Contents:	Creation functions for X widgets
 *	These functions are used to create windows having the same look
 *	and feel required by X DataSlice.  There should not be any other
 *	explicit Xt calls outside of the gr_xwi*.c files.  This is an
 *	effort to contain porting problems within a smaller scope.
 */

#include "gr_com.h"
#include "gr_xwi.h"


/*
 * Return an Athena Box widget of the given dimensions.
 */
Widget
gr_MakeBox(shellName,parent,x,y,width,height)
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
	shellWind = XtCreateManagedWidget(shellName,boxWidgetClass,parent,
									  argList,i);

	return(shellWind);
}


/*
 * Return a HP scrolledWindow widget with vertical and horizontal scrolled
 * events attached to it, or an Athena ViewPort widget.
 */
Widget
gr_MakeVPort(shellName,parent,scrollType,widgetType,
			 vscrlEv,hscrlEv,grain,client_data,
				x,y,width,height)
char		*shellName;
Widget		parent;
A_Scroll_t	scrollType;
A_Widget_t	widgetType;
XtCallbackProc	vscrlEv,hscrlEv;
int			grain;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		shellWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;
	XtSetArg(argList[i],XtNgranularity,grain); i++;
	XtSetArg(argList[i],XtNinitialDelay,1000); i++;
	XtSetArg(argList[i],XtNrepeatRate,500); i++;
	switch (scrollType)
	{
		case NOSCROLL:
			XtSetArg(argList[i],XtNforceVerticalSB,FALSE); i++;
			XtSetArg(argList[i],XtNforceHorizontalSB,FALSE); i++;
			break;
		case VERTONLY:
			if (widgetType == ATHENA)
			{
				XtSetArg(argList[i],XtNallowVert,TRUE); i++;
				XtSetArg(argList[i],XtNforceBars,TRUE); i++;
			}
			else
			{
				XtSetArg(argList[i],XtNforceVerticalSB,TRUE); i++;
				XtSetArg(argList[i],XtNforceHorizontalSB,FALSE); i++;
			}
			break;
		case HORIZONLY:
			if (widgetType == ATHENA)
			{
				XtSetArg(argList[i],XtNallowHoriz,TRUE); i++;
				XtSetArg(argList[i],XtNforceBars,TRUE); i++;
			}
			else
			{
				XtSetArg(argList[i],XtNforceVerticalSB,FALSE); i++;
				XtSetArg(argList[i],XtNforceHorizontalSB,TRUE); i++;
			}
			break;
		case VERTHORIZ:
			if (widgetType == ATHENA)
			{
				XtSetArg(argList[i],XtNallowVert,TRUE); i++;
				XtSetArg(argList[i],XtNallowHoriz,TRUE); i++;
				XtSetArg(argList[i],XtNforceBars,TRUE); i++;
			}
			else
			{
				XtSetArg(argList[i],XtNforceVerticalSB,TRUE); i++;
				XtSetArg(argList[i],XtNforceHorizontalSB,TRUE); i++;
			}
			break;
	}

	if (widgetType == ATHENA)
		shellWind = XtCreateManagedWidget(shellName,
					viewportWidgetClass,parent,argList,i);
	else
	{
		shellWind = XtCreateManagedWidget(shellName,
					XwswindowWidgetClass,parent,argList,i);
		if (vscrlEv != NULL)
			XtAddCallback(shellWind,"vScrollEvent",vscrlEv,client_data);
		if (hscrlEv != NULL)
			XtAddCallback(shellWind,"hScrollEvent",hscrlEv,client_data);
	}

	return(shellWind);
}

void
gr_WindowEventEnter(w,client_data,call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_Wind_t	windType=(A_Wind_t)client_data;

#ifdef TEKXD88
	switch (windType)
	{
		case ANIWIND:
		case DSPLWIND:
		case ISOWIND:
		case SUBSWIND:
		case TILEWIND:
		case VIEWWIND:
			gr_ImageSetCMap(w);
			break;
		case DICEWIND:
			gr_ImageSetCMapSplit(w);
			break;
	}
#else
#ifdef OPEN_WINDOWS
	switch (windType)
	{
		case ANIWIND:
		case DSPLWIND:
		case ISOWIND:
		case SUBSWIND:
		case TILEWIND:
		case VIEWWIND:
			gr_ImageSetCMap(w);
			break;
		case DICEWIND:
			gr_ImageSetCMapSplit(w);
			break;
	}
#endif
#endif
}

/*
 * Return an X DataSlice "window" with the given termination callBack
 * procedure.  It has a Close button on the top left hand corner and
 * a title bar to the right of the button.
 */
Widget
gr_MakeWindow(shellName,parent,boxWind,cbp,windType,label,quit_label,
				client_data,x,y,width,height)
char		*shellName;
Widget		parent,*boxWind;
XtCallbackProc	cbp;
A_Wind_t 	windType;
char		*label,*quit_label;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		shellWind,boardWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width+2); i++;
	XtSetArg(argList[i],XtNheight,height+2); i++;
	XtSetArg(argList[i],XtNborderWidth,1); i++;
	XtSetArg(argList[i],XtNborderColor,
				BlackPixel(XtDisplay(parent),
						   DefaultScreen(XtDisplay(parent)))); i++;
	switch (windType)
	{
		case ANIWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
						parent,argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case ARBWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
						parent,argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case BOSSWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
						parent,argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case CNTRLWIND:
			shellWind = XtCreateManagedWidget(shellName,
						XwbulletinWidgetClass,
						parent,argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case CUBEWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
						parent,argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case DICEWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
						parent,argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case DSPLWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
					  	parent, argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case HISTWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
					  	parent, argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case FILEWIND:
			shellWind = XtCreateManagedWidget(shellName,
						overrideShellWidgetClass,
					  	parent, argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case ISOWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
					  	parent, argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case SUBSWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
					  	parent, argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case TILEWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
					  	parent, argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		case VIEWWIND:
			shellWind = XtCreateManagedWidget(shellName,
						applicationShellWidgetClass,
					  	parent, argList,i);
			boardWind = gr_MakeBulletin("XDSbulletin",shellWind,
						0,0,width+2,height+2);
			break;
		default:
			fprintf(stderr,"Error\t: Undefined window type.\n");
			exit(-1);
			break;
	}
	(void)gr_MakeTitleBar("XDSlabel",boardWind,label,44,2,width-46,25);
	(void)gr_MakeButton("XDSbutton",boardWind,quit_label,
						(XtCallbackProc)cbp,client_data,2,2,38,25);

	*boxWind = gr_MakeBulletin("XDSbulletin",boardWind,
		2,30,width-4,height-32);

#ifndef NO_FOCUS
	XtAddEventHandler(shellWind,EnterWindowMask,FALSE,
		(XtCallbackProc)gr_WindowEventEnter,windType);
#endif

	return(shellWind);
}


/*
 * Return a "simple" Athena list using the given viewport window.
 */
Widget
gr_MakeListSimple(shellName,vportWind,ncols,callBackProc,
			strings,client_data,x,y,width,height)
char		*shellName;
Widget		*vportWind;
char		*strings[];
int			ncols;
XtCallbackProc	callBackProc;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		listWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNborderWidth,0); i++;
	XtSetArg(argList[i],XtNdefaultColumns,ncols);	i++;
	XtSetArg(argList[i],XtNforceColumns,TRUE);	i++;
	XtSetArg(argList[i],XtNlist,strings);	i++;
	listWind = XtCreateManagedWidget(shellName,
					listWidgetClass, *vportWind, argList, i);
						
	if (callBackProc)
		XtAddCallback(listWind,"callback",callBackProc,client_data);

	return(listWind);
}


/*
 * Return an Athena list widget contained in an HP scrolledWindow
 * widget which has the specified vertical and horizontal scroll events.
 * Note that the viewport window is returned via the parameter vportWind.
 */
Widget
gr_MakeList(shellName,parent,vportWind,scrollType,ncols,grain,
			vscrlEv,hscrlEv,callBackProc,strings,
			client_data,x,y,width,height)
char		*shellName;
Widget		parent;
Widget		*vportWind;
A_Scroll_t	scrollType;
char		*strings[];
int			ncols,grain;
XtCallbackProc	callBackProc,vscrlEv,hscrlEv;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		listWind;

	*vportWind = gr_MakeVPort("XTDvport",parent,scrollType,HP,
							 vscrlEv,hscrlEv,grain,client_data,
							 x,y,width,height);
	listWind = gr_MakeListSimple(shellName,vportWind,
								 ncols,callBackProc,strings,
								 client_data,1,1,width,height);

	return(listWind);
}


/*
 * Return an Athena list widget contained in a "static" HP bulletinBoard
 * widget.
 */
Widget
gr_MakeListStatic(shellName,parent,vportWind,ncols,callBackProc,
			strings,client_data,x,y,width,height)
char		*shellName;
Widget		parent;
Widget		*vportWind;
char		*strings[];
int			ncols;
XtCallbackProc	callBackProc;
caddr_t		client_data;
int			x,y;
int			width,height;
{
	Widget		listWind;

	*vportWind = gr_MakeBulletin("XDSbulletin",parent,x,y,width,height);

	listWind = gr_MakeListSimple(shellName,vportWind,
								 ncols,callBackProc,strings,
								 client_data,1,1,width,height);

	return(listWind);
}


/*
 * Return an Athena text widget.
 */
Widget
gr_MakeText(shellName,parent,vportWind,scrollType,textSelect,textSource,
			string,x,y,width,height)
char		*shellName;
Widget		parent;
Widget		*vportWind;
A_Scroll_t		scrollType;
A_TextSelect_t	textSelect;
A_TextSource_t	textSource;
char		*string;
int			x,y;
int			width,height;
{
	Widget		textWind;
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNheight,height);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;

	*vportWind = parent;
	switch (scrollType)
	{
		case NOSCROLL:
			break;
		case VERTONLY:
			XtSetArg(argList[i],XtNtextOptions,scrollVertical); i++;
			break;
		case HORIZONLY:
			XtSetArg(argList[i],XtNtextOptions,scrollHorizontal); i++;
			break;
		case VERTHORIZ:
			XtSetArg(argList[i],XtNtextOptions,
			scrollHorizontal|scrollVertical); i++;
			break;
	}
	if (textSelect != SELECTABLE)
		XtSetArg(argList[i],XtNsensitive,FALSE);
	else
		XtSetArg(argList[i],XtNsensitive,TRUE);
	i++;
	if (textSource == STRINGSOURCE)
	{
		XtSetArg(argList[i],XtNeditType,XttextEdit);	i++;
		XtSetArg(argList[i],XtNlength,1000);	i++;
		XtSetArg(argList[i],XtNstring,string);	i++;
		textWind = XtCreateManagedWidget(shellName,
					asciiStringWidgetClass, *vportWind, argList, i);
	}
	else
	{
		XtSetArg(argList[i],XtNeditType,XttextAppend);	i++;
		XtSetArg(argList[i],XtNfile,string);	i++;
		textWind = XtCreateManagedWidget(shellName,
					asciiDiskWidgetClass, *vportWind, argList, i);
	}

	XtTextEnableRedisplay(textWind);

	return(textWind);
}
