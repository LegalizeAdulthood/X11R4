
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr17.c,v 1.3 89/10/18 10:45:06 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr17.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr17.c,v $
 * Revision 1.3  89/10/18  10:45:06  jr
 * Add type decl to Dummy routine
 * 
 * Revision 1.2  89/10/13  11:31:48  jr
 * Hacks for buggy VMS loader.
 * 
 * Revision 1.1  89/09/21  06:18:39  mss
 * Initial revision
 * 
 * Revision 1.1  89/02/01  17:03:57  mss
 * Initial revision
 * 

*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr17.c,v $";
#endif

/* Module for separating compiling the Attribute Name data
    structure initialization in attr0.h  */


#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR17
#define TK_IMPL_CONST
#define TK_TKI_STR
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <parms.h>
#include <acd.h>
#include <str.h>
#include <mm.h>
#include <attr0.h>
#include <attr17.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR17
#undef TK_IMPL_CONST
#undef TK_TKI_STR
#undef TK_TKI_MM
#undef TK_TKI_TKERR

/* put #define's and declarations local to this file here */

/*
   This routine is necessary due to a bug on VMS -- a module that
   does not appear to have a called routine will not be loaded
   correctly.

   This routine is called from the INIT module.
*/

INT_type ATTR17_Dummy()
{
    return SUCCESS_INT;
}
