/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/fnotev.ch,v 1.4 89/07/24 11:18:24 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/fnotev.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_fnotev_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/fnotev.ch,v 1.4 89/07/24 11:18:24 tpn Exp $ ";
#endif

class fnotev: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ObservedChanged (struct observable *changed, long value);
    DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
    LinkTree(struct view *parent);
    GetOrigin(long width, long height, long *originX, long *originY);
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
    ReceiveInputFocus();
methods:
    pushchild();
    popchild();
macromethods:
    SetDisplayStr(S) ((self)->displaystr = (S))
    GetParentTextview() ((self)->parentview)
    GetParentText() ((self)->parenttext)
classprocedures:
    SetEndnote(doendnotes);
    InitializeClass() returns boolean;
    InitializeObject(struct fnote *self) returns boolean;
    FinalizeObject(struct fnote *self) ;
data:
    struct impair *imp;
    struct fontdesc *fd,*ofd;
    struct cursor *cursor;
    int fnotetype;
    struct text *parenttext;
    struct textview *parentview;
    char *displaystr;
    struct  fontdesc_charInfo *ci;
};

