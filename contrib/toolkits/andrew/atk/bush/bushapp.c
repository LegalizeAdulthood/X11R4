/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#ifndef lint
static char *rcsidbushapp_c = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/bush/RCS/bushapp.c,v 1.6 89/09/07 19:56:19 tom Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	Bush Application

MODULE	bushapp.c

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the Application layer for the Bush Inset..

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  08/21/85	Created (TCP)
  01/15/89	Convert to ATK (GW Keim)
  05/01/89	Revise Initializing msg to conform with Majro/Minor Version msg (TCP)
  06/06/89	Added call to chdir() to set the current-working-directory to the 
	               path specified on the command line; (GW Keim)
  07/05/89	Use super_ParseArgs (TCP)
  07/28/89	Changed all occurances of AptTree to just Tree; (GW Keim)
  09/07/89	Set V1.0 msg (TCP)

END-SPECIFICATION  ************************************************************/

#include <sys/param.h>
#include <class.h>
#include "bush.h"
#include "im.ih"
#include "view.ih"
#include "frame.ih"
#include "tree.ih"
#include "bush.ih"
#include "bushv.ih"
#include "bushapp.eh"

#define	Bush		((self)->bush)
#define	Debug		((self)->debug)
#define	ProgramName	((self)->program_name)
#define	CW		((self)->current_window)
#define	NodesWindow	((self)->nodes_window)
#define	EntriesWindow	((self)->entries_window)
#define	EntryWindow	((self)->entry_window)
#define imPtr		((self)->im)
#define framePtr	((self)->frame)

#define			nodes_object		    1
#define			entries_object		    2
#define			entry_object		    4

boolean
bushapp__InitializeObject( ClassID, self )
    struct classheader	*ClassID;
    struct bushapp	*self;
{
    Bush = NULL;
    imPtr = NULL;
    framePtr = NULL;
    Debug = 0;
    *ProgramName = NULL;
    CW = NodesWindow = EntriesWindow = EntryWindow = NULL;
    bushapp_SetMajorVersion( self, 1 );
    bushapp_SetMinorVersion( self, 0 );
    return TRUE;
}

void
bushapp__FinalizeObject( ClassID, self )
    struct classheader	*ClassID;
    struct bushapp	*self;
{
    if(imPtr) im_Destroy(imPtr);
    if(framePtr) frame_Destroy(framePtr);
    if(Bush) bush_Destroy(Bush);
    if(NodesWindow) bushv_Destroy(NodesWindow);
    if(EntriesWindow) bushv_Destroy(EntriesWindow);
    if(EntryWindow) bushv_Destroy(EntryWindow);
}

boolean
bushapp__ParseArgs( self, argc, argv )
    struct bushapp  *self;
    int		     argc;
    char	   **argv;
{
    if ( ! super_ParseArgs( self, argc, argv ) )
      return FALSE;
    bushapp_SetFork(self,TRUE);
    self->argc = argc;
    self->argv = argv;
    return(TRUE);
}

boolean
bushapp__Start( self )
    struct bushapp  *self;
{
    register char   *StartDir = NULL;
	
    if(self->argc > 1) StartDir = self->argv[1];
    if(!(super_Start(self)) ||
       !(imPtr = im_Create(NULL)) ||
       !(framePtr = frame_New()) ||
       !(Bush = bush_Create(StartDir)) ||
       !(CW = NodesWindow = bushv_Create(nodes_object))) return(FALSE);
    bushv_SetDataObject(NodesWindow,Bush);
    frame_SetView(framePtr,NodesWindow);
    im_SetView(imPtr,framePtr);
    if(bush_TreeRoot(Bush)) 
	im_SetTitle(imPtr,bush_DirName(Bush,bush_TreeRoot(Bush)));
    chdir(bush_DirPath(Bush,bush_TreeRoot(Bush)));
    bushv_WantInputFocus(NodesWindow,NodesWindow);
    return(TRUE);
}



