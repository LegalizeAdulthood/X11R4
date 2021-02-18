/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/nonfatal.c,v 2.3 89/02/08 15:43:54 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/nonfatal.c,v $
*/
#include <stdio.h>
#include <errprntf.h>

/* This is in a separate file to make it easy for a no-snap client to override it if it so desires. */


NonfatalBizarreError(text)
char *text;
{
    BizarreError(text, ERR_WARNING);
}

