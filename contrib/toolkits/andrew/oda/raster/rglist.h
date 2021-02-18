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
 *  File:   list.h
 *  Author: Andrew Werth
 *
 *  Header info for list handler routines.
 *
 */

#ifndef TK_INCLUDED_list
#define TK_INCLUDED_list

#ifdef TK_TKI_list
#define TK_list
#else
#ifdef TK_IMPL_list
#define	TK_list
#else
#define AP_list
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_list.

*/



/* 
    EVERYONE SEES THIS
*/

#ifdef TK_IMPL_list

typedef struct LISTEL_struct {
    POINTER_type	    element;
    struct LISTEL_struct    *next;
} LISTEL_t;

#else
typedef POINTER_type LISTEL_t;
#endif

typedef struct {
    LISTEL_t	    *head;
    LISTEL_t	    *tail;
    LISTEL_t	    *current;
    LISTEL_t	    *prev;
} LIST_t;

#define NULL_LIST	((LIST_t *) RG_NULL)
#define ERROR_LIST	((LIST_t *) 1)

LIST_t		*LST_MakeList();
INT_type	LST_FreeList();
INT_type	LST_MoveHead();
INT_type	LST_AddElement();
POINTER_type	LST_NextElement();

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_list

#define NULL_LISTEL	((LISTEL_t *) RG_NULL)

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_list

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
