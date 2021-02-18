
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug.h,v 1.2 89/09/29 11:40:00 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/debug.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	debug.h,v $
 * Revision 1.2  89/09/29  11:40:00  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:27:51  mss
 * Initial revision
 * 
 * Revision 1.11  88/09/06  13:19:25  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.10  88/09/06  12:04:19  annm
 * 
 * no change
 * 
 * Revision 1.9  88/07/19  12:26:18  mss
 * Added debug2 files
 * 
 * 
 * Revision 1.8  88/06/08  16:55:58  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.7  88/06/08  14:35:06  annm
 * added Copyright notice
 * 
*/

/*

    Model for the master .h file

    put the global comment for the .h file here

*/


#ifndef TK_INCLUDED_DEBUG
#define	TK_INCLUDED_DEBUG

#ifdef TK_TKI_DEBUG
#define TK_DEBUG
#else
#ifdef TK_IMPL_DEBUG
#define	TK_DEBUG
#else
#define AP_DEBUG
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>
#include <tkerr.h>

/* Everyone sees these definitions */

#ifdef  TK_DEBUG

/* These are for the Tool Kit */

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_DEBUG

#endif

/* 
    include(s) for the subordinate .h file(s)

*/

#include <debug1.h>
#include <debug2.h>

#endif
