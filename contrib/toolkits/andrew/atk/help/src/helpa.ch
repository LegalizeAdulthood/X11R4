/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpa.ch,v 2.8 89/03/30 19:10:58 mp1w Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpa.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpa.ch,v 2.8 89/03/30 19:10:58 mp1w Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/*---------------------------------------------------------------------------*/
/*	MODULE: helpa.ch						     */
/*		Subclass of application, front-end for help.  Parses switches*/
/*		and creates a new help object.  Sets up a resident service   */
/*		on a socket to listen for new help connections.	             */
/*---------------------------------------------------------------------------*/

class helpapp[helpa] : application[app] {
    overrides:
	ParseArgs(int argc,char **argv) returns boolean;
        Start() returns boolean;
	Run() returns int;
    classprocedures:
	InitializeObject(struct helpapp *self) returns boolean;
};

