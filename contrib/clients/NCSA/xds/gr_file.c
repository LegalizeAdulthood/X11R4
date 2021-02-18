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
 *	File:		gr_file.c
 *	Contents:	File manipulation routines
 */

#include "gr_com.h"

static	char	tstrng[STRNG160];
static	char	gr_DirName[STRNG160];

void
gr_FileInit()
{
	gr_topWin.fileWin->aniWin = NULL;
	gr_topWin.fileWin->arbWin = NULL;
	gr_topWin.fileWin->cubeWin = NULL;
	gr_topWin.fileWin->diceWin = NULL;
	gr_topWin.fileWin->dsplWin = NULL;
	gr_topWin.fileWin->isoWin = NULL;
	gr_topWin.fileWin->subsWin = NULL;
	gr_topWin.fileWin->tileWin = NULL;
	gr_topWin.fileWin->viewWin = NULL;
}


/*
 *	Set load all SDS toggle to true
 */
void
gr_FileSetLoadAll(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->loadAllSDS = TRUE;
}


/*
 *	Set load all SDS toggle to false
 */
void
gr_FileSetLoadSingle(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->loadAllSDS = FALSE;
}


/*
 *	Set save separate files toggle to true
 */
void
gr_FileSetSeparate(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->separateFiles = TRUE;
}


/*
 *	Set save separate files toggle to false
 */
void
gr_FileSetSingle(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->separateFiles = FALSE;
}


/*
 *	Set use HDF SDS format toggle to true
 */
void
gr_FileUseHDF(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->format = HDF;
}


/*
 *	Set use Miriad format toggle to true
 */
void
gr_FileUseMiriad(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->format = MIRIAD;
}


/*
 *	Set load global palette toggle to true
 */
void
gr_FileSetGlobal(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->globalPalette = TRUE;
}


/*
 *	Set load global palette toggle to false
 */
void
gr_FileSetLocal(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->globalPalette = FALSE;
}


/*
 *	Set use disk toggle to true
 */
void
gr_FileUseDisk(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->useDisk = TRUE;
}


/*
 *	Set use disk toggle to false
 */
void
gr_FileUseMemory(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->useDisk = FALSE;
}


/*
 *	Set use pixmap toggle to true
 */
void
gr_FileUsePixmap(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
/* For some obscure reasons, we cannot use pixmaps for animation on the IRIS */
#ifdef IRIS
	((A_FileWind_t *)client_data)->usePixmap = FALSE;
#else
	((A_FileWind_t *)client_data)->usePixmap = TRUE;
#endif
}


/*
 *	Set use pixmap toggle to false
 */
void
gr_FileUseXImage(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	((A_FileWind_t *)client_data)->usePixmap = FALSE;
}


/*
 *	Change working directory
 */
void
gr_FileDirAccept(fileWin,dirName)
A_FileWind_t	*fileWin;
char	*dirName;
{
	char	**dirStrings;
	char 	strng[300];

	gr_WidgetCursor(fileWin->shell,XC_watch);

	dirStrings = td_FileDirStrings(dirName);
	if (dirStrings == NULL)
	{
		sprintf(strng,
		"Error: Cannot open directory %s\nCurrent directory is %s\n",
		dirName,td_getDirName());
		gr_DialogSetValue(fileWin->dirDialog,td_getDirName());
		gr_TextMsgOut(strng);
	}
	else
	{
		gr_ListChange(fileWin->fileWin,dirStrings,0,0,TRUE);
		td_setDirName(dirName);
		gr_DialogSetValue(fileWin->dirDialog,dirName);
		strncpy(gr_FileName,dirStrings[0],strlen(dirStrings[0])-1);
		gr_FileName[strlen(dirStrings[0])-1] = '\0';
		gr_DialogSetValue(fileWin->fileDialog,gr_FileName);
	}

	gr_WidgetCursor(fileWin->shell,XC_draped_box);
}


/*
 *	Front-end function for processing return key on File Directory Dialog
 */
void
gr_FileDirReturn(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
int num_params;
{
	gr_FileDirAccept(gr_topWin.fileWin,
		gr_DialogGetValue(gr_topWin.fileWin->dirDialog));
}


/*
 *	Move up directory tree
 */
void
gr_FileDirUp(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t	*fileWin=(A_FileWind_t *)client_data;
	int i,len;

	strcpy(gr_DirName,td_getDirName());
	len = strlen(gr_DirName);
	for (i=len-1;i>0;i--)
		if ((i != len-1) && (gr_DirName[i] == '/'))
		{
			gr_DirName[i]='\0';
			gr_FileDirAccept(fileWin,gr_DirName);
			return;
		}
}


/*
 *	Move down directory tree
 */
void
gr_FileDirDown(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t	*fileWin=(A_FileWind_t *)client_data;

	sprintf(gr_DirName,"%s/%s",td_getDirName(),
		gr_DialogGetValue(fileWin->fileDialog));

	gr_FileDirAccept(fileWin,gr_DirName);
}


/*
 *	Check selected file type
 */
void
gr_FileCheck(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t	*fileWin=(A_FileWind_t *)client_data;
	int	 len,ret,index,xsize,ysize,ispal;

	strcpy(tstrng,gr_ListgetStruct(fileWin->fileWin,&index));
	len = strlen(tstrng)-1;
	strncpy(gr_FileName,tstrng,len);
	gr_FileName[len]='\0';
	gr_DialogSetValue(fileWin->fileDialog,gr_FileName);
	ret = td_HdfCheck(gr_FileName);
	if (ret != -1)
	{
		sprintf(msg,"File %s has %d SDS.\n",gr_FileName,ret);
		gr_TextMsgOut(msg);
		if ((ret >= 1) && 
			((fileWin->mode == LOADSDS) || (fileWin->mode == SAVEPROC)) )
			gr_DialogSetValue(fileWin->sdsDialog,"1");
	}
	else
	{
		sprintf(msg,"%s/%s",td_getDirName(),gr_FileName);
		ret = td_HdfgetRasDims(msg,&xsize,&ysize,&ispal);
		if (ret != -1)
		{
			sprintf(msg,"File %s has %d Raster8 frames.\n",gr_FileName,ret);
			gr_TextMsgOut(msg);
			if (fileWin->mode == LOADRAS)
			{
				sprintf(msg,"%d",ret-1);
				gr_DialogSetValue(fileWin->rasStartDialog,"0");
				gr_DialogSetValue(fileWin->rasEndDialog,msg);
			}
		}
		else
			if (fileWin->mode == LOADRAS)
			{
				gr_DialogSetValue(fileWin->rasStartDialog,"-");
				gr_DialogSetValue(fileWin->rasEndDialog,"-");
			}
		if ((fileWin->mode == LOADSDS) || (fileWin->mode == SAVEPROC))
			gr_DialogSetValue(fileWin->sdsDialog,"-");
	}
}


/*
 *	Load 3D SDS file and open a Boss Window
 */
void
gr_FileLoad(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_BossWind_t	*tmp=gr_topWin.bossWin;
	A_FileWind_t	*fileWin=(A_FileWind_t *)client_data;
	int i,ret,index,num;

	strcpy(tstrng, gr_DialogGetValue(fileWin->fileDialog));

	ret = td_HdfCheck(tstrng);

	if ((ret != -1) && (fileWin->format == HDF))
	{
		gr_WidgetCursor(fileWin->shell,XC_watch);
		if (fileWin->loadAllSDS == FALSE)
		{
			num = atoi(gr_DialogGetValue(fileWin->sdsDialog));
			gr_CloseFileLevel(w,client_data, call_data);
			if (gr_topWin.numBossWins < MAX_BOSLEV_WINDS)
			{
				tmp = 
				 gr_InitBossLevel(tmp,tstrng,gr_topLevel,num,&gr_topWin,HDF);

				if (tmp != NULL)
				{
					gr_topWin.bossWin = tmp;
					gr_topWin.numBossWins++;
				}
			}
			else
			{
				sprintf(msg,"Only %d Attributes window is allowed!\n",
					MAX_BOSLEV_WINDS);
				gr_TextMsgOut(msg);
			}
		}
		else
		{
			gr_CloseFileLevel(w,client_data, call_data);

			for (i=1;i<=ret;i++)
			if (gr_topWin.numBossWins < MAX_BOSLEV_WINDS)
			{
				tmp = 
				 gr_InitBossLevel(tmp,tstrng,gr_topLevel,i,&gr_topWin,HDF);

				if (tmp != NULL)
				{
					gr_topWin.bossWin = tmp;
					gr_topWin.numBossWins++;
				}
			}
			else
			{
				sprintf(msg,"Only %d Attributes window is allowed!\n",
					MAX_BOSLEV_WINDS);
				gr_TextMsgOut(msg);
			}
		}
	}
	else
	if (fileWin->format == MIRIAD)
	{
		if (gr_topWin.numBossWins < MAX_BOSLEV_WINDS)
		{
			gr_CloseFileLevel(w,client_data, call_data);

			tmp =
			 gr_InitBossLevel(tmp,tstrng,gr_topLevel,i,&gr_topWin,MIRIAD);

			if (tmp != NULL)
			{
				gr_topWin.bossWin = tmp;
				gr_topWin.numBossWins++;
			}
		}
		else
		{
			sprintf(msg,"Only %d Attributes window is allowed!\n",
				MAX_BOSLEV_WINDS);
			gr_TextMsgOut(msg);
		}
	}
	else
	{
		sprintf(msg,"Cannot load file %s.\n",tstrng);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open an Animation window for saved Raster images
 */
void
gr_LoadRASTER(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t	*tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,LOADRAS);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = LOADRAS;
			gr_FileInit();
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a Load SDS window
 */
void
gr_LoadSDS(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t	*tmp = gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,LOADSDS);
		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = LOADSDS;
			gr_FileInit();
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a split palette window
 */
void
gr_LoadPALSplit(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DiceWind_t	*diceWin=(A_DiceWind_t *)client_data;
	A_FileWind_t	*tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,LOADPALSPLIT);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = LOADPALSPLIT;
			gr_topWin.fileWin->callShell = (Widget)diceWin->shell;
			gr_FileInit();
        	gr_topWin.numFileWins++;
			gr_SliderSetValue(diceWin->shadeSlider,0);
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a  palette window
 */
void
gr_LoadPAL(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t	*tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,LOADPAL);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = LOADPAL;
			gr_topWin.fileWin->callShell = (Widget)client_data;
			gr_FileInit();
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save plane animation in Disk window
 */
void
gr_SaveDiskAAni(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ArbWind_t *arbWin=(A_ArbWind_t	*)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEDISKA);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEDISKA;
			gr_FileInit();
			gr_topWin.fileWin->arbWin = arbWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save plane animation in Disk window
 */
void
gr_SaveDiskPAni(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_CubeWind_t *cubeWin=(A_CubeWind_t	*)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEDISKP);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEDISKP;
			gr_FileInit();
			gr_topWin.fileWin->cubeWin = cubeWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save animation window
 */
void
gr_SaveAni(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_AniWind_t *aniWin=(A_AniWind_t	*)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEANI);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEANI;
			gr_FileInit();
			gr_topWin.fileWin->aniWin = aniWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save single plane window
 */
void
gr_SaveDspl(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DsplWind_t *dsplWin=(A_DsplWind_t *)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEDSPL);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEDSPL;
			gr_FileInit();
			gr_topWin.fileWin->dsplWin = dsplWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save single plane window
 */
void
gr_SaveIso(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_IsoWind_t	*isoWin=(A_IsoWind_t *)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEISO);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEISO;
			gr_FileInit();
			gr_topWin.fileWin->isoWin = isoWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save processed SDS window
 */
void
gr_SaveProc(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEPROC);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEPROC;
			gr_FileInit();
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save frame window
 */
void
gr_SaveTile(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_TileWind_t *tileWin=(A_TileWind_t *)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVETILE);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVETILE;
			gr_FileInit();
			gr_topWin.fileWin->tileWin = tileWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save dicer window
 */
void
gr_SaveDicer(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_DiceWind_t *diceWin=(A_DiceWind_t *)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEDICER);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEDICER;
			gr_FileInit();
			gr_topWin.fileWin->diceWin = diceWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save arbitrary view slice window
 */
void
gr_SaveView(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_ViewWind_t *viewWin=(A_ViewWind_t *)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEVIEW);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEVIEW;
			gr_FileInit();
			gr_topWin.fileWin->viewWin = viewWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Open a save vbuffer window window
 */
void
gr_SaveVbuff(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_SubsWind_t *subsWin=(A_SubsWind_t *)client_data;
	A_FileWind_t *tmp=gr_topWin.fileWin;

    if (gr_topWin.numFileWins < MAX_FILLEV_WINDS)
    {
        tmp = gr_InitFileLevel(tmp,"XDSwindow",gr_topLevel,SAVEVBUFF);

		if (tmp != NULL)
		{
			gr_topWin.fileWin = tmp;
			gr_topWin.fileWin->mode = SAVEVBUFF;
			gr_FileInit();
			gr_topWin.fileWin->subsWin = subsWin;
        	gr_topWin.numFileWins++;
		}
    }
    else
	{
		sprintf(msg,"Only %d File window is allowed!\n",
			MAX_FILLEV_WINDS);
		gr_TextMsgOut(msg);
	}
}


/*
 *	Perform Save operation
 */
void
gr_FileSave(w, client_data, call_data)
Widget w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t	*fileWin=(A_FileWind_t *)client_data;
	A_AniWind_t		*aniWin;
	A_ArbWind_t		*arbWin;
	A_CubeWind_t	*cubeWin;
	A_Box_t			*box;
	char	*strng,pathname[STRNG160],savename[STRNG160];
	char	*fileName,*dirName;
	char	**dirStrings;
	int		i=0,index;
	int		numframes=0,fileexist,ret,num,scale;
    int     maxplanes,plane,max,numplanes,inc;
	double	zincr;

	fileName = gr_DialogGetValue(fileWin->fileDialog);

	if (fileName == NULL)
	{
        gr_TextMsgOut("Please enter a filename!\n");
		return;
	}
	if (strcmp(fileName," ")==0)
	{
        gr_TextMsgOut("Please input a proper filename!\n");
		return;
	}
	else
	{
		dirName = td_getDirName();
		sprintf(pathname,"%s/%s",dirName,fileName);
		fileexist = td_FileExist(pathname);
		switch(fileWin->mode)
		{
			case LOADSDS:
			case LOADPAL:
				fprintf(stderr,"Error\t: Load condition encountered!\n");
				exit(-1);
				break;
			case SAVEANI:
				numframes = fileWin->aniWin->numplanes;
				for (i=0;i<numframes;i++)
				{
					if (fileWin->separateFiles == FALSE)
						sprintf(savename,"%s",pathname);
					else
						sprintf(savename,"%s%04d",pathname,i);

					fileexist = td_FileExist(savename);
					if (fileexist == 0)
						ret = td_HdfPutImage(savename,fileWin->aniWin->data[i],
							gr_color.palette,
							fileWin->aniWin->imagexsize[i],
							fileWin->aniWin->imageysize[i]);
					else
						ret = td_HdfAddImage(savename,fileWin->aniWin->data[i],
							fileWin->aniWin->imagexsize[i],
							fileWin->aniWin->imageysize[i]);

					if (ret == -1)
						gr_TextMsgOut("Error in saving Animation window!\n");
					else
					if (fileWin->separateFiles == TRUE)
					{
						sprintf(msg,"Saved frame %d in %s.\n",i,savename);
						gr_TextMsgOut(msg);
					}
				}
				break;
			case SAVEDICER:
				numframes = 1;
				if (fileexist == 0)
				{
					ret = td_HdfPutImage(pathname,
						fileWin->diceWin->data[fileWin->diceWin->dimysize-1],
						gr_colorSplit.palette,
						fileWin->diceWin->dimxsize,fileWin->diceWin->dimysize);
				}
				else
				{
					ret = td_HdfAddImage(pathname,
						fileWin->diceWin->data[fileWin->diceWin->dimysize-1],
						fileWin->diceWin->dimxsize,fileWin->diceWin->dimysize);
				}
				if (ret == -1)
					gr_TextMsgOut("Error in saving Dicer window!\n");
				break;
			case SAVEDISKA:
				arbWin = fileWin->arbWin;
				aniWin = arbWin->aniWin;
				scale = gr_ArbgetCurScale(arbWin);
				numframes = gr_ArbgetCurNumFrames(arbWin);
				zincr = gr_ArbgetCurZPercent(arbWin);
				aniWin = gr_InitAniLevel3(aniWin,"XDSanimate",gr_topLevel,
					numframes,zincr,scale,arbWin,pathname);
				arbWin->aniWin = aniWin;
				arbWin->numAniWins++;
				break;
			case SAVEDISKP:
				cubeWin=fileWin->cubeWin;
				aniWin=cubeWin->aniWin;
    			plane = cubeWin->axesOrient.plane = gr_CubegetCurPlane(cubeWin);
    			inc     = gr_CubegetCurIncr(cubeWin);
    			scale   = gr_CubegetCurScale(cubeWin);
    			numplanes = gr_CubegetCurNumPlanes(cubeWin);
    			if (inc > 0)
    			{
        			max = td_HdfgetDim(cubeWin->parent->data,
						cubeWin->axesOrient.axis);
        			maxplanes = (int)gr_rint((double)((max-plane)/inc));
    			}   
    			else
        			maxplanes = (int)gr_rint((double)(plane/(inc*-1)))+1;
    			if ( maxplanes < numplanes)
    			{
        			sprintf(msg,"Warning: Only %d planes can be obtained\n",
						maxplanes);
        			gr_TextMsgOut(msg);
        			numplanes = maxplanes;
    			}   

				aniWin = gr_InitAniLevel(aniWin,"XDSanimate",gr_topLevel,
					numplanes,inc,scale,cubeWin->axesOrient,cubeWin,pathname);
				cubeWin->aniWin = aniWin;
				cubeWin->numAniWins++;
				numframes = numplanes;
				break;
			case SAVEDSPL:
				numframes = 1;
				if (fileexist == 0)
					ret = td_HdfPutImage(pathname,fileWin->dsplWin->data,
						gr_color.palette,
						fileWin->dsplWin->imagexsize,
						fileWin->dsplWin->imageysize);
				else
					ret = td_HdfAddImage(pathname,fileWin->dsplWin->data,
						fileWin->dsplWin->imagexsize,
						fileWin->dsplWin->imageysize);
				if (ret == -1)
					gr_TextMsgOut("Error in saving Display Window!\n");
				break;
			case SAVEISO:
				numframes = 1;
				if (fileexist == 0)
					ret = td_HdfPutImage(pathname,
						fileWin->isoWin->data[fileWin->isoWin->ysize-1],
						gr_color.palette,
						fileWin->isoWin->xsize,fileWin->isoWin->ysize);
				else
					ret = td_HdfAddImage(pathname,
						fileWin->isoWin->data[fileWin->isoWin->ysize-1],
						fileWin->isoWin->xsize,fileWin->isoWin->ysize);
				if (ret == -1)
					gr_TextMsgOut("Error in saving Iso-surface Window!\n");
				break;
			case SAVEPROC:
				strng = gr_DialogGetValue(fileWin->fileDialog);
				num = atoi(gr_DialogGetValue(fileWin->sdsDialog));
				scale = atoi(gr_DialogGetValue(fileWin->scaleDialog));
				gr_ProcSDS(num,scale,strng,fileName,fileWin);
				break;
			case SAVETILE:
				numframes = fileWin->tileWin->numTiles;
				if (fileexist == 0)
					ret = td_HdfPutImage(pathname,fileWin->tileWin->data[0],
						gr_color.palette,
						fileWin->tileWin->imagexsize[0],
						fileWin->tileWin->imageysize[0]);
				else
					ret = td_HdfAddImage(pathname,fileWin->tileWin->data[0],
						fileWin->tileWin->imagexsize[0],
						fileWin->tileWin->imageysize[0]);
				for (i=1;i<numframes;i++)
				{
					ret = td_HdfAddImage(pathname,fileWin->tileWin->data[i],
						fileWin->tileWin->imagexsize[i],
						fileWin->tileWin->imageysize[i]);
					if (ret == -1)
						gr_TextMsgOut("Error in saving Tile window!\n");
				}
				break;
			case SAVEVIEW:
				numframes = 1;
				box = fileWin->viewWin->parent->xybox;
				if (fileexist == 0)
					ret = td_HdfPutImage(pathname,fileWin->viewWin->data,
						gr_color.palette,
						box->xpicsize,box->ypicsize);
				else
					ret = td_HdfAddImage(pathname,fileWin->viewWin->data,
						box->xpicsize,box->ypicsize);
				if (ret == -1)
					gr_TextMsgOut("Error in saving View window!\n");
				break;
			case SAVEVBUFF:
				numframes = 1;
				if (fileexist == 0)
				{
					ret = td_HdfPutImage(pathname,fileWin->subsWin->data,
						gr_color.palette,
						fileWin->subsWin->xsize,fileWin->subsWin->ysize);
				}
				else
				{
					ret = td_HdfAddImage(pathname,fileWin->subsWin->data,
						fileWin->subsWin->xsize,fileWin->subsWin->ysize);
				}
				if (ret == -1)
					gr_TextMsgOut("Error in saving Substance window!\n");
				break;
		}

	    if ((fileWin->mode != SAVEPROC) && (ret != -1))
		{
		if (numframes == 1)
			sprintf(msg,"%d frame",numframes);
		else
			sprintf(msg,"%d frames",numframes);

		if (fileWin->separateFiles == FALSE)
		{
			if (fileexist == 0)
				sprintf(msg,"%s saved in file %s\n",msg,fileName);
			else
				sprintf(msg,"%s added to file %s\n",msg,fileName);
			gr_TextMsgOut(msg);
		}
		}
	}

	gr_CloseFileLevel(w,client_data,call_data);
}


/*
 *	Front-end function for processing return key on Save File Dialog
 */
void
gr_FileSaveReturn(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
int num_params;
{
	gr_FileSave(w,(caddr_t)gr_topWin.fileWin,NULL);
}
