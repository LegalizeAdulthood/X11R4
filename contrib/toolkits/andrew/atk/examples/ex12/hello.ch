/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex12/RCS/hello.ch,v 2.6 89/09/13 11:42:44 tpn Exp $ */
/* Mouse drag, key bindings, menus, scrollbar, data object, read/write */

#define POSUNDEF (-1)

class helloworld[hello]: dataobject[dataobj] {
overrides:
    Read(FILE *file,long id) returns long;
    Write(FILE *file,long writeId,int level) returns long;
data:
    long x,y;
    boolean blackOnWhite;
};
