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
 *	File:		gr_hist.c
 *	Contents:	View level routines for graphics module
 */

#include "gr_com.h"
#define XORIGIN 50
#define XOFFSET 30
#define YOFFSET 30

void
gr_HistExpose(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_HistWind_t	*histWin=(A_HistWind_t *)client_data;
	A_DsplWind_t	*dsplWin=histWin->parent;
	A_Data_t		*hdf=dsplWin->parent->parent->data;
	A_Axes_t		orient;
	float32			min=hdf->min,max=hdf->max,frac=hdf->rangeFrac;
	float32			***hdfdata=hdf->data;
	Widget			wid=histWin->imageWin;
	Display			*dpy=XtDisplay(wid);
	Window			win=XtWindow(wid);
	int				scr = DefaultScreen(dpy);
	GC				gc;
	XGCValues		gcvals;
	XFontStruct		*font_info;
	char			*fontname = "6x10",strng[10];
	int				i,dstart,dend,width,xpos,ypos,x,y,yoff,axis;
	long			val,black;
	unsigned char	nColors = (unsigned char)gr_color.nColors;

	gcvals.foreground = black = BlackPixel(dpy,scr);
	gc = XtGetGC(wid, GCForeground, &gcvals);
    if ((font_info = XLoadQueryFont(dpy,fontname)) != NULL)
		XSetFont(dpy,gc,font_info->fid);

	XClearWindow(dpy,win);
	orient=dsplWin->axesOrient;
	dstart = orient.plane;
	dend = dsplWin->zdim;
	xpos = histWin->xpos;
	ypos = histWin->ypos;
	width = dsplWin->width;
	axis = orient.axis;
	if (axis < 0) axis = -axis;
	yoff = gr_color.nColors+YOFFSET;
	sprintf(strng,"%d",dstart);
	XDrawString(dpy,win,gc,XORIGIN,yoff+YOFFSET/2,strng,
		(int)strlen(strng));
	sprintf(strng,"%d",dend-1);
	XDrawString(dpy,win,gc,histWin->xsize-XOFFSET/2,
		yoff+YOFFSET/2,strng,(int)strlen(strng));
	sprintf(strng,"%s-axis",axesLabels[axis-1]);
	XDrawString(dpy,win,gc,(int)(histWin->xsize/2),
		(int)(YOFFSET/4*3),strng,6);
	sprintf(strng,"%5.2f",hdf->min);
	XDrawString(dpy,win,gc,(int)2,
		(int)yoff,strng,strlen(strng));
	sprintf(strng,"%5.2f",hdf->max);
	XDrawString(dpy,win,gc,(int)2,
		YOFFSET+10,strng,strlen(strng));
	sprintf(strng,"%5.2f",(hdf->range*0.75+hdf->min));
	XDrawString(dpy,win,gc,(int)2,YOFFSET+(histWin->ysize-2*YOFFSET)/4,
		strng,strlen(strng));
	sprintf(strng,"%5.2f",(hdf->range*0.5+hdf->min));
	XDrawString(dpy,win,gc,(int)2,histWin->ysize/2,
		strng,strlen(strng));
	sprintf(strng,"%5.2f",(hdf->range*0.25+hdf->min));
	XDrawString(dpy,win,gc,(int)2,YOFFSET+(histWin->ysize-2*YOFFSET)*3/4,
		strng,strlen(strng));
	XDrawRectangle(dpy,win,gc,XORIGIN,YOFFSET,histWin->xsize-2*XOFFSET,
		gr_color.nColors);

	x = XORIGIN;
	switch (orient.axis)
	{
		case -1:
		case  1:
			if ((orient.col == -2) || (orient.col == 2))
			for (i=dstart;i<dend;i++)
			{
				val=XDS_CNV2PIX(hdfdata[i][xpos][ypos],min,max,frac,nColors);
				y = yoff-(int)val;
				XSetForeground(dpy,gc,val);
				XFillRectangle(dpy,win,gc,x,y,width,(int)val);
				if (width > 1)
				{
					XSetForeground(dpy,gc,black);
					XDrawRectangle(dpy,win,gc,x,y,width,(int)val);
				}
				x += width;
			}
			else
			for (i=dstart;i<dend;i++)
			{
				val=XDS_CNV2PIX(hdfdata[i][ypos][xpos],min,max,frac,nColors);
				XSetForeground(dpy,gc,val);
				y = yoff-(int)val;
				XFillRectangle(dpy,win,gc,x,y,width,(int)val);
				if (width > 1)
				{
					XSetForeground(dpy,gc,black);
					XDrawRectangle(dpy,win,gc,x,y,width,(int)val);
				}
				x += width;
			}
		break;
		case -2:
		case  2:
			if ((orient.col == -1) || (orient.col == 1))
			for (i=dstart;i<dend;i++)
			{
				val=XDS_CNV2PIX(hdfdata[xpos][i][ypos],min,max,frac,nColors);
				XSetForeground(dpy,gc,val);
				y = yoff-(int)val;
				XFillRectangle(dpy,win,gc,x,y,width,(int)val);
				if (width > 1)
				{
					XSetForeground(dpy,gc,black);
					XDrawRectangle(dpy,win,gc,x,y,width,(int)val);
				}
				x += width;
			}
			else
			for (i=dstart;i<dend;i++)
			{
				val=XDS_CNV2PIX(hdfdata[ypos][i][xpos],min,max,frac,nColors);
				XSetForeground(dpy,gc,val);
				y = yoff-(int)val;
				XFillRectangle(dpy,win,gc,x,y,width,(int)val);
				if (width > 1)
				{
					XSetForeground(dpy,gc,black);
					XDrawRectangle(dpy,win,gc,x,y,width,(int)val);
				}
				x += width;
			}
		break;
		case -3:
		case  3:
			if ((orient.col == -1) || (orient.col == 1))
			for (i=dstart;i<dend;i++)
			{
				val=XDS_CNV2PIX(hdfdata[xpos][ypos][i],min,max,frac,nColors);
				XSetForeground(dpy,gc,val);
				y = yoff-(int)val;
				XFillRectangle(dpy,win,gc,x,y,width,(int)val);
				if (width > 1)
				{
					XSetForeground(dpy,gc,black);
					XDrawRectangle(dpy,win,gc,x,y,width,(int)val);
				}
				x += width;
			}
			else
			for (i=dstart;i<dend;i++)
			{
				val=XDS_CNV2PIX(hdfdata[ypos][xpos][i],min,max,frac,nColors);
				XSetForeground(dpy,gc,val);
				y = yoff-(int)val;
				XFillRectangle(dpy,win,gc,x,y,width,(int)val);
				if (width > 1)
				{
					XSetForeground(dpy,gc,black);
					XDrawRectangle(dpy,win,gc,x,y,width,(int)val);
				}
				x += width;
			}
		break;
	}
	XSetForeground(dpy,gc,black);
	XDrawLine(dpy,win,gc,XORIGIN,YOFFSET+(histWin->ysize-2*YOFFSET)/4,
		histWin->xsize-10,YOFFSET+(histWin->ysize-2*YOFFSET)/4);
	XDrawLine(dpy,win,gc,XORIGIN,histWin->ysize/2,
		histWin->xsize-10,histWin->ysize/2);
	XDrawLine(dpy,win,gc,XORIGIN,YOFFSET+(histWin->ysize-2*YOFFSET)/4*3,
		histWin->xsize-10,YOFFSET+(histWin->ysize-2*YOFFSET)/4*3);
}
