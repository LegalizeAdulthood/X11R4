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
 *	File:		gr_xwi.c
 *	Contents:	Manipulation functions for X widgets
 * Author: Eng-Whatt Toh
 */

/*#include "gr_com.h"*/
#include <stdio.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Cardinals.h>

#include <Xw/Xw.h>
#include "gr_xwi.h"
#include <X11/IntrinsicP.h>
#include <X11/BoxP.h>
#include <X11/DialogP.h>
#include <X11/ViewportP.h>
#include <Xw/XwP.h>
#include <Xw/ArrowP.h>
#include <Xw/ScrollBarP.h>
#include <Xw/BBoardP.h>
#include <Xw/SWindowP.h>
#include <Xw/ValuatorP.h>

void
gr_WidgetSensitive(wid,sen)
Widget wid;
Boolean sen;
{
	Arg		argList[3];
	int		i=0;

	XtSetArg(argList[i],XtNsensitive,sen); i++;
	XtSetValues(wid,argList,i);
}

void
gr_WidgetCursor(wid,cursorfont)
Widget wid;
int cursorfont;
{
	Cursor cursor;
	Window win = XtWindow(wid);
	Display *dpy = XtDisplay(wid);

	if (win)
	{
		cursor = XCreateFontCursor(dpy,cursorfont);
		XDefineCursor(dpy,win,cursor);
	}

	return;
}

void
gr_ListChange(wid,list,nitems,longest,resize)
Widget wid;
String *list;
int nitems,longest;
Boolean resize;
{
	XtListChange(wid,list,nitems,longest,resize);
}

void
gr_ListHighlight(wid,item)
Widget wid;
int item;
{
	XtListHighlight(wid,item);
}

char
*gr_ListgetStruct(wid,ind)
Widget wid;
int	   *ind;
{
	XtListReturnStruct	*strngStruct;

	strngStruct = XtListShowCurrent(wid);
	*ind = strngStruct->index;

	return(strngStruct->string);
}

void
gr_TextReplace(wid,startPos,endPos,strng)
Widget wid;
char *strng;
long startPos,endPos;
{
	XtTextBlock	text;

	text.firstPos = 0;
	text.length = strlen(strng);
	text.ptr = strng;
	text.format = FMT8BIT;

	XtTextReplace(wid,startPos,endPos,&text);
}

void
gr_TextSetInsertionPoint(wid,atPos)
Widget wid;
long   atPos;
{
	XtTextSetInsertionPoint(wid,atPos);
}

Widget
gr_DialogGetTextWind(w)
Widget w;
{
	return (((DialogWidget)w)->dialog.valueW);
}

char
*gr_DialogGetValue(wid)
Widget wid;
{
	return(XtDialogGetValueString(wid));
}

void
gr_DialogSetValue(wid,strng)
Widget wid;
char *strng;
{
	Widget textWid;
	String oldStrng;

	textWid = ((DialogWidget)wid)->dialog.valueW;
	oldStrng = ((DialogWidget)wid)->dialog.value;
	gr_TextReplace(textWid,0,strlen(oldStrng),strng);
}

void
gr_LabelSetValue(wid,strng)
Widget wid;
char *strng;
{
	Arg		argList[2];

	XtSetArg(argList[0],XtNlabel,strng);
	XtSetValues(wid,argList,1);
}

void
gr_StaticRasterSetImage(wid,image)
Widget wid;
XImage *image;
{
	Arg		argList[3];
	int		i=0;

	XtSetArg(argList[i],XtNsRimage,image); i++;
	XtSetValues(wid,argList,i);
}

void
gr_SliderSetValue(wid,pos)
Widget wid;
int	   pos;
{
	Arg		argList[3];
	Cardinal i=0;

	XtSetArg(argList[i],XtNsliderOrigin,pos); i++;
	XtSetValues(wid,argList,i);
}

/*
	This function is called to translate the position of the child in
	a scrolledWindowWidget so that the point (xpos,ypos) of the child
	is centered in the scrolled window of size: xsize by ysize.
	The actual resulting coordinates of the translation is returned in
	(xpos,ypos).  They may then be used to update other children based 
	on the same horizontal and vertical slider values as needed.
*/
void
gr_VPortMoveChild(w,xpos,ypos,xsize,ysize)
Widget w;
int	   *xpos,*ypos,xsize,ysize;
{
	XwSWindowWidget		 sw = (XwSWindowWidget)w;
	XwScrollBarWidget	 vscroll,hscroll;
	Widget	 child;
	int		 vnewPos,hnewPos,vorg,horg;
	int		 vmin,vmax,vext,vsize;
	int		 hmin,hmax,hext,hsize;

	vscroll = sw->swindow.vScrollBar;
	hscroll = sw->swindow.hScrollBar;
	child  = sw->swindow.bbChild;

	/* Calculate the new vertical (y) position of the child */
	if (vscroll != NULL)
	{
		vmin = sw->swindow.vmin;
		vmax = sw->swindow.vmax;
		vext = sw->swindow.vExtent;
		vorg = sw->swindow.vOrigin;
		vsize = vmax - vmin;
		vnewPos = ((float)*ypos/(float)ysize)*(float)vsize;
		if ((vnewPos < vorg+20) ||
			(vnewPos > (vorg+vext-20)))
		{
			vnewPos = vnewPos - (float)((float)vext/(float)2.0);
			if (vnewPos < vmin)
				vnewPos = vmin;
			else
			if (vnewPos > (vmax-vext-1))
				vnewPos = vmax-vext-1;
			sw->swindow.vOrigin = vnewPos;
			gr_SliderSetValue((Widget)vscroll,vnewPos);
		}
		else
			vnewPos = vorg;
	}

	/* Calculate the new horizontal (x) position of the child */
	if (hscroll != NULL)
	{
		hmin = sw->swindow.hmin;
		hmax = sw->swindow.hmax;
		hext = sw->swindow.hExtent;
		horg = sw->swindow.hOrigin;
		hsize = hmax - hmin;
		hnewPos = ((float)*xpos/(float)xsize)*(float)hsize;
		if ((hnewPos < horg+20) ||
			(hnewPos > (horg+hext-20)))
		{
			hnewPos = hnewPos - (float)((float)hext/(float)2.0);
			if (hnewPos<hmin)
				hnewPos = hmin;
			else
			if (hnewPos>(hmax-hext-1))
				hnewPos = hmax-hext-1;
			sw->swindow.hOrigin = hnewPos;
			gr_SliderSetValue((Widget)hscroll,hnewPos);
		}
		else
			hnewPos = horg;
	}

	/* Translate child in the scrolledWindow viewport */
	if ((vscroll != NULL) && (hscroll != NULL) && (child != NULL))
	{
		*xpos = hnewPos;
		*ypos = vnewPos;
		XtMoveWidget(child,(Position)-hnewPos,(Position)-vnewPos);
	}
}

/*
	This function moves the first child of a box widget horizontally
	in the box to effect a horizontal scroll on the contents of the
	box.
*/
void
gr_VPortHoriMove(w,newpos)
Widget w;
int	   newpos;
{
	BoxWidget	bbw = (BoxWidget)w;
	register Widget	child = bbw->composite.children[0];
	int		 pos;

	pos = newpos + 1;
	if (pos < 1)
		pos = 1;
	else
	if (pos > child->core.width)
		pos = child->core.width;
	child->core.x = pos;

	XtMoveWidget(child, (Position)-pos, (Position)child->core.y);
}

/*
	This function moves the first child of a box widget vertically
	in the box to effect a vertical scroll on the contents of the
	box.
*/
void
gr_VPortVertMove(w,newpos)
Widget w;
int	   newpos;
{
	BoxWidget	bbw = (BoxWidget)w;
	register Widget	child = bbw->composite.children[0];
	int		 pos;

	pos	= newpos + 1;
	if (pos < 1)
		pos = 1;
	else
	if (pos > child->core.height)
		pos = child->core.height;
	child->core.y = pos;

	XtMoveWidget(child, (Position)child->core.x, (Position)-pos);
}
