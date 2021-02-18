/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/*
 * $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/lib/RCS/eerror.c,v 2.6 89/06/17 15:38:23 bobg Exp $
 *
 * $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/lib/RCS/eerror.c,v $
 */

#include  <eerror.h>

static char    *eliErrStrs[] =
{
    "Error from ELI client",
    "Out of Memory",
    "Bad Syntax",
    "Unbound Atom",
    "Undefined Function",
    "Bad Function Parameter",
    "Bad Argument",
    "Symbol Does Not Exist",
    "Wrong Number of Arguments to Function",
    "System Error",
    "User Error"
};

/* Definition of EliError, the all-purpose error-handler */

void            EliError(st, errtype, node, loc, unixerr)
EliState_t     *st;
int             errtype;
EliSexp_t      *node;
char           *loc;
int unixerr;
{
    eliErr_Set(st, EliErrNode(st), errtype, node, loc, unixerr);
    st->g_errflag = TRUE;
    if (errtype & EliCatchMask(st))
        (*(EliCatchFn(st))) (st);
}

char           *EliErrStr(code)        /* If code is a power of two <= 128,
                                        * this will return a string from
                                        * eliErrStrs. */
int             code;
{
    int             i, j;

    for (i = code, j = 0; i; (i = (i >> 1)), ++j);
    return (eliErrStrs[j]);
}

void            eliyyerror(s)
char           *s;
{
}

int             eliyywrap()
{
    EliProcessInfo.u_wrap = TRUE;
    return (1);
}
