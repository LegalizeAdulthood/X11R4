
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts4.h,v 1.1 89/09/21 06:26:22 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts4.h,v $

$Log:	atts4.h,v $
 * Revision 1.1  89/09/21  06:26:22  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/11  14:37:23  mss
 * removed leading #
 * 
 * Revision 1.4  89/08/29  15:27:05  annm
 * added real names for too long names
 * 
 * Revision 1.3  89/08/29  15:26:05  annm
 * added real names for too long names
 * 
 * Revision 1.2  89/08/06  08:44:07  mss
 * Changed name of SetStringAttribute to SetStringAttr
 * 
 * Revision 1.1  89/08/04  17:53:27  mss
 * Initial revision
 * 
 * Revision 1.1  89/07/19  21:04:19  mss
 * Initial revision
 * 

*/


#ifndef TK_INCLUDED_ATTS4
#define TK_INCLUDED_ATTS4

/* 

    subordinate .h file for deletion support


*/

#ifdef EXTERN
#undef EXTERN
#endif
#ifdef TK_GVARS_ATTS4
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/* Everyone sees the API routines in ATTR */
EXTERN INT_type SetFontFamily(/*FontReference, FamilyName*/);
EXTERN INT_type SetFontFamilyArray(/*FontReference, FamilyArray, NumNames*/);
EXTERN INT_type String2StrctName(/*NameString, StructuredName*/);
EXTERN INT_type Array2StrctName(/*NameArray, NumNames, StructuredName*/);
EXTERN INT_type SetStringAttr(/*constituent, attribute, data, chars*/);
EXTERN INT_type AppendSubordinate(/*parent,child*/);

/* Constants for SetStringAttr */
#define	DATA_CHAR_TYPE	(BOOL_true)
#define	DATA_BYTE_TYPE	(BOOL_false)

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_ATTS

#ifndef USE_LONG_NAMES

#define BreakUpName	gasbun

#endif


#endif

/*
    Declarations seen within the Attribute Support module go here.
*/

#ifdef TK_IMPL_ATTS

#endif

#endif
