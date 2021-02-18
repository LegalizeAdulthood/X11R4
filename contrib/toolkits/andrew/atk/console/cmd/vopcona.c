/*LIBS: -lconsole -lerrors -lutil -lcont -lvint -lr -llwp -lm
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/vopcona.c,v 1.12 89/10/04 18:59:45 cfe Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/vopcona.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/vopcona.c,v 1.12 89/10/04 18:59:45 cfe Exp $";
#endif /* lint */

/* 
 * console application class - to run dynamically loaded console with
 * shared libraries.
 */

#include <class.h>

#include <app.ih>

#include <view.ih>
#include <im.ih>
#include <conclass.ih>
#include <logv.ih>
#include <initglbs.h>
#include <vopcona.eh>
#include <convers.h>
#include <proctbl.ih>


boolean vopconapp__InitializeObject(classID,self)
struct classheader *classID;
struct vopconapp *self;
{
    self->consoleName = NULL;
    vopconapp_SetMajorVersion(self, MAJORVERSION);
    vopconapp_SetMinorVersion(self, MINORVERSION);
    return TRUE;
}

void vopconapp__FinalizeObject(classID,self)
struct classheader *classID;
struct vopconapp *self;
{
    if(self->consoleName != NULL){
	free(self->consoleName);
    }
}

/*
 * These 3 routines are the heart of the vopconapp interface
 */
int ForceErrorMonitoring = FALSE;

boolean vopconapp__ParseArgs(self,argc,argv)
struct vopconapp *self;
int argc;
char **argv;
{
    if(!super_ParseArgs(self,argc,argv))
	return FALSE;

#define GETARGSTR(var)\
{\
    if((*argv)[2]!='\0')\
        var= ((*argv)[2]=='=' ? &(*argv)[3] : &(*argv)[2]);\
    else if(argv[1]==NULL){\
	fprintf(stderr,"%s: %s switch requires an argument.\n",vopconapp_GetName(self),*argv);\
        return FALSE;\
    }else\
    	var= *++argv;\
}

    while(*++argv!=NULL)
	if(**argv=='-')
	    switch((*argv)[1]){
		case 'c':
		    ForceErrorMonitoring = TRUE;
		    break;
		case 'D':
		    MYDEBUGGING = TRUE;
		    mydbg(("Entering my debugging mode\n"));
		    break;
		case 'q':
		    HighestPriority = 0;
		    break;
		case 'v':
		    HighestPriority = 6;
		    break;
		case 'V':
		    HighestPriority = 9;
		    break;
		default:
		    fprintf(stderr,"%s: unrecognized switch: %s\n", vopconapp_GetName(self), *argv);
		    return FALSE;
	    }
	else {
	    if(self->consoleName!=NULL){
		fprintf(stderr,"%s: only one console at a time\n", vopconapp_GetName(self));
		return FALSE;
	    }
	    self->consoleName = (char *)malloc(strlen(*argv)+1);
	    strcpy(self->consoleName, *argv);
	}

    return TRUE;
}


boolean vopconapp__Start(self)
struct vopconapp *self;
{
    struct consoleClass *con;
    struct im *im;

    if(!super_Start(self))
	return FALSE;

    PostParseArgs(self->consoleName);
    con = consoleClass_New();
    OneTimeInit(con);
    SetupFromConsoleFile(con,TRUE);
    if (MaxWidth < MinWidth) MaxWidth = MinWidth;
    if (MaxHeight < MinHeight) MaxHeight = MinHeight;
    im_SetPreferedDimensions(0, 0, MaxWidth, MaxHeight);
    if ((im = im_Create(NULL)) == NULL) {
        printf("Could not open the display; this program will not run without a window system.\n");
        return FALSE;
    }
    im_SetView(im, con);
    consoleClass_WantInputFocus(con,con);
    return(TRUE);
}



boolean vopconapp__InitializeClass(classID)
struct classheader *classID;
{
    consoleClass_StaticEntry;
    logview_StaticEntry;
    return(TRUE);
}
