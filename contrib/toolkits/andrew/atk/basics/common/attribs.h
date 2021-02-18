/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/attribs.h,v 2.5 89/02/17 18:35:15 ghoti Exp $ */
/* $ACIS:attribs.h 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/attribs.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidattributes = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/attribs.h,v 2.5 89/02/17 18:35:15 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* attribs.h
 * Declartion of a property list like structure for data object attribute
 * propagation. Used by the filetype module and the dataobject_SetAttributes
 * Method.
 */

#ifndef ATTRIBUTES_DEFINED

#define ATTRIBUTES_DEFINED

struct attributes {
    struct attributes *next; /* Representation is a linked list. */
    char *key; /* attribute name. */
    union { /* Value can either be an integer or a string. */
        long integer;
        char *string;
    } value;
};
#endif /* ATTRIBUTES_DEFINED */
