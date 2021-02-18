/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/clklistv.ch,v 1.8 89/02/23 10:22:29 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/clklistv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_clicklistV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/clklistv.ch,v 1.8 89/02/23 10:22:29 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
class clicklistV[clklistv] : stringV[stringv] {
overrides:
   ObservedChanged( struct observable * observed, long status );
   GetApplicationLayer() returns struct view *;
   WantInputFocus(struct view *requestor);
classprocedures:
   InitializeObject(struct clicklistV *self) returns boolean;
   FinalizeObject(struct clicklistV *self) returns boolean;
data:
    struct text *text;
    struct cltextview *cltextview;
    long count,csize;
    char *choice;
    boolean choicechanged;
};

