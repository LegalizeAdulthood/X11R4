/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex3/RCS/hellov.ch,v 2.6 89/09/13 11:46:33 tpn Exp $ */
/*Mouse drag example*/

class helloworldview[hellov]: view {
  overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
  data:
    long x;
    long y;
    long newX;
    long newY;
    long distX;
    long distY;
    boolean HaveDownTransition;
};
