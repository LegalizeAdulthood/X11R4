/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/pscripta.ch,v 2.7 89/06/22 13:05:36 ghoti Exp $ */
/* $ACIS:pscripta.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/pscripta.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidpscriptapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/pscripta.ch,v 2.7 89/06/22 13:05:36 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class pipescriptapp[pscripta] :application[app] {
overrides:
    ParseArgs(int argc, char **argv) returns boolean;
    Start() returns boolean;
data:
    FILE *df;
    struct typescript *ss;
    struct frame *frame;
    int ShellMenu;
    int filemenu;
    char *Menu;
    char *titleLine ;
    char *font ;
    char **argv;
};

