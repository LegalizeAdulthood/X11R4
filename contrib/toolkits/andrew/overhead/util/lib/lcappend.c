/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/lcappend.c,v 2.3 89/02/10 23:26:33 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/lcappend.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

/*
  lcappend.c

  LCappend(s1, s2): append a lower-alpha-case copy of s2 to s1.
      */

#include <ctype.h>

LCappend(s1, s2)
char *s1, *s2;
{
    char *e1;

    for (e1 = &s1[strlen(s1)]; *s2 != '\0'; ++e1) {
	*e1 = *s2++;
	if (isupper(*e1)) *e1 = tolower(*e1);
    }
    *e1 = '\0';
}
#ifdef TESTINGONLYTESTING
#include <stdio.h>
main()
{
    char a[100];
    strcpy(a, "This is nice: ``");
    LCappend(a, "Andrew.CMU.EDU");
    strcat(a, "''.\n");
    fputs(a, stdout);
}
#endif /* TESTINGONLYTESTING */
