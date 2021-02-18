
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/doc.h,v 1.2 89/09/29 11:40:05 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/doc.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	doc.h,v $
 * Revision 1.2  89/09/29  11:40:05  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:28:48  mss
 * Initial revision
 * 
 * Revision 1.26  88/09/06  13:19:39  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.25  88/09/06  12:04:29  annm
 * 
 * no change
 * 
 * Revision 1.24  88/06/13  13:08:20  jr
 * Remove dupl defn of DOCUMENT_type
 * 
 * Revision 1.23  88/06/08  16:56:15  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.22  88/06/08  14:35:21  annm
 * added Copyright notice
 * 
*/

#ifndef	TK_INCLUDED_DOC
#define	TK_INCLUDED_DOC

#ifdef TK_TKI_DOC
#define TK_DOC
#else
#ifdef TK_IMPL_DOC
#define	TK_DOC
#else
#define AP_DOC
#endif
#endif

#include <lcsite.h>

#include <errors.h>
#include <apirt.h>

#include <tkerr.h>

/* Data structure for a document */

typedef POINTER_type	DOCUMENT_type;

#ifdef TK_DOC

#endif

#ifdef TK_TKI_DOC

#endif

#ifdef TK_IMPL_DOC

typedef struct DOCUMENT_struct {
    
    /* field for linking documents together in a doubly linked list */

    struct DOCUMENT_struct   *forward;
    struct DOCUMENT_struct   *back;


    /* pointer to the document profile  */

    CONSTITUENT	    document_profile;


    /* pointers to the document roots */

    CONSTITUENT	    specific_logical_root;
    CONSTITUENT	    specific_layout_root;
    CONSTITUENT	    generic_logical_root;
    CONSTITUENT	    generic_layout_root;


    /* pointers to the styles */
    /* Both a circular doubly linked lists. */

    struct CONSTITUENT_struct	    layout_styles_list;
    struct CONSTITUENT_struct	    presentation_styles_list;

    /* 
	doubly linked list of constituents in this doucment.
	does not include the styles or the doc profile.
    */

    struct CONSTITUENT_struct	    constituent_list;

    /* Here's some debugging stuff (used in DEBUG module) */

	/* The id # for this document */
    INT_type			    id;
	/* The next id available for a constituient of this document */
    INT_type			    next_constituent_id;

} *INTERNAL_DOCUMENT_type;

#endif

/* Everyone can see these */

#define ERROR_DOCUMENT	((DOCUMENT_type) 0)

#include <doc1.h>

#endif
