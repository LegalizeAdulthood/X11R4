
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn14.h,v 1.1 89/09/21 06:43:10 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn14.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn14.h,v $
 * Revision 1.1  89/09/21  06:43:10  mss
 * Initial revision
 * 
 * Revision 1.8  89/08/31  15:30:23  annm
 * still fixing real names
 * 
 * Revision 1.7  89/08/30  15:51:03  annm
 * added real names for names that are too long
 * 
 * Revision 1.6  88/10/13  22:03:06  mz0k
 * added raster attributes and raster coding attributes
 * 
 * Revision 1.5  88/09/06  13:22:50  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/08/04  17:46:44  jr
 * Change TK_IMPL_WASN14 -> TK_IMPL_WASN1
 * 
 * Revision 1.3  88/08/04  14:39:18  mz0k
 * added copyright notice
 * 
*/

#ifndef TK_INCLUDED_WASN14
#define TK_INCLUDED_WASN14

#undef EXTERN

#ifdef TK_GVARS_WASN14
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

#ifdef TK_WASN14


#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_WASN1

#ifndef USE_LONG_NAMES

#define	WASN1_PageAttributes		    gwsnpa
#define	WASN1_MediumType		    gwsnmd
#define	WASN1_DefaultDimensions		    gwsndd
#define	WASN1_Dimensions		    gwsndm
#define	WASN1_FrameAttributes		    gwsnfa
#define	WASN1_DefaultPosition		    gwsndp
#define	WASN1_BlockAttributes		    gwsnba
#define	WASN1_RgClipping		    gwsnrg
#define	WASN1_RgPelSpacing		    gwsnps
#define	WASN1_RasterCodingAttributes	    gwsnrc
#define	WASN1_RgImageDimensions		    gwsnri

#endif


EXTERN INT_type		WASN1_PageAttributes();
EXTERN INT_type		WASN1_MediumType();
EXTERN INT_type		WASN1_DefaultDimensions();
EXTERN INT_type		WASN1_Dimensions();
EXTERN INT_type		WASN1_FrameAttributes();
EXTERN INT_type		WASN1_DefaultPosition();
EXTERN INT_type		WASN1_BlockAttributes();
EXTERN INT_type		WASN1_RgClipping();
EXTERN INT_type		WASN1_RgPelSpacing();
EXTERN INT_type		WASN1_RasterCodingAttributes();
EXTERN INT_type		WASN1_RgImageDimensions();

#endif


#endif
