/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 *	All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 * $Header: xinqpixel.c,v 4.0 89/08/31 16:29:04 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/x/RCS/xinqpixel.c,v $
 *
 * $Log:	xinqpixel.c,v $
 * Revision 4.0  89/08/31  16:29:04  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.37  89/06/05  10:26:50  bruce
 * DCR# d1:	Changed include file name from gks_implement.h to gks_implem.h
 * 		for the AIX compiler.  Also added a display argument to all
 * 		calls to SIGIO functions.
 * 
 * Revision 3.36  89/03/15  19:18:59  amy
 * PTR c1162	xInqPixelArray and xInqPixel:  verify that pixel inquiries 
 * 		are within the boundaries of the root window or
 * 		the values returned are invalid.
 * 
 * Revision 3.35  89/02/04  15:29:26  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.34  88/12/14  15:09:36  amy
 * PTR c1126	xInqPixelarray & xInqPixel:  Undid changes originally 
 * 		made for PTR c1126.
 * 		Changed xInqPixelarray because XGetPixel returns the pixel
 * 		in an un-normalized form.
 * 
 * Revision 3.33  88/12/08  14:19:16  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * Changed spelling of color to colour.
 * 
 * Revision 3.32  88/11/02  13:01:33  amy
 * PTR c1126	xInqPixel:  made code for ZPixmap and XYPixmap look the same
 * 		as in xInqPixelArray.
 * 
 * Revision 3.31  88/08/19  07:06:14  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:52:03  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: xinqpixel.c,v 4.0 89/08/31 16:29:04 amy Exp $";

#include "gks_implem.h"		/* d1 */


#define one 0xFF000000
#define two 0x00FF0000
#define tre 0x0000FF00
#define fou 0x000000FF


/* xXgksInqPixelarrayDim(ws, rect, dim)	INQUIRY PIXEL ARRAY DIMENSIONS
 *
 * WS_STATE_PTR ws;		workstation state list pointer.
 * Grect *rect;			rectangele pointer.
 * Gipoint *dim;		OUT dimensions of the pixel array.
 */


xXgksInqPixelarrayDim(ws, rect, dim)				/*c1147*/
WS_STATE_PTR ws;
Grect *rect;
Gipoint *dim;
{	Gpoint p1, p2;

	XPoint xp1, xp2;

	XgksSIGIO_OFF(ws->dpy);				/* d1 *//*c1147*/

/* transformation the rectangle from WC to X */
	WcToNdc(&(rect->ll), &p1);
	WcToNdc(&(rect->ur), &p2);

	NdcToX(ws, &p1, &xp1);
	NdcToX(ws, &p2, &xp2);

/* return the values of the dimensions */
	dim->x = (unsigned)(xp2.x - xp1.x + 1);
	dim->y = (unsigned)(xp1.y - xp2.y + 1);

	XgksSIGIO_ON(ws->dpy);				/* d1 *//*c1147*/

}


/*
 * xXgksInqPixelarray(ws, point, dim, pxarr)	INQUIRY PIXEL ARRAY
 *
 * WS_STATE_PTR ws;		workstation state list pointer.
 * Gpoint *point;		pixel array location pointer.
 * Gipoint *dim;		dimensions of the pixel array.
 * Gpxarray *pxarr;		OUT pixel array.
 *
 */


xXgksInqPixelarray(ws, point, dim, pxarr)			/*c1147*/
WS_STATE_PTR ws;
Gpoint *point;
Gipoint *dim;
Gpxarray *pxarr;
{
 	Gpoint p1;

	Display *dpy;
	Window win;
	XWindowAttributes rootattr;		/* c1162 */
	XWindowAttributes wsattr;		/* c1162 */
	XPoint xp1;
	XImage *image;

	Gint *intp, x, y, xx, yy;
	Gint x1, y1, x2, y2;
	Gint maxx, maxy;			/* c1162 */

	XgksSIGIO_OFF(ws->dpy);			/* d1 *//*c1147*/

/* transformation the rectangle from WC to X */
	WcToNdc(point, &p1);

	NdcToX(ws, &p1, &xp1);

/* get the image from the X--server */
	dpy = ws->dpy;
	win = ws->win;

/* find boundaries of screen in case window is not totally on screen   c1162 */
	XGetWindowAttributes (dpy, DefaultRootWindow (dpy), &rootattr);
	XGetWindowAttributes (dpy, win, &wsattr);
	maxx =  ((wsattr.x + ws->wbound.x + wsattr.border_width + 1) > rootattr.width) ? 
		(rootattr.width-wsattr.x-wsattr.border_width-1) : ws->wbound.x;
	maxy =  ((wsattr.y + ws->wbound.y + wsattr.border_width + 1) > rootattr.height) ? 
		(rootattr.height-wsattr.y-wsattr.border_width-1) : ws->wbound.y;
	
	pxarr->covalid = GABSENT;

	if (xp1.x < 0) {
		x1 = 0;
		pxarr->covalid = GPRESENT;
	}
	else if (xp1.x > maxx) {		/* c1162 */
		x1 = maxx;			/* c1162 */
		pxarr->covalid = GPRESENT;
	}
	     else 
		x1 = xp1.x; 
		
	if (xp1.y < 0) {
		y1 = 0;
		pxarr->covalid = GPRESENT;
	}
	else if (xp1.y > maxy) {		/* c1162 */
		y1 = maxy;			/* c1162 */
		pxarr->covalid = GPRESENT;
	}
	     else 
		y1 = xp1.y; 
		
	if ((xp1.x + dim->x) < 0) {
		x2 = 0;
		pxarr->covalid = GPRESENT;
	}
	else if ((xp1.x + dim->x) > maxx) {	/* c1162 */
		x2 = maxx;			/* c1162 */
		pxarr->covalid = GPRESENT;
	}
	     else 
		x2 = xp1.x + dim->x; 
		
	if ((xp1.y + dim->y) < 0) {
		y2 = 0;
		pxarr->covalid = GPRESENT;
	}
	else if ((xp1.y + dim->y) > maxy) {	/* c1162 */
		y2 = maxy;			/* c1162 */
		pxarr->covalid = GPRESENT;
	}
	     else 
		y2 = xp1.y + dim->y; 
		
	/* removed code for Z pixmap - not needed with new server  PTR# c1126  */
	                                         /* for XYPixmap */

	image = XGetImage(dpy, win, x1, y1, x2-x1+1, y2-y1+1, AllPlanes, XYPixmap);

/* return the values of the dimensions */
	pxarr->array = (Gint *)malloc( dim->x * dim->y * sizeof(int) );
	GKSERROR( (pxarr->array == NULL), 300, errxInqPixelarray);
	intp = pxarr->array;

	if (pxarr->covalid == GPRESENT) {
	for (y=0; y<dim->y; y++)
		for (x=0; x<dim->x; x++) {
			xx = xp1.x + x;
			yy = xp1.y + y;
			if (xx >= x1 && xx <= x2 && yy >= y1 && yy <= y2)
				*intp = (Gint)XGetPixel(image, xx-x1, yy-y1);
			else
				*intp = -1;
			intp++;
		}
	}
	else
	for (y=0; y<dim->y; y++)
		for (x=0; x<dim->x; x++) {
			*intp = (Gint)XGetPixel(image, x, y);
			intp++;
		}


	XgksSIGIO_ON(ws->dpy);			/* d1 *//*c1147*/

	return(OK);

}


/*
 * xXgksInqPixel(ws, ppoint, pix)	INQUIRY PIXEL
 *
 * WS_STATE_PTR ws;		workstation state list pointer.
 * Gpoint *ppoint;		pixel location pointer.
 * Gint *pix;  			OUT pixel colour.
 *
 */


xXgksInqPixel(ws, ppoint, pix)					/*c1147*/
WS_STATE_PTR ws;
Gpoint *ppoint;
Gint *pix;
{
 	Gpoint p1;

	Display *dpy;
	Window win;
	XWindowAttributes rootattr;	/* c1162 */
	XWindowAttributes wsattr;	/* c1162 */
	XPoint xp1;
	XImage *image;
	Gint maxx, maxy;		/* c1162 */


	XgksSIGIO_OFF(ws->dpy);			/* d1 *//*c1147*/

/* transformation the rectangle from WC to X */
	WcToNdc(ppoint, &p1);

	NdcToX(ws, &p1, &xp1);

/* get the image from the X--server and set the returned value */
	dpy = ws->dpy;
	win = ws->win;


	
/* find boundaries of screen in case window is not totally on screen    c1162 */
	XGetWindowAttributes (dpy, DefaultRootWindow (dpy), &rootattr);
	XGetWindowAttributes (dpy, win, &wsattr);
	maxx =  ((wsattr.x + ws->wbound.x + wsattr.border_width + 1) > rootattr.width) ? 
		(rootattr.width-wsattr.x-wsattr.border_width-1) : ws->wbound.x;
	maxy =  ((wsattr.y + ws->wbound.y + wsattr.border_width + 1) > rootattr.height) ? 
		(rootattr.height-wsattr.y-wsattr.border_width-1) : ws->wbound.y;
	
	if (xp1.x >= 0 && xp1.x <= maxx && xp1.y >= 0 && xp1.y <= maxy) {	/* c1162 */
	    /* removed code for Z pixmap - not needed with new server  PTR# c1126  */
		image = XGetImage(dpy, win, xp1.x, xp1.y, 1, 1, AllPlanes, XYPixmap);
		*pix = XGetPixel(image, 0, 0);
	}
	else
		*pix = -1;

	XgksSIGIO_ON(dpy);		/* d1 *//*c1147*/
}

