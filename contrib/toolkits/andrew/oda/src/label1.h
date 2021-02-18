
/*

$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/label1.h,v 1.1 89/09/21 06:33:37 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/label1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	label1.h,v $
 * Revision 1.1  89/09/21  06:33:37  mss
 * Initial revision
 * 
 * Revision 1.12  89/08/31  15:05:05  annm
 * still fixing real names
 * 
 * Revision 1.11  89/08/29  16:17:56  annm
 * added real names for names that are too long
 * 
 * Revision 1.10  88/09/06  13:21:12  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.9  88/09/06  12:05:22  annm
 * 
 * no change
 * 
 * Revision 1.8  88/07/05  16:56:54  jr
 * Does lots more
 * 
*/

#ifndef TK_INCLUDED_LABEL1
#define TK_INCLUDED_LABEL1

#undef EXTERN

#ifdef TK_GVARS_LABEL1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN INT_type LabelStructure();
EXTERN INT_type LabelStyles();
EXTERN INT_type LabelDocument();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_LABEL

#ifndef USE_LONG_NAMES

#define LABEL_Init  glbint	
#define	LABEL_Final glbfnl	

#define LabelConstructionFactor	    glblcf
#define LabelSpecificStructure	    glblsp

#endif

EXTERN INT_type LABEL_Final();
EXTERN INT_type LABEL_Init();

#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_LABEL

#ifndef USE_LONG_NAMES

#define AssignIdAttr			glbaia
#define CreateAndAssignId		glbcid
#define LabelConstructionExpr		glblce
#define LableConstructionFactor		glblcf
#define LabelConstructionTerm		glblct
#define LabelContentPortions		glbcpt
#define LabelGenericStructure		glbgst
#define LabelSeqConstructionTerm	glbsct
#define Ndigits				glbdig
#define WalkAndLabelGenericStructure	glblgs
#define WalkAndLabelSpecificStructure	glblss

#endif

#endif

#endif
