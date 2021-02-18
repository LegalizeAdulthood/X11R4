/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/*
		arpadate.c -- Shell command to print current time in
			      ARPAnet format
*/

#include <stdio.h>
#include <ctype.h>

extern char *arpadate();

int main(argc, argv)
char *argv[];
{
    int AnyArgs, ac;
    unsigned long int Then;
    static char Usage[] = "[-n] [ctime1 ctime2 ...]";

    AnyArgs = 0;
    for (ac = 1; ac < argc; ++ac) {
	if (argv[ac][0] == '-') {
	    if (strcmp(argv[ac], "-n") == 0) {
		printf("%lu\n", time(0));
		AnyArgs = 1;
	    } else {
		fprintf(stderr, "Unknown option ``%s''\nusage: %s %s\n", argv[ac], argv[0], Usage);
		exit(1);
	    }
	} else if (isdigit(argv[ac][0])) {
	    Then = atol(argv[ac]);
	    printf("ctime(%lu)=%s", Then, ctime(&Then));
	    AnyArgs = 1;
	} else {
	    fprintf(stderr, "Unknown argument ``%s''\nusage: %s %s\n", argv[ac], argv[0], Usage);
	    exit(1);
	}
    }
    if (AnyArgs == 0) fputs(arpadate(), stdout);
    return 0;
}
