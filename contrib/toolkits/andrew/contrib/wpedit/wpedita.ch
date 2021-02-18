/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/wpedit/RCS/wpedita.ch,v 1.4 89/08/07 12:19:15 mcinerny Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/wpedit/RCS/wpedita.ch,v $ */

#ifndef lint
static char *rcsid_messagesapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/wpedit/RCS/wpedita.ch,v 1.4 89/08/07 12:19:15 mcinerny Exp $ ";
#endif /* lint */

#include <wpi.h>

class wpeditapp[wpedita] : application[app] {
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct wpeditapp *self) returns boolean;
    FinalizeObject(struct folders *self);
overrides:
    Start() returns boolean;
    ParseArgs(int argc, char **argv) returns boolean;
data:
    struct chlist *admin, *user; /* the admin and user panes */
    struct text *desc;		/* the description pane */
    int adminflag;		/* is requestor an administrator? */
    WPI_entry_t entry;		/* WPI record for user */
    char EditUser[200];		/* user we're editing */
    char requestor[200];	/* user making changes */
    char reqdomain[200];	/* changer's domain */
};
