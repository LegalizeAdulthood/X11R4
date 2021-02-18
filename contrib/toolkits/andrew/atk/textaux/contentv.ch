/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/textaux/RCS/contentv.ch,v 1.2 89/10/26 17:30:01 tpn Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/textaux/RCS/contentv.ch,v $ */

#ifndef lint
static char *rcsidcontentv_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/textaux/RCS/contentv.ch,v 1.2 89/10/26 17:30:01 tpn Exp $";
#endif /* lint */

class contentv : textview [textv] {
overrides:
     PostMenus(struct menulist *menulist);
     GetClickPosition(long position, long numberOfClicks, enum view_MouseAction action, long startLeft, long startRight, long *leftPos, long *rightPos);
classprocedures:
    InitializeClass() returns boolean;
    MakeWindow(struct text *self);
macromethods:
SetSourceView(vw) self->srcview = vw
data:
    struct menulist *menus;
    struct textview *srcview;
};
