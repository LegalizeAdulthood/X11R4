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

/*	ImageTool "Shade" plot   --				*/
/*	makes an n "grey-level" black and white plot of data	*/
/* 	Author: Dave Whittington */

#include	<stdio.h>
#include	<X11/Xlib.h>
#include	<X11/Intrinsic.h>
#include	"patterns.h"

#ifndef	TRUE
#define TRUE 1
#endif
#define FALSE 0
#define	DIVISOR	(8 - SHIFTBITS)
#define DONERTN	((unsigned char) 255)

typedef	struct	{
	int	x;
	int	y;
	int	width;
	int	height;
	} Rect;

extern	unsigned long	black,white;
static	Display	*disp;
static	Drawable d;
static	GC	the_gc;
static	char	*ri;
static	Pixmap	patList[NUMPATTERNS];
static	Rect	plotRect;
static	short	xmax,ymax;
static	float	dimv,dimh;
static	short	lmar,tmar;

static	unsigned char data(x,y)
int	x,y;
{
	return(ri[y*xmax+x]);
}

void PlotRect(r,pat)
Rect	*r;
unsigned char	pat;
{
	XSetStipple(disp,the_gc,patList[pat]);
	XFillRectangle(disp,d,the_gc,r->x,r->y,r->width,r->height);
}

void InitShade(w)
Widget w;
{
	int	count;

	disp = XtDisplay(w);
	d = XtWindow(w);
	for (count = 0; count < NUMPATTERNS; count++)
	patList[count] = XCreateBitmapFromData(disp,d,pats[count],16,16);
}

unsigned char horizontal(y,xl,xr)
short	y,xl,xr;
{
short	count;
unsigned char	old,temp;
Boolean	biff;
Rect	r;

	old = (data(xl,y)>>DIVISOR);
	biff = TRUE;
	for(count = 1; count<(xr-xl); count++)
	{
		temp = (data((xl+count),y)>>DIVISOR);
		if (old != temp) biff = FALSE;
		old = temp;
	}
	if (biff) return(old);
	else
	{
		for(count = 0; count <(xr-xl); count++)
		{
			r.y = ((y-plotRect.y)*dimv + tmar);
			r.height = dimv;
			r.x = ((xl+count-plotRect.x)*dimh + lmar);
			r.width = dimh;
			PlotRect(&r,data((xl,count),y)>>DIVISOR);
		}
		return(DONERTN);
	}
}

unsigned char vertical(x,yt,yb)
short	x,yt,yb;
{
short	count;
unsigned char	old,temp;
Boolean	biff;
Rect	r;

	old = (data(x,yt)>>DIVISOR);
	biff = TRUE;
	for(count = 1; count<(yb-yt); count++)
	{
		temp = (data(x,(yt+count))>>DIVISOR);
		if (old != temp) biff = FALSE;
		old = temp;
	}
	if (biff) return(old);
	else
	{
		for(count = 0; count <(yb-yt); count++)
		{
			r.y = ((yt+count-plotRect.y)*dimv + tmar);
			r.height = dimv;
			r.x = ((x-plotRect.x)*dimh + lmar);
			r.width = dimh;
			PlotRect(&r,data(x,(yt+count))>>DIVISOR);
		}
		return(DONERTN);
	}
}

unsigned char shadePlot(left,top,right,bottom)
short	left,top,right,bottom;
{
short	lwidth,rwidth,theight,bheight;
unsigned char	ul,ur,ll,lr;
Rect	m;

	lwidth = (right-left)/2;
	rwidth = (right-left+1)/2;
	theight = (bottom-top)/2;
	bheight = (bottom-top+1)/2;

	if (lwidth == 1)
	{
		if (theight == 1)
		{
			ul = (data(left,top)>>DIVISOR);
			if (rwidth == 1)
			{
				ur = (data((left+1),top)>>DIVISOR);
				if (bheight == 1)
				{
					ll = (data(left,(top+1))>>DIVISOR);
					lr = (data((left+1),(top+1))>>DIVISOR);
				}
				else
				{
					ll = vertical(left,(top+1),bottom);
					lr = vertical((left+1),(top+1),bottom);
				}
			}
			else
			{
				ur = horizontal(top,(left+1),right);
				if (bheight == 1)
				{
					ll = data(left,(top+1))>>DIVISOR;
					lr = horizontal((top+1),(left+1),right);
				}
				else
				{
					ll = vertical(left,(top+1),bottom);
					lr = shadePlot((left+1),(top+1),right,bottom);
				}
			}
		}
		else
		{
			ul = vertical(left,top,(top+theight));
			ll = vertical(left,(top+theight),bottom);
			if (rwidth != 1)
			{
				ur = shadePlot((left+1),top,right,(top+theight));
				lr = shadePlot((left+1),(top+theight),right,bottom);
			}
			else
			{
				ur = vertical((left+1),top,(top+theight));
				lr = vertical((left+1),(top+theight),bottom);
			}
		}
	}
	else
	{
		if (theight == 1)
		{
			ul = horizontal(top,left,(left+lwidth));
			ur = horizontal(top,(left+lwidth),right);
			if (bheight != 1)
			{
				ll = shadePlot(left,(top+1),(left+lwidth),bottom);
				lr = shadePlot((left+lwidth),(top+1),right,bottom);
			}
			else
			{
				ll = horizontal((top+1),left,(left+lwidth));
				lr = horizontal((top+1),(left+lwidth),right);
			}
		}
		else
		{
			ul = shadePlot(left,top,(left+lwidth),(top+theight));
			ur = shadePlot((left+lwidth),top,right,(top+theight));
			ll = shadePlot(left,(top + theight),(left + lwidth),bottom);
			lr = shadePlot((left+lwidth),(top+theight),right,bottom);
		}
	}

	if ((ll == ul) && (ur == ul) && (lr == ul)) return(ul);
	else
	{
		if (ul != DONERTN)
		{
			m.y = ((top-plotRect.y)*dimv + tmar);
			m.x = ((left-plotRect.x)*dimh + lmar);
			m.width = lwidth*dimh;
			m.height = theight*dimv;
			PlotRect(&m,ul);
		}
		if (ur != DONERTN)
		{
			m.y = ((top-plotRect.y)*dimv + tmar);
			m.x = ((left+lwidth-plotRect.x)*dimh + lmar);
			m.width = (right-left-lwidth)*dimh;
			m.height = theight*dimv;
			PlotRect(&m,ur);
		}
		if (ll != DONERTN)
		{
			m.y = ((top+theight-plotRect.y)*dimv + tmar);
			m.x = ((left-plotRect.x)*dimh + lmar);
			m.width = lwidth*dimh;
			m.height = (bottom-top-theight)*dimv;
			PlotRect(&m,ll);
		}
		if (lr != DONERTN)
		{
			m.y = ((top+theight-plotRect.y)*dimv + tmar);
			m.x = ((left+lwidth-plotRect.x)*dimh + lmar);
			m.width = (right-left-lwidth)*dimh;
			m.height = (bottom-top-theight)*dimv;
			PlotRect(&m,lr);
		}
		return(DONERTN);
	}
}


void Updt_Shaded(w,data,xdim,ydim,lx,ty,rx,by)
Widget w;
char *data;
int xdim,ydim,lx,ty,rx,by;
{
	unsigned char	myPat;
	XGCValues	values;

	xmax = xdim;
	ymax = ydim;
	ri = data;

	disp = XtDisplay(w);
	d = XtWindow(w);
	InitShade(w);

	values.stipple = patList[5];
	values.foreground = black;
	values.background = white;
	the_gc = XCreateGC(disp,d,(GCStipple | GCForeground | GCBackground
		),&values);
	XSetFillStyle(disp,the_gc,FillOpaqueStippled);
	dimv = 1;
	dimh = 1;
	plotRect.x = lx;
	plotRect.y = ty;
	plotRect.width = rx - lx;
	plotRect.height = by - ty;
	if (plotRect.width > xdim - lx) plotRect.width = xdim - lx;
	if (plotRect.width < 0) plotRect.width = 1;
	if (plotRect.height > ydim - ty) plotRect.height = ydim - ty;
	if (plotRect.height < 0) plotRect.height = 1;

	lmar = lx;
	tmar = ty;

	myPat = shadePlot(plotRect.x,plotRect.y,plotRect.x + plotRect.width,
		plotRect.y + plotRect.height);
	if (myPat != DONERTN) PlotRect(&plotRect,myPat);
	XFreeGC(disp,the_gc);
}
 
