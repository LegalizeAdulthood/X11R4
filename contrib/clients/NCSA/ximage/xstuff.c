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
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/Command.h>
#include <X11/Shell.h>
#include <X11/Box.h>
#include <X11/Form.h>
#include <X11/Label.h>
#include <X11/AsciiText.h>
#include <X11/Dialog.h>
#include <X11/cursorfont.h>
#include <X11/Viewport.h>
#include <X11/List.h>

#include "list.h"

#include <Xw/Xw.h>
#include <Xw/PButton.h>
#include <Xw/BBoard.h>
#include <Xw/Toggle.h>
#include <Xw/SRaster.h>
#include <Xw/Valuator.h>
#include <Xw/TextEdit.h>
#include <Xw/WorkSpace.h>

#define	SCREEN_DEPTH	8
#define LABELFONT	"8x13bold"

void Set_Pointer();

extern Widget toplevelform;

Visual *GetVisual(dpy)
Display *dpy;
{
static char haveIComplainedYet = FALSE;
XVisualInfo vinfo;

	if (!XMatchVisualInfo(dpy,XDefaultScreen(dpy),8,PseudoColor,&vinfo)) {
		if (! haveIComplainedYet) {
/*		PrintMesg("Couldn't get a PseudoColor 8 bit visual\n");*/
			haveIComplainedYet = TRUE;
			}
		return(XDefaultVisual(dpy,XDefaultScreen(dpy)));
		}
	return(vinfo.visual);
}

void PlaceWindow(xpos,ypos)
int	*xpos,*ypos;
{
static	char	start = 20;
static	int	x = 50;
static	int	y = 50;
Display	*dpy;
int	screen;

	*xpos = x;
	*ypos = y;
	x+=100;
	y+=100;

	dpy = XtDisplay(toplevelform);
	screen = XDefaultScreen(dpy);
	if (x > DisplayWidth(dpy,screen)-300){
		x = y = 50 + start;
		start = (start > 100)? 20 :start + 20;
		}
	if (y > DisplayHeight(dpy,screen)-300){
		x = y = 50 + start;
		start = (start > 100)? 20 :start + 20;
		}

	return;
}

static void CBSetInputFocus(w,client_data,call_data)
/* called when on an Enter window event*/
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XSetInputFocus(XtDisplay(w),XtWindow(w),RevertToNone,CurrentTime);
}


Widget MakeButton(label,parent,xloc,yloc,cbp,client_data,xdim,ydim)
char	*label;			/* Button Label */
Widget	parent;			/* Parent widget of the button */
int	xloc,yloc;		/* Location in Parent widget */
XtCallbackProc cbp;		/* Procedure to call back when button press */
caddr_t	client_data;		/* data for call back */
int	xdim,ydim;		/* dimensions of button; 0 if default */
{
Arg             arglist[10];
Cardinal        i=0;
Widget		the_button;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNlabel,label);	i++;
	if (xdim) {
		XtSetArg(arglist[i],XtNwidth,xdim);	i++;
		}
	if (ydim) {
		XtSetArg(arglist[i],XtNheight,ydim);	i++;
		}
	
	the_button = XtCreateManagedWidget("XIButton", 
			commandWidgetClass, parent, arglist, i);
	if (cbp)
		 XtAddCallback(the_button,"callback",cbp,client_data);
	Set_Pointer(the_button,XC_hand2);

	return(the_button);

} /* Make_Button() */



Widget Make_Window(label,parent,xloc,yloc,width,height)
char	*label;
Widget	parent;
int	xloc,yloc;
int	width,height;
{
XFontStruct	*labelfont;
Widget		the_window;
Arg             arglist[10];
Cardinal        i=0;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;
	XtSetArg(arglist[i],XtNborderWidth,3);	i++;
	XtSetArg(arglist[i],XtNborder,
            WhitePixel(XtDisplay(parent),DefaultScreen(XtDisplay(parent))));i++;

	the_window = XtCreateManagedWidget("XIWindow",
				XwbulletinWidgetClass, parent, arglist, i);

	labelfont = XLoadQueryFont(XtDisplay(the_window),LABELFONT);
	i=0;
	XtSetArg(arglist[i],XtNx,1);		i++;
	XtSetArg(arglist[i],XtNy,1);		i++;
	XtSetArg(arglist[i],XtNwidth,width-2);	i++;
	XtSetArg(arglist[i],XtNlabel,label);	i++;
	if (labelfont)
		XtSetArg(arglist[i],XtNfont,labelfont); i++;
	(void) XtCreateManagedWidget("XILabel",labelWidgetClass,
				the_window, arglist, i);

	Set_Pointer(the_window,XC_gumby);

	return(the_window);

} /* Make_Window() */


Widget Make_ButtonBox(parent,xloc,yloc,width,height)
Widget	parent;		/* Parent Widget */
int	xloc,yloc;	/* location inside of parent widget */
int	width,height;	/* Size of the box */
{
Widget		the_window;
Arg             arglist[10];
Cardinal        i=0;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;
	
	the_window = XtCreateManagedWidget("XIWindow",
				/*boxWidgetClass, parent, arglist, i);*/
				XwbulletinWidgetClass, parent, arglist, i);
	return(the_window);

} /* Make_ButtonBox() */

Widget MakeBox(name,parent,xloc,yloc,width,height)
char	*name;
Widget	parent;		/* Parent Widget */
int	xloc,yloc;	/* location inside of parent widget */
int	width,height;	/* Size of the box */
{
Widget		the_window;
Arg             arglist[10];
Cardinal        i=0;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;
	
	the_window = XtCreateManagedWidget(name,
				boxWidgetClass, parent, arglist, i);
	return(the_window);

} /* MakeBox() */






Widget MakeWorkSpace(name,parent,xloc,yloc,width,height,exp_cbp,exp_param,
							key_cbp,key_param)
char	*name;
Widget	parent;		/* Parent Widget */
int	xloc,yloc;	/* location inside of parent widget */
int	width,height;	/* Size of the box */
XtCallbackProc exp_cbp;	/* expose call back procedure */
caddr_t exp_param;	/* expose call back procedure parameter */
XtCallbackProc key_cbp;	/* key press call back procedure */
caddr_t key_param;	/* key press call back procedure parameter */
{
Widget		w;
Arg             arglist[10];
Cardinal        i=0;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;
	
	w = XtCreateManagedWidget(name,
				XwworkSpaceWidgetClass, parent, arglist, i);
	if (exp_cbp)
		 XtAddCallback(w,"expose",exp_cbp,exp_param);
	if (key_cbp)
		 XtAddCallback(w,"keyDown",key_cbp,key_param);

	return(w);

} /* MakeWorkSpace() */




Widget Make_Text(parent,xloc,yloc,width,height,string)
Widget	parent;
int	xloc,yloc;
int	width,height;
char	*string;
{
Widget		the_window;
Arg             arglist[10];
Cardinal        i=0;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;
	XtSetArg(arglist[i],XtNstring,string);	i++;

	
/*	the_window = XtCreateManagedWidget("XIWindow",
				asciiStringWidgetClass, parent, arglist, i);
*/
	the_window = XtCreateManagedWidget("XIWindow",
				XwtexteditWidgetClass, parent, arglist, i);
	return(the_window);

} /* Make_Text() */

Widget Make_Toggle(label,parent,xloc,yloc,cbp,data,cbp2,data2,square,set)
char	*label;			/* Toggle Label */
Widget	parent;			/* Parent widget of the toggle*/
int	xloc,yloc;		/* Location in Parent widget */
XtCallbackProc cbp,cbp2;	/* Procedure to call back when toggled*/
caddr_t	data,data2;
int	square;			/* Should it be a square? */
unsigned char set;		/* should toggle be set? */
{
Arg             arglist[10];
Cardinal        i=0;
Widget		the_toggle;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNlabel,label);	i++;
	XtSetArg(arglist[i],XtNsquare,square);	i++;
	XtSetArg(arglist[i],XtNset,set);	i++;
	
		the_toggle = XtCreateManagedWidget("XIToggle", 
				XwtoggleWidgetClass, parent, arglist, i);
		if (cbp)
			 XtAddCallback(the_toggle,"select",cbp,data);
		if (cbp2)
			 XtAddCallback(the_toggle,"release",cbp2,data2);

	Set_Pointer(the_toggle,XC_hand2);

	return(the_toggle);

} /* Make_Toggle() */

Widget MakeAltToggle(label,parent,xloc,yloc,cbp,data,cbp2,data2)
char	*label;			/* Toggle Label */
Widget	parent;			/* Parent widget of the toggle*/
int	xloc,yloc;		/* Location in Parent widget */
XtCallbackProc cbp,cbp2;	/* Procedure to call back when toggled*/
caddr_t	data,data2;
{
Arg             arglist[10];
Cardinal        i=0;
Widget		the_toggle;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNlabel,label);	i++;
	XtSetArg(arglist[i],XtNsquare,False);	i++;
	XtSetArg(arglist[i],XtNset,True);	i++;
	the_toggle = XtCreateManagedWidget("toggle", 
			XwtoggleWidgetClass, parent, arglist, i);
	if (cbp)
		 XtAddCallback(the_toggle,"select",cbp,data);
	if (cbp2)
		 XtAddCallback(the_toggle,"release",cbp2,data2);

	Set_Pointer(the_toggle,XC_hand2);

	return(the_toggle);
}


Widget Make_Label(label,parent,xloc,yloc,width)
char    *label;
Widget  parent;
int     xloc,yloc;
int	width;
{
Widget          the_label;
Arg             arglist[10];
Cardinal        i=0;

        XtSetArg(arglist[i],XtNx,xloc);         i++;
        XtSetArg(arglist[i],XtNy,yloc);         i++;
        XtSetArg(arglist[i],XtNwidth,width-2);  i++;
        XtSetArg(arglist[i],XtNlabel,label);    i++;

        the_label = XtCreateManagedWidget("XILabel",labelWidgetClass,
                                parent, arglist, i);

	return(the_label);

} /* Make_Label() */


Widget MakeForm(widget_name,parent,xloc,yloc,width,height,moreOps,numOps)
char    *widget_name;
Widget  parent;
int     xloc,yloc;
int	width;
int	height;
Arg	moreOps[];
int	numOps;
{
Widget          w;
Arg             arglist[20];
Cardinal        i=0;
int		x;

	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;
	if (moreOps)
	    for (x=0; x < numOps; x++) 
		XtSetArg(arglist[i],moreOps[x].name,moreOps[x].value); i++;

	w = XtCreateManagedWidget(widget_name,formWidgetClass,
                                parent, arglist, i);

	return(w);

} /* Make_Form() */




Widget Make_Dialog(widget_name,parent,label,defstring,maxlength,
			x,y,width,height)
char	*widget_name;
Widget	parent;
char	*label;
char	*defstring;
int	maxlength;
int	x,y;
int	width,height;
{
Widget          w;
Arg             arglist[10];
Cardinal        i=0;

	XtSetArg(arglist[i],XtNx,x);		i++;
	XtSetArg(arglist[i],XtNy,y);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;
	XtSetArg(arglist[i],XtNlabel,label);	i++;
	XtSetArg(arglist[i],XtNvalue,defstring);	i++;
	XtSetArg(arglist[i],XtNmaximumLength,maxlength);i++;
	XtSetArg(arglist[i],XtNsensitive,True);	i++;

	w = XtCreateManagedWidget(widget_name,dialogWidgetClass,
				parent,arglist,i);

	XtAddEventHandler(w,EnterWindowMask,0,CBSetInputFocus,NULL);
	return(w);

}/* Make_Dialog() */

Widget Make_Board(widget_name,parent,x,y,width,height)
Widget parent;
int	x,y;
int	width,height;
{
Widget          w;
Arg             arglist[10];
Cardinal        i=0;

	XtSetArg(arglist[i],XtNx,x);		i++;
	XtSetArg(arglist[i],XtNy,y);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;
	w = XtCreateManagedWidget(widget_name,XwbulletinWidgetClass,
				parent,arglist,i);

	return(w);
}


Widget Make_Text_Inp(widget_name,parent,string,strmaxlen,x,y,width,height)
/* A Null in string deliminates the end of 
 * the prompt and the rest of string space */
char	*widget_name;
Widget	parent;
char	*string;
int	strmaxlen;
int x,y;
int width,height;
{
Widget	w;
Arg	arglist[10];
Cardinal i=0;

	XtSetArg(arglist[i],XtNx,x);			i++;
	XtSetArg(arglist[i],XtNy,y);			i++;
	XtSetArg(arglist[i],XtNheight,height);		i++;
	XtSetArg(arglist[i],XtNwidth,width);		i++;
	XtSetArg(arglist[i],XtNstring,string);		i++;
	XtSetArg(arglist[i],XtNlength,strmaxlen);	i++;
	XtSetArg(arglist[i],XtNeditType,XttextEdit);	i++;

	w = XtCreateManagedWidget(widget_name,asciiStringWidgetClass,
		parent,arglist,i);

	XtAddEventHandler(w,EnterWindowMask,0,CBSetInputFocus,NULL);

	return(w);

} /* Make_Text_Inp() */

void Get_Text_Inp(w,startpos,endpos,string,stringsize)
Widget w;
char *string;
{
int i;

	XwTextReadSubString(w,startpos,endpos,string,stringsize,&i);

	return;

} /* Get_Text_Inp() */


Widget Make_Pal_Window(parent,xloc,yloc,xdim,ydim)
Widget	parent;			/* parent widget */
int	xloc,yloc;		/* location to be placed in parent widget */
int	xdim,ydim;		/* size of widget */
{
static	char	*pbuff;			/* palette image in here */
static	XImage	*pal;			/* palette image struct in here */
Widget	palw;				/* The final product */
int	x,y;
Visual	*vis;		
Arg arglist[5];
Cardinal i;
int	z;
int	xmax;
int	repeat;

/*	xdim = (xdim > 255)?(xdim/255)*255:xdim;
	xmax = (xdim > 255)?255:xdim;
	repeat = xdim / 255;

	pbuff = (char *) XIMalloc(xdim*ydim);

	for(y = 0; y < ydim; y++) {
		for(x = 0; x < xmax; x++)
			for (z = 0 ; z < repeat ; z++)
				pbuff[y*xdim+x*z+z] = x;
		}
*/
	xdim = (xdim > 255)?255:xdim;
	pbuff = (char *) XIMalloc(xdim*ydim);
	for (y = 0; y < ydim ; y++) {
		for(x = 0; x < xdim ; x++)
			pbuff[y*xdim+x]=x;
		}
	
	/* Make palette structure */
	vis = GetVisual(XtDisplay(parent));
	pal = XCreateImage(XtDisplay(parent),vis,SCREEN_DEPTH,ZPixmap,0,
		pbuff,xdim,ydim,8,xdim);

	/* Create a HP Static Raster widget */
	i=0;
        XtSetArg(arglist[i],XtNsRimage,pal);	i++;
	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
        palw = XtCreateManagedWidget("XIWindow",
                        XwsrasterWidgetClass,parent,arglist,i);

	return(palw);

} /* Make_Pal_Window() */


XImage *MakePalette(myDpy,pal_width,isHoriz)
/* Make an XImage of a palette */
Display	*myDpy;
int	pal_width;
char	isHoriz; /* should palette progress 0 to 255 horizontally */
{
Visual *myVis;
register int x;
register int y;
register int z;
int planes;	/* number of planes the default screen has */
int screen;
register int dooda;	 /*number of bytes needed per pixel */
register unsigned char *p;
XImage *palXImage;
unsigned char	*palImage;

	myVis  = GetVisual(myDpy);
	screen = XDefaultScreen(myDpy);
	planes = XDisplayPlanes(myDpy,screen);

	dooda = (planes > 7) ? (planes / 8 + ((planes%8)?1:0)) :1;
	if (dooda >= 3)
		dooda = 4;

	if ( ! (palImage = (unsigned char *) XIMalloc(pal_width*256*dooda))) {
		printf("Couldn't allocate enough memory\n");
		return(0);
		}

	p = palImage;

	if (isHoriz) {
		for(y = 0; y < pal_width; y++)
			for(x = 0; x < 256; x++) {
				for (z = 1; z < dooda; z++) {
					*p = 0;
					p++;
					}
				*p = x;
				p++;
				}
		palXImage = XCreateImage(myDpy,myVis,planes,ZPixmap,0,
				palImage,256,pal_width,8*dooda,
				256*dooda);
		}
	else {
		for(y = 0; y < 256; y++) 
			for(x = 0; x < pal_width; x++){
				for (z = 1; z < dooda; z++) {
					*p = 0;
					p++;
					}
				*p = 255-y;
				p++;
				}
	palXImage = XCreateImage(myDpy,myVis,planes,ZPixmap,0,palImage,
				pal_width,256,8*dooda,pal_width*dooda);
		}


	return(palXImage); 

} /* MakePalette() */




Widget MakeImageWindow(widget_name,ximage,defpixel,parent,xloc,yloc,xdim,ydim)
char	*widget_name;
XImage	**ximage;
unsigned long defpixel;
Widget	parent;			/* parent widget */
int	xloc,yloc;		/* location to be placed in parent widget */
int	xdim,ydim;		/* size of widget */
{
char	*pbuff;	
Widget	w;		
int	x,y;
Visual	*vis;		
Arg arglist[5];
Cardinal i;

	pbuff = (char *) XIMalloc(xdim*ydim);
/*	for (y = 0; y < ydim ; y++) {
		for(x = 0; x < xdim ; x++)
			pbuff[y*xdim+x]=x;
		}
*/
	for(x=0; x < xdim*ydim; x++)
		pbuff[x]=defpixel;
	
	/* Make palette structure */
	vis = GetVisual(XtDisplay(parent));
	*ximage = XCreateImage(XtDisplay(parent),vis,SCREEN_DEPTH,ZPixmap,0,
		pbuff,xdim,ydim,8,xdim);

	/* Create a HP Static Raster widget */
	i=0;
        XtSetArg(arglist[i],XtNsRimage,*ximage);	i++;
	XtSetArg(arglist[i],XtNx,xloc);		i++;
	XtSetArg(arglist[i],XtNy,yloc);		i++;
        w= XtCreateManagedWidget(widget_name,
                        XwsrasterWidgetClass,parent,arglist,i);

	return(w);
}


void Set_Pointer(w,cursorfont)
/* Set pointer to cursorfont if widget has been mapped */
/* (see X11/cursorfont.h for cursorfonts)( */
Widget	w;
int	cursorfont;
{
Cursor  cursor;
Window  win = XtWindow(w);
Display *dpy = XtDisplay(w);

        if(win) {
            cursor = XCreateFontCursor(dpy,cursorfont);
            XDefineCursor(dpy,win,cursor);
            }

        return;

} /* Set_Pointer() */


Widget MakeVPort(shellName,parent,scrollType,x,y,width,height)
char		*shellName;
Widget		parent;
A_Scroll_t	scrollType;
int			x,y;
int			width,height;
{
	Widget		shellWind;
	Arg			argList[10];
	Cardinal	i=0;

	XtSetArg(argList[i],XtNx,x); i++;
	XtSetArg(argList[i],XtNy,y); i++;
	XtSetArg(argList[i],XtNwidth,width); i++;
	XtSetArg(argList[i],XtNheight,height); i++;

	switch (scrollType)
	{
		case NOSCROLL:
			break;
		case VERTONLY:
			XtSetArg(argList[i],XtNallowVert,True); i++;
			XtSetArg(argList[i],XtNforceBars,True); i++;
			break;
		case HORIZONLY:
			XtSetArg(argList[i],XtNallowHoriz,TRUE); i++;
			XtSetArg(argList[i],XtNforceBars,TRUE); i++;
			break;
		case VERTHORIZ:
			XtSetArg(argList[i],XtNallowVert,TRUE); i++;
			XtSetArg(argList[i],XtNallowHoriz,TRUE); i++;
			XtSetArg(argList[i],XtNforceBars,TRUE); i++;
			break;
	}

	shellWind = XtCreateManagedWidget(shellName,viewportWidgetClass,parent,
									  argList,i);

	return(shellWind);
}


Widget MakeList(shellName,parent,scrollType,vportWind,ncols,callBackProc,
			strings,x,y,width,height,nitems,rowspacing)
char		*shellName;
Widget		parent;
A_Scroll_t	scrollType;
Widget 		*vportWind;	/* returned */
char		*strings[];
int			ncols;
XtCallbackProc	callBackProc;
int			x,y;
int			width,height;
int		nitems;
int		rowspacing;
{
	Widget		listWind;
	Arg			argList[15];
	Cardinal	i=0;

	*vportWind = MakeVPort("XTDvport",parent,scrollType,
							 x,y,width,height);
	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
/*	XtSetArg(argList[i],XtNwidth,width);	i++; 
	XtSetArg(argList[i],XtNheight,height);	i++;
*/

	XtSetArg(argList[i],XtNdefaultColumns,ncols);	i++;
	XtSetArg(argList[i],XtNforceColumns,TRUE);	i++;
/*	XtSetArg(argList[i],XtNverticalList,True);	i++; */
	XtSetArg(argList[i],XtNpasteBuffer,True);	i++;
	XtSetArg(argList[i],XtNlist,strings);	i++;
	if (nitems) {
		XtSetArg(argList[i],XtNnumberStrings,nitems);	i++;
		}
	if (rowspacing) {
		XtSetArg(argList[i],XtNrowSpacing,rowspacing);	i++;
		}

	listWind = XtCreateManagedWidget(shellName,
				listWidgetClass, *vportWind, argList, i);
						
	if (callBackProc)
		XtAddCallback(listWind,"callback",callBackProc,NULL);

	return(listWind);
} /* MakeList */

Widget MakeATopLevel(widget_name,parent,x,y,width,height)
char	*widget_name;
Widget	parent;
int	x,y;
int	width,height;

{
Arg             arglist[10];
Cardinal        i=0;
Widget		w;

	XtSetArg(arglist[i],XtNx,x);		i++;
	XtSetArg(arglist[i],XtNy,y);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;

/*	w = XtCreateManagedWidget(widget_name, transientShellWidgetClass, */
	w = XtCreateManagedWidget(widget_name, applicationShellWidgetClass,
				parent, arglist, i);

/*	XtAddEventHandler(w,EnterWindowMask,0,CBSetInputFocus,NULL);*/
/*	XtAddEventHandler(w,LeaveWindowMask,0,CBSetInputFocus,NULL);*/
	return(w);
} /* MakeATopLevel() */

Widget MakeATopLevelOverride(widget_name,parent,x,y,width,height)
char	*widget_name;
Widget	parent;
int	x,y;
int	width,height;

{
Arg             arglist[10];
Cardinal        i=0;
Widget		w;

	XtSetArg(arglist[i],XtNx,x);		i++;
	XtSetArg(arglist[i],XtNy,y);		i++;
	XtSetArg(arglist[i],XtNwidth,width);	i++;
	XtSetArg(arglist[i],XtNheight,height);	i++;

	w = XtCreateManagedWidget(widget_name, overrideShellWidgetClass,
				parent, arglist, i);

	XtAddEventHandler(w,EnterWindowMask,0,CBSetInputFocus,NULL);

	return(w);
} /* MakeATopLevelOverride() */


Widget MakeRasterImage(shellName,parent,x,y,image,callBackProc,cbp_param)
char		*shellName;
Widget		parent;
int		x,y;
XImage		*image;
XtCallbackProc	callBackProc;
caddr_t		cbp_param;
{
Widget	w;
Arg	argList[10];
Cardinal i=0;

	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNsRimage,image);	i++;
	XtSetArg(argList[i],XtNinvertOnSelect,False);	i++;
	w = XtCreateManagedWidget(shellName,XwsrasterWidgetClass,
				parent,argList,i);
	if (callBackProc)
		XtAddCallback(w,"select",callBackProc,cbp_param);

	return(w);

} /* MakeRasterImage() */

Widget MakeValuator(shellName,parent,x,y,width,height,vert,min,max,origin,
			extent,
			cbpMoved,mdata,cbpReleased,rdata,cbpSelected,sdata)
char		*shellName;
Widget		parent;
int		x,y;
int		width,height;
char		vert;		/*is it vertical ? */
int		min,max,origin;
XtCallbackProc	cbpMoved;
caddr_t		mdata;
XtCallbackProc	cbpReleased;
caddr_t		rdata;
XtCallbackProc	cbpSelected;
caddr_t		sdata;
{
Widget	w;
Arg	argList[10];
Cardinal i=0;
	XtSetArg(argList[i],XtNx,x);	i++;
	XtSetArg(argList[i],XtNy,y);	i++;
	XtSetArg(argList[i],XtNwidth,width);	i++;
	XtSetArg(argList[i],XtNheight,height);	i++;
	XtSetArg(argList[i],XtNsliderMin,min);	i++;
	XtSetArg(argList[i],XtNsliderMax,max);	i++;
	XtSetArg(argList[i],XtNsliderOrigin,origin);	i++;
	XtSetArg(argList[i],XtNsliderExtent,extent);	i++;
	if (!vert)
		XtSetArg(argList[i],XtNslideOrientation,XwHORIZONTAL);	i++;

	w = XtCreateManagedWidget(shellName,XwvaluatorWidgetClass,
				parent,argList,i);
	if (cbpMoved)
		XtAddCallback(w,"sliderMoved",cbpMoved,mdata);
	if (cbpReleased)
		XtAddCallback(w,"sliderReleased",cbpReleased,rdata);
	if (cbpSelected)
		XtAddCallback(w,"areaSelected",cbpSelected,sdata);

	return(w);

} /* MakeValuator() */



DialogAddTrans(w,trans)
Widget w;
char *trans;
{
XtTranslations	transTable;
	
	transTable = XtParseTranslationTable(trans);
	XtOverrideTranslations(gr_DialogGetTextWind(w),transTable);
}
