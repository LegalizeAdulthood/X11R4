
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr9.h,v 1.1 89/09/21 06:24:46 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr9.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr9.h,v $
 * Revision 1.1  89/09/21  06:24:46  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/11  14:32:36  mss
 * removed leading #
 * 
 * Revision 1.4  89/08/29  13:27:49  annm
 * added real names for too long names
 * 
 * Revision 1.3  89/03/09  15:54:37  annm
 * moved the subroutines specific to DVL defaulting from attr8.c to attr9.c
 * 
 * Revision 1.2  88/09/06  13:18:56  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.1  88/08/16  18:13:55  annm
 * Initial revision
 * 
*/
#ifndef TK_INCLUDED_ATTR9
#define TK_INCLUDED_ATTR9

#undef EXTERN

#ifdef TK_GVARS_ATTR9
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

#define ERROR_VALUE	((VALUE_type) 0)

/* real names for routines in attr9.c */

#ifndef USE_LONG_NAMES

#define ATTR_InheritFromDVL		    gatdvl
#define ATTR_AttemptInheritFromDVL	    gatadv
#define ATTR_InheritFromDVLConstituent	    gatdvc

#define	    ATTR_FindRightObjectType	gatfro
#define	    ATTR_GetConstituentKind	gatgck
#define	    ATTR_IsLayoutDirective	gatild
#define	    ATTR_IsPresentationAttribute    gatipa
#define	    ATTR_FindConstituent	gatfco

#define IsLayoutDirective		gatild
#define IsPresentationAttribute		gatipa

#endif


/* declarations for routines in attr9.c */

EXTERN INT_type ATTR_InheritFromDVL();
EXTERN INT_type ATTR_AttemptInheritFromDVL();
EXTERN INT_type ATTR_InheritFromDVLConstituent();

EXTERN CONSTITUENT ATTR_FindRightObjectType();
EXTERN VALUE_type ATTR_GetConstituentKind();
EXTERN INT_type IsLayoutDirective();
EXTERN INT_type IsPresentationAttribute();
EXTERN CONSTITUENT ATTR_FindConstituent();

#endif

#endif

