/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/viewref.c,v 2.3 89/02/17 17:15:20 ghoti Exp $ */
/* $ACIS:viewref.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/viewref.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/viewref.c,v 2.3 89/02/17 17:15:20 ghoti Exp $";
#endif /* lint */

#include <class.h>
#include <viewref.eh>

static long viewID = 0;

boolean viewref__InitializeObject(classID, self)
struct classheader *classID;
struct viewref *self;
{
    self->viewType = NULL;
    self->viewID = viewID++;
    self->dataObject = NULL;
    self->desw = self->desh = 0;
    return TRUE;
}

void viewref__FinalizeObject(classID, self)
struct classheader *classID;
struct viewref *self;
{
    if (self->viewType != NULL)
        free(self->viewType);
}

struct viewref *viewref__Create(classID, viewType, dataObject)
struct classheader *classID;
char *viewType;
struct dataobject *dataObject;
{
    struct viewref *newvr;
    
    if ((newvr = viewref_New()))  {
	if ((newvr->viewType = (char *) malloc(strlen(viewType)+1)))  {
	    strcpy(newvr->viewType, viewType);
	    newvr->dataObject = dataObject;
	    return newvr;
	}
    }
    fprintf(stderr, "Could not allocate viewref structure - exiting\n");
    exit(1);
}
