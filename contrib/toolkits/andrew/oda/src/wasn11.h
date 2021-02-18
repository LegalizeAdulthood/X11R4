
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn11.h,v 1.1 89/09/21 06:42:05 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn11.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn11.h,v $
 * Revision 1.1  89/09/21  06:42:05  mss
 * Initial revision
 * 
 * Revision 1.8  89/08/31  15:30:06  annm
 * still fixing real names
 * 
 * Revision 1.7  89/08/30  15:40:10  annm
 * added real names for names that are too long
 * 
 * Revision 1.6  88/10/31  13:13:09  mz0k
 * deleted wasn1_final and wasn1_init
 * 
 * Revision 1.5  88/09/06  13:22:35  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/08/04  14:24:18  mz0k
 * added copyright notice
 * 
*/

#ifndef TK_INCLUDED_WASN11
#define TK_INCLUDED_WASN11

#undef EXTERN

#ifdef TK_GVARS_WASN11
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


#ifdef TK_WASN11


#endif
/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_WASN1

#ifndef USE_LONG_NAMES

#define WASN1_EOC		wsneoc
#define WASN1_Length		wsnlen
#define	WASN1_Integer		wsnint
#define	WASN1_ValueType		wsnvlt

#define IO_INT_Value		gwaioi
#define Nbytes			gwanby
#define PositiveInteger		gwapos
#define TwosComplement		gwatwo

#endif


EXTERN INT_type	    WASN1_EOC();
EXTERN INT_type	    WASN1_Length();
EXTERN INT_type	    Nbytes();
EXTERN INT_type	    NegativeInteger();
EXTERN INT_type	    WASN1_Integer();
EXTERN INT_type	    WASN1_ValueType();

#endif


#endif
