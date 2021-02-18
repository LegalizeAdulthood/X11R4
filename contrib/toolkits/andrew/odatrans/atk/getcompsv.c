/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getcompsv.c,v 1.3 89/09/11 13:55:16 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getcompsv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getcompsv.c,v 1.3 89/09/11 13:55:16 mss Exp $";
#endif

/*
  $Log:	getcompsv.c,v $
 * Revision 1.3  89/09/11  13:55:16  mss
 * Got rid of extraneous inclusion of text.ih
 * 
 * Revision 1.2  89/09/08  17:26:53  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:15:02  mss
 * Initial revision
 * 
 * Revision 1.2  89/04/30  12:06:42  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.1  88/11/18  15:52:53  mss
 * Initial revision
 * 


*/

/* This module keeps track of style/sv relationships and calculations */


#include <odatk.h>
#include <class.h>
#include <odadef.h>
#include <attsup.h>
#include <text.ih>
#include <textv.ih>
#include <txtstvec.h>
#include <style.ih>
#include <stylesht.ih>
#include <envrment.ih>
#include <fontdesc.ih>
#include <odalib.ih>
#include <odaatk.h>
#include <getcompsv.h>




struct text_statevector * GetCompensatedSV(gD, specConstituent, curSV, styleToUse, compensatingStyle)
p_odainfo_type gD;
CONSTITUENT specConstituent;
struct text_statevector * curSV;   /* Current value of environment that contains this subtree */
struct style * styleToUse;
struct style ** compensatingStyle;
{
    struct text_statevector * rval;

    if (gD->doDebugging) {
	printf("Entering GetCompensatedSV\n");
    }

    *compensatingStyle = (struct style *) NULL;
    rval = (struct text_statevector * ) NULL;

    if (gD->doDebugging) {
	printf("Leaving GetCompensatedSV\n");
    }

    return rval;

}


typedef struct styleApplicationBlk_struct {
    struct styleApplicationBlk_struct	*next;
    long				contentStart;
    long				contentLength;
    struct style			*styleToUse;
} styleApplicationBlk, * p_styleApplicationBlk;

static p_styleApplicationBlk outstandingStyles  = NULL;
static long curContentStart;
static long curContentLength;

INT_type FlushStyleList(gD)
p_odainfo_type gD;
{
    INT_type rval;
    struct environment		*newEnv;	 /* env from wrapping style */
    p_styleApplicationBlk	curBlk, prevBlk;

    rval = SUCCESS_INT;

    for(curBlk=outstandingStyles; curBlk != NULL; prevBlk = curBlk, curBlk = curBlk->next, free(prevBlk)) {
	newEnv = text_AlwaysAddStyle(gD->topLevelDocument, curBlk->contentStart, curBlk->contentLength, curBlk->styleToUse);
	/* end the style */
	newEnv->header.nestedmark.includeEnding = 0;
	newEnv->header.nestedmark.includeBeginning = 1;
    }

    outstandingStyles = (p_styleApplicationBlk) NULL;

    return rval;

}

INT_type DoStyleApplication(gD, contentStart, contentLength, styleToUse)
p_odainfo_type gD;
long		contentStart, contentLength;
struct style * styleToUse;
{
    INT_type		    rval;
    p_styleApplicationBlk   tempBlk;

    if (gD->doDebugging) {
	printf("Entering DoStyleApplication\n");
    }

    rval = SUCCESS_INT;

    if ((outstandingStyles != NULL) &&
	(contentStart != curContentStart) &&
	(contentLength != curContentLength) ) {
	FlushStyleList(gD);
    }

    tempBlk = (p_styleApplicationBlk) malloc(sizeof(styleApplicationBlk));
    tempBlk->next = outstandingStyles;
    outstandingStyles = tempBlk;
    tempBlk->contentStart = contentStart;
    tempBlk->contentLength = contentLength;
    tempBlk->styleToUse = styleToUse;
    curContentStart = contentStart;
    curContentLength = contentLength;
    
    if (gD->doDebugging) {
	printf("Leaving DoStyleApplication\n");
    }

    return rval;

}

INT_type FinishStyleApplications(gD)
p_odainfo_type gD;
{
    INT_type rval;

    if (gD->doDebugging) {
	printf("Entering FinishStyleApplications\n");
    }

    rval = SUCCESS_INT;

    FlushStyleList(gD);

    if (gD->doDebugging) {
	printf("Leaving FinishStyleApplications\n");
    }

    return rval;

}
