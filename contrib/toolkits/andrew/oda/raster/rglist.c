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
 *  File:   rglist.c
 *  Author: Andrew Werth
 *
 *  Handles generic list creation and manipulation.
 *
 */

#define TK_TKI_mem
#define TK_TKI_list
#define TK_IMPL_list
#define TK_GVARS_rglist

#include <rgsite.h>
#include <rgmem.h>
#include <rglist.h>

#undef TK_GVARS_rglist
#undef TK_IMPL_list
#undef TK_TKI_list
#undef TK_TKI_mem

/***
static LIST_t	    A_NULL_LIST;
#define NULL_POINTER ((POINTER_type) NULL)
#define NULL_LIST   &A_NULL_LIST;
***/

/*\
 *  MAKELIST:  Create the list structure.
\*/

LIST_t *LST_MakeList()
{
    LIST_t	*rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    rval = (LIST_t *) MEM_Malloc(sizeof(LIST_t));
    rval->head = NULL_LISTEL;
    rval->tail = NULL_LISTEL;
    rval->current = NULL_LISTEL;
    rval->prev = NULL_LISTEL;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}

/*\
 *  LST_ADDELEMENT:  Add an element to the list.
\*/

INT_type LST_AddElement(list,element)
    POINTER_type    element;
    LIST_t	    *list;
{
    INT_type	    rval;
    LISTEL_t	    *listel;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    listel = (LISTEL_t *) MEM_Malloc(sizeof(LISTEL_t));
    if (listel == (LISTEL_t *) RG_ERROR_POINTER) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    listel->next = NULL_LISTEL;
    listel->element = element;

    if (list->head == NULL_LISTEL) {
	list->head = listel;
	list->tail = listel;
    }
    else {
	list->tail->next = listel;
	list->tail = listel;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  LST_DELETEELEMENT:  Delete an element of a list.
\*/

INT_type LST_DeleteElement(list, element)
    IN	LIST_t		*list;
    IN	POINTER_type	element;
{
    INT_type	rval;
    LISTEL_t	*listel;
    LISTEL_t	*prev;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    listel = list->head;
    prev = NULL_LISTEL;

    while (listel->next != NULL_LISTEL) {

	/* Is current element the one to delete? */

	if (listel->element == element) {

	    if (listel == list->head) {
		/* HEAD OF LIST DELETION */
		list->head = listel->next;
		if (listel == list->tail) {
		    list->tail = NULL_LISTEL;
		}
		MEM_Free(listel);
	    }
	    else if (listel == list->tail) {
		/* TAIL OF LIST DELETION */
		list->tail = prev;
		prev->next = NULL_LISTEL;
		MEM_Free(listel);
	    }
	    else {
		/* DELETE ELEMENT IN MIDDLE OF LIST */
		prev->next = listel->next;
		MEM_Free(listel);
	    }
	}
	prev = listel;
	listel = listel->next;
    }
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}




/*\
 *  LST_MOVEHEAD:  Move to the head of the list.
\*/

INT_type LST_MoveHead(list)
    LIST_t	*list;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    list->current = list->head;
    list->prev = NULL_LISTEL;
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  LST_NEXTELEMENT:  Move to the next element in the list.
\*/

POINTER_type LST_NextElement(list)
    LIST_t	    *list;
{
    POINTER_type    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (list->current == RG_NULL) {
	rval = RG_ERROR_POINTER;
	RGError = 0;
    }
    else {
	rval = list->current->element;	
        list->current = list->current->next;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}

/*\
 *  LST_FREELIST:  Free up the entire list.
\*/

INT_type LST_FreeList(list)
    LIST_t  *list;
{
    INT_type	rval;
    LISTEL_t	*current;
    LISTEL_t	*nextel;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    for (current = list->head; current != NULL_LISTEL;
				current = nextel) {
	nextel = current->next;	    
	if (MEM_Free(current) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }
    rval = MEM_Free(list);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*  End of file .c  */
