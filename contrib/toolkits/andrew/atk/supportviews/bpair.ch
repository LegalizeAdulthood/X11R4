/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/bpair.ch,v 2.6 89/04/04 19:38:36 wjh Exp $ */
/* $ACIS:bpair.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/bpair.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidbpair_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/bpair.ch,v 2.6 89/04/04 19:38:36 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
bpair.H

	Class definitions for bpair

*/

#define  bpair_PROGRAMMERVERSION    1

class bpair: lpair
{
overrides:

	FullUpdate(/* struct bpair *self, */ enum view_UpdateType type, 
			long left, long top, long width, long height);

classprocedures:

	InitializeObject(/* struct classhdr *ClassID;*/ struct bpair *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct bpair *self);

};
