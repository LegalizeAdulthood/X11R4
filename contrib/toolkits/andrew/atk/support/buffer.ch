/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/buffer.ch,v 2.7 89/06/07 12:20:58 tpn Exp $ */
/* $ACIS:buffer.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/buffer.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidbuffer_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/buffer.ch,v 2.7 89/06/07 12:20:58 tpn Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

struct bufferContents { /* May not have to be in this file. */
    struct view *bufferView; /* The actual view that is looking at the data object. */
    struct view *bufferInputFocus; /* The view that is taking input. */
    struct view *bufferApplicationView; /* The view the window is pointing at. (May not be neccesary). */
    boolean used; /* Probably should be boolean. */
};

/* Flag values for buffer_GetBufferOnFile function. */
#define buffer_MustExist 1 /* File must exist for operation to succeed. */
#define buffer_ForceNew 2 /* Make a new buffer even if one already exists. */
#define buffer_ReadOnly 4 /* Buffer will be readonly regardless of file status. */

/* Flag values for buffer_WriteToFile function. */
#define buffer_ReliableWrite 1 /* Use vclose to ensure the file is written to server or an error code is returned. */
#define buffer_MakeBackup 2 /* Keep a backup file if the user preferences so indicates. */

class buffer: observable[observe] {
    methods:
        EnumerateViews(procedure mapFunction, long functionData) returns struct view *; /* ??? */
        SetData(struct dataobject *data);
        SetName(char *name);
        SetFilename(char *fileName);
        SetWriteVersion(long version);
        SetCkpVersion(long version);
        SetCkpClock(long clock);
        SetCkpLatency(long latency);
        SetScratch(boolean scratch);
        GetView(struct view **inputFocus, struct view **targetView, char *ViewName) returns struct view *;
        Visible() returns boolean;
        RemoveView(struct view *unusedView);
        ReadFile(char *filename) returns int;
        WriteToFile(char *filename, long flags) returns int;
        /* Disk file date may differ from GetTouchDate if file is modified externally. */
        GetFileDate() returns long;
        SetDefaultViewname(char *name);
    macromethods:
        GetData() ((self)->bufferData)
        GetName() ((self)->bufferName)
        GetFilename() ((self)->filename)
        GetCkpFilename() ((self)->ckpFilename)
        GetWriteVersion() ((self)->writeVersion)
        GetCkpVersion() ((self)->ckpVersion)
        GetCkpClock() ((self)->ckpClock)
        GetCkpLatency() ((self)->ckpLatency)
        GetScratch() ((self)->scratch)
        GetMadeBackup() ((self)->madeBackup)
        SetMadeBackup(boolean value) ((self)->madeBackup = (value))
        /* Unavailable/unapplicable dates are returned as 0 */
        GetLastTouchDate() ((self)->lastTouchDate)
        GetDefaultViewname() ((self)->viewname)
    classprocedures:
        Enumerate(procedure mapFunction, long functionData) returns struct buffer *;
        Create(char *bufferName, char *fileName, char *objetName, struct dataobject *data) returns struct buffer *;
        InitializeClass() returns boolean;
	
/* Lookup functions */
        FindBufferByName(char *bufferName) returns struct buffer *;
        FindBufferByFile(char *filename) returns struct buffer *;
        FindBufferByData(struct dataobject *data) returns struct buffer *;

/* File functions. */
        GetBufferOnFile(char *filename, long flags) returns struct buffer *;
        GuessBufferName( char *filename, char *bufferName, int nameSize);
        FinalizeObject(struct buffer *self);
        InitializeObject(struct buffer *self) returns boolean;
        SetDefaultObject(char *objectname);

    data:
        struct buffer *next; /* Used to chain buffers. */
        struct dataobject *bufferData; /* The dataobject associated with this buffer. Can't be NULL.*/
        char *bufferName; /* This buffers name. Must be filled in. */
        char *filename; /* The filename this dataobject is saved in. May be NULL. */
        char *ckpFilename; /* Checkpoint file, filled in automatically. */
        struct bufferContents *viewList; /* Array of view triples viewing this buffer's dataobj. */
        int viewsAllocated, viewsUsed; /* viewsAllocated = number of slots in above table. viewsUsed = number of slots containing views. */
        long ckpVersion, writeVersion, ckpClock, ckpLatency; /* Dataobject version last checkpointed, last written. ckpClock is a sequence clock used for intelligent checkpointing. */
        long lastTouchDate; /* When was the file last read or written by the buffer */
        boolean destroyData; /* Are we supposed to destroy this when we are destroyed. */
        boolean scratch; /* Indicates that this buffer is temporary in nature. Namely, don't checkpoint it. */
        boolean readOnly; /* This is only a hint. If the dataobject ignores the read only attribute, this won't be true. */
        boolean madeBackup; /* This is used to tell if we have made a backup file yet this session. */
        char *viewname;
};
