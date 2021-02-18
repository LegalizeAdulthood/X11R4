/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/logv.ch,v 2.6 89/03/02 15:31:01 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/logv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlogview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/logv.ch,v 2.6 89/03/02 15:31:01 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class logview[logv] : textview[textv] {
overrides:
    LoseInputFocus();
    PostMenus(struct menulist *menulist);
    FrameDot(long pos);
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject((struct logview *)self) returns boolean;
    FinalizeObject();
data:
    struct menulist *menu;
};
