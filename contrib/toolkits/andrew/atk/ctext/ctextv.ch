/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ctext/RCS/ctextv.ch,v 2.6 89/02/20 12:33:40 ghoti Exp $ */
/* $ACIS:ctextv.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ctext/RCS/ctextv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidctextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ctext/RCS/ctextv.ch,v 2.6 89/02/20 12:33:40 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class ctextview[ctextv]: textview[textv] {
  overrides:
    PostKeyState(struct keystate *keystate);
    PostMenus(struct menulist *menulist);
  classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct ctextview *self) returns boolean;
    FinalizeObject(struct ctextview *self);
    SetBounceTime(long time) returns long;
  data:
    struct keystate *c_state;
    struct menulist *c_menus;
};
