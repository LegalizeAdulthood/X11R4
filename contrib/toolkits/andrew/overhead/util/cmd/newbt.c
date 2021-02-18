/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	newbt.c
	Create a new B-tree of specified desired-node size.
\* ************************************************************ */

static char rcsid[] = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/cmd/RCS/newbt.c,v 2.6 89/10/23 14:08:15 cfe Exp $";

#include <andyenv.h>
#include <andrewos.h> /* strings.h */
#include <sys/param.h>
#include <sys/dir.h>
#include <stdio.h>
#ifdef WHITEPAGES_ENV  /* avoid makedepend "errors" */
#include "bt.h"
#endif /* WHITEPAGES_ENV   */

extern int errno;

extern char *malloc();
extern char *realloc();

static int Debugging = 0;

static char *NewFile;
static int EntrySize;

static void ParseArguments(argc,argv)
int argc;
char *argv[];
{
    int thisarg;
    char *Swch;
    static char *UsageArgs = " [-dDrRwW] FileToCreate EntrySize\n";

    for (thisarg = 1; thisarg < argc && argv[thisarg][0] == '-'; ++thisarg) {
	Swch = &argv[thisarg][1];
	if (strcmp(Swch, "d") == 0) Debugging = 1;
	else if (strcmp(Swch, "D") == 0) Debugging = 2;
	else if (strcmp(Swch, "r") == 0) (void) btr_SetDebugging(1);
	else if (strcmp(Swch, "R") == 0) (void) btr_SetDebugging(2);
	else if (strcmp(Swch, "w") == 0) (void) btw_SetDebugging(1);
	else if (strcmp(Swch, "W") == 0) (void) btw_SetDebugging(2);
	else {
	    fprintf(stdout, "Unrecognized option: ``%s''\nusage: %s%s",
		    argv[thisarg], argv[0], UsageArgs);
	    exit(1);
	}
    }

    if (thisarg < argc) {
	NewFile = argv[thisarg++];
    } else {
	fprintf(stdout, "Missing NewFile argument.\nusage: %s%s", argv[0], UsageArgs);
	exit(2);
    }

    if (thisarg < argc) {
	EntrySize = atoi(argv[thisarg++]);
    } else {
	fprintf(stdout, "Missing NewFile argument.\nusage: %s%s", argv[0], UsageArgs);
	exit(2);
    }

    if (thisarg < argc) {
	fprintf(stdout, "Extra arguments beginning with ``%s'';\nusage: %s%s",
		argv[thisarg], argv[0], UsageArgs);
	exit(3);
    }
}

main(argc, argv)
int argc;
char *argv[];
{
    bt_ErrorCode ErrVal;

    ParseArguments(argc,argv);

    ErrVal = bt_Create(NewFile, EntrySize, btlock_UseFileLock, NULL);
    if (ErrVal == bterr_NoError) {
	fprintf(stdout, "NewBT: File ``%s'' created.\n", NewFile);
	exit(0);
    } else {
	fprintf(stderr, "Error in bt_Create(%s): %s.\n", NewFile,
		bt_ErrorString(ErrVal));
	exit(1);
    }
}

