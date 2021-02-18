
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr19.h,v 1.1 89/09/21 06:19:25 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr19.h,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr19.h,v $
 * Revision 1.1  89/09/21  06:19:25  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:34:43  mss
 * removed leading #
 * 
 * Revision 1.3  89/04/11  15:34:10  annm
 * support for GetContent
 * 
 * Revision 1.2  89/03/23  06:54:21  mss
 * Added extern declarations for GetContentKind and
 * GetContent.
 * 
 * Revision 1.1  89/02/24  17:14:53  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR19
#define TK_INCLUDED_ATTR19

#undef EXTERN

#ifdef TK_GVARS_ATTR19
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

EXTERN SEQUENCE_BYTE_type GetContent();
EXTERN VALUE_type	  GetContentKind();

/* Now for ATTR specific routines */

#ifdef TK_ATTR

#ifndef USE_LONG_NAMES

#define ATTR_LookOnConstituent		gatloc
#define ATTR_AccumulateContent		gatacc
#define ATTR_LookOnObjClass		gatooc
#define ATTR_LookForContentGenerator	gatfcg
#define ATTR_LookInResourceDoc		gatlrd

#define ATTR_GetObjClContentKind	gatgcl
#define ATTR_GetObjectContentKind	gatgoc
#define	ATTR_FindContentArchClass	gatfca

#endif

EXTERN INT_type ATTR_LookOnConstituent();
EXTERN INT_type ATTR_AccumulateContent();
EXTERN INT_type ATTR_LookOnObjClass();
EXTERN INT_type ATTR_LookForContentGenerator();
EXTERN INT_type ATTR_LookInResourceDoc();

EXTERN VALUE_type ATTR_GetObjClContentKind();
EXTERN VALUE_type ATTR_GetObjectContentKind();
EXTERN VALUE_type ATTR_FindContentArchClass();

#endif


#endif
