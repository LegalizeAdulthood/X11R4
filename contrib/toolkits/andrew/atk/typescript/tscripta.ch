/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscripta.ch,v 2.6 89/06/22 13:05:31 ghoti Exp $ */
/* $ACIS:tscripta.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscripta.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtscriptapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscripta.ch,v 2.6 89/06/22 13:05:31 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class typescriptapp[tscripta] : application[app] {
overrides:
    ParseArgs(int argc, char **argv) returns boolean;
    Start() returns boolean;
data:
    struct typescript *ss;
    struct frame *frame;
    int ShellMenu;
    int filemenu;
    char *Menu;
    char *titleLine ;
    char *font ;
    char **argv;
 };

