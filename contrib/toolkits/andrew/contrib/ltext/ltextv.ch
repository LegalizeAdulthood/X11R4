/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/ltext/RCS/ltextv.ch,v 2.5 89/02/20 12:55:44 ghoti Exp Locker: wl0g $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/ltext/RCS/ltextv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_ltextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/ltext/RCS/ltextv.ch,v 2.5 89/02/20 12:55:44 ghoti Exp Locker: wl0g $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* ltextv.ch: Textview subclass for use with ltext.ch */

class ltextview[ltextv]: textview[textv] {
  overrides:
    PostKeyState(struct keystate *keystate);
    PostMenus(struct menulist *menulist);
  classprocedures:
    InitializeClass() returns boolean;
    SetBounceTime(long time) returns long;
  data:
    struct keystate *l_state;
    struct menulist *l_menus;    
};
