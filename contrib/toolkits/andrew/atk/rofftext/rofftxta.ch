/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftxta.ch,v 2.5 89/02/20 12:58:22 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftxta.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_rofftextapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftxta.ch,v 2.5 89/02/20 12:58:22 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* rofftext application */

class rofftextapp[rofftxta] : application[app] {
    overrides:
	ParseArgs(argc,argv) returns boolean;
        Start() returns boolean;
        Run() returns int;
    classprocedures:
        InitializeObject(self) returns boolean;
    data:
        char *macrofile;
        boolean RoffType;
        char *outputfile;
        char *inputfile;
        char **inputfiles;
        char **argv;
        int argc;
        boolean HelpMode;
        boolean BeCompletelyBogus;
        boolean PrintWarnings;
};
