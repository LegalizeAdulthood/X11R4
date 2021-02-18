/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/cmpfts.c,v 2.4 89/02/15 11:39:44 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/cmpfts.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/cmpfts.c,v 2.4 89/02/15 11:39:44 ghoti Exp $ ";
#endif /* lint */

#include <ms.h>

CompareFileTimeStructs(ft1, ft2)
struct FileTime *ft1, *ft2;
{
    return((ft1->Time > ft2->Time) ? 1 : -1);
}

FreeFTList(FTL, nf)
struct FileTime *FTL;
int nf;
{
    int i;

    if (FTL) {
	for(i=0; i<nf; ++i) {
	    if (FTL[i].Name) free(FTL[i].Name);
	}
	free(FTL);
    }
}

