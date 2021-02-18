/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* Modified 14-Nov-89 by bobg@andrew.cmu.edu */

#include <stdio.h>

static void (*errfn)() = ((void (*) ()) 0);

void (*regerrhandler(fn))()
void (*fn)();
{
    void (*oldfn)() = errfn;

    errfn = fn;
    return (oldfn);
}

void            regerror(s)
char           *s;
{
    if (errfn == ((void (*) ()) 0))
        fprintf(stderr, "regexp: %s\n", s);
    else
        (*errfn)(s);
}
