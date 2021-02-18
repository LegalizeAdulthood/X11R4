/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/hash.ch,v 2.4 89/02/17 17:05:06 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/hash.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_hash_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/hash.ch,v 2.4 89/02/17 17:05:06 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* A hash table
 *
 */

#define hash_BUCKETS 32

class hash {

classprocedures:
    InitializeObject(struct hash *self) returns boolean;
    FinalizeObject(struct hash *self);
    InitializeClass() returns boolean;
methods:
    Lookup(char *key) returns char *;
    Store(char *key, char *value);
    Delete(char *key) returns char *;
    Rename(char *key, char *new) returns char *;
    Debug();
macromethods:
    SetHash(fn) (self->hash = (fn))
data:
    struct glist *buckets[hash_BUCKETS];
    procedure hash;
};
