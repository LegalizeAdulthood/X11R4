
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/prior.h,v 1.2 89/09/29 11:41:00 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/prior.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	prior.h,v $
 * Revision 1.2  89/09/29  11:41:00  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:35:14  mss
 * Initial revision
 * 
 * Revision 1.12  89/09/11  14:39:11  mss
 * removed leading #
 * 
 * Revision 1.11  88/09/06  13:21:57  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.10  88/09/06  12:06:10  annm
 * 
 * no change
 * 
 * Revision 1.9  88/07/15  15:38:02  mss
 * Added include of tags.h
 * 
 * Revision 1.8  88/06/08  16:57:53  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.7  88/06/08  14:36:59  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_PRIOR
#define	TK_INCLUDED_PRIOR

#ifdef TK_TKI_PRIOR
#define TK_PRIOR
#else
#ifdef TK_IMPL_PRIOR
#define	TK_PRIOR
#else
#define AP_PRIOR
#endif
#endif

#include <lcsite.h>

#include <acd.h>

#include <errors.h>
#include <apirt.h>

#include <tkerr.h>

#include <apityp.h>
#include <tags.h>

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_PRIOR.

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

#ifdef  TK_PRIOR

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_PRIOR


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <prior1.h>

#endif
