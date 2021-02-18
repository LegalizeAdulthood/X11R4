/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framecmd.ch,v 2.7 89/02/24 17:01:43 ghoti Exp $ */
/* $ACIS:framecmd.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framecmd.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidframecmds_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framecmd.ch,v 2.7 89/02/24 17:01:43 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* framecmd.H
 * A vestigial class to make static loading of framecmds optional.
 *
 */

package framecmds[framecmd] {
    classprocedures:
        InitKeymap(struct menuslit **menuOut) returns struct keymap *;
        InitializeClass() returns boolean;
};
