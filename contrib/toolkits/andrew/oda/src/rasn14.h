
/*

$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn14.h,v 1.1 89/09/21 06:37:00 mss Exp $

$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn14.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.


$Log:	rasn14.h,v $
 * Revision 1.1  89/09/21  06:37:00  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:39:47  mss
 * removed leading #
 * 
 * Revision 1.3  89/08/30  16:18:37  annm
 * fixed real names to start with g
 * 

*/

#ifndef TK_INCLUDED_RASN14
#define TK_INCLUDED_RASN14

#undef EXTERN

#ifdef TK_GVARS_RASN14
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/*
    Declarations seen only inside the tool kit are seen here
*/


#ifdef TK_RASN1


#endif
/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_RASN1

#ifndef USE_LONG_NAMES

#define	RASN1_StructuredName		    grasnm

#define RASN1_CompleteStructName	    gracsn
#define RASN1_Identifier		    graidn

#endif

EXTERN INT_type	RASN1_StructuredName();

#endif


#endif
