/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/runadewa.c,v 1.1 89/10/03 14:09:54 tpn Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/runadewa.c,v $ */

#ifndef lint 
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/runadewa.c,v 1.1 89/10/03 14:09:54 tpn Exp $";
#endif /* lint */

#include <andrewos.h> /* sys/types.h sys/file.h */

#include <andyenv.h>
#include <class.h>

#include <im.ih>
#include <frame.ih>
#include <dataobj.ih>
#include <buffer.ih>
#include <message.ih>
#include <environ.ih>
#include <app.ih>
#include <proctbl.ih>

#include <runadewa.eh>


static char **Gargv;
static int Gargc;
extern int errno;

boolean runadewapp__InitializeObject(classID,self)
struct classheader *classID;
struct runadewapp *self;
{
    self->initFile=TRUE;
    self->files=NULL;
    self->fileLink= &self->files;
    self->haveBufferInWindow = FALSE;
    self->im = NULL;
    self->frame = NULL;
    self->buffer = NULL;
    runadewapp_SetMajorVersion(self, 7);
    runadewapp_SetMinorVersion(self, 0);
    self->cls = NULL;
    self->func = "go";
    self->title = NULL;
    return TRUE;
}


static void StartupError( string)
    char *string;
{
	fprintf(stderr,"%s\n",string);
	fflush(stderr);
}


static void addFile(self,name,newWin,ro)
struct runadewapp *self;
char *name;
boolean newWin,ro;
{
    struct runadewapp_fileList *fileEntry=
      (struct runadewapp_fileList *) malloc(sizeof(struct runadewapp_fileList));

    fileEntry->filename=name;
    fileEntry->newWindow=newWin;
    fileEntry->readOnly=ro;
    fileEntry->next=NULL;
    *self->fileLink=fileEntry;
    self->fileLink=(&(fileEntry->next));
}
char *getarg(argv,argc)
char **argv;
int *argc;
{
    int cnt;
    char *opt;
    if(argv[0][2] == '\0'){
	opt = argv[1];
	cnt = 2;
    }
    else {
	opt = (*argv) + 2;
	cnt = 1;
    }
    application_DeleteArgs(argv,cnt);
    *argc -= cnt;
    return opt;
}
boolean runadewapp__ParseArgs(self,argc,argv)
struct runadewapp *self;
int argc;
char **argv;
{
    int maxInitWindows=environ_GetProfileInt("MaxInitWindows", 2);
    boolean useNewWindow = FALSE;
    boolean pendingReadOnly = TRUE;
    Gargc = argc;
    Gargv = argv;

    if(!super_ParseArgs(self,argc,argv))
	return FALSE;


    if (maxInitWindows < 2)
	maxInitWindows = 1;
    argv++;
    while(*argv!=NULL){
	if(**argv=='-')
	    switch((*argv)[1]){
		case 'C': 
		    self->cls = getarg(argv,&Gargc);
		    break;
		case 'F':
		    self->func =getarg(argv,&Gargc);
		    break;
		case 'T': 
		    self->title =getarg(argv,&Gargc);
		    break;
		case 'S':
		    addFile(self,
			    getarg(argv,&Gargc),
			    (useNewWindow || maxInitWindows-->0),
			    pendingReadOnly);
		    break;		    
		default:
		    argv++;
	    }
	else{
	    argv++;
	}
    }
    return TRUE;
}



boolean runadewapp__Start(self)
struct runadewapp *self;
{
    struct runadewapp_fileList *fileEntry, *next;
    char iname[256];
    struct proctable_Entry *pr;
    int (*proc)();
    if(!super_Start(self))
	return FALSE;
    for (fileEntry = self->files; fileEntry != NULL; fileEntry = next) {
        self->buffer = buffer_GetBufferOnFile(fileEntry->filename, fileEntry->readOnly ? buffer_ReadOnly : 0);

	if (self->buffer != NULL && fileEntry->newWindow) {
	    self->frame = frame_New();
	    self->im = im_Create(NULL);
	    im_SetView(self->im, self->frame);
	    frame_PostDefaultHandler(self->frame, "message", frame_WantHandler(self->frame, "message"));
	    frame_SetBuffer(self->frame, self->buffer, TRUE);
	    buffer_SetScratch(self->buffer,TRUE);
	    frame_SetCommandEnable(self->frame, FALSE);
	    if(self->title)
		frame_SetTitle(self->frame,self->title);
	    if(self->cls){
		strcpy(iname,self->cls);
		if(class_Load(iname) != NULL){
		    strcat(iname,"-");
		    strcat(iname,self->func);
		    if((pr = proctable_Lookup(iname)) != NULL && proctable_Defined(pr) ){
			proc = proctable_GetFunction(pr) ;
			(*proc)(frame_GetView(self->frame),0);
		    }
		}
	    }
	    self->haveBufferInWindow = TRUE;
	}
        else {
            char errorMessage[200];
            sprintf(errorMessage, "File %s does not exist and could not be created.", fileEntry->filename);
            StartupError(errorMessage);
        }
        next = fileEntry->next;
        free(fileEntry);
    }

    if(self->haveBufferInWindow == FALSE){
	return FALSE;
    }
    return TRUE;
}


boolean runadewapp__InitializeClass()
{
    Gargv = NULL;
    Gargc = 0;
    return TRUE;
}
char **runadewapp__GetArguments(classID,argc)
struct classheader *classID;
int *argc;
{
    if(argc != NULL) *argc = Gargc;
    return Gargv;
}
