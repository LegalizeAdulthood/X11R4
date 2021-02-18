/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* test object for dynamic loader */

#include "class.h"
#include "testobj2.eh"

boolean testobject2__InitializeClass()
{
    printf("InitializeClass entered.\n");
    return TRUE;
}

boolean testobject2__InitializeObject()
{
    printf("InitializeObject entered.\n");
    return TRUE;
}

void testobject2__Diddle(X, n)
struct testobject2 *X;
int n;
{
    printf("Diddle entered, n=%d\n", n);
}

