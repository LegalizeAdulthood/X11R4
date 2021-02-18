/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/filter.ch,v 2.4 89/02/17 16:07:43 ghoti Exp $ */
/* $ACIS:filter.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/filter.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidfilter_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/filter.ch,v 2.4 89/02/17 16:07:43 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* filter the textview selection region through a command
 */

package filter {
    classprocedures:
        InitializeClass() returns boolean;
};
