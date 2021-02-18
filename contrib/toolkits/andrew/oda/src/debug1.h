
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug1.h,v 1.1 89/09/21 06:28:13 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	debug1.h,v $
 * Revision 1.1  89/09/21  06:28:13  mss
 * Initial revision
 * 
 * Revision 1.19  89/08/31  14:53:27  annm
 * still fixing real names
 * 
 * Revision 1.18  89/08/29  15:32:02  annm
 * added real names for too long names
 * added real names for too long names
 * 
 * Revision 1.17  89/03/27  12:35:18  mss
 * Made undef'ing of EXTERN conditional on it being
 * already defined.
 * 
 * Revision 1.16  88/10/17  11:58:22  jr
 * Add support for INT's
 * 
 * Revision 1.15  88/09/06  14:53:16  annm
 * no change
 * 
 * Revision 1.14  88/09/06  12:04:24  annm
 * 
 * no change
 * 
 * Revision 1.13  88/07/29  17:13:26  jr
 * Generator for subordinates working
 * 
 * Revision 1.12  88/07/19  12:27:01  mss
 * Made debug_counter a sharable global for use wit
 * other debug modules
 * 
 * Revision 1.11  88/06/08  16:56:11  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.10  88/06/08  14:35:17  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_DEBUG1
#define TK_INCLUDED_DEBUG1

#ifdef EXTERN
#undef EXTERN
#endif

#ifdef TK_GVARS_DEBUG1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/* declarations of exported routines */
EXTERN INT_type WriteConstituentState();
EXTERN INT_type WriteDocumentState();
EXTERN INT_type WriteSubgraphState();
EXTERN INT_type WriteStylesState();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_DEBUG

#ifndef USE_LONG_NAMES

#define DEBUG_Init		    gdbint
#define DEBUG_Final		    gdbfnl
#define DEBUG_WriteDebugConstituent dbgwcn
#define DEBUG_WriteBytes	    dbgwbs

#define last_debug_counter	    gdbctr

#define InternalSubgraphState		gdbiss
#define WalkSpecificObject		gdbwso
#define WriteAttributesState		gdbwas
#define WriteDebugByte			gdbwdb
#define WriteDebugString		gdbwds
#define WriteUnreachableConstituents	gdbwuc

#define roots	gdbrot

#endif

EXTERN INT_type DEBUG_Init();
EXTERN INT_type DEBUG_Final();
EXTERN INT_type DEBUG_WriteDebugConstituent();
EXTERN INT_type DEBUG_WriteBytes();

/* Variable declarations */
EXTERN INT_type	    last_debug_counter;

#endif

/*
    Declarations seen within the DEBUG module go here.
*/

#ifdef TK_IMPL_DEBUG
#endif

#endif
