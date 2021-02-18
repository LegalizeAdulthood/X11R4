
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn15.h,v 1.1 89/09/21 06:43:38 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn15.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn15.h,v $
 * Revision 1.1  89/09/21  06:43:38  mss
 * Initial revision
 * 
 * Revision 1.3  89/08/31  15:30:27  annm
 * still fixing real names
 * 
 * Revision 1.2  89/08/30  15:53:27  annm
 * added real names for names that are too long
 * 
 * Revision 1.1  88/10/21  16:32:49  mz0k
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_WASN15
#define TK_INCLUDED_WASN15

#undef EXTERN

#ifdef TK_GVARS_WASN15
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

#ifdef TK_WASN15


#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_WASN1

#ifndef USE_LONG_NAMES

#define	WASN1_Seq9541IdType		    gwsns9
#define	WASN1_9541IdType		    gwsn95
#define	WASN1_StructuredName		    gwsnst
#define	WASN1_SeqIntsInStruct		    gwsnsi
#define	WASN1_Modes			    gwsnmo

#define WASN1_ModalFontAttrs		    gwamfa
#define WASN1_VariantScript		    gwavst
#define WASN1_VariantScripts		    gwavss

#endif

EXTERN INT_type WASN1_Seq9541IdType();
EXTERN INT_type WASN1_9541IdType();
EXTERN INT_type	WASN1_StructuredName();
EXTERN INT_type	WASN1_SeqIntsInStruct();
EXTERN INT_type WASN1_Modes();

#endif


#endif
