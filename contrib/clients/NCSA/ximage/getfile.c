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
#include <X11/Xos.h>
/*#include <sys/types.h>*/
#include <sys/stat.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/List.h>
#include <X11/Dialog.h>
#include <X11/IntrinsicP.h>
#include <X11/ViewportP.h>

#ifdef SYSV
#include <dirent.h>
#define MAXPATHLEN	255
#else
#include <sys/dir.h>
#include <sys/param.h>
#endif

#include "list.h"
#include "xstuff.h"
#include "getfile.h"


#define	XDIM	300
#define YDIM	308
#define	MAXFILES	512			/* Max number of files/dir */

extern	char	*getenv();
extern	Widget	toplevelform;

static	char	usecwd = TRUE;			/* Use current working dir*/
static	char	*defdir;
static	char	*cfilename = "";
/*static	char	*defdir = ".";*/
static	char	directory[MAXPATHLEN];
static	DIR	*dirFD;
static	char	dirFDopen = FALSE;
static	Widget	topFileWidget;
static	Widget	dir_dialog_widget;
static	Widget	cfile_dialog;
static	Widget	file_list_widget;
static	Widget	file_list_vport_widget;
static	char	*listOfFiles[MAXFILES];
static	int	numFiles = 0;		/* number of files in listOfFiles */
static	Widget	inputBox;
static	char	aFileHasBeenChosen = FALSE;
static	char	fileChosen[80];
static	struct	GetFile	*thisFile;
static	Widget	xDimDialog;
static	Widget	yDimDialog;
static	char	amIAlreadyOpen = FALSE;

static  char ChangeDirTrans[] = "<Key>Return:    ETDirReturn()";

void CBChoseFile();
void CBOpen();
void CBChangeDirectory();
void CBCancel();

int InitGetAFile(dpy)
Display *dpy;
{
int x;

	if (!(defdir = (char *) XIMalloc(MAXPATHLEN))) {
		printf("Can't XIMalloc any memory\n");
		return(NULL);
		}

	for(x = 0; x < MAXPATHLEN; x++)
		directory[x]=' ';

#ifdef SYSV
	if (!getcwd(defdir,MAXPATHLEN)){
#else
	if (!getwd(defdir)) {
#endif
		printf("Can't get working directory\n");
		return(NULL);
		}

	dirFDopen = FALSE;

	return(1);
}



char DoesFileExist(f)
char *f;
{
FILE *fp;
	if (fp = fopen(f,"r")) {
		fclose(fp);
		return(TRUE);
		}
	else
		return(FALSE);
}


void GetAFile(fileinfo)
struct	GetFile	*fileinfo;
{
Widget	board;
Widget	fileCtrlWidget;
Widget	bbox;
Widget	bbox2;
Widget	box3;
int	x;
/*int	numFiles;*/
Display	*dpy;
int	screen;

	if (amIAlreadyOpen)
		return;

	thisFile = fileinfo;

	dpy = XtDisplay(toplevelform);
	screen = XDefaultScreen(dpy);
	topFileWidget = MakeATopLevelOverride("GetAFile",
				toplevelform, 
				(DisplayWidth(dpy,screen)-XDIM)/2,
				(DisplayHeight(dpy,screen)-YDIM)/2,
				XDIM,YDIM);

	fileCtrlWidget = Make_Window("NCSA X Image",topFileWidget,
				5,5,XDIM,YDIM);

	if (usecwd) {
		/* should be a getenv*/
/*		if (! (theDir = getenv("CWD")))
			PrintMesg("Can't get PWD environment variable\n");
			return(NULL);
*/

/**/		strcpy(directory,defdir);
		}

	
	if (!(CreateListofFiles(directory,listOfFiles,&numFiles)))
		return;


	board= Make_Board("blabla",fileCtrlWidget,2,22,XDIM-7,55);
	
	dir_dialog_widget = Make_Dialog("DirOfFiles",board,
			"Directory:                    ",
				directory,80,2,2,XDIM-60,50);

	DialogAddTrans(dir_dialog_widget,ChangeDirTrans);
	
	(void) MakeButton("Change", board,XDIM-54,2,CBChangeDirectory,NULL,
				0,50);

	box3 = Make_Board("blabla",fileCtrlWidget,2,80,XDIM-7,55);
	cfile_dialog = Make_Dialog("DirOfFiles",box3,
			"Current Filename:                    ",
				cfilename,80,2,2,XDIM-60,50);

	gr_DialogSetValue(cfile_dialog,"");

	aFileHasBeenChosen = FALSE;

	file_list_widget = MakeList("ListOfFiles",fileCtrlWidget,
			VERTONLY,&file_list_vport_widget,1,CBChoseFile,
			listOfFiles,2,138,150,YDIM-142,NULL,NULL);

	bbox = Make_ButtonBox(fileCtrlWidget,155,138,XDIM-160,YDIM-142);
	bbox2 = Make_ButtonBox(bbox,2,2,XDIM-165,30);
	(void) MakeButton(" Open ",bbox2,10,5,CBOpen,NULL,0,0);
	(void) MakeButton(" Cancel ",bbox2,70,5,CBCancel,NULL,0,0);
	inputBox = Make_ButtonBox(bbox,2,35,XDIM-165,YDIM-181);

	fileChosen[0]='\0';
	amIAlreadyOpen = TRUE;
	
	return;

} /* GetAFile() */


static void CloseFileWidget()
{
char **p;

	p = listOfFiles;
	while (*p) {
		XIFree(*p);
		p++;
		}

	if(dirFDopen) {
		closedir(dirFD);
		dirFDopen = FALSE;
		}
	XtDestroyWidget(topFileWidget);

/*	XFlush(XtDisplay(topFileWidget));*/
	XSync(XtDisplay(topFileWidget),False);

	amIAlreadyOpen = FALSE;
	return;
}


void CBCancel(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{

	CloseFileWidget();
	thisFile->nofile(thisFile->param);

} /*CBCancel() */


BubbleSort(list,num)
char *list[];
{
char *tmp;
int x,y;
	for(x=0; x< num; x++)
		for(y=0; y < num-x-1; y++)
			if (strcmp(list[y],list[y+1]) > 0) {
				tmp = list[y+1];
				list[y+1] = list[y];
				list[y] = tmp;
				}

}


int CreateListofFiles(theDir,listOfFiles,nf)
char	*theDir;
char	*listOfFiles[];
int	*nf; /* initially contains num files in listOfFiles,  */
	     /* and returns new value */
{
char errmesg[100];
int	count;
DIR	*newdirFD;
char **p;

#ifdef SYSV
struct dirent *dirInfo;
#else
struct	direct	*dirInfo;
#endif

	if (!(newdirFD = opendir(theDir))) {
		sprintf(errmesg,"Can't open directory %s\n",theDir);
		PrintMesg(errmesg);
		return(NULL);
		}

	if (dirFDopen) {
		closedir(dirFD);

		/* free old Malloc list */
/*{ int c; c=0;
		p = listOfFiles;
		while (*(p+1)) {
fprintf(stderr,"****** Freeing %d: %s\n",c++,*p);
			XIFree(*p);
			p++;
			}
}
*/
{ int x;
	for (x=0,p=listOfFiles; x < *nf; x++,p++) {
/* fprintf(stderr,"****** Freeing %d: %s\n",x,*p);*/
		XIFree(p);
		}
	
}
		}
	dirFD= newdirFD;
	dirFDopen = TRUE;

	count = 0;
	while((dirInfo = readdir(dirFD))  && (count < (MAXFILES-2))) {
		if (strcmp(dirInfo->d_name,".")&
			(strcmp(dirInfo->d_name,".."))) {
			listOfFiles[count] = (char *) XIMalloc(strlen(
							dirInfo->d_name));
			strcpy(listOfFiles[count], dirInfo->d_name);
			count++;
			}
		}
	*nf = count;

	BubbleSort(listOfFiles,count);

/**/	listOfFiles[count] = NULL;

	while(count < MAXFILES-2) {
		listOfFiles[count] = " ";
		count++;
		}

	listOfFiles[count] = NULL;

	return(1);
}


void CBChangeDirectory(w,client_data,call_data)
Widget w;
caddr_t client_data; /* if TRUE, directory already contains new dir */
caddr_t call_data;
{
static  Arg arglist[]={{XtNlist,(XtArgVal) listOfFiles}};
/*int	numFiles;*/

	if (!client_data) {
		strcpy(directory,XtDialogGetValueString(dir_dialog_widget));
		Terminate(directory);
		}

	usecwd = FALSE;
	if (!CreateListofFiles(directory,listOfFiles,&numFiles))
		return;

	gr_DialogSetValue(cfile_dialog,"");
	
	XtListChange(file_list_widget,listOfFiles,numFiles,0,False);

	XtMoveWidget(file_list_widget,0,0);
	XtScrollBarSetThumb(
		((ViewportWidget) file_list_vport_widget)->viewport.vert_bar,
		0,0);
	
} /* CBChangeDirectory() */


void ETDirReturn(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
int num_params;
{
	CBChangeDirectory(w,NULL,NULL);
	
}


	

void CBChoseFile(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
char path[MAXPATHLEN];
struct stat fileStat;

	strcpy (fileChosen,((XtListReturnStruct *)call_data)->string);

	sprintf(path,"%s/%s",directory,fileChosen);
	stat(path,&fileStat);
	if (fileStat.st_mode & S_IFDIR) {
		strcpy(directory,path);
		gr_DialogSetValue(dir_dialog_widget,directory);
		CBChangeDirectory(w,TRUE,NULL);
		return;
		}
		
	
	aFileHasBeenChosen = TRUE;
	gr_DialogSetValue(cfile_dialog,fileChosen);

/*	sprintf(buff,"Just chose file %s/%s\n",directory,fileChosen);
	PrintMesg(buff);
*/
}

static void CBOk(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	/* return values */
	thisFile->xdim = atoi(XtDialogGetValueString(xDimDialog));
	thisFile->ydim = atoi(XtDialogGetValueString(yDimDialog));

	CloseFileWidget();


	thisFile->yesfile(thisFile->param);
}


void CBOpen(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
char	xstring[80];
char buff[256];
char firstFile[512];

	cfilename = XtDialogGetValueString(cfile_dialog);
	if (!cfilename[0]) {
		PrintMesg("Please select a file\n");
		return;
		}

	thisFile->base = (char *) XIMalloc(strlen(cfilename)+1);
	sprintf(thisFile->base,cfilename);

	thisFile->directory = (char *) XIMalloc(strlen(directory)+1);
	sprintf(thisFile->directory,directory);

	if (!Glob(directory,cfilename,&(thisFile->numFiles),
				&(thisFile->file))){
		sprintf(buff,"Can't open directory %s\n",directory);
		PrintMesg(buff);
		return;
		}
	
	sprintf(firstFile,"%s/%s",thisFile->directory,*(thisFile->file));

	if (! DoesFileExist(firstFile)){
		sprintf(buff,"Can't open file %s\n",firstFile);
		PrintMesg(buff);
		return;
		}

	if (thisFile->numFiles > 1)
		sprintf(buff,"Selected files %s/%s\n",thisFile->directory,
						thisFile->base);
	else
		sprintf(buff,"Selected file %s\n",firstFile);
	PrintMesg(buff);

	if (thisFile->type == FTYPE_IMAGE) {
	    /* if fileChosen is HDF */
	    if (IsItHDF(firstFile,&(thisFile->xdim),&(thisFile->ydim),
			&(thisFile->isFloat))){
		thisFile->isHDF = TRUE;
		CloseFileWidget();
		thisFile->yesfile(thisFile->param);

		}
	    else {
		thisFile->isHDF = FALSE;
		PrintMesg("Please Type in the dimensions of the image\n");
		xDimDialog = Make_Dialog("GetXDimension",inputBox,
					"Width of Image:","",4,
					2,2,XDIM-170,50);
		yDimDialog = Make_Dialog("GetYDimension",inputBox,
					"Height of Image:","",4,
					2,55,XDIM-170,50);
		MakeButton("         OK         ",inputBox,2,108,CBOk,
				NULL,0,0);
		}
	    }
	else { /*Else it's a FTYPE_PAL */
		CloseFileWidget();
		thisFile->isHDF = FALSE;
		if (DFishdf(firstFile) == 0)
			thisFile->isHDF = TRUE;
		thisFile->yesfile(thisFile->param);
		}
}
