
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif4.h,v 1.1 89/09/21 06:38:48 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif4.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif4.h,v $
 * Revision 1.1  89/09/21  06:38:48  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:40:36  mss
 * removed leading #
 * 
 * Revision 1.3  89/08/30  16:30:09  annm
 * real names now start with g
 * 
 * Revision 1.2  89/08/29  17:37:43  annm
 * added short real names for names that are too long
 * 
 * Revision 1.1  88/10/21  17:00:27  mz0k
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_RODIF4
#define TK_INCLUDED_RODIF4

#undef EXTERN

#ifdef TK_GVARS_RODIF4
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

#ifdef TK_TKI_RODIF


#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_RODIF

#ifndef USE_LONG_NAMES

#define	RODIF_DocumentProfile			grodpr

#define RODIF_AddDocCharacteristics		groadc
#define RODIF_DocumentCharacteristics		grodch
#define RODIF_FontList				groflt
#define RODIF_FontReference			grofrf
#define RODIF_FontsList				grofsl    
#define RODIF_ModalFontAttrs			gromfa
#define RODIF_VariantScript			grovsc
#define RODIF_VariantScripts			grovss
#define RODIF_WritingModes			growmd

#endif

EXTERN INT_type	RODIF_DocumentProfile();

#endif

#endif
