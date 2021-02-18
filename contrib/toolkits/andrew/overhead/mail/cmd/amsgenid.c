/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/mail/cmd/RCS/amsgenid.c,v 1.4 89/02/10 23:17:19 ghoti Exp $

ams_genid -- generate a world-unique identifier on stdout for usage in scripts.

*/

#include <stdio.h>
#include <mailconf.h>
#include <mail.h>

main(argc, argv)
int argc;
unsigned char **argv;
{
    int i;
    int isFile = 0;
    
    for (i = 1; i < argc; ++i) {
	if (strcmp(argv[i], "-f") == 0) isFile = 1;
	else {
		fprintf(stderr, "usage: %s [-f]\n", argv[0]);
		exit(2);
	}
    }
    puts(ams_genid(isFile));
    exit(0);
}
