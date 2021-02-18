/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/dbgprntf.c,v 2.3 89/02/08 15:32:25 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/dbgprntf.c,v $
*/
#include <stdio.h>

/*VARARGS*/
dbgprintf(format, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20)
char   *format,
       *s1,
       *s2,
       *s3,
       *s4,
       *s5,
       *s6,
       *s7,
       *s8,
       *s9,
       *s10,
       *s11,
       *s12,
       *s13,
       *s14,
       *s15,
       *s16,
       *s17,
       *s18,
       *s19,
       *s20;
{
    safefprintf(stdout, format, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20);
}

