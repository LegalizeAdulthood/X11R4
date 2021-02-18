/*\
 *
 *  (C) Copyright 1989 by Carnegie Mellon University
 *
 *  Permission to use, copy, modify, and distribute these programs
 *  and their documentation for any purpose and without fee is
 *  hereby granted, provided that this copyright and permission
 *  notice appear on all copies and supporting documentation, and
 *  that the name of Carnegie Mellon University not be used in
 *  advertising or publicity pertaining to distribution of the
 *  programs without specific prior permission and notice be given
 *  in supporting documentation that copying and distribution is
 *  by permission of Carnegie Mellon University.
 *
 *  Carnegie Mellon University makes no representations about the
 *  suitability of this software for any purpose.  It is provided
 *  as is, without express or implied warranty.
 *
 *  Software by Andrew Werth, Information Technology Center,
 *  Carnegie Mellon University, except where noted.
\*/

/*
 *  File:   rgdbg.c
 *  Author: Andrew Werth
 *
 *  Temorary debugging routines TraceRoutine() and Entering().
 *
 */

#include <rgsite.h>

static INT_type	    Level = 0;
static CHAR_type    *Spaces =
    "                                                           ";

void Entering(routine)
    CHAR_type *routine;
{
    INT_type	len;
    CHAR_type	*ch;

    len = 0;
    ch = routine;
    while (*ch != (CHAR_type) '\0') {
	len++;
	ch++;
    }
    LOCAL_WriteBytes(2,Spaces, Level);
    LOCAL_WriteBytes(2,"Entering: ",10);
    LOCAL_WriteBytes(2,routine,len);
    LOCAL_WriteBytes(2,"\n",1);
    Level++;
}

void Leaving(routine)
    CHAR_type *routine;
{
    INT_type	len;
    CHAR_type	*ch;

    Level--;
    len = 0;
    ch = routine;
    while (*ch != (CHAR_type) '\0') {
	len++;
	ch++;
    }
    LOCAL_WriteBytes(2,Spaces, Level);
    LOCAL_WriteBytes(2,"Leaving:  ",10);
    LOCAL_WriteBytes(2,routine,len);
    LOCAL_WriteBytes(2,"\n",1);
}

/*  End of file .c  */
