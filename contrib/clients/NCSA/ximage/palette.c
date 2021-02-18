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
#include <X11/Viewport.h>
#include <X11/Form.h>
#include <X11/Label.h>
#include <X11/cursorfont.h>

#include <Xw/Xw.h>
#include <Xw/Toggle.h>

#include <stdio.h>
#include <math.h>

#include "buttons.h"
#include "xstuff.h"
#include "paletteP.h"
#include "getfile.h"

#define PALCOMPHEIGHT	100
#define PAL_WIDTH	28

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* copy a color cell */
#define cpccell(a,b)	{\
			a.red = b.red; \
			a.green = b.green; \
			a.blue = b.blue;\
			a.flags = b.flags; \
			a.pad = b.pad;\
			}

#define MAX3(a,b,c) ((a>b)&&(a>c)) ? a : ((b > c)? b : c)
#define MIN3(a,b,c) ((a<b)&&(a<c)) ? a : ((b < c)? b : c)
#define SIGN(a) ((a)?(a/abs(a)):1) /* return 1 if zero */


extern	Widget	toplevelform;			/* The top dude (composite)*/
extern	void CBEditPalEntry();

unsigned long white;
unsigned long black;

static	Display	*myDpy;				/* Display for this program*/
static	Visual	*myVis;
static	Widget	top_pal_ctrl_widget;		/* pal ctrl window ap shell*/
static	Widget	pal_ctrl_widget;		/* composite widget ctrl win*/
static	XWindowAttributes pcw_attrib;		/* window attributes */
static	char	pal_loaded	= FALSE;	/* Is the palette loaded */
/*static*/	Colormap fcmap;
static	char palette_ctrl_window_showing = FALSE;	/* self explanitory */
static	XColor	uccells[256];			/* Undo to these colors*/
static	XColor	bccells[256];			/* Back up copy */
	XColor	rccells[256];			/* current colors */
	XColor	origccells[256];		/* original copy */
static	int	numColors;		/* max num colors on this screen*/

static	char	pal_sfile[80];			/* palette save file name */
static	char	use_entire_palette=FALSE;
static	XImage	*palImage;			/* the palette display XImage*/
static	Widget	pal_image;			/* The palette display window*/
static	crlastx = 0;
static	Widget pal_comp;
static	XImage *pal_comp_image;		/* Stores component pal image*/
static	GC 	pal_comp_gc;
static	int	comp_pen=0;		/* color to draw new component */
static	unsigned long comp_pen_dcolor;	/* color to draw comp pen */
static	int	comp_last_x=0;
static	int	comp_last_y=0;
static	XPoint	rpoints[256];		/*drawn red points(CBDrawComponent)*/
static	XPoint	gpoints[256];		/*drawn green points*/
static	XPoint	bpoints[256];		/*drawn blue points*/
static	XPoint	cpoints[256];		/*drawn cyan  */
static	XPoint	mpoints[256];		/*drawn magenta  */
static	XPoint	ypoints[256];		/*drawn yellow */
static	XPoint	hpoints[256];		/*drawn hue */
static	XPoint	spoints[256];		/*drawn saturation*/
static	XPoint	vpoints[256];		/*drawn value */
static	unsigned int	rgbcmyhsv=0;	/* What's plotted (r=(1<<9),v=(1<<1))*/
static	struct	GetFile	palfile;
static	palRedraw = FALSE;

void	CBInitRotate();
void	CBReset();
void	CBEvalExpr();
void	CBNewLoadPal();
void	CBSetPen();
void	CBUnSetPen();
void	CBPrepChangeComp();
void	CBChangeComponent();
void	CBEndChangeComp();
void	RedrawComponent();
void	CBLoadPal();
void	CBTranspose();
void	TransposeCMap();
void	CBFiddle();
void	CRotate();
char	*Get_Save_Pal();
int	ChooseColor();
void	CBRedrawComponents();
void	CBCompKey();
void	CBPalSave();
void	CBUndo();
void	SaveForUndo();
void	CBSmooth();
void	CBInstallCMap();
void	CBUnInstallCMap();
void	CBDrawComponent();
void	CBUnDrawComponent();



void StoreColors(ccells,redraw,cmap)
/* Store colors in colormap */
XColor	ccells[];	/* colors to be stored */
int	redraw;		/* Should I redraw components */
Colormap cmap;		/* colormap to store colors in */
/* store colors in colormap, except for black & white if 
 * the global variable use_entire_palette is set to false*/
{
Visual *vis;

	vis = myVis;
	if (vis->class == StaticColor) {
		PrintMesg("Can't set colors for this StaticColor display\n");
		return;
		}
	if (vis->class != PseudoColor) {
	    PrintMesg("Can't set colors for this non-PseudoColor display\n");
	    return;
	    }
	if (!use_entire_palette) {
		XColor	inblackcell;
		XColor	inwhitecell;

		inblackcell = ccells[black];
		inwhitecell = ccells[white];

		/* restore black and white */
		ccells[black].red   = (unsigned short) 0;
		ccells[black].blue  = (unsigned short) 0;
		ccells[black].green = (unsigned short) 0;
		ccells[black].flags = DoRed|DoGreen|DoBlue;

		ccells[white].red   = (unsigned short) 65535;
		ccells[white].blue  = (unsigned short) 65535;
		ccells[white].green = (unsigned short) 65535;
		ccells[white].flags = DoRed|DoGreen|DoBlue;

		XStoreColors(myDpy,cmap,ccells,numColors);

		/* take it back out */
		ccells[black]=inblackcell;
		ccells[white]=inwhitecell;
		}
	else
		XStoreColors(myDpy,cmap,ccells,numColors);

	if (redraw)
		RedrawComponent();

	return;
} /* StoreColors() */



static void CBUseEntirePalette(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
static Arg use[]={{XtNlabel,(int)"Use entire palette"},{XtNset,True}};
static Arg dont[]={{XtNlabel,(int)"Don't use entire palette"},{XtNset,True}};

	use_entire_palette = ! use_entire_palette;
	StoreColors(rccells,TRUE,fcmap);

	if (use_entire_palette) {
		/*PrintMesg("Use entire palette\n");*/
		XtSetValues(w,use,2);
		}
	else {
		/*PrintMesg("Don't use entire palette\n");*/
		XtSetValues(w,dont,2);
		}
}


void ClosePaletteBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
/*	XtUnmanageChild(top_pal_ctrl_widget);*/
	if (palette_ctrl_window_showing) {
		CBUnInstallCMap(top_pal_ctrl_widget,NULL,NULL);
		XtDestroyWidget(top_pal_ctrl_widget);
		}


	palette_ctrl_window_showing = FALSE;
}

int InitPalette(dpy)
Display *dpy;
{
int x;
int screen;
int planes;

	myDpy = dpy;
	myVis = GetVisual(dpy);
	screen = DefaultScreen(dpy);
	planes = DisplayPlanes(myDpy,screen);

	white = WhitePixel(dpy,screen);
	black = BlackPixel(dpy,screen);

	numColors = XDisplayCells(dpy,screen);
	if (numColors > 256) {
		numColors = 256;   /* only use max 256 color entries */
		}

	/* Create startup default palette (use X servers) */ 
	for(x=0;x < 256; x++)
		origccells[x].pixel = x;
	XQueryColors(dpy,DefaultColormap(dpy,screen),
			origccells,numColors);
	for(x=0;x < 256; x++) {
		rccells[x].pixel = x;
		cpccell(rccells[x],origccells[x]);
		}

	if (!(palImage = MakePalette(myDpy,PAL_WIDTH,TRUE)))
		return(0);

	return(1); /* initialize ok */
}


void CBInitPal(w,client_data,call_data)
/* This procedure called on exposure of palette control window*/
/* Sets the colormap for the window */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display *dpy;

	w = top_pal_ctrl_widget;
/*	printf("CBInitPal() I've been called\n");*/
	fflush(stdout);
	dpy = XtDisplay(w);
	XSync(dpy,False);
#ifndef DEF_STAT_VISUAL
	XSetWindowColormap(dpy,XtWindow(w),fcmap);
#endif
	XtRemoveEventHandler(w,ExposureMask,0,CBInitPal,NULL);

	/* I don't think this command should be here, but it seems *
	 * to make it work.....oh well... 			   *
	 * Shake up the ICCCM window manager maybe? 		   */
/**/	XInstallColormap(dpy,fcmap);

#ifndef DEF_STAT_VISUAL
	{/* Make compatible with Release 3 window managers */
		/* try this twice since it's not working */
		XSetWindowAttributes attrib;
		attrib.colormap = fcmap;

		XChangeWindowAttributes(dpy,XtWindow(w),
					CWColormap,&attrib);
		XSync(dpy,False);
		XSetWindowColormap(dpy,XtWindow(w),fcmap);
		XSync(dpy,False);
	}
#endif

}


void PaletteBox(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Widget	buttonbox1;
Widget	buttonbox2;
Widget	buttonbox3;
Display *dpy;
Visual	*vis;
int x;
Arg             arglist[10];
Cardinal        i=0;
int	xpos,ypos;

	if (palette_ctrl_window_showing) {
		PrintMesg("Palette window already open\n");
		return;
		}

	dpy = XtDisplay(toplevelform);
	vis = GetVisual(myDpy);
	if (vis->class != PseudoColor) {
		PrintMesg("Can't use X Image Palette on this display.\n");
		PrintMesg("Not a Pseudo Color device\n");
		return;
		}
/*
	if ( DisplayPlanes(dpy,XDefaultScreen(dpy)) != 8 ) {
		PrintMesg("Can't use X Image Palette on this display.\n");
		PrintMesg("Not an 8 bit display\n");
		return;
		}
*/
		

	PrintMesg("Opening Palette Box\n");

	rgbcmyhsv = 0;
	comp_pen = 0;
	comp_last_x = comp_last_y = 0;
	crlastx = 0;

	PlaceWindow(&xpos,&ypos);
	top_pal_ctrl_widget = MakeATopLevel("NCSA X Image [Palette]",
				toplevelform,xpos,ypos,360,285+PALCOMPHEIGHT);
	pal_ctrl_widget = Make_Window("NCSA X Image [Palette]",
				top_pal_ctrl_widget,5,5,360,285+PALCOMPHEIGHT);
/**/	XSync(dpy,False);
	fcmap = XCreateColormap(dpy, XtWindow(top_pal_ctrl_widget),
			GetVisual(dpy),AllocAll);
	for(x=0;x < 256; x++)
		origccells[x].pixel = x;
	XQueryColors(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),
			origccells,numColors);
	for(x=0;x < 256; x++) {
		uccells[x].pixel = rccells[x].pixel = x;
		cpccell(rccells[x],origccells[x]);
		cpccell(uccells[x],origccells[x]);
		}


/*#ifdef	NON_ICCCM_WINDOW_MANAGER*/
	XInstallColormap(dpy,fcmap);
	/* Make compatible with Release 2 window managers */
	XtAddEventHandler(top_pal_ctrl_widget,EnterWindowMask,FALSE,
			CBInstallCMap,NULL);
	XtAddEventHandler(top_pal_ctrl_widget,LeaveWindowMask,FALSE,
			CBUnInstallCMap,NULL);
/*#else*/

	XSync(dpy,False);
	{/* Make compatible with Release 3 window managers */
#ifndef DEF_STAT_VISUAL
		XSetWindowAttributes attrib;
		attrib.colormap = fcmap;

		XChangeWindowAttributes(dpy,XtWindow(top_pal_ctrl_widget),
					CWColormap,&attrib);
		XSetWindowColormap(dpy,XtWindow(top_pal_ctrl_widget),fcmap);
#endif

        XtAddEventHandler(pal_ctrl_widget,ExposureMask,0,CBInitPal,NULL);

	}
	XSync(dpy,False);
/*#endif*/


	buttonbox1 = Make_ButtonBox(pal_ctrl_widget,1,200+PALCOMPHEIGHT,
					355,80);
	b[PAL_LOAD] = MakeButton("Load",buttonbox1,5,1,CBNewLoadPal,NULL,0,0);
	b[PAL_RESET] = MakeButton("Reset",buttonbox1,40,1,CBReset,NULL,0,0);
	(void) MakeButton("Undo",buttonbox1,83,1,CBUndo,NULL,0,0);
	b[PAL_SMOOTH] = MakeButton("Smooth",buttonbox1,300,1,
				CBSmooth,&comp_pen,0,0);
#ifdef UNDER_DEVELOPMENT
	(void) MakeButton("Edit Pal Entry",buttonbox1,142,1,CBEditPalEntry,
					rccells,0,0);
	b[PAL_EVAL] = MakeButton("Eval Expr",buttonbox1,240,1,
				CBEvalExpr,NULL,0,0);
#endif



	b[PAL_SAVE] = MakeButton("Save",buttonbox1,5,30,CBPalSave,NULL,0,0);
	pal_sfile[0]='\0';
	(void)Make_Text_Inp("pal_save_file",buttonbox1,
			pal_sfile,80,45,32,300,20);

	(void)MakeAltToggle("Don't use entire palette",buttonbox1,45,60,
			CBUseEntirePalette,NULL,CBUseEntirePalette,NULL);
/*	(void)MakeAltToggle("Lock in palette",buttonbox1,210,60,
			NULL,NULL,NULL,NULL);
*/

	buttonbox2 = Make_ButtonBox(pal_ctrl_widget,1,60+PALCOMPHEIGHT,
					80,135);
	(void)	Make_Label("Wholistic",buttonbox2,1,1,78);
	b[PAL_FIDDLE] = MakeButton("Fiddle",buttonbox2,5,25,CBFiddle,NULL,0,0);
	b[PAL_INVERT] = MakeButton("Flip",buttonbox2,5,55,CBTranspose,NULL,0,0);
/*b[PAL_T_ROTATE] =MakeButton("Track Rotate",buttonbox2,70,25,
			CBRotate,NULL,0,0);*/
/*	b[PAL_ROTATE] = MakeButton("Rotate",buttonbox2,5,85,NULL,NULL,0,0);*/
	Set_Pointer(buttonbox2,XC_hand2);

	buttonbox3 = Make_ButtonBox(pal_ctrl_widget,85,60+PALCOMPHEIGHT,
					270,135);
	(void)Make_Label("Component Palette Manipulation",buttonbox3,1,1,268);
	b[PAL_PRED] = Make_Toggle("",buttonbox3,5,25,
				CBDrawComponent,CRED,
				CBUnDrawComponent,CRED,True,False);
	b[PAL_RED] = Make_Toggle("Red",buttonbox3,25,25,
				CBSetPen,CRED,CBUnSetPen,CRED,False,False);
	b[PAL_PGREEN] = Make_Toggle("",buttonbox3,5,55,
				CBDrawComponent,CGREEN,
				CBUnDrawComponent,CGREEN,True,False);
	b[PAL_GREEN] = Make_Toggle("Green",buttonbox3,25,55,
				CBSetPen,CGREEN,CBUnSetPen,CGREEN,False,False);
	b[PAL_PBLUE] = Make_Toggle("",buttonbox3,5,85,
				CBDrawComponent,CBLUE,
				CBUnDrawComponent,CBLUE,True,False);
	b[PAL_BLUE] = Make_Toggle("Blue",buttonbox3,25,85,
				CBSetPen,CBLUE,CBUnSetPen,CBLUE,False,False);
	b[PAL_PCYAN] = Make_Toggle("",buttonbox3,80,25,
				CBDrawComponent,CCYAN,
				CBUnDrawComponent,CCYAN,True,False);
	b[PAL_CYAN] = Make_Toggle("Cyan",buttonbox3,100,25,
				CBSetPen,CCYAN,CBUnSetPen,CCYAN,False,False);
	b[PAL_PMAGENTA] = Make_Toggle("",buttonbox3,80,55,
				CBDrawComponent,CMAGENTA,
				CBUnDrawComponent,CMAGENTA,True,False);
	b[PAL_MAGENTA] = Make_Toggle("Magenta",buttonbox3,100,55,
				CBSetPen,CMAGENTA,CBUnSetPen,CMAGENTA,
				False,False);
	b[PAL_PYELLOW] = Make_Toggle("",buttonbox3,80,85,
				CBDrawComponent,CYELLOW,
				CBUnDrawComponent,CYELLOW,True,False);
	b[PAL_YELLOW] = Make_Toggle("Yellow",buttonbox3,100,85,
				CBSetPen,CYELLOW,CBUnSetPen,CYELLOW,
				False,False);
	b[PAL_PHUE] = Make_Toggle("",buttonbox3,170,25,
				CBDrawComponent,CHUE,
				CBUnDrawComponent,CHUE,True,False);
	b[PAL_HUE] = Make_Toggle("Hue",buttonbox3,190,25,
				CBSetPen,CHUE,CBUnSetPen,CHUE,False,False);
	b[PAL_PSATURATION] = Make_Toggle("",buttonbox3,170,55,
				CBDrawComponent,CSATURATION,
				CBUnDrawComponent,CSATURATION,True,False);
	b[PAL_SATURATION] = Make_Toggle("Saturation",buttonbox3,190,55,
				CBSetPen,CSATURATION,CBUnSetPen,CSATURATION,
				False,False);
	b[PAL_PVALUE] = Make_Toggle("",buttonbox3,170,85,
				CBDrawComponent,CVALUE,
				CBUnDrawComponent,CVALUE,True,False);
	b[PAL_VALUE] = Make_Toggle("Value",buttonbox3,190,85,
				CBSetPen,CVALUE,CBUnSetPen,CVALUE,
				False,False);
	Set_Pointer(buttonbox3,XC_hand2);

	b[PAL_DONE]=MakeButton("Done",pal_ctrl_widget,320,25,
				ClosePaletteBox,NULL,0,0);

/*        pal_image = Make_Pal_Window(pal_ctrl_widget,47,23,255,28);*/
	pal_image = MakeRasterImage("PaletteDisplay",pal_ctrl_widget,
				47,23,palImage,NULL,NULL);

	pal_comp = Make_Board("Palette_Components",pal_ctrl_widget,47,55,255,
				PALCOMPHEIGHT+2);
	pal_comp = MakeWorkSpace("Palette Components",pal_ctrl_widget,
				47,55,255,PALCOMPHEIGHT+2,
				CBRedrawComponents,NULL,
				CBCompKey,NULL);
	/*(void) MakeImageWindow("Palette_comp_canvas",&pal_comp_image,white,
				pal_comp,0,0,255,PALCOMPHEIGHT);
	*/
	{
	static XGCValues gcvalues;
	gcvalues.foreground = black;
	gcvalues.background = white;
	pal_comp_gc = XtGetGC(pal_comp,(GCForeground|GCBackground),&gcvalues);
	}

	
	/* Component palette display window drawing */
	XtAddEventHandler(pal_comp,ButtonPressMask,0,
			CBPrepChangeComp,NULL );
        XtAddEventHandler(pal_comp,Button1MotionMask,0,
			CBChangeComponent,NULL );
	XtAddEventHandler(pal_comp,ButtonReleaseMask|LeaveWindowMask,0,
			CBEndChangeComp,NULL );
	Set_Pointer(pal_comp,XC_pencil);


        XtAddEventHandler(pal_image,ButtonPressMask,0,CBInitRotate,NULL );
        XtAddEventHandler(pal_image,Button1MotionMask,0,CRotate,NULL );
        Set_Pointer(pal_image,XC_sb_h_double_arrow);

	XSync(dpy,False);
	StoreColors(rccells,TRUE,fcmap);
	XSync(dpy,False);
	palette_ctrl_window_showing = TRUE;

	return;

} /* PaletteBox() */



static void CBRedrawComponents(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	RedrawComponent();
}

static void CBCompKey(w,client_data,call_data)
/* this routine is suppose to set which component to draw with 
 * but the HP workspace widget doesn't seem to be returning anything
 * meaningful when a key pressed.... either that or I don't know how
 * to use it. 
 */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
/*	printf("Just pressed %d key(call_data)\n",(int) call_data);
	printf("Just pressed %c key(call_data)\n",(char *) call_data);
	printf("Just pressed %d key(client_data)\n",(int) client_data);
*/
	/* will use CBSetPen() */
	/* then set the toggle */
}

static void CBTranspose(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	TransposeCMap();
	return;
}

void CBInstallCMap(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XInstallColormap(XtDisplay(w),fcmap);
}

void CBUnInstallCMap(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XUninstallColormap(XtDisplay(w),fcmap);
}

static void CBInitRotate(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Window root, child;
int	root_x,root_y;
int	win_x,win_y;
unsigned int	keys_buttons;

	SaveForUndo(rccells);
	if (XQueryPointer(XtDisplay(w),XtWindow(w),&root,&child,
		&root_x,&root_y,&win_x,&win_y,&keys_buttons)) {
		crlastx = win_x;
		}
}

static void GotPalFile()
{
	CBLoadPal(top_pal_ctrl_widget,NULL,NULL);
	
}

static void DidntGetPalFile()
{
	PrintMesg("Palette load canceled\n");
}

static void CBNewLoadPal(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	palfile.type = FTYPE_PAL;
	palfile.yesfile = GotPalFile;
	palfile.nofile = DidntGetPalFile;
	GetAFile(&palfile);
}



static void CBLoadPal(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Display *dpy = XtDisplay(w);
int x;
char fileName[512];

	SaveForUndo(rccells);

        sprintf(fileName,"%s/%s",palfile.directory,*(palfile.file));
	if (! palfile.isHDF) {
		if(! ReadSEQPalette(fileName,origccells))
			return;	/* Error reading Colormap */
		}
	else {
		if(! ReadHDFPalette(fileName,origccells))
			return;	/* Error reading Colormap */
		}

	for(x=0; x < 256 ; x++) {
                rccells[x].pixel = x;
                cpccell(rccells[x],origccells[x]);
                }

        StoreColors(rccells,TRUE,fcmap);


	pal_loaded = TRUE;
	return;

} /* CBLoadPal() */



static void DrawComponent(component)
/* Draw the color components in the component window */
int	component;	/* which component to draw */
{
int x;
int y;
XGCValues	gcval;

        gcval.foreground = ChooseColor(rccells,component);
        XChangeGC(XtDisplay(pal_comp), pal_comp_gc, GCForeground, &gcval);
	switch (component){
		case CRED :
			for (x = 0 ; x < 256 ; x++ ) {
				y = (int) ((float) (rccells[x].red >> 8)* 
						(float) (100.0 / 255.0));
				rpoints[x].x = x;
				rpoints[x].y = 100 - y;
				}
			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,rpoints,256,CoordModeOrigin);
			break;
		case CGREEN:
			for (x = 0 ; x < 256 ; x++ ) {
				y = (int) ((float) (rccells[x].green >> 8)* 
						(float) (100.0 / 255.0));
				gpoints[x].x = x;
				gpoints[x].y = 100 - y;
				}
			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,gpoints,256,CoordModeOrigin);
			break;
		case CBLUE:
			for (x = 0 ; x < 256 ; x++ ) {
				y = (int) ((float) (rccells[x].blue >> 8)* 
						(float) (100.0 / 255.0));
				bpoints[x].x = x;
				bpoints[x].y = 100 -y;
				}
			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,bpoints,256,CoordModeOrigin);
			break;
		case CCYAN:
			for (x = 0 ; x < 256 ; x++ ) {
				y = (int) ((float) (rccells[x].red >> 8)* 
						(float) (100.0 / 255.0));
				cpoints[x].x = x;
				cpoints[x].y = y;
				}
			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,cpoints,256,CoordModeOrigin);
			break;
		case CMAGENTA:
			for (x = 0 ; x < 256 ; x++ ) {
				y = (int) ((float) (rccells[x].green >> 8)* 
						(float) (100.0 / 255.0));
				mpoints[x].x = x;
				mpoints[x].y = y;
				}
			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,mpoints,256,CoordModeOrigin);
			break;
		case CYELLOW:
			for (x = 0 ; x < 256 ; x++ ) {
				y = (int) ((float) (rccells[x].blue >> 8)* 
						(float) (100.0 / 255.0));
				ypoints[x].x = x;
				ypoints[x].y = y;
				}
			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,ypoints,256,CoordModeOrigin);
			break;
		case CHUE: {
			double	h;
			int	r,g,b; 
			int	max,min;
			int	range;
			double	rl,gl,bl; 

			for (x = 0 ; x < 256 ; x++ ) {
			    r = (int) rccells[x].red >> 8;
			    g = (int) rccells[x].green >> 8;
			    b = (int) rccells[x].blue >> 8;
		
			    max = MAX3(r,g,b);
			    min = MIN3(r,g,b);

			    range = max - min;
			    if (range) {
				rl = ((double) (max  - r)) / ((double) range);
				gl = ((double)  (max - g)) / ((double) range);
				bl = ((double) (max - b )) /  ((double) range);

				if ( max == r ) {
					if ( min == g)
						h = 5.0 + bl;
					else
						h = 1.0 - gl;
					}
				else {
					if ( max == g) {
					    if ( min == b)
						h = 1.0 + rl;
					    else
						h = 3.0 - bl;
					    }
					else {
					    if (min == r) 
						h = 3.0 + gl;
					    else
						h = 5.0 - rl;
					    }
					}
				h = h * 60.0;
			
				}

				y = (int) ((double)(h * (100.0 / 360.0)));
		
				hpoints[x].x = x;
				hpoints[x].y = 100 - y;
			} /*for*/


			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,hpoints,256,CoordModeOrigin);
			}
			break; /*CHUE*/
		case CSATURATION: {
			int r,g,b;
			int s;
			int max,min;

			for (x = 0 ; x < 256 ; x++ ) {
				r = (int) (((float) (rccells[x].red >> 8))* 
						(float) (100.0 / 255.0));
				g = (int) (((float) (rccells[x].green >> 8))* 
						(float) (100.0 / 255.0));
				b = (int) (((float) (rccells[x].blue >> 8))* 
						(float) (100.0 / 255.0));

				max = MAX3(r,g,b);
				min = MIN3(r,g,b);

				if (max != 0)
		    		    s=(int)(100.0 *(((float) (max - min)) / 
						((float) max)));
				else
				     s = 0;
		
				spoints[x].x = x;
				spoints[x].y = 100 - s;
				}
			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,spoints,256,CoordModeOrigin);
			}
			break;
		case CVALUE: {
			int r,g,b;
			int value;
			for (x = 0 ; x < 256 ; x++ ) {
				r = (int) ((float) (rccells[x].red >> 8)* 
						(float) (100.0 / 255.0));
				g = (int) ((float) (rccells[x].green >> 8)* 
						(float) (100.0 / 255.0));
				b = (int) ((float) (rccells[x].blue >> 8)* 
						(float) (100.0 / 255.0));

				value = (int) MAX3(r,g,b);

				vpoints[x].x = x;
				vpoints[x].y = 100 - value;
				}
			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,vpoints,256,CoordModeOrigin);
			}
			break;
		} /* switch */

			
	
	rgbcmyhsv = rgbcmyhsv | component;
	return;
	
} /* DrawComponent() */

static void CBDrawComponent(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	DrawComponent((int) client_data);
}


static void UnDrawComponent(component)
int component;
{
int x;
int y;
XGCValues	gcval;
static	Arg arglist[]={{XtNset,(XtArgVal) True}};

        gcval.foreground = white;
        XChangeGC(XtDisplay(pal_comp), pal_comp_gc, GCForeground, &gcval);
	switch (component) {
		case CRED:
			if ((comp_pen == CRED) && (! palRedraw)){
				XtSetValues(b[PAL_PRED],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,rpoints,256,CoordModeOrigin);
			break;
		case CGREEN:
			if ((comp_pen == CGREEN) && (! palRedraw)){
				XtSetValues(b[PAL_PGREEN],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,gpoints,256,CoordModeOrigin);
			break;
		case CBLUE:
			if ((comp_pen == CBLUE) && (! palRedraw)){
				XtSetValues(b[PAL_PBLUE],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,bpoints,256,CoordModeOrigin);
			break;
		case CCYAN:
			if ((comp_pen == CCYAN) && (! palRedraw)){
				XtSetValues(b[PAL_PCYAN],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,cpoints,256,CoordModeOrigin);
			break;
		case CMAGENTA:
			if ((comp_pen == CMAGENTA) && (! palRedraw)){
				XtSetValues(b[PAL_PMAGENTA],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,mpoints,256,CoordModeOrigin);
			break;
		case CYELLOW:
			if ((comp_pen == CYELLOW) && (! palRedraw)){
				XtSetValues(b[PAL_PYELLOW],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,ypoints,256,CoordModeOrigin);
			break;
		case CHUE:
			if ((comp_pen == CHUE) && (! palRedraw)){
				XtSetValues(b[PAL_PHUE],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,hpoints,256,CoordModeOrigin);
			break;
		case CSATURATION:
			if ((comp_pen == CSATURATION) && (! palRedraw)){
				XtSetValues(b[PAL_PSATURATION],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,spoints,256,CoordModeOrigin);
			break;
		case CVALUE:
			if ((comp_pen == CVALUE) && (! palRedraw)){
				XtSetValues(b[PAL_PVALUE],arglist,1);
				return;
				}

			XDrawPoints(XtDisplay(pal_comp),XtWindow(pal_comp),
				pal_comp_gc,vpoints,256,CoordModeOrigin);
			break;
	} /* switch */

	rgbcmyhsv = rgbcmyhsv ^ (rgbcmyhsv & component) ;
	if (! palRedraw)
		RedrawComponent();

} /* UnDrawComponent() */

static void CBUnDrawComponent(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	UnDrawComponent((int) client_data);
}



static void CBReset(w,client_data,call_data)
/* Set colormap back to last loaded colors */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int x;
	SaveForUndo(rccells);
	for(x=0; x < 256 ; x++) {
		rccells[x].pixel = x;
		cpccell(rccells[x],origccells[x]);
		}
	RedrawComponent();
	StoreColors(rccells,TRUE,fcmap);
} /* CBReset() */


static void CBEvalExpr(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	/* Not working yet, but we'll blame it on the user for
	   not knowing how to use this feature */
	PrintMesg("Bad expression in cut/paste buffer\n");
}


static void TransposeCMap()
/* Flip the colormap:  (0<-> 255, 1<->254, etc.)*/
{
int	x;
XColor	tmp;

	SaveForUndo(rccells);
	for (x=0; x < 128 ;x++) {
		cpccell(tmp,rccells[255-x]);
		cpccell(rccells[255-x],rccells[x]);
		cpccell(rccells[x],tmp);
		}
	StoreColors(rccells,TRUE,fcmap);

	return;

} /* TransposeCMap() */


static void FiddlePalette(w,xdim,ydim,xpos,ypos)
Widget w;
int	xdim,ydim;
int	xpos,ypos;
{
double a,b,b1;
double unit_Theta = (double) 3.14159 / (double) (ydim*2);
double up,down;
register int prej,i,j,k;
int	NUMCOLORS = 256;

	xpos = (((xpos < 0)?0:xpos)>xdim)?xdim:xpos;
	ypos = (((ypos < 0)?0:ypos)>ydim)?ydim:ypos;

	a = (double) tan((double) (unit_Theta * (double) (ydim - ypos)));

	up = 1 + (short) (127.0 * a);
	down = 256 - (short) (128.0 * a);
	if (up > down)
		b = (((double)xpos / (double)xdim) * (double)(up - down))
			 + (double) down;
	else
		b = (((double)xpos / (double)xdim) * (double)(down - up))
			 + (double) up;
	
	prej = 1;

	for ( i = 1 ; i <= NUMCOLORS - 2 ; i++) {
	    j = (short) (a * (double) (i - (NUMCOLORS/2)) + b);
	    if ((j > 0) && (j <= NUMCOLORS-2)) {
		for ( k = prej; k <= j - 1 ; k++)
		   /*if((rccells[k].pixel!=white)&&(rccells[k].pixel!=black))*/
				cpccell(rccells[k],bccells[i]);
		/*if ((rccells[j].pixel!=white)&&(rccells[j].pixel!=black))*/
			cpccell(rccells[j],bccells[i]);
		}
	    else
		if (j >= NUMCOLORS-1) {
			for ( k = prej; k <= NUMCOLORS-2; k++)
				/*if ((rccells[k].pixel!=white)
				    &&(rccells[k].pixel!=black))*/
					cpccell(rccells[k],bccells[i]);
			break;
			}
		else
			continue;

	    prej = j + 1;
 	    }

	for(;j < NUMCOLORS;j++) 
		/*ccells[j].pixel = bccells[255].pixel;*/
		cpccell(rccells[j],bccells[255]);

        StoreColors(rccells,TRUE,fcmap);
}

static void Cfiddle(w,client_data,call_data)
/* Do linear expansion and compression on palette */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Window root, child;
int	root_x,root_y;
int	win_x,win_y;
unsigned int	keys_buttons;

	if (XQueryPointer(XtDisplay(w),XtWindow(w),&root,&child,
	    &root_x,&root_y,&win_x,&win_y,&keys_buttons)) {
		FiddlePalette(w,pcw_attrib.width,pcw_attrib.height,
			win_x,win_y);
		}

} /* Cfiddle() */


static void CBPrepFiddle(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int	x;
	SaveForUndo(rccells);
	for (x=0; x< 256 ; x++) {
		cpccell(bccells[x],rccells[x]);
		bccells[x].pixel = rccells[x].pixel;
		}
}

static void CBEndFiddle(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Window root, child;
int     root_x,root_y;	/* X & Y coordinates with respect to root */
int     win_x,win_y;	/* X & Y coordinates with respect to window */
unsigned int    keys_buttons;
int	win;

	win = XtWindow(w);

        if (XQueryPointer(XtDisplay(w),win,&root,&child,
                &root_x,&root_y,&win_x,&win_y,&keys_buttons)) {
		XWarpPointer(XtDisplay(w),win,win,win_x,win_y,
		pcw_attrib.width,pcw_attrib.height,30,200);
		}
	XUngrabPointer(XtDisplay(w),CurrentTime);
        XtRemoveEventHandler(pal_ctrl_widget,Button1MotionMask,0,Cfiddle,NULL);
	XtRemoveEventHandler(pal_ctrl_widget,ButtonReleaseMask,0,
			CBEndFiddle,NULL);
} /* CBEndFiddle() */


static void CBFiddle(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int x;

	if(XGrabPointer(XtDisplay(w),XtWindow(pal_ctrl_widget),False,
		PointerMotionMask|ButtonReleaseMask,
		GrabModeAsync,GrabModeSync,
		XtWindow(pal_ctrl_widget),
		XCreateFontCursor(XtDisplay(w),XC_fleur),
		CurrentTime)) {
			PrintMesg("Can't grab pointer for fiddle\n");
			return;
			}

        XtAddEventHandler(pal_ctrl_widget,Button1MotionMask,0,Cfiddle,NULL );
	XtAddEventHandler(pal_ctrl_widget,ButtonReleaseMask,0,
			CBEndFiddle,NULL);

	CBPrepFiddle(pal_ctrl_widget,NULL,NULL);


	XGetWindowAttributes(XtDisplay(w),XtWindow(pal_ctrl_widget),
			&pcw_attrib);
} /* CBFiddle() */

static RotatePalette(w,xdim,ydim,xpos,ypos)
Widget w;
int xdim,ydim;
int xpos,ypos;
{
int offset;
int x;
int i;
int end;
int new;
int	tmp[256];
int	pno;


	if ((crlastx)&& (crlastx != xpos)) {
		for(x = 0;x < 256; x++)
			cpccell(bccells[x],rccells[x]);

		offset = xpos - crlastx;

		for(x=0; x < 256; x++) {
			new = x + offset;
			new = (new > 255) ? (new - 256) : 
					(new < 0) ? (new + 256): new;
			cpccell(rccells[new],bccells[x]);
			}

		StoreColors(rccells,TRUE,fcmap);
		}
	crlastx = xpos;
	return;

} /* RotatePalette() */


static void CRotate(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Window root, child;
int     root_x,root_y;	/* X & Y coordinates with respect to root */
int     win_x,win_y;	/* X & Y coordinates with respect to window */
unsigned int    keys_buttons;

        if (XQueryPointer(XtDisplay(w),XtWindow(w),&root,&child,
                &root_x,&root_y,&win_x,&win_y,&keys_buttons)) {

		if ((win_x <= 255) && (win_x > 0))
	                RotatePalette(w,255,28,win_x,win_y);
                }
}




void Terminate(s)
char *s;
{
        while (*s > ' ')
                s++;
        *s='\0';
	return;
}
static char *Get_Save_Pal()
{
        Terminate(pal_sfile);
        return(pal_sfile);
}


static void RedrawComponent()
{
	palRedraw = TRUE;
	if (rgbcmyhsv & CRED ){
		UnDrawComponent(CRED);
		DrawComponent(CRED);
		}
	if (rgbcmyhsv & CGREEN) {
		UnDrawComponent(CGREEN);
		DrawComponent(CGREEN);
		}
	if (rgbcmyhsv & CBLUE) {
		UnDrawComponent(CBLUE);
		DrawComponent(CBLUE);
		}
	if (rgbcmyhsv & CCYAN) {
		UnDrawComponent(CCYAN);
		DrawComponent(CCYAN);
		}
	if (rgbcmyhsv & CMAGENTA) {
		UnDrawComponent(CMAGENTA);
		DrawComponent(CMAGENTA);
		}
	if (rgbcmyhsv & CYELLOW) {
		UnDrawComponent(CYELLOW);
		DrawComponent(CYELLOW);
		}
	if (rgbcmyhsv & CHUE) {
		UnDrawComponent(CHUE);
		DrawComponent(CHUE);
		}
	if (rgbcmyhsv & CSATURATION) {
		UnDrawComponent(CSATURATION);
		DrawComponent(CSATURATION);
		}
	if (rgbcmyhsv & CVALUE) {
		UnDrawComponent(CVALUE);
		DrawComponent(CVALUE);
		}
	palRedraw = FALSE;
	return;
} /* RedrawComponent() */

static void CBSetPen(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Arg	arglist[10];
Cardinal i=0;

	XtSetArg(arglist[i],XtNset,False);	i++;
	/*****reset old toggle*/
	switch(comp_pen) {
		case CRED:
			XtSetValues(b[PAL_RED],arglist,i);
			break;
		case CGREEN:
			XtSetValues(b[PAL_GREEN],arglist,i);
			break;
		case CBLUE:
			XtSetValues(b[PAL_BLUE],arglist,i);
			break;
		case CCYAN:
			XtSetValues(b[PAL_CYAN],arglist,i);
			break;
		case CMAGENTA:
			XtSetValues(b[PAL_MAGENTA],arglist,i);
			break;
		case CYELLOW:
			XtSetValues(b[PAL_YELLOW],arglist,i);
			break;
		case CHUE:
			XtSetValues(b[PAL_HUE],arglist,i);
			break;
		case CSATURATION:
			XtSetValues(b[PAL_SATURATION],arglist,i);
			break;
		case CVALUE:
			XtSetValues(b[PAL_VALUE],arglist,i);
			break;
		default:
			break;
		} /* switch */
	
	comp_pen = (unsigned short) client_data;

	if (!(comp_pen & rgbcmyhsv)) { /* if not already drawn then draw it*/
		i=0;
	 	XtSetArg(arglist[i],XtNset,True);	i++;
		switch (comp_pen) {
			case CRED:
				DrawComponent(CRED);
				XtSetValues(b[PAL_PRED],arglist,i);
				break;
			case CGREEN:
				DrawComponent(CGREEN);
				XtSetValues(b[PAL_PGREEN],arglist,i);
				break;
			case CBLUE:
				DrawComponent(CBLUE);
				XtSetValues(b[PAL_PBLUE],arglist,i);
				break;
			case CCYAN:
				DrawComponent(CCYAN);
				XtSetValues(b[PAL_PCYAN],arglist,i);
				break;
			case CMAGENTA:
				DrawComponent(CMAGENTA);
				XtSetValues(b[PAL_PMAGENTA],arglist,i);
				break;
			case CYELLOW:
				DrawComponent(CYELLOW);
				XtSetValues(b[PAL_PYELLOW],arglist,i);
				break;
			case CHUE:
				DrawComponent(CHUE);
				XtSetValues(b[PAL_PHUE],arglist,i);
				break;
			case CSATURATION:
				DrawComponent(CSATURATION);
				XtSetValues(b[PAL_PSATURATION],arglist,i);
				break;
			case CVALUE:
				DrawComponent(CVALUE);
				XtSetValues(b[PAL_PVALUE],arglist,i);
				break;
			default:
				break;
			} /* switch */
		} /* if */
} /* CBSetPen() */



static void CBUnSetPen(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	comp_pen = 0;
}



static void SetPenColor(dpy,wingc,color)
/*
 * Set pen color for drawing.
 * Using an 8-bit palette, 0 <= color <= 255
 */
Display		*dpy;
GC		wingc;
unsigned short color;
{
XGCValues	gcval;
        gcval.foreground = color;
        XChangeGC(dpy,wingc, GCForeground, &gcval);
}

		
PlotComp(w,x,y,comp)
Widget w;
int x,y;
int comp;
{
Display	*dpy; 
Window	win;

#ifdef UNICOS
/* Unicos 5.0 C compiler (Cray PCC Version CRAY2 4.1.7 08/23/89)
 * is a little retarted. it gives an internal compiler error when it 
 * gets here.... and I'm tired of trying to work around it, so in 
 * this version, the following just won't work.....oh well.. */

/**/printf("Sorry, this doesn't work under UNICOS 5.0\n");
/**/printf("problem with Cray PCC Version CRAY2 4.1.7 08/23/89 \n");
}
#else
	dpy = XtDisplay(w);
	win = XtWindow(w);

	switch(comp) {
		case CRED:	
			rccells[x].red = (int) ((((float) y)/
					((float) PALCOMPHEIGHT)) * 65535.0);
			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,rpoints[x].y);
			rpoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,rpoints[x].y);
			break;
		case CGREEN:	
			rccells[x].green= (int) ((((float)y)/
					((float) PALCOMPHEIGHT)) * 65535.0);
			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,gpoints[x].y);
			gpoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,gpoints[x].y);
			break;
		case CBLUE:	
			rccells[x].blue= (int) ((((float) y)/
					((float) PALCOMPHEIGHT)) * 65535.0);
			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,bpoints[x].y);
			bpoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,bpoints[x].y);
			break;
		case CCYAN:	
			rccells[x].red = (int) (65535.0 - ((((float) y)/
						((float) PALCOMPHEIGHT))
						 * 65535.0));
			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,cpoints[x].y);
			cpoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,cpoints[x].y);
			break;
		case CMAGENTA:	
			rccells[x].green= (int) (65535.0 - ((((float) y)/
						((float) PALCOMPHEIGHT))
						 * 65535.0));
			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,mpoints[x].y);
			mpoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,mpoints[x].y);
			break;
		case CYELLOW:	
			rccells[x].blue = (int) (65535.0 - ((((float) y)/
						((float) PALCOMPHEIGHT))
						 * 65535.0));
			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,ypoints[x].y);
			ypoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,ypoints[x].y);
			break;
		case CHUE:{
			int max,min;
			double	hue;
			double	saturation;
			int	p1,p2,p3;
			double	frac;
			
			max = MAX3(rccells[x].red,rccells[x].green,
				rccells[x].blue);
			min = MIN3(rccells[x].red,rccells[x].green,
                                rccells[x].blue);

/*			if (max != 0)
				saturation = ((double)(max - min)) 
						/ (double) max;
			else
				saturation = 0;
*/
			saturation = (max)?(((double)(max - min))/(double)max)
					:0;

			hue = ((double) y) 
				* (6.0 / ((double) PALCOMPHEIGHT));

			frac = hue - ((double) ((int) hue));

			p1 = (int) (((double)max) * (1.0 - saturation));
			p2 = (int) (((double)max) * (1.0 - 
				(saturation * frac)));
			p3 = (int) (((double)max) * (1.0 - 
				(saturation * (1.0 - frac))));
			
			switch ( (int) hue) {
				case 0 : 
					rccells[x].red = max;
					rccells[x].green = p3;
					rccells[x].blue = p1;
					break;
				case 1 : 
					rccells[x].red = p2;
					rccells[x].green = max;
					rccells[x].blue = p1;
					break;
				case 2:
					rccells[x].red = p1;
					rccells[x].green = max;
					rccells[x].blue = p3;
					break;
				case 3:
					rccells[x].red = p1;
					rccells[x].green = p2;
					rccells[x].blue = max;
					break;
				case 4:
					rccells[x].red = p3;
					rccells[x].green = p1;
					rccells[x].blue = max;
					break;
				case 5:
					rccells[x].red = max;
					rccells[x].green = p1;
					rccells[x].blue = p2;
					break;
				} /* switch*/
		

			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,hpoints[x].y);
			hpoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,hpoints[x].y);
			
			}
			break;
		case CSATURATION:
			{
			int max;
			int r,g,b;
			double s;

			r = rccells[x].red;
			g = rccells[x].green;
			b = rccells[x].blue;

			max = MAX3(r,g,b);

			s = ((double)y) / ((double) PALCOMPHEIGHT);
			if (max == r) {
/*				rccells[x].green = max - (int) (((double)g)*s);
				rccells[x].blue= max - (int) (((double)b)*s);
*/
				rccells[x].green = ((double)(max - g)) * s
						+ SIGN(((double)g)/65535.0)*g;
				rccells[x].blue= ((double)(max - b)) * s
						+ SIGN(((double)b)/65535.0)*b;
				}
			if (max == g) {
/*				rccells[x].red= max - (int) (((double)r)*s);
				rccells[x].blue= max - (int) (((double)b)*s);
*/
				rccells[x].red= ((double)(max - r)) * s
						+ SIGN(((double)r)/65535.0)*r;
				rccells[x].blue= ((double)(max - b)) * s
						+ SIGN(((double)b)/65535.0)*b;
				}
			if (max == b) {
				rccells[x].red= ((double)(max - r)) * s
						+ SIGN(((double)r)/65535.0)*r;
				rccells[x].green = ((double)(max - g)) * s
						+ SIGN(((double)g)/65535.0)*g;
/*				rccells[x].red= max - (int) (((double)r)*s);
				rccells[x].green= max - (int) (((double)g)*s);
*/
				}
			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,spoints[x].y);
			spoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,spoints[x].y);
			
			}
			break;
		case CVALUE: {
			int max;
			double ratio;
			double yval;

			yval = ((double) y) * 
					(65535.0 / ((double)PALCOMPHEIGHT));
			max = MAX3(rccells[x].red,rccells[x].green,
				rccells[x].blue);
			if (max != 0) {
				ratio =((double) rccells[x].red)/(double) max;
				rccells[x].red = (int) (ratio *  yval);
				ratio =((double) rccells[x].green)/(double)max;
				rccells[x].green = (int) (ratio *  yval);
				ratio =((double) rccells[x].blue)/(double) max;
				rccells[x].blue = (int) (ratio *  yval);
				}
			else {
				rccells[x].red = rccells[x].blue = 
					rccells[x].green = yval;
				}
			SetPenColor(dpy,pal_comp_gc,white);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,vpoints[x].y);
			vpoints[x].y = PALCOMPHEIGHT - y;
			SetPenColor(dpy,pal_comp_gc,comp_pen_dcolor);
			XDrawPoint(dpy,win,pal_comp_gc,
					x,vpoints[x].y);
			}
			break;

		default:
			break;
		}
		
	return;
} /* PlotComp() */
#endif



static void CBChangeComponent(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
Window root, child;
int     root_x,root_y;
int     win_x,win_y;
unsigned int    keys_buttons;
int	x;
float	y;
float	yinc,ydiff;
Display	*dpy; 
Window	win;

#ifdef UNICOS
/* problem compiling routine PlotComp()... see it for more details */
/**/printf("Sorry, this doesn't work under UNICOS 5.0\n");
/**/printf("problem with Cray PCC Version CRAY2 4.1.7 08/23/89 \n");
}
#else
	if (!comp_pen)		/* No component selected */
		return;

	dpy = XtDisplay(w);
	win = XtWindow(w);
        if (!XQueryPointer(dpy,win,&root,&child,
                &root_x,&root_y,&win_x,&win_y,&keys_buttons)) 
		return;

	win_x = (win_x > 255)? 255 : (win_x <0)? 0 : win_x;
	win_y = (win_y > PALCOMPHEIGHT)? PALCOMPHEIGHT: 
					(win_y < 0)? 0 : win_y;

	win_y = PALCOMPHEIGHT - win_y; /* 4th quadrant -> 1st quadrant */

	if (comp_last_x == 0)
		PlotComp(pal_comp,win_x,win_y,comp_pen);
	else {
		ydiff = (comp_last_x > win_x)?(comp_last_y - win_y):
				(win_y - comp_last_y);

		if (comp_last_x > win_x) {
			yinc = (float) ydiff /((comp_last_x == win_x)?1.0:
					((float)(comp_last_x - win_x)));
			for (x = win_x,y=win_y; x < comp_last_x; x++,y=y+yinc)
				PlotComp(pal_comp,x,(int) y,comp_pen);
			}
		else{
			yinc = (float) ydiff / ((comp_last_x == win_x)?1.0:
					((float)( win_x - comp_last_x)));
			for (x = comp_last_x, y=comp_last_y; x < win_x; x++,y=y+yinc)
				PlotComp(pal_comp,x,(int) y,comp_pen);
			}
		}
	comp_last_x = win_x;
	comp_last_y = win_y;
	StoreColors(rccells,FALSE,fcmap);
} /* CBChangeComponent() */
#endif
			


static void CBPrepChangeComp(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	comp_last_x = 0;
	comp_last_y = 0;
	comp_pen_dcolor = ChooseColor(rccells,comp_pen);
	SaveForUndo(rccells);
	CBChangeComponent(w,client_data,call_data);
}

static void CBEndChangeComp(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (comp_last_x || comp_last_y)
		RedrawComponent();
	comp_last_x = 0;
	comp_last_y = 0;
}

static int ChooseColor(ccells,whichone)
/* pick the best entry in the palette to match color */
XColor	ccells[];	/* the palette */
int	whichone;	/* which color to find in palette */
{
int best;
int best_cmpval;
int cmpval;
int x;
int r,b,g;

	best = black;
	best_cmpval = -1 - (1 >> 18);
	switch (whichone) {
	case CRED:	/* find purest red */
		for(x=0; x < 256 ; x++) {
			g = ccells[x].green;
			b = ccells[x].blue;
			cmpval = ccells[x].red - g - b - abs(g-b);
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	case CGREEN:	/* find purest green */
		for(x=0; x < 256 ; x++) {
			r = ccells[x].red;
			b = ccells[x].blue;
			cmpval = ccells[x].green - r - b - abs(r-b);
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	case CBLUE:	/* find purest blue */
		for(x=0; x < 256 ; x++) {
			r = ccells[x].red;
			g = ccells[x].green;
			cmpval = ccells[x].blue - r - g - abs(r-g);
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	case CCYAN:	/* find purest cyan */
		for(x=0; x < 256 ; x++) {
			cmpval = ccells[x].green + ccells[x].blue
						- ccells[x].red;
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	case CMAGENTA:	/* find purest magenta */
		for(x=0; x < 256 ; x++) {
			cmpval = ccells[x].red + ccells[x].blue
						- ccells[x].green;
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	case CYELLOW:	/* find purest yellow */
		for(x=0; x < 256 ; x++) {
			cmpval = ccells[x].red + ccells[x].green
						- ccells[x].blue;
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	case CHUE:	/* find purest orange */
		for(x=0; x < 256 ; x++) {
			r = ccells[x].red;
			b = ccells[x].blue;
			cmpval = r-b - abs(((r+b)>>1) - ccells[x].green);
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	case CSATURATION:	/* find purest purplish color?? */
		for(x=0; x < 256 ; x++) {
			r = ccells[x].red;
			g = ccells[x].green;
			cmpval = b-g - abs(((b+g)>>1) - ccells[x].red);
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	
	case CVALUE:	
	default:
		/* find purest black */
		for(x=0; x < 256 ; x++) {
			cmpval = b-r - abs(((b+r)>>1) - ccells[x].green);
			/*cmpval = 65535 - ((ccells[x].red + ccells[x].green
					+ccells[x].blue)/3);
			*/
			if (cmpval > best_cmpval) {
				best_cmpval = cmpval;
				best = x;
				}
			}
		break;
	} /* switch */

	return(best);
}

static void CBPalSave(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{

	WriteSEQPalette(Get_Save_Pal(),rccells);
}



static void CBUndo(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
int	x;
XColor	real_undo[256];

	for(x=0;x < 256; x++) {
		rccells[x].pixel = x;
		cpccell(real_undo[x],uccells[x]);
		}

	SaveForUndo(rccells);

	for(x=0;x < 256; x++) {
		rccells[x].pixel = x;
		cpccell(rccells[x],real_undo[x]);
		}
	StoreColors(rccells,TRUE,fcmap);
}

static void SaveForUndo(rccells)
XColor	rccells[];
{
int	x;

	for(x=0;x < 256; x++) {
		uccells[x].pixel = x;
		cpccell(uccells[x],rccells[x]);
		}
}

static void CBSmooth(w,client_data,call_data)
Widget w;
caddr_t client_data;	/* contains a pointer to the component*/
caddr_t call_data;
{
int x;
int	comp_pen;	/* Drawing color; smooth this one */

	SaveForUndo(rccells);

	comp_pen = *( (int *) client_data);
	if (!comp_pen) {
		PrintMesg("Please select a component to manipulate first\n");
		return;
		}

	switch (comp_pen){
		case CRED:
		case CCYAN:
		    rccells[0].red = (rccells[255].red +rccells[1].red)/2;
		    for(x=1; x< 255; x++)
			rccells[x].red = 
				(rccells[x-1].red + rccells[x+1].red)/2;
		    rccells[255].red = (rccells[254].red +rccells[0].red)/2;
		    break;
		case CGREEN:
		case CMAGENTA:
		    rccells[0].green = 
				(rccells[255].green + rccells[1].green)/2;
		    for(x=1; x< 255; x++)
			rccells[x].green = 
				(rccells[x-1].green + rccells[x+1].green)/2;
		    rccells[255].green = 
				(rccells[254].green + rccells[0].green)/2;
		    break;
		case CBLUE:
		case CYELLOW:
		    rccells[0].blue = 
				(rccells[255].blue + rccells[1].blue)/2;
		    for(x=1; x< 255; x++)
			rccells[x].blue = 
				(rccells[x-1].blue + rccells[x+1].blue)/2;
		    rccells[255].blue = 
				(rccells[254].blue + rccells[0].blue)/2;
		    break;
		case CHUE:
		case CSATURATION:
		case CVALUE:
			/* ick, I'll do this some other time */
		    break;
		} /*switch */
		
	StoreColors(rccells,TRUE,fcmap);
} /* CBSmooth() */
