/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 *      Common defines and typedefs for command parsing routines
 */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/msclients/vui/RCS/cmdargs.h,v 2.3 89/02/15 12:12:39 ghoti Exp $ */


#ifndef PRIVATE
#define PRIVATE static
#endif /* PRIVATE */

#ifndef NIL
#define NIL  (char *)0
#endif /* NIL */

#ifndef TRUE
#define TRUE    1
#endif /* TRUE */

#ifndef FALSE
#define FALSE   0
#endif /* FALSE */

#ifndef ERROR
#define ERROR(prtargs)\
            if (TRUE)\
            {printf("Error: ");printf prtargs;printf("\n");fflush(stdout);\
            exit(-1);}\
            else
#endif /* ERROR */

typedef struct {
    char option, reqarg;
    char **target;
    int  (*handler)();
    } STABLE;

typedef char **PTABLE;

