/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/complete.ch,v 2.6 89/02/23 09:03:18 ghoti Exp $ */
/* $ACIS:complete.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/complete.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidcompletion_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/complete.ch,v 2.6 89/02/23 09:03:18 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* complete.H
 * Class header for message line completion support module.
 */

#include <message.ih>

struct result {
    char *partial;
    int partialLen; /* Length in chars not including terminating NUL */
    char *best;
    int max;
    int bestLen; /* Length in chars not including terminating NUL */
    enum message_CompletionCode code;
};

package completion[complete] {
    classprocedures:
        CompletionWork(char *string, struct result *data);
        GetFilename(struct view *view, char *prompt, char *startPath, char *buffer, long bufsiz, boolean directoryP, boolean mustMatch) returns int;
        FindCommon(char *string1, char *string2) returns long;
        InitializeClass() returns boolean;
        FileHelp(char *partialPath, long dummyData, int (*helpTextFunction) (), long helpTextRock);
        FileComplete(char *pathname, boolean directory, char *buffer, int bufferSize) returns enum message_CompletionCode;
};
