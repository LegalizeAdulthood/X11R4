/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/nesscomp.h,v 1.3 89/06/01 16:03:05 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/nesscomp.h,v $ */
/*
nesscomp.h

	compiler public declarations
*/
#ifndef _nesscomp_h_
#define _nesscomp_h_
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	static char *nesscomp_h_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/nesscomp.h,v 1.3 89/06/01 16:03:05 wjh Exp $";
#endif

/*
 *    $Log:	nesscomp.h,v $
 * Revision 1.3  89/06/01  16:03:05  wjh
 * campus release version
 * 
 * Revision 1.1  88/10/21  10:59:42  wjh
 * Initial revision
 * 
 * Revision 1.0  88/05/02  20:13:52  wjh
 * created by WJHansen
 *
 */

#include "nesssym.ih"
#include "error.h"

int yyparse();

extern struct nesssym **AttrDest;  /* pts to list to be extended for each defined attr
				(the new attr is patched at the front) */

extern struct errornode *ErrorList;

struct errornode *yycompile();

#endif _nesscomp_h_
