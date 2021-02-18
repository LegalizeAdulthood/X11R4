/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* test main program for dynamic loader */

#include <stdio.h>

#include "class.h"
#include "testobj.ih"
#include "testobj2.ih"


int debug = 1;

int caller(victim)
int (*victim)();
{
    int i, j;

    printf("In caller.\n");
    j = 1;
    i = victim(j);
    printf("Caller returning.\n");
    return i;
}

int callee(k)
int k;
{
    int n;

    printf("in callee, k = %d.\n", k);
    n = k + 1;
    return n;
}


main()
{
    struct testobject *X;
    struct testobject2 *Y;
    int m;
    char *bogus;

    class_Init(".");
    class_SetClassPath(".");
    printf("In main, beginning.\n");
    m = caller(callee);
    printf("first call done.\n");
    bogus = (char *)callee;
    m = (*(int (*)())bogus)(2);
    printf("second call done.\n");
    X = testobject_New();
    Y = testobject2_New();
    testobject_Diddle(X, 3);
    printf("In main, ending.\n");
    printf(" ...looping....\n");
    while (1);

}

