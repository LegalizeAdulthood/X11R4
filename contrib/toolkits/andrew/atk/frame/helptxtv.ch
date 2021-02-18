/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/helptxtv.ch,v 2.6 89/09/08 16:29:29 ghoti Exp $ */
/* $ACIS:helptxtv.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/helptxtv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidhelptextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/helptxtv.ch,v 2.6 89/09/08 16:29:29 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class helptextview[helptxtv] : textview[textv] {
    overrides:
      WantInputFocus( struct view *requestor);
      GetClickPosition(long position, long numberOfClicks, enum view_MouseAction action, long startLeft, long startRight, long *leftPos, long *rightPos);
      Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
      FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    classprocedures:
      FinalizeObject(struct helptextview *self);
      InitializeObject(struct helptextview *self) returns boolean;    
    data:
      struct cursor *myCursor;

};

