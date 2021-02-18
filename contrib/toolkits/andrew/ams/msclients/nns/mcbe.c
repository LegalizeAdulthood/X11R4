/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* Methods for Message Cache Bucket Entries
*/

#include <big.h>

char           *MCBEGetFilename(mcbe)
MCacheBucketEntry_t *mcbe;
{
    return (mcbe->filename);
}

MCacheBucketEntry_t *MCBEGetNext(mcbe)
MCacheBucketEntry_t *mcbe;
{
    return (mcbe->next);
}

struct MS_Message *MCBEGetMsg(mcbe)
MCacheBucketEntry_t *mcbe;
{
    return (mcbe->Msg);
}

void            MCBESet(mcbe, filename, Msg, next)
MCacheBucketEntry_t *mcbe;
char           *filename;
struct MS_Message *Msg;
MCacheBucketEntry_t *next;
{
    mcbe->filename = filename;
    mcbe->Msg = Msg;
    mcbe->next = next;
}

void            MCBESetNext(mcbe, next)
MCacheBucketEntry_t *mcbe, *next;
{
    mcbe->next = next;
}
