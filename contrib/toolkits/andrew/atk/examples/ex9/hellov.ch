/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex9/RCS/hellov.ch,v 2.6 89/09/13 11:48:25 tpn Exp $ */
/*Mouse drag, key bindings, menus and a scroll bar*/

class helloworldview[hellov]: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit(enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
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
    long x,y;
    long distX,distY;
    long frameX, frameY;
    boolean blackOnWhite;
    long newX,newY;
    boolean newBlackOnWhite;
    long newFrameX, newFrameY;
    int vrWidth,vrHeight;
};

