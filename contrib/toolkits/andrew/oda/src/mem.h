
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mem.h,v 1.1 89/09/21 06:33:55 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mem.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	mem.h,v $
 * Revision 1.1  89/09/21  06:33:55  mss
 * Initial revision
 * 
 * Revision 1.11  89/09/11  14:38:46  mss
 * removed leading #
 * 
 * Revision 1.10  88/09/30  17:26:11  annm
 * looking for what turned out to be a very dumb bug (missing ;)
 * 
 * Revision 1.9  88/09/06  13:21:17  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.8  88/09/06  12:05:27  annm
 * 
 * no change
 * 
 * Revision 1.7  88/06/08  16:57:18  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.6  88/06/08  14:36:26  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_MEM
#define	TK_INCLUDED_MEM

#ifdef TK_TKI_MEM
#define TK_MEM
#else
#ifdef TK_IMPL_MEM
#define	TK_MEM
#else
#define AP_MEM
#endif
#endif

#include <errors.h>
#include <apirt.h>

#include <tkerr.h>

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_MEM.

*/

/* 
    put the stuff seen across the user interface here 
    as given in the API.

*/

/* 
    put the stuff seen inside the toolkit after the
    #ifdef TK_<filename>: i.e. TK_TKI or TK_IMPL/TK_GVARS
    sees this section.

*/

#ifdef  TK_MEM


#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_MEM


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <mem1.h>

#endif
