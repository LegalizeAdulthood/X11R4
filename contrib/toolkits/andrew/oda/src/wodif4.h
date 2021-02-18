
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif4.h,v 1.1 89/09/21 06:45:23 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif4.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wodif4.h,v $
 * Revision 1.1  89/09/21  06:45:23  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/11  14:41:52  mss
 * removed leading #
 * 
 * Revision 1.5  89/09/01  10:51:53  mss
 * Made unique names
 * 
 * Revision 1.4  89/08/31  15:55:42  annm
 * still fixing real names
 * 
 * Revision 1.3  89/08/30  16:10:50  annm
 * added real names for names that are too long
 * 
 * 
 * Revision 1.2  88/10/21  16:47:08  mz0k
 * added font stuffs
 * 
 * Revision 1.1  88/10/13  22:19:54  mz0k
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_WODIF4
#define TK_INCLUDED_WODIF4

#undef EXTERN

#ifdef TK_GVARS_WODIF4
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

#ifdef TK_WODIF

#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_WODIF

#ifndef USE_LONG_NAMES

#define	WODIF_RasterGraphicsAttributes	    gworga
#define	WODIF_DocumentProDescriptor	    gwodpd

#define WODIF_AdditionalDocChars	    gwoadc
#define WODIF_DocumentCharacteristics	    gwodch
#define WODIF_FontPair			    gwofpr
#define WODIF_FontReference		    gwofrf
#define WODIF_FontsList			    gwofls

#endif


EXTERN INT_type			WODIF_RasterGraphicsAttributes();
EXTERN INT_type			WODIF_DocumentProDescriptor();

#endif

#endif
