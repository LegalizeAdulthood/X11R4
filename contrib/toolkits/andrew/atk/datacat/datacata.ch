/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/datacat/RCS/datacata.ch,v 2.6 89/09/13 15:04:52 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/datacat/RCS/datacata.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_datacatapp_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/datacat/RCS/datacata.ch,v 2.6 89/09/13 15:04:52 tpn Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class datacatapp[datacata] : application[app] {
    overrides:
	ParseArgs(int argc,char **argv) returns boolean;
	Run() returns int;
        ReadInitFile();
    classprocedures:
	InitializeObject(struct datacatapp *self) returns boolean;
	FinalizeObject(struct datacatapp *self);

};
