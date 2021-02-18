/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/nesssym.c,v 1.5 89/06/01 16:01:17 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/nesssym.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/nesssym.c,v 1.5 89/06/01 16:01:17 wjh Exp $";
#endif

/* nesssym.c		

	Code for the nesssym object
*/
/*
 *    $Log:	nesssym.c,v $
 * Revision 1.5  89/06/01  16:01:17  wjh
 * campus release version
 * 
 * Revision 1.1  88/10/21  11:01:09  wjh
 * Initial revision
 *  
 * Revision 1.0  88/07/14  13:22:05WJHansen
 * Copied from sym.c and discarded EVERYTHING
 */

/*****************************************************************************\
\*****************************************************************************/



#include <class.h>
#include <nesssym.eh>
#include <nodeclss.h>
#include <compdefs.h>	/* for Tunk */


	boolean
nesssym__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	struct nesssym *self;
{
	self->next = NULL;
	self->parent.ness = NULL;
	self->flags = 0;
	self->type = Tunk;
	return TRUE;
}

	void
nesssym__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	struct nesssym *self;
{
}
