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
 *	File:		gr_xwi.c
 *	Contents:	Manipulation functions for X widgets
 *	The functions defined are specific to X-windows Athena and HP widget
 *	sets.  Some of them actually goes into the internals of the widgets
 *	to gain capabilities that the widgets would not otherwise have.
 */

#include "gr_com.h"
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


/*
 *	Attempt to get a PseudoColor 8-plane visual if possible
 */
Visual
*gr_GetVisual(dpy,scr)
Display	*dpy;
int		scr;
{
	XVisualInfo	vinfo;

	if (XMatchVisualInfo(dpy,scr,(int)8,PseudoColor,&vinfo))
		return(vinfo.visual);
	else
		return(DefaultVisual(dpy,scr));
}

/*
 * Turn a widget On or Off
 */
void
gr_WidgetSensitive(wid,sen)
Widget wid;
Boolean sen;
{
	XtSetArg(argList[0],XtNsensitive,sen);
	XtSetValues(wid,argList,1);
}


/*
 * Turn a Button On or Off, mostly for HP toggles
 */
void
gr_ButtonSet(wid,isOn)
Widget wid;
Boolean isOn;
{
	XtSetArg(argList[0],XtNset,isOn);
	XtSetValues(wid,argList,1);
}


/*
 * Change a cursor font for a shell window
 */
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
		XFlush(dpy);
	}

	return;
}


/*
 * Change a cursor font for a special widget
 */
void
gr_WidgetCursor1(wid,cursorfont)
Widget wid;
int cursorfont;
{
	Cursor cursor;
	Display *dpy = XtDisplay(wid);

	cursor = XCreateFontCursor(dpy,cursorfont);
	XtSetArg(argList[0],XtNcursor,cursor);
	XtSetValues(wid,argList,1);

	return;
}


/*
 * Change the contents of an Athena list widget
 */
void
gr_ListChange(wid,list,nitems,longest,resize)
Widget wid;
String *list;
int nitems,longest;
Boolean resize;
{
	XtListHighlight(wid,0);
	XtListChange(wid,list,nitems,longest,resize);
	XtListHighlight(wid,0);
}


/*
 * Highlight an entry in an Athena list widget
 */
void
gr_ListHighlight(wid,item)
Widget wid;
int item;
{
	XtListHighlight(wid,item);
}


/*
 * Get the string and index of the selected list item in Athena list
 */
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


/*
 * Replace text in an Athena text widget
 */
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


/*
 * Set insertion pointer in Athena text widget
 */
void
gr_TextSetInsertionPoint(wid,atPos)
Widget wid;
long   atPos;
{
	XtTextSetInsertionPoint(wid,atPos);
}


/*
 * Get internal Athena text widget from Athena dialog widget
 */
Widget
gr_DialogGetTextWind(w)
Widget w;
{
	return (((DialogWidget)w)->dialog.valueW);
}


/*
 * Get string from Athena dialog widget
 */
char
*gr_DialogGetValue(wid)
Widget wid;
{
	return(XtDialogGetValueString(wid));
}


/*
 * Set string in Athena dialog widget
 */
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


/*
 * Set label string in Athena Label/Command widget
 */
void
gr_LabelSetValue(wid,strng)
Widget wid;
char *strng;
{
	XtSetArg(argList[0],XtNlabel,strng);
	XtSetValues(wid,argList,1);
}


/*
 * Set image for HP static raster image
 */
void
gr_StaticRasterSetImage(wid,image)
Widget wid;
XImage *image;
{
	XtSetArg(argList[0],XtNsRimage,image);
	XtSetValues(wid,argList,1);
}


/*
 * Set slider origin in HP valuator widget
 */
void
gr_SliderSetValue(wid,pos)
Widget wid;
int	   pos;
{
	XtSetArg(argList[0],XtNsliderOrigin,pos);
	XtSetValues(wid,argList,1);
}


/*
 * Get slider origin in HP valuator widget
 */
int
gr_SliderGetValue(wid)
Widget wid;
{
	int pos;

	XtSetArg(argList[0],XtNsliderOrigin,&pos);
	XtGetValues(wid,argList,1);
	return(pos);
}


/*
 * Move child widget in HP scrolled window widget
 * This function is called to translate the position of the child in
 * a scrolledWindowWidget so that the point (xpos,ypos) of the child
 * is centered in the scrolled window of size: xsize by ysize.
 * The actual resulting coordinates of the translation is returned in
 * (xpos,ypos).  They may then be used to update other children based 
 * on the same horizontal and vertical slider values as needed.
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
		if ((vnewPos < vorg+10) ||
			(vnewPos > (vorg+vext-10)))
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
		if ((hnewPos < horg+10) ||
			(hnewPos > (horg+hext-10)))
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
 * This function moves the first child of a box widget horizontally
 * in the box to effect a horizontal scroll on the contents of the
 * box.
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
 * This function moves the first child of a box widget vertically
 * in the box to effect a vertical scroll on the contents of the
 * box.
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


/*
 *	Override existing key translations for text Widget in Dialog Widget.
 */
void
gr_DialogAddTrans(wid,trans)
Widget wid;
char   *trans;
{
	XtTranslations	trans_table;
	Widget			textWid=gr_DialogGetTextWind(wid);

	trans_table = XtParseTranslationTable(trans);

	XtOverrideTranslations(textWid,trans_table);
}
