
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/str1.h,v 1.1 89/09/21 06:40:03 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/str1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	str1.h,v $
 * Revision 1.1  89/09/21  06:40:03  mss
 * Initial revision
 * 
 * Revision 1.11  89/08/30  17:11:31  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.10  88/12/13  19:57:01  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.9  88/09/06  13:22:17  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.8  88/09/06  12:06:35  annm
 * 
 * no change
 * 
 * Revision 1.7  88/06/08  16:58:40  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.6  88/06/08  14:37:46  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_STR1
#define TK_INCLUDED_STR1

#undef EXTERN

#ifdef TK_GVARS_STR1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN STRING_type  Chars2String();
EXTERN STRING_type  ConcatString();
EXTERN INT_type	    DeleteString();
EXTERN STRING_type  MakeString();

    /* Note that ERROR_STRING must be the same as ERROR_SEQUENCE */
#define ERROR_STRING	((STRING_type) ERROR_SEQUENCE)

    /* Note that NULL_STRING must be the same as NULL_SEQUENCE */
#define NULL_STRING	((STRING_type) NULL_SEQUENCE)

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_STR

#ifndef USE_LONG_NAMES

#define STR_Init	gstint	
#define STR_Final	gstfnl	

#endif

EXTERN INT_type STR_Final();
EXTERN INT_type STR_Init();

#endif

/*
    Declarations seen within the STR module go here.
*/

#ifdef TK_IMPL_STR

#endif

#endif
