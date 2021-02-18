/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/viewref.ch,v 2.5 89/02/20 13:00:44 ghoti Exp $ */
/* $ACIS:viewref.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/viewref.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidviewref_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/viewref.ch,v 2.5 89/02/20 13:00:44 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define viewref_VERSION 1

class viewref: observable[observe] {
classprocedures:
    InitializeObject(struct viewref *self) returns boolean;
    FinalizeObject(struct viewref *self);
    Create(char *viewtype, struct dataobject *dataobject) returns struct viewref *;
data:
    char *viewType;
    long viewID;
    struct dataobject *dataObject;
    long desw,desh;
};
