
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn13.h,v 1.2 89/11/01 16:09:50 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn13.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rasn13.h,v $
 * Revision 1.2  89/11/01  16:09:50  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.1  89/09/21  06:36:42  mss
 * Initial revision
 * 
 * Revision 1.6  89/08/31  15:55:22  annm
 * still fixing real names
 * 
 * Revision 1.5  89/08/30  16:18:25  annm
 * fixed real names to start with g
 * 
 * Revision 1.4  88/11/03  10:48:50  mz0k
 * Added copyright
 * 
*/

#ifndef TK_INCLUDED_RASN13
#define TK_INCLUDED_RASN13

#undef EXTERN

#ifdef TK_GVARS_RASN13
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

#define	RASN1_PageAttributes		    grapat
#define	RASN1_FrameAttributes		    grafat
#define	RASN1_BlockAttributes		    grabat
#define	RASN1_RasterGrCodingAttrs	    grarga
#define	RASN1_GetIntPair		    gragip

#define RASN1_RasterAttributes		    grarat
#define RASN1_RgClipping		    grargc
#define RASN1_RgImageDimensions		    grarid
#define RASN1_RgPelSpacing		    grarps
#define RASN1_RgSetAreaControlled	    grarac
#define RASN1_RgSetSpacing		    grarss

#endif

EXTERN INT_type	RASN1_PageAttributes();
EXTERN INT_type	RASN1_FrameAttributes();
EXTERN INT_type	RASN1_BlockAttributes();
EXTERN INT_type	RASN1_RasterAttributes();
EXTERN INT_type	RASN1_RasterGrCodingAttrs();
EXTERN INT_type	RASN1_GetIntPair();

#endif


#endif
