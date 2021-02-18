/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex10/RCS/hello.ch,v 2.6 89/09/13 11:41:16 tpn Exp $ */
/* Mouse drag, key bindings, menus, scrollbar, data object */

#define POSUNDEF -1

class helloworld[hello]: dataobject[dataobj] {
data:
    long x,y;
    boolean blackOnWhite;
};
