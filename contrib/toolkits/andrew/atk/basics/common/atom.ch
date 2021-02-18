/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/atom.ch,v 2.6 89/09/27 14:34:07 wjh Exp $ */
/* $ACIS:atom.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/atom.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidatom_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/atom.ch,v 2.6 89/09/27 14:34:07 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class atom {
classprocedures:
    Intern(char *name) returns struct atom *;
    InternRock(long rock) returns struct atom *;
    InitializeClass() returns boolean;
    InitializeObject(struct atom *self) returns boolean;
    FinalizeObject(struct observable *self);
macromethods:
    Name() (self->name)
data:
    char *name;
};
