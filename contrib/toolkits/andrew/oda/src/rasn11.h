
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn11.h,v 1.1 89/09/21 06:36:02 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn11.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rasn11.h,v $
 * Revision 1.1  89/09/21  06:36:02  mss
 * Initial revision
 * 
 * Revision 1.10  89/08/30  16:18:14  annm
 * fixed real names to start with g
 * 
 * Revision 1.9  89/08/29  16:25:14  annm
 * added real names for names that are too long
 * 
 * Revision 1.8  88/11/03  10:47:38  mz0k
 * Added copyright
 * 
*/

#ifndef TK_INCLUDED_RASN11
#define TK_INCLUDED_RASN11

#undef EXTERN

#ifdef TK_GVARS_RASN11
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

#define	RASN1_ReadByte				grarbt
#define	RASN1_ReadBytes				grarbs
#define	RASN1_Length				gralng
#define	RASN1_ObjectType			graoty
#define	RASN1_GetString				gragst
#define	RASN1_SetValue				grasva
#define	RASN1_GetValue				gragva

#define Read2sComplement			grar2c
#define bytecount				grabct
#define doc					gradoc
#define nextbyte				granby

#endif

/* Pre-fetched next byte */
EXTERN BYTE_type		nextbyte;

/* The counter for the current position in the data stream */
EXTERN INT_type			bytecount;

/* The document descriptor external to all files */
EXTERN DOCUMENT_type		    doc;

/* Hash table header */
EXTERN HASH_type    *hash[HASHSIZE];	

EXTERN INT_type	RASN1_ReadByte();
EXTERN INT_type	RASN1_ReadBytes();
EXTERN INT_type	RASN1_Length();
EXTERN INT_type RASN1_GetInteger();
EXTERN INT_type RASN1_ObjectType();
EXTERN STRING_type RASN1_GetString();
EXTERN INT_type RASN1_SetValue();
EXTERN INT_type RASN1_GetValue();

#endif


#endif
