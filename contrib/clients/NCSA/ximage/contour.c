
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

/* Author: Dave Whittington */
/*  All new contour plot -- for faster execution speeds and extra  */
/*  compactness.  Now available in convenient "FamileePaks".	   */

#include	<X11/Xlib.h>
#include	<X11/Intrinsic.h>

#define	data(x,y)	(unsigned char) ri[y*xmax + x]

void Updt_Contour(w,ri,cl,xmax,ymax,px,py,xdim,ydim,fore,back)
Widget	w;
char	*ri;
unsigned char	cl[];
int	xmax,ymax,px,py,xdim,ydim;
unsigned long	fore,back;
{
int	x,y,count;
unsigned char	d0,d1,d2;
Display	*disp;
Drawable d;
GC	the_gc;
XGCValues values;
Boolean	plotMe;

/*	set up XWindows specific stuff...	*/

	disp = XtDisplay(w);
	d = XtWindow(w);
	values.foreground = fore;
	values.background = back;
	the_gc = XtGetGC(w,(GCForeground | GCBackground),&values);

	XSetForeground(disp,the_gc,fore);
	XSetBackground(disp,the_gc,back);
/*	Now, check the bounds and start plotting	*/

	if (xdim > xmax) xdim = xmax;
	if (ydim > ymax) ydim = ymax;
	for (y = py; y < ydim; y++)
	{
		x = px;
		d1 = data(x,y);
		d2 = data(x,(y+1));
		while (x < xdim)
		{
			d0 = d1;
			d1 = data(x+1,y);
			d2 = data(x,(y+1));
			plotMe = (d0 >= cl[1]) && (d0 <= cl[cl[0]]);
			plotMe = plotMe || (d1 >= cl[1]) && (d1 <= cl[cl[0]]);
			plotMe = plotMe || (d2 >= cl[1]) && (d2 >= cl[cl[0]]);
			count = 1;
			while ((count <= cl[0]) && plotMe)
			{
				if (((d0 >= cl[count]) && (d1 < cl[count])) ||
				   ((d0 < cl[count]) && (d1 >= cl[count])) ||
				   ((d0 >= cl[count]) && (d2 < cl[count])) ||
				   ((d1 < cl[count]) && (d2 >= cl[count])))
				{
					XDrawPoint(disp,d,the_gc,x,y);
					count = cl[0];
				}
				count++;
				plotMe = (d0 <= cl[cl[0]]);
				plotMe = (d1 <= cl[cl[0]]) || plotMe;
				plotMe = (d2 <= cl[cl[0]]) || plotMe;
			}
			x++;
		}
	}
}
