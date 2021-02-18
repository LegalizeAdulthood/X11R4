/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/vopcona.ch,v 2.6 89/03/02 15:55:27 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/vopcona.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidvopconapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/vopcona.ch,v 2.6 89/03/02 15:55:27 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class vopconapp[vopcona] : application[app] {
    overrides:
	ParseArgs(int argc,char **argv) returns boolean;
	Start() returns boolean;
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject((struct vopconapp *) self) returns boolean;
      FinalizeObject(self);
    data:
        char *consoleName;
};
