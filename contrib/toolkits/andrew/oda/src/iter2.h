
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter2.h,v 1.1 89/09/21 06:32:23 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter2.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter2.h,v $
 * Revision 1.1  89/09/21  06:32:23  mss
 * Initial revision
 * 
 * Revision 1.8  89/08/31  15:00:36  annm
 * still fixing real names
 * 
 * Revision 1.7  89/08/29  16:10:59  annm
 * added real names for names that are too long
 * 
 * Revision 1.6  89/01/13  14:17:18  mss
 * Added missing semicolon in structure definition
 * 
 * Revision 1.5  89/01/10  11:46:26  jr
 * Coerce root-locating constants
 * correctly.
 * 
 * Revision 1.4  89/01/09  13:53:06  jr
 * Align with new API.
 * 
 * Revision 1.3  88/12/23  16:53:47  mss
 * Made an internal procedure accessable to iter3
 * 
 * Revision 1.2  88/09/06  13:21:02  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.1  88/08/25  15:11:43  mss
 * Initial revision
 * 

*/

#ifndef TK_INCLUDED_ITER2
#define TK_INCLUDED_ITER2

#undef EXTERN

#ifdef TK_GVARS_ITER2
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

#define PARENTS_FIRST	((VALUE_type) 1)
#define CHILDREN_FIRST	((VALUE_type) 2)

#define BREADTH_FIRST	((VALUE_type) 1)
#define DEPTH_FIRST	((VALUE_type) 2)

EXTERN	ITERATOR_type	MakeSubgraphIterator();
EXTERN	CONSTITUENT	NextConstituent();
EXTERN	INT_type	DeleteIterator();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_ITER

#endif

/*
    Declarations seen within the iter module go here.
*/

#ifdef TK_IMPL_ITER

/* Private definitions */

#define	GenericTree		( (INT_type) 14 )
#define	SpecificTree		( (INT_type) 15 )
#define	PresentationStyleList	( (INT_type) 16 )
#define	LayoutStyleList		( (INT_type) 17 )

typedef struct ITER_toBeVisited_type_struct {
    INTERNAL_CONSTITUENT    objectToVisit;
    BOOL_type	    childrenAlreadyProcessed;
} ITER_toBeVisited_type, *ITER_p_toBeVisited_type;

typedef struct ITER_state_type_struct {
    VALUE_type		before_kids;	    /* when to consider parent */
    VALUE_type		breadth_first;	    /* Breadth or depth first search */
    INT_type		constituentType;	    /* specific, generic, lstyle, pstyle? */
    INTERNAL_CONSTITUENT
			iteratorRoot;	    /* where iteration started */
    INT_type		nodeMarkerValue;    /* marker to leave in nodes for graphs */
    INT_type		nodeArrayActualSize;    /* last array elt index */
    INT_type		nodeArrayFilledMark;    /* last array index used, -1 if none */
    ITER_p_toBeVisited_type
			nodeArray;	    /* Blocks of information */
} ITER_state_type, *ITER_p_state_type;

typedef ITER_p_state_type INTERNAL_ITERATOR_type;

#ifndef USE_LONG_NAMES

#define ITER_AddKidsToParent gitakp

#define ITER_AddKidsToQueue		gitakq
#define ITER_AddSpecificKids		gitask
#define ITER_NextSubgraphConstituent	gitnsc
#define ITER_ShiftDownNodeArray		gitsda
#define ITER_ShiftUpNodeArray		gitsua
#define ITER_ValidateArraySize		gitvas

#endif

EXTERN INT_type ITER_AddKidsToParent();

#endif

#endif
