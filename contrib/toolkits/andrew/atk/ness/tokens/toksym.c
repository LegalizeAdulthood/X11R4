/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/tokens/RCS/toksym.c,v 1.4 89/06/01 10:09:24 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/tokens/RCS/toksym.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/tokens/RCS/toksym.c,v 1.4 89/06/01 10:09:24 wjh Exp $";
#endif

/* toksym.c		

	Code for the toksym object
*/
/*
 *    $Log:	toksym.c,v $
 * Revision 1.4  89/06/01  10:09:24  wjh
 * revision for new securtiy system
 * 
 * Revision 1.1  88/10/21  10:43:34  wjh
 * Initial revision
 *  
 * Revision 1.0  88/07/14  13:22:05WJHansen
 * Copied from sym.c and discarded EVERYTHING
 */


#include <class.h>
#include <toksym.eh>


	boolean
toksym__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	struct toksym *self;
{
	self->loc = 0;	/* (because error terminator set uses -3 as flag) */
	self->len = 0;
	self->info.obj = NULL;
	return TRUE;
}


	void
toksym__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	struct toksym *self;
{
}
