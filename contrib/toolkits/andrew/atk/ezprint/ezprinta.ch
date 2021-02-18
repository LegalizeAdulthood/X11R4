/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ezprint/RCS/ezprinta.ch,v 2.6 89/06/08 13:32:58 nsb Exp $ */
/* $ACIS:ezprinta.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ezprint/RCS/ezprinta.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidezprintapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ezprint/RCS/ezprinta.ch,v 2.6 89/06/08 13:32:58 nsb Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class ezprintapp[ezprinta] : application[app] {
    classprocedures:
      InitializeObject(struct ezprintapp *self) returns boolean;
    overrides:
	ParseArgs(int argc,char **argv) returns boolean;
	Run() returns int;
        ReadInitFile();
};
