/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/etextv.ch,v 2.6 89/02/20 13:09:07 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/etextv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_etextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/etextv.ch,v 2.6 89/02/20 13:09:07 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
class etextview[etextv]:textview[textv] {
overrides :
    ReceiveInputFocus();
    LoseInputFocus();
macromethods:
    SetClearOnRIF(val) self->ClearOnRIF = val
    SetResetOnLIF(val) self->ResetOnLIF = val
    SetValueview(val) self->valueview = val
classprocedures:
    InitializeObject(struct etextview *self) returns boolean;
    InitializeClass(struct etextview *self) returns boolean;
data:
    struct keystate *keystate;
    boolean ClearOnRIF,ResetOnLIF;
    struct valueview *valueview;
};
