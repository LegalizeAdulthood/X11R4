/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ctext/RCS/ctext.ch,v 2.6 89/04/27 01:48:06 cm26 Exp $ */
/* $ACIS:ctext.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ctext/RCS/ctext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidctext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ctext/RCS/ctext.ch,v 2.6 89/04/27 01:48:06 cm26 Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* ctext.ch: Text subclass specialized for dealing with C code. */

#include <mark.ih>

class ctext: text {
  overrides:
    SetAttributes(struct attributes *atts);
    Clear();
    Read(FILE *file, long id) returns long;
    Write(FILE *file, long writeID, int level) returns long;
  methods:
    Indent(struct mark *range) returns long;
    ReindentLine(long pos);
    RecommentBackward(long pos);
    ReverseBalance(long pos) returns long;
    RedoStyles();
    TabAndOptimizeWS(long pos, int inc) returns long;
  data:
    struct style *comment_style, *fnname_style, *root_style;
};
