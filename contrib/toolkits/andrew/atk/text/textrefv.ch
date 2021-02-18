/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textrefv.ch,v 1.1 89/07/13 16:29:22 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textrefv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_textrefv_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textrefv.ch,v 1.1 89/07/13 16:29:22 tpn Exp $ ";
#endif

class textrefv: fnotev {
overrides:
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct textrefv * self) returns boolean;
data :
    char *cref;
    long loc;
};

