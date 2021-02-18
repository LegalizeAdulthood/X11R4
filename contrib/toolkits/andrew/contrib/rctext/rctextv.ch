/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/rctext/RCS/rctextv.ch,v 1.1 89/09/10 14:58:51 ghoti Exp $ */
/* $ACIS:rctextv.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/rctext/RCS/rctextv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidrctextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/rctext/RCS/rctextv.ch,v 1.1 89/09/10 14:58:51 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class rctextview[rctextv]: textview[textv] {
  overrides:
    PostKeyState(struct keystate *keystate);
    PostMenus(struct menulist *menulist);
  classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct rctextview *self) returns boolean;
    FinalizeObject(struct rctextview *self);
    SetBounceTime(long time) returns long;
  data:
    struct keystate *c_state;
    struct menulist *c_menus;
};
