/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#include <ms.h>
#include <hdrparse.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/file.h>

#define CANT_OPEN_DIR (-1)
#define DEFAULTMAILBOX ("./")
#define DIRPROBLEMS (2)
#define FILECHUNKSIZE (1024)
#define GL_GROWSIZE (128)
#define GL_TOOBIG (8000)                         /* Arbitrary */
#define GROUPSLINESIZE (2048)                    /* Arbitrary, hope it's
                                                  * enough (I've seen one
                                                  * case requiring 1105
                                                  * chars! */
#define LOSTDIR ("lost")
#define NETNEWSROOT ("/afs/andrew.cmu.edu/usr0/netbb/.MESSAGES/ext/nn/")
#define NEWSGROUPNAMELEN (80)
#define NOTIMPLEMENTED (5)
#define NUMMCBUCKETS (64)
#define NUMSTBUCKETS (64)
#define RETRY_INTERVAL (900)
#define STB_GROWSIZE (128)
#define STUPIDUSER (1)
#define TSLASH (4)
#define UNKNOWN (3)

typedef char   *STableBucketEntry_t;

typedef struct {
    int             num, size;
    STableBucketEntry_t *entries;
}               STableBucket_t;

typedef struct {
    int             (*HashFn) ();
    STableBucket_t  buckets[NUMSTBUCKETS];
}               STable_t;

typedef struct {
    int             verbose, runOnce, dontDo, sortByTime;
    char           *mailboxDir, *NetnewsRoot, *ActiveGroups;
}               Options_t;

typedef struct {
    Options_t       Options;
    STable_t        STable;
}               Globals_t;

extern Globals_t Globals;

typedef struct {
    char           *filename, *newsgroup;
    int             ahead, before, ignore;
    long            time;
}               GListEntry_t;

typedef struct {
    int             num, size;
    GListEntry_t   *entries;
}               GList_t;

typedef struct MCacheBucketEntry_t {
    char           *filename;
    struct MS_Message *Msg;
    struct MCacheBucketEntry_t *next;
}               MCacheBucketEntry_t;

typedef MCacheBucketEntry_t *MCacheBucket_t;

typedef struct {
    int             (*HashFn) ();
    MCacheBucket_t  buckets[NUMMCBUCKETS];
}               MCache_t;

extern GListEntry_t *GLGetEntry();
extern MCacheBucketEntry_t *MCBEGetNext();
extern char    *GLEGetFilename();
extern char    *GLEGetNewsgroup();
extern char    *MCBEGetFilename();
extern char    *STBEGetString();
extern char    *STBFind();
extern char    *STBMake();
extern char    *STFind();
extern char    *STFindOrMake();
extern char    *STMake();
extern int      GLBuildList();
extern int      GLECompare();
extern int      GLEGetAhead();
extern int      GLEGetBefore();
extern int      GLEGetIgnore();
extern int      GLGetNum();
extern int      MCBMake();
extern int      MCMake();
extern int      ProcessEntry();
extern int      ShouldAlreadyBeCached();
extern int      ShouldBeCached();
extern int      ShouldRename();
extern int      ShouldUnlink();
extern struct MS_Message *MCBEGetMsg();
extern struct MS_Message *MCBFind();
extern struct MS_Message *MCFind();
extern void     DotsToSlashesInPlace();
extern void     GLESet();
extern void     GLESetAhead();
extern void     GLESetBefore();
extern void     GLESetIgnore();
extern void     GLIgnore();
extern void     GLInit();
extern void     GLPurge();
extern void     MCBDelete();
extern void     MCBESet();
extern void     MCBESetNext();
extern void     MCBInit();
extern void     MCBPurge();
extern void     MCDelete();
extern void     MCInit();
extern void     MCPurge();
extern void     ProcessList();
extern void     STBESetString();
extern void     STBInit();
extern void     STBPurge();
extern void     STInit();
extern void     STPurge();
extern void     Verbiage();
