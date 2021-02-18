/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/spell.ch,v 2.4 89/02/17 16:08:06 ghoti Exp $ */
/* $ACIS:spell.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/spell.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidspell_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/spell.ch,v 2.4 89/02/17 16:08:06 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

package spell {
    classprocedures:
        CheckDocument(struct textview *self, long rock);
        InitializeClass() returns boolean;
};
