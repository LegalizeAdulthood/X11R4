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

#include <X11/Xlib.h> 
#include <X11/Intrinsic.h> 
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Label.h>
#include <X11/cursorfont.h>
#include <Xw/Xw.h>
#include <Xw/Toggle.h>

#include "i.h"
#include "xstuff.h"

#define	HISTO_HEIGHT	200
#define	HISTO_WIDTH	256
#define MAXNUMCONTLEVELS 50
#define	NUM_REDRAW_REGIONS	4		/* sqrt(# redraw regions) */

/*extern	void Contour_Init();*/
extern	Widget	toplevelform;
extern  unsigned long   black,white;
extern	void	CBCtrlContourBox();

static	unsigned char   contourLevels[MAXNUMCONTLEVELS] 
				= {5,64,128,192,224,255,0}; 

/*static	unsigned char   contourLevels[MAXNUMCONTLEVELS] 
				= {33,0,4,8,12,16,20,24,28,32,36,40,44,48,
				52,56,60,64,68,72,76,80,
				84,88,92,96,100,104,108,112,116,120,124,128,0};
*/
void CBDoneHistogram();
void CBHistogram();

CBDoneGraphBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XtDestroyWidget((Widget) client_data);
}


CBDoneContourBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct	IMAGE *i;

	i = (struct IMAGE *) client_data;
	i->gcws.amIShowing = FALSE;
	XtDestroyWidget(i->gcws.toplevel);
} 


void CBContourExpose(w,client_data,call_data)
/* Draw the contour in parts of the window that is exposed */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct	IMAGE *i;
Region	region;
int	x,y;
int	xrdim,yrdim;	/* dimension of those rectanagles */
int	overlap;
int 	pdim,prdim;

	i = (struct IMAGE *) client_data;
	region  = (Region) call_data;
	
	if (i->gcws.data1) { /* plot contour */
	    xrdim=i->xdim/NUM_REDRAW_REGIONS;
	    yrdim=i->ydim/NUM_REDRAW_REGIONS;

	    for (y=0; y < i->ydim ; y+=yrdim) 
		for (x=0; x < i->xdim; x+=xrdim) {
			overlap = XRectInRegion(region,x,y,xrdim,yrdim);
			if ((overlap==RectangleIn)||(overlap==RectanglePart)){
				Updt_Contour(i->gcws.box3,i->ri,
					i->contourLevels,i->xdim,i->ydim,
					x,y,x+xrdim,y+yrdim,
					black,white);
				}
			}
		}
	else { /* plot polar */
	    pdim = i->ydim << 1;
	    prdim = pdim / NUM_REDRAW_REGIONS;
	    for (y=0; y < pdim ; y+=prdim) 
		for (x=0; x < pdim; x+=prdim) {
			overlap = XRectInRegion(region,x,y,prdim,prdim);
			if ((overlap==RectangleIn)||(overlap==RectanglePart))
				Updt_Contour(i->gcws.box3,i->polarimage,
					i->contourLevels,pdim,pdim,
					x,y,x+prdim,y+prdim,
					black,white);
			} /*for x*/
	    } /*else*/

} /* CBContourExpose() */

void CBContour(w,client_data,call_data)
/* Make Box for contour to be drawn in */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int xpos,ypos;
struct IMAGE *i;
int xdim,ydim;

	i = (struct IMAGE *) client_data;

	if (i->gcws.amIShowing) {
		PrintMesg("Contour graph already displayed.\n");
		return;
		}

	if (i->plotSepWindow) {

		if (!(i->plotCartesian)) {
			if (!i->polarimage) {
				PrintMesg("Please calculate polar first\n");
				return;
				}
			xdim = ydim = i->ydim * 2;
			}
		else {
			xdim = i->xdim;
			ydim = i->ydim;
			}
		PlaceWindow(&xpos,&ypos);
		i = (struct IMAGE *) client_data;
		i->gcws.toplevel = MakeATopLevel(
				"X Image [Image.Graph.Contour]",
				toplevelform,xpos,ypos,xdim+10,ydim+45);
		i->gcws.composite = Make_Board(i->name,i->gcws.toplevel,0,0,
				xdim+50,ydim+50);
		i->gcws.box1 =Make_ButtonBox(i->gcws.composite,5,5,xdim-50,25);
		(void) Make_Label(i->label,i->gcws.box1,2,2,30);
		(void) Make_Label(i->name,i->gcws.box1,33,2,xdim-85);

		i->gcws.box3 = MakeWorkSpace("contour",i->gcws.composite,5,35,
				xdim,ydim,
				CBContourExpose,i,
				NULL,NULL);

		(void) MakeButton("Done",i->gcws.composite,xdim-40,6,
				CBDoneContourBox,i,0,0);

		i->gcws.data1 = i->plotCartesian;  

		}
	i->gcws.amIShowing = TRUE;

} /* CBContour() */

void CBShadeExpose(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct	IMAGE *i;
Region	region;
int	x,y;
int	xrdim,yrdim;	/* dimension of those rectanagles */
int	overlap;
int 	pdim,prdim;

	i = (struct IMAGE *) client_data;
	region  = (Region) call_data;
	
	if (i->gsws.data1) { /* plot shade */
	    xrdim=i->xdim/NUM_REDRAW_REGIONS;
	    yrdim=i->ydim/NUM_REDRAW_REGIONS;

	    for (y=0; y < i->ydim ; y+=yrdim) 
		for (x=0; x < i->xdim; x+=xrdim) {
			overlap = XRectInRegion(region,x,y,xrdim,yrdim);
			if ((overlap==RectangleIn)||(overlap==RectanglePart)){
				Updt_Shaded(i->gsws.box3,i->ri,
					i->xdim,i->ydim,
					x,y,x+xrdim,y+yrdim);
				}
			}
		}
	else { /* plot polar */
	    pdim = i->ydim << 1;
	    prdim = pdim / NUM_REDRAW_REGIONS;
	    for (y=0; y < pdim ; y+=prdim) 
		for (x=0; x < pdim; x+=prdim) {
			overlap = XRectInRegion(region,x,y,prdim,prdim);
			if ((overlap==RectangleIn)||(overlap==RectanglePart))
				Updt_Shaded(i->gsws.box3,i->polarimage,
					pdim,pdim,
					x,y,x+prdim,y+prdim);
			} /*for x*/
	    } /*else*/

} /* CBShadeExpose() */


CBDoneShadeBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct	IMAGE *i;

	i = (struct IMAGE *) client_data;
	i->gsws.amIShowing = FALSE;
	XtDestroyWidget(i->gsws.toplevel);
} 

void CBShade(w,client_data,call_data)
/* Make Box for contour to be drawn in */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int xpos,ypos;
struct IMAGE *i;
int xdim,ydim;

	i = (struct IMAGE *) client_data;

	if (i->gsws.amIShowing) {
		PrintMesg("Shade graph already displayed.\n");
		return;
		}

	if (i->plotSepWindow) {

		if (!(i->plotCartesian)) {
			if (!i->polarimage) {
				PrintMesg("Please calculate polar first\n");
				return;
				}
			xdim = ydim = i->ydim * 2;
			}
		else {
			xdim = i->xdim;
			ydim = i->ydim;
			}
		PlaceWindow(&xpos,&ypos);
		i = (struct IMAGE *) client_data;
		i->gsws.toplevel = MakeATopLevel(
				"X Image [Image.Graph.Shade]",
				toplevelform,xpos,ypos,xdim+10,ydim+45);
		i->gsws.composite = Make_Board(i->name,i->gsws.toplevel,0,0,
				xdim+50,ydim+50);
		i->gsws.box1 =Make_ButtonBox(i->gsws.composite,5,5,xdim-50,25);
		(void) Make_Label(i->label,i->gsws.box1,2,2,30);
		(void) Make_Label(i->name,i->gsws.box1,33,2,xdim-85);

		i->gsws.box3 = MakeWorkSpace("shade",i->gsws.composite,5,35,
				xdim,ydim,
				CBShadeExpose,i,
				NULL,NULL);

		(void) MakeButton("Done",i->gsws.composite,xdim-40,6,
				CBDoneShadeBox,i,0,0);

		i->gsws.data1 = i->plotCartesian;  

		i->gsws.amIShowing = TRUE;
		}

	 /* plot shade in same window */
/*
	else {
		if (i->plotCartestian) {
			Updt_Shaded(i->iws.box3,i->ri,
				i->xdim,i->ydim,
				0,0,i->xdim,i->ydim);
		else
			Updt_Shaded(i->pws.box3,i->polarimage,
				i->ydim*2,i->ydim*2,
				0,0,i->ydim*2,i->ydim*2);
		}
*/
} /* CBShade */




/**** get rid of me */
void OLDCBShade(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int xpos,ypos;
struct IMAGE *i;
int xdim,ydim;

	i = (struct IMAGE *) client_data;


	if (i->plotSepWindow) {

		if (!(i->plotCartesian))
			xdim = ydim = i->ydim * 2;
		else {
			xdim = i->xdim;
			ydim = i->ydim;
			}
		PlaceWindow(&xpos,&ypos);
		i = (struct IMAGE *) client_data;
		i->gsws.toplevel = MakeATopLevel(
				"X Image [Image.Graph.Shade]"
				,toplevelform,xpos,ypos,xdim+50,ydim+50);
		i->gsws.composite = Make_Board(i->name,i->gsws.toplevel,0,0,
				xdim+50,ydim+50);
		i->gsws.box1 = Make_ButtonBox(i->gsws.composite,5,5,250,25);
		(void) Make_Label(i->label,i->gsws.box1,2,2,30);
		(void) Make_Label(i->name,i->gsws.box1,33,2,215);

		i->gsws.box3 = Make_ButtonBox(i->gsws.composite,5,35,
				xdim,ydim);

		(void) MakeButton("Done",
				i->gsws.composite,260,6,CBDoneGraphBox,
				i->gsws.toplevel,0,0);

		/* Should Actually wait for window to appear */
		XSync(XtDisplay(i->gsws.box3),False);

		if (i->plotCartesian)
			Updt_Shaded(i->gsws.box3,i->ri,i->xdim,i->ydim);
		else {
			if (i->polarimage)
				Updt_Shaded(i->gsws.box3,i->polarimage,
						i->ydim*2,i->ydim*2);
			else {
				PrintMesg("Please calculate polar first\n");
				return;
				}
			}
		}
	else {
		if (i->plotCartesian) {
			if (i->cart_showing)
			 	Updt_Shaded(i->iws.box3,i->ri,i->xdim,i->ydim);
			else {
				PrintMesg("Cartesian image not showing\n");
				return;
				}
			}
		else {
			if (i->polar_showing) {
				Updt_Shaded(i->pws.box3,i->polarimage,
						i->ydim*2,i->ydim*2);
				}
			else {
				PrintMesg("Polar image not showing\n");
				return;
				}
			} /*else*/
		} /*else*/
} /* CBShade() */

void CBIntensity(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int xpos,ypos;
struct IMAGE *i;
int xdim,ydim;

	i = (struct IMAGE *) client_data;


	if (i->plotSepWindow) {

		if (!(i->plotCartesian))
			xdim = ydim = i->ydim * 2;
		else {
			xdim = i->xdim;
			ydim = i->ydim;
			}
		PlaceWindow(&xpos,&ypos);
		i = (struct IMAGE *) client_data;
		i->gsws.toplevel = MakeATopLevel(
				"X Image [Image.Graph.Intensity]",
				toplevelform,xpos,ypos,xdim+50,ydim+50);
		i->gsws.composite = Make_Board(i->name,i->gsws.toplevel,0,0,
				xdim+50,ydim+50);
		i->gsws.box1 = Make_ButtonBox(i->gsws.composite,5,5,250,25);
		(void) Make_Label(i->label,i->gsws.box1,2,2,30);
		(void) Make_Label(i->name,i->gsws.box1,33,2,215);

		i->gsws.box3 = Make_ButtonBox(i->gsws.composite,5,35,
				xdim,ydim);

		(void) MakeButton("Done",i->gsws.composite,260,6,CBDoneGraphBox,
				i->gsws.toplevel,0,0);

		/* Should Actually wait for window to appear */
		XSync(XtDisplay(i->gsws.box3),False);

		if (i->plotCartesian)
			Updt_Intensity(i->gsws.box3,i->ri,i->xdim,i->ydim,
						black,FALSE);
		else {
			if (i->polarimage)
				Updt_Intensity(i->gsws.box3,i->polarimage,
						i->ydim*2,i->ydim*2,
						black,FALSE);
			else {
				PrintMesg("Please calculate polar first\n");
				return;
				}
			}
		}
	else {
		if (i->plotCartesian) {
			if (i->cart_showing)
			 	Updt_Intensity(i->iws.box3,i->ri,i->xdim,
						i->ydim,0,TRUE);
			else {
				PrintMesg("Cartesian Image not Showing\n");
				return;
				}
			}
		else {
			if (i->polar_showing) {
				Updt_Intensity(i->pws.box3,i->polarimage,
						i->ydim*2,i->ydim*2,
						0,TRUE);
				}
			else {
				PrintMesg("Polar Image not Showing\n");
				return;
				}
			} /*else*/
		} /*else*/
}



CBPlotWhichDataSet(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct	IMAGE *i;
static Arg cart[]={{XtNlabel,(int)"Plot Cartesian"},{XtNset,True}};
static Arg polar[]={{XtNlabel,(int)"Plot Polar"},{XtNset,True}};

	i = (struct IMAGE *)client_data;

	i->plotCartesian = ! i->plotCartesian;

	if (i->plotCartesian)
		XtSetValues(w,cart,2);
	else
		XtSetValues(w,polar,2);
		
}

void CBPlotInWhichWindow(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct	IMAGE *i;
static Arg sep[]={{XtNlabel,(int)"Plot in Separate Window"},{XtNset,True}};
static Arg overlay[]={{XtNlabel,(int)"Overlay Graph in Image Window"},
			{XtNset,True}};

	i = (struct IMAGE *)client_data;
	i->plotSepWindow = ! i->plotSepWindow;
	if (i->plotSepWindow)
		XtSetValues(w,sep,2);
	else
		XtSetValues(w,overlay,2);
}


void CBDoneGraphControlBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i;
	i = (struct IMAGE *) client_data;

	if (i->gcws.amIShowing)
		CBDoneContourBox(w,i,0);
	if (i->gsws.amIShowing)
		CBDoneShadeBox(w,i,0);
	if (i->ghws.amIShowing)
		CBDoneHistogram(w,i,0);


	XtDestroyWidget(i->gws.toplevel);
	i->graph_showing = FALSE;
}


void CBGraphControlBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct	IMAGE *i;
int	xpos,ypos;

	i = (struct IMAGE *) client_data;
	
	if (i->graph_showing){
		PrintMesg("Graph control window already showing\n");
		/* should bring graph control window to front */
		return;
		}

	PlaceWindow(&xpos,&ypos);
	i->gws.toplevel = MakeATopLevel("X Image [Image.Graph]",
				toplevelform,xpos,ypos,300,125);
	Set_Pointer(i->gws.toplevel,XC_gumby);
	i->gws.composite= Make_Board("Graph",i->gws.toplevel,0,0,300,125);

	i->gws.box1 = Make_ButtonBox(i->gws.composite,5,5,250,25);
	(void) Make_Label(i->label,i->gws.box1,2,2,30);
	(void) Make_Label(i->name,i->gws.box1,33,2,215);

	i->gws.box2 = Make_ButtonBox(i->gws.composite,5,35,290,50);
	(void) MakeAltToggle("Plot Cartesian",i->gws.box2,5,5,
			CBPlotWhichDataSet,i,CBPlotWhichDataSet,i,
			False,False);
/*	(void) MakeAltToggle("Plot in Separate Window",i->gws.box2,5,30,
			CBPlotInWhichWindow,i,CBPlotInWhichWindow,i,
			False,False);
*/
	
	i->gws.box3 = Make_ButtonBox(i->gws.composite,5,90,290,25);
/*	(void) MakeButton("Contour",i->gws.box3,5,2,CBContour,i,0,0);*/
	(void) MakeButton("Contour",i->gws.box3,5,2,CBCtrlContourBox,i,0,0);
/*	(void) MakeButton("Intensity",i->gws.box3,000,2,CBIntensity,i,0,0);*/
	(void) MakeButton("Shade",i->gws.box3,62,2,CBShade,i,0,0);
/*	(void) MakeButton("3D",i->gws.box3,195,2,NULL,NULL,0,0);*/
/*	(void) MakeButton("XY",i->gws.box3,220,2,NULL,NULL,0,0);*/
	(void) MakeButton("Histogram",i->gws.box3,107,2,CBHistogram,i,0,0);

	(void) MakeButton("Done",i->gws.composite,260,6,
				CBDoneGraphControlBox,i,0,0);

	i->graph_showing = TRUE;
	return;
}

void CBDoneHistogram(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i;
	i = (struct IMAGE *) client_data;

	XtDestroyWidget(i->ghws.toplevel);
	i->ghws.amIShowing = FALSE;
}

void CBHistoRedraw(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display	*dpy;
Window	win;
GC	gc;
static	XGCValues	gcval;
int	height;
struct IMAGE *i;
int	x;
int	*h;

	i = (struct IMAGE *) client_data;
	h = (int *)i->ghws.pdata;
	dpy = XtDisplay(i->ghws.box3);
        XSync(dpy,False);
        win = XtWindow(i->ghws.box3);
        gcval.foreground = black;
        gcval.background = white;
        gc = XtGetGC(i->ghws.box3,GCForeground | GCBackground, &gcval);
        for(x=0; x < 256; x++) {
                XSetForeground(dpy,gc,x);
                if (h[x] != i->ghws.data1) {
                        height = (int)((((float) h[x])
					/((float)i->ghws.data2))
                                        *((float) HISTO_HEIGHT));
/*	printf("data1=%d,data2=%d,h[x]=%d,height = %d\n",i->ghws.data1,
			i->ghws.data2,h[x],height);
*/

                        XDrawLine(dpy,win,gc,x+5,HISTO_HEIGHT,x+5,
                                        HISTO_HEIGHT-height);
                        }
		else 
			XDrawLine(dpy,win,gc,x+5,HISTO_HEIGHT,x+5,0);
                }
}


void CBHistoUpdate(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Window root, child;
int     root_x,root_y;
int     win_x,win_y;
unsigned int    keys_buttons;
struct IMAGE *i;
Arg	arglist[1];
char	buff[80];
float	fnum;
int 	*h;

	i = (struct IMAGE *) client_data;
        if (XQueryPointer(XtDisplay(w),XtWindow(w),&root,&child,
                &root_x,&root_y,&win_x,&win_y,&keys_buttons)) {
                }

	win_x = win_x - 5;
	if ((win_x > 255) || ( win_x < 0))
		return;
	
	if (i->isFloat) {
		fnum = ((((float)win_x)/255.0)* (i->maxvalue - i->minvalue))
			+ i->minvalue;
		sprintf(buff,"%8.3f",fnum);
		}
	else
		 sprintf(buff,"%d",win_x);


	XtSetArg(arglist[0],XtNlabel,buff);
	XtSetValues(i->ghws.box2,arglist,1);

	h = (int *)i->ghws.pdata;
	sprintf(buff,"%8.5f",
		((float)h[win_x])/((float)(i->xdim*i->ydim))*100.0);
	XtSetArg(arglist[0],XtNlabel,buff);
	XtSetValues(i->ghws.box4,arglist,1);
}



void CBHistogram(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int	*h;	/* an array containing frequency */
int 	x;
int	xpos,ypos;
int	theend;
int	highest;
int	highest2;
struct IMAGE *i;
Widget	box;

	i = (struct IMAGE *) client_data;

	PlaceWindow(&xpos,&ypos);

	i->ghws.toplevel = MakeATopLevel("X Image [Image.Graph.Histogram]",
			toplevelform,xpos,ypos,
			HISTO_WIDTH+42,HISTO_HEIGHT+120);
	SetColormap(i->ghws.toplevel,i->iws.cmap);
	Set_Pointer(i->ghws.toplevel,XC_gumby);
	i->ghws.composite = Make_Board(i->name,i->ghws.toplevel,0,0,
			HISTO_WIDTH+42,HISTO_HEIGHT+120);
	i->ghws.box1 = Make_ButtonBox(i->ghws.composite,5,5,250,25);
	(void) Make_Label(i->label,i->ghws.box1,2,2,30);
	(void) Make_Label(i->name,i->ghws.box1,33,2,215);

/*	i->ghws.box3 = Make_ButtonBox(i->ghws.composite,5,35,
			HISTO_WIDTH+10,HISTO_HEIGHT+10);
*/
	i->ghws.box3 = MakeWorkSpace("histo",i->ghws.composite,5,35,
			HISTO_WIDTH+10,HISTO_HEIGHT+10,
			CBHistoRedraw,i,NULL,NULL);
	Set_Pointer(i->ghws.box3,XC_left_ptr);
	box = Make_ButtonBox(i->ghws.composite,5,HISTO_HEIGHT+50,
			HISTO_WIDTH+10,60);
	(void) Make_Label("             Data Value:",box,5,5,HISTO_WIDTH/3*2);
	(void) Make_Label("   Frequency Percentage:",box,5,30,HISTO_WIDTH/3*2);
	i->ghws.box2=Make_Label("0",box,HISTO_WIDTH/3*2+10,5,HISTO_WIDTH/3-15);
	i->ghws.box4=Make_Label("0",box,HISTO_WIDTH/3*2+10,30,HISTO_WIDTH/3-15);
	
	XtAddEventHandler(i->ghws.box3,PointerMotionMask,0,
				CBHistoUpdate,i);

	(void) MakeButton("Done",i->ghws.composite,
			260,6,CBDoneHistogram,i,0,0);

	if (!(h = (int *) XICalloc(256,sizeof(int)))){
		PrintMesg("Can't allocate memory for Histogram\n");
		return;
		}
	i->ghws.pdata = (char *) h;


	/* MakeHistogram */
	/*if (! i->isFloat)*/
		if (i->plotCartesian) {
			theend = i->xdim * i->ydim;
			for(x=0; x < theend; x++)
				/*(h[i->ri[x]])++;*/
				h[i->ri[x]] = h[i->ri[x]]+1;
			}
		else {
			theend = (i->ydim << 1) * (i->ydim << 1);
			for(x=0; x < theend; x++)
				h[i->polarimage[x]] = h[i->polarimage[x]]+1;
				/*(h[i->polarimage[x]])++;*/
			}

	highest = h[0];
	for (x=0; x < 256; x++)
		if (h[x] > highest)
			highest = h[x];

	if  (highest == h[0])
		highest2 = h[1];
	else
		highest2 = h[0];
	for (x=1; x < 256; x++) {
		if (h[x] > highest2)
			if (highest != h[x])
				highest2 = h[x];
		}
		
		
	i->ghws.data1 =highest;
	i->ghws.data2 =highest2;
/*	printf("***** i->ghws.data2 = %d\n",i->ghws.data2);*/
	i->ghws.amIShowing = TRUE;
} /* CBHistogram() */

