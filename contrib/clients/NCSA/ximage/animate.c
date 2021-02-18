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
#include <Xw/Valuator.h>
#include "animate.h"
#include "xstuff.h"
#include "getfile.h"

#define	PAL_WIDTH	30
#define PBOXHEIGHT	40
#define PBOXWIDTH	360
#define	ACBOXWIDTH	360
#define ACBOXHEIGHT	100

#define	ROUND(f)	((int)(f + 0.5))

#ifdef SYSV
/*#include <sys/time.h>*/
struct timeval   st_delay;
#define usleep(x)   { \
        st_delay.tv_usec = (x % 1000000); \
        st_delay.tv_sec = (x / 1000000); \
        select(32, NULL, NULL, NULL, &st_delay); }
#endif

extern	Widget	toplevelform;		/* the top composite widget */
extern  XColor  origccells[];		/* currently defined in palette.c*/
extern  XColor  rccells[];		/* currently defined in palette.c*/
extern	void	SetColormap();	
extern	struct	ANIMATION *NewAnimation();
extern	void	DestroyAnimation();

static	char	frame[80];
static	char	skip[80];
static	XImage	*palXImage;	/* display palette with this */
static	char	*palImage;
static	struct	GetFile	animateFile;
static	Widget	toggled,togglem;
static	Visual	*myVis;
static	Display *myDpy;

void	AnimateCtrlBox();




int InitAnimation(dpy)
Display *dpy;
{
register int x;
register int y;

	myDpy = dpy;
	myVis = GetVisual(myDpy);

	if (!(palXImage = MakePalette(myDpy,PAL_WIDTH,TRUE))) {
		return(0); /* malloc probably failed */
		}

	return(1);
} /* InitAnimation() */



char *ComboFile(directory,file)    /*what?*/
char *directory;
char *file;
{
static char combo[512];

	sprintf(combo,"%s/%s",directory,file);
	return(combo); /* good till called again */
}


static void DidntGetANewAnimation(a)
struct ANIMATION *a;
{
	PrintMesg("Get new animation sequence cancelled\n");
}

CBinMemory(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
static  Arg setFalse[]={{XtNset,False}};
struct	ANIMATION *a;

	a = (struct ANIMATION *) client_data;
	a->inMemory = ! a->inMemory;
	if (a->inMemory) {
		PrintMesg("Animate from server memory\n");
                XtSetValues(toggled,setFalse,1);
		}
	else {
		PrintMesg("Animate from disk\n");
                XtSetValues(togglem,setFalse,1);
		}
	
}

CBinMemoryDone(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct	ANIMATION *a;
	a = (struct ANIMATION *) client_data;

	XtDestroyWidget(a->topQbox);
	AnimateCtrlBox(a);
}

static void GotANewAnimation(a)
struct ANIMATION *a;
{ /* initialize animation record */
char buff[256];

	a->name = animateFile.base;

	a->directory = animateFile.directory;
	a->files= animateFile.file;
	a->numFiles = animateFile.numFiles;

	a->xdim = animateFile.xdim;
	a->ydim = animateFile.ydim;

	a->isHDF = animateFile.isHDF;
	a->isFloat = animateFile.isFloat;
	a->frameNum = 1;
	a->skip = 0;
	a->status = A_STOP;

/**/	a->numFrames = a->numFiles;
	if (a->numFiles == 1)  {
		if (a->isHDF) {
			if (a->isFloat) 
			    a->numFrames = HowManySDSInHDF(
					ComboFile(a->directory,a->files[0]));
			else {
			    a->numFrames = HowManyImagesInHDF(
					ComboFile(a->directory,a->files[0]));
/*			    if(!(a->frameIndex = (int *) XICalloc(a->numFrames,
						sizeof(int)))) {
					PrintMesg("out of memory\n");
					return;
					}
			    if (!MakeHDFImageIndex(ComboFile(a->directory,
							a->files[0]),
						a->frameIndex,
						a->numFrames)) {
					PrintMesg("Can't read HDF file\n");
					return;
					}
*/
			    }
			}
		}
	
	if (a->numFrames < 2) {
		PrintMesg("Animation requires more than one frame\n");
		PrintMesg("Cancelling Animation\n");
		return;
		}
		
	sprintf(buff,"Animation sequence has %d images\n",a->numFrames);
	PrintMesg(buff);

	/* read in Palette */
/*	a->hasPalette = FALSE;*/

	a->hasPalette = TRUE;
/**/	a->palette = origccells;
	if (a->isHDF) {
	    if (ReadHDFHasPalette(ComboFile(a->directory,a->files[0])) > 0){
		    if (!(a->palette=(XColor *)XICalloc(256,sizeof(XColor)))){
			PrintMesg("Not enough memory for palette\n");
			return;
			}
		    ReadHDFPalette(ComboFile(a->directory,a->files[0]),
				a->palette);
		    a->hasPalette = TRUE;
		    }
		}
	a->inMemory = False;
	{
	Widget comp;
	int screen;

        screen = XDefaultScreen(myDpy);
	a->topQbox = MakeATopLevelOverride("X Image [Animation].inMemory?",
				toplevelform,
				(DisplayWidth(myDpy,screen)-200)/2,
				(DisplayHeight(myDpy,screen)-100)/2,
				200,100);
	comp = Make_Board("inMemory?",a->topQbox,0,0,200,200);
	togglem =  Make_Toggle("animate from X server memory",comp,5,20,
			CBinMemory,a,CBinMemory,a,False,a->inMemory);
	toggled =  Make_Toggle("animate from disk",comp,5,50,
			CBinMemory,a,CBinMemory,a,False,!a->inMemory);
	(void) MakeButton("     Ok     ",comp,5,70,CBinMemoryDone,a,0,0);
	}
	
} /* GotANewAnimation() */


CBNewAnimate(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct	ANIMATION *a;

	if (!(a = NewAnimation())){
		PrintMesg("Out of memory\n");
		return;
		}
	animateFile.yesfile = GotANewAnimation;
	animateFile.nofile = DidntGetANewAnimation;
	animateFile.type = FTYPE_IMAGE;
	animateFile.param = (char *) a;
	PrintMesg("Please select animation file(s)\n");
	GetAFile(&animateFile);
} /* CBNewAnimate() */

void CBDoneAnimateCtrlBox(w,client_data,call_data)
Widget	w;
caddr_t	client_data;
caddr_t call_data;
{
int x;
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;

	if (a->status != A_STOP)
		return;	/* animation running, can't quit now */

	if (a->inMemory) {
		PrintMesg("Freeing server memory\n");
		for (x = 0; x < a->numFrames; x++)
			XFreePixmap(myDpy,a->pixmaps[x]);
		}
	XIFree(a->pixmaps);
	XIFree(a->raster);

	if (a->ctrl_showing) {
/*#ifdef  NON_ICCCM_WINDOW_MANAGER*/
	        CBUnInstallImageCMap(a->topLevel,a->cmap,call_data);
/*#endif*/
		XtDestroyWidget(a->topLevel);
		}

	DestroyAnimation(a);

	return;
} /*CBDoneAnimateCtrlBox() */


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


static void CBLockPalette(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;

        a->lockcmap = ! a->lockcmap;
        if (a->lockcmap) {
                StoreColors(rccells,FALSE,a->cmap);
                XtAddEventHandler(a->topLevel,EnterWindowMask,0,
                        CBUpdateCMap,a->cmap);
		}
        else {
                if (a->hasPalette) 
                        StoreColors(a->palette,FALSE,a->cmap);
                else
                        StoreColors(origccells,FALSE,a->cmap);

                XtRemoveEventHandler(a->topLevel,EnterWindowMask,
			0,CBUpdateCMap,a->cmap);
		}
} /* CBLockPalette() */


static int GetNextImage(a,frameNum)
/* prepare and put next image into a->raster or a->expRaster*/
/* takes care of source: HDF or SEQ, multiple or single file */
/* return 0 on error */

struct ANIMATION *a;
int frameNum; /* start at 1 (1..infinity) */
{
register int x;
register int y;
register int z;
int     planes;
int     nbp;    /* number of bytes per pixel */
unsigned char *p;

	if (! a->isHDF) {
		/* multiple SEQ files */

		if (! ReadSEQAnimation(ComboFile(a->directory,
				a->files[frameNum-1]),
			a->raster,a->xdim,a->ydim))
			return(0);
		}
	else {
		/* it's an HDF file */

		if (a->numFiles == 1) {
			/* animate from one HDF file */

			if (a->isFloat) {
				/*read in float
				convert it
				*/
				return(0); /*for now */
				}
			else { 
				if (!ReadHDFAnimationImage(ComboFile(
						a->directory,a->files[0]),
/*				   a->raster,a->frameIndex[frameNum-1]))*/
				   a->raster,frameNum))
					return(0);
				}
			}
		else { /* animate from multiple HDF files */
			if (a->isFloat) {
				/*
				read in float
				convert it
				*/
				return(0); /*for now */
				}
			else {
				if (!ReadHDFAnimationImage(
				    ComboFile(a->directory,
					a->files[frameNum-1]),a->raster,
				    /*a->frameIndex[0]))*/
				    1))
					return(0);
				}
			}
		} /* else: it's an HDF */


	if (a->expRaster) {
	        planes = DisplayPlanes(myDpy,DefaultScreen(myDpy));
	
		nbp = (planes > 7) ? (planes / 8 + ((planes%8)?1:0)) :1;

	        if (nbp >= 3)
			nbp = 4;

		p = a->expRaster;
		for(y = 0; y < a->ydim; y++)
			for(x = 0; x < a->xdim; x++) {
				for (z = 1; z < nbp; z++){
					(*p) = (unsigned char) 0;
					p++;
					}
				(*p) = a->raster[y*a->xdim+x];
				p++;
				}
		}


	return(1);/* ok */

} /* GetNextImage() */


static int DisplayImage(a,frameNum)
struct ANIMATION *a;
int frameNum;
{
Arg	arglist[1];
char	buff[256];

	if (! a->inMemory) {
		if (!GetNextImage(a,frameNum)) {
			if (a->numFiles == 1) 
			    sprintf(buff,"Couldn't get %dth image out of %s\n",
				frameNum,ComboFile(a->directory,a->files[0]));
			else 
			    sprintf(buff,"Couldn't get image from file %s\n",
				ComboFile(a->directory,a->files[frameNum-1]));
			PrintMesg(buff);
			return(0);
			}

		XPutImage(myDpy,XtWindow(a->awin),
				a->awinGC,a->image,
				0,0,0,0,a->xdim,a->ydim);
		}
	else {

		XSetWindowBackgroundPixmap(myDpy,
					XtWindow(a->awin),
					a->pixmaps[frameNum-1]);
		XClearArea(myDpy,XtWindow(a->awin),
				0,0,a->xdim,a->ydim,False);
		XSync(myDpy,False);
		}
	
	/* Set dialog */
	sprintf(a->dFrameString,"%d",a->frameNum);
	gr_DialogSetValue(a->dFrame,a->dFrameString);

	/* Set valuator */
	XtSetArg(arglist[0],XtNsliderOrigin,frameNum);
	XtSetValues(a->slidebar,arglist,1);

	return(TRUE);
}

static int ReadThemAll(a)
/* return 1 on success, 0 on failure */
struct ANIMATION *a;
{
register int x,j;
char buff[80];
Arg	arglist[1];
float	fourth;
int	screen;

	screen = XDefaultScreen(myDpy);

	sprintf(buff,"Reading in %d frames into server memory\n",a->numFrames);
	PrintMesg(buff);

	fourth = ((float) a->numFrames) / 4.0;
	for (j = 0; j < 4; j++) {
	    for (x = ROUND(((float)j)*fourth) 
		     ;x < ROUND((((float)j)+1.0)*fourth); x++ ) {

		if (! GetNextImage(a,x+1))
			return(0);

		a->pixmaps[x] = XCreatePixmap(myDpy,
					XtWindow(a->awin),
					a->xdim,a->ydim,
					DisplayPlanes(myDpy,screen));

/*		if (XCreatePixmap failed, BadAlloc) {
			int y;
			PrintMesg("Server out of memory \n");
			for (y = 0; y < x; y++) 
				XFreePixmap(XtDisplay->topLevel,a->pixmaps[y]);
			return(0);
			}
*/
				

		XPutImage(myDpy,a->pixmaps[x],a->awinGC,a->image,
					0,0,0,0,a->xdim,a->ydim);

/*		XPutImage(myDpy,XtWindow(a->awin),
				a->awinGC,a->image,
				0,0,0,0,a->xdim,a->ydim);
*/
		XSetWindowBackgroundPixmap(myDpy,
					XtWindow(a->awin),
					a->pixmaps[x]);
		XClearArea(myDpy,XtWindow(a->awin),
				0,0,a->xdim,a->ydim,False);
		XSync(myDpy,False);

/* doesn't work 
		XtSetArg(arglist[0],XtNsliderOrigin,x);
		XtSetValues(a->slidebar,arglist,1);
*/

		}
	    switch (j) {
		case 0: PrintMesg("Done loading 25% of animation images\n");
			break;
		case 1: PrintMesg("Done loading 50% of animation images\n");
			break;
		case 2: PrintMesg("Done loading 75% of animation images\n");
			break;
		case 3: PrintMesg("Done loading animation images\n");
			break;
		};
	}
	return(1);
} /* CBReadThemAll() */


static void CBSingleForward(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;

	a->frameNum = a->frameNum + 1 + a->skip;

	if ((a->frameNum > a->numFrames) || (a->frameNum < 1)) {
		if (a->repeat || (a->status == A_STOP))
			a->frameNum = 1;
		else {
			a->frameNum = a->frameNum - 1 - a->skip;
			a->status = A_STOP;
			return;
			}
		}

	DisplayImage(a,a->frameNum);

	return;

} /* CBSingleForward */



static void CBSingleReverse(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;

	a->frameNum = a->frameNum - 1 - a->skip;

	if ((a->frameNum > a->numFrames) || (a->frameNum < 1))
		if (a->repeat || (a->status == A_STOP))
			a->frameNum = a->numFrames;
		else {
			a->frameNum = a->frameNum + 1 + a->skip;
			a->status = A_STOP;
			return;
			}

	DisplayImage(a,a->frameNum);


	return;
} /* CBSingleReverse() */



static void CBStop(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
	((struct ANIMATION *)client_data)->status = A_STOP;
}

static void CBContinuousForward(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
XEvent event_return;
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;

	a->status = A_CFOR;
	while(a->status == A_CFOR) {

		CBSingleForward(w,client_data,call_data);

		while(XtPending()) {
			XtNextEvent(&event_return);
			XtDispatchEvent(&event_return);
			}
		if (a->delay)
			usleep((a->delay)*100000); /* a->delay in 1/10 sec*/
		}
} /* CBContinuousForward() */


static void CBContinuousReverse(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
XEvent event_return;
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;

	a->status = A_CREV;
	while(a->status == A_CREV) {

		CBSingleReverse(w,client_data,call_data);

		while(XtPending()) {
			XtNextEvent(&event_return);
			XtDispatchEvent(&event_return);
			}
		if (a->delay)
			usleep((a->delay)*100000); /* a->delay in 1/10 sec*/
		}
} /* CBContinuousReverse() */


static void CBSetFrame(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct ANIMATION *a;
int	newFrame;
char	buff[80];

	a = (struct ANIMATION *) client_data;

	newFrame = atoi(XtDialogGetValueString(a->dFrame));
	if ((newFrame > a->numFrames) || ( newFrame < 1)) {
		sprintf(buff,"Frame number out of Range (1 - %d)\n",
					a->numFrames);
		PrintMesg(buff);
		}
	else {
		a->frameNum = newFrame;
		DisplayImage(a,a->frameNum);
		}

	sprintf(a->dFrameString,"%d",a->frameNum);
	gr_DialogSetValue(a->dFrame,a->dFrameString);


} /* CBSetFrame */

static void CBValuatorRelease(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
int	newFrame;
char	buff[80];
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;
	newFrame = (int) call_data;

	if ((newFrame > a->numFrames) || ( newFrame < 1)) {
		/* this should never happen */
		sprintf(buff,"Frame number out of Range (1 - %d)\n",
					a->numFrames);
		PrintMesg(buff);
		}
	else {
		a->frameNum = newFrame;
		DisplayImage(a,a->frameNum);
		}

	sprintf(a->dFrameString,"%d",a->frameNum);
	gr_DialogSetValue(a->dFrame,a->dFrameString);

}

static void CBSetSkip(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct ANIMATION *a;
int	newSkip;
char	buff[80];

	a = (struct ANIMATION *) client_data;

	newSkip = atoi(XtDialogGetValueString(a->dSkip));
	if ((newSkip > a->numFrames) || ( newSkip < 0)) {
		sprintf(buff,"Number out of Range (0 - %d)\n",a->numFrames);
		PrintMesg(buff);
		}
	else {
		a->skip = newSkip;
		}
	sprintf(a->dSkipString,"%d",a->skip);
	gr_DialogSetValue(a->dSkip,a->dSkipString);
} /* CBSetSkip */

static void CBSetDelay(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
char	buff[80];
struct ANIMATION *a;
int	newDelay;

	a = (struct ANIMATION *) client_data;

	newDelay= atoi(XtDialogGetValueString(a->dDelay));
	if ((newDelay > 1000) || ( newDelay < 0)) 
		PrintMesg("Delay: That number just won't do...\n"); 
	else 

		a->delay = newDelay;

	sprintf(a->dDelayString,"%d",a->delay);
	gr_DialogSetValue(a->dDelay,a->dDelayString);

	if (a->delay == 10)/* I know this is silly.....adds to the esthetics */
		PrintMesg("Setting delay to 1 second\n");
	else {
		sprintf(buff,"Setting delay to %.1f seconds\n",
			((float)a->delay)/10.0);
		PrintMesg(buff);
		}
}

static void CBExpose(w,client_data,call_data)
/* Animation window exposed, redraw image */
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;

	if (a->frameNum == 0){
		DisplayImage(a,1);
		a->frameNum = 1;
		}
	else
		DisplayImage(a,a->frameNum);

} /* CBExpose() */

static void CBRepeat(w,client_data,call_data)
Widget  w;
caddr_t client_data;
caddr_t call_data;
{
struct ANIMATION *a;
	a = (struct ANIMATION *) client_data;

	a->repeat = ! a->repeat;
}


void AnimateCtrlBox(a)
struct ANIMATION	*a;
{
int	xpos,ypos;
int	width,height;
Widget	box1,box3,box4;
Widget	acbox;	/*animation control box */
Widget	pbox;	/*palette box */
Widget	box5,box6,box7;
XGCValues       gcvalues;
char	buff[256];


	PlaceWindow(&xpos,&ypos);
	width = a->xdim + 10;
	width = (width < ACBOXWIDTH+10)? ACBOXWIDTH +10 : width;
	width = (width < PBOXWIDTH+10)? PBOXWIDTH+10: width;
	height = a->ydim + 40 + ACBOXHEIGHT+ PBOXHEIGHT;

	if (!(a->pixmaps = (Pixmap *) 
			XICalloc(a->numFrames,sizeof(Pixmap)))) {
		PrintMesg("Can't Allocate any memory\n");
		return;
		}

	if(!(a->raster = (unsigned char *)
			XIMalloc( sizeof(char) * a->xdim * a->ydim ))){
		PrintMesg("Can't allocate memory for animation\n");
		/**** should clean up stuff here (like trash a)*/
		return;	
		}

	{
	int planes;
	int	nbp; /* number of bits per pixel */


        planes = DisplayPlanes(myDpy,DefaultScreen(myDpy));
	
	if (planes > 8){
       	    nbp = (planes > 7) ? (planes / 8 + ((planes%8)?1:0)) :1;

	   if(!(a->expRaster=(unsigned char *) XIMalloc(a->xdim*a->ydim*nbp))){
		PrintMesg("Couldn't allocate enough memory\n");
		return;
		}
	    a->image = XCreateImage(myDpy,myVis,planes,ZPixmap,0,
					a->expRaster,a->xdim,a->ydim,
					8*nbp,a->xdim*nbp);
	    a->isExpanded = TRUE;
	    }
	else {
	    a->image = XCreateImage(myDpy, myVis, 8, ZPixmap, 0, 
				a->raster, a->xdim, a->ydim, 8, a->xdim);
	    a->isExpanded = FALSE;
	    }
	}

	a->topLevel = MakeATopLevel("X Image [Animate]",toplevelform,
				xpos,ypos,width,height);
	a->lockcmap = FALSE;
	if (myVis->class == PseudoColor) {
		a->cmap = XCreateColormap(myDpy,XtWindow(a->topLevel),
			myVis,AllocAll);
			}
	else {
		a->cmap = XCreateColormap(myDpy,XtWindow(a->topLevel),
			myVis,AllocNone);
			}
        SetColormap(a->topLevel,a->cmap);

	Set_Pointer(a->topLevel,XC_gumby);

	a->composite = Make_Board(a->name,a->topLevel,0,0,
				width,height);
	
	box1 = Make_ButtonBox(a->composite,5,5,width-50,25);
        (void) Make_Label(a->name,box1,2,2,width-55);

	(void) MakeButton("Done",a->composite,width-40,5,
				CBDoneAnimateCtrlBox,a,0,0);

	a->awin = Make_Board("Animate widget",a->composite,
				(width - a->xdim)/2,35,
				a->xdim,a->ydim);

		
				
		

	a->awinGC = XtGetGC(a->awin,NULL,&gcvalues);

	pbox = Make_ButtonBox(a->composite,(width-PBOXWIDTH)/2,
				height - ACBOXHEIGHT- PBOXHEIGHT,
                                PBOXWIDTH,PAL_WIDTH +5);


        a->pal_win = MakeRasterImage("palette",pbox,
                                2,2,
                                palXImage,NULL,NULL);

	(void) Make_Toggle("Lock Palette",pbox,265,10,
			CBLockPalette,a,CBLockPalette,a,True,False);


	a->frameNum = 1;

	acbox = Make_ButtonBox(a->composite,
				(width-ACBOXWIDTH)/2,
				height-ACBOXHEIGHT,
                                ACBOXWIDTH,ACBOXHEIGHT-10);

	a->repeat = FALSE;
	Make_Toggle("Repeat",acbox,5,70,CBRepeat,a,CBRepeat,a,False,False);
#ifdef UNDER_DEVELOPMENT
	Make_Toggle("Reverse",acbox,70,70,NULL,NULL,NULL,NULL,False,False);
#endif

	box4 = Make_ButtonBox(acbox,2,2,155,60);

	a->bcrev = MakeButton("<<",box4,5,5,CBContinuousReverse,a,0,0);
	a->bsrev = MakeButton("< ",box4,30,5,CBSingleReverse,a,0,0);
	a->bstop = MakeButton("<STOP>",box4,55,5,CBStop,a,0,0);
	a->bsfor = MakeButton(" >",box4,105,5,CBSingleForward,a,0,0);
	a->bcfor = MakeButton(">>",box4,130,5,CBContinuousForward,a,0,0);

	if (a->inMemory)
		a->slidebar =MakeValuator("valuator",box4,5,30,145,25,FALSE,1,
				a->numFrames+1,a->frameNum,1,
				CBValuatorRelease,a,
				CBValuatorRelease,a,
				CBValuatorRelease,a);
	else
		a->slidebar =MakeValuator("valuator",box4,5,30,145,25,FALSE,1,
				a->numFrames+1,a->frameNum,1,
				NULL,NULL,
				CBValuatorRelease,a,
				CBValuatorRelease,a);

        Set_Pointer(a->slidebar,XC_sb_h_double_arrow);

	sprintf(a->dFrameString,"1");
	box5 = Make_ButtonBox(acbox,160,2,62,ACBOXHEIGHT-15);
	a->dFrame = Make_Dialog("name",box5,"Frame:",
			a->dFrameString,DSTRINGSIZE,2,2,56,56);
	MakeButton("  Set   ",box5,2,65,CBSetFrame,a,0,0);
	a->frameNum = 1;
	a->delay = 0;

	sprintf(a->dSkipString,"%d",a->skip);
	box6 = Make_ButtonBox(acbox,227,2,62,ACBOXHEIGHT-15);
	a->dSkip = Make_Dialog("name",box6,"Skip:",a->dSkipString,
				DSTRINGSIZE,2,2,56,56);
	MakeButton("  Set   ",box6,2,65,CBSetSkip,a,0,0);

	sprintf(a->dDelayString,"%d",a->delay);
	box6 = Make_ButtonBox(acbox,293,2,62,ACBOXHEIGHT-15);
	a->dDelay= Make_Dialog("name",box6,"Delay:",a->dDelayString,
				DSTRINGSIZE,2,2,56,56);
	MakeButton("  Set   ",box6,2,65,CBSetDelay,a,0,0);


        XtAddEventHandler(a->awin,ExposureMask,0,CBExpose,a);

	XSync(myDpy,False);	 /* a->awin must be created before cont. */


        Set_Pointer(a->awin,XC_left_ptr);

	if (a->hasPalette) {
		StoreColors(a->palette,FALSE,a->cmap);
                }
        else
                StoreColors(origccells,FALSE,a->cmap);


	if(!(a->frameIndex = (int *) XICalloc(a->numFrames,sizeof(int)))) {
		PrintMesg("out of memory\n");
		return;
		}
/*	if (!MakeHDFImageIndex(ComboFile(a->directory,a->files[0]),
			a->frameIndex,a->numFrames)) {
		PrintMesg("Can't read HDF file\n");
		return;
		}
*/

    sprintf(buff,"Dimensions of animation sequence: %dx%d\n",a->xdim,a->ydim);
	PrintMesg(buff);

	if (a->inMemory) {
		if (! ReadThemAll(a)) {
			CBDoneAnimateCtrlBox(a->awin,a,NULL);
			return;
			}
		}
	a->ctrl_showing = TRUE;

} /* AnimateCtrlBox() */

