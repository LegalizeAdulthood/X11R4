
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/expr1.h,v 1.1 89/09/21 06:29:44 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/expr1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	expr1.h,v $
 * Revision 1.1  89/09/21  06:29:44  mss
 * Initial revision
 * 
 * Revision 1.12  89/09/11  14:37:47  mss
 * removed leading #
 * 
 * Revision 1.11  89/09/01  15:44:19  annm
 * fixing USE_LONG_NAMES problems
 * 
 * Revision 1.10  88/09/06  13:19:54  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.9  88/09/06  12:04:45  annm
 * 
 * no change
 * 
 * Revision 1.8  88/06/08  16:56:39  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.7  88/06/08  14:35:44  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_EXPR1
#define TK_INCLUDED_EXPR1

#undef EXTERN

#ifdef TK_GVARS_EXPR1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/
EXTERN	INT_type    EvalNumExpr();
EXTERN	CONSTITUENT EvalObjIdExpr();
EXTERN	INT_type    EvalRefBind();
EXTERN	STRING_type EvalStringExpr();


/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_EXPR

#ifndef USE_LONG_NAMES 

#define EXPR_Init   gexint	
#define EXPR_Final  gexfnl	

#endif

EXTERN INT_type EXPR_Final();
EXTERN INT_type EXPR_Init();


#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_EXPR

#endif

#endif
