/*
 *	Original Source: ./contrib/windowmgrs/twm/util.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*****************************************************************************/
/**       Copyright 1988 by Evans & Sutherland Computer Corporation,        **/
/**                          Salt Lake City, Utah                           **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    name  of Evans & Sutherland  not be used in advertising or publi-    **/
/**    city pertaining to distribution  of the software without  specif-    **/
/**    ic, written prior permission.                                        **/
/**                                                                         **/
/**    EVANS  & SUTHERLAND  DISCLAIMS  ALL  WARRANTIES  WITH  REGARD  TO    **/
/**    THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILI-    **/
/**    TY AND FITNESS, IN NO EVENT SHALL EVANS &  SUTHERLAND  BE  LIABLE    **/
/**    FOR  ANY  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY  DAM-    **/
/**    AGES  WHATSOEVER RESULTING FROM  LOSS OF USE,  DATA  OR  PROFITS,    **/
/**    WHETHER   IN  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS    **/
/**    ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE  OR PER-    **/
/**    FORMANCE OF THIS SOFTWARE.                                           **/
/*****************************************************************************/

/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/***********************************************************************
 *
 * $Header: util.c,v 1.2 88/10/15 19:12:02 jim Exp $
 *
 * utility routines for swm
 *
 * 28-Oct-87 Thomas E. LaStrange	File created
 *
 ***********************************************************************/

#ifndef lint
static char RCSinfo[]=
"$Header: util.c,v 1.2 88/10/15 19:12:02 jim Exp $";
#endif

#include <stdio.h>
#include "swm.h"
#include "util.h"
#include "gram.h"
#include "iconify.bm"

int ZoomCount = 8;		/* number of outlines to draw while zooming */

typedef struct ListRoot
{
    struct WList *first;
    struct WList *last;
    int count;
} ListRoot;


/***********************************************************************
 *
 *  Procedure:
 *	MoveOutline - move a window outline
 *
 *  Inputs:
 *	root	    - the window we are outlining
 *	x	    - upper left x coordinate
 *	y	    - upper left y coordinate
 *	width	    - the width of the rectangle
 *	height	    - the height of the rectangle
 *
 ***********************************************************************
 */

void
MoveOutline(root, x, y, width, height)
    Window root;
    int x, y, width, height;
{
    static int	lastx = 0;
    static int	lasty = 0;
    static int	lastWidth = 0;
    static int	lastHeight = 0;
    int		xl, xr, yt, yb;
    int         edgewidth;
    XSegment	outline[16];
    XSegment	*r = outline;
    
    if (x == lastx && y == lasty && width == lastWidth && height == lastHeight)
	return;
    
    edgewidth = EdgeWidth;
    if (TempContext == C_ICON)
       edgewidth = 0;

    xl = lastx;
    xr = lastx + lastWidth - 1;
    yt = lasty;
    yb = lasty + lastHeight - 1;

    if (lastWidth || lastHeight)
    {
	r->x1 = xl;
	r->y1 = yt;
	r->x2 = xr;
	r++->y2 = yt;

	r->x1 = xl;
	r->y1 = yb;
	r->x2 = xr;
	r++->y2 = yb;

	r->x1 = xl;
	r->y1 = yt;
	r->x2 = xl;
	r++->y2 = yb;

	r->x1 = xr;
	r->y1 = yt;
	r->x2 = xr;
	r++->y2 = yb;

        if (edgewidth != 0){
        r->x1 = xl+edgewidth;
        r->y1 = yt+edgewidth;
        r->x2 = xr-edgewidth;
        r++->y2 = yt+edgewidth;

        r->x1 = xl+edgewidth;
        r->y1 = yb-edgewidth;
        r->x2 = xr-edgewidth;
        r++->y2 = yb-edgewidth;

        r->x1 = xl+edgewidth;
        r->y1 = yt+edgewidth;
        r->x2 = xl+edgewidth;
        r++->y2 = yb-edgewidth;

        r->x1 = xr-edgewidth;
        r->y1 = yt+edgewidth;
        r->x2 = xr-edgewidth;
        r++->y2 = yb-edgewidth;}
    }

    lastx = x;
    lasty = y;
    lastWidth=width;
    lastHeight=height;
    xl = lastx;
    xr = lastx + lastWidth - 1;
    yt = lasty;
    yb = lasty + lastHeight - 1;

    if (lastWidth || lastHeight)
    {
	r->x1 = xl;
	r->y1 = yt;
	r->x2 = xr;
	r++->y2 = yt;

	r->x1 = xl;
	r->y1 = yb;
	r->x2 = xr;
	r++->y2 = yb;

	r->x1 = xl;
	r->y1 = yt;
	r->x2 = xl;
	r++->y2 = yb;

	r->x1 = xr;
	r->y1 = yt;
	r->x2 = xr;
	r++->y2 = yb;

        if (edgewidth != 0){
        r->x1 = xl+edgewidth;
        r->y1 = yt+edgewidth;
        r->x2 = xr-edgewidth;
        r++->y2 = yt+edgewidth;

        r->x1 = xl+edgewidth;
        r->y1 = yb-edgewidth;
        r->x2 = xr-edgewidth;
        r++->y2 = yb-edgewidth;

        r->x1 = xl+edgewidth;
        r->y1 = yt+edgewidth;
        r->x2 = xl+edgewidth;
        r++->y2 = yb-edgewidth;

        r->x1 = xr-edgewidth;
        r->y1 = yt+edgewidth;
        r->x2 = xr-edgewidth;
        r++->y2 = yb-edgewidth;}

    }
    if (r != outline)
    {
	XDrawSegments(dpy, root, DrawGC, outline, r - outline);
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	Zoom - zoom in or out of an icon
 *
 *  Inputs:
 *      tmp_win - SwmWindow
 *	wf	- window to zoom from
 *	wt	- window to zoom to
 *
 ***********************************************************************
 */

void
Zoom(tmp_win, wf, wt)
SwmWindow *tmp_win;
Window *wf, *wt;
{
    int fx, fy, fw, fh;
    int tx, ty, tw, th;
    int xl, yt, xr, yb;
    int dx, dy, dw, dh;
    int w, h, i;
    int frx, fry, trx, try;
    XSegment	outline[4];
    Window *children;
    int nchildren;
    int rfx, rfy, xx, yy;
    SwmWindow *tDesktop;

    if (!DoZoom)
	return;

    XQueryPointer(dpy, Root, &JunkRoot, &JunkChild, &JunkX, &JunkY,
	&xx, &yy, &JunkMask);
    GetMousePointer(tmp_win, &rfx, &rfy);

    XGetGeometry(dpy, wf, &JunkRoot, &fx, &fy, &fw, &fh, &JunkBW, &JunkDepth);
    XGetGeometry(dpy, wt, &JunkRoot, &tx, &ty, &tw, &th, &JunkBW, &JunkDepth);
    
    dx = (tx - fx) / ZoomCount;
    dy = (ty - fy) / ZoomCount;
    dw = (tw - fw) / ZoomCount;
    dh = (th - fh) / ZoomCount;

    xl = fx + (xx - rfx);
    yt = fy + (yy - rfy);
    xr = fx + fw + (xx - rfx);
    yb = fy + fh + (yy - rfy);
    w = fw;
    h = fh;

    for (i = 0; i < ZoomCount; i++)
    {
	outline[0].x1 = xl;
	outline[0].y1 = yt;
	outline[0].x2 = xr;
	outline[0].y2 = yt;

	outline[1].x1 = xr;
	outline[1].y1 = yt;
	outline[1].x2 = xr;
	outline[1].y2 = yb;

	outline[2].x1 = xr;
	outline[2].y1 = yb;
	outline[2].x2 = xl;
	outline[2].y2 = yb;

	outline[3].x1 = xl;
	outline[3].y1 = yb;
	outline[3].x2 = xl;
	outline[3].y2 = yt;

	XDrawSegments(dpy, Root, DrawGC, outline, 4); 

	w += dw;
	h += dh;
	xl += dx;
	yt += dy;
	xr = xl + w;
	yb = yt + h;
    }

    xl = fx + (xx - rfx);
    yt = fy + (yy - rfy);
    xr = fx + fw + (xx - rfx);
    yb = fy + fh + (yy - rfy);
    w = fw;
    h = fh;

    for (i = 0; i < ZoomCount; i++)
    {
	outline[0].x1 = xl;
	outline[0].y1 = yt;
	outline[0].x2 = xr;
	outline[0].y2 = yt;

	outline[1].x1 = xr;
	outline[1].y1 = yt;
	outline[1].x2 = xr;
	outline[1].y2 = yb;

	outline[2].x1 = xr;
	outline[2].y1 = yb;
	outline[2].x2 = xl;
	outline[2].y2 = yb;

	outline[3].x1 = xl;
	outline[3].y1 = yb;
	outline[3].x2 = xl;
	outline[3].y2 = yt;

	XDrawSegments(dpy, Root, DrawGC, outline, 4);

	w += dw;
	h += dh;
	xl += dx;
	yt += dy;
	xr = xl + w;
	yb = yt + h;
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	MakeCenteredPixmap - make a pixmap centered in a space
 *
 *  Returned Value:
 *	pid	- the pixmap id
 *
 *  Inputs:
 *	w	- the window to associate the pixmap with
 *	gc	- the graphics context to use
 *	width	- the width of the pixmap to create
 *	height  - the height of the pixmap to create
 *	data	- pointer to the pixmap data
 *	pwidth	- the width of the pixmap
 *	pheight	- the height of the pixmap
 *
 ***********************************************************************
 */

Pixmap
MakeCenteredPixmap(w, gc, width, height, data, pwidth, pheight)
    Drawable w;
    GC gc;
    int width, height;
    short *data;
    int pwidth, pheight;
{
    XImage ximage;
    Pixmap pid;
    int dx, dy;

    pid = XCreatePixmap(dpy, w, width, height, d_depth);

    ximage.height = pheight;
    ximage.width = pwidth;
    ximage.xoffset = 0;
    ximage.format = XYBitmap;
    ximage.data = (char *) data;
    ximage.byte_order = LSBFirst;
    ximage.bitmap_unit = 16;
    ximage.bitmap_bit_order = LSBFirst;
    ximage.bitmap_pad = 16;
    ximage.bytes_per_line = (pwidth + 15) / 16 * 2;
    ximage.depth = 1;

    dx = (width - pwidth) / 2;
    dy = (height - pheight) / 2;

    XPutImage(dpy, pid, gc, &ximage, 0, 0, dx, dy, pwidth, pheight);
    return (pid);
}

/***********************************************************************
 *
 *  Procedure:
 *	MakePixmap - make a pixmap
 *
 *  Returned Value:
 *	pid	- the pixmap id
 *
 *  Inputs:
 *	w	- the window to associate the pixmap with
 *	gc	- the graphics context to use
 *	data	- pointer to the pixmap data
 *	width	- the width of the pixmap
 *	height	- the height of the pixmap
 *
 ***********************************************************************
 */

Pixmap
MakePixmap(w, gc, data, width, height)
    Drawable w;
    GC gc;
    short *data;
    int width, height;
{
    return MakeCenteredPixmap(w, gc, width, height, data, width, height);
}

/***********************************************************************
 *
 *  Procedure:
 *	ExpandFilename - expand the tilde character to HOME
 *		if it is the first character of the filename
 *
 *  Returned Value:
 *	a pointer to the new name
 *
 *  Inputs:
 *	name	- the filename to expand
 *
 ***********************************************************************
 */

char *
ExpandFilename(name)
char *name;
{
    char *newname;

    if (name[0] != '~')
	return (name);

    newname = (char *)malloc(HomeLen + strlen(name) + 2);
    sprintf(newname, "%s/%s", Home, &name[1]);

    return (newname);
}

/***********************************************************************
 *
 *  Procedure:
 *	GetUnknownIcon - read in the bitmap file for the unknown icon
 *
 *  Inputs:
 *	name - the filename to read
 *
 ***********************************************************************
 */

void
GetUnknownIcon(name)
char *name;
{
    UnknownPm = GetBitmap(name);
    if (UnknownPm != NULL)
    {
	XGetGeometry(dpy, UnknownPm, &JunkRoot, &JunkX, &JunkY,
	    &UnknownWidth, &UnknownHeight, &JunkBW, &JunkDepth);
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	GetBitmap - read in a bitmap file
 *
 *  Returned Value:
 *	the pixmap associated with the bitmap
 *
 *  Inputs:
 *	name	- the filename to read
 *
 ***********************************************************************
 */

Pixmap
GetBitmap(name)
char *name;
{
    char *bigname;
    int status, junk_hotx, junk_hoty;
    Pixmap pm;

    if  (name == NULL)
	return (NULL);

    name = ExpandFilename(name);
    bigname = name;

    status = XReadBitmapFile(dpy, Root, bigname, &JunkWidth,
	&JunkHeight, &pm, &junk_hotx, &junk_hoty);

    if (status != BitmapSuccess && IconDirectory && name[0] != '/')
    {
	bigname = (char *)malloc(strlen(name) + strlen(IconDirectory) + 2);
	sprintf(bigname, "%s/%s", IconDirectory, name);
	status = XReadBitmapFile(dpy, Root, bigname, &JunkWidth,
	    &JunkHeight, &pm, &junk_hotx, &junk_hoty);
    }

    if (status != BitmapSuccess && name[0] != '/')
    {
	bigname = (char *)malloc(strlen(name) + strlen(BITMAPS) + 2);
	sprintf(bigname, "%s/%s", BITMAPS, name);
	status = XReadBitmapFile(dpy, Root, bigname, &JunkWidth,
	    &JunkHeight, &pm, &junk_hotx, &junk_hoty);
    }

    switch(status)
    {
	case BitmapSuccess:
	    break;

	case BitmapFileInvalid:
	    fprintf(stderr, ".swmrc: invalid bitmap file \"%s\"\n", bigname);
	    break;

	case BitmapNoMemory:
	    fprintf(stderr, ".swmrc: out of memory \"%s\"\n", bigname);
	    break;

	case BitmapOpenFailed:
	    fprintf(stderr, ".swmrc: failed to open bitmap file \"%s\"\n",
		bigname);
	    break;

	default:
	    fprintf(stderr,".swmrc: bitmap error = 0x%x on file \"%s\"\n",
		status, bigname);
	    break;
    }

    if (status != BitmapSuccess)
	return (NULL);

    return (pm);
}

int
GetColor(kind, what, name)
int kind;
int *what;
char *name;
{
    XColor color, junkcolor;

#ifndef TOM
    if (!FirstTime)
	return;
#endif

    if (Monochrome != kind)
	return;

    if (!XParseColor(dpy, CMap, name, &color))
    {
	fprintf(stderr, "swm: invalid color \"%s\"\n", name);
	return;
    }

    if (!XAllocColor(dpy, CMap, &color))
    {
	fprintf(stderr, "swm: invalid color \"%s\"\n", name);
	return;
    }

    *what = color.pixel;
}

GetFont(font)
MyFont *font;
{
    char *deffontname = "fixed";
#ifndef TOM
    if (!FirstTime)
	return;
#endif

    if (font->font != NULL)
	XFreeFont(dpy, font->font);

    if ((font->font = XLoadQueryFont(dpy, font->name)) == NULL)
    {
	if (DefaultFont.name) {
	    deffontname = DefaultFont.name;
	}
	if ((font->font = XLoadQueryFont(dpy, deffontname)) == NULL)
	{
	    fprintf(stderr, "swm: couldn't open font \"%s\" or \"%s\"\n",
		    font->name, deffontname);
	    exit(1);
	}

    }
    font->height = font->font->ascent + font->font->descent;
    font->y = font->font->ascent + 1;
}

/*******************************
*
*  DrawWindowName
*       Drawing Window name
*
*       twin: SwmWindow
*                                89.6.9 FXIS
*
********************************/

void DrawWindowName(twin)
SwmWindow *twin;
{
int titlex;
int title_width;

        title_width = XTextWidth(IconFont.font, twin->window_name,
			 strlen(twin->window_name));
        titlex =(twin->attr.width - title_width) /2; 
        if (titlex < iconify_width)
           titlex = iconify_width + 2;

        XDrawImageString(dpy, twin->title_w,
                twin->title_gc,
                titlex, TitleBarFont.y,
                twin->window_name, strlen(twin->window_name));
}

/*******************************
*
*  SetHilite
*       show Hilite Status
*
*       twin: SwmWindow
*                                89.7.20 FXIS
*
********************************/

void SetHilite(twin)
SwmWindow *twin;
{
	XUnmapWindow(dpy, twin->title_w);
        XSetWindowBackground(dpy, twin->title_w, TitleC.fore);
        XSetForeground(dpy, twin->title_gc, TitleC.back);
        XSetBackground(dpy, twin->title_gc, TitleC.fore);
        DrawWindowName(twin);
        XMapWindow(dpy, twin->title_w);
}

/*******************************
*
*  SetNormal 
*       show Normal Status
*
*       twin: SwmWindow
*       color: backGround Color
*                                89.7.20 FXIS
*
********************************/

void SetNormal(twin)
SwmWindow *twin;
{
        XUnmapWindow(dpy, twin->title_w);
        XSetWindowBackground(dpy, twin->title_w, TitleC.back);
        XSetWindowBorderPixmap(dpy, twin->title_w, GrayTile);
        XSetForeground(dpy, twin->title_gc, TitleC.fore);
        XSetBackground(dpy, twin->title_gc, TitleC.back);
        DrawWindowName(twin);
        XMapWindow(dpy, twin->title_w);
}

/*******************************
*
*  MoveIcon
*       the Icon follows the MousPointer 
*
*       twin: SwmWindow
*       AddingX: Icon x coodinate
*       AddingY: Icon y coodinate
*
*                                89.6.15 FXIS
*
********************************/

void MoveIcon(X, Y)
 int *X;
 int *Y;

{XEvent event;
 int AddingX;
 int AddingY;
 
     while(TRUE)
	{
                XQueryPointer(dpy, Root, &JunkRoot, &JunkChild, &JunkX, &JunkY,
                              &AddingX, &AddingY, &JunkMask);

		MoveOutline(Root, AddingX-IconWidth/2, AddingY-IconHeight/2, IconWidth, IconHeight);

                if(XCheckTypedEvent(dpy, ButtonPress, &event))
                        continue;
                if(XCheckTypedEvent(dpy, ButtonRelease, &event))
		{
			*X = event.xbutton.x_root-IconWidth/2;
                        *Y = event.xbutton.y_root-IconHeight/2;
			MoveOutline(Root, X, Y, IconWidth, IconHeight);
			break;
		}
	}
}

/*******************************
*
*    GetMousePointer
*      Get Mouse Pointer Coodinate
*
*       tmp_win : temp Window
*       x : mouse point (x) 
*       y : mouse Point (y) 
*                                89.8.14 FXIS
*
********************************/
GetMousePointer(tmp_win, x, y)

SwmWindow *tmp_win;
int *x, *y;

{
    int rx, ry, rw, rh, rb, rd;
    int rx2, ry2;
    int junkFx, junkFy, addFx, addFy;
    int junkWx, junkWy, addWx, addWy;
    int junkCx, junkCy, addCx, addCy;
    Window junkFrame, junkWindow, junkChild;

    if (tmp_win->parent == 0)
        {
        XQueryPointer(dpy,Root,&JunkRoot,&junkFrame,&JunkX,&JunkY,
                &addFx,&addFy,&JunkMask);   /* RootDesktop Frame */
        XQueryPointer(dpy,junkFrame,&JunkRoot,&junkWindow,&junkFx,&junkFy,
                &addFx,&addFy,&JunkMask);   /* RootDesktop Window */
        XGetGeometry(dpy, junkWindow, &JunkRoot, &rx, &ry, &rw, &rh, &rb, &rd);
        *x = addFx - rx-BorderWidth;
        *y = addFy - ry-BorderWidth;
        }
    if (tmp_win->parent == 1)
        {
        XQueryPointer(dpy,Root,&JunkRoot,&junkFrame,&JunkX,&JunkY,
                &addFx,&addFy,&JunkMask);   /* RootDesktop Frame */
        XQueryPointer(dpy,junkFrame,&JunkRoot,&junkWindow,&junkFx,&junkFy,
                &addFx,&addFy,&JunkMask);   /* RootDesktop Window */
        XQueryPointer(dpy,junkWindow,&JunkRoot,&junkChild,&JunkX,&JunkY,
                &addCx,&addCy,&JunkMask);   /* Desktop Frame */
        XGetGeometry(dpy, junkWindow, &JunkRoot, &rx, &ry, &rw, &rh, &rb, &rd);
        XGetGeometry(dpy, junkChild, &JunkRoot, &rx2, &ry2, &rw, &rh, &rb, &rd);
        *x = addCx - (rx + rx2)-BorderWidth*3;
        *y = addCy - (ry + ry2)-BorderWidth*3;
        }
}

/*******************************
*
*    GetWindowOffset
*      Get Window Offset 
*
*       tmp_win : temp Window
*       x : mouse point (x)
*       y : mouse Point (y)
*                                89.8.17 FXIS
*
********************************/
GetWindowOffset(tmp_win, x, y)

SwmWindow *tmp_win;
int *x, *y;
{
	int rx, ry, xx, yy;

	XQueryPointer(dpy, Root, &JunkRoot, &JunkChild, &JunkX, &JunkY,
	   &xx, &yy, &JunkMask);
	GetMousePointer(tmp_win, &rx, &ry);

        switch (tmp_win->windowFlag){
	   case 0:
		*x = BorderWidth;
		*y = BorderWidth;
		break;
	   case 1:
		*x = xx - rx + BorderWidth*1;
		*y = yy - ry + BorderWidth*1;
		break;
	   default:
                if (tmp_win->parent == 0)
		   { 
                   *x = xx - rx + BorderWidth*1;
                   *y = yy - ry + BorderWidth*1;
                   }
		else
		   {
		   *x = xx - rx + BorderWidth*2;
                   *y = yy - ry + BorderWidth*2;
		   }
           }
}

/*******************************
*
*    GetDesktopId 
*      Get Desktop window  Id
*
*                                89.8.18 FXIS
*
********************************/
SwmWindow *
GetDesktopId()

{
  Window junkFrame;	/* Desktop frame */
  Window root, *children;
  Window window, junkWindow;
  SwmWindow *twin;
  int nchildren;
  int AddX, AddY;

	XQueryPointer(dpy,RootDesktop->w,&JunkRoot,&junkFrame,&JunkX,&JunkY,
           &AddX,&AddY,&JunkMask); 

        if (junkFrame == NULL)
           {
           XQueryTree(dpy, RootDesktop->w, &root, &junkWindow, &children, &nchildren);
           window = junkWindow;
           }
        else 
           {
           window = junkFrame;
           }

        for ( twin = SwmRoot.next; twin != NULL; twin = twin -> next)
	     {
             if (twin->frame==window)
               {
                if ((junkFrame != NULL) && (twin->windowFlag == 2))
                   {
                   XQueryTree(dpy, RootDesktop->w, &root, &junkWindow, &children, &nchildren);
                   window = junkWindow;
                   for ( twin = SwmRoot.next; twin != NULL; twin = twin -> next)
                       {
                       if (twin->frame == window)
                          {
			  return(twin);
                          break;
			  }
                       }
                   }
                 else 
		   {
                    return(twin); 
                    break;
                   }
                }
	     }

         for ( twin = SwmRoot.next; twin != NULL; twin = twin -> next)
             {
             if ((twin->icon_w == window) || (twin->dummyicon_w == window))
               {
                 XQueryTree(dpy, RootDesktop->w, &root, &junkWindow, &children, &nchildren);
                 window = junkWindow;
                 for ( twin = SwmRoot.next; twin != NULL; twin = twin -> next)
                    {
                    if (twin->frame == window)
                       {
                       return(twin);
                       break;
                       }
                    }
                 }
                }


	return(NULL);
}


/*******************************
*
*    SetIconTable 
*      Set Icon Table  
*
*       x : x coodinate 
*       y : y coodinate
*                                89.6.15 FXIS
*
********************************/

int SetIconTable(tmp_win, x, y)

SwmWindow *tmp_win;
int *x;
int *y;

{
 Window tmpw;
 int prex,prey;
 unsigned int iw,ih,bd,dp;
 int wx,wy;
 int pwx,pwy;
 int maxtablex, maxtabley;
 int tablex, tabley;
 int rx, ry, xx, yy;
 int did;
 SwmWindow *junkDesktop;

	junkDesktop = GetDesktopId(&did);

        GetWindowOffset(tmp_win, &rx, &ry);
        *x = *x - rx;
        *y = *y - ry;
        if (*x < 0)
           *x = 0;
        if (*x > MyDisplayWidth)
           *x = 0;
        if (*y < 0)
           *y = 0;
        if (*y > MyDisplayHeight)
           *y = 0;

        maxtablex = MyDisplayWidth / IconTableWidth;
        maxtabley = MyDisplayHeight / IconTableWidth;

        wx = ((*x+(IconWidth/2))/IconTableWidth);
        wy = ((*y+(IconHeight/2))/IconTableWidth);
        if ((wx > maxtablex) || (wy > maxtabley))
           return(1); 

        if (tmp_win->iconified){
            XGetGeometry(dpy,tmp_win->icon_w,&tmpw,&prex,&prey,&iw,&ih,&bd,&dp);
            if (prex < 0)
               prex = 0;
            if (prey < 0)
               prey = 0;
            
            pwx = (prex+(IconWidth/2))/IconTableWidth;
            pwy = (prey+(IconHeight/2))/IconTableWidth;
            junkDesktop->iconTable [pwx][pwy]=0;
            }
	if(junkDesktop->iconTable [wx][wy] != 1)
            {  
                *x=wx*IconTableWidth+2;
                *y=wy*IconTableWidth+2;
                junkDesktop->iconTable [wx][wy] = 1;
            }
        else
            {
              if (tmp_win->iconified){
                wx = (prex+(IconWidth/2))/IconTableWidth;
                wy = (prey+(IconHeight/2))/IconTableWidth;
                junkDesktop->iconTable [wx][wy]=1;
                *x = prex;
                *y = prey;
                }
              else{
		 for (tablex = 0; tablex < maxtablex; tablex++)
		   for (tabley = 0; tabley < maxtabley; tabley++)
			if(junkDesktop->iconTable[tablex][tabley] == 0)
			  {
			    *x = tablex*IconTableWidth+2;
			    *y = tabley*IconTableWidth+2;
                            junkDesktop->iconTable[tablex][tabley] = 1;
                            return(0);
			  }
                  printf("Warning:No Space left in IconTable--swm\n");
                  return(1);
                 }
	     }
         return(0);
}


