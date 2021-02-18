
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/init.h,v 1.2 89/09/29 11:40:24 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/init.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	init.h,v $
 * Revision 1.2  89/09/29  11:40:24  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:30:52  mss
 * Initial revision
 * 
 * Revision 1.14  89/09/11  14:37:59  mss
 * removed leading #
 * 
 * Revision 1.13  88/09/06  13:20:34  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.12  88/09/06  12:04:49  annm
 * 
 * no change
 * 
 * Revision 1.11  88/07/15  15:37:37  mss
 * Added include of tags.h
 * 
 * Revision 1.10  88/06/08  16:56:43  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.9  88/06/08  14:35:48  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_INIT
#define	TK_INCLUDED_INIT

#ifdef TK_TKI_INIT
#define TK_INIT
#else
#ifdef TK_IMPL_INIT
#define	TK_INIT
#else
#define AP_INIT
#endif
#endif

#include <lcsite.h>

#include <apityp.h>
#include <tags.h>

#include <errors.h>
#include <apirt.h>

#include <tkerr.h>

#include <acd.h>
#include <str.h>


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_INIT.

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

#ifdef  TK_INIT

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_INIT


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <init1.h>

#endif
