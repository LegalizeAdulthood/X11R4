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
 *	File:		gr_anilev.c
 *	Contents:	Animate level window functions for graphics module
 */

#include "gr_com.h"

#include <signal.h>
extern long td_CurrentTime();
extern int	gr_AniTimeOut();

#define GR_ANIDELAYMAX  2000000
#define GR_ANIDELAYGRD  10000
#define	MINWINXSIZE	285
#define MINWINYSIZE	100
#define	MAXWINXSIZE	900
#define MAXWINYSIZE	900
#define LEEWAY		2


/*
 * Make control buttons for animation window
 */
gr_MakeAniControls(boxWind,RCWind,tmp,numplanes)
Widget		boxWind,RCWind;
A_AniWind_t	*tmp;
int			numplanes;
{
		XImage *palimage;

		palimage = gr_ImageCreate(boxWind,gr_color.maxColors,30,gr_palData);
		gr_MakeImageStatic("XDSstaticimage",boxWind,palimage,
					  NULL,(caddr_t)NULL,10,5,256,30);
		gr_MakeButton("XDSbutton",boxWind,"<<",
					  (XtCallbackProc)gr_AniPlayBackward,(caddr_t)tmp,
					  55,42,20,20);
		gr_MakeButton("XDSbutton",boxWind,"<",
					  (XtCallbackProc)gr_AniStepBackward,(caddr_t)tmp,
					  78,42,20,20);
		gr_MakeButton("XDSbutton",boxWind,"<>",
					  (XtCallbackProc)gr_AniPlayStop,(caddr_t)tmp,
					  101,42,20,20);
		gr_MakeButton("XDSbutton",boxWind,">",
					  (XtCallbackProc)gr_AniStepForward,(caddr_t)tmp,
					  124,42,20,20);
		gr_MakeButton("XDSbutton",boxWind,">>",
					  (XtCallbackProc)gr_AniPlayForward,(caddr_t)tmp,
					  147,42,20,20);

		tmp->frameSlider =
			gr_MakeSlider("SDSslider",boxWind,0,numplanes,1,0,1,
				(XtCallbackProc)gr_AniFrameSliderSel,
				(XtCallbackProc)gr_AniFrameSliderMov,
				(XtCallbackProc)gr_AniFrameSliderRel,
				HORIZONLY, (caddr_t)tmp,55,65,112,20);

		RCWind = gr_MakeRowColumn("XDSrowcolumn",boxWind,ONEOFMANY,3,
					2,90,200,20);
		gr_MakeToggle("XDStoggle",RCWind,"Off",DIAMOND,
					TRUE, (XtCallbackProc)gr_AniSetAutoOff,
					NULL,(caddr_t)tmp, 2,2,40,20);
		gr_MakeToggle("XDStoggle",RCWind,"Repeat",DIAMOND,
					FALSE, (XtCallbackProc)gr_AniSetAutoRep,
					NULL,(caddr_t)tmp, 42,2,60,20);
		gr_MakeToggle("XDStoggle",RCWind,"Reverse",DIAMOND,
					FALSE, (XtCallbackProc)gr_AniSetAutoRev,
					NULL,(caddr_t)tmp, 102,2,60,20);

		tmp->frameDialog=
			gr_MakeDialog("XDSdialog",boxWind,"Frame","0",5,
						170,42,50,48);
		gr_MakeButton("XDSbutton",boxWind,"Set",
					  (XtCallbackProc)gr_AniFrameSet,(caddr_t)tmp,
					  170,90,50,20);
		tmp->skipDialog=
			gr_MakeDialog("XDSdialog",boxWind,"Skip ","0",5,
						222,42,50,48);
		gr_MakeButton("XDSbutton",boxWind,"Set",
					  (XtCallbackProc)gr_AniSkipSet,(caddr_t)tmp,
					  222,90,50,20);
		gr_MakeButton("XDSbutton",boxWind,"Fast <-",
					  (XtCallbackProc)gr_AniSpeedFast,(caddr_t)tmp,
					  2,115,50,15);
		tmp->speedSlider =
			gr_MakeSlider("SDSslider",boxWind,0,GR_ANIDELAYMAX+1,1,0,1,
				(XtCallbackProc)gr_AniSpeedSliderSel,
				(XtCallbackProc)gr_AniSpeedSliderMov,
				(XtCallbackProc)gr_AniSpeedSliderRel,
				HORIZONLY, (caddr_t)tmp,54,115,166,15);
		gr_MakeButton("XDSbutton",boxWind,"-> Slow",
					  (XtCallbackProc)gr_AniSpeedSlow,(caddr_t)tmp,
					  222,115,50,15);
}


/*
 * Animate along cartesian axes
 */
A_AniWind_t
*gr_InitAniLevel(header,shellName,parent,numplanes,incr,scale,orient,tparent,
	pathname)
A_AniWind_t *header;
char   	 *shellName;
Widget	 parent;
int		 numplanes,incr,scale;
A_Axes_t		orient;
A_CubeWind_t	*tparent;
char	*pathname;
{
	A_BossWind_t	*bossWin=tparent->parent;
	A_AniWind_t		*tmp;
	Widget			boxWind,RCWind;
	int				usePlane,winxsize,winysize,xsize,ysize,numFrames,ret,fileexist;
	short			i,j;
	long			startTime,remTime;
	char			label[STRNG40],title[STRNG40],strng[STRNG40];
	XGCValues		gcvals;
	Display			*dpy;
	Window			win;

	gr_WidgetCursor(tparent->shell,XC_watch);

	tmp = (A_AniWind_t *)td_Malloc(sizeof(A_AniWind_t),"A_AniWind_t");

	td_HdfsetScale(bossWin->data,scale);

	tmp->usePixmap = tparent->usePixmap;
	tmp->useDisk = tparent->useDisk;

	if (tmp->useDisk == TRUE)
	{
		numFrames = 1;
		tmp->pathname = (char *)td_Malloc1D(1,strlen(pathname)+1,sizeof(char),
		"td_Malloc1D");
		strcpy(tmp->pathname,pathname);
	}
	else
	{
		numFrames = numplanes;
	}

	tmp->imagexsize = (int *)calloc(numFrames,sizeof(int));
	if (tmp->imagexsize == NULL)
	{
		fprintf(stderr,"Error\t: Out of memory in calloc imagexsize.\n");
		exit(-1);
	}
	tmp->imageysize = (int *)calloc(numFrames,sizeof(int));
	if (tmp->imageysize == NULL)
	{
		fprintf(stderr,"Error\t: Out of memory in calloc imageysize.\n");
		exit(-1);
	}

	if (tparent->interp == FALSE)
	{
		xsize = td_HdfgetDim(bossWin->data,orient.col)*scale;
		ysize = td_HdfgetDim(bossWin->data,orient.row)*scale;
	}
	else
	{
		xsize = (td_HdfgetDim(bossWin->data,orient.col)-1)*scale;
		ysize = (td_HdfgetDim(bossWin->data,orient.row)-1)*scale;
	}

	if (xsize > MINWINXSIZE)
		if (xsize > MAXWINXSIZE)
			winxsize = MAXWINXSIZE;
		else
			winxsize = xsize+LEEWAY;
	else
		winxsize = MINWINXSIZE+LEEWAY;

	if (ysize > MINWINYSIZE)
		if (ysize > MAXWINYSIZE)
			winysize = MAXWINYSIZE;
		else
			winysize = ysize+LEEWAY;
	else
		winysize = MINWINYSIZE+LEEWAY;

	sprintf(label,"%s: Animate %d planes",tparent->parent->filename,
			numplanes);

	tmp->shell = gr_MakeWindow("XDS Frame",parent,&(tmp->win),
					(XtCallbackProc)gr_CloseAniLevel,
					ANIWIND,label,"Close",(caddr_t)tmp,
					110,10,winxsize+8,winysize+170);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,
		2,winysize+2,275,134);
		gr_MakeButton("XDSbutton",boxWind,"Save",
					  (XtCallbackProc)gr_SaveAni,(caddr_t)tmp,
					  2,42,50,20);
		gr_MakeButton("XDSbutton",boxWind,"Palette",
					  (XtCallbackProc)gr_LoadPAL,(caddr_t)tmp->shell,
					  2,65,50,20);

	gr_MakeAniControls(boxWind,RCWind,tmp,numplanes);

	tmp->imageVPort = gr_MakeVPort("XDSvport",tmp->win,
						NOSCROLL,HP,NULL, NULL,5,(caddr_t)tmp,
						2,2,winxsize+2,winysize+2);

	tmp->imageWin = gr_MakeWorkSpace("XDSworkspace",tmp->imageVPort,
					(XtCallbackProc)gr_AniExpose,NULL,NULL,(caddr_t)tmp,
					0,0,xsize,ysize);

	tmp->data = td_Malloc2D(1,numplanes,(long)sizeof(char *),"td_Malloc2D");
	if (tmp->usePixmap == TRUE)
	tmp->image = (Pixmap *)td_Malloc1D(1,numFrames,(long)sizeof(Pixmap),
		"td_Malloc1D");
	else
	tmp->ximage = (XImage **)td_Malloc1D(1,numFrames,(long)sizeof(XImage *),
		"td_Malloc1D");

	tmp->aniType = 1;
	tmp->playStop = 1;
	tmp->autoMode = AUTOOFF;
	tmp->curplane = 0;
	tmp->speed = 0;
	tmp->skip = 0;
	tmp->numplanes = numplanes;
	tmp->axesOrient = orient;

	dpy = XtDisplay(tmp->imageWin);
	win = XtWindow(tmp->imageWin);
	gcvals.foreground = BlackPixel(dpy,DefaultScreen(dpy));
	tmp->imageWinGC = XtGetGC(tmp->imageWin, GCForeground, &gcvals);

	startTime = td_CurrentTime();

	i=0;
	for (j=0;j<numplanes;j++)
	{
		usePlane = orient.plane+(j*incr);
		if (tmp->useDisk == FALSE)
			i = j;

		tmp->imagexsize[i] = xsize;
		tmp->imageysize[i] = ysize;
		if ((tmp->useDisk == TRUE) && (j > 0))
		{
			if (tmp->usePixmap == TRUE)
				XFreePixmap(dpy,tmp->image[0]);
			else
				XDestroyImage(tmp->ximage[0]);
			td_Free((char *)tmp->data[0]);
		}
		tmp->data[i] = td_HdfgetPixData(bossWin->data,
				orient.row,orient.col,orient.axis,
				usePlane,tparent->interp,
				(unsigned char)gr_color.nColors);

		if (tmp->usePixmap == TRUE)
		{
		tmp->image[i] = gr_PixmapCreate(tmp->imageWin,tmp->imageWinGC,
				tmp->imagexsize[i],tmp->imageysize[i],tmp->data[i]);
		gr_AniDraw(dpy,win,tmp->image[i]);
		}
		else
		{
		tmp->ximage[i] = gr_ImageCreate(tmp->imageWin,
				tmp->imagexsize[i],tmp->imageysize[i],tmp->data[i]);
		gr_AniDrawImage(dpy,win,tmp,i);
		}

		if (tmp->useDisk == TRUE)
		{
			fileexist = td_FileExist(pathname);

			if (fileexist == 0)
				ret = td_HdfPutImage(pathname,tmp->data[i],gr_color.palette,
				tmp->imagexsize[i],tmp->imageysize[i]);
			else
				ret = td_HdfAddImage(pathname,tmp->data[i],
				tmp->imagexsize[i],tmp->imageysize[i]);
			if (ret == -1)
				gr_TextMsgOut("Error in saving Animation image!\n");
			else
				gr_TextMsgOut("Saved ");
		}
		remTime = (long)((td_CurrentTime()-startTime)/(j+1.0)*
						 (numplanes-j-1.0));
		sprintf(strng,"Generated frame %d, %5lds to completion...\n",j,remTime);
		gr_TextMsgOut(strng);
	}

	if (tmp->useDisk == TRUE)
		gr_AniDrawLoad(dpy,win,tmp,0);
	else
	if (tmp->usePixmap == TRUE)
		gr_AniDraw(dpy,win,tmp->image[0]);
	else
		gr_AniDrawImage(dpy,win,tmp,0);

	tmp->parent	= tparent;
	tmp->arbparent = NULL;
	tmp->prev	= NULL;
	tmp->next	= header;
	if (header != NULL)
		header->prev = tmp;

	gr_WidgetCursor(tmp->shell,XC_draped_box);
	gr_WidgetCursor(tparent->shell,XC_draped_box);

	return(tmp);
}


/*
 * Animate along arbitrary axes
 */
A_AniWind_t
*gr_InitAniLevel3(header,shellName,parent,numplanes,zincr,scale,tparent,pathname)
A_AniWind_t *header;
char   	 *shellName;
Widget	 parent;
int		 numplanes,scale;
double	 zincr;
A_ArbWind_t	*tparent;
char	 *pathname;
{
	A_BossWind_t	*bossWin=tparent->parent;
	A_AniWind_t		*tmp;
	A_Box_t			*box=tparent->xybox;
	Widget			boxWind,RCWind;
	int				winxsize,winysize,xsize,ysize,numFrames,fileexist,ret;
	short			i,j;
	long			startTime,remTime;
	char			label[STRNG40],title[STRNG40],strng[STRNG40];
	double			useDepth,startInDepth;
	Boolean			quit = FALSE;
	XGCValues		gcvals;
	Display			*dpy;
	Window			win;

	gr_WidgetCursor(tparent->shell,XC_watch);

	tmp = (A_AniWind_t *)td_Malloc(sizeof(A_AniWind_t),"A_AniWind_t");

	td_HdfsetScale(bossWin->data,scale);

	tmp->usePixmap = tparent->usePixmap;
	tmp->useDisk = tparent->useDisk;

	xsize = (int)(((int)box->xmax-(int)box->xmin)*scale);
	ysize = (int)(((int)box->ymax-(int)box->ymin)*scale);
	if (xsize > MINWINXSIZE)
		if (xsize > MAXWINXSIZE)
			winxsize = MAXWINXSIZE;
		else
			winxsize = xsize+LEEWAY;
	else
		winxsize = MINWINXSIZE+LEEWAY;

	if (ysize > MINWINYSIZE)
		if (ysize > MAXWINYSIZE)
			winysize = MAXWINYSIZE;
		else
			winysize = ysize+LEEWAY;
	else
		winysize = MINWINYSIZE+LEEWAY;

	startInDepth = (double)box->indepth;

	i = 0; quit = FALSE;
	while ((i< numplanes) && (quit == FALSE))
	{
		useDepth = (double)i*zincr+startInDepth;
		if (useDepth < 100.0)
			i++;
		else
			quit = TRUE;
	}

	if (numplanes > i)
	{
	sprintf(strng,"Warning: Only %d frames can be generated.\n",numplanes);
	gr_TextMsgOut(strng);
	}

	numplanes = i;

	if (tmp->useDisk == TRUE)
	{
		numFrames = 1;
		tmp->pathname = (char *)td_Malloc1D(1,strlen(pathname)+1,sizeof(char),
		"td_Malloc1D");
		strcpy(tmp->pathname,pathname);
	}
	else
	{
		numFrames = numplanes;
	}

	tmp->imagexsize = (int *)td_Malloc1D(1,numFrames,sizeof(int),"td_Malloc1D");
	if (tmp->imagexsize == NULL)
	{
		fprintf(stderr,"Error\t: Out of memory in calloc imagexsize.\n");
		exit(-1);
	}
	tmp->imageysize = (int *)td_Malloc1D(1,numFrames,sizeof(int),"td_Malloc1D");
	if (tmp->imageysize == NULL)
	{
		fprintf(stderr,"Error\t: Out of memory in calloc imageysize.\n");
		exit(-1);
	}

	sprintf(label,"%s: Animate %d planes",tparent->parent->filename,
			numplanes);

	tmp->shell = gr_MakeWindow("XDS Frame",parent,&(tmp->win),
					(XtCallbackProc)gr_CloseAniLevel,
					ANIWIND,label,"Close",(caddr_t)tmp,
					110,10,winxsize+8,winysize+170);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,
		2,winysize+2,275,134);
		gr_MakeButton("XDSbutton",boxWind,"Save",
					  (XtCallbackProc)gr_SaveAni,(caddr_t)tmp,
					  2,42,50,20);
		gr_MakeButton("XDSbutton",boxWind,"Palette",
					  (XtCallbackProc)gr_LoadPAL,(caddr_t)tmp->shell,
					  2,65,50,20);

	gr_MakeAniControls(boxWind,RCWind,tmp,numplanes);

	tmp->imageVPort = gr_MakeVPort("XDSvport",tmp->win,
						NOSCROLL,HP,NULL, NULL,5,(caddr_t)tmp,
						2,2,winxsize,winysize);

	tmp->imageWin = gr_MakeWorkSpace("XDSworkspace",tmp->imageVPort,
					(XtCallbackProc)gr_AniExpose,NULL,NULL,(caddr_t)tmp,
					0,0,xsize,ysize);

	tmp->data = (char **)td_Malloc2D(1,numFrames,sizeof(char *),"td_Malloc2D");
	if (tmp->usePixmap == TRUE)
	tmp->image = (Pixmap *)td_Malloc1D(1,numFrames,sizeof(Pixmap),
		"td_Malloc1D");
	else
	tmp->ximage = (XImage **)td_Malloc1D(1,numFrames,sizeof(XImage *),
		"td_Malloc1D");

	dpy = XtDisplay(tmp->imageWin);
	win = XtWindow(tmp->imageWin);
	gcvals.foreground = BlackPixel(dpy,DefaultScreen(dpy));
	tmp->imageWinGC = XtGetGC(tmp->imageWin, GCForeground, &gcvals);
	tmp->aniType = 3;
	tmp->playStop = 1;
	tmp->autoMode = AUTOOFF;
	tmp->curplane = 0;
	tmp->speed = 0;
	tmp->skip = 0;
	tmp->numplanes = numplanes;

	startTime = td_CurrentTime();

	i = 0;
	for (j=0;j<numplanes;j++)
	{
		if (tmp->useDisk == FALSE)
			i=j;

		useDepth = (double)j*zincr+startInDepth;
		gr_ArbZSliderSel(tparent->zSlider,tparent,100-(int)useDepth);
		tmp->imagexsize[i] = xsize;
		tmp->imageysize[i] = ysize;
		if ((tmp->useDisk == TRUE) && (j > 0))
		{
			td_Free((char *)tmp->data[0]);
			if (tmp->usePixmap == TRUE)
				XFreePixmap(dpy,tmp->image[0]);
			else
			XDestroyImage(tmp->ximage[0]);
		}
		tmp->data[i] = gr_ViewgetData(box,scale,2,bossWin);
		if (tmp->usePixmap == TRUE)
		{
		tmp->image[i] = gr_PixmapCreate(tmp->imageWin,tmp->imageWinGC,
					tmp->imagexsize[i],tmp->imageysize[i],tmp->data[i]);
		gr_AniDraw(dpy,win,tmp->image[i]);
		}
		else
		{
		tmp->ximage[i] = gr_ImageCreate(tmp->imageWin,
					tmp->imagexsize[i],tmp->imageysize[i],tmp->data[i]);
		gr_AniDrawImage(dpy,win,tmp,i);
		}
		if (tmp->useDisk == TRUE)
		{
			fileexist = td_FileExist(pathname);

			if (fileexist == 0)
				ret = td_HdfPutImage(pathname,tmp->data[i],gr_color.palette,
				tmp->imagexsize[i],tmp->imageysize[i]);
			else
				ret = td_HdfAddImage(pathname,tmp->data[i],
				tmp->imagexsize[i],tmp->imageysize[i]);
			if (ret == -1)
				gr_TextMsgOut("Error in saving Animation image!\n");
			else
				gr_TextMsgOut("Saved ");
		}
		remTime = (long)((td_CurrentTime()-startTime)/(j+1.0)*
						 (numplanes-j-1.0));
		sprintf(strng,"Generated frame %d, %5lds to completion...\n",j,remTime);
		gr_TextMsgOut(strng);
	}

	if (tmp->useDisk == TRUE)
		gr_AniDrawLoad(dpy,win,tmp,0);
	else
	if (tmp->usePixmap == TRUE)
		gr_AniDraw(dpy,win,tmp->image[0]);
	else
		gr_AniDrawImage(dpy,win,tmp,0);

	tmp->arbparent	= tparent;
	tmp->parent		= NULL;
	tmp->prev	= NULL;
	tmp->next	= header;
	if (header != NULL)
		header->prev = tmp;

	gr_WidgetCursor(tmp->shell,XC_draped_box);
	gr_WidgetCursor(tparent->shell,XC_draped_box);

	return(tmp);
}


/*
 *	Animate from stored raster images
 */
A_AniWind_t
*gr_InitAniLevel2(header,filename,pathname,parent,start,stop,numplanes,
	xsize,ysize,ispal,usePixmap,useDisk)
A_AniWind_t *header;
char   	 *filename,*pathname;
Widget	 parent;
int		 start,stop,numplanes,xsize,ysize,ispal;
Boolean	 usePixmap,useDisk;
{
	A_AniWind_t	*tmp;
	Widget		boxWind,RCWind;
	short		i,j,winxsize,winysize,numFrames,frameStop;
	char		label[STRNG40],title[STRNG40],strng[STRNG40];
	XGCValues		gcvals;
	Display			*dpy;
	Window			win;
    Boolean     noCMap=TRUE;

	gr_WidgetCursor(gr_topWin.shell,XC_watch);

	tmp = (A_AniWind_t *)td_Malloc(sizeof(A_AniWind_t),"A_AniWind_t");

	sprintf(label,"Animate %d planes from %s",numplanes,filename);

	tmp->usePixmap = usePixmap;
	tmp->useDisk = useDisk;

	if (tmp->useDisk == TRUE)
	{
		numFrames = 1;
		frameStop = start;
		tmp->pathname = (char *)td_Malloc1D(1,strlen(pathname)+1,sizeof(char),
		"td_Malloc1D");
		strcpy(tmp->pathname,pathname);
	}
	else
	{
		numFrames = numplanes;
		frameStop = stop;
	}
	tmp->imagexsize = (int *)td_Malloc1D(1,numFrames,sizeof(int),
		"td_Malloc1D");
	if (tmp->imagexsize == NULL)
	{
		fprintf(stderr,"Error\t: Out of memory in calloc imagexsize.\n");
		exit(-1);
	}
	tmp->imageysize = (int *)td_Malloc1D(1,numFrames,sizeof(int),
		"td_Malloc1D");
	if (tmp->imageysize == NULL)
	{
		fprintf(stderr,"Error\t: Out of memory in calloc imageysize.\n");
		exit(-1);
	}

	if (xsize > MINWINXSIZE)
		if (xsize > MAXWINXSIZE)
			winxsize = MAXWINXSIZE;
		else
			winxsize = xsize+LEEWAY;
	else
		winxsize = MINWINXSIZE+LEEWAY;

	if (ysize > MINWINYSIZE)
		if (ysize > MAXWINYSIZE)
			winysize = MAXWINYSIZE;
		else
			winysize = ysize+LEEWAY;
	else
		winysize = MINWINYSIZE+LEEWAY;

	tmp->shell = gr_MakeWindow("XDS Frame",parent,&(tmp->win),
					(XtCallbackProc)gr_CloseAniLevel,
					ANIWIND,label,"Close",(caddr_t)tmp,
					110,10,winxsize+8,winysize+170);

	boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,
		2,winysize+2,275,134);
		gr_MakeButton("XDSbutton",boxWind,"Save",
					  (XtCallbackProc)gr_SaveAni,(caddr_t)tmp,
					  2,42,50,20);
		gr_MakeButton("XDSbutton",boxWind,"Palette",
					  (XtCallbackProc)gr_LoadPAL,(caddr_t)tmp->shell,
					  2,65,50,20);
/*		gr_MakeButton("XDSbutton",boxWind,"Palette",
					  (XtCallbackProc)gr_LoadPAL,(caddr_t)tmp->shell,
					  2,42,50,45); */

	gr_MakeAniControls(boxWind,RCWind,tmp,numplanes);

	tmp->imageVPort = gr_MakeVPort("XDSvport",tmp->win,
						NOSCROLL,HP,NULL, NULL,5,(caddr_t)tmp,
						2,2,winxsize,winysize);

	tmp->imageWin = gr_MakeWorkSpace("XDSworkspace",tmp->imageVPort,
					(XtCallbackProc)gr_AniExpose,NULL,NULL,(caddr_t)tmp,
					0,0,xsize,ysize);

	tmp->data = (char **)td_Malloc2D(1,numFrames,sizeof(char *),"td_Malloc2D");
	if (tmp->usePixmap == TRUE)
	tmp->image = (Pixmap *)td_Malloc1D(1,numFrames,sizeof(Pixmap),
		"td_Malloc1D");
	else
	tmp->ximage = (XImage **)td_Malloc1D(1,numFrames,sizeof(XImage *),
		"td_Malloc1D");

	dpy = XtDisplay(tmp->imageWin);
	win = XtWindow(tmp->imageWin);
	gcvals.foreground = BlackPixel(dpy,DefaultScreen(dpy));
	gcvals.background = WhitePixel(dpy,DefaultScreen(dpy));
	tmp->imageWinGC = XtGetGC(tmp->imageWin,
		GCForeground|GCBackground, &gcvals);
	tmp->aniType = 2;
	tmp->playStop = 1;
	tmp->autoMode = AUTOOFF;
	tmp->curplane = 0;
	tmp->skip = 0;
	tmp->speed = 0;
	tmp->numplanes = numplanes;

	for (j=0;j<start;j++)
	   td_HdfgetRasData(pathname,gr_color.palette,
			&(tmp->imagexsize[0]),&(tmp->imageysize[0]),&ispal,FALSE);

    i = 0;
	for (j=start;j<=frameStop;j++)
	{
		tmp->data[i] = td_HdfgetRasData(pathname,gr_color.palette,
				&(tmp->imagexsize[i]),&(tmp->imageysize[i]),&ispal,TRUE);

	    if ((ispal == 1) && (noCMap))
		{
		    gr_ImageInitCMapHDF(gr_color.palette);
			gr_ImageSetCMap(tmp->shell);
			noCMap = FALSE;
		}
		if (tmp->usePixmap == TRUE)
		{
		tmp->image[i] = gr_PixmapCreate(tmp->imageWin,tmp->imageWinGC,
						tmp->imagexsize[i],tmp->imageysize[i],tmp->data[i]);
		gr_AniDraw(dpy,win,tmp->image[i]);
		}
		else
		{
		tmp->ximage[i] = gr_ImageCreate(tmp->imageWin,
						tmp->imagexsize[i],tmp->imageysize[i],tmp->data[i]);
		gr_AniDrawImage(dpy,win,tmp,i);
		}
		i++;
	}

	if (tmp->useDisk == FALSE)
	{
		if (tmp->usePixmap == TRUE)
			gr_AniDraw(dpy,win,tmp->image[0]);
		else
			gr_AniDrawImage(dpy,win,tmp,0);
	}
	sprintf(strng,"First frame ID=%d, Last frame ID =%d\n",start,stop);
	gr_TextMsgOut(strng);

	tmp->parent = NULL;
	tmp->arbparent = NULL;
	tmp->prev 	= NULL;
	tmp->next	= header;
	if (header != NULL)
		header->prev = tmp;

	gr_WidgetCursor(tmp->shell,XC_draped_box);
	gr_WidgetCursor(gr_topWin.shell,XC_draped_box);

	return(tmp);
}


/*
 *	Close animation windows
 */
void
gr_CloseAniLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_AniWind_t	*aniWin=(A_AniWind_t *)client_data;
	Display		*dpy=XtDisplay(aniWin->imageWin);
	short i,numFrames;

	if (aniWin != NULL)
	{
		if (aniWin->playStop == 0)
		{
			/* stop the playing first */
			aniWin->playStop = 1;
			return;
		}

		/* update previous neighbour's pointer */
        if (aniWin->prev != NULL)
            aniWin->prev->next = aniWin->next;
        else
		{
			/* update parent of aniWin */
			switch (aniWin->aniType)
			{
				case 1:
            		aniWin->parent->aniWin = aniWin->next;
        			aniWin->parent->numAniWins--;
					break;
				case 2:
					gr_topWin.aniWin = aniWin->next;
					gr_topWin.numAniWins--;
					break;
				case 3:
            		aniWin->arbparent->aniWin = aniWin->next;
        			aniWin->arbparent->numAniWins--;
					break;
			}
		}

		/* update next neighbour's pointer */
        if (aniWin->next != NULL)
            aniWin->next->prev = aniWin->prev;

		if (aniWin->useDisk == TRUE)
			numFrames = 1;
		else
			numFrames = aniWin->numplanes;

		for (i=0;i<numFrames;i++)
		{
			if (aniWin->usePixmap == TRUE)
				XFreePixmap(dpy,aniWin->image[i]);
			else
			{
				XDestroyImage(aniWin->ximage[i]);
				td_Free((char *)aniWin->ximage[i]);
			}
			td_Free((char *)aniWin->data[i]);
		}
		if (aniWin->usePixmap == TRUE)
			td_Free((char *)aniWin->image);
		else
			td_Free((char *)aniWin->ximage);
		td_Free((char *)aniWin->data);
		td_Free((char *)aniWin->imagexsize);
		td_Free((char *)aniWin->imageysize);
		XtReleaseGC(aniWin->imageWin,aniWin->imageWinGC);
		XtDestroyWidget(aniWin->shell);
		td_Free((char *)aniWin);
	}
}
