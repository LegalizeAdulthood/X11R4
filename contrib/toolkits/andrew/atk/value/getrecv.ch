/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/getrecv.ch,v 2.6 89/02/23 10:22:54 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/getrecv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_GetRecV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/getrecv.ch,v 2.6 89/02/23 10:22:54 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <rect.h>

struct GetRecV_recpair {
    struct rectangle parent,child;
};
class GetRecV[getrecv] : valueview[valuev]
{
 overrides:
  DrawFromScratch(long x,y,width,height);
  DrawHighlight();
  DrawDehighlight();
  DrawNewValue();
  DoHit(enum view_MouseAction type, long x,y,hits)
    returns struct GetRecV  *;
  LookupParameters();
methods:
macromethods:
  GetTmpVal() (self->tmpval)
classprocedures:
  InitializeClass() returns boolean;
data:
   long x,y,width,height;
   long lastx,lasty,firstx,firsty;
   struct GetRecV_recpair *tmpval;
};


