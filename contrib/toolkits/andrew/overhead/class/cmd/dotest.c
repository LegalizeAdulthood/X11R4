/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
	dotest.c - test loadability of .do file

	Author:  John H Howard - June 11, 1987
 */

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/class/cmd/RCS/dotest.c,v 2.4 89/04/11 15:42:26 pgc Exp $";


#include <stdio.h>
#include <andrewos.h> /* sys/file.h */

char *doload();

extern int doload_trace;				/* nonzero for debugging */

/* globals.o calls for some wmclient routines, and wmclient */
/* calls FlagRedraw.  Therefore we must provide a dummy here. */

FlagRedraw()
{
    fprintf(stderr, "dolist:  FlagRedraw called ... what a blunder!\n");
    exit(69);
}

/* main program for testing loadability */

main(argc, argp)
int argc;
char **argp;
{
    int fd;
    int gotcha = 0;
    char *result;

    while (--argc > 0) {
	if (**++argp == '-') {
	    switch (*++*argp) {
	    case 'd':
		doload_trace++;
		break;
	    default:
		fprintf(stderr, "dotest:  Unknown switch -%c ignored\n", **argp);
	    }
	}
	else {
	    gotcha++;
	    fd = open(*argp, O_RDONLY, 0);
	    if (fd < 0)
		fprintf(stderr, "dotest:  File %s not found\n", *argp);
	    else {
		result = doload(fd, *argp, NULL, NULL);
                close(fd);
                if (result == NULL) {
                    printf("dotest:  error loading %s\n", *argp);
                    exit(2);
                }
	    }
	}
    }
    if (gotcha == 0) {
	printf("dotest:  no files specified\n");
	exit(1);
    }
    exit(0);	/* if everything goes well, return 0 */
}
