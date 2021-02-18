/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/typetext.ch,v 2.5 89/10/09 13:49:07 tpn Exp $ */
/* $ACIS:typetext.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/typetext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtypetext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/typetext.ch,v 2.5 89/10/09 13:49:07 tpn Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* typetext.H
 * Class header for a basic fenced view.
 *
 */

class typetext: text {
overrides:
    HandleBegindata(long pos,FILE *file) returns long;
    HandleKeyWord(long pos, char *keyword, FILE *file) returns long;
    HandleCloseBrace(long pos, FILE *file) returns long;
    ViewName() returns char *;
data:
int hashandler;
};

