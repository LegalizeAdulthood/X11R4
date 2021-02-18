/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/lset.ch,v 2.5 89/02/20 12:55:16 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/lset.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_lset_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/lset.ch,v 2.5 89/02/20 12:55:16 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define STRING 1
#define LONG 2
#define FLOAT 3

class lset : dataobject[dataobj] {
overrides:
    Read (FILE *file, long id) returns long;
    Write (FILE *file, long writeid, int level) returns long;
    GetModified() returns long;
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct lset *self) returns boolean;
methods:
    InsertObject (char *name,char *viewname);
    registername(char *rf) returns char *;
data:
	int type;
	int pct;
	char dataname[32];
	char viewname[32];
	char refname[64];
    struct dataobject *dobj;
    struct dataobject *left,*right;
    int application;
    struct text *pdoc;
    int revision;
};
