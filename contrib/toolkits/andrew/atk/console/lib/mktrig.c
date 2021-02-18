/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/mktrig.c,v 2.4 89/02/17 15:31:01 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/mktrig.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/mktrig.c,v 2.4 89/02/17 15:31:01 ghoti Exp $";
#endif /* lint */

/*************************************************************

	mktrig.c

	Fills trig tables and creates a trig.h file
	that allows them to be compiled into draw.c

*/

#include <math.h>
#define PI 3.14159265

main() {
    int i;
    double f,g;

    f = 180.0/PI;
    printf("int SineMult[] = {\n");
    for (i=0; i<430; ++i) {
	g = (double) i / f;
	printf("\t%d,\n", (int) (sin(g) * 10000));
    }
    printf("};\n\n");
    printf("int CosineMult[] = {\n");
    for (i=0; i<430; ++i) {
	g = (double) i / f;
	printf("\t%d,\n", (int) (cos(g) * 10000));
    }
    printf("};\n\n");
}


