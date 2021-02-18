/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex5/RCS/hellov.ch,v 2.6 89/09/13 11:47:09 tpn Exp $ */
/* Mouse drag, key bindings, menus */

class helloworldview[hellov]: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ReceiveInputFocus();
    LoseInputFocus();
classprocedures:
    InitializeClass() returns boolean;
data:
    struct menulist *menulist;
    struct keystate *keystate;
    boolean HaveDownTransition;
    boolean haveInputFocus;
    long x,y;
    long distX,distY;
    boolean blackOnWhite;
    long newX,newY;
    boolean newBlackOnWhite;
};
