
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts1.h,v 1.1 89/09/21 06:25:25 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts1.h,v $

(c) Copyright 1989 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	atts1.h,v $
 * Revision 1.1  89/09/21  06:25:25  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:36:49  mss
 * removed leading #
 * 
 * Revision 1.3  89/08/29  13:30:00  annm
 * added real names for too long names
 * 
 * Revision 1.2  89/03/27  12:32:39  mss
 * Shortedn name of parameter to less tahn 31 chars
 * 
 * Revision 1.1  89/03/21  17:13:05  mss
 * Initial revision
 * 

*/


#ifndef TK_INCLUDED_ATTS1
#define TK_INCLUDED_ATTS1

/* 

    model for the subordinate .h file

    put the global comment of the .h file here

*/

#ifdef EXTERN
#undef EXTERN
#endif
#ifdef TK_GVARS_ATTS1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/* Everyone sees the API routines in ATTR */

EXTERN	INT_type	UnspecifyDefaultValueLists();
EXTERN  INT_type	InitializeDefaultValueLists();

#define PAGE_ATTRIBUTES_parm	((PARM_MASK_type) (1<<2))
#define FRAME_ATTRIBUTES_parm	((PARM_MASK_type) (1<<3))
#define BLOCK_ATTRIBUTES_parm	((PARM_MASK_type) (1<<4))
#define COMP_LOGICAL_ATTRIBUTES_parm	((PARM_MASK_type) (1<<5))
#define BAS_LOGICAL_ATTRIBUTES_parm	((PARM_MASK_type) (1<<6))

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_ATTS

#ifndef USE_LONG_NAMES

#define	ATTS_Init   gasint
#define ATTS_Final  gasfnl	    

#define ATTS_FillInOneDefaultConst  gasfod

#endif

EXTERN INT_type ATTS_Final();
EXTERN INT_type ATTS_Init();


#endif

/*
    Declarations seen within the Attribute Support module go here.
*/

#ifdef TK_IMPL_ATTS

#endif

#endif
