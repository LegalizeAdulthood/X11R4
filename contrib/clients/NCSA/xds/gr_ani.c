/*
 * National Center for SuperComputing Applications, University of Illinois
 *
 * This NCSA software product is public domain software.  Permission
 * is hereby granted to do whatever you like with it. Should you wish
 * to make a contribution towards the production of this software, please
 * send us your comments about your experience with the software,  why
 * you liked or disliked it, how you use it, and most importantly, how it
 * helps your work. We will receive your comments at softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Eng-Whatt Toh, National Center for Supercomputing Applications
 *         ewtoh@ncsa.uiuc.edu
 */

/*
 *	File:		gr_ani.c
 *	Contents:	Animation level routines for graphics module
 */

#include "gr_com.h"

#define	GR_ANIDELAYMAX	2000000
#define	GR_ANIDELAYGRD	10000

XEvent	report;

#ifdef SYSV
struct timeval   st_delay;
#define usleep(x)   { \
	st_delay.tv_usec = (x % 1000000); \
	st_delay.tv_sec = (x / 1000000); \
	select(32, NULL, NULL, NULL, &st_delay); }
#else
#ifdef UNICOS
struct timeval   st_delay;
#define usleep(x)   { \
	st_delay.tv_usec = (x % 1000000); \
	st_delay.tv_sec = (x / 1000000); \
	select(32, NULL, NULL, NULL, &st_delay); }
#endif
#endif

/*
 *	Delay and polling
 */
void
gr_AniEvent(delay)
long delay;
{
	int	i;
	
	if (delay > 0)
		usleep(delay);

	while (XtPending() == TRUE)
	{
		XtNextEvent(&report);
		XtDispatchEvent(&report);
	}
}


/*
 *	Draw a Pixmap
 */
void
gr_AniDraw(display,window,pix)
Display *display;
Window  window;
Pixmap  pix;
{
	XSetWindowBackgroundPixmap(display,window,pix);
	XClearWindow(display,window);
	XSync(display,FALSE);
}


/*
 *	Draw an XImage
 */
void
gr_AniDrawImage(display,window,aniWin,i)
Display *display;
Window  window;
A_AniWind_t	*aniWin;
int	i;
{
	XPutImage(display,window,aniWin->imageWinGC,aniWin->ximage[i],0,0,0,0,
		aniWin->imagexsize[i],aniWin->imageysize[i]);
	XSync(display,FALSE);
}


/*
 *	Draw an XImage
 */
void
gr_AniDrawLoad(display,window,aniWin,i)
Display *display;
Window  window;
A_AniWind_t	*aniWin;
int	i;
{
	int ispal;

    if (td_HdfgetRaster(aniWin->pathname,
		aniWin->data[0],gr_color.palette,
        &(aniWin->imagexsize[0]),&(aniWin->imageysize[0]),&ispal,i) != NULL)
    {
    	if (aniWin->usePixmap == TRUE)
    	{
		XFreePixmap(display,aniWin->image[0]);
     	aniWin->image[0] = gr_PixmapCreate(aniWin->imageWin,aniWin->imageWinGC,
            aniWin->imagexsize[0],aniWin->imageysize[0],aniWin->data[0]);
     	gr_AniDraw(display,window,aniWin->image[0]);
    	}
    	else
    	{
     	aniWin->ximage[0] = gr_ImageCreate(aniWin->imageWin,
            aniWin->imagexsize[0],aniWin->imageysize[0],aniWin->data[0]);
     	gr_AniDrawImage(display,window,aniWin,0);
    	}
	}
}


/*
 *	Load frames from hdf file
 */
void
gr_AniLoad(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_FileWind_t	*fileWin=(A_FileWind_t *)client_data;
	char	pathname[STRNG160];
	char	*filename;
	int		ret,index,xsize,ysize,ispal,start,stop,numFrames;

	filename = gr_DialogGetValue(fileWin->fileDialog);
	sprintf(pathname,"%s/%s",td_getDirName(),filename);

	if ((ret = td_HdfgetRasDims(pathname,&xsize,&ysize,&ispal)) == -1)
	{
		sprintf(msg,"***ERROR: Cannot open file %s.\n",filename);
		gr_TextMsgOut(msg);
		gr_TextMsgOut
		("Are you sure it is a RASTER8 HDF file?\n");
	}
	else
	{
	    start = atoi(gr_DialogGetValue(fileWin->rasStartDialog));
	    stop = atoi(gr_DialogGetValue(fileWin->rasEndDialog));
		if (start < 0) start = 0; if (start >= ret) start=ret-1;
		if (stop < 0) stop = 0; if (stop >= ret) stop=ret-1;
		if (stop < start) stop = start;
		sprintf(msg,"%d",start);
		gr_DialogSetValue(fileWin->rasStartDialog,msg);
		sprintf(msg,"%d",stop);
		gr_DialogSetValue(fileWin->rasEndDialog,msg);
		numFrames = stop-start+1;
		sprintf(msg,
		"Loading %d frames from file %s...Please wait.\n",numFrames,filename);
		gr_TextMsgOut(msg);
		if (gr_topWin.numAniWins < MAX_ANILEV_WINDS)
		{
			gr_topWin.aniWin =
				gr_InitAniLevel2(gr_topWin.aniWin,filename,pathname,
					gr_topLevel,start,stop,numFrames,xsize,ysize,
					ispal,fileWin->usePixmap,fileWin->useDisk);
			gr_topWin.numAniWins++;
		}
		gr_CloseFileLevel(w,(caddr_t)fileWin,(caddr_t)NULL);
	}

    return;
}


/*
 *	Turn off Auto play
 */
void
gr_AniSetAutoOff(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->autoMode = AUTOOFF;

    return;
}


/*
 *	Set Auto play to Auto-repeat
 */
void
gr_AniSetAutoRep(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->autoMode = AUTOREPEAT;

    return;
}


/*
 *	Set Auto play to Auto-reverse
 */
void
gr_AniSetAutoRev(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->autoMode = AUTOREVERSE;

    return;
}


/*
 *	Jump to frame specified by the Frame dialog
 */
void
gr_AniFrameSet(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;
	int			frame;

	frame = atoi(gr_DialogGetValue(aniWin->frameDialog));

	if ((frame >= 0) && (frame < aniWin->numplanes))
	{
		aniWin->curplane = frame;

		gr_SliderSetValue(w,aniWin->curplane);
		if (aniWin->useDisk == TRUE)
		gr_AniDrawLoad(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
		else
		if (aniWin->usePixmap == TRUE)
		gr_AniDraw(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin->image[aniWin->curplane]);
		else
		gr_AniDrawImage(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
	}
	else
		gr_TextMsgOut("Error: Requested frame is out of limits!\n");

    return;
}


/*
 *	Set the frame skip increment specified by the Skip dialog
 */
void
gr_AniSkipSet(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;
	int			skip;

	skip = atoi(gr_DialogGetValue(aniWin->skipDialog));

	if ((skip >= 0) && (skip < aniWin->numplanes))
		aniWin->skip = skip;
	else
		gr_TextMsgOut("Error: Frame skip amount is out of limits!\n");

    return;
}


/*
 *	Increment the amount of delay
 */
void
gr_AniSpeedSlow(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	if (aniWin->speed < (long)(GR_ANIDELAYMAX-GR_ANIDELAYGRD))
		aniWin->speed+=(long)GR_ANIDELAYGRD;
	else
		aniWin->speed=(long)GR_ANIDELAYMAX;

	gr_SliderSetValue(aniWin->speedSlider,(int)aniWin->speed);

    return;
}


/*
 *	Decrement the amount of delay
 */
void
gr_AniSpeedFast(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	if (aniWin->speed > (long)GR_ANIDELAYGRD)
		aniWin->speed-=(long)GR_ANIDELAYGRD;
	else
		aniWin->speed=(long)0;

	gr_SliderSetValue(aniWin->speedSlider,(int)aniWin->speed);

    return;
}


/*
 *	Change the amount of delay when speed slider is selected
 */
void
gr_AniSpeedSliderSel(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->speed = (long)call_data;
	gr_SliderSetValue(w,(int)aniWin->speed);

	return;
}


/*
 *	Change the amount of delay when speed slider is moved
 */
void
gr_AniSpeedSliderMov(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->speed = (long)call_data;
	return;
}


/*
 *	Change the amount of delay when speed slider is released
 */
void
gr_AniSpeedSliderRel(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	return;
}


/*
 *	Change the frame when manual frame slider is selected
 */
void
gr_AniFrameSliderSel(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->curplane = (int)call_data;
	gr_SliderSetValue(w,aniWin->curplane);
	sprintf(msg,"%d",aniWin->curplane);
	gr_DialogSetValue(aniWin->frameDialog,msg);

	if (aniWin->useDisk == TRUE)
	gr_AniDrawLoad(XtDisplay(aniWin->imageWin),
		XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
	else
	if (aniWin->usePixmap == TRUE)
	gr_AniDraw(XtDisplay(aniWin->imageWin),
		XtWindow(aniWin->imageWin),aniWin->image[aniWin->curplane]);
	else
	gr_AniDrawImage(XtDisplay(aniWin->imageWin),
		XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);

    return;
}


/*
 *	Change the frame when manual frame slider is moved
 */
void
gr_AniFrameSliderMov(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->curplane = (int)call_data;
	sprintf(msg,"%d",aniWin->curplane);
	gr_DialogSetValue(aniWin->frameDialog,msg);

	/* Show frames only if animating from memory, else it will be too slow */
	if (aniWin->useDisk == FALSE)
	{
		if (aniWin->usePixmap == TRUE)
		gr_AniDraw(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin->image[aniWin->curplane]);
		else
		gr_AniDrawImage(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
	}

    return;
}


/*
 *	Change the frame when manual frame slider is released
 */
void
gr_AniFrameSliderRel(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->curplane = (int)call_data;
	sprintf(msg,"%d",aniWin->curplane);
	gr_DialogSetValue(aniWin->frameDialog,msg);

	/* Show last frame selected only if animating from disk */
	if (aniWin->useDisk == TRUE)
		gr_AniDrawLoad(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
    return;
}


/*
 *	Update display on expose event
 */
void
gr_AniExpose(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

    gr_ImageSetCMap(aniWin->shell);
	if (aniWin->useDisk == TRUE)
	gr_AniDrawLoad(XtDisplay(aniWin->imageWin),
		XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
	else
	if (aniWin->usePixmap == TRUE)
	gr_AniDraw(XtDisplay(aniWin->imageWin),
		XtWindow(aniWin->imageWin),aniWin->image[aniWin->curplane]);
	else
	gr_AniDrawImage(XtDisplay(aniWin->imageWin),
		XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
	
    return;
}


/*
 *	Turn on the stop flag
 */
void
gr_AniPlayStop(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;

	aniWin->playStop = 1;
}


/*
 *	Play sequence forward
 */
void
gr_AniForward(display,window,aniWin)
Display	*display;
Window	window;
A_AniWind_t	*aniWin;
{
	int skip,i;

	aniWin->playStop = 0;

	aniWin->curplane+=aniWin->skip+1;
	skip = aniWin->skip+1;

	while ((aniWin->curplane<aniWin->numplanes) && (aniWin->playStop == 0))
	{
		sprintf(msg,"%d",aniWin->curplane);
		gr_DialogSetValue(aniWin->frameDialog,msg);

		gr_SliderSetValue(aniWin->frameSlider,aniWin->curplane);

		if (aniWin->useDisk == TRUE)
		gr_AniDrawLoad(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
		else
		if (aniWin->usePixmap == TRUE)
		gr_AniDraw(display,window,aniWin->image[aniWin->curplane]);
		else
		gr_AniDrawImage(display,window,aniWin,aniWin->curplane);

		gr_AniEvent((long)aniWin->speed);

		aniWin->curplane+=skip;
	}

	aniWin->curplane--;
	aniWin->curplane-=aniWin->skip;
}


/*
 *	Play sequence backward
 */
void
gr_AniBackward(display,window,aniWin)
Display	*display;
Window	window;
A_AniWind_t	*aniWin;
{
	int skip,i;

	aniWin->playStop = 0;

	aniWin->curplane-=(aniWin->skip+1);
	skip = -aniWin->skip-1;

	while ((aniWin->curplane>=0) && (aniWin->playStop == 0))
	{
		sprintf(msg,"%d",aniWin->curplane);
		gr_DialogSetValue(aniWin->frameDialog,msg);

		gr_SliderSetValue(aniWin->frameSlider,aniWin->curplane);

		if (aniWin->useDisk == TRUE)
		gr_AniDrawLoad(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
		else
		if (aniWin->usePixmap == TRUE)
		gr_AniDraw(display,window,aniWin->image[aniWin->curplane]);
		else
		gr_AniDrawImage(display,window,aniWin,aniWin->curplane);

		gr_AniEvent((long)aniWin->speed);

		aniWin->curplane+=skip;
	}
	aniWin->curplane++;
	aniWin->curplane+=aniWin->skip;
}


/*
 *	Play sequence forward (and backward if autoreverse) until stopped
 */
void
gr_AniPlayForward(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;
	Display	*dpy=XtDisplay(aniWin->imageWin);
	Window	win=XtWindow(aniWin->imageWin);
	int		rev=1;

	if (aniWin->numplanes == 1)
	{
		gr_TextMsgOut(
		"There is only 1 frame in the sequence.\nI cannot animate this sequence.\n");
		return;
	}

	gr_AniForward(dpy,win,aniWin);

	while (aniWin->playStop == 0)
	switch(aniWin->autoMode)
	{
		case AUTOOFF:
			aniWin->playStop = 1;
			break;
		case AUTOREPEAT:
			aniWin->curplane = -1;
			gr_AniForward(dpy,win,aniWin);
			break;
		case AUTOREVERSE:
			if (rev == 1)
			{
				aniWin->curplane = aniWin->numplanes;
				gr_AniBackward(dpy,win,aniWin);
				rev=0;
			}
			else
			{
				aniWin->curplane = -1;
				gr_AniForward(dpy,win,aniWin);
				rev=1;
			}
			break;
	}

    return;
}


/*
 *	Play sequence backward (and forward if autoreverse) until stopped
 */
void
gr_AniPlayBackward(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;
	Display	*dpy=XtDisplay(aniWin->imageWin);
	Window	win=XtWindow(aniWin->imageWin);
	int			rev=1;

	if (aniWin->numplanes == 1)
	{
		gr_TextMsgOut(
		"There is only 1 frame in the sequence.\nI cannot animate this sequence.\n");
		return;
	}

	gr_AniBackward(dpy,win,aniWin);

	while (aniWin->playStop == 0)
	switch(aniWin->autoMode)
	{
		case AUTOOFF:
			aniWin->playStop = 1;
			break;
		case AUTOREPEAT:
			aniWin->curplane = aniWin->numplanes;
			gr_AniBackward(dpy,win,aniWin);
			break;
		case AUTOREVERSE:
			if (rev==1)
			{
				aniWin->curplane = 0;
				gr_AniForward(dpy,win,aniWin);
				rev=0;
			}
			else
			{
				aniWin->curplane = aniWin->numplanes;
				gr_AniBackward(dpy,win,aniWin);
				rev=1;
			}
			break;
	}

    return;
}


/*
 *	Step forward to next frame
 */
void
gr_AniStepForward(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;
	Display	*dpy=XtDisplay(aniWin->imageWin);
	Window	win=XtWindow(aniWin->imageWin);

	if (aniWin->curplane < (aniWin->numplanes-1) )
	{
		aniWin->playStop = 1;
		aniWin->curplane++;

		if (aniWin->useDisk == TRUE)
		gr_AniDrawLoad(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
		else
		if (aniWin->usePixmap == TRUE)
		gr_AniDraw(dpy,win,aniWin->image[aniWin->curplane]);
		else
		gr_AniDrawImage(dpy,win,aniWin,aniWin->curplane);

		sprintf(msg,"%d",aniWin->curplane);
		gr_DialogSetValue(aniWin->frameDialog,msg);
		gr_SliderSetValue(aniWin->frameSlider,aniWin->curplane);
	}
    return;
}


/*
 *	Step backward to next frame
 */
void
gr_AniStepBackward(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;
	Display	*dpy=XtDisplay(aniWin->imageWin);
	Window	win=XtWindow(aniWin->imageWin);

	if (aniWin->curplane > 0)
	{
		aniWin->playStop = 1;
		aniWin->curplane--;

		if (aniWin->useDisk == TRUE)
		gr_AniDrawLoad(XtDisplay(aniWin->imageWin),
			XtWindow(aniWin->imageWin),aniWin,aniWin->curplane);
		else
		if (aniWin->usePixmap == TRUE)
		gr_AniDraw(dpy,win,aniWin->image[aniWin->curplane]);
		else
		gr_AniDrawImage(dpy,win,aniWin,aniWin->curplane);

		sprintf(msg,"%d",aniWin->curplane);
		gr_DialogSetValue(aniWin->frameDialog,msg);
		gr_SliderSetValue(aniWin->frameSlider,aniWin->curplane);
	}
    return;
}
