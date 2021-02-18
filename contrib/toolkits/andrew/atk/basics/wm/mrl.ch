/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/mrl.ch,v 2.6 89/05/21 23:10:21 wjh Exp $ */
/* $ACIS:mrl.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/mrl.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidmrl_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/mrl.ch,v 2.6 89/05/21 23:10:21 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class mrl {
methods:
	Disect(struct rectangle *r) returns struct mrl *;
	Reset(struct rectangle *r)returns struct mrl *;
classprocedures:
	InitializeObject(struct mrl *self) returns boolean;
	FinalizeObject(struct mrl *self);
	Create(long region, struct mrl *next,struct rectangle *r) returns struct mrl *;
data:
	long wmregion;
	long top;
	long bottom;
	long left;
	long right;
	struct mrl *next;
};

