/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/tindex.ch,v 1.1 89/06/15 23:19:58 tpn Exp $ */
/* $ACIS:tindex.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/tindex.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtindex_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/tindex.ch,v 1.1 89/06/15 23:19:58 tpn Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

package tindex {
    classprocedures:
      IndexTerm(struct text *d,char *term,char **error)returns int;
      WriteIndexList(struct text *d,FILE *f);
      ReadIndexList(struct text *d,FILE *f) returns char *;
      InitializeClass() returns boolean;
      PrintIndex(struct view *self);
      PreviewIndex(struct view *self);
};

