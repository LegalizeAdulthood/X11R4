

/*LIBS: -lconsole -lerrors -lutil -lm
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/consolea.c,v 1.11 89/10/04 18:56:59 cfe Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/consolea.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/consolea.c,v 1.11 89/10/04 18:56:59 cfe Exp $";
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
#include <consolea.eh>
#include <convers.h>
#include <proctbl.ih>


boolean consoleapp__InitializeObject(classID,self)
struct classheader *classID;
struct consoleapp *self;
{
    self->consoleName = NULL;
    /* NOTE: the following defines for the version info come from ../lib/convers.h */
    consoleapp_SetMajorVersion(self, MAJORVERSION); 
    consoleapp_SetMinorVersion(self, MINORVERSION);
    return TRUE;
}

void consoleapp__FinalizeObject(classID,self)
struct classheader *classID;
struct consoleapp *self;
{
    if(self->consoleName != NULL){
	free(self->consoleName);
    }
}

/*
 * These 3 routines are the heart of the consoleapp interface
 */
int ForceErrorMonitoring = FALSE;

boolean consoleapp__ParseArgs(self,argc,argv)
struct consoleapp *self;
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
	fprintf(stderr,"%s: %s switch requires an argument.\n",consoleapp_GetName(self),*argv);\
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
		    fprintf(stderr,"%s: unrecognized switch: %s\n", consoleapp_GetName(self), *argv);
		    return FALSE;
	    }
	else {
	    if(self->consoleName!=NULL){
		fprintf(stderr,"%s: only one console at a time\n", consoleapp_GetName(self));
		return FALSE;
	    }
	    self->consoleName = (char *)malloc(strlen(*argv)+1);
	    strcpy(self->consoleName, *argv);
	}

    return TRUE;
}



boolean consoleapp__Start(self)
struct consoleapp *self;
{
    struct consoleClass *con;
    struct im *im;

    if(!super_Start(self))
	return FALSE;
    PostParseArgs(self->consoleName);
    con = consoleClass_New();
    OneTimeInit(con);
    SetupFromConsoleFile(con,TRUE);
    InitializeGetStats(con);
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



boolean consoleapp__InitializeClass(classID)
struct classheader *classID;
{
    consoleClass_StaticEntry;
    logview_StaticEntry;
    return(TRUE);
}
