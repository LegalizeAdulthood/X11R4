/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/ptext/RCS/ptextv.ch,v 1.1 89/05/17 23:35:35 cm26 Exp $ */
/* $ACIS:ptextview.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/ptext/RCS/ptextv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidptextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/ptext/RCS/ptextv.ch,v 1.1 89/05/17 23:35:35 cm26 Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class ptextview[ptextv]: textview[textv] {
  overrides:
    PostKeyState(struct keystate *keystate);
    PostMenus(struct menulist *menulist);
  classprocedures:
    InitializeClass() returns boolean;
    SetBounceTime(long time) returns long;
  data:
    struct keystate *c_state;
    struct menulist *c_menus;    
};
