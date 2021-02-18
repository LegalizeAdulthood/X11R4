/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/tm/RCS/tma.ch,v 2.6 89/02/20 13:04:42 ghoti Exp $ */
/* $ACIS:tma.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/tm/RCS/tma.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtmapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/contrib/tm/RCS/tma.ch,v 2.6 89/02/20 13:04:42 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class tmapp[tma] : application[app] {
    overrides:
	ParseArgs(argc,argv) returns boolean;
	Start() returns boolean;
    classprocedures:
        InitializeObject(self) returns boolean;
    data:
	char **args;
        char *title;
	char *menufile;
	boolean fileMenus;
	int rows,cols;
};
