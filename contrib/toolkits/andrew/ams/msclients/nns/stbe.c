/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* Methods for String Table Bucket Entries
*/

#include <big.h>

char           *STBEGetString(stbe)
STableBucketEntry_t *stbe;
{
    return (*stbe);
}

void            STBESetString(stbe, string)
STableBucketEntry_t *stbe;
char           *string;
{
    *stbe = string;
}
