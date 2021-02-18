/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/msgsa.ch,v 2.6 89/02/20 13:16:38 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/msgsa.ch,v $ */

#ifndef lint
static char *rcsid_messagesapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/msgsa.ch,v 2.6 89/02/20 13:16:38 ghoti Exp $ ";
#endif /* lint */

#define MAXARGS 100
#define MAXADDHEADS 50

class messagesapp[msgsa] : application[app] {
overrides:
    ParseArgs(int argc, char **argv) returns boolean;
    Start() returns boolean;
data:
    int SendOnly, AppendBugInfo, MailOnly, ConsiderChanged, UsingSnap, RetainedArgc, SeparateWindows;
    char *AddHeaderList[MAXADDHEADS];
    char MailSourceFile[1+MAXPATHLEN];
    char *RetainedArg[MAXARGS];
};
