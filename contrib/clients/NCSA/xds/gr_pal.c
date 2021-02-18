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
 *	File:		gr_pal.c
 *	Contents:	routines for color pallet manipulation
 */

#include "gr_com.h"

extern	int	td_HdfPalLoad();

XColor	gr_origCells[256];


/*
 *	Initialize palette data
 */
void
gr_PalInit(wid)
Widget	wid;
{
	Display	*dpy = XtDisplay(wid);
	int 	scr = DefaultScreen(dpy);
	Visual  *vis = gr_GetVisual(dpy,scr);
	int		bprgb = vis->bits_per_rgb;
	int		depth = DisplayPlanes(dpy,scr);
	int		x,y,i=0;

	if (bprgb == 0)
		bprgb = 8;

	gr_color.white = WhitePixel(dpy,scr);
	gr_color.black = BlackPixel(dpy,scr);
	gr_color.depth = depth;
	gr_color.shiftbits = 8;

	gr_colorSplit.white = WhitePixel(dpy,scr);
	gr_colorSplit.black = BlackPixel(dpy,scr);
	gr_colorSplit.depth = depth;
	gr_colorSplit.shiftbits = 8;

	gr_TextMsgOut("X Server characteristics:\n");
	sprintf(msg,"# color planes: %d, ",depth);
	gr_TextMsgOut(msg);

	if (depth == 1)
	{
		/* 1 plane monochrome */
		gr_TextMsgOut("Warning\t: XDataSlice will not display color on a \n");
		gr_TextMsgOut("single plane monochrome device.\n");
		gr_color.nColors = 2;
		gr_color.maxColors = 2;
		gr_color.canSet = FALSE;
		gr_colorSplit.nColors = 2;
		gr_colorSplit.maxColors = 2;
		gr_colorSplit.canSet = FALSE;
	}
	else
	{
		switch (vis->class)
		{
			case PseudoColor:
			gr_TextMsgOut("Pseudo Color device, ");
			gr_color.nColors = vis->map_entries-2*RESERVED_COLORS;
			gr_color.maxColors = vis->map_entries;
			gr_color.canSet = TRUE;
			gr_colorSplit.nColors = (vis->map_entries/3)-2*RESERVED_COLORS;
			gr_colorSplit.maxColors = vis->map_entries;
			gr_colorSplit.canSet = TRUE;
			break;

			case StaticColor:
			gr_TextMsgOut("\nWarning\t: XDataSlice cannot change the color\n");
			gr_TextMsgOut("map on a StaticColor device.\n");
			gr_color.nColors = vis->map_entries-2*RESERVED_COLORS;
			gr_color.maxColors = vis->map_entries;
			gr_color.canSet = FALSE;
			gr_colorSplit.nColors = (vis->map_entries/3)-2*RESERVED_COLORS;
			gr_colorSplit.maxColors = vis->map_entries;
			gr_colorSplit.canSet = FALSE;
			break;

			case DirectColor:
			gr_TextMsgOut("Direct Color device, ");
			gr_color.nColors = vis->map_entries-2*RESERVED_COLORS;
			gr_color.maxColors = vis->map_entries;
			gr_color.canSet = TRUE;
			gr_colorSplit.nColors = (vis->map_entries/3)-2*RESERVED_COLORS;
			gr_colorSplit.maxColors = vis->map_entries;
			gr_colorSplit.canSet = TRUE;
			break;

			case TrueColor:
			gr_TextMsgOut("\nWarning\t: XDataSlice cannot change the color\n");
			gr_TextMsgOut("map on a TrueColor device.\n");
			gr_color.nColors = vis->map_entries-2*RESERVED_COLORS;
			gr_color.maxColors = vis->map_entries;
			gr_color.canSet = FALSE;
			gr_colorSplit.nColors = (vis->map_entries/3)-2*RESERVED_COLORS;
			gr_colorSplit.maxColors = vis->map_entries;
			gr_colorSplit.canSet = FALSE;
			break;

			case GrayScale:
			gr_TextMsgOut("\nWarning\t: XDataSlice cannot change the color\n");
			gr_TextMsgOut("map on a GrayScale device.\n");
			gr_color.nColors = vis->map_entries-2*RESERVED_COLORS;
			gr_color.maxColors = vis->map_entries;
			gr_color.canSet = FALSE;
			gr_colorSplit.nColors = (vis->map_entries/3)-2*RESERVED_COLORS;
			gr_colorSplit.maxColors = vis->map_entries;
			gr_colorSplit.canSet = FALSE;
			break;

			case StaticGray:
			gr_TextMsgOut("\nWarning\t: XDataSlice cannot change the color\n");
			gr_TextMsgOut("map on a StaticGray device.\n");
			gr_color.nColors = vis->map_entries-2*RESERVED_COLORS;
			gr_color.maxColors = vis->map_entries;
			gr_color.canSet = FALSE;
			gr_colorSplit.nColors = (vis->map_entries/3)-2*RESERVED_COLORS;
			gr_colorSplit.maxColors = vis->map_entries;
			gr_colorSplit.canSet = FALSE;
			break;
		}

	}

	if (gr_color.maxColors > 256)
	{
		gr_color.maxColors = 256;
		gr_color.nColors = 256-2*RESERVED_COLORS;
	}
	if (gr_colorSplit.maxColors > 256)
	{
		gr_colorSplit.maxColors = 256;
		gr_colorSplit.nColors = (256/3)-2*RESERVED_COLORS;
	}

	gr_palData = td_Malloc1D(gr_color.maxColors,30,
		(long)sizeof(unsigned char),"Malloc palData");

	for (y=0;y<30;y++)
		for (x=0;x<gr_color.maxColors;x++)
		{
			gr_palData[i] = (char)x;
			i++;
		}

	sprintf(msg,"# colors used = %d\n",gr_color.nColors);

	gr_TextMsgOut(msg);
}


/*
 *	Set palette data using HDF palette data format
 */
void
gr_ImageInitCMapHDF(palette)
unsigned char *palette;
{
	int	x;

	for (x = 0; x<gr_color.maxColors; x++)
	{
		gr_color.cmapCells[x].pixel = x;
		gr_color.cmapCells[x].red=
			(unsigned short)palette[x*3] << gr_color.shiftbits ;
		gr_color.cmapCells[x].green=
			(unsigned short) palette[x*3+1] << gr_color.shiftbits ;
		gr_color.cmapCells[x].blue=
			(unsigned short) palette[x*3+2] << gr_color.shiftbits;
		gr_color.cmapCells[x].flags=DoRed|DoGreen|DoBlue;

		gr_color.palette[x] = palette[x];
	}

	gr_color.cmapCells[gr_color.black].red		= (unsigned short) 0;
	gr_color.cmapCells[gr_color.black].green	= (unsigned short) 0;
	gr_color.cmapCells[gr_color.black].blue		= (unsigned short) 0;

	gr_color.cmapCells[gr_color.white].red		= (unsigned short) 65535;
	gr_color.cmapCells[gr_color.white].green	= (unsigned short) 65535;
	gr_color.cmapCells[gr_color.white].blue		= (unsigned short) 65535;
}


/*
 *	Set palette data using SEQ palette data format
 */
void
gr_ImageInitCMapRGB(r,g,b)
unsigned char r[],g[],b[];
{
	int	x;
	unsigned char	*p;

	p = gr_color.palette;
	for (x = 0; x<gr_color.maxColors; x++)
	{
		gr_color.cmapCells[x].pixel = x;
		gr_color.cmapCells[x].red = r[x] << gr_color.shiftbits ;
		gr_color.cmapCells[x].green = g[x] << gr_color.shiftbits ;
		gr_color.cmapCells[x].blue = b[x] << gr_color.shiftbits ;
		gr_color.cmapCells[x].flags = DoRed|DoGreen|DoBlue;
		*p++ = r[x];
		*p++ = g[x];
		*p++ = b[x];
	}

	gr_color.cmapCells[gr_color.black].red		= (unsigned short) 0;
	gr_color.cmapCells[gr_color.black].green	= (unsigned short) 0;
	gr_color.cmapCells[gr_color.black].blue		= (unsigned short) 0;

	gr_color.cmapCells[gr_color.white].red		= (unsigned short) 65535;
	gr_color.cmapCells[gr_color.white].green	= (unsigned short) 65535;
	gr_color.cmapCells[gr_color.white].blue		= (unsigned short) 65535;
}


/*
 *	Split a palette into 3 parts
 */
void
gr_PaletteSplit()
{
	int	x,i,s1,s2;
	unsigned char *p=gr_colorSplit.palette;

	/* Pull every third color off full palette */
	i=0;
	for (x = 0; x<gr_colorSplit.maxColors; x+=3)
	{
		gr_origCells[i]=gr_color.cmapCells[x];
		gr_origCells[i].pixel = i;
		i++;
	}

	gr_origCells[gr_colorSplit.black].red   = (unsigned short)0;
	gr_origCells[gr_colorSplit.black].green = (unsigned short)0;
	gr_origCells[gr_colorSplit.black].blue  = (unsigned short)0;

	gr_origCells[gr_colorSplit.white].red   = (unsigned short)65535;
	gr_origCells[gr_colorSplit.white].green = (unsigned short)65535;
	gr_origCells[gr_colorSplit.white].blue  = (unsigned short)65535;

	s1 = gr_colorSplit.nColors+(2*RESERVED_COLORS);
	s2 = 2*(gr_colorSplit.nColors+(2*RESERVED_COLORS));
	for (x = 0; x < s1; x++)
	{
		gr_colorSplit.cmapCells[x]=gr_origCells[x];
		*p++ = gr_colorSplit.cmapCells[x].red >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].green >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].blue >> gr_colorSplit.shiftbits;
	}

	i = 0;
	for (x = s1; x < s2; x++)
	{
		gr_colorSplit.cmapCells[x] = gr_origCells[i];
		gr_colorSplit.cmapCells[x].pixel = x;
		*p++ = gr_colorSplit.cmapCells[x].red >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].green >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].blue >> gr_colorSplit.shiftbits;
		i++;
	}

	i = 0;
	for (x = s2; x < gr_colorSplit.maxColors; x++)
	{
		gr_colorSplit.cmapCells[x] = gr_origCells[i];
		gr_colorSplit.cmapCells[x].pixel = x;
		*p++ = gr_colorSplit.cmapCells[x].red >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].green >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].blue >> gr_colorSplit.shiftbits;
		i++;
	}

	gr_colorSplit.cmapCells[gr_colorSplit.black].red   =(unsigned short)0;
	gr_colorSplit.cmapCells[gr_colorSplit.black].green =(unsigned short)0;
	gr_colorSplit.cmapCells[gr_colorSplit.black].blue  =(unsigned short)0;

	gr_colorSplit.cmapCells[gr_colorSplit.white].red   =(unsigned short)65535;
	gr_colorSplit.cmapCells[gr_colorSplit.white].green =(unsigned short)65535;
	gr_colorSplit.cmapCells[gr_colorSplit.white].blue  =(unsigned short)65535;
}


/*
 *	Shade a palette to change contrast
 */
void
gr_PaletteShade(val)
int val;
{
	int	x,i,s1,s2;
	unsigned char *p;
	register int valshift;

	s1 = gr_colorSplit.nColors+(2*RESERVED_COLORS);
	s2 = 2*(gr_colorSplit.nColors+(2*RESERVED_COLORS));
	i=RESERVED_COLORS;
	p = gr_colorSplit.palette;
	for (x = RESERVED_COLORS; x< s1; x++)
	{
		valshift = val;
		if (gr_origCells[i].red < 65535-valshift)
			gr_colorSplit.cmapCells[x].red = 
				gr_origCells[i].red + valshift;
		else
			gr_colorSplit.cmapCells[x].red = 65535;

		if (gr_origCells[i].green < 65535-valshift)
			gr_colorSplit.cmapCells[x].green = 
				gr_origCells[i].green + valshift;
		else
			gr_colorSplit.cmapCells[x].red = 65535;

		if (gr_origCells[i].blue < 65535-valshift)
			gr_colorSplit.cmapCells[x].blue = 
				gr_origCells[i].blue + valshift;
		else
			gr_colorSplit.cmapCells[x].blue = 65535;

		*p++ = gr_colorSplit.cmapCells[x].red >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].green >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].blue >> gr_colorSplit.shiftbits;
		i++;
	}
	i=RESERVED_COLORS;
	for (x = RESERVED_COLORS+s1; x < s2; x++)
	{
		valshift = val;
		if (gr_origCells[i].red > valshift)
			gr_colorSplit.cmapCells[x].red =
				gr_origCells[i].red - valshift;
		else
			gr_colorSplit.cmapCells[x].green = 0;

		if (gr_origCells[i].green > valshift)
			gr_colorSplit.cmapCells[x].green = 
				gr_origCells[i].green - valshift;
		else
			gr_colorSplit.cmapCells[x].red = 0;

		if (gr_origCells[i].blue > valshift)
			gr_colorSplit.cmapCells[x].blue = 
				gr_origCells[i].blue - valshift;
		else
			gr_colorSplit.cmapCells[x].blue = 0;

		*p++ = gr_colorSplit.cmapCells[x].red >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].green >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].blue >> gr_colorSplit.shiftbits;
		i++;
	}
	i=RESERVED_COLORS;
	for (x = RESERVED_COLORS+s2; x < gr_colorSplit.maxColors; x++)
	{
		valshift = val+val+val;
		if (gr_origCells[i].red > valshift)
			gr_colorSplit.cmapCells[x].red =
				gr_origCells[i].red - valshift;
		else
			gr_colorSplit.cmapCells[x].red = 0;

		if (gr_origCells[i].green > valshift)
			gr_colorSplit.cmapCells[x].green = 
				gr_origCells[i].green - valshift;
		else
			gr_colorSplit.cmapCells[x].red = 0;

		if (gr_origCells[i].blue > valshift)
			gr_colorSplit.cmapCells[x].blue = 
				gr_origCells[i].blue - valshift;
		else
			gr_colorSplit.cmapCells[x].blue = 0;

		*p++ = gr_colorSplit.cmapCells[x].red >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].green >> gr_colorSplit.shiftbits;
		*p++ = gr_colorSplit.cmapCells[x].blue >> gr_colorSplit.shiftbits;
		i++;
	}
	gr_colorSplit.cmapCells[gr_colorSplit.black].red   =(unsigned short)0;
	gr_colorSplit.cmapCells[gr_colorSplit.black].green =(unsigned short)0;
	gr_colorSplit.cmapCells[gr_colorSplit.black].blue  =(unsigned short)0;

	gr_colorSplit.cmapCells[gr_colorSplit.white].red   =(unsigned short)65535;
	gr_colorSplit.cmapCells[gr_colorSplit.white].green =(unsigned short)65535;
	gr_colorSplit.cmapCells[gr_colorSplit.white].blue  =(unsigned short)65535;
}


/*
 *	Set color map of widget to a split palette
 */
void
gr_ImageSetCMapSplit(wid)
Widget wid;
{
	Display *dpy = XtDisplay(wid);
	Window 	win = XtWindow(wid);
	int 	scr = DefaultScreen(dpy);
	Visual  *vis = gr_GetVisual(dpy,scr);
	XSetWindowAttributes attrib;
	Colormap  cmap;

	if (gr_colorSplit.canSet == FALSE)
		return;

	cmap = XCreateColormap(dpy,win,vis,AllocAll);

	XStoreColors(dpy,cmap,gr_colorSplit.cmapCells,256);

#ifndef OPEN_WINDOWS 
#ifndef IRIS
	attrib.colormap = cmap;
	XChangeWindowAttributes(dpy,win,CWColormap,&attrib);
#endif
#endif

	XInstallColormap(dpy,cmap);

	return;
}


/*
 *	Set color map of widget to a normal palette
 */
void
gr_ImageSetCMap(wid)
Widget wid;
{
	Display *dpy = XtDisplay(wid);
	Window 	win = XtWindow(wid);
	int 	scr = DefaultScreen(dpy);
	Visual  *vis = gr_GetVisual(dpy,scr);
	XSetWindowAttributes attrib;
	Colormap  cmap;

	if (gr_color.canSet == FALSE)
		return;

	cmap = XCreateColormap(dpy,win,vis,AllocAll);

	XStoreColors(dpy,cmap,gr_color.cmapCells,255);

#ifndef OPEN_WINDOWS
#ifndef IRIS
	attrib.colormap = cmap;
	XChangeWindowAttributes(dpy,win,CWColormap,&attrib);
#endif
#endif

	XInstallColormap(dpy,cmap);


	return;
}


/*
 *	Set color map of widget to default gray-scaled palette
 */
void
gr_ImageDefCMap()
{
	unsigned char r[256],g[256],b[256];
	int	x;

	for (x=0; x<gr_color.maxColors; x++)
		r[x] = g[x] = b[x] = (unsigned char)x;

	gr_ImageInitCMapRGB(r,g,b);
	gr_PaletteSplit();
}


/*
 *	Load color map
 */
void
gr_PalletLoad(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	Widget				shell;
	A_BossWind_t		*bossWin=gr_topWin.bossWin;
	A_FileWind_t		*fileWin=(A_FileWind_t *)client_data;
	A_AniWind_t			*curAni;
	A_TileWind_t		*curTile;
	A_DsplWind_t		*curDspl;
	A_SubsWind_t		*curSubs;
	A_ViewWind_t		*curView;
	char				*strng;
	unsigned char		rbuff[256];
	unsigned char		gbuff[256];
	unsigned char		bbuff[256];
	char				errbuff[260];
	char				pfile[80];
	int					fd,index;
	extern				errno;

	strng = gr_DialogGetValue(fileWin->fileDialog);
	sprintf(pfile,"%s/%s",td_getDirName(),strng);

	if (td_FileIsHdf(pfile))
	{
		if (td_HdfPalLoad(pfile,gr_color.palette) == -1)
		{
			sprintf(errbuff,"Cannot open Hdf palette %s. ->%d\n",strng,errno);
			gr_TextMsgOut(errbuff); return;
		}
		else
		{
			gr_ImageInitCMapHDF(gr_color.palette);
			sprintf(errbuff,"Loaded HDF palette %s.\n",strng);
			gr_TextMsgOut(errbuff);
		}
	}
	else
	{
		if ((fd = open(pfile,O_RDONLY,NULL)) < 0)
		{
			sprintf(errbuff,"Cannot open palette %s. ->%d\n",strng,errno);
			gr_TextMsgOut(errbuff); return;
		}
		if (read(fd,rbuff,256) != 256)
		{
			sprintf(errbuff,"%s is not a seq colormap\n",strng);
    		(void) close(fd); gr_TextMsgOut(errbuff); return;
    	}
    	if (read(fd,gbuff,256) != 256)
		{
			sprintf(errbuff,"%s is not a seq colormap\n",strng);
    		(void) close(fd); gr_TextMsgOut(errbuff); return;
		}
		if (read(fd,bbuff,256) != 256)
		{
			sprintf(errbuff,"%s is not a seq colormap\n",strng);
    		(void) close(fd); gr_TextMsgOut(errbuff); return;
		}
    	(void) close(fd);
		gr_ImageInitCMapRGB(rbuff,gbuff,bbuff);
		sprintf(errbuff,"Loaded SEQ palette %s.\n",strng);
		gr_TextMsgOut(errbuff);
	}

  if (fileWin->globalPalette == TRUE)
  {

	curAni = gr_topWin.aniWin;
	while (curAni != NULL)
	{
		gr_ImageSetCMap(curAni->shell);
		curAni = curAni->next;
	}

	while (bossWin != NULL)
	{
		if (bossWin->cubeWin != NULL)
		{
			curDspl = bossWin->cubeWin->dsplWin;
			while (curDspl != NULL)
			{
				gr_ImageSetCMap(curDspl->shell);
				curDspl = curDspl->next;
			}

			curTile = bossWin->cubeWin->tileWin;
			while (curTile != NULL)
			{
				gr_ImageSetCMap(curTile->shell);
				curTile = curTile->next;
			}

			curAni = bossWin->cubeWin->aniWin;
			while (curAni != NULL)
			{
				gr_ImageSetCMap(curAni->shell);
				curAni = curAni->next;
			}

			curSubs = bossWin->cubeWin->subsWin;
			while (curSubs != NULL)
			{
				gr_ImageSetCMap(curSubs->shell);
				curSubs = curSubs->next;
			}
		}
		if (bossWin->arbWin != NULL)
		{
			curView = bossWin->arbWin->viewWin;
			while (curView != NULL)
			{
				gr_ImageSetCMap(curView->shell);
				curView = curView->next;
			}

			curAni = bossWin->arbWin->aniWin;
			while (curAni != NULL)
			{
				gr_ImageSetCMap(curAni->shell);
				curAni = curAni->next;
			}
		}

		bossWin = bossWin->next;
	}
  }
  else
	gr_ImageSetCMap(fileWin->callShell);

	gr_CloseFileLevel(w,client_data,call_data);

}


/*
 * Load and split a color map
 */
void
gr_PalletLoadSplit(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	Widget				shell;
	A_BossWind_t		*bossWin=gr_topWin.bossWin;
	A_FileWind_t		*fileWin=(A_FileWind_t *)client_data;
	A_AniWind_t			*curAni;
	A_TileWind_t		*curTile;
	A_DsplWind_t		*curDspl;
	A_ViewWind_t		*curView;
	char				*strng;
	unsigned char		rbuff[256];
	unsigned char		gbuff[256];
	unsigned char		bbuff[256];
	char				errbuff[260];
	char				pfile[80];
	int					fd,index;
	extern				errno;

	strng = gr_DialogGetValue(fileWin->fileDialog);
	sprintf(pfile,"%s/%s",td_getDirName(),strng);

	if (td_FileIsHdf(pfile))
	{
		if (td_HdfPalLoad(pfile,gr_color.palette) == -1)
		{
			sprintf(errbuff,"Cannot open Hdf palette %s. ->%d\n",strng,errno);
			gr_TextMsgOut(errbuff); return;
		}
		else
		{
			gr_ImageInitCMapHDF(gr_color.palette);
			sprintf(errbuff,"Loaded HDF palette %s.\n",strng);
			gr_TextMsgOut(errbuff);
		}
	}
	else
	{
		if ((fd = open(pfile,O_RDONLY,NULL)) < 0)
		{
			sprintf(errbuff,"Cannot open palette %s. ->%d\n",strng,errno);
			gr_TextMsgOut(errbuff); return;
		}
		if (read(fd,rbuff,256) != 256)
		{
			sprintf(errbuff,"%s is not a seq colormap\n",strng);
    		(void) close(fd); gr_TextMsgOut(errbuff); return;
    	}
    	if (read(fd,gbuff,256) != 256)
		{
			sprintf(errbuff,"%s is not a seq colormap\n",strng);
    		(void) close(fd); gr_TextMsgOut(errbuff); return;
		}
		if (read(fd,bbuff,256) != 256)
		{
			sprintf(errbuff,"%s is not a seq colormap\n",strng);
    		(void) close(fd); gr_TextMsgOut(errbuff); return;
		}
    	(void) close(fd);
		gr_ImageInitCMapRGB(rbuff,gbuff,bbuff);
		sprintf(errbuff,"Loaded SEQ palette %s.\n",strng);
		gr_TextMsgOut(errbuff);
	}

	gr_PaletteSplit();
	gr_ImageSetCMapSplit(fileWin->callShell);

	gr_CloseFileLevel(w,client_data,call_data);
}
