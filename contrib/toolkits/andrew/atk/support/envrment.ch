/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/envrment.ch,v 2.7 89/08/04 16:28:29 zs01 Exp $ */
/* $ACIS:envrment.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/envrment.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidenvironment_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/envrment.ch,v 2.7 89/08/04 16:28:29 zs01 Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define environment_VERSION 1

union environmentcontents {
    struct style *style;
    struct viewref *viewref;
};

enum environmenttype {
    environment_None,
    environment_Style,
    environment_View,
    environment_Any
};

class environment[envrment] : nestedmark[nstdmark]  {
methods:
/* the next two are obsolete. Use WrapStyle, WrapView, InsertStyle, or
 * InsertView instead.
 */
    Wrap(long pos, long length, enum environmenttype type, union environmentcontents data) returns struct thisobject *;
    Insert(long rpos, enum environmenttype type, union environmentcontents data, boolean doinsert) returns struct thisobject *;
    WrapStyle(long pos, long length, struct style *style) returns struct thisobject *;
    WrapView(long pos, long length, struct view *view) returns struct thisobject *;
    InsertStyle(long rpos, struct style *style, boolean doinsert) returns struct thisobject *;
    InsertView(long rpos, struct view *view, boolean doinsert) returns struct thisobject *;
    Remove(long pos, long length, enum environmenttype, boolean deleteall) returns boolean;
overrides:
    NewButSimilar() returns struct environment *;
classprocedures:
    InitializeObject(self) returns boolean;
    FinalizeObject(self);
    GetRootEnvironment() returns struct environment *;
    Allocate() returns struct environment *;
    Deallocate(struct environment *self);
data:
    enum environmenttype type;
    union environmentcontents data;
};
