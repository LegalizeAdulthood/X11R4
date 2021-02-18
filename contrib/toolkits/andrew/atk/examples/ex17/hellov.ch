/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex17/RCS/hellov.ch,v 2.6 89/09/13 11:44:52 tpn Exp $ */
/* Mouse drag, key bindings, menus, scroll bar, data object, messages, prompting for input, GetApplicationLayer */

#include "rect.h"

class helloworldview[hellov]: view {
overrides:
    SetDataObject(struct helloworld *hw);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ReceiveInputFocus();
    LoseInputFocus();
    GetInterface(int type) returns char *;
    GetApplicationLayer() returns struct view *;
    DeleteApplicationLayer(struct view *);
    LinkTree(struct view *parent);
classprocedures:
    InitializeClass() returns boolean;
data:
    struct keystate *keystate;
    struct menulist *menulist;
    boolean HaveDownTransition;
    boolean haveInputFocus;
    long hitX,hitY;
    int x,y;
    boolean blackOnWhite;
    long frameX, frameY;
    long newFrameX, newFrameY;
    int vrWidth,vrHeight;
    struct view *view;
    struct view *applayer;
};
