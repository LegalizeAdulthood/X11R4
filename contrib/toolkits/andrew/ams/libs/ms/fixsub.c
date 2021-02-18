/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/fixsub.c,v 2.3 89/02/08 15:45:18 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/fixsub.c,v $
*/
#include <ms.h>

extern char *malloc();

struct {
    char *left, *right;
} SubjectProductions[] = {
    "re: re:", "Re:",
    "fwd: fwd:", "Fwd:",
    "re: fwd:", "Re:",
    "fwd: re:", "Fwd:",
    0, 0};

char *
RewriteSubject(oldsub)
char *oldsub;
{
    char *newsub, *tempsub;
    Boolean DidSomething = TRUE;
    int i;

    newsub = malloc(1+strlen(oldsub));
    if (!newsub) return(NULL);
    strcpy(newsub, StripWhiteEnds(oldsub));
    ReduceWhiteSpace(newsub);
    while (DidSomething) {
	DidSomething = FALSE;
	for (i=0; SubjectProductions[i].left; ++i) {
	    if (!lc2strncmp(SubjectProductions[i].left, newsub, strlen(SubjectProductions[i].left))) {
		DidSomething = TRUE;
		tempsub = malloc(1+strlen(newsub));
		if (!tempsub) {
		    free(newsub);
		    return(NULL);
		}
		sprintf(tempsub, "%s%s", SubjectProductions[i].right, newsub+strlen(SubjectProductions[i].left));
		free(newsub);
		newsub = tempsub;
		break;
	    }
	}
    }
    return(newsub);
}
