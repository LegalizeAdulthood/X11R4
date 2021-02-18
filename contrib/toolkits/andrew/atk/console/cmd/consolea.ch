/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/consolea.ch,v 2.6 89/03/02 15:55:19 ghoti Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/consolea.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidconsoleapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/console/cmd/RCS/consolea.ch,v 2.6 89/03/02 15:55:19 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class consoleapp[consolea] : application[app] {
    overrides:
	ParseArgs(int argc,char **argv) returns boolean;
	Start() returns boolean;
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject((struct consoleapp *) self) returns boolean;
      FinalizeObject(self);
    data:
        char *consoleName;
};
