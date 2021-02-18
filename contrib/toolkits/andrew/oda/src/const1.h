
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const1.h,v 1.1 89/09/21 06:27:01 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	const1.h,v $
 * Revision 1.1  89/09/21  06:27:01  mss
 * Initial revision
 * 
 * Revision 1.17  89/09/01  14:17:59  annm
 * fixing USES_LONG_NAMES problems
 * 
 * Revision 1.16  89/08/30  17:08:03  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.15  89/01/19  18:01:03  annm
 * no change
 * 
 * Revision 1.14  89/01/19  14:17:50  annm
 * added declaration of MakeDefaultComponent
 * 
 * Revision 1.13  88/12/13  19:56:11  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.12  88/09/06  13:19:10  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.11  88/09/06  12:04:09  annm
 * 
 * no change
 * 
 * Revision 1.10  88/06/08  16:55:46  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.9  88/06/08  14:34:50  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_CONST1
#define TK_INCLUDED_CONST1

#undef EXTERN

#ifdef TK_GVARS_CONST1
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

EXTERN	CONSTITUENT	CopyConstituent();
EXTERN	INT_type	DeleteConstituent();
EXTERN	CONSTITUENT	MakeComponent();
EXTERN	CONSTITUENT	MakeContent();
EXTERN	CONSTITUENT	MakeDocumentProfile();
EXTERN	CONSTITUENT	MakeStyle();
EXTERN  CONSTITUENT	MakeDefaultComponent();

/* the variable for the error constituent */

#ifndef USE_LONG_NAMES

#define ERROR_CONSTITUENT   gercon

#else

#define ERROR_CONSTITUENT Error_Constituent

#endif

EXTERN CONSTITUENT ERROR_CONSTITUENT;

#ifndef USE_LONG_NAMES

#define NULL_CONSTITUENT    gnlcon

#else

#define NULL_CONSTITUENT Null_Constituent

#endif

EXTERN CONSTITUENT NULL_CONSTITUENT;

#ifdef TK_CONST

#ifndef USE_LONG_NAMES

#define CONST_Init	gcsint	
#define CONST_Final	gcsfnl

#endif

EXTERN INT_type CONST_Final();
EXTERN INT_type CONST_Init();

#endif


#endif
