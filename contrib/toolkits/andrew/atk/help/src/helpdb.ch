/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpdb.ch,v 1.2 89/05/03 17:28:40 mp1w Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpdb.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/help/src/RCS/helpdb.ch,v 1.2 89/05/03 17:28:40 mp1w Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*---------------------------------------------------------------------------*/
/*	MODULE: helpdb.c						     */
/*		Package of routines to do the index-related lookups and	     */
/*		matching algorithms.					     */
/*---------------------------------------------------------------------------*/

package helpdb {
    classprocedures:
        InitializeClass(struct helpdb *self) returns boolean;
	CheckIndex(struct view *v) returns int;
	SetIndex(char *anindex) returns int;
	SetupHelp(struct cache *c, char *aname, int strip) returns int;
	GetHelpDirs() returns struct helpDir *;
	AddSearchDir(char *dirname);
	ReadAliasesFile(char *aname);
	MapAlias(char *name) returns char *;
	Enumerate(void (*proc)(), char *self);
	PrintSearchDirs();
};
