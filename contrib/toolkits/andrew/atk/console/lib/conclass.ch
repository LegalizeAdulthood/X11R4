/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/conclass.ch,v 2.7 89/03/28 07:26:00 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/conclass.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidconsoleClass_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/lib/RCS/conclass.ch,v 2.7 89/03/28 07:26:00 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */



#define consoleClass_VERSION 1
#define BOXINTERVAL 10

class consoleClass[conclass] : view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Hit(enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct consoleClass *;
    ReceiveInputFocus();
    LoseInputFocus();
    Update();
    WantUpdate(struct view *requestor);
    PostMenus(struct menulist *menulist);
classprocedures:
    InitializeObject((struct consoleClass *)self) returns boolean;
    InitializeClass() returns boolean;
data:
    struct menulist *stdMenulist;
    struct menulist *userMenulist;
    boolean haveInputFocus;
    boolean interactive;
    long menuMask;
};
