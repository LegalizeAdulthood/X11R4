/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/entrtext.ch,v 2.6 89/02/20 13:09:03 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/entrtext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_entertext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/entrtext.ch,v 2.6 89/02/20 13:09:03 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
#define entertext_BUFCHANGEDFLAG 10101
class entertext[entrtext] : text {
overrides:
    InsertCharacters(long pos, char *str, long len) returns boolean;
    DeleteCharacters(long pos, long len) returns boolean;
    ViewName() returns char *;
methods:
    updatebuf();
    Changed()returns boolean;
    SetChars(char *str,int len);
macromethods:
    buflen() (self->buflen)
    GetString() (self->buf)
classprocedures:
    InitializeObject(struct entertext *self)returns boolean;
    FinalizeObject(struct entertext *self)returns boolean;
data:
    char *buf;
    long buflen,mod,realbuflen;
    struct style *Style;
    boolean needswrap;
};
