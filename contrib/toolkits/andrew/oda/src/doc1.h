
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/doc1.h,v 1.1 89/09/21 06:29:10 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/doc1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	doc1.h,v $
 * Revision 1.1  89/09/21  06:29:10  mss
 * Initial revision
 * 
 * Revision 1.20  89/09/11  14:36:26  mss
 * removed leading #
 * 
 * Revision 1.19  89/09/01  15:41:46  annm
 * fixing USE_LONG_NAMES problems
 * 
 * Revision 1.18  89/08/29  15:51:18  annm
 * added real names for too long name
 * 
 * Revision 1.17  89/04/17  14:49:02  annm
 * implemented DeleteDocument
 * 
 * Revision 1.16  88/09/06  13:19:44  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.15  88/09/06  12:04:35  annm
 * 
 * no change
 * 
 * Revision 1.14  88/06/08  16:56:26  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.13  88/06/08  14:35:30  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_DOC1
#define TK_INCLUDED_DOC1

#undef EXTERN

#ifdef TK_GVARS_DOC1
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the exported procedures */

EXTERN	INT_type	DeleteDocument();
EXTERN	DOCUMENT_type	MakeDocument();

#ifdef TK_DOC

#ifndef USE_LONG_NAMES

#define DOC_Init    gdcint	
#define DOC_Final   gdcfnl	

#endif

EXTERN INT_type DOC_Final();
EXTERN INT_type DOC_Init();

#endif


/* head of the list of documents */

#ifdef TK_IMPL_DOC

#ifndef USE_LONG_NAMES

#define DOC_ZapConstituents	gdoczc

#define next_document_id	gdocid

#endif

EXTERN INT_type DOC_ZapConstituents();

/* aliases */

#ifndef USE_LONG_NAMES

#define	DOC_DocumentList gdoclt

#else

#define DOC_DocumentList DOC_TheDocumentList

#endif

EXTERN struct DOCUMENT_struct DOC_DocumentList;

#endif

#endif
