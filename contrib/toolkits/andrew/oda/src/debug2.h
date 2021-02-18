
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug2.h,v 1.1 89/09/21 06:28:31 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug2.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	debug2.h,v $
 * Revision 1.1  89/09/21  06:28:31  mss
 * Initial revision
 * 
 * Revision 1.6  89/08/31  14:53:42  annm
 * co -l iter2.h iter3.h
 * still fixing real names
 * 
 * 
 * Revision 1.5  89/08/29  15:49:05  annm
 * added real names for too long names
 * 
 * Revision 1.4  89/03/27  12:35:45  mss
 * Made undef'ing of EXTERN conditional on it being
 * already defined.
 * 
 * Revision 1.3  88/09/06  13:19:34  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/07/29  17:13:44  jr
 * Generator for subordinates working
 * 
 * Revision 1.1  88/07/19  12:27:23  mss
 * Initial revision
 * 
 
*/

#ifndef TK_INCLUDED_DEBUG2
#define TK_INCLUDED_DEBUG2

#ifdef EXTERN
#undef EXTERN
#endif

#ifdef TK_GVARS_DEBUG2
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/* declarations of exported routines */
/* None */


/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_DEBUG

#ifndef USE_LONG_NAMES

#define DEBUG_WalkGenericObject	dbgwgo

#define WalkConstructionExpr	    gdbkce
#define WalkConstructionFactor	    gdbkcf
#define WalkConstructionTerm	    gdbkct
#define WriteConstructionExpr	    gdbwce	    
#define WriteConstructionFactor	    gdbwcf
#define WriteConstructionTerm	    gdbwct

#define WriteDebugDsclass	    gdbwds

#define WriteDebugFont		    gdbwdf
#define WriteDebugISO9541Id	    gdbw95
#define WriteDebugISO9541IdArray    gdbwar
#define WriteDebugModalFontAttr	    gdbwfa
#define WriteDebugModes		    gdbwms
#define WriteDebugReal		    gdbwrl
#define WriteDebugScore		    gdbwss
#define WriteDebugScores	    gdbwsc
#define WriteDebugStructName	    gdbwsn
#define WriteDebugVarScript	    gdbwvs
#define WriteDebugVarScripts	    gdbwvv
#define WriteFontEnum		    gdbwfe
#define WriteSeqConstructionTerm    gdbwsq
#define WalkSeqConstructionTerm	    gdbksq

#define EsclassTable		    gdbetb
#define PostureTable		    gdbptb
#define PropwdthTable		    gdbpwt
#define StructurTable		    gdbstb
#define WeightTable		    gdbwtb

#endif

EXTERN INT_type DEBUG_WalkGenericObject();

#endif

/*
    Declarations seen within the DEBUG module go here.
*/

#ifdef TK_IMPL_DEBUG
#endif

#endif
