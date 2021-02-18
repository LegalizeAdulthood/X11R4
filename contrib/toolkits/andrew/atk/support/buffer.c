/*LIBS: -lutil
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/buffer.c,v 2.16 89/11/01 11:18:27 wjh Exp $ */
/* $ACIS:buffer.c 1.6$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/buffer.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/buffer.c,v 2.16 89/11/01 11:18:27 wjh Exp $";
#endif /* lint */

#include <andyenv.h>
#include <andrewos.h> /* sys/types.h sys/file.h */
#include <sys/errno.h>
#include <sys/stat.h>

#include <class.h>
#include <observe.ih>
#include <dataobj.ih>
#include <view.ih>
#include <im.ih>
#include <filetype.ih>
#include <attribs.h>
#include <environ.ih>
#include <message.ih>
#include <buffer.eh>

#ifndef MAXPATHLEN 
#include <sys/param.h>
#endif
#define DEFAULTOBJECT "text"

extern int errno;
static struct buffer *allBuffers;
static char *backupExtension = NULL;
static char *checkpointExtension = ".CKP";
static boolean overwriteFiles = TRUE;
static char defaultobjectname[64] = DEFAULTOBJECT;

boolean buffer__InitializeObject(classID, self)
    struct classheader *classID;
    struct buffer *self;
{
    self->bufferData = NULL;
    self->bufferName = NULL;
    self->filename = NULL;
    self->ckpFilename = NULL;
    self->viewList = (struct bufferContents *) malloc(sizeof(struct bufferContents) * 3);
    self->viewsAllocated = 3;
    self->viewsUsed = 0;
    self->ckpVersion = self->writeVersion = 0;
    self->ckpClock = 0;
    self->ckpLatency = 0;
    self->lastTouchDate = 0L;
    self->scratch = FALSE;
    self->readOnly = FALSE;
    self->madeBackup = FALSE;
    self->next = allBuffers;
    self->viewname = NULL;
    allBuffers = self;
    
    return TRUE;
}

void buffer__FinalizeObject(classID, self)
    struct classheader *classID;
    struct buffer *self;
{
    int counter;
    struct buffer *traverse, **previous;

    for (counter = 0; counter < self->viewsUsed; counter++) {
        if (self->viewList[counter].bufferApplicationView != NULL)
            view_DeleteApplicationLayer(self->viewList[counter].bufferView, self->viewList[counter].bufferApplicationView);
        view_Destroy(self->viewList[counter].bufferView);
    }
    free(self->viewList);
    if (self->destroyData)
        dataobject_Destroy(self->bufferData);
    if (self->filename != NULL)
        free(self->filename);
    if (self->ckpFilename != NULL)
        free(self->ckpFilename);
    if (self->bufferName != NULL)
        free(self->bufferName);
    if(self->viewname != NULL)
	free(self->viewname);
    previous = &allBuffers;
    for (traverse = allBuffers; traverse != NULL && traverse != self; traverse = traverse->next)
        previous = &traverse->next;
    if (traverse != NULL) /* Bad error if this is false. */
        *previous = traverse->next;
}

struct buffer *buffer__Create(classID, bufferName, filename, objectName, data)
    struct classheader *classID;
    char *bufferName, *filename, *objectName;
    struct dataobject *data;
{
    char realName[MAXPATHLEN];
    struct buffer *thisBuffer;

/* Probably ought to insure that buffer names are unique. */
    thisBuffer = buffer_New();

    thisBuffer->bufferName = malloc(strlen(bufferName) + 1);
    strcpy(thisBuffer->bufferName, bufferName);

    if (filename != NULL) {
        int len;
        filetype_CanonicalizeFilename(realName, filename, sizeof(realName) - 1);
        filename = realName;
        len = strlen(filename);
        thisBuffer->filename = malloc(len + 1);
        strcpy(thisBuffer->filename, filename);
        thisBuffer->ckpFilename = malloc(len + strlen(checkpointExtension) + 1);
        strcpy(thisBuffer->ckpFilename, filename);
        strcat(thisBuffer->ckpFilename, checkpointExtension);
    }
    else {
        int len;
        len = strlen(bufferName);
        thisBuffer->ckpFilename = malloc(len + strlen(checkpointExtension) + 1);
        strcpy(thisBuffer->ckpFilename, bufferName);
        strcat(thisBuffer->ckpFilename, checkpointExtension);
    }
    if (data == NULL) {
        struct attributes *attributes = NULL;

        if (objectName == NULL)  {
            if (filename != NULL)
                objectName = filetype_Lookup(NULL, filename, NULL, &attributes);
            else
                objectName = filetype_Lookup(NULL, bufferName, NULL, &attributes);
	    if (objectName == NULL)
		objectName = defaultobjectname;
	}
        if ((data = (struct dataobject *) class_NewObject(objectName)) == NULL) {
            buffer_Destroy(thisBuffer);
            return NULL;
        }
        if (attributes != NULL)
            dataobject_SetAttributes(data, attributes);
        thisBuffer->destroyData = TRUE;
    }
    else
        thisBuffer->destroyData = FALSE;

    thisBuffer->bufferData = data;
    thisBuffer->ckpVersion = thisBuffer->writeVersion = dataobject_GetModified(data);
    thisBuffer->lastTouchDate = buffer_GetFileDate(thisBuffer);
    
    return thisBuffer;
}

struct view *buffer__GetView(self, inputFocus, targetView, viewName)
    struct buffer *self;
    struct view **inputFocus, **targetView;
    char *viewName;
{
    int counter;

    for (counter = 0; (counter < self->viewsUsed) && self->viewList[counter].used && (viewName == NULL ||
      strcmp(viewName, class_GetTypeName(self->viewList[counter].bufferView)) != 0); counter++)
        ;

    if (counter >= self->viewsUsed) {
        if (self->viewsAllocated <= self->viewsUsed) {
            self->viewList = (struct bufferContents *) realloc(self->viewList, sizeof(struct bufferContents) * (self->viewsAllocated + 3));
            self->viewsAllocated += 3;
        }
	if(viewName == NULL){
	    if(self->viewname != NULL) viewName = self->viewname;
            else viewName = dataobject_ViewName(self->bufferData);
	}
        if ((self->viewList[counter].bufferView = (struct view *) class_NewObject(viewName)) == NULL)
            return NULL;
        view_SetDataObject(self->viewList[counter].bufferView, self->bufferData);
        self->viewList[counter].bufferInputFocus = NULL;
        ++self->viewsUsed;
    }

    self->viewList[counter].bufferApplicationView = view_GetApplicationLayer(self->viewList[counter].bufferView);
    self->viewList[counter].used = TRUE;
    if (inputFocus != NULL)
        *inputFocus = self->viewList[counter].bufferInputFocus;
    if (targetView != NULL)
        *targetView = self->viewList[counter].bufferView;

    return self->viewList[counter].bufferApplicationView;
}

void buffer__RemoveView(self, unusedView)
    struct buffer *self;
    struct view *unusedView;
{
    int counter;

    for (counter = 0; counter < self->viewsUsed; counter++)
        if ((self->viewList[counter].bufferApplicationView == unusedView) && (self->viewList[counter].used == TRUE)) {
            view_UnlinkTree(unusedView); /* Remove it from tree. */
            view_DeleteApplicationLayer(self->viewList[counter].bufferView, unusedView); /* Deallocate application layer to save space. */
            self->viewList[counter].bufferApplicationView = NULL; /* NULL out field for safety. */
            self->viewList[counter].used = FALSE;
            break;
        }
}

boolean buffer__Visible(self)
    struct buffer *self;
{
    int counter;

    for (counter = 0; counter < self->viewsUsed; counter++)
        if (self->viewList[counter].used)
            return TRUE;
    return FALSE;
}

struct buffer *buffer__Enumerate(classID, mapFunction, functionData)
    struct classheader *classID;
    boolean (*mapFunction)();
    long functionData;
{
    struct buffer *traverse, *next;

    for (traverse = allBuffers; traverse != NULL; traverse = next) {
        next = traverse->next; /* So mapFunction is allowed to delete the buffer. */
        if ((*mapFunction)(traverse, functionData))
            return traverse;
    }
    return NULL;
}

void buffer__SetData(self, bufferData)
    struct buffer *self;
    struct dataobject *bufferData;
{
    self->bufferData = bufferData;
    self->ckpVersion = self->writeVersion = dataobject_GetModified(bufferData);
    self->destroyData = FALSE;
    if(self->viewname != NULL){
	free(self->viewname);
	self->viewname = NULL;
    }
    buffer_NotifyObservers(self, 0);
}

void buffer__SetName(self, bufferName)
    struct buffer *self;
    char *bufferName;
{
    if (self->bufferName != NULL)
        free(self->bufferName);
    self->bufferName = malloc(strlen(bufferName) + 1);
    strcpy(self->bufferName, bufferName);
    buffer_NotifyObservers(self, 0);
}

void buffer__SetFilename(self, filename)
    struct buffer *self;
    char *filename;
{

    char realName[MAXPATHLEN];
    int len;

    filetype_CanonicalizeFilename(realName, filename, sizeof(realName) - 1);
    filename = realName;
    if (self->filename)
        free(self->filename);
    if (self->ckpFilename)
        free(self->ckpFilename);
    len = strlen(filename);
    self->filename = malloc(len + 1);
    strcpy(self->filename, filename);
    self->ckpFilename = malloc(len + strlen(checkpointExtension) + 1);
    strcpy(self->ckpFilename, filename);
    strcat(self->ckpFilename, checkpointExtension);
    self->lastTouchDate = buffer_GetFileDate(self);
    buffer_NotifyObservers(self, 0);/* Tuck it into slot. */
}

void buffer__SetWriteVersion(self, version)
    struct buffer *self;
    long version;
{
    self->writeVersion = version;
    buffer_NotifyObservers(self, 0);/* Tuck it into slot. */
}

void buffer__SetCkpVersion(self, version)
    struct buffer *self;
    long version;
{

    self->ckpVersion = version;
    buffer_NotifyObservers(self, 0);/* Tuck it into slot. */
}

void buffer__SetCkpClock(self, clock)
    struct buffer *self;
    long clock;
{

    self->ckpClock = clock;
}

void buffer__SetCkpLatency(self, latency)
    struct buffer *self;
    long latency;
{
    self->ckpLatency = latency;
}

void buffer__SetScratch(self, scratch)
    struct buffer *self;
    boolean scratch;
{

    self->scratch = scratch;
    buffer_NotifyObservers(self, 0);/* Tuck it into slot. */
}

struct view *buffer__EnumerateViews(self, mapFunction, functionData)
    struct buffer *self;
    struct view *(*mapFunction)();
    long functionData;
{

    int counter;
    struct view *value;

    value = NULL;

    for (counter = 0; counter < self->viewsUsed; counter++)
        if (self->viewList[counter].used && (value = (*mapFunction)(self->viewList[counter].bufferApplicationView, self->viewList[counter].bufferView,
                           self->viewList[counter].bufferInputFocus, functionData)))
            break;
    if (counter < self->viewsUsed)
        return value;
    else
        return NULL;
}

struct buffer *buffer__FindBufferByFile(classID, filename)
    struct classheader *classID;
    char *filename;
{

    char realName[MAXPATHLEN];
    struct buffer *traverse;

    filetype_CanonicalizeFilename(realName, filename, sizeof(realName) - 1);
    filename = realName;
    for (traverse = allBuffers; traverse != NULL && ((traverse->filename == NULL) || (strcmp(traverse->filename, filename) != 0)); traverse = traverse->next)
        ;
    return traverse;
}

struct buffer *buffer__FindBufferByData(classID, bufferData)
    struct classheader *classID;
    struct dataobject *bufferData;
{
    struct buffer *traverse;

    for (traverse = allBuffers; traverse != NULL && (traverse->bufferData != bufferData); traverse = traverse->next);
    return traverse;
}

struct buffer *buffer__FindBufferByName(classID, bufferName)
    struct classheader *classID;
    char *bufferName;
{
    struct buffer *traverse;
    for (traverse = allBuffers; traverse != NULL && strcmp(traverse->bufferName, bufferName); traverse = traverse->next)
        ;
    return traverse;
}

/* This routine is supposed to be used for re-reading a file into a buffer.
 * Unfortunately, at present it is somewhat special cased for use within ez and
 * does not handle reseting all buffer state "intelligently."
 * Refuses to read directory; the caller (framecmds) tries GetBufferOnFile
 * if this call fails.
 */

int buffer__ReadFile(self, filename)
    struct buffer *self;
    char *filename;
{
    int objectID;
    int returnCode = 0;
    char realName[MAXPATHLEN];
    char *objectName;
    FILE *thisFile;
    struct stat stbuf;
    struct attributes *attributes;
    struct attributes *tempAttribute;

    filetype_CanonicalizeFilename(realName, filename, sizeof(realName) - 1);
    filename = realName;

    if (stat(filename, &stbuf) < 0 || (stbuf.st_mode & S_IFMT) == S_IFDIR)
        return -1;

    if ((thisFile = fopen(filename, "r")) == NULL)
            return -1;

    self->lastTouchDate = (long) stbuf.st_mtime;

    objectName = filetype_Lookup(thisFile, filename, &objectID, &attributes);

/* This next thing is a hack. Really need a flags parameter so we can keep
 * the thing readonly if need be.
 */
    self->readOnly = FALSE;
    for (tempAttribute = attributes; tempAttribute != NULL; tempAttribute = tempAttribute->next)
        if (strcmp(tempAttribute->key, "readonly") == 0)
            self->readOnly = tempAttribute->value.integer;

    if (objectName == NULL)
        objectName = "text"; /* The default... */

    if (strcmp(class_GetTypeName(buffer_GetData(self)), objectName) == 0) {

	long version;

        dataobject_SetAttributes(buffer_GetData(self), attributes);
        dataobject_Read(buffer_GetData(self), thisFile, objectID);
        dataobject_NotifyObservers(buffer_GetData(self), 0);
        buffer_SetFilename(self, filename);

	version = dataobject_GetModified(buffer_GetData(self));
        buffer_SetCkpClock(self, 0);
        buffer_SetCkpVersion(self, version);
        buffer_SetWriteVersion(self, version);
    }
    else
        returnCode = -1;

    fclose(thisFile);
    buffer_NotifyObservers(self, 0);/* Tuck it into slot. */
    return returnCode;
}

/* The interaction between this routine and buffer_Create is treacherous.
 * I have buffer_Create allocate the data object so buffer_Destroy will
 * deallocate it.  Yet this routine must look up the class of the data
 * object because it has the FILE *. Perhaps this is all bogus...
 */

struct buffer *buffer__GetBufferOnFile(classID, filename, flags)
struct classheader *classID;
char *filename;
long flags;
{
    char realName[MAXPATHLEN];
    struct buffer *thisBuffer;
    struct stat statBuf;
    boolean fileExists, fileIsDir;
    int objectID;
    long version;
    boolean readOnly;
    char bufferName[100], *objectName;
    FILE *thisFile;
    struct attributes *attributes, *tempAttribute, readOnlyAttribute;

    filetype_CanonicalizeFilename(realName, filename, sizeof (realName) - 1);
    filename = realName;

    fileExists = fileIsDir = FALSE;
    if (stat(filename, &statBuf) >= 0) {
	fileExists = TRUE;
	if ((statBuf.st_mode & S_IFMT) == S_IFDIR)
	    fileIsDir = TRUE;
    }

    if (fileIsDir) {
	struct attributes attr;
	struct dataobject *dobj;
	/* Force filename to end in '/' before visiting directories. */
	if (filename[strlen(filename) - 1] != '/') {
	    errno = EISDIR;
	    return NULL;
	}
	objectName = "dired";
	strcpy(bufferName, "Directory-Edit-1");
	/* Use existing dired buffer and dired if exists */
	thisBuffer = buffer_FindBufferByName(bufferName);
	if (thisBuffer) {
	    /* In case it exists by accident */
	    dobj = thisBuffer->bufferData;
	    if (strcmp(class_GetTypeName(dobj), objectName) != 0)
		thisBuffer = NULL;
	} else {
	    /* Create a new dired and dired buffer */
	    if ((flags & buffer_MustExist) && ! fileExists)
		return NULL;
	    dobj = (struct dataobject *) class_NewObject(objectName);
	    if (dobj == NULL)
		thisBuffer = NULL;
	    else
		thisBuffer = buffer_Create("Directory-Edit-1", NULL, NULL, dobj);
	}
	if (thisBuffer == NULL) {
	    errno = EISDIR;
	    return NULL;
	}
	/* Tell it which dir to use */
	attr.key = "dir";
	attr.value.string = filename;
	attr.next = NULL;
	dataobject_SetAttributes(dobj, &attr);
	if (thisBuffer->filename != NULL) {
	    free(thisBuffer->filename);
	    thisBuffer->filename = NULL;
	}
    } else {

	if ((flags & buffer_MustExist) && ! fileExists)
	    return NULL;

	/* Try to find existing buffer. */

	if ((flags & buffer_ForceNew) == 0)
	    if ((thisBuffer = buffer_FindBufferByFile(filename)) != NULL)
		return thisBuffer;

	if ((thisFile = fopen(filename, "r")) == NULL) {
	    if (access(filename, W_OK) < 0) {
		extern char *rindex();
		char *slash;
		if (errno != ENOENT)
		    return NULL;
		slash = rindex(filename, '/');
		if (slash == NULL)
		    return NULL;
		*slash = '\0';
		if (access(filename, W_OK) < 0) {
		    *slash = '/'; return NULL;
		}
		*slash = '/';
	    }
	}

	objectName = filetype_Lookup(thisFile, filename, &objectID, &attributes);

	readOnly = FALSE;
	for (tempAttribute = attributes; tempAttribute != NULL; tempAttribute = tempAttribute->next)
	    if (strcmp(tempAttribute->key, "readonly") == 0)
		readOnly = tempAttribute->value.integer;

	if (!readOnly && (flags & buffer_ReadOnly)) {
	    readOnlyAttribute.next = attributes;
	    readOnlyAttribute.key = "readonly";
	    readOnlyAttribute.value.integer = TRUE;
	    attributes = &readOnlyAttribute;
	    readOnly = TRUE;
	}

	buffer_GuessBufferName(filename, bufferName, sizeof(bufferName));
	thisBuffer = buffer_Create(bufferName, filename, objectName, NULL);

	if (thisBuffer == NULL) {
	    errno = 0;      /* Don't signal Unix error unless one occurred */
	    return NULL;
	}

	dataobject_SetAttributes(buffer_GetData(thisBuffer), attributes);
	if (thisFile != NULL) {
	    dataobject_Read(buffer_GetData(thisBuffer), thisFile, objectID);
	    fclose(thisFile);
/* 	    thisBuffer->lastTouchDate = (long) time(0); */
	    thisBuffer->lastTouchDate = (long) statBuf.st_mtime;
	}

	thisBuffer->readOnly = readOnly;
    }

    version = dataobject_GetModified(buffer_GetData(thisBuffer));
    buffer_SetCkpClock(thisBuffer, 0);
    buffer_SetCkpVersion(thisBuffer, version);
    buffer_SetWriteVersion(thisBuffer, version);
    return thisBuffer;
}

void buffer__GuessBufferName (classID, filename, bufferName, nameSize)
    struct classheader *classID;
    char *filename, *bufferName;
    int nameSize;
{

    register int uniquefier, nameLength;
    char *slash;
    extern char *rindex();

    slash = rindex(filename, '/');
    if (slash != NULL)
        strncpy(bufferName, ++slash, nameSize - 2); /* Save room for uniquefier. */
    else
        strncpy(bufferName, filename, nameSize - 2); /* Save room for uniquefier. */

/* Find out if buffer exists. */
    if (buffer_FindBufferByName(bufferName) == NULL)
        return;
/* Otherwise we must uniquify it.
 * This is a bug, it may overflow the string buffer we were given...
 */
    nameLength = strlen(bufferName);
    for (uniquefier = 1; uniquefier < 100; uniquefier++) {
        sprintf(bufferName + nameLength, "-%d", uniquefier);
        if (buffer_FindBufferByName(bufferName) == NULL)
            return;
    }
    *bufferName = '\0'; /* Make sure we don't return a non-unique buffername. */
    return;
}

int buffer__WriteToFile(self, filename, flags)
    struct buffer *self;
    char *filename;
    long flags;
{

    char realName[MAXPATHLEN];
    char tempFilename[MAXPATHLEN];
    char *originalFilename = NULL;
    int closeCode;
    int errorCode;
    int originalMode;
    int fd;
    FILE *outFile;
    struct stat statBuf;
    boolean makeBackup = !buffer_GetMadeBackup(self);
    boolean differentFile = FALSE;
    boolean fileExists = TRUE;

    errorCode = 0;
    if (filename == NULL) {
	if ((filename = self->filename) == NULL)
	    return -1;
    }
    else {
	filetype_CanonicalizeFilename(realName, filename, sizeof(realName) - 1);
	filename = realName;
	if ((self->filename == NULL) || (strcmp(filename, self->filename) != 0)) { /* If writing to a different file than normal. */
	    differentFile = TRUE;
	    makeBackup = TRUE;
	}
    }

    if (access(filename, W_OK) < 0) {
	if (errno == EACCES) {
	    return -1;
	} else if (errno == ENOENT) {
	    fileExists = makeBackup = FALSE;
	}
    }

    if (stat(filename, &statBuf) >= 0)
	originalMode = statBuf.st_mode & (~S_IFMT);
    else
	originalMode = 0666; /* Default open mode. */

    if (!overwriteFiles && differentFile && makeBackup) {
        char prompt[MAXPATHLEN + sizeof("``'' already exists. Overwrite? ")];
        char answer[5];

        sprintf(prompt, "``%s'' already exists. Overwrite? ", filename);

        if (message_AskForString(NULL, 0, prompt, NULL, answer, sizeof(answer)) < 0 || (answer[0] != 'y' && answer[0] != 'Y'))
            return -1;
    }

    if ((flags & buffer_MakeBackup) && backupExtension && makeBackup) {
	strcpy(tempFilename, filename);
	strcat(tempFilename, backupExtension);
	if ((rename(filename, tempFilename) < 0) && errno != ENOENT)
	    return -1;
	buffer_SetMadeBackup(self, TRUE);
    }
    else if ((flags & buffer_ReliableWrite) && fileExists) {

	char *endString;
	int counter = 1;

	strcpy(tempFilename, filename);
	strcat(tempFilename, ".NEW");
	endString = tempFilename + strlen(tempFilename);
	while (access(tempFilename, F_OK) >= 0) /* While the file exists. */
	    sprintf(endString, ".%d", counter++);
	originalFilename = filename;
	filename = tempFilename;
    }

    if ((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, originalMode)) < 0
	 || (outFile = fdopen(fd, "w")) == NULL)
	return -1;
    dataobject_Write(self->bufferData, outFile, im_GetWriteID(), 0);
    fflush(outFile);

/* This next test is somewhat bogus. In theory, if an error occured while
 * writing the file, this will catch it. In practice, dataobjects are not
 * required to use stdio to write objects and stdio also seems to lose error
 * indications on occasion. In any event, we assume that if the error has
 * occured, there is no point in using vclose to close the file and we
  * preserve the original file. The code here actually simulates the error
  * return (with an unknown error 0 since stdio doesn't give any indication of
  * what actually went wrong.
  */
    if (ferror(outFile)) {
	fclose(outFile);
	errorCode = 0;
	closeCode = -1;
    }
    else {
#ifdef AFS_ENV
	if (flags & buffer_ReliableWrite) { /* Go for the expensive but safe operation. */
	    if ((closeCode = vclose(fileno(outFile))) < 0) /* stdio can trash errno. */
		errorCode = errno; /* Protect it from the fclose below. */
	    else
		if (originalFilename != NULL)
		    if ((closeCode = rename(filename, originalFilename)) < 0)
			errorCode = errno;
	}
	else /* Fast and loose. */
	    if ((closeCode = close(fileno(outFile))) < 0) /* stdio can trash errno. */
		errorCode = errno; /* Protect it from the fclose below. */
#else /* AFS_ENV */
	if ((closeCode = close(fileno(outFile))) < 0) /* stdio can trash errno. */
	    errorCode = errno; /* Protect it from the fclose below. */
	else
	    if (originalFilename != NULL)
		if ((closeCode = rename(filename, originalFilename)) < 0)
		    errorCode = errno;
#endif /* AFS_ENV */
	fclose(outFile); /* Free stdio resources. */
	if (closeCode >= 0) { /* Reset readonly mode. */

	    struct attributes attributes;

	    attributes.next = NULL;
	    attributes.key = "readonly";
	    if (access(filename, W_OK) == -1 && errno == EACCES)
		attributes.value.integer = self->readOnly = TRUE;
	    else
		attributes.value.integer = self->readOnly = FALSE;
	    dataobject_SetAttributes(buffer_GetData(self), &attributes);
	}
    }
    errno = errorCode;
/* Get a fresh stat() on the file after we've now written it. */
    if(differentFile == FALSE){
	/* don't modify if writing to another file */
	self->lastTouchDate = buffer_GetFileDate(self);
    }
    return closeCode;
}

long buffer__GetFileDate(self)
struct buffer *self;
{
    struct stat stbuf;
    if (self->filename == NULL)
        return 0L;
    if (stat(self->filename, &stbuf) < 0)
        return 0L;
    return (long) stbuf.st_mtime;
}

boolean buffer__InitializeClass(classID)
    struct classheader *classID;
{
    char *s;

    if ((s = environ_GetProfile("BackupExtension")) != NULL) {
        backupExtension = malloc(strlen(s) + 1);
        strcpy(backupExtension, s);
    }

    if ((s = environ_GetProfile("CheckpointExtension")) != NULL) {
        checkpointExtension = malloc(strlen(s) + 1);
        strcpy(checkpointExtension, s);
    }

    overwriteFiles = environ_GetProfileSwitch("OverwriteFiles", overwriteFiles);

    return TRUE;
}

void buffer__SetDefaultObject(classID, objectname)
struct classheader *classID;
char *objectname;
{
    if (objectname != NULL)
        strncpy(defaultobjectname,objectname, sizeof(defaultobjectname));
    else
        strncpy(defaultobjectname, DEFAULTOBJECT, sizeof(defaultobjectname));
}
void buffer__SetDefaultViewname(self,name)
struct buffer *self;
char *name;
{
    if(self->viewname != NULL)
	free(self->viewname);
    if(name == NULL) self->viewname = NULL;
    else {
	self->viewname = malloc(strlen(name) + 1);
	if(self->viewname == NULL) return;
	strcpy(self->viewname, name);
    }
}
