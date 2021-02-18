/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/tree23.ch,v 2.5 89/02/20 13:00:40 ghoti Exp $ */
/* $ACIS:tree23.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/tree23.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtree23int_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/tree23.ch,v 2.5 89/02/20 13:00:40 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define tree23int_VERSION 1

class tree23int[tree23] {
methods:
    AddIn(long offset, struct tree23int *newnode);
    Apply(procedure proc);
    Filter(long offset, struct tree23int *left, struct tree23int *right, long lowval, long highval, procedure proc, char *procdata);
    FindL(long offset, long key) returns long;
    FindR(long offset, long key) returns long;
    Free();
    Merge(struct tree23int *ancestor, long offset, procedure proc, char *procdata);
    Update(long pos, long size);
    Enumerate(procedure proc, char *procdata) returns long;
    NumberOfLeaves() returns long; 
    Dump(long offset);
    Delete() returns struct tree23int *;
    Eval() returns long;
    Insert(long key, long data) returns struct tree23int *;
    Jam(struct tree23int *newnode);
    Remove(struct tree23int *child) returns struct tree23int *;
    Twiddle(struct tree23int *child);
    GetLeftMostNode() returns struct tree23int *;
    GetNextNode(struct tree23int *node) returns struct tree23int *;
classprocedures:
    Allocate() returns struct tree23int *;
    Deallocate();
data:
    struct tree23int *parent;/* 	Parent of the node */
    long bump;			/* Amount to add to parents value to get key */
    long data;			/* The real data */
    boolean leaf;		/* True if the node is a leaf. */
    short nKids;
    struct tree23int *kid[3];
};

