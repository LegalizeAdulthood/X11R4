/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex16/RCS/hellov.ch,v 2.6 89/09/13 11:44:23 tpn Exp $ */
/* Mouse drag, key bindings, menus, scroll bar, data object, messages, prompting for input, GetApplicationLayer */

class helloworldview[hellov]: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ReceiveInputFocus();
    LoseInputFocus();
    GetInterface(int type) returns char *;
    GetApplicationLayer() returns struct view *;
    DeleteApplicationLayer(struct view *);
classprocedures:
    InitializeClass() returns boolean;
data:
    struct keystate *keystate;
    struct menulist *menulist;
    boolean HaveDownTransition;
    boolean haveInputFocus;
    long hitX,hitY;
    long x,y;
    boolean blackOnWhite;
    long frameX, frameY;
    long newFrameX, newFrameY;
    int vrWidth,vrHeight;
    boolean didMove;
};
