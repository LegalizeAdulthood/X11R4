/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Dave Thompson, National Center for Supercomputing Applications
 * davet@ncsa.uiuc.edu
 */

/*
 *	File:		gr_xwi1.c
 *	Contents:	Creation functions for X widgets
 *	Author:		Eng-Whatt Toh
 */

#include "gr_com.h"

extern unsigned long black,white;
Widget
gr_MakeBox(shellName,parent,x,y,width,height)
char		*shellName;
Widget		parent;
int			x,y;
int			width,height;
{
	Widget		shellWind;
	Arg			argList[10];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;
	XtSetArg(argList[i],XtNborderWidth,DEF_BORDER_WIDTH); i++;
	XtSetArg(argList[i],XtNborder,
				BlackPixel(XtDisplay(parent),
						   DefaultScreen(XtDisplay(parent)))); i++;
	shellWind = XtCreateManagedWidget(shellName,boxWidgetClass,parent,
									  argList,i);

	return(shellWind);
}

Widget
gr_MakeVPort(shellName,parent,scrollType,x,y,width,height)
char		*shellName;
Widget		parent;
A_Scroll_t	scrollType;
int			x,y;
int			width,height;
{
	Widget		shellWind;
	Arg			argList[10];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;
	switch (scrollType)
	{
		case NOSCROLL:
			break;
		case VERTONLY:
			XtSetArg(argList[i],XtNallowVert,TRUE); i++;
			XtSetArg(argList[i],XtNforceBars,TRUE); i++;
			break;
		case HORIZONLY:
			XtSetArg(argList[i],XtNallowHoriz,TRUE); i++;
			XtSetArg(argList[i],XtNforceBars,TRUE); i++;
			break;
		case VERTHORIZ:
			XtSetArg(argList[i],XtNallowVert,TRUE); i++;
			XtSetArg(argList[i],XtNallowHoriz,TRUE); i++;
			XtSetArg(argList[i],XtNforceBars,TRUE); i++;
			break;
	}

	shellWind = XtCreateManagedWidget(shellName,viewportWidgetClass,parent,
									  argList,i);

	return(shellWind);
}

Widget
gr_MakeWindow(shellName,parent,windType,label,x,y,width,height)
char		*shellName;
Widget		parent;
A_Wind_t 	windType;
char		*label;
int			x,y;
int			width,height;
{
	XFontStruct	*labelFont;
	Widget		shellWind,boxWind;
	Arg			argList[10];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;
	XtSetArg(argList[i],XtNborderWidth,DEF_BORDER_WIDTH); i++;
	XtSetArg(argList[i],XtNborder,
				BlackPixel(XtDisplay(parent),
						   DefaultScreen(XtDisplay(parent)))); i++;
	XtSetArg(argList[i],XtNallowShellResize,FALSE); i++;
	if (windType == CNTRLWIND)
	shellWind = XtCreateManagedWidget(shellName,boxWidgetClass,parent,
									  argList,i);
	else
	shellWind = XtCreateManagedWidget(shellName,transientShellWidgetClass,
									  parent, argList,i);

	boxWind = gr_MakeBox(shellName,shellWind,2,2,width-2,height-2);
	labelFont = XLoadQueryFont(XtDisplay(boxWind),DEF_LABEL_FONT);
	i=0;
	XtSetArg(argList[i],XtNx,1);	i++;
	XtSetArg(argList[i],XtNy,1);	i++;
	XtSetArg(argList[i],XtNwidth,width-2);	i++;
	XtSetArg(argList[i],XtNlabel,label);	i++;
	XtSetArg(argList[i],XtNfont,labelFont);	i++;

	(void) XtCreateManagedWidget("XTDLabel",labelWidgetClass,boxWind,
								argList, i);

	return(shellWind);
}

Widget
gr_MakeList(shellName,parent,scrollType,ncols,callBackProc,
			strings,x,y,width,height)
char		*shellName;
Widget		parent;
A_Scroll_t	scrollType;
char		*strings[];
int			ncols;
XtCallbackProc	callBackProc;
int			x,y;
int			width,height;
{
	Widget		listWind,vportWind;
	Arg			argList[10];
	Cardinal	i=0;

	vportWind = gr_MakeVPort("XTDvport",parent,scrollType,
							 x,y,width,height);
	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
/*	XtSetArg(argList[i],XtNwidth,width);	i++; */
/*	XtSetArg(argList[i],XtNheight,height);	i++; */
	XtSetArg(argList[i],XtNdefaultColumns,ncols);	i++;
	XtSetArg(argList[i],XtNforceColumns,TRUE);	i++;
/*	XtSetArg(argList[i],XtNverticalList,TRUE);	i++; */
	XtSetArg(argList[i],XtNpasteBuffer,TRUE);	i++;
	XtSetArg(argList[i],XtNlist,strings);	i++;
	listWind = XtCreateManagedWidget(shellName,
					listWidgetClass, vportWind, argList, i);
						
	if (callBackProc)
		XtAddCallback(listWind,"callback",callBackProc,NULL);

	return(listWind);
}

Widget
gr_MakeText(shellName,parent,scrollType,textSelect,textSource,
			string,x,y,width,height)
char		*shellName;
Widget		parent;
A_Scroll_t		scrollType;
A_TextSelect_t	textSelect;
A_TextSource_t	textSource;
char		*string;
int			x,y;
int			width,height;
{
	Widget		textWind;
	Arg			argList[15];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;
	XtSetArg(argList[i],XtNheight,height);	i++;
	XtSetArg(argList[i],XtNforeground,black);	i++;
	XtSetArg(argList[i],XtNbackground,white);	i++;

	switch (scrollType)
	{
		case NOSCROLL:
			break;
		case VERTONLY:
			XtSetArg(argList[i],XtNtextOptions,
					scrollVertical); i++;
			break;
		case HORIZONLY:
			XtSetArg(argList[i],XtNtextOptions,
					scrollHorizontal); i++;
			break;
		case VERTHORIZ:
			XtSetArg(argList[i],XtNtextOptions,
				scrollHorizontal|scrollVertical); i++;
			break;
	}
	if (textSelect == SELECTABLE)
		XtSetArg(argList[i],XtNsensitive,TRUE);
	else
		XtSetArg(argList[i],XtNsensitive,FALSE);
	i++;
	if (textSource == STRINGSOURCE)
	{
		XtSetArg(argList[i],XtNlength,1000);	i++;
		XtSetArg(argList[i],XtNstring,string);	i++;
                XtSetArg(argList[i],XtNeditType,XttextEdit);  i++;
		textWind = XtCreateManagedWidget(shellName,
				asciiStringWidgetClass, parent, argList, i);
	}
	else
	{
                XtSetArg(argList[i],XtNeditType,XttextAppend);  i++;
		if (string)
			XtSetArg(argList[i],XtNfile,string);	i++;
		textWind = XtCreateManagedWidget(shellName,
					asciiDiskWidgetClass, parent, argList, i);
	}

	XtTextEnableRedisplay(textWind);

	return(textWind);
}
