/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex17/RCS/hello.ch,v 2.6 89/09/13 11:44:47 tpn Exp $ */
/* Mouse drag, key bindings, menus, scrollbar, data object, read/write */

#define POSUNDEF (-1)

class helloworld[hello]: dataobject[dataobj] {
overrides:
    Read(FILE *file,long id) returns long;
    Write(FILE *file,long writeId,int level) returns long;
classprocedures:
    InitializeObject(struct helloworld *hw) returns boolean;
    FinalizeObject(struct helloworld *hw);
data:
    long x,y;
    boolean blackOnWhite;
    struct dataobject *dobj;
};
