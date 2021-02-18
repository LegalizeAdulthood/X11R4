
/*

$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/font1.h,v 1.1 89/09/21 06:30:43 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/font1.h,v $

		(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	font1.h,v $
 * Revision 1.1  89/09/21  06:30:43  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/01  13:17:58  mss
 * Removed mapping ds2int (is mapped name!)
 * 
 * Revision 1.4  89/08/29  15:57:29  annm
 * added real names for too long name
 * 
 * Revision 1.3  88/09/19  13:13:06  annm
 * fixed tkerr.h include problem (no GENINCLDIR
 * 
 * Revision 1.2  88/09/14  17:41:12  jr
 * Remove dupl defns for routines.
 * 
 * Revision 1.1  88/09/12  17:42:21  jr
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_FONT1
#define TK_INCLUDED_FONT1

#undef EXTERN

#ifdef TK_GVARS_FONT1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN INT_type		    UnspecifyFont();
EXTERN SEQUENCE_INT_type    Dsclass2Ints();
EXTERN VALUE_type	    Ints2Dsclass();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_FONT

#ifndef USE_LONG_NAMES

#define FONT_Init  fntint
#define	FONT_Final fntfnl

#define UnspecifyModalFont	gftumf
#define UnspecifyScore		gftusr
#define UnspecifyStructName	gftusn
#define UnspecifyVarScript	gftuvs

#define dsclasses		gftdsc

#endif

EXTERN INT_type FONT_Final();
EXTERN INT_type FONT_Init();

#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_FONT

#endif

#endif
