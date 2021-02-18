/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/fatalerr.c,v 2.3 89/02/08 15:33:07 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/fatalerr.c,v $
*/
#include <stdio.h>
#include <errprntf.h>

/* The following should never return, but should notify the postmaster
	that an error has occurred. */

FatalError(text)
char *text;
{
    errprintf("ms", ERR_CRITICAL, 0, 0, "Message server terminating: %s\n", text);
    fflush(stdout);
    fflush(stderr);
    MS_UpdateState(); /* Might actually help, who knows? */
    freepag();
    abort();			/* Let's have a core dump! */
    printf("Abort failed!\n");	/* Should not be reached */
    exit(-1);
}
