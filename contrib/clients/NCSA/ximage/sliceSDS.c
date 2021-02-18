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
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include <Xw/Xw.h>
#include <Xw/Toggle.h>

#include "buttons.h"
#include "xstuff.h"

#define	ARROW_TURN1	1
#define ARROW_FLIP	2
#define ARROW_TURN2	3

#define ROT_CLOCKW	1
#define ROT_FLIP	2
#define ROT_COUNTCW	3

#define	X_AXIS	1
#define Y_AXIS	2
#define Z_AXIS	3

extern  Widget  toplevelform;
extern	unsigned long black,white;

void CBDrawDiagram();
void CBRedrawArrows();
void CBRedrawArrows1();
void CBRedrawArrows2();
void CBRedrawArrows3();
void CBHighlight();
void CBUnhighlight();
void CBRotate();
void CBNewPlane();
void DrawPlane();
void CBOk();

static	int	diagramOrientation;
static	char	flipped;
static	int	axis =Z_AXIS;
static	Widget	diagram;
static	char	isSDSBoxDrawn = FALSE;

DrawSDSBox(name)
char *name;		/* name of file */
{
int	xpos,ypos;
Widget	composite;
Widget	box1,box2,box3;
Widget	toplevel;
static	char	title[256];
Display	*dpy;
int	screen;
Widget	b1,b2,b3;

	if (isSDSBoxDrawn)
		 return;

	sprintf(title,"X Image [SDS Selector]: %s",name);

/*	PlaceWindow(&xpos,&ypos);*/
	dpy = XtDisplay(toplevelform);
	screen = XDefaultScreen(dpy);

	toplevel = MakeATopLevelOverride(title,toplevelform,
				(DisplayWidth(dpy,screen)-305)/2,
				(DisplayHeight(dpy,screen)-180)/2,
				305,180);

	Set_Pointer(toplevel,XC_gumby);
	
/*	composite = Make_Board(title, toplevel, 0,0,305,180);*/
	composite = Make_Window("X Image: [SDS selector]",toplevel,
				0,0,305,180);

/*	(void) Make_Label("X Image: SDS selector",composite,2,2,295);*/

	diagramOrientation = 1;
	flipped = FALSE;
	diagram = MakeWorkSpace("Diagram",composite,2,25,150,150,
				CBDrawDiagram,NULL,
				NULL,NULL);

	box1 = Make_Board("controls",composite,155,25,145,150);

	box2 = Make_Board("plane",box1,2,2,140,50);
	(void) MakeAltToggle("Z Plane",box2,5,30,
			CBNewPlane,NULL,CBNewPlane,NULL);
	axis =Z_AXIS;
	(void) Make_Dialog("plane #",box2,"Plane #","0    ",4,
				75,2,60,45);

	box3 = Make_Board("Orientation",box1,2,55,140,70);
	(void) Make_Label("Orientation",box3,2,2,135);
	b1 = MakeWorkSpace("turn1",box3,2,25,40,40,
				CBRedrawArrows1,ARROW_TURN1,
				NULL,NULL);
	XtAddEventHandler(b1,EnterWindowMask,0,CBHighlight,NULL);
	XtAddEventHandler(b1,LeaveWindowMask,0,CBUnhighlight,NULL);
	XtAddEventHandler(b1,ButtonReleaseMask,0,CBRotate,ROT_CLOCKW);
	Set_Pointer(b1,XC_hand2);

	b2 = MakeWorkSpace("flip",box3,48,25,40,40,
				CBRedrawArrows2,ARROW_FLIP,
				NULL,NULL);
	XtAddEventHandler(b2,EnterWindowMask,0,CBHighlight,NULL);
	XtAddEventHandler(b2,LeaveWindowMask,0,CBUnhighlight,NULL);
	XtAddEventHandler(b2,ButtonReleaseMask,0,CBRotate,ROT_FLIP);
	Set_Pointer(b2,XC_hand2);

	b3 = MakeWorkSpace("turn2",box3,94,25,40,40,
				CBRedrawArrows3,ARROW_TURN2,
				NULL,NULL);
	XtAddEventHandler(b3,EnterWindowMask,0,CBHighlight,NULL);
	XtAddEventHandler(b3,LeaveWindowMask,0,CBUnhighlight,NULL);
	XtAddEventHandler(b3,ButtonReleaseMask,0,CBRotate,ROT_COUNTCW);
	Set_Pointer(b3,XC_hand2);

	(void) MakeButton("          OK          ",box1,2,130,
			CBOk,toplevel,0,0);

	isSDSBoxDrawn = TRUE;
}

void CBDrawSDSBox(w,bla,bla2)
Widget w;
int	bla;
int	bla2;
{
DrawSDSBox("/file/name/goes/here");
}


void CBDrawDiagram(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int	todraw;

Display	*dpy;
Window	win;
GC	gc;
XGCValues	gcvalues;
int	xydim = 150;
	
	dpy = XtDisplay(w);
	win = XtWindow(w);

	gcvalues.foreground = white;
	gc = XtGetGC(w,GCForeground, &gcvalues);
	
	XSetForeground(dpy,gc,white);
	XDrawLine(dpy,win,gc,20,xydim-20,20,50); /*vert */
	XDrawLine(dpy,win,gc,20,xydim-20,xydim-50,xydim-20);
	XDrawLine(dpy,win,gc,20,xydim-20,50,xydim-60); /*diag*/
	XDrawLine(dpy,win,gc,20,xydim-20,20,50);
	XDrawLine(dpy,win,gc,20,50,xydim-50,50);
	XDrawLine(dpy,win,gc,20,50,50,10);
	XDrawLine(dpy,win,gc,xydim-50,xydim-20,xydim-50,50); /*vert */
	XDrawLine(dpy,win,gc,20,50,xydim-50,50); /* horiz*/
	XDrawLine(dpy,win,gc,xydim-50,50,xydim-20,10); /*diag*/
	XDrawLine(dpy,win,gc,xydim-50,xydim-20,xydim-50,50); /*vert */
	XDrawLine(dpy,win,gc,20,xydim-20,xydim-50,xydim-20);
	XDrawLine(dpy,win,gc,xydim-50,xydim-20,xydim-20,xydim-60);

	XDrawString(dpy,win,gc,55,10,"z",1);
	XDrawString(dpy,win,gc,55,xydim-61,"z",1);
	XDrawString(dpy,win,gc,xydim-15,10,"z",1);
	XDrawString(dpy,win,gc,xydim-15,xydim-60,"z",1);
			    XDrawString(dpy,win,gc,xydim-45,xydim-20,"x",1);
			    XDrawString(dpy,win,gc,20,45,"y",1);
			    XDrawString(dpy,win,gc,20,xydim-10,"x",1);
			    XDrawString(dpy,win,gc,xydim-45,50,"y",1);
                            XDrawString(dpy,win,gc,10,50,"x",1);
                            XDrawString(dpy,win,gc,xydim-50,xydim-10,"y",1);
			    XDrawString(dpy,win,gc,xydim-45,50,"x",1);
			    XDrawString(dpy,win,gc,20,xydim-10,"y",1);

			    XDrawString(dpy,win,gc,xydim-45,xydim-20,"y",1);
			    XDrawString(dpy,win,gc,20,45,"x",1);
			    XDrawString(dpy,win,gc,10,50,"y",1);
			    XDrawString(dpy,win,gc,xydim-50,xydim-10,"x",1);
	XSetForeground(dpy,gc,black);

	todraw = diagramOrientation;

	if (flipped) {
		switch (diagramOrientation) {
			case 1: todraw = 2;
			    XDrawString(dpy,win,gc,20,xydim-10,"y",1);
			    XDrawString(dpy,win,gc,xydim-45,50,"x",1);
				break;
			case 2: todraw = 1;
			    XDrawString(dpy,win,gc,xydim-45,xydim-20,"y",1);
			    XDrawString(dpy,win,gc,20,45,"x",1);
				break;
			case 3: todraw = 4;
			    XDrawString(dpy,win,gc,xydim-45,50,"y",1);
			    XDrawString(dpy,win,gc,20,xydim-10,"x",1);
				break;
			case 4: todraw = 3;
			    XDrawString(dpy,win,gc,10,50,"y",1);
			    XDrawString(dpy,win,gc,xydim-50,xydim-10,"x",1);
				break;
			}
		}
	else {
		switch (diagramOrientation) {
			case 1:
			    XDrawString(dpy,win,gc,xydim-45,xydim-20,"x",1);
			    XDrawString(dpy,win,gc,20,45,"y",1);
				break;
			case 2:
			    XDrawString(dpy,win,gc,20,xydim-10,"x",1);
			    XDrawString(dpy,win,gc,xydim-45,50,"y",1);
				break;
			case 3:
			    XDrawString(dpy,win,gc,10,50,"x",1);
			    XDrawString(dpy,win,gc,xydim-50,xydim-10,"y",1);
				break;
			case 4:
			    XDrawString(dpy,win,gc,xydim-45,50,"x",1);
			    XDrawString(dpy,win,gc,20,xydim-10,"y",1);
				break;
		}
		}
	switch(todraw) {
		case 1: 
			XDrawLine(dpy,win,gc,20,xydim-20,20,50); /*vert */
			XDrawLine(dpy,win,gc,20,xydim-20,xydim-50,xydim-20);
			XDrawLine(dpy,win,gc,20,xydim-20,50,xydim-60); /*diag*/
			XDrawString(dpy,win,gc,55,xydim-61,"z",1);
			break;
		case 2:
			XDrawLine(dpy,win,gc,20,xydim-20,20,50);
			XDrawLine(dpy,win,gc,20,50,xydim-50,50);
			XDrawLine(dpy,win,gc,20,50,50,10); /* diag */
			XDrawString(dpy,win,gc,55,10,"z",1);
			break;
		case 3:
			XDrawLine(dpy,win,gc,xydim-50,xydim-20,xydim-50,50);
			XDrawLine(dpy,win,gc,20,50,xydim-50,50); /* horiz*/
			XDrawLine(dpy,win,gc,xydim-50,50,xydim-20,10); /*diag*/
			XDrawString(dpy,win,gc,xydim-15,10,"z",1);
			break;
		case 4:
			XDrawLine(dpy,win,gc,xydim-50,xydim-20,xydim-50,50); 
			XDrawLine(dpy,win,gc,20,xydim-20,xydim-50,xydim-20);
			XDrawLine(dpy,win,gc,xydim-50,xydim-20,
					xydim-20,xydim-60);
			XDrawString(dpy,win,gc,xydim-15,xydim-60,"z",1);
			break;
		}
		DrawPlane(w,axis,black,white);
}

void CBRedrawArrows(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display	*dpy;
Window	win;
GC	gc;
XGCValues	gcvalues;
	
	dpy = XtDisplay(w);
	win = XtWindow(w);

	gcvalues.foreground = black;
	gcvalues.background = white;
	gc = XtGetGC(w,(GCForeground|GCBackground), &gcvalues);
	
/**/	printf("CBRedrawArrows():I've been called: call_data = %d\n",
/**/				(int)call_data);
	switch ((int) call_data) {
		case ARROW_TURN1:
				XDrawArc(dpy,win,gc,0,0,30,20,
						135/64,225/64);
				break;
		case ARROW_FLIP:
				XDrawLine(dpy,win,gc,20,5,20,15);
				XDrawLine(dpy,win,gc,20,20,20,35);
				break;
		case ARROW_TURN2:
				XDrawArc(dpy,win,gc,0,0,30,20,
						135/64,235/64);
				break;
		}
		
}

void CBRedrawArrows1(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display	*dpy;
Window	win;
GC	gc;
XGCValues	gcvalues;
	
	dpy = XtDisplay(w);
	win = XtWindow(w);

	gcvalues.foreground = black;
	gcvalues.background = white;
	gc = XtGetGC(w,(GCForeground|GCBackground), &gcvalues);
	
	XDrawArc(dpy,win,gc,15,0,40,40,135*64,90*64);
/*	XDrawLine(dpy,win,gc,22,5,22,9);
	XDrawLine(dpy,win,gc,22,5,18,6);
*/
	XDrawLine(dpy,win,gc,21,6,21,10);
	XDrawLine(dpy,win,gc,21,6,17,7);
}


void CBRedrawArrows2(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display	*dpy;
Window	win;
GC	gc;
XGCValues	gcvalues;
	
	dpy = XtDisplay(w);
	win = XtWindow(w);

	gcvalues.foreground = black;
	gcvalues.background = white;
	gc = XtGetGC(w,(GCForeground|GCBackground), &gcvalues);
	
	XDrawLine(dpy,win,gc,20,5,20,18);
	XDrawLine(dpy,win,gc,20,5,17,8);
	XDrawLine(dpy,win,gc,20,5,23,8);
	XDrawLine(dpy,win,gc,20,22,20,35);
	XDrawLine(dpy,win,gc,17,33,20,35);
	XDrawLine(dpy,win,gc,23,33,20,35);
}


void CBRedrawArrows3(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display	*dpy;
Window	win;
GC	gc;
XGCValues	gcvalues;
	
	dpy = XtDisplay(w);
	win = XtWindow(w);

	gcvalues.foreground = black;
	gcvalues.background = white;
	gc = XtGetGC(w,(GCForeground|GCBackground), &gcvalues);
	
	XDrawArc(dpy,win,gc,-15,0,40,40, 315*64,90*64);
	XDrawLine(dpy,win,gc,18,5,18,9);
	XDrawLine(dpy,win,gc,18,5,22,6);
}



void CBHighlight(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display	*dpy;
Window	win;
GC	gc;
XGCValues	gcvalues;
	
	dpy = XtDisplay(w);
	win = XtWindow(w);

	gcvalues.foreground = black;
	gcvalues.line_width= 3;
	gc = XtGetGC(w,(GCForeground|GCLineWidth), &gcvalues);
	
	XDrawLine(dpy,win,gc,0,0,39,0);
	XDrawLine(dpy,win,gc,0,0,0,39);
	XDrawLine(dpy,win,gc,39,0,39,39);
	XDrawLine(dpy,win,gc,0,39,39,39);
}



void CBUnhighlight(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display	*dpy;
Window	win;
GC	gc;
XGCValues	gcvalues;
	
	dpy = XtDisplay(w);
	win = XtWindow(w);

	gcvalues.foreground = white;
	gcvalues.line_width= 3;
	gc = XtGetGC(w,(GCForeground|GCLineWidth), &gcvalues);
	
	XDrawLine(dpy,win,gc,0,0,39,0);
	XDrawLine(dpy,win,gc,0,0,0,39);
	XDrawLine(dpy,win,gc,39,0,39,39);
	XDrawLine(dpy,win,gc,0,39,39,39);
}


static void CBRotate(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	switch ((int) client_data) {
		case ROT_CLOCKW:
				if (! flipped) {
				    diagramOrientation++;
				    diagramOrientation = 
						(diagramOrientation >4)?
						1:diagramOrientation;
					}
				else {
				    diagramOrientation--;
				    diagramOrientation = 
						(diagramOrientation < 1)?
						4:diagramOrientation;
				     }
				break;
		case ROT_FLIP:
				flipped = ! flipped;
				break;
		case ROT_COUNTCW:
				if (! flipped) {
				    diagramOrientation--;
				    diagramOrientation = 
						(diagramOrientation < 1)?
						4:diagramOrientation;
					}
				else {
				    diagramOrientation++;
				    diagramOrientation = 
						(diagramOrientation >4)?
						1:diagramOrientation;
				     }
				break;
		}
	
	CBDrawDiagram(diagram,NULL,NULL);
}


void DrawPlane(w,plane,fcolor,bcolor)
Widget w;
int plane; 
unsigned long fcolor,bcolor;	/* foreground and background color */
{
Display	*dpy;
Window	win;
GC	gc;
XGCValues	gcvalues;
static	unsigned char	dashlist[]={(char)3,(char)4,(char)3,(char)1};
int	xydim = 150;

	dpy = XtDisplay(w);
	win = XtWindow(w);

	gcvalues.foreground = fcolor;
	gcvalues.background= bcolor;
	gcvalues.dash_offset = 0;
	gcvalues.dashes = 4;
	gc = XtGetGC(w,(GCForeground|GCBackground|GCDashOffset|GCDashList), 
			&gcvalues);

	XSetDashes(dpy,gc,0,dashlist,4);
	XSetForeground(dpy,gc,fcolor);
	XSetBackground(dpy,gc,bcolor);

	XSync(dpy,False);

	switch (plane) {
	    case X_AXIS: /* X Axis */
		XDrawLine(dpy,win,gc,60,xydim-20,60,50);
		XDrawLine(dpy,win,gc,60,xydim-20,90,xydim-60);
		XDrawLine(dpy,win,gc,60,50,90,10);
		XDrawLine(dpy,win,gc,90,10,90,xydim-60);
		break;
	    case Y_AXIS: /* Y Axis */
		XDrawLine(dpy,win,gc,20,xydim-60,xydim-50,xydim-60); /*front*/
		XDrawLine(dpy,win,gc,20,xydim-60,50,xydim-101);	/*left*/
		XDrawLine(dpy,win,gc,xydim-50,xydim-60,xydim-20,xydim-101); 
		XDrawLine(dpy,win,gc,50,xydim-101,xydim-20,xydim-101); /*back*/
		break;
	    case Z_AXIS: /* Z Axis */
		XDrawLine(dpy,win,gc,40,xydim-47,40,35);
		XDrawLine(dpy,win,gc,xydim-30,xydim-47,xydim-30,35);
		XDrawLine(dpy,win,gc,40,xydim-47,xydim-30,xydim-47);
		XDrawLine(dpy,win,gc,40,35,xydim-30,35);
	    }
}

void CBNewPlane(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
static Arg x_axis[] = {{XtNlabel,(int)"X Plane"},{XtNset,True}};
static Arg y_axis[] = {{XtNlabel,(int) "Y Plane"},{XtNset,True}};
static Arg z_axis[] = {{XtNlabel,(int) "Z Plane"},{XtNset,True}};

	switch (axis) {
		case X_AXIS:
			DrawPlane(diagram,X_AXIS,white,white);
			DrawPlane(diagram,Y_AXIS,black,white);
			axis = Y_AXIS;
			XtSetValues(w,y_axis,2);
			break;
		case Y_AXIS:
			DrawPlane(diagram,Y_AXIS,white,white);
			DrawPlane(diagram,Z_AXIS,black,white);
			axis = Z_AXIS;
			XtSetValues(w,z_axis,2);
			break;
		case Z_AXIS:
			DrawPlane(diagram,Z_AXIS,white,white);
			DrawPlane(diagram,X_AXIS,black,white);
			axis = X_AXIS;
			XtSetValues(w,x_axis,2);
			break;
		}
}


static void CBOk(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XtDestroyWidget((Widget) client_data);
	isSDSBoxDrawn = FALSE;
}
