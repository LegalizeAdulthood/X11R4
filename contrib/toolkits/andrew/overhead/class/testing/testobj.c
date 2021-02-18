/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* test object for dynamic loader */

#include "class.h"
#include "testobj.eh"

boolean testobject__InitializeClass()
{
    printf("InitializeClass entered.\n");
    return TRUE;
}

boolean testobject__InitializeObject()
{
    printf("InitializeObject entered.\n");
    return TRUE;
}

void testobject__Diddle(X, n)
struct testobject *X;
int n;
{
    printf("Diddle entered, n=%d\n", n);
}

