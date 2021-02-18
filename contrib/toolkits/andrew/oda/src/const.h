
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const.h,v 1.4 89/11/01 11:29:05 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	const.h,v $
 * Revision 1.4  89/11/01  11:29:05  mss
 * Casted UnsafeSetAttr for explicit truncation
 * 
 * Revision 1.3  89/10/26  14:07:56  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/09/29  11:39:53  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:26:41  mss
 * Initial revision
 * 
 * Revision 1.38  89/09/11  14:28:46  mss
 * revmoed leading #
 * 
 * Revision 1.37  89/03/09  15:54:45  annm
 * support for the new DAPD stuff
 * 
 * Revision 1.36  89/02/23  20:45:30  annm
 * changes for new DVL stuff
 * 
 * Revision 1.35  89/01/19  18:01:27  annm
 * no change
 * 
 * Revision 1.34  89/01/19  14:54:20  annm
 * primping the Check value code - tightened up restrictions on presentation attributes going on content dependent DVL/DAPD components. 
 * ALso tightened up on presentation attribute placement to account for the shared versus logical versus layout presentation attributes.
 * 
 * Revision 1.33  89/01/17  16:57:11  annm
 * changes for DVL defaulting
 * 
 * Revision 1.32  88/12/13  19:55:53  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.31  88/09/19  16:57:31  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.30  88/09/06  13:19:01  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.29  88/09/06  12:04:03  annm
 * 
 * no change
 * 
 * Revision 1.28  88/08/25  16:11:57  annm
 * added remove from document routines for support
 * for fill in constituent, collaspe content portions
 * and delete constituent
 * 
 * Revision 1.27  88/08/15  18:17:32  annm
 * no change
 * 
 * Revision 1.26  88/08/09  13:46:01  jr
 * Add CONST_IsBasic & CONST_IsBasLay macros
 * 
 * Revision 1.25  88/06/08  16:55:35  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.24  88/06/08  14:34:39  annm
 * added Copyright notice
 * 
*/

#ifndef	TK_INCLUDED_CONST
#define	TK_INCLUDED_CONST

#ifdef TK_TKI_CONST
#define TK_CONST
#else
#ifdef TK_IMPL_CONST
#define	TK_CONST
#else
#define AP_CONST
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>
#include <attrid.h>

#include <tkerr.h>

/*
defined constants for content architectures can be
seen by all.
*/
#define	CHARACTER_CONTENT	    ((VALUE_type) 1)
#define	RASTER_GRAPHICS_CONTENT	    ((VALUE_type) 2)
#define	GEOMETRIC_GRAPHICS_CONTENT  ((VALUE_type) 3)

/*
    defined constants for the MakeComponent kind parameter 
*/
#define	SPECIFIC_COMPONENT	    ((VALUE_type) 1)
#define GENERIC_COMPONENT	    ((VALUE_type) 2)
#define DVL_COMPONENT		    ((VALUE_type) 3)
#define DAPD_COMPONENT		    ((VALUE_type) 4)

/*
    defined constants for the MakeStyle kind parameter
*/
#define PRESENTATION_STYLE	    ((VALUE_type) 1)
#define LAYOUT_STYLE		    ((VALUE_type) 2)

/*
    CONSTITUENT as the application sees it
*/

typedef POINTER_type	CONSTITUENT, CONSTITUENT_type;

#ifdef AP_CONST

#endif

#ifdef TK_CONST

#endif

#ifdef TK_TKI_CONST

#endif

#ifdef TK_IMPL_CONST


/* info for the what_kind constituent field */

#define	    DOC_PROFILE		( (VALUE_type) 1)

#define	    LOUT_STYLE		( (VALUE_type) 2)
#define	    PRESENT_STYLE	( (VALUE_type) 3)

#define	    CP_CHAR_CONTENT	( (VALUE_type) 4)
#define	    CP_RAST_CONTENT	( (VALUE_type) 5)
#define	    CP_GEO_CONTENT	( (VALUE_type) 6)

/* logical objects - NC is non-class */
#define	    DOC_LOG_ROOT_NC	( (VALUE_type) 7)
#define	    COMP_LOG_NC		( (VALUE_type) 8)
#define	    BAS_LOG_NC		( (VALUE_type) 9)

/* logical object classes - CL is class */
#define	    DOC_LOG_ROOT_CL	( (VALUE_type) 10)
#define	    COMP_LOG_CL		( (VALUE_type) 11)
#define	    BAS_LOG_CL		( (VALUE_type) 12)

/* layout objects - NC is non-class */
#define	    DOC_LAY_ROOT_NC	( (VALUE_type) 13)
#define	    PAGE_SET_NC		( (VALUE_type) 14)
#define	    PAGE_NC		( (VALUE_type) 15)
#define	    FRAME_NC		( (VALUE_type) 16)
#define	    BLOCK_NC		( (VALUE_type) 17)

/* layout object classes - CL is class */
#define	    DOC_LAY_ROOT_CL	( (VALUE_type) 18)
#define	    PAGE_SET_CL		( (VALUE_type) 19)
#define	    PAGE_CL		( (VALUE_type) 20)
#define	    FRAME_CL		( (VALUE_type) 21)
#define	    BLOCK_CL		( (VALUE_type) 22)

/* empty constituents are used for forward references
    in the ReadODIF process
*/
#define	    EMPTY		( (VALUE_type) 23)

/*
   what_kind value for DVL components
*/

#define	    DVL_PAGE		( (VALUE_type) 24)
#define	    DVL_FRAME		( (VALUE_type) 25)
#define	    DVL_BLOCK		( (VALUE_type) 26)
#define	    DVL_COMP		( (VALUE_type) 27)
#define	    DVL_BAS 		( (VALUE_type) 28)

/*
    what_kind values for DAPD components
*/
#define	    DAPD_BLOCK		( (VALUE_type) 29 )
#define	    DAPD_BAS		( (VALUE_type) 30 )

/* Macros for testing what kind of constituent - arg is (INTERNAL_CONSTITUENT *) */

/*
    WARNING: beware of using an expression as an argument to any of these
	     macros!!!!  The expression may be evaluated multiply, causing
	     unexpected results.
*/

/* Macros for identifying DVL components */
#define	    CONST_IsDVLPage(cp)	    ((cp)->what_kind == DVL_PAGE)
#define	    CONST_IsDVLFrame(cp)    ((cp)->what_kind == DVL_FRAME)
#define	    CONST_IsDVLBlock(cp)    ((cp)->what_kind == DVL_BLOCK)
#define	    CONST_IsDVLComp(cp)	    ((cp)->what_kind == DVL_COMP)
#define	    CONST_IsDVLBas(cp)	    ((cp)->what_kind == DVL_BAS)

#define	    CONST_IsDVLBasic(cp) \
(CONST_IsDVLBlock(cp) || CONST_IsDVLBas(cp))
#define	    CONST_IsDVLLogical(cp) \
(CONST_IsDVLComp(cp) || CONST_IsDVLBas(cp))

/* macros for identifying DAPD components */
#define	    CONST_IsDAPDBlock(cp)    ((cp)->what_kind == DAPD_BLOCK )
#define	    CONST_IsDAPDBasLogical(cp)  ((cp)->what_kind == DAPD_BAS )

#define	    CONST_IsDAPD(cp) \
(CONST_IsDAPDBLock(cp) || CONST_IsDAPDBasLogical(cp) )

/* macros for testing for other kinds of constituents */

#define	    CONST_IsDocProf(cp)	( (cp)->what_kind == DOC_PROFILE )

#define	    CONST_IsLayoutStyle(cp)	( (cp)->what_kind == LOUT_STYLE )
#define	    CONST_IsPresentStyle(cp)	( (cp)->what_kind == PRESENT_STYLE )
#define	    CONST_IsStyle(cp) \
( CONST_IsLayoutStyle(cp) || CONST_IsPresentStyle(cp) )

#define	    CONST_IsCharContent(cp)	( (cp)->what_kind == CP_CHAR_CONTENT )
#define	    CONST_IsRasterContent(cp)	( (cp)->what_kind == CP_RAST_CONTENT )
#define	    CONST_IsGeoContent(cp)	( (cp)->what_kind == CP_GEO_CONTENT )
#define	    CONST_IsContent(cp) \
    ( CONST_IsCharContent(cp) || CONST_IsRasterContent(cp) || \
      CONST_IsGeoContent(cp) )

#define	    CONST_IsDocLogRootNc(cp)	( (cp)->what_kind == DOC_LOG_ROOT_NC )
#define	    CONST_IsCompLogNc(cp)	( (cp)->what_kind == COMP_LOG_NC )
#define	    CONST_IsBasLogNc(cp)	( (cp)->what_kind == BAS_LOG_NC )

#define	    CONST_IsDocLogRootCl(cp)	( (cp)->what_kind == DOC_LOG_ROOT_CL )
#define	    CONST_IsCompLogCl(cp)	( (cp)->what_kind == COMP_LOG_CL )
#define	    CONST_IsBasLogCl(cp)	( (cp)->what_kind == BAS_LOG_CL )

#define	    CONST_IsBasLog(cp)	\
    ( CONST_IsBasLogNc(cp) || CONST_IsBasLogCl(cp) )

#define	    CONST_IsCompLog(cp) \
    ( CONST_IsCompLogNc(cp) || CONST_IsCompLogCl(cp) )

#define	    CONST_IsDocLogRoot(cp) \
    ( CONST_IsDocLogRootNc(cp) || CONST_IsDocLogRootCl(cp) )

#define	    CONST_IsLogNc(cp) \
    ( CONST_IsDocLogRootNc(cp) || CONST_IsCompLogNc(cp) || \
    CONST_IsBasLogNc(cp) )

#define	    CONST_IsLogCl(cp) \
    ( CONST_IsDocLogRootCl(cp) || CONST_IsCompLogCl(cp) || \
    CONST_IsBasLogCl(cp) )

#define	    CONST_IsLogical(cp) \
    ( CONST_IsLogNc(cp) || CONST_IsLogCl(cp) )

#define	    CONST_IsDocLayRootNc(cp)	( (cp)->what_kind == DOC_LAY_ROOT_NC )
#define	    CONST_IsPageSetNc(cp)	( (cp)->what_kind == PAGE_SET_NC )
#define	    CONST_IsPageNc(cp)		( (cp)->what_kind == PAGE_NC )
#define	    CONST_IsFrameNc(cp)	( (cp)->what_kind == FRAME_NC )
#define	    CONST_IsBlockNc(cp)	( (cp)->what_kind == BLOCK_NC )    

#define	    CONST_IsDocLayRootCl(cp)	( (cp)->what_kind == DOC_LAY_ROOT_CL )
#define	    CONST_IsPageSetCl(cp)	( (cp)->what_kind == PAGE_SET_CL )
#define	    CONST_IsPageCl(cp)		( (cp)->what_kind == PAGE_CL )
#define	    CONST_IsFrameCl(cp)	( (cp)->what_kind == FRAME_CL )
#define	    CONST_IsBlockCl(cp)	( (cp)->what_kind == BLOCK_CL )    

#define	    CONST_IsDocLayRoot(cp) \
    ( CONST_IsDocLayRootNc(cp) || CONST_IsDocLayRootCl(cp) )

#define	    CONST_IsPageSet(cp) \
    ( CONST_IsPageSetNc(cp) || CONST_IsPageSetCl(cp) )

#define	    CONST_IsPage(cp) \
    ( CONST_IsPageNc(cp) || CONST_IsPageCl(cp) )

#define	    CONST_IsFrame(cp) \
    ( CONST_IsFrameNc(cp) || CONST_IsFrameCl(cp) )

#define	    CONST_IsBlock(cp) \
    ( CONST_IsBlockNc(cp) || CONST_IsBlockCl(cp) )

#define	    CONST_IsLayCl(cp) \
    ( CONST_IsDocLayRootCl(cp) || CONST_IsPageSetCl(cp) || \
    CONST_IsPageCl(cp) || CONST_IsFrameCl(cp) || \
    CONST_IsBlockCl(cp) )

#define	    CONST_IsLayNc(cp) \
    ( CONST_IsDocLayRootNc(cp) || CONST_IsPageSetNc(cp) || \
      CONST_IsPageNc(cp) || CONST_IsFrameNc(cp) || \
      CONST_IsBlockNc(cp) )

#define	    CONST_IsLayout(cp)	\
    ( CONST_IsLayCl(cp) || CONST_IsLayNc(cp) )

#define	    CONST_IsObjCl(cp) \
    ( CONST_IsLayCl(cp) || CONST_IsLogCl(cp) )

#define	    CONST_IsObjNc(cp) \
    ( CONST_IsLayNc(cp) || CONST_IsLogNc(cp) )

#define	    CONST_IsComponent(cp) \
    ( CONST_IsObjCl(cp) || CONST_IsObjNc(cp) )

#define CONST_IsBasLay(cp)  (CONST_IsBlock(cp))

#define CONST_IsBasic(cp)   (CONST_IsBasLog(cp) || CONST_IsBasLay(cp))

#define CONST_IsComposite(cp) \
    (CONST_IsCompLog(cp) || CONST_IsDocLogRoot(cp) || \
     CONST_IsDocLayRoot(cp) || CONST_IsPageSet(cp) || \
     CONST_IsPage(cp) || CONST_IsFrame(cp ) )

/* Macros for determining generic/specific -- only use for components!! */
#define CONST_IsGeneric(cp)	(CONST_IsLogCl(cp) || CONST_IsLayCl(cp))
#define CONST_IsSpecific(cp)	(CONST_IsLogNc(cp) || CONST_IsLayNc(cp))

/* Macro for identifying empty constituents */
#define CONST_IsEmpty(cp)	((cp)->what_kind == EMPTY)


/* number of hash buckets for the attributes */
/* Note: the next set of defines can't be casted because of VMS restrictions on
   casting in array bounds */
#define	NBUCKETS    (20)

/* number of bits per element of bit mask */
#define BITS_PER_ELEMENT	    (8)

#define NBYTES	    ((MAX_ATTR-MIN_ATTR)/BITS_PER_ELEMENT + 1)

typedef 
    BYTE_type	
CONST_which_attributes[NBYTES];

/*
    bit mask for telling which attributes are present 
	and associated macro.
*/

#define CONST_UnsafeHasAttr(cp, attrid) \
    ((cp)->which_attributes[((attrid)-MIN_ATTR)/BITS_PER_ELEMENT] \
	& (1 << (((attrid)-MIN_ATTR) % BITS_PER_ELEMENT)))

#define CONST_UnsafeSetAttr(cp, attrid) \
    (cp)->which_attributes[((attrid)-MIN_ATTR)/BITS_PER_ELEMENT] \
	|= ( (BYTE_type) (1 << (((attrid)-MIN_ATTR) % BITS_PER_ELEMENT)))

/* data structure for a constituent */

typedef struct CONSTITUENT_struct {

    /* 
	pointer for linking together a document's constituents
	in a doubly linked list
    */

    struct CONSTITUENT_struct	    *forward;
    struct CONSTITUENT_struct	    *back;

    /* thread to the parent */

    struct CONSTITUENT_struct	    *parent;


    /* information to identify what kind of constituent:*/
    
    VALUE_type	    what_kind;


    /* Hash headers for the attributes in singly linked list */

    struct ATTR_attribute_struct    *attr_hash_buckets[NBUCKETS];


    /* Info for testing the presence of attributes */

    CONST_which_attributes	    which_attributes;

/*
QUESTION: DO WE NEED OTHER INFO TO BE ABLE TO
DEAL WITH INHERITANCE IN GETATTR ???
*/

    /* Needed for debugging & consistency checking */
    struct DOCUMENT_struct	*document;	/* Document contained in */

    /* Some debugging stuff (for use in DEBUG module) */
    INT_type			id;		/* ID for output */
    INT_type			debug_counter;	/* Counter for debug routines */

} *INTERNAL_CONSTITUENT;

#endif
/* end of the IMPL section */

/* everyone can see this */

/*
    The API defined CONSTITUENTS. Note that the ERROR_CONSTITUENT
    is a constituent named gercon. Storage for gercon is
    allocated in CONST_Init. Similarly with NULL_CONSTITUENT
    which is named gnlcon.
*/


#include <const1.h>
#include <const2.h>
#include <const3.h>

#endif
