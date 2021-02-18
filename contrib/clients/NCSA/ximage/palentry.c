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
#include <X11/cursorfont.h>
#include <Xw/Xw.h>

#include "xstuff.h"
#include "palentry.h"

extern Widget   toplevelform;
/*test*/ extern Colormap fcmap;
extern	void CBInstallCMap();
extern	void CBUnInstallCMap();

#define PEBOX_XDIM	310
#define PEBOX_YDIM	230
#define	MAX_COLORS	256

#define	VCDIM		50	/* View color entry box dimension (sqrt) */
#define	VCDIMX		100
#define	VCDIMY		50	
#define	PAL_WIDTH	30

static	unsigned char viewColor[VCDIMX*VCDIMY];

static	unsigned	char	*palImage;
static	XImage	*palXImage;



int InitPalEntry(dpy)
Display *dpy;
{
Visual *vis;
register int x;
register int y;

        if ( ! (palImage = (unsigned char *) XIMalloc(PAL_WIDTH*256))) {
	    printf("Couldn't allocate enough memory for pal entry editor\n");
            return(0);
            }

        for(x = 0; x < 256; x++)
                for(y = 0; y < PAL_WIDTH; y++)
                        palImage[y*256+ x] = x;
	vis = GetVisual(dpy);
        palXImage = XCreateImage(dpy,vis,8,ZPixmap,0,palImage,
                                256,PAL_WIDTH,8,256);
        return(1);
}


EditPalEntry(ccells,numColors)
XColor  ccells[];
int     numColors;
{
static	struct	PALEDIT p;

	if (numColors > MAX_COLORS) {
		printf("Can't do more than 256 color pals...remember?\n");
		return;
		}
	MakePalEntryEditor(&p,ccells,numColors);
}

void CBEditPalEntry(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	EditPalEntry(client_data,256);
}

static void CBDone(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct PALEDIT *p;
	p = (struct PALEDIT *) client_data;

	XtDestroyWidget((struct PALEDIT *) p->topLevel);
}

static void CBValuator1(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
char    buff[80];
struct PALEDIT *p;
Arg	arglist[2];
int	newVal;

        p = (struct PALEDIT *) client_data;
        newVal = (int) call_data;


	p->ccells[p->entry].red = newVal << 8;
	StoreColors(p->ccells,False,fcmap);

	sprintf(buff,"%d",newVal);
        XtSetArg(arglist[0],XtNlabel,buff);
        XtSetValues(p->val1,arglist,1);

}

static void CBValuator2(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
char    buff[80];
struct PALEDIT *p;
Arg	arglist[2];
int	newVal;

        p = (struct PALEDIT *) client_data;
        newVal = (int) call_data;


	p->ccells[p->entry].green = newVal << 8;
	StoreColors(p->ccells,False,fcmap);

	sprintf(buff,"%d",newVal);
        XtSetArg(arglist[0],XtNlabel,buff);
        XtSetValues(p->val2,arglist,1);

}

static void CBValuator3(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
char    buff[80];
struct PALEDIT *p;
Arg	arglist[2];
int	newVal;

        p = (struct PALEDIT *) client_data;
        newVal = (int) call_data;


	p->ccells[p->entry].blue = newVal << 8;
	StoreColors(p->ccells,False,fcmap);

	sprintf(buff,"%d",newVal);
        XtSetArg(arglist[0],XtNlabel,buff);
        XtSetValues(p->val3,arglist,1);

}


CBColorVCE(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct PALEDIT *p;
Display *dpy;
	
	dpy = XtDisplay(w);
	p = (struct PALEDIT *) client_data;

	XSetWindowBackground(dpy,XtWindow(p->vce),p->entry);
	XClearArea(dpy,XtWindow(p->vce),0,0,VCDIMX,VCDIMY,False);
}

PalEntryChooser(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Window root, child;
int     root_x,root_y;
int     win_x,win_y;
unsigned int    keys_buttons;
struct	PALEDIT *p;
char	buff[10];
int	v; 

	p = (struct PALEDIT *) client_data;
        if (XQueryPointer(XtDisplay(w),XtWindow(w),&root,&child,
                &root_x,&root_y,&win_x,&win_y,&keys_buttons)) {
                }
        if ((win_x > 255) || ( win_x < 0))
                return;
	p->entry = win_x;

	sprintf(p->dEntryString,"%3d",p->entry);

	CBColorVCE(w,p,0);
	gr_DialogSetValue(p->dEntry,p->dEntryString);

	v = p->ccells[p->entry].red >> 8;
	CBValuator1(w,p,v);
	v = p->ccells[p->entry].green >> 8;
	CBValuator2(w,p,v);
	v = p->ccells[p->entry].blue >> 8;
	CBValuator3(w,p,v);
}


MakePalEntryEditor(p,ccells,numColors)
struct	PALEDIT *p;
XColor	ccells[];
int	numColors;
{
int     xpos,ypos;
int	initE;
Display *dpy;
char	buff[20];
int	v1,v2,v3;
Widget	ebox;

	
	dpy = XtDisplay(toplevelform);
	initE = numColors -1;
	p->ccells = ccells;

	PlaceWindow(&xpos,&ypos);
        p->topLevel = MakeATopLevel("X Image [Palette.PalEntryEditor]",
			toplevelform,
                        xpos,ypos,PEBOX_XDIM,PEBOX_YDIM);

        Set_Pointer(p->topLevel,XC_gumby);

/*#ifdef  NON_ICCCM_WINDOW_MANAGER*/
        XInstallColormap(dpy,fcmap);
        /* Make compatible with Release 2 window managers */
        XtAddEventHandler(p->topLevel,EnterWindowMask,FALSE,
                        CBInstallCMap,NULL);
        XtAddEventHandler(p->topLevel,LeaveWindowMask,FALSE,
                        CBUnInstallCMap,NULL);
/*#else*/

        XSync(dpy,False);
#ifndef DEF_STAT_VISUAL
        {/* Make compatible with Release 3 window managers */
                XSetWindowAttributes attrib;
                attrib.colormap = fcmap;

                XChangeWindowAttributes(dpy,XtWindow(p->topLevel),
                                        CWColormap,&attrib);
                /* aaaaaaarrrrrrrgh */
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
		}
#endif
/*#endif*/

	p->composite = Make_Board("palEntryEditor",
			p->topLevel,0,0,PEBOX_XDIM,PEBOX_YDIM);

	p->composite = Make_Window("X Image [Palette Entry Editor]",
			p->composite,0,0,PEBOX_XDIM,PEBOX_YDIM);


	p->box1 = Make_Board("colors",p->composite,
			2,22,PEBOX_XDIM-10,95);

/*	{
	unsigned char *v;
	int	x;
	for ( x= 0, v = viewColor; x < VCDIMX*VCDIMY ; x++, v++)
		*v = initE;
	}
*/

	/* vce == View Color Entry  */
	p->vce = MakeWorkSpace("vce",p->box1,260-VCDIMX,PAL_WIDTH+10,
			VCDIMX,VCDIMY,CBColorVCE,p,NULL,NULL);

	p->palImage=MakeRasterImage("Palette",p->box1,5,5,palXImage,NULL,NULL);
        XtAddEventHandler(p->palImage,ButtonMotionMask,0,
                                PalEntryChooser,p);

        (void) MakeButton("Done",p->box1,265,5,CBDone,p,0,0);
	
	p->entry = initE;
	sprintf(p->dEntryString,"%d",p->entry);
        ebox = Make_ButtonBox(p->box1,5,PAL_WIDTH+10,PEBOX_XDIM-VCDIMX-60,
				95 - PAL_WIDTH -20);
        p->dEntry = Make_Dialog("name",ebox,"Pal Entry:",p->dEntryString,
                                DSTRINGSIZE,2,2,256-70-VCDIMX,56);
        MakeButton("  Set   ",ebox,95,4,NULL,p,0,0);
/*
	ebox = Make_Board("ebox",p->box1,5,PAL_WIDTH+10,150,27);
	(void) MakeButton("Palette Entry",ebox,2,4,NULL,NULL,0,0);
	p->dEntry = Make_Text(ebox,95,2,50,20,p->dEntryString);
	(void) Make_Toggle("rgb",ebox,5,PAL_WIDTH+47,
			NULL,NULL,NULL,NULL,False);
*/
	

	v1 = p->ccells[initE].red >> 8;
	v2 = p->ccells[initE].green >> 8;
	v3 = p->ccells[initE].blue >> 8;
	p->box2 = Make_Board("colors",p->composite,
			2,122,PEBOX_XDIM-10,PEBOX_YDIM-132);

	p->label1 = Make_Label("red",p->box2,5,10,40);
	p->slide1 = MakeValuator("slide1",p->box2,50,10,210,20,
				False,0,MAX_COLORS+9,v1,10,
				CBValuator1,p,
				CBValuator1,p,
				CBValuator1,p);
	Set_Pointer(p->slide1,XC_sb_h_double_arrow);
	sprintf(buff,"%d",v1);
	p->val1	= Make_Label(buff,p->box2,265,10,30);

	p->label2 = Make_Label("green",p->box2,5,40,40);
	p->slide2 = MakeValuator("slide2",p->box2,50,40,210,20,
				False,0,MAX_COLORS+9,v2,10,
				CBValuator2,p,
				CBValuator2,p,
				CBValuator2,p);
	Set_Pointer(p->slide2,XC_sb_h_double_arrow);

	sprintf(buff,"%d",v2);
	p->val2	= Make_Label(buff,p->box2,265,40,30);

	p->label3 = Make_Label("blue",p->box2,5,70,40);
	p->slide3 = MakeValuator("slide3",p->box2,50,70,210,20,
				False,0,MAX_COLORS+9,v3,10,
				CBValuator3,p,
				CBValuator3,p,
				CBValuator3,p);
	Set_Pointer(p->slide3,XC_sb_h_double_arrow);
	sprintf(buff,"%d",v3);
	p->val3	= Make_Label(buff,p->box2,265,70,30);
	

/*#ifdef  NON_ICCCM_WINDOW_MANAGER*/
        /* Make compatible with Release 2 window managers */
/*        XInstallColormap(dpy,fcmap);
        XtAddEventHandler(p->topLevel,EnterWindowMask,FALSE,
                        CBInstallCMap,NULL);
        XtAddEventHandler(p->topLevel,LeaveWindowMask,FALSE,
                        CBUnInstallCMap,NULL);
*/
/*#else*/

        XSync(dpy,False);
#ifndef DEF_STAT_VISUAL
        {/* Make compatible with Release 3 window managers */
                XSetWindowAttributes attrib;
                attrib.colormap = fcmap;

                XChangeWindowAttributes(dpy,XtWindow(p->topLevel),
                                        CWColormap,&attrib);
                /* aaaaaaarrrrrrrgh */
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
                XSetWindowColormap(dpy,XtWindow(p->topLevel),fcmap);
                XSync(dpy,False);
		}
#endif
/*#endif*/

}
