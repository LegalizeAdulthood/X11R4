
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/io.h,v 1.1 89/09/21 06:31:18 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/io.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	io.h,v $
 * Revision 1.1  89/09/21  06:31:18  mss
 * Initial revision
 * 
 * Revision 1.6  88/09/06  13:20:44  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.5  88/09/06  12:04:59  annm
 * 
 * no change
 * 
 * Revision 1.4  88/06/15  12:07:31  jr
 * Typos.
 * 
 * Revision 1.3  88/06/08  16:56:54  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.2  88/06/08  14:36:01  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_IO
#define	TK_INCLUDED_IO

#ifdef TK_TKI_IO
#define TK_IO
#else
#ifdef TK_IMPL_IO
#define	TK_IO
#else
#define AP_IO
#endif
#endif

/* 
    Everyone sees the defn's here.
*/

#ifdef  TK_IO

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_IO

#endif

/* 
    Include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter
*/

#include <io1.h>

#endif
