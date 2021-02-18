
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr1.h,v 1.1 89/09/21 06:16:00 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr1.h,v $
 * Revision 1.1  89/09/21  06:16:00  mss
 * Initial revision
 * 
 * Revision 1.34  89/09/11  14:30:58  mss
 * removed leading #
 * 
 * Revision 1.33  89/09/01  14:17:51  annm
 * fixing USES_LONG_NAMES problems
 * 
 * Revision 1.32  89/08/30  16:57:22  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.31  89/01/26  12:41:54  annm
 * no change
 * 
 * Revision 1.30  88/12/13  19:58:00  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.29  88/09/30  15:39:52  annm
 * no change
 * 
 * Revision 1.28  88/09/06  13:17:39  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.27  88/09/06  12:03:41  annm
 * 
 * no change
 * 
 * Revision 1.26  88/08/01  18:44:14  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.25  88/07/29  10:37:02  mss
 * Added constant definitions for HasIndepParm;
 * Added exported functino def for HasIndepParm
 * 
 * Revision 1.24  88/07/15  18:35:34  annm
 * no change
 * 
 * Revision 1.23  88/07/14  18:13:04  annm
 * no changes
 * 
 * Revision 1.22  88/07/07  19:14:21  annm
 * changed dcl of GetAttrName and GetValueName to returning CHAR_type *
 * 
 * Revision 1.21  88/07/07  18:49:30  annm
 * no change - wierdness during attempting to check in 
 * 
 * Revision 1.20  88/06/17  15:02:44  annm
 * changes to Set/Get Attr to:
 * 	1) on set attr side: plug in
 * 		more attributes whose
 * 		types are already there.
 * 	2) on Get side: Get now does all
 * 	the types and attributes that 
 * 	Set does
 * 
 * Revision 1.19  88/06/08  16:53:02  annm
 * dealt with line too long in copyright notice and changed
 * #end's to #endif's
 * 
 * Revision 1.18  88/06/08  14:34:05  annm
 * added Copyright notice
 * 
*/


#ifndef TK_INCLUDED_ATTR1
#define TK_INCLUDED_ATTR1

#undef EXTERN

#ifdef TK_GVARS_ATTR1
#define EXTERN
#else
#define EXTERN extern
#endif

/* Everyone sees the API routines in ATTR */

EXTERN	INT_type	GetAttr();
EXTERN  INT_type	GetIntAttr();
EXTERN  INT_type	GetValueAttr();
EXTERN	CHAR_type	*GetAttrName();
EXTERN	INT_type	GetParmMask();
EXTERN	CHAR_type	*GetValueName();
EXTERN	INT_type	HasAttr();
EXTERN	INT_type	RemAttr();
EXTERN	INT_type	SetAttr();
EXTERN  INT_type	SetIntAttr();
EXTERN  INT_type	SetValueAttr();
EXTERN	INT_type	UnspecParms();

#ifdef TK_ATTR

#ifndef USE_LONG_NAMES

#define	ATTR_Init   gatint	    	    
#define ATTR_Final  gatfnl	    

#endif

EXTERN INT_type ATTR_Final();
EXTERN INT_type ATTR_Init();

#endif

#ifdef TK_IMPL_ATTR

/* the error attribute */
#ifndef USE_LONG_NAMES

#define	ERROR_ATTRIBUTE	    geratt

#else

#define ERROR_ATTRIBUTE Error_Attribute

#endif

EXTERN ATTR_attribute_t ERROR_ATTRIBUTE;

#ifdef TK_ATTR

/* the NULL attribute */

#ifndef USE_LONG_NAMES

#define NULL_ATTRIBUTE	    gnlatt

#else

#define NULL_ATTRIBUTE Null_Attribute

#endif

EXTERN ATTR_attribute_t NULL_ATTRIBUTE;

#endif

#endif

#endif

