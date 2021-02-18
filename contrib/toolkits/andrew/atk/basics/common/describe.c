/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/describe.c,v 2.5 89/02/17 18:35:52 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/describe.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/describe.c,v 2.5 89/02/17 18:35:52 ghoti Exp $";
#endif /* lint */

#include <class.h>
#include <view.ih>
#include <describe.eh>

/* changes to describe.c */

enum view_DescriberErrs describer__Describe(self,viewToDescribe,format,file,rock)
struct describer * self;
struct view * viewToDescribe;
char * format;
FILE * file;
long rock;
{
    /* Print out a console message saying that this format is not available */
    return view_NoDescriptionAvailable;
}

boolean describer_InitializeObject(classID,self)
struct classheader * classID; 
struct describer * self; {
    return TRUE;
}
