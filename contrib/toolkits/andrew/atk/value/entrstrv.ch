/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/entrstrv.ch,v 2.6 89/02/20 13:08:58 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/entrstrv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_enterstrV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/entrstrv.ch,v 2.6 89/02/20 13:08:58 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
class enterstrV[entrstrv] : buttonV[buttonv] {
overrides:
   DoHit(enum view_MouseAction type, long x,y,hits) returns struct buttonV *;
   ObservedChanged( struct observable * observed, long status );
   GetApplicationLayer() returns struct view *;
   WantInputFocus(struct view *requestor);
classprocedures:
   InitializeObject(struct enterstrV *self) returns boolean;
   FinalizeObject(struct enterstrV *self) returns boolean;
data:
    struct entertext *etext;
    struct etextview *etextview;
};

