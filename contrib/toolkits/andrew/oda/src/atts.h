
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts.h,v 1.2 89/09/29 11:39:43 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts.h,v $

(c) Copyright 1989 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	atts.h,v $
 * Revision 1.2  89/09/29  11:39:43  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:25:08  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/11  14:36:38  mss
 * removed leading #
 * 
 * Revision 1.4  89/08/04  17:54:05  mss
 * added atts4
 * 
 * Revision 1.3  89/07/19  21:04:33  mss
 * Added new atts3 module
 * 
 * Revision 1.2  89/04/25  16:30:08  mss
 * Added atts2.h
 * 
 * Revision 1.1  89/03/21  17:12:54  mss
 * Initial revision
 * 

*/

/*

    Master include file for the ATTribute Support routines


*/


#ifndef TK_INCLUDED_ATTS
#define	TK_INCLUDED_ATTS

#ifdef TK_TKI_ATTS
#define TK_ATTS
#else
#ifdef TK_IMPL_ATTS
#define	TK_ATTS
#else
#define AP_ATTS
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_ATTS.

*/

/* 
    put the stuff seen across the user interface here 
    as given in the API.

*/

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>
#include <apityp.h>
#include <tkerr.h>

/* 
    put the stuff seen inside the toolkit after the
    #ifdef TK_<filename>: i.e. TK_TKI or TK_IMPL/TK_GVARS
    sees this section.

*/

#ifdef  TK_ATTS

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_ATTS


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <atts1.h>
#include <atts2.h>
#include <atts3.h>
#include <atts4.h>
#endif
