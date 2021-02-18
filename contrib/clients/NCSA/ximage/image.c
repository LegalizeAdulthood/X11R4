/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Dave Thompson, National Center for Supercomputing Applications
 *         davet@ncsa.uiuc.edu
 */

#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include "buttons.h"
#include "xstuff.h"
#include "i.h"
#include "getfile.h"

#define PAL_WIDTH	50
#define	SQUARE(x)	(x*x)
#define	ROUND(x)	((int) (x + 0.5))
#define	PI		3.1415927   /* <- defined here for convenience, should
					  the value of PI ever change :-) */


extern	Widget	toplevelform;
extern	unsigned long	black,white;
extern	struct	IMAGE *NewIMAGE();
extern 	void	CBDisplayData();
extern	void	CBGraphControlBox();
extern	XColor	rccells[];
extern	XColor	origccells[];

static	Display	*myDpy;	
static	Visual	*myVis;		
static	unsigned char	*palImage;
XImage	*palXImage;
static  struct  GetFile imageFile;
/**/	struct IMAGE *ci;	/* left in for debugging (current image)*/


void DisplayImage();
void CBDoCart();
void CBDoPolar();
int  ReadInImage();
void CreateImageCtrlBox();
void CBUnInstallImageCMap();
void CBCloseCartImageBox();
void CBClosePolarImageBox();


/**/ void CBTestShaded();

int InitImage(dpy)
Display	*dpy;
{
register int x;
register int y;
register int z;
int planes;	/* number of planes the default screen has */
int screen;
register int dooda;	 /*number of bytes needed per pixel */
register unsigned char *p;

	myDpy  = dpy;
	myVis  = GetVisual(myDpy);
/*
	screen = XDefaultScreen(myDpy);
	planes = DisplayPlanes(myDpy,screen);

	dooda = (planes > 7) ? (planes / 8 + ((planes%8)?1:0)) :1;
	if (dooda >= 3)
		dooda = 4;

	if ( ! (palImage = (unsigned char *) XIMalloc(PAL_WIDTH*256*dooda))) {
		printf("Couldn't allocate enough memory\n");
		return(0);
		}

	p = palImage;
	for(y = 0; y < 256; y++) 
		for(x = 0; x < PAL_WIDTH; x++){
			for (z = 1; z < dooda; z++) {
				*p = 0;
				p++;
				}
			*p = 255-y;
			p++;
			}

	palXImage = XCreateImage(myDpy,myVis,planes,ZPixmap,0,palImage,
				PAL_WIDTH,256,8*dooda,PAL_WIDTH*dooda);

*/
	if (!(palXImage = MakePalette(myDpy,PAL_WIDTH,FALSE)))
		return(0);

	return(1); 

} /* InitImage() */



XImage *ConformImageToNPlanes(raster,xdim,ydim,newRaster)
/* return 0 on memory failure */
/* newRaster is set to NULL if no expansion was necessary */
unsigned char *raster;	/* 8 bit raster image */
int xdim,ydim;		/* dimensions of raster */
unsigned char **newRaster; /* RETURNED: expanded raster image */
{
register int x;
register int y;
register int z;
int	planes;
int	nbp;	/* number of bytes per pixel */
XImage	*returnMe;
unsigned char *p;

	planes = DisplayPlanes(myDpy,DefaultScreen(myDpy));
	
	nbp = (planes > 7) ? (planes / 8 + ((planes%8)?1:0)) :1;

	if (nbp >= 3)
		nbp = 4;

	if (nbp > 1) {

		if ( ! (*newRaster=(unsigned char *)XIMalloc(xdim*ydim*nbp))){
			printf("Couldn't allocate enough memory\n");
			return(0);
			}
		p = *newRaster;
	    	for(y = 0; y < ydim; y++) 
		    for(x = 0; x < xdim; x++) {
			for (z = 1; z < nbp; z++){
				(*p) = 0;
				p++;
				}
			(*p) = raster[y*xdim+x];
			p++;
			}

        	returnMe = XCreateImage(myDpy,myVis,planes,ZPixmap,0,*newRaster,
                                xdim,ydim,8*nbp,xdim*nbp);
		}
	else {
        	returnMe = XCreateImage(myDpy,myVis,8,ZPixmap,0,raster,
                                xdim,ydim,8,xdim);

		*newRaster = (unsigned char *) NULL;
		}


	return(returnMe);

} /* ConformImageToNPlanes() */

static void DidntGetAFile(i)
struct IMAGE *i;
{
	PrintMesg("Get new image cancelled\n");
}

static void GotANewFile(i)
struct IMAGE *i;
{
	i->name = (char *) XIMalloc(strlen(imageFile.directory) + 
				strlen(*(imageFile.file)) + 2);
	sprintf(i->name,"%s/%s",imageFile.directory,*(imageFile.file));
	i->xdim = imageFile.xdim;
	i->ydim = imageFile.ydim;
	i->isHDF = imageFile.isHDF;
	i->isFloat = imageFile.isFloat;

	if ((i->xdim == 0 ) || (i->ydim == 0)) {
		char  errbuff[80];
		sprintf(errbuff,"Error: xdim=%d, ydim=%d\n",i->xdim,i->ydim);
		PrintMesg(errbuff);
		DestroyIMAGE(i);
		return;
		}

	if (! ReadInImage(i)){ 
		/* error reading in image */
		DestroyIMAGE(i);	
		return;
		}

	CreateImageCtrlBox(i);

	return;

} /* GotANewFile() */


CBNewImage(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct	IMAGE	*i;

	i = NewIMAGE();
/**/	ci = i;	/* left in for debugging */
	imageFile.yesfile = GotANewFile;
	imageFile.nofile = DidntGetAFile;
	imageFile.type = FTYPE_IMAGE;
	imageFile.param = (char *) i;
	PrintMesg("Please select a file\n");
	GetAFile(&imageFile);

} /* CBNewImage */


static int ReadInImage(i)
struct IMAGE *i;
/* Read in an image either seq or HDF */
/* i->name,i->isHDF,i->xdim,i->ydim must be defined */
/* if HDF, xdim & ydim don't have to be defined */
/* return NULL if error, length of image if ok */
{
int	stat;	/* return stat from reading image */

	if (i->isHDF) {
		if (i->isFloat) 
			stat = ReadHDFSDS(i,i->xdim,i->ydim,1);
		else
			stat = ReadHDFImage(i);
		}
	else
		stat = ReadSEQImage(i);


	return(stat);

} /* ReadInImage() */


void CBCloseImageCtrlBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i;

	i = (struct IMAGE *) client_data;

	if (i->polar_showing)
		CBClosePolarImageBox(w,i,0);
	if (i->cart_showing)
		CBCloseCartImageBox(w,i,0);
	if (i->data_showing)
		CBDoneData(w,i,0);
	if (i->graph_showing)
		CBDoneGraphControlBox(w,i,0);
	if (i->cws.amIShowing || i->ctrl_showing)  /*<-- fix this ambiguity */
		XtDestroyWidget(i->cws.toplevel);

	/* held on for image and histogram */
	XFreeColormap(myDpy,i->iws.cmap);

	DestroyIMAGE(i);

	return;

} /* CBCloseImageCtrlBox() */


static void CreateImageCtrlBox(i)
struct	IMAGE *i;
{
int	xpos,ypos;

	PlaceWindow(&xpos,&ypos);

	i->cws.toplevel = MakeATopLevel("X Image [Image]",
				toplevelform,xpos,ypos,300,100);
	Set_Pointer(i->cws.toplevel,XC_gumby);

	i->cws.composite = Make_Board(i->name,i->cws.toplevel,0,0,300,100);

	i->cws.box1 = Make_ButtonBox(i->cws.composite,5,5,250,25);
	i->cws.label = Make_Label(i->label,i->cws.box1,2,2,30);
	i->cws.title = Make_Label(i->name,i->cws.box1,33,2,215);

	i->cws.box2 = Make_ButtonBox(i->cws.composite,5,35,140,60);
        b[IMAGE_CART] = MakeButton("Display Cartesian",i->cws.box2,
                                5,6,CBDoCart,i,0,0);
        b[IMAGE_POLAR] = MakeButton("Display Polar",i->cws.box2,
				5,30,CBDoPolar,i,0,0);

	i->cws.box3 = Make_ButtonBox(i->cws.composite,155,35,140,60);
	b[IMAGE_DATA] = MakeButton("Display Data Set",i->cws.box3,5,6,
				CBDisplayData,i,0,0);

	b[IMAGE_GRAPH] = MakeButton("Graph",i->cws.box3,5,30,
				CBGraphControlBox,i,0,0);

        b[IMAGE_DONE] = MakeButton("Done",i->cws.composite,
                                260,5,CBCloseImageCtrlBox,i,0,0);

	/* Create colormap used by Cartesian and Histogram */
	
        if (myVis->class== PseudoColor) {
                i->iws.cmap = XCreateColormap(myDpy,
                        XtWindow(i->cws.toplevel),
                        myVis,AllocAll);
                }
        else {
                if (myVis->class == StaticColor) {
                        i->iws.cmap = XCreateColormap(myDpy,
                                XtWindow(i->cws.toplevel),
                                myVis,AllocNone);
                        }
		else {
/**/	printf("\nNot sure how to create a colormap for this display\n");
/**/		printf("I think I'll die ungracefully\n");
/**/		fflush(stdout);
		}
	}
	if (i->hasPalette) {
		StoreColors(i->palette,FALSE,i->iws.cmap);
		}
	else
		StoreColors(origccells,FALSE,i->iws.cmap);

	i->ctrl_showing = TRUE;

} /* CreateImageCtrlBox() */

static void CBTestShaded(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data; 		/* should contain the IMAGE */
{
struct	IMAGE	*i;
int     xpos,ypos;

        PlaceWindow(&xpos,&ypos);

	i = (struct IMAGE *) client_data;

	i->gsws.toplevel = MakeATopLevel(i->name,toplevelform,xpos,ypos,
                                        i->xdim+50,i->ydim+50);
        i->gsws.composite = Make_Board(i->name,i->gsws.toplevel,0,0,
                                        i->xdim+50,i->ydim+50);

        i->gsws.box3 = Make_ButtonBox(i->gsws.composite,5,35,
                                i->xdim,i->ydim);

	InitShade(i);
	Updt_Shaded(i,DISPLAY_CART);
}

static void CBDoCart(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data; 		/* should contain the IMAGE */
{
struct	IMAGE	*i;

	i = (struct IMAGE *)client_data;

	
	if (i->cart_showing) {
		PrintMesg("Cartesian image already showing\n");
/****/		return;		/* <---- actually should bring to front */
		}

	if (DisplayPlanes(myDpy,XDefaultScreen(myDpy)) < 8 ) {
	  PrintMesg("Display must have more than 8 planes to display image\n");
	  return;
	  }

	i->cimage = ConformImageToNPlanes(i->ri,i->xdim,i->ydim,&(i->dExpRI));
	if (!i->cimage)
		return; /* malloc problems */
	if (i->dExpRI)
		i->isDExpRI = TRUE; /* image was depth expanded */


	DisplayImage(i,&(i->iws),DISPLAY_CART);

	i->iws.data1 = FALSE; /* point not selected (hilighted) */

	i->cart_showing = TRUE;		/* <-- this is the same(fix me)*/
	i->iws.amIShowing = TRUE;

	return;

}/* CBDoCart() */



static void CBDoPolar(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data; 		/* should contain the IMAGE */
{
struct	IMAGE	*i;
int	ydim_x_2;

	i = (struct IMAGE *)client_data;

	if (i->polar_showing) {
		PrintMesg("Polar image already showing\n");
		return;		/* <--- actually should bring to front */
		}

	if (DisplayPlanes(myDpy,XDefaultScreen(myDpy)) < 8 ) {
	  PrintMesg("Display must have more than 8 planes to display image\n");
	  return;
	  }

	ydim_x_2 = i->ydim << 1;
	if (!i->pimage) {
		if(! DrawPolar2(i))
			return; /* error creating polar (probably XIMalloc) */
	
/*		i->pimage = XCreateImage(myDpy,myVis,8,ZPixmap,0,i->polarimage,
					ydim_x_2,ydim_x_2,8,ydim_x_2);
*/
		i->pimage = ConformImageToNPlanes(i->polarimage,
					ydim_x_2,ydim_x_2,&(i->dExpPI));
		if (!i->pimage)
			return; /* malloc problems */
		if (i->dExpPI)
			i->isDExpPI = TRUE; /* image was depth expanded */
		}


	DisplayImage(i,&(i->pws),DISPLAY_POLAR); 

	i->pws.data1 = FALSE; /* point not selected (hilighted) */

	i->polar_showing = TRUE;
	i->pws.amIShowing = TRUE;
	return;

}/* CBDoPolar() */



static void CBCloseCartImageBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data; 		/* should contain the IMAGE */
{
struct IMAGE *i;

	i = (struct IMAGE *) client_data;

/*#ifdef  NON_ICCCM_WINDOW_MANAGER*/
	CBUnInstallImageCMap(i->iws.toplevel,i->iws.cmap,call_data);
/*#endif*/

/*	XFreeColormap(myDpy,i->iws.cmap);*/
	XtDestroyWidget(i->iws.toplevel);

	i->cart_showing = FALSE;
	i->iws.amIShowing = FALSE;

} /* CBCloseCartImageBox() */



static void CBClosePolarImageBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data; 		/* should contain the IMAGE */
{
struct IMAGE *i;

	i = (struct IMAGE *) client_data;

/*#ifdef  NON_ICCCM_WINDOW_MANAGER*/
	CBUnInstallImageCMap(i->pws.toplevel,i->pws.cmap,call_data);
/*#endif*/

	XFreeColormap(myDpy,i->pws.cmap);
	XtDestroyWidget(i->pws.toplevel);

	i->polar_showing = FALSE;
	i->pws.amIShowing = FALSE;

} /* CBClosePolarImageBox() */



/* these three palette setting routines should be in their own module */
void CBInstallImageCMap(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
        XInstallColormap(myDpy,(Colormap) client_data);
}

void CBUnInstallImageCMap(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{        
	XUninstallColormap(myDpy,(Colormap) client_data);
}

void SetColormap(w,cmap)
Widget w;
Colormap cmap;
{

/*#ifdef  NON_ICCCM_WINDOW_MANAGER*/
        XtAddEventHandler(w,EnterWindowMask,FALSE,
                        CBInstallImageCMap,cmap);
        XtAddEventHandler(w,LeaveWindowMask,FALSE,
                        CBUnInstallImageCMap,cmap);
/*#else*/
#ifndef DEF_STAT_VISUAL
        {/* Make compatible with Release 3 window managers */
                /* most anoying */
/*                XSetWindowAttributes attrib;
                attrib.colormap = cmap;
                XChangeWindowAttributes(myDpy,XtWindow(w),
                                        CWColormap,&attrib);
*/
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
                XSetWindowColormap(myDpy,XtWindow(w),cmap);
                XSync(myDpy,False);
        }
#endif
/*#endif*/
}


static void CBUpdateCMap(w,client_data,call_data)
/* Update colormap in client_data to be same as palette module's rccells */
/* ....Sync image palette with palette windows palette */
Widget w;
caddr_t client_data;
caddr_t call_data; 
{
Colormap cmap;
	cmap = (Colormap) client_data;
	StoreColors(rccells,FALSE,cmap);
}

static void CBLockPaletteCart(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data; 		/* should contain the IMAGE */
{
struct	IMAGE *i;
	i = (struct IMAGE *) client_data;

	i->iws.lockcmap = ! i->iws.lockcmap;

	if (i->iws.lockcmap) {
		StoreColors(rccells,FALSE,i->iws.cmap);
		XtAddEventHandler(i->iws.toplevel,EnterWindowMask,0,
			CBUpdateCMap,i->iws.cmap);
		}
	else {
		if (i->hasPalette) 
			StoreColors(i->palette,FALSE,i->iws.cmap);
		else
			StoreColors(origccells,FALSE,i->iws.cmap);

		XtRemoveEventHandler(i->iws.toplevel,
			EnterWindowMask,0,CBUpdateCMap,i->iws.cmap);
		}
			
} /* CBLockPaletteCart() */

static void CBLockPalettePolar(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data; 		/* should contain the IMAGE */
{
struct	IMAGE *i;
	i = (struct IMAGE *) client_data;

	i->pws.lockcmap = ! i->pws.lockcmap;

	if (i->pws.lockcmap) {
		StoreColors(rccells,FALSE,i->pws.cmap);
		XtAddEventHandler(i->pws.toplevel,EnterWindowMask,0,
			CBUpdateCMap,i->pws.cmap);
		}
	else {
		if (i->hasPalette) 
			StoreColors(i->palette,FALSE,i->pws.cmap);
		else
			StoreColors(origccells,FALSE,i->pws.cmap);

		XtRemoveEventHandler(i->pws.toplevel,
			EnterWindowMask,0,CBUpdateCMap,i->pws.cmap);
		}

} /* CBLockPalettePolar() */


MakeInc(w,maxvalue,minvalue,isFloat,yOffset)
Widget	w;
float   maxvalue,minvalue;
char isFloat;
int	yOffset;
{
static	char	buff[10];
static	GC	gc;
static	XGCValues	gcval;
float	d,dinc;	/* real data scale, data scale increment */
int	y,yinc;
char	*format;

/*?*/	XSync(myDpy,False);

	yinc = 25;

	dinc = (isFloat) ? ((maxvalue - minvalue) / 10.0): 25.0 ;
	
/*	gcval.font =    GCFont*/
	gcval.foreground = black;
	gcval.background = white;
	gc = XtGetGC(w,GCForeground | GCBackground ,&gcval);

	XSetForeground(myDpy,gc,black);
	XSetBackground(myDpy,gc,white);
	for (y = yinc, d= (float) minvalue+dinc; y < 231; y+=yinc,d+=dinc) {
		if (isFloat)
			sprintf(buff,"%-5.3f",d);
		else 
			sprintf(buff,"%-3d",(int) d);
		XDrawString(myDpy,XtWindow(w),gc,
			10, 255-((int)y+yOffset),buff,strlen(buff));
		}
		
/*	sprintf(buff,"%-4.f",maxvalue);
	XDrawString(myDpy,XtWindow(w),gc,
			10,(int)0+yOffset,buff,4);
*/
}


void CBRedrawTest2(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i;

	i = (struct IMAGE *) client_data;
	MakeInc(w,i->maxvalue,i->minvalue,i->isFloat,0);

}

UnDrawSelectedArea(image,drawable,gc,x,y,xdim,ydim)
unsigned char	*image;		/* the raster image */
Drawable 	drawable;	/* window to draw on */
GC		gc;		/* gc of window to be undrawn on */
int		x,y;		/* point to undraw selection */
int		xdim,ydim;	/* dimensions of image */
{
int	sx,sy;	/* start x&y */
int	ex,ey;	/* end x&y*/
int	i,j;	/* index */

	sx = (x == 0)? x: x-1;
	sy = (y == 0)? y: y-1;
	ex = (x == xdim)? x:x+1;
	ey = (y == ydim)? y:y+1;

	for (j = sy; j <= ey; j++)
		for (i = sx; i <= ex; i++) {
			XSetForeground(myDpy,gc,image[j*xdim+i]);
/*			printf("i,j = %d,%d color=%d\n",i,j,image[j*xdim+i]);*/
			XSync(myDpy,False);
			XDrawPoint(myDpy,drawable,gc,i,j);
			XSync(myDpy,False);
			}
		
}


void CartHighlightSelectedImage(i,x,y)
struct	IMAGE *i;
int	x,y;
{
GC	gc;
XGCValues gcval;

	if (! i->iws.amIShowing)
		return;

	gcval.foreground = white;
	gcval.line_width = 1;
	gc = XtGetGC(i->iws.box3,GCForeground|GCLineWidth,
			&gcval);
	if (i->iws.data1) { 
			UnDrawSelectedArea(i->ri,XtWindow(i->iws.box3),gc,
					i->iws.data2,i->iws.data3,
					i->xdim,i->ydim);

		if (( x== i->iws.data2) && (y == i->iws.data3)) {
			i->iws.data1 = FALSE;	/* is image selected=FALSE*/
			return;
			}
		}


	if (i->ri[i->xdim*y+x] == white) 	/* yeah, I know I'm lazy...*/
		XSetForeground(myDpy,gc,black); 
	else
		XSetForeground(myDpy,gc,white);
	XDrawRectangle(myDpy,XtWindow(i->iws.box3),
			gc,x-1,y-1,2,2);
	i->iws.data1 = TRUE;
	i->iws.data2 = x;
	i->iws.data3 = y;

	return;

} /* CartHighlightSelectedImage() */

void PolarHighlightSelectedImage(i,x,y)
struct	IMAGE *i;
int	x,y;
{
GC	gc;
XGCValues gcval;

	if (! i->pws.amIShowing)
		return;

	gcval.foreground = white;
	gcval.line_width = 1;
	gc = XtGetGC(i->pws.box3,GCForeground|GCLineWidth,
			&gcval);
	if (i->pws.data1) { 
			UnDrawSelectedArea(i->polarimage,XtWindow(i->pws.box3),
					gc, i->pws.data2,i->pws.data3,
					i->ydim*2,i->ydim*2);

		if (( x== i->pws.data2) && (y == i->pws.data3)) {
			i->pws.data1 = FALSE;	/* is image selected=FALSE*/
			return;
			}
		}


	if (i->polarimage[i->ydim*2*y+x] == white)/*yeah?, so what..I'm lazy.*/
		XSetForeground(myDpy,gc,black); 
	else
		XSetForeground(myDpy,gc,white);
	XDrawRectangle(myDpy,XtWindow(i->pws.box3),
			gc,x-1,y-1,2,2);
	i->pws.data1 = TRUE;
	i->pws.data2 = x;
	i->pws.data3 = y;

	return;

} /* PolarHighlightSelectedImage() */



static void CBCartImageSelected(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i;
XButtonPressedEvent *buttonEvent;
char buff[256];
char buff2[256];
int x,y;

	i = (struct IMAGE *) client_data;
	buttonEvent = (XButtonPressedEvent *) call_data;
	x = buttonEvent->x;
	y = buttonEvent->y;
	if ((x < 0) || (x > i->xdim) || (y < 0) || (y > i->ydim))
		return;

	if (i->data_showing) 
		SelectPointFromImage(i,x,y);
	else {
		if (i->isFloat) {
		    sprintf(buff,"Just selected location %d,%d; value=%s\n",
					x,y,i->format);
		    sprintf(buff2,buff,i->fdataset[y*i->xdim+x]);
		    }
		else {
		    sprintf(buff,"Just selected location %d,%d; value=%s\n",
					x,y,i->format);
		    sprintf(buff2,buff,i->ri[y*i->xdim+x]);
		    }
		PrintMesg(buff2);
		}

	CartHighlightSelectedImage(i,x,y);

} /* CBCartImageSelected */



static void CBPolarImageSelected(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i;
XButtonPressedEvent *buttonEvent;
char buff[256];
char buff2[256];
int x,y;
int	yd;	/* ydim  doubled */
int	dx,dy;	/*data set x&y*/
double	r,theta; /* polar coordinates */


	i = (struct IMAGE *) client_data;
	buttonEvent = (XButtonPressedEvent *) call_data;
	x = buttonEvent->x;
	y = buttonEvent->y;

	yd = i->ydim *2;
	if ((x < 0) || (x > yd) || (y < 0) || (y > yd))
		return;	 /* if out of bounds, then ignore it */


	r = sqrt((double) (SQUARE((i->ydim-x)) + SQUARE((i->ydim-y))));
	theta = atan(((double)(y - i->ydim))/((double)(x - i->ydim)));

	dy = ROUND(r);
	dx = ROUND(((double)i->xdim) * ((double) theta/(2.0*PI)));

	/* the following quadrant ajustments should be made on theta*/
	/* before the dx calculations so theta value will be correct */
	/* when printed out ... but I'm lazy...I'll put off til tomorrow*/
	if ( x < i->ydim) /* Q2 & Q3 */
		dx = -dx + i->xdim/2;
	else {
		if (y <= i->ydim)  /* Q1 */
			dx = -dx;
		else /* Q4 */
			dx = -dx + i->xdim;
		}


/*	printf("dx = %d, dy = %d\n",dx,dy);*/

	if ((dx > i->xdim) || (dx < 0)) {
		printf("Error in translation of polar mapping dx =%d\n",dx);
		return;
		}
	if ((dy > i->ydim) || (dy < 0)){
		PrintMesg("Out of Polar data set range\n");
		return;
		}

	if (i->data_showing)
		SelectPointFromImage(i,dx,dy);
	else {
		float rad;
/*		degrees = (int) ((((float) dx) / ((float) i->xdim)) * 360.0);*/
		rad = ((((float) dx) / ((float) i->xdim)) * 2.0*PI);

		if (i->isFloat) {
			sprintf(buff,
		    "Selected location r=%d, theta=%.4f; value=%s\n",
					dy,rad,i->format);
			sprintf(buff2,buff,i->fdataset[dy*i->xdim+dx]);
			}
		else {
			sprintf(buff,
		    "Selected location r=%d, theta=%.4f; value=%s\n",
					dy,rad,i->format);
			sprintf(buff2,buff,i->ri[dy*i->xdim+dx]);
			}
		PrintMesg(buff2);
		}
	PolarHighlightSelectedImage(i,x,y);
}


static void CBInitSetColors(w,client_data,call_data)
/* set palette for the window when exposed */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IWS *ws;

	ws = (struct IWS *) client_data;
	SetColormap(ws->toplevel,ws->cmap);
        XtRemoveEventHandler(w,ExposureMask,FALSE,
                        CBInitSetColors,ws);
}



void DisplayImage(i,ws,type)
/* Create color display window for cartesian or polar */
struct	IMAGE	*i;
struct	IWS	*ws;
int	type;
{
int	xpos,ypos;
int	s;
int	width,height;

	PlaceWindow(&xpos,&ypos);
	if (type == DISPLAY_POLAR) {
		s = i->ydim << 1 ;
		width = ((s+PAL_WIDTH+65) > 200) ? (s+PAL_WIDTH+65) : 200;
		height = ((s+40)  > 350) ? s+40 : 350;
		}
	else {
		width = ((i->xdim+PAL_WIDTH+65) > 200)?
					(i->xdim+PAL_WIDTH+65): 200;
		height = ((i->ydim+40) > 350)?(i->ydim+40): 350;
		}

	ws->toplevel = MakeATopLevel(i->name,toplevelform,xpos,ypos,
					width,height);
	ws->lockcmap = FALSE;

	if (type == DISPLAY_POLAR) {
		if (myVis->class== PseudoColor) {
		       	ws->cmap = XCreateColormap(myDpy,
				XtWindow(ws->toplevel),
				myVis,AllocAll);
			}
		else {
			if (myVis->class == StaticColor) {
		       		ws->cmap = XCreateColormap(myDpy,
					XtWindow(ws->toplevel),
					myVis,AllocNone);
				}
			else {
/**/	printf("\nNot sure how to create a colormap for this display\n");
/**/		printf("I think I'll die ungracefully\n");
/**/		fflush(stdout);
				}
			}
		if (i->hasPalette) {
			StoreColors(i->palette,FALSE,ws->cmap);
			}
		else
			StoreColors(origccells,FALSE,ws->cmap);

		}

	SetColormap(ws->toplevel,ws->cmap);
		


	Set_Pointer(ws->toplevel,XC_gumby);
	ws->composite = Make_Board(i->name,ws->toplevel,0,0,
					width,height);

        ws->box1 = Make_ButtonBox(ws->composite,5,5,width-50,25);
        ws->label = Make_Label(i->label,ws->box1,2,2,30);
        ws->title = Make_Label(i->name,ws->box1,35,2,width-85);

	ws->box2 = Make_ButtonBox(ws->composite,5,35,
				PAL_WIDTH+50,height-40);
	ws->pal_win = MakeRasterImage("palette",ws->box2,
				2,(height-40)/2 -128,
				palXImage,NULL,NULL);



	if (type == DISPLAY_CART) {
        	b[IMAGE_CDONE] = MakeButton("Done",ws->composite,
                                width-40,5,CBCloseCartImageBox,i,0,0);
		ws->box3 = MakeRasterImage(i->name,ws->composite,
					PAL_WIDTH+60,35,
					i->cimage,CBCartImageSelected,i);
		ws->box4 = MakeWorkSpace("Values",ws->box2,PAL_WIDTH+5,
				(height-40)/2-128,40,256,
				CBRedrawTest2,i,NULL,NULL);
		MakeInc(ws->box4,i->maxvalue,i->minvalue,
				i->isFloat,0);
		
		Make_Toggle("Lock Palette",ws->box2,
				2,(height-40)/2 +140,
				CBLockPaletteCart,i,CBLockPaletteCart,i,
				True,False);
		}
	else {
        	b[IMAGE_PDONE] = MakeButton("Done",ws->composite,
                                width-40,5,CBClosePolarImageBox,i,0,0);
		ws->box3 = MakeRasterImage(i->name,ws->composite,
					PAL_WIDTH+60,35,
					i->pimage,CBPolarImageSelected,i);
		ws->box4 = MakeWorkSpace("Values",ws->box2,PAL_WIDTH+5,
				(height-40)/2-128,40,256,
				CBRedrawTest2,i,NULL,NULL);
		MakeInc(ws->box4,i->maxvalue,i->minvalue,
				i->isFloat,0);
		Make_Toggle("Lock Palette",ws->box2,
				2,(height-40)/2 +140,
				CBLockPalettePolar,i,
				CBLockPalettePolar,i,True,False);
		}


	Set_Pointer(ws->box3,XC_left_ptr);


/*	SetColormap(ws->toplevel,ws->cmap);*/

        XtAddEventHandler(ws->toplevel,ExposureMask,FALSE,
                        CBInitSetColors,ws);
} /* DisplayImage() */


