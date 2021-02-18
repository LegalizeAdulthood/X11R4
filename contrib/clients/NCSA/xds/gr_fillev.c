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
 *	File:		gr_fillev.c
 *	Contents:	File level window functions for graphics module
 */

#include "gr_com.h"


/*
 *	Return a File Window corresponding to fileaction
 */
A_FileWind_t
*gr_InitFileLevel(header,shellName,parent,fileaction)
A_FileWind_t	*header;
char *shellName;
Widget parent;
A_FileAction_t	fileaction;
{
	A_FileWind_t *tmp;
	Widget		boxWind,RCWind;
	char		label[STRNG40];

	gr_WidgetCursor(gr_topLevel,XC_watch);

	switch (fileaction)
	{
		case LOADRAS :
		gr_TextMsgOut
		("\nSelect a HDF RASTER8 file and click the ANIMATE RASTER8 button\n");
		gr_TextMsgOut
		("to load all frames into memory for animation.\n");
		sprintf(label,"Load a RASTER8 set");
		break;

		case LOADSDS :
		gr_TextMsgOut
		("\nSelect a 3D SDS file, and input the ID of the SDS in the\n");
		gr_TextMsgOut
		("file that you wish to load into memory.  If you wish to load \n");
		gr_TextMsgOut
		("all the SDSs in the file, turn on the Load All SDSs toggle.\n");
		gr_TextMsgOut
		("Click on the LOAD SDS button to load the dataset/s.\n");
		sprintf(label,"Load a SCIENTIFIC set");
		break;

		case LOADPAL :
		gr_TextMsgOut
		("\nSelect a HDF/SEQ palette file, and click on the LOAD PALETTE\n");
		gr_TextMsgOut
		("button to change the color map.  If the Global Change toggle is\n");
		gr_TextMsgOut
		("switched on, the color map will be loaded into all previously\n");
		gr_TextMsgOut
		("opened raster image windows.\n");
		sprintf(label,"Load a color map");
		break;

		case LOADPALSPLIT :
		gr_TextMsgOut
		("\nSelect a HDF/SEQ palette file, and click on the LOAD PALETTE\n");
		gr_TextMsgOut
		("button to change the color map.  If the Global Change toggle is\n");
		gr_TextMsgOut
		("switched on, the color map will be loaded into all previously\n");
		gr_TextMsgOut
		("opened image windows.  Note that the palette will be split into\n");
		gr_TextMsgOut
		("3 equal portions for use by the Dicer Window.\n");
		sprintf(label,"Split a color map");
		break;

		case SAVEANI :
		gr_TextMsgOut
		("\nInput the filename of the HDF file for storing the animation\n");
		gr_TextMsgOut
		("sequence and click on the Save Button to save the sequence of\n");
		gr_TextMsgOut
		("of images.  You may also select a filename out of the File List\n");
		gr_TextMsgOut
		("so to append the sequence to the contents of the file.\n");
		sprintf(label,"Save animation frames");
		break;

		case SAVEDICER :
		gr_TextMsgOut
		("\nInput the filename of the HDF file for storing the Dicer image\n");
		gr_TextMsgOut
		("and click on the Save Button to save the image.  You may also\n");
		gr_TextMsgOut
		("select a filename out of the File List to append the image to\n");
		gr_TextMsgOut
		("the end of the contents of the file.\n");
		sprintf(label,"Save a Dicer image");
		break;

		case SAVEDISKA :
		case SAVEDISKP :
		gr_TextMsgOut
		("\nInput the filename of the HDF file for storing the Animation\n");
		gr_TextMsgOut
		("sequence and click on the Save Button to save the image.  You\n");
		gr_TextMsgOut
		("may also select a filename out of the File List to append the\n");
		gr_TextMsgOut
		("images to the end of the contents of the selected file.\n");
		sprintf(label,"Save an Animation sequence");
		break;

		case SAVEDSPL :
		gr_TextMsgOut
		("\nInput the filename of the HDF file for storing the raster\n");
		gr_TextMsgOut
		("and click on the Save Button to save the image.  You may also\n");
		gr_TextMsgOut
		("select a filename out of the File List to append the image to\n");
		gr_TextMsgOut
		("the end of the contents of the file.\n");
		sprintf(label,"Save a display frame");
		break;

		case SAVEISO :
		gr_TextMsgOut
		("\nInput the filename of the HDF file for storing the raster\n");
		gr_TextMsgOut
		("and click on the Save Button to save the image.  You may also\n");
		gr_TextMsgOut
		("select a filename out of the File List to append the image to\n");
		gr_TextMsgOut
		("the end of the contents of the file.\n");
		sprintf(label,"Save an Iso-surface Image");
		break;

		case SAVEPROC :
		gr_TextMsgOut
		("\nSelect the 3D HDF SDS file containing the dataset to be\n");
		gr_TextMsgOut
		("interpolated and input the filename of the HDF to which the\n");
		gr_TextMsgOut
		("enlarged dataset is to be stored.  Click on Save to start the\n");
		gr_TextMsgOut
		("process which will save the dataset automatically when done.\n");
		sprintf(label,"Process & save a SDS");
		break;

		case SAVETILE :
		("\nInput the filename of the HDF file for storing all the images\n");
		gr_TextMsgOut
		("and click on the Save Button to save the images.  You may also\n");
		gr_TextMsgOut
		("select a filename out of the File List to append the images to\n");
		gr_TextMsgOut
		("the end of the contents of the file.\n");
		sprintf(label,"Save tile frames");
		break;

		case SAVEVIEW :
		("\nInput the filename of the HDF file for storing the image\n");
		gr_TextMsgOut
		("and click on the Save Button to save the image.  You may also\n");
		gr_TextMsgOut
		("select a filename out of the File List to append the image to\n");
		gr_TextMsgOut
		("the end of the contents of the file.\n");
		sprintf(label,"Save an oblique frame");
		break;

		case SAVEVBUFF :
		gr_TextMsgOut
		("\nInput the filename of the HDF file for storing the VBuff image\n");
		gr_TextMsgOut
		("and click on the Save Button to save the image.  You may also\n");
		gr_TextMsgOut
		("select a filename out of the File List to append the image to\n");
		gr_TextMsgOut
		("the end of the contents of the file.\n");
		sprintf(label,"Save a VBuff image");
		break;

	}

	if ((tmp = (A_FileWind_t *)td_Malloc(sizeof(A_FileWind_t),
			   "A_FileWind_t")) == NULL)
		return(NULL);

	if ((tmp->fileData = td_FileDirStrings(td_getDirName())) == NULL)
		return(NULL);

	gr_TextMsgOut("Click Cancel to abort the File Window...\n");

	tmp->shell = gr_MakeWindow(label,parent,&(tmp->win),
					(XtCallbackProc)gr_CloseFileLevel,
					FILEWIND,label,"Cancel",(caddr_t)tmp,0,0,304,350);
	
	boxWind = gr_MakeBulletin("XDSbox",tmp->win,2,2,293,60);
		tmp->dirDialog = gr_MakeDialog("XDSdialog",boxWind,
			"Current Working Directory:          ",
			td_getDirName(),256,2,2,240,50);
		gr_DialogAddTrans(tmp->dirDialog,gr_ChangeDirTrans);
		gr_MakeButton("XDSbutton",boxWind,"Up",
			(XtCallbackProc)gr_FileDirUp,(caddr_t)tmp,
			245,2,45,24);
		gr_MakeButton("XDSbutton",boxWind,"Down",
			(XtCallbackProc)gr_FileDirDown,(caddr_t)tmp,
			245,28,45,24);

	strncpy(gr_FileName,tmp->fileData[0],strlen(tmp->fileData[0])-1);
	gr_FileName[strlen(tmp->fileData[0])-1] = '\0';
	boxWind = gr_MakeBulletin("XDSbox",tmp->win,2,62,293,60);
		tmp->fileDialog = gr_MakeDialog("XDSdialog",boxWind,
			"Current Filename:                   ",
			gr_FileName,STRNG160,2,2,240,50);
	if ((fileaction == SAVEANI) || (fileaction == SAVEDSPL) ||
		(fileaction == SAVEDISKA) || (fileaction == SAVEDISKP) ||
		(fileaction == SAVEPROC) || (fileaction == SAVEDICER) ||
		(fileaction == SAVETILE) || (fileaction == SAVEVIEW) ||
		(fileaction == SAVEVBUFF) || (fileaction == SAVEISO) )
	{
		gr_DialogAddTrans(tmp->fileDialog,gr_SaveFileTrans);
		gr_MakeButton("XDSbutton",boxWind,"SAVE",
			(XtCallbackProc)gr_FileSave,(caddr_t)tmp,
			245,2,45,50);
	}

	if ((fileaction == LOADSDS) || (fileaction == SAVEPROC))
	{
		boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,155,128,140,60);
		tmp->sdsDialog = gr_MakeDialog("XDSdialog",boxWind,
			"SDS #","-",STRNG160,2,2,50,50);
		if (fileaction == SAVEPROC)
			tmp->scaleDialog = gr_MakeDialog("XDSdialog",boxWind,
				"SCALE","1",STRNG160,52,2,50,50);
    }
	if (fileaction == LOADRAS)
	{
		boxWind = gr_MakeBulletin("XDSbulletin",tmp->win,155,128,140,60);
		tmp->rasStartDialog = gr_MakeDialog("XDSdialog",boxWind,
				"Start ","-",STRNG160,2,2,68,50);
		tmp->rasEndDialog = gr_MakeDialog("XDSdialog",boxWind,
				"End   ","-",STRNG160,70,2,68,50);
    }
	if (fileaction == SAVEANI)
	{
		boxWind = gr_MakeBox("XDSbox",tmp->win,155,128,140,185);
		gr_MakeToggle("XDStoggle",boxWind,"Save separately",SQUARE,
				FALSE,(XtCallbackProc)gr_FileSetSeparate,
				(XtCallbackProc)gr_FileSetSingle,
				(caddr_t)tmp,2,2,130,30);
	}

/*
	tmp->fileVPort = gr_MakeVPort("XDSvport",tmp->win,VERTONLY,ATHENA,
			NULL, NULL, 20, NULL, 2, 160, 150, 185);

	tmp->fileWin = gr_MakeListSimple("XDSlist",tmp->fileVPort,
			1,(XtCallbackProc)gr_FileCheck,
			tmp->fileData,(caddr_t)tmp,1,1,150,185);
*/
	tmp->fileWin = gr_MakeList("XDSlist",tmp->win, &(tmp->fileVPort),
			VERTONLY,1,20,NULL,NULL,(XtCallbackProc)gr_FileCheck,
			tmp->fileData,(caddr_t)tmp,2,128,150,185);

	if ((fileaction == LOADSDS) || (fileaction == LOADPAL) || 
		(fileaction == LOADPALSPLIT) || (fileaction == LOADRAS))
	{
		boxWind = gr_MakeBox("XDSbox",tmp->win,155,190,140,123);
		if (fileaction == LOADSDS)
		{
			gr_MakeButton("XDSbutton",boxWind,"LOAD SDS",
				(XtCallbackProc)gr_FileLoad,(caddr_t)tmp,
				2,2,130,30);
			gr_MakeToggle("XDStoggle",boxWind,"Load all SDSs",SQUARE,
					FALSE,(XtCallbackProc)gr_FileSetLoadAll,
					(XtCallbackProc)gr_FileSetLoadSingle,
					(caddr_t)tmp,2,35,130,20);
#ifdef MIRIADFMT
			RCWind = gr_MakeRowColumn("XDSrowcolumn",boxWind,ONEOFMANY,1,
				0,50,150,60);
			gr_MakeToggle("XDStoggle",RCWind,"HDF format",DIAMOND,
					TRUE,(XtCallbackProc)gr_FileUseHDF,
					NULL,(caddr_t)tmp,2,2,125,20);
			gr_MakeToggle("XDStoggle",RCWind,"Miriad format",DIAMOND,
					FALSE,(XtCallbackProc)gr_FileUseMiriad,
					NULL,(caddr_t)tmp,2,25,125,20);
#endif
		}
		else
		if (fileaction == LOADPAL)
		{
			gr_MakeButton("XDSbutton",boxWind,"LOAD PALETTE",
				(XtCallbackProc)gr_PalletLoad,(caddr_t)tmp,
				2,2,130,30);
			gr_MakeToggle("XDStoggle",boxWind,"Global Change",SQUARE,
					FALSE,(XtCallbackProc)gr_FileSetGlobal,
					(XtCallbackProc)gr_FileSetLocal,
					(caddr_t)tmp,2,35,130,30);
		}
		else
		if (fileaction == LOADPALSPLIT)
		{
			gr_MakeButton("XDSbutton",boxWind,"LOAD PALETTE",
				(XtCallbackProc)gr_PalletLoadSplit,(caddr_t)tmp,
				2,2,130,30);
		}
		else
		if (fileaction == LOADRAS)
		{
			gr_MakeButton("XDSbutton",boxWind,"ANIMATE RASTER8",
				(XtCallbackProc)gr_AniLoad,(caddr_t)tmp,
				2,2,130,30);
			gr_MakeToggle("XDStoggle",boxWind,"Use Pixmaps",SQUARE,
					TRUE,(XtCallbackProc)gr_FileUsePixmap,
					(XtCallbackProc)gr_FileUseXImage,
					(caddr_t)tmp,2,35,130,20);
			gr_MakeToggle("XDStoggle",boxWind,"Animate from Disk",SQUARE,
					FALSE,(XtCallbackProc)gr_FileUseDisk,
					(XtCallbackProc)gr_FileUseMemory,
					(caddr_t)tmp,2,58,130,20);
		}
	}

	gr_ListHighlight(tmp->fileWin,0);

	tmp->separateFiles = FALSE;
	tmp->globalPalette = FALSE;
	tmp->loadAllSDS = FALSE;
	tmp->format = HDF;
	tmp->useDisk = FALSE;

/* Cannot use pixmaps on the IRIS */
#ifdef IRIS
	tmp->usePixmap = FALSE;
#else
	tmp->usePixmap = TRUE;
#endif
	tmp->prev = NULL;
	tmp->next = header;
	if (header != NULL)
		header->prev = tmp;

	gr_WidgetCursor(tmp->shell,XC_draped_box);
	gr_WidgetCursor(gr_topLevel,XC_draped_box);

	return(tmp);
}


/*
 * Close File Window
 */
void
gr_CloseFileLevel(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
	A_FileWind_t	*tmp;

	if (gr_topWin.fileWin != NULL)
	{
		tmp = gr_topWin.fileWin;
		gr_topWin.fileWin = tmp->next;
		td_Free2d((char **)tmp->fileData);
		XtDestroyWidget(tmp->shell);
		gr_topWin.numFileWins--;
	}
}
