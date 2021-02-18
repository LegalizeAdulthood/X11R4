/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framecmd.c,v 2.33 89/11/13 13:07:43 ghoti Exp $ */
/* $ACIS:framecmd.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framecmd.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framecmd.c,v 2.33 89/11/13 13:07:43 ghoti Exp $";
#endif /* lint */

/* framecmd.c
 * The user commands for the frame package.
 * Makes up the file, buffer, and window handling functions of the editor.
 */

/* 
 * How to improve this module:
 * 
 * 1. Improve the message interface and you can simplify functions 
 * like preventOverwriteLossage.
 * 
 * 2. Convince everyone to allow interface overhauls and export 
 * functions like saveBuffer and saveTheWorld.
 * 
 * 3. Think of clever ways to eliminate most of the file manipulation 
 * commands (hide the file system).
 * 
 */


#include <andyenv.h>
#include <util.h>
#include <class.h>
#include <frame.ih>
#include <view.ih>
#include <dataobj.ih>
#include <buffer.ih>
#include <im.ih>
#include <message.ih>
#include <keymap.ih>
#include <menulist.ih>
#include <print.ih>
#include <text.ih>
#include <environ.ih>
#include <proctbl.ih>
#include <bind.ih>
#include <cursor.ih>
#include <complete.ih>
#include <style.ih>
#include <fontdesc.ih>
#include <framecmd.eh>

#include <andrewos.h> /* sys/types.h sys/file.h */
#include <sys/param.h>
#include <signal.h> /* needed for hp - sometimes included in sys/param.h */
#include <sys/stat.h>
#include <sys/errno.h>


extern int errno;
extern char *rindex();
static struct keymap *framecmdsKeymap;
static struct menulist *framecmdsMenus;
static struct cursor *waitCursor;


/* isString(arg)
	tests arg to see if it is a string
	returns TRUE if it is and FALSE otherwise
*/
static jmp_buf trap;
static SigHandler() {longjmp(trap, 1);}
	static boolean
isString(arg)
	char *arg;
{
	int (*oldBus)(), (*oldSeg)(); /* save signal handlers */
	char c;

	/* first test to see if it is a character instead of a pointer */
	if ((long)arg < (1<<16) && (long)arg > -(1<<16))  
		return FALSE;	/* too small an integer to be a pointer (in ATK) */

	/* now be sure we can fetch from the location */
	oldBus = signal(SIGBUS, SigHandler);
	oldSeg = signal(SIGSEGV, SigHandler);
	if (setjmp(trap) != 0) 
		/* return here from longjmp */
		c = '\1';	/* not a legal string */
	else 
		/* normal setjmp return location */
		c = *arg;		/* this could fail if arg were not a pointer */
	signal(SIGBUS, oldBus);
	signal(SIGSEGV, oldSeg);

	/* return value depending on whether it points to an ASCII printable character */
	return (c >= ' ' && c < '\177');
}

static boolean bufferDirtyP(buffer)
    struct buffer *buffer;
{
    return !buffer_GetScratch(buffer) &&
	buffer_GetWriteVersion(buffer) <
	    dataobject_GetModified(buffer_GetData(buffer));
}



/* 
 * preventOutofSyncLossage checks to see if a given buffer is in-sync 
 * with the version on disk.  If the version on disk has changed, the 
 * user may choose to re-read it.
 */

static char SyncLossage[] =
    "The version on disk is more recent than the version in the buffer.";
static char *SyncChoices[] = {
    "Read the new version from disk",
    "Use the old version in the buffer",
    0};
#define SYNC_READ  0
#define SYNC_LOSE  1

static int LocalReadFile();

static int
preventOutofSyncLossage(outputFrame, buffer)
    struct frame *outputFrame;
    struct buffer *buffer;
{
    if (buffer_GetFileDate(buffer) <= buffer_GetLastTouchDate(buffer))
	return 0;
    else {
	long answer;

	if (message_MultipleChoiceQuestion(outputFrame, 0, SyncLossage,
					   SYNC_READ, &answer, SyncChoices,
					   NULL) == -1)
	    answer = SYNC_LOSE;

	if (answer == SYNC_READ)
	    return LocalReadFile(outputFrame, NULL, TRUE);
	else {
	    message_DisplayString(outputFrame, 0, "");
	    return 0;
	}
    }
}

/* 
 * preventReversionLossage should be called when a buffer is about to 
 * re-read from disk.  It guards against the accidental loss of 
 * modifications without offering to trash the unmodified copy.
 */

static char ReversionLossage [] =
    "Rereading this file will cause changes in the buffer to be lost.";
static char *ReversionChoices [] = {
    "Cancel",
    "Read the file from disk (losing the changes in the buffer)",
    0};
#define REVERSION_CANCEL 0
#define REVERSION_LOSE 1

static boolean
preventReversionLossage(outputFrame, buffer)
    struct frame *outputFrame;
    struct buffer *buffer;
{
    if (!bufferDirtyP(buffer))
	return TRUE;
    else {
	long answer;

	if (message_MultipleChoiceQuestion(outputFrame, 0,
					   ReversionLossage, REVERSION_CANCEL,
					   &answer, ReversionChoices, NULL)
	    == -1)
	    return FALSE;
	else if (answer == REVERSION_LOSE)
	    return TRUE;
	else {
	    message_DisplayString(outputFrame, 0,
	  	"You can use `Save As' to save your changes to a different file.");
	    return FALSE;
	}
    }
}


/* 
 * preventOverwriteLossage guards against writing the contents of a 
 * buffer to a file that has been changed since it was last read.  
 * Returns true if the operation should proceed.
 */

static char OverwriteLossage[] =
    "The file `%.*s' has changed on disk since it was last read.";
static char *OverwriteChoices[] = {
    "Cancel",
    "Proceed with save (losing the changes on disk).",
    0};
#define OVERWRITE_CANCEL 0
#define OVERWRITE_LOSE   1

static boolean
preventOverwriteLossage(outputFrame, buffer)
    struct frame *outputFrame;
    struct buffer *buffer;
{
    long foo = buffer_GetFileDate(buffer);
    if ( foo == 0l || foo == buffer_GetLastTouchDate(buffer))
	return TRUE;
    else {
	long answer;
	char msg[sizeof(OverwriteLossage) + MAXPATHLEN];

	sprintf(msg, OverwriteLossage, MAXPATHLEN, buffer_GetFilename(buffer));
	if (message_MultipleChoiceQuestion(outputFrame, 0,
					   msg, OVERWRITE_CANCEL,
					   &answer, OverwriteChoices, NULL)
	    == -1)
	    return FALSE;
	else if (answer == OVERWRITE_LOSE)
	    return TRUE;
	else {
	    message_DisplayString(outputFrame, 0,
	  	"You can use `Save As' to write to a different file.");
	    return FALSE;
	}
    }
}



/*
 * saveBuffer tries to write a file to disk.  outputFrame is used 
 * for delivering messages to the user.
 * 
 * The buffer's default filename can be overridden by a non-null 
 * filename.
 * 
 * A number less than 0 is returned on failure.
 */

#define NOFILE_MSG \
   "No file associated with buffer. Use \"Save As\" to save buffer contents."
#define DIR_MSG \
   "Write aborted: specified output file is a directory."

static int
saveBuffer(outputFrame, buffer, filename)
    struct frame *outputFrame;
    struct buffer *buffer;
    char *filename;
{
    int result;
    long version;
    char message[sizeof("Wrote file ''.") + sizeof("Could not save file") + MAXPATHLEN];
    char *buffile;
    struct stat statbuf;

    if (outputFrame == NULL || buffer == NULL)
	return -1;

    buffile = buffer_GetFilename(buffer);

    if (filename == NULL)
	filename = buffile;

    if (filename == NULL) {
        message_DisplayString(outputFrame, 0, NOFILE_MSG);
        return -1;
    }

    if (stat(filename, &statbuf) == 0 && (statbuf.st_mode & S_IFDIR)) {
	message_DisplayString(outputFrame, 0, DIR_MSG);
	return -1;
    }

    /* 
     * Unless we're changing the filename, make sure we're not 
     * overwriting changes that have been made on disk.
     */
    if (buffile != NULL && !strcmp(buffile, filename) &&
	!preventOverwriteLossage(outputFrame, buffer))
	return -1;


    im_SetProcessCursor(waitCursor);

    result = buffer_WriteToFile(buffer, filename,
				buffer_ReliableWrite | buffer_MakeBackup);

    if (result >= 0) {
        version = dataobject_GetModified(buffer_GetData(buffer));

        unlink(buffer_GetCkpFilename(buffer));
        buffer_SetCkpClock(buffer, 0);
        buffer_SetCkpVersion(buffer, version);
        buffer_SetWriteVersion(buffer, version);

        sprintf(message, "Wrote file '%.*s'.%s", MAXPATHLEN, filename,
		buffer->readOnly ? " File is now readonly." : "");
        message_DisplayString(outputFrame, 0, message);
    }
    else
        switch (errno) {
            case EACCES:
                message_DisplayString(outputFrame, 0,
				 "Could not save file; permission denied.");
                break;
#ifdef ETIMEDOUT
            case ETIMEDOUT:
                message_DisplayString(outputFrame, 0,
				 "Could not save file; a server is down.");
                break;
#endif /* ETIMEDOUT */
#ifdef EFAULT
            case EFAULT:
                message_DisplayString(outputFrame, 0,
				 "Could not save file; a server is down.");
                break;
#endif /* EFAULT */
#ifdef EDQUOT
            case EDQUOT:
                message_DisplayString(outputFrame, 0,
			    "Could not save file; you are over your quota.");
                break;
#endif /* EDQUOT */
            case ENOSPC:
                message_DisplayString(outputFrame, 0,
		      "Could not save file; no space left on partition.");
                break;
#ifdef EIO
            case EIO:
                message_DisplayString(outputFrame, 0,
	 "Could not save file; an I/O error occurred on the disk.");
                break;
#endif /* EIO */
            case EISDIR:
                message_DisplayString(outputFrame, 0,
	 "File not found; could not create. Attempt to write to a directory.");
                break;
            default:
                sprintf(message, "Could not save file: %s.", UnixError(errno));
                message_DisplayString(outputFrame, 0, message);
        }

    im_SetProcessCursor(NULL);
    return result;
}



static boolean frame_clear2exit()
{
    return buffer_Enumerate(bufferDirtyP, 0) == NULL;
}


/* 
 * saveTheWorld iterates over all buffers, trying to save those which 
 * have been modified.  It ignores buffers without filenames.
 */

struct saveTheWorldRock {
    /* input fields: */
    struct frame *outputFrame;

    /* output fields: */
    char *names;
    int lengthLeft;
    boolean anyModified;
    boolean modifiedWithoutFilenames;
};

static boolean
saveAllWork(buffer, returnBuf)
    struct buffer *buffer;
    struct saveTheWorldRock *returnBuf;
{
    int len;

    if (bufferDirtyP(buffer)) {
        if (buffer_GetFilename(buffer) != NULL) {
            returnBuf->anyModified = TRUE;
	    if (saveBuffer(returnBuf->outputFrame, buffer, NULL) < 0)
                if ((len = (strlen(buffer_GetName(buffer)) + 1))
		    < returnBuf->lengthLeft) {
                    strcat(returnBuf->names, " ");
                    strcat(returnBuf->names, buffer_GetName(buffer));
                    returnBuf->lengthLeft -= len;
		    }
	} else
            returnBuf->modifiedWithoutFilenames = TRUE;
    }
    return FALSE; /* Keep on enumerating. */
}

/* 
 * The return value is TRUE if the save-all succeeded. We err in the 
 * direction of being conservative by returning FALSE if there are 
 * modified buffers without files.
 */
boolean
saveTheWorld(outputFrame)
    struct frame *outputFrame;
{
    struct saveTheWorldRock returnCode;
    char errors[1024];
    int origLen;
    boolean succeeded = FALSE;

    returnCode.outputFrame = outputFrame;
    returnCode.names = errors + sizeof("Couldn't save buffers:") - 1;
    *returnCode.names = '\0';
    origLen = returnCode.lengthLeft = sizeof(errors) -
	sizeof("Couldn't save buffers:") - 2;
    returnCode.anyModified = returnCode.modifiedWithoutFilenames = FALSE;

    im_SetProcessCursor(waitCursor);
    buffer_Enumerate(saveAllWork, &returnCode);
    im_SetProcessCursor(NULL);

    if (returnCode.lengthLeft != origLen) {
        strncpy(errors, "Couldn't save buffers:",
		sizeof("Couldn't save buffers:") - 1);
        message_DisplayString(outputFrame, 0, errors);
	succeeded = FALSE;
    }
    else {

        char messageBuf[256]; /* This is enuf. */

	succeeded = TRUE;	/* but look out below */

        if (returnCode.anyModified)
            strcpy(messageBuf, "Saved all modified buffers.");
        else
            strcpy(messageBuf, "No buffers needed saving.");
        if (returnCode.modifiedWithoutFilenames) {
            strcat(messageBuf,
		   " There are modified buffers without associated files.");
	    succeeded = FALSE;
	}
        message_DisplayString(outputFrame, 0, messageBuf);
    }

    return succeeded;
}



/* 
 * preventBufferLossage checks to see that a buffer-destroying operation 
 * will not be disasterous.  If an unsaved, modified buffer will be 
 * lost, preventBufferLossage issues a standard warning and offers to save 
 * one or all buffers. 
 *
 * outputFrame is the frame in which the dialog box will pop up. 
 * 
 * preciousBuffer, if non-null, is the only buffer that 
 * preventBufferLossage will try to protect.  If it is null, 
 * preventBufferLossage will try to protect all buffers.
 * 
 * The return value is FALSE if the user wishes the operation to be 
 * aborted.
 */

static char lossageWarning[] =
  "You have unsaved changes that would be lost by this operation.";
static char *lossageChoices[] = {
	"Cancel",
	"Save changes and proceed",
	"Proceed without saving changes",
	NULL};

#define LOSSAGE_CANCEL 0
#define LOSSAGE_SAVE   1
#define LOSSAGE_LOSE   2

static boolean preventBufferLossage(outputFrame, preciousBuffer)
    struct frame *outputFrame;
    struct buffer * preciousBuffer;
{
    long answer;

    if (preciousBuffer != NULL ? !bufferDirtyP(preciousBuffer)
			       : frame_clear2exit())
	return TRUE;

    if (message_MultipleChoiceQuestion(outputFrame, 0,
				       lossageWarning, LOSSAGE_CANCEL,
				       &answer, lossageChoices, NULL)
	== -1)
	return FALSE;

    switch (answer) {
    case LOSSAGE_CANCEL:
	return FALSE;

    case LOSSAGE_LOSE:
	return TRUE;

    case LOSSAGE_SAVE:
	if(preciousBuffer != NULL)
	    return  (saveBuffer(outputFrame, preciousBuffer,NULL) >= 0);
	else 
	    return  saveTheWorld(outputFrame);
    }
}

static void
bufferFilename(buffer, filename)
    struct buffer *buffer;
    char *filename;		/* Output: MAXPATHLEN */
{
    if (buffer_GetFilename(buffer) != NULL)
        strcpy(filename, buffer_GetFilename(buffer));
    else {
        im_GetDirectory(filename);
        strcat(filename, "/");
    }
}


static void
bufferDirectory(buffer, dir)
    struct buffer *buffer;
    char *dir;			/* Output: At least MAXPATHLEN, please. */
{
    char *slash;

    if ((buffer_GetFilename(buffer)) != NULL) {
        strcpy(dir, buffer_GetFilename(buffer));
        slash = rindex(dir, '/');
        if (slash != NULL)
            slash[1] = '\0';
    } else {
        im_GetDirectory(dir);
        strcat(dir, "/");
    }
}


/* Used to implement recursive editing functions.
 * Executes function recursively and the pops back
 * to the current buffer.
 */
static void frame_SaveExcursion(self, function)
    struct frame *self;
    void (*function)();
{
    char bufferName[100];
    struct buffer *oldBuffer;

    if (self->buffer != NULL)
        strncpy(bufferName, buffer_GetName(self->buffer), sizeof(bufferName));
    else
        *bufferName = '\0';
    (*function)(self);
    if (*bufferName != '\0') {
        oldBuffer = buffer_FindBufferByName(bufferName);
        if (oldBuffer != NULL)
            frame_SetBuffer(self, oldBuffer, TRUE);
    }
}

/* Auxilliary function for RecursiveVisitFile */
static void frame_RecursiveVisitWork(self)
    struct frame *self;
{
    long code;

    code = frame_VisitFilePrompting(self, "Recursive visit file: ", FALSE);
    if (code >= 0)
        im_KeyboardProcessor();
}

/* Allows you to visit a file and then pop back to the current buffer. */
void frame_RecursiveVisitFile(self)
    struct frame *self;
{
    frame_SaveExcursion(self, frame_RecursiveVisitWork);
}

/* 
 * Used to get a function pointer to im_KeyboardProcessor (which is a 
 * macro...) 
 */ 
static void myKeyboardProcessor()
{
    im_KeyboardProcessor();
}
 
/* A recursive edit... Plain, simple, and useless. */
void frame_RecursiveEdit(self)
    struct frame *self;
{
    frame_SaveExcursion(self, myKeyboardProcessor);
}

static int countFrames(self, rock)
    struct frame *self;
    long *rock;
{
    (*rock)++;
    return 0;
}
static char lastWindowWarning[] =
  "This is the last window.";
static char *lastWindowChoices[] = {
	"Continue Running",
	"Quit Application",
	NULL};

#define lastWindow_CANCEL 0
#define lastWindow_QUIT   1

void frame_Exit();

void frame_DeleteWindow(self)
    struct frame *self;
{
    long count = 0;
    extern struct frame * frame__Enumerate(); /* BOGUS!!! */

    frame_Enumerate(countFrames, &count);
    if (count > 1) {
	im_Destroy(frame_GetIM( self));
	frame_Destroy(self);
    }
    else {
	long answer;
	if (message_MultipleChoiceQuestion(self, 0,
					   lastWindowWarning, lastWindow_CANCEL,
					   &answer, lastWindowChoices, NULL)
	    == -1)
	    return;
	switch(answer){
	    case lastWindow_CANCEL:
		return;

	    case lastWindow_QUIT :
		frame_Exit(self);
	}
    }
}

void frame_ExitRecursiveEdit(self)
    struct frame *self;
{
    if (im_KeyboardLevel() <= 1)
        message_DisplayString(self, 0, "Not in recursive edit");
    else
        im_KeyboardExit();
}


void frame_Exit(self)
    struct frame *self;
{
    if (im_KeyboardLevel() == 1) {
	if (!preventBufferLossage(self, NULL)) {
            message_DisplayString(self, 0, "Exit aborted.");
            return;
        }
    }
    im_KeyboardExit();
}

void frame_NewWindow(self)
    struct frame *self;
{
    register struct buffer *buffer;
    struct frame *newFrame;
    struct im *window;

    newFrame = frame_New();
    frame_SetCommandEnable(newFrame, TRUE);

    window = im_Create(NULL);
    im_SetView(window, newFrame);

    frame_PostDefaultHandler(newFrame, "message",
			     frame_WantHandler(newFrame, "message"));

    buffer = frame_GetBuffer(self);
    frame_SetBuffer(newFrame, buffer, TRUE);
}

static boolean BufferCompletionWork(buffer, data)
    struct buffer *buffer;
    struct result *data;
{
    completion_CompletionWork(buffer_GetName(buffer), data);
    return FALSE;
}

/* ARGUSED */
static enum message_CompletionCode BufferComplete(partial, dummyData,
						  buffer, bufferSize)
    char *partial;
    long dummyData; /* Just along for the ride... */
    char *buffer;
    int bufferSize;
{
    struct result result;
    char textBuffer[100];

    *textBuffer = '\0';
    result.partial = partial;
    result.partialLen = strlen(partial);
    result.bestLen = 0;
    result.code = message_Invalid;
    result.best = textBuffer;
    result.max = sizeof(textBuffer) - 1; /* Leave extra room for a NUL. */

    buffer_Enumerate(BufferCompletionWork, &result);

    strncpy(buffer, result.best, bufferSize);
    if (result.bestLen == bufferSize) /* Now make sure buffer ends in a NUL. */
        buffer[result.bestLen] = '\0';
    return result.code;
}

struct helpData {
    char *partial;
    int (*textFunction)();
    long textRock;
};

static boolean BufferHelpWork(buffer, helpData)
struct buffer *buffer;
struct helpData *helpData;
{
    char infoBuffer[1024];

    if (completion_FindCommon(helpData->partial,
       buffer_GetName(buffer)) == strlen(helpData->partial)) {
        char *modStatus;
        char sizeBuf[16];
        char *className;
        char *fileName, shortFileName[256];
	long md;
	
	if(buffer_GetScratch(buffer))
	    modStatus = "";
	else {
	    md = dataobject_GetModified(buffer_GetData(buffer));
	    if (buffer_GetWriteVersion(buffer) < md) {
		if (buffer_GetCkpVersion(buffer) >= md)
		    modStatus = "Ckp";
		else
		    modStatus = "Mod";
	    } else if (buffer->readOnly)
		modStatus = "R/O";
	    else
		modStatus = "";
	}
        className = class_GetTypeName(buffer_GetData(buffer));

        sizeBuf[0] = '\0';
        if (class_IsTypeByName(className, "text"))
            sprintf(sizeBuf, "%d",
              text_GetLength((struct text *) buffer_GetData(buffer)));

        fileName = buffer_GetFilename(buffer);
        shortFileName[0] = '\0';

        if (fileName != NULL) {
            char *home = environ_Get("HOME");
            if (home != NULL) {
                int hlen = strlen(home);
                if (strncmp(fileName, home, hlen) == 0) {
                    strcpy(shortFileName, "~");
                    fileName += hlen;
                }
            }
            strcat(shortFileName, fileName);
        }

        sprintf(infoBuffer, "%-16s%7s %-3s %-8s %s",
          buffer_GetName(buffer), sizeBuf, modStatus,
          className, shortFileName);

        (*helpData->textFunction)(helpData->textRock,
            message_HelpListItem, infoBuffer, NULL);
    }
    return FALSE; /* Keep on enumerating. */
}

static void BufferHelp(partial, listInfo, helpTextFunction, helpTextRock)
char *partial;
long listInfo;
int (*helpTextFunction)();
long helpTextRock;
{
    struct helpData helpData;

    helpData.partial = partial;
    helpData.textFunction = helpTextFunction;
    helpData.textRock = helpTextRock;

    buffer_Enumerate(BufferHelpWork, &helpData);
}

static char lastBuffer[100] = "";

static boolean FindFirstBuffer(tryBuffer, cannotMatchBuffer)
struct buffer *tryBuffer, *cannotMatchBuffer;
{
    /* Find first one not matching */
    return (tryBuffer != cannotMatchBuffer);
}

void frame_OldBuffer(self)
    struct frame *self;
{

    char bufferName[100], prompt[256];
    register struct buffer *buffer;

    if (*lastBuffer == '\0') {
        buffer = frame_GetBuffer(self);   /* Any except current */
        buffer = buffer_Enumerate(FindFirstBuffer, buffer);
        if (buffer != NULL)
            strcpy(lastBuffer, buffer_GetName(buffer));
    }
    if (*lastBuffer != '\0')
        sprintf(prompt, "Visit existing buffer [%.100s] : ", lastBuffer);
    else
        strcpy(prompt, "Visit existing buffer: ");
    if (message_AskForStringCompleted(self, 0, prompt,
				      (*lastBuffer == '\0') ?
				        NULL : lastBuffer, bufferName,
				      sizeof(bufferName), NULL, BufferComplete,
				      BufferHelp, 0,
				      message_MustMatch |
				        message_NoInitialString) == -1)
        return;
    buffer = buffer_FindBufferByName(bufferName);
    if (self->buffer != buffer)
        strncpy(lastBuffer, buffer_GetName(self->buffer), sizeof(lastBuffer));
    frame_SetBuffer(self, buffer, TRUE);
    message_DisplayString(self, 0, "Done.");
}

void frame_VisitBuffer(self)
    struct frame *self;
{
    char bufferName[100], prompt[100 + sizeof("Visit buffer [] : ") - 1];
    register struct buffer *buffer;

    if (*lastBuffer == '\0') {
        buffer = frame_GetBuffer(self);     /* Any except current */
        buffer = buffer_Enumerate(FindFirstBuffer, buffer);
        if (buffer != NULL)
            strcpy(lastBuffer, buffer_GetName(buffer));
        else
            strcpy(lastBuffer, "Scratch");
    }
    sprintf(prompt, "Visit buffer [%.100s] : ", lastBuffer);
    if (message_AskForStringCompleted(self, 0, prompt,
      lastBuffer, bufferName, sizeof(bufferName), NULL, BufferComplete,
      BufferHelp, 0, message_NoInitialString) == -1)
        return;
    buffer = buffer_FindBufferByName(bufferName);
    if (buffer == NULL)
        if ((buffer = buffer_Create(bufferName, NULL, NULL, NULL)) == NULL) {
            message_DisplayString(self, 0,
              "Buffer does not exist; could not create.");
            return;
        }
    if (self->buffer != buffer)
        strncpy(lastBuffer, buffer_GetName(self->buffer), sizeof(lastBuffer));
    frame_SetBuffer(self, buffer, TRUE);
    message_DisplayString(self, 0, "Done.");
}

static void ListBuffersWork(helpDoc, dummyData, bufferInfo, dummyInfo)
struct text *helpDoc;
long dummyData;
char *bufferInfo;
char *dummyInfo;
{
    int c;
    int c2;
    long initPos;
    long inc;
    long pos = 0;
    char *tempString;

    do {
        initPos = pos;
        tempString = bufferInfo;
        while (((c = text_GetChar(helpDoc, pos)) == *tempString) &&
	       (*tempString != '\0') && (c != EOF)) {
            pos++;
            tempString++;
        }
        while ((c2 = text_GetChar(helpDoc, pos)) != '\n' && c2 != EOF)
            pos++;
        if (c2 == '\n')
            pos++;
    } while (c != EOF && (c < *tempString));
    if (c == EOF)
        initPos = pos;

    text_InsertCharacters(helpDoc, initPos, bufferInfo,
			  (inc = strlen(bufferInfo)));
    text_InsertCharacters(helpDoc, initPos + inc, "\n", 1);
}

void frame_ListBuffers(self, key)
    struct frame *self;
    long key;
{

    struct buffer *helpBuffer = frame_GetHelpBuffer(self);

    if (frame_GetBuffer(self) == NULL)
        return;

    if (helpBuffer == NULL)
        message_DisplayString(self, 0, "Couldn't list buffers.");
    else {
        struct text *listDoc = (struct text *) buffer_GetData(helpBuffer);
        struct buffer *oldBuffer;
        char oldBufferName[100];
        char dummy[2], *s;
        static struct style *boldStyle = NULL,
           *ulineStyle = NULL, *fixedStyle = NULL;

        if (boldStyle == NULL) {
            boldStyle = style_New();
            style_AddNewFontFace(boldStyle, fontdesc_Bold);
            ulineStyle = style_New();
            style_AddUnderline(ulineStyle);
            fixedStyle = style_New();
            style_SetFontFamily(fixedStyle, "andytype");
            style_AddNewFontFace(fixedStyle, fontdesc_Fixed);
            style_SetFontSize(fixedStyle, style_PreviousFontSize, -2);
            style_AddNoWrap(fixedStyle);
        }

        text_Clear(listDoc);
        BufferHelp("", 0, (int (*) ()) ListBuffersWork, (long) listDoc);

        s = "Name               Size Sav Object   File\n";
        text_InsertCharacters(listDoc, 0, s, strlen(s));
        text_AddStyle(listDoc, 0, 4, ulineStyle);
        text_AddStyle(listDoc, 19, 4, ulineStyle);
        text_AddStyle(listDoc, 24, 3, ulineStyle);
        text_AddStyle(listDoc, 28, 6, ulineStyle);
        text_AddStyle(listDoc, 37, 4, ulineStyle);

        text_AddStyle(listDoc, 0, text_GetLength(listDoc), fixedStyle);

        s = "Current Buffers:\n\n";
        text_InsertCharacters(listDoc, 0, s, strlen(s));
        text_AddStyle(listDoc, 0, strlen(s), boldStyle);

        text_NotifyObservers(listDoc, 0);
        strcpy(oldBufferName, buffer_GetName(frame_GetBuffer(self)));
        frame_SetBuffer(self, helpBuffer, TRUE);
        message_AskForString(self, 0,
          "Press Enter to continue.", "", dummy, 1);
        if ((oldBuffer = buffer_FindBufferByName(oldBufferName)) != NULL) {
            frame_SetBuffer(self, oldBuffer, TRUE);
            message_DisplayString(self, 0, "Done.");
        }
        else
            message_DisplayString(self, 0,
		"Couldn't restore old buffer; it has probably been deleted.");
        text_NotifyObservers(listDoc, 0);
    }
}

struct bufferPair {
    struct buffer *buffer1, *buffer2;
};

static boolean ReplaceBufferWork(frame, bufferPair)
    struct frame *frame;
    struct bufferPair *bufferPair;
{

    if (frame_GetBuffer(frame) == bufferPair->buffer1)
        frame_SetBuffer(frame, bufferPair->buffer2, TRUE);
    return FALSE;
}

static void ReplaceBuffer(oldBuffer, newBuffer)
    struct buffer *oldBuffer, *newBuffer;
{
        struct bufferPair buffers;

        buffers.buffer1 = oldBuffer;
        if ((buffers.buffer2 = newBuffer) == oldBuffer)
            return;
        frame_Enumerate(ReplaceBufferWork, &buffers);
}

void frame_DeleteBuffer(self)
    struct frame *self;
{
    struct buffer *thisBuffer, *targetBuffer;
    char bufferName[100], *defaultName, prompt[356];

    thisBuffer = frame_GetBuffer(self);
    if (thisBuffer != NULL)
        defaultName = buffer_GetName(thisBuffer);
    sprintf(prompt, "Delete buffer [%.100s] : ", defaultName);
    if (message_AskForStringCompleted(self, 0, prompt, defaultName,
				      bufferName, sizeof(bufferName), NULL,
				      BufferComplete, BufferHelp, 0,
				      message_MustMatch |
				      message_NoInitialString) == -1)
        return;

    targetBuffer = buffer_FindBufferByName(bufferName);

    if (!preventBufferLossage(self,targetBuffer)) {
	message_DisplayString(self, 0, "Buffer not deleted.");
	return;
    }

    if (buffer_Visible(targetBuffer)) {
        struct buffer *newBuffer;
        newBuffer = NULL;
        if (*lastBuffer != '\0')
            newBuffer = buffer_FindBufferByName(lastBuffer);
        if (newBuffer == targetBuffer)
            newBuffer = NULL;
        if (newBuffer == NULL)
            newBuffer = buffer_Enumerate(FindFirstBuffer, targetBuffer);
        if (newBuffer == NULL)
            newBuffer = buffer_Create("Scratch", NULL, NULL, NULL);
        if (newBuffer == NULL) {
            message_DisplayString(self, 0, "Buffer not deleted.");
            return;
        }
        *lastBuffer = '\0';
        ReplaceBuffer(targetBuffer, newBuffer);
    } else
        *lastBuffer = '\0';

#if 0
    im_ForceUpdate(); /* Clear out any update requests having pointers to things we are going to nuke. */
#endif /* 0 */

    buffer_Destroy(targetBuffer);
    message_DisplayString(self, 0, "Done.");
}

static struct buffer *LocalGetBufferOnFile(self, filename, flags)
struct frame *self;
char *filename;
int flags;
{
    struct buffer *buffer;
    char msgbuf[MAXPATHLEN + 100];

    im_SetProcessCursor(waitCursor);
    buffer = buffer_GetBufferOnFile(filename, flags);
    im_SetProcessCursor(NULL);

    if (buffer == NULL)
        switch (errno) {
            case 0:
                message_DisplayString(self, 0,
				      "Could not create correct data object.");
                break;
            case EACCES:
                message_DisplayString(self, 0,
		     "File not found; could not create. Permission denied.");
                break;
#ifdef ETIMEDOUT
            case ETIMEDOUT:
                message_DisplayString(self, 0,
		 "File not found; could not create. A file server is down.");
                break;
#endif /* ETIMEDOUT */
#ifdef EIO
            case EIO:
                message_DisplayString(self, 0,
	 "File not found; could not create. An I/O error occurred on the disk.");
                break;
#endif /* EIO */
/* This next case is somewhat based on internal knowledge of the buffer package... */
            case EISDIR:
                message_DisplayString(self, 0,
			"Attempt to visit directory without trailing slash.");
                break;
            default:
                sprintf(msgbuf, "File not found; could not create. %s.", UnixError(errno));
                message_DisplayString(self, 0, msgbuf);
                break;
        }


    return buffer;
}


/* 
 * The preserveBuffer parameter to LocalReadFile is a hack to satisfy 
 * the desired semantics of switch-file.  As with most elements of 
 * ATK, the frame package could be profitably replaced.  I find the 
 * current package's relation to the buffer class confusing.  There is 
 * little direct help given for common operations like reading and 
 * writing buffers.  Blah.
 * 
 * In the mean time, this is the solution that is minimally disruptive.
 * 
 * preserveBuffer should be a boolean value.  If true, LocalReadFile 
 * will make an effort (which is not guaranteed to succeed) to reuse 
 * the existing buffer.  If false, LocalReadFile is guaranteed to 
 * start with a new buffer.
 */

static int LocalReadFile(self, fname, preserveBuffer)
struct frame *self;
char *fname;
boolean preserveBuffer;
{
    struct buffer *buffer = frame_GetBuffer(self), *oldBuffer;
    char tempName[256];
    char filename[MAXPATHLEN];

    if (fname != NULL)
	strcpy(filename, fname);
    else if (buffer_GetFilename(buffer) == NULL)
	return -1;
    else
	strcpy(filename, buffer_GetFilename(buffer));

    /* don't use fname after this point, use filename. */

    im_SetProcessCursor(waitCursor);

/* The purpose of this next piece of code is to support dataobjects which may
 * have some loose persistient state across reads. Therefore if we are
 * re-reading the same file, we try to read it into the same object using
 * buffer_ReadFile. Otherwise we create a new buffer from scratch. An example
 * of this is the text object which maintains the cursor position across read
 * calls.
 */
    if (!preserveBuffer || buffer_ReadFile(buffer, filename) < 0) {
        if ((buffer = LocalGetBufferOnFile(self, filename, buffer_ForceNew)) ==
	    NULL) {
            im_SetProcessCursor(NULL);
            return -1;
        }

        oldBuffer = frame_GetBuffer(self);
        if (oldBuffer != NULL && oldBuffer != buffer) {
            ReplaceBuffer(oldBuffer, buffer);
            buffer_Destroy(oldBuffer);
            frame_SetBuffer(self, buffer, TRUE);
        }
    } else {
        buffer_SetName(buffer, "");
        buffer_GuessBufferName(filename, tempName, sizeof(tempName));
        buffer_SetName(buffer, tempName);
    }

    im_SetProcessCursor(NULL);

    if (buffer->readOnly)
        message_DisplayString(self, 0, "File is read only.");
    else if (access(buffer_GetFilename(buffer), F_OK) >= 0)
        message_DisplayString(self, 0, "Done.");
    else
        message_DisplayString(self, 0, "New file.");
    return 0;
}



/* Not static so it can be used from eza.c */
int frame_VisitFilePrompting(self, prompt, newWindow)
    struct frame *self;
    char *prompt;
    boolean newWindow;
{
    char filename[MAXPATHLEN];
    struct buffer *buffer;

    if ((buffer = frame_GetBuffer(self)) == NULL)
        return -1;

    bufferDirectory(buffer, filename);

    if (completion_GetFilename(self, prompt, filename, filename,
			       sizeof(filename), FALSE, FALSE) == -1) {
        errno = 0;
        return -1;
    }

    return VisitNamedFile(self, filename, newWindow);
}

int VisitNamedFile(self, filename, newWindow)
    struct frame *self;
    char *filename;
    boolean newWindow;
{
    struct buffer *buffer;
    if ((buffer = LocalGetBufferOnFile(self, filename, 0)) == NULL)
        return -1;

    if (frame_GetBuffer(self) != buffer)
        strncpy(lastBuffer, buffer_GetName(frame_GetBuffer(self)),
		sizeof(lastBuffer));

    if (newWindow) {
        struct frame *newFrame;
        struct im *window;

        if ((newFrame = frame_New()) != NULL)
            if ((window = im_Create(NULL)) != NULL) {
                frame_SetCommandEnable(newFrame, TRUE);
                im_SetView(window, newFrame);
                frame_PostDefaultHandler(newFrame, "message",
				 frame_WantHandler(newFrame, "message"));
                self = newFrame;
                message_DisplayString(self, 0, "");
            }
            else
                frame_Destroy(newFrame);
    }

    frame_SetBuffer(self, buffer, TRUE);
    return preventOutofSyncLossage(self, buffer);
}

/* like frame_VisitFilePrompting, but won't prompt if arg is non-NULL */
int frame_VisitAFile(self, arg, prompt, newWindow)
    struct frame *self;
    char *arg, *prompt;
    boolean newWindow;
{
    if (isString(arg))
	return VisitNamedFile(self, arg, newWindow);
    else
	return frame_VisitFilePrompting(self, prompt, newWindow);
}

int frame_VisitFile(self, arg)
    struct frame *self;
    char *arg;
{
    return frame_VisitAFile(self, arg, "Visit file: ",
				    im_ArgProvided(frame_GetIM(self)));
}

int frame_VisitFileNewWindow(self, arg)
    struct frame *self;
    char *arg;
{
    return frame_VisitAFile(self, arg, "Visit file: ", TRUE);
}

static int frame_SwitchFile(self, key)
struct frame *self;
long key;
{
    char bufferfile[MAXPATHLEN];
    char filename[MAXPATHLEN];
    struct buffer *buffer;

    if ((buffer = frame_GetBuffer(self)) == NULL)
        return -1;

    bufferDirectory(buffer, filename);
    bufferFilename(buffer, bufferfile);

    if (completion_GetFilename(self, "Switch File: ", filename, filename,
			       sizeof(filename), FALSE, FALSE) == -1)
        return -1;

    if (strcmp(bufferfile, filename) ?
	!preventBufferLossage(self, frame_GetBuffer(self)) :
	!preventReversionLossage(self, frame_GetBuffer(self))) {

        message_DisplayString(self, 0, "Aborted");
        return -1;
    }

    return LocalReadFile(self, filename, FALSE);
}

static int frame_ReadFile(self, key)
struct frame *self;
long key;
{
    char bufferfile[MAXPATHLEN];
    char filename[MAXPATHLEN];
    struct buffer *buffer;

    if ((buffer = frame_GetBuffer(self)) == NULL)
        return -1;


    bufferFilename(buffer, bufferfile);

    if (completion_GetFilename(self, "Read file: ", bufferfile, filename,
			       sizeof(filename), FALSE, FALSE) == -1)
        return -1;

    if (strcmp(bufferfile, filename) ?
	!preventBufferLossage(self, frame_GetBuffer(self)) :
	!preventReversionLossage(self, frame_GetBuffer(self))) {

        message_DisplayString(self, 0, "Aborted");
        return -1;
    }
    
    return LocalReadFile(self, filename, TRUE);
}


int frame_SaveFile(self, key)
struct frame *self;
long key;
{
    struct buffer *buffer = frame_GetBuffer(self);

    if (buffer == NULL)
        return -1;

    return saveBuffer(self, buffer, NULL);
}

int frame_WriteFile(self)
struct frame *self;
{
    char filename[MAXPATHLEN], tempName[256];
    struct buffer *buffer = frame_GetBuffer(self);

    if (buffer == NULL)
        return -1;

    bufferFilename(buffer, filename);

    if (completion_GetFilename(self, "Write to file: ", filename, filename,
			       sizeof(filename), FALSE, FALSE) == -1)
        return -1;

    if (saveBuffer(self, buffer, filename) >= 0) {
        buffer_SetFilename(buffer, filename); /* Sets checkpoint filename... */
        buffer_SetName(buffer, ""); /* So buffer_GuessBufferName does */
				    /* not return "foo-1" if our name */
				    /* is "foo" and new filename is */
				    /* ".../foo". */
        buffer_GuessBufferName(filename, tempName, sizeof(tempName));
        buffer_SetName(buffer, tempName);
        return 0;
    }
    else
        return -1;
}


void frame_SaveAll(self, key)
    struct frame *self;
    long key;
{
    (void)saveTheWorld(self);
}

char *frame_pwd(self, key)
    register struct frame *self;
    long key;
{

    char wd[MAXPATHLEN];
    char message[MAXPATHLEN + 30];

    im_GetDirectory(wd);
    sprintf(message, "Current directory is %s.", wd);
    message_DisplayString(self, 0, message);
    return wd;
}

boolean frame_cd (self, arg)
    struct frame *self;
    char *arg;
{

    char newdir[MAXPATHLEN];

    if (isString(arg)) 
	return (im_ChangeDirectory(arg) >= 0);

    im_GetDirectory(newdir);
    strcat(newdir, "/");

    if (completion_GetFilename(self, "Change directory to: ", newdir, newdir,
			       sizeof(newdir), TRUE, TRUE) == -1)
	return FALSE;

    if (im_ChangeDirectory(newdir) < 0) {
	message_DisplayString(self, 0, "Change directory failed.");
	return FALSE;
    }
    else {
	message_DisplayString(self, 0, "Done");
	return TRUE;
    }
}

void frame_PrintCmd(self)
    struct frame *self;
{

    struct buffer *buf;

    im_SetProcessCursor(waitCursor);
    message_DisplayString(self, 0, "Processing print request.");
    im_ForceUpdate();

    if (class_Load("print") == NULL) {
        message_DisplayString(self, 0,
			   "Print aborted; could not load class \"print\".");
        return;
    }
    buf = frame_GetBuffer(self);
    print_ProcessView(frame_GetView(self), 1, 1, buffer_GetFilename(buf), "");
    message_DisplayString(self, 0,
		     "Print request submitted; watch console for results.");
    im_SetProcessCursor(NULL);
}

void frame_PreviewCmd(self)
    struct frame *self;
{

    struct buffer *buf;

    im_SetProcessCursor(waitCursor);
    message_DisplayString(self, 0, "Processing preview request.");
    im_ForceUpdate();

    if (class_Load("print") == NULL) {
        message_DisplayString(self, 0,
			   "Print aborted; could not load class \"print\".");
        im_SetProcessCursor(NULL);
        return;
    }
    buf = frame_GetBuffer(self);
    print_ProcessView(frame_GetView(self), 0, 1, buffer_GetFilename(buf), "");
    message_DisplayString(self, 0, "Preview window should appear soon.");
    im_SetProcessCursor(NULL);
}

void frame_SetPrinter(self)
    struct frame *self;
{
    char *currentPrinter, *defaultPrinter, answer[256], prompt[sizeof("Current printer is . Set printer to []: ") + 128];

    currentPrinter = environ_Get("PRINTER");
    defaultPrinter = environ_GetProfile("print.spooldir");
    if (currentPrinter != NULL && defaultPrinter != NULL)
        sprintf(prompt, "Current printer is %.64s. Set printer to [%.64s]: ", currentPrinter, defaultPrinter);
    else if (defaultPrinter != NULL)
        sprintf(prompt, "Set printer to [%.64s]: ", defaultPrinter);
    else
        strcpy(prompt, "Set printer to: ");
    if (message_AskForString(self, 0, prompt, NULL, answer, sizeof(answer)) == -1)
        return;
    if (*answer != '\0') {
        environ_Put("PRINTER", answer);
        defaultPrinter = answer;
    }
    else
        environ_Delete("PRINTER");
    if (defaultPrinter != NULL) {
        sprintf(prompt, "Printer set to %.64s.", defaultPrinter);
        message_DisplayString(self, 0, prompt);
    }
    else
        message_DisplayString(self, 0, "Printer not set.");
}

/* Finds the frame immediately before the one passed in as the rock (nextFrame). */
int FindFrame(frame, nextFrame)
    struct frame *frame, *nextFrame;
{

    if (frame->next == nextFrame || frame->next == NULL)
        return TRUE;
    else
        return FALSE;
}

/* Trivially finds the first frame on the list... */
FirstFrame(frame, rock)
    struct frame *frame;
    long rock;
{

    return TRUE;
}

void frame_PreviousWindow(self)
    struct frame *self;
{

    struct frame *desiredFrame;

    desiredFrame = frame_Enumerate(FindFrame, self);
    if (desiredFrame != NULL)
        im_SetWMFocus(frame_GetIM( desiredFrame));
}

void frame_NextWindow(self)
    struct frame *self;
{

    struct frame *desiredFrame;

    if (self->next == NULL)
        desiredFrame = frame_Enumerate(FirstFrame, NULL);
    else
        desiredFrame = self->next;
    if (desiredFrame != NULL)
        im_SetWMFocus(frame_GetIM(desiredFrame));
}

void frame_HideWindow(self)
    struct frame *self;
{

    im_HideWindow(frame_GetIM(self));
    frame_NextWindow(self);
}

void frame_ExposeWindow(self)
    struct frame *self;
{

    im_ExposeWindow(frame_GetIM( self));
    im_SetWMFocus(frame_GetIM( self));
}

/* Can't use frame_HideWindow because it does input focus operations also. */
static boolean HideWindow(frame)
    struct frame *frame;
{

    im_HideWindow(frame_GetIM( frame));
    return FALSE;
}

void frame_SingleWindow(self)
    struct frame *self;
{

    frame_Enumerate(HideWindow, 0);
    frame_ExposeWindow(self);
}

void frame_SetBufferModified(self)
    struct frame *self;
{

    long version;
    struct buffer *thisBuffer = frame_GetBuffer(self);

    if (thisBuffer == NULL)
        return;

    version = dataobject_GetModified(buffer_GetData(thisBuffer));
    buffer_SetCkpClock(thisBuffer, 0);
    buffer_SetCkpVersion(thisBuffer, version);
    buffer_SetWriteVersion(thisBuffer, version);
    message_DisplayString(self, 0, "Reset buffer modified status.");
}

static struct bind_Description framecmdBindings[]={

    /* the functions called by these first few check their second argument.
	If it is a pointer, it is assumed to point to a character string
	and is used instead of prompting the user. */
    {"frame-change-directory",	    "\033c",0,	    NULL,0,0,	
		(void (*)())frame_cd, "Change current working directory."},
    {"frame-visit-file",	    "\030\026",0,   NULL,0,0,	
		(void (*)())frame_VisitFile, "Prompts for a file to visit."},
    {"frame-visit-file-new-window", NULL,0,   NULL,0,0,	
		(void (*)())frame_VisitFileNewWindow, 
		"Prompts for a file to visit. Creates a new window for it."},
    /* end of functions that check their second arg */

    {"frame-recursive-visit-file",  "\030\024",0,   NULL,0,0,	
		frame_RecursiveVisitFile, 
		"Visit file saving current buffer state."},
    {"frame-recursive-edit",	    NULL,0,	    NULL,0,0,	
		frame_RecursiveEdit, "Enter a recursive edit."},
    {"frame-exit-recursive-edit",   "\003",0,	    NULL,0,0,	
		frame_ExitRecursiveEdit, 
		"Exit after frame-recursive-edit or frame-recursive-visit-file"},
    {"frame-delete-window",	    "\030\004",0,   "Delete Window~89",0,0,	
		frame_DeleteWindow, 
		"Delete the window in which command is executed."},
    {"frame-exit",		    "\030\003",0,   "Quit~99",0,0, 
		frame_Exit, 
		"Exit editor.  If any buffer is modified ask for confirmation."},
    {"frame-current-directory",	    "\033p",0,	    NULL,0,0,	
		(void (*)())frame_pwd, "Show current working directory."},
    {"frame-new-window",	    "\0302",0,	    NULL,0,0,	
		frame_NewWindow, "Creates a new window."},
    {"frame-delete-buffer",	    "\030k",0,	    NULL,0,0,	
		frame_DeleteBuffer, "Prompts for a buffer to delete."},
    {"frame-old-buffer",	    "\030\017",0,   NULL,0,0,	
		frame_OldBuffer, "Visits an already existing buffer."},
    {"frame-visit-buffer",	    "\030b",0,	    NULL,0,0,	
		frame_VisitBuffer, "Changes to an arbitrarily named buffer."},
    {"frame-list-buffers",	    "\030\002",0,   NULL,0,0,	
		frame_ListBuffers, "Lists current buffers."},
    {"frame-switch-file",	    "\030\022",0,   "Switch File~30",0,0,
		(void (*)())frame_SwitchFile, 
		"Switches to editing another buffer."},
    {"frame-read-file",		    NULL,0,   NULL,0,0,	
		(void (*)())frame_ReadFile, 
		"Reads a file into the current buffer."},
    {"frame-save-file",		    "\030\023",0,   "Save~20",0,0, 
		(void (*)())frame_SaveFile, 
		"Saves buffer into its current file."},
    {"frame-write-file",	    "\030\027",0,   "File~10,Save As~1",0,0, 
		(void (*)())frame_WriteFile,
		"Prompts for a file to save the current buffer in."},
    {"frame-save-all-files",	    "\030\015",0,   "File~10,Save All~2",0,0,
		frame_SaveAll, "Saves all files."},
    {"frame-set-printer",	    NULL,0,	 "File~10,Set Printer~20",0,0,
		frame_SetPrinter, 
		"Set the printer for the print command to spool to."},
    {"frame-preview",		    NULL,0,	    "File~10,Preview~21",0,0,
		frame_PreviewCmd, "Previews document."},
    {"frame-print",		    NULL,0,	    "File~10,Print~22",0,0, 
		frame_PrintCmd, "Prints document."},
    {"frame-previous-window",	    NULL,0,	    NULL,0,0,	
		frame_PreviousWindow, 
		"Moves the input focus to the \"previous\" window."},
    {"frame-next-window",	    NULL,0,	    NULL,0,0,	
		frame_NextWindow, 
		"Moves the input focus to the \"next\" window."},
    {"frame-set-buffer-modified",   NULL,0,	    NULL,0,0,	
		frame_SetBufferModified, 
		"Sets buffer to be unmodified, unless given a prefix argument, in which case it sets the buffer to be modified."},
    {"frame-hide-window",	    NULL,0,	    NULL,0,0,	
		frame_HideWindow, 
		"Hides the window this command is executed in."},
    {"frame-expose-window",	    NULL,0,	    NULL,0,0,	  
		frame_ExposeWindow, 
		"Exposes the window this command is executed in."},
    {"frame-single-window",	    NULL,0,	    NULL,0,0,	  
		frame_SingleWindow, 
		"Hides all other windows and expands the current window."},
    NULL
};
struct proctable_DescriptionWithType typetable[] = {
	{"frame-visit-file", NULL, NULL, NULL, NULL, proctable_Long},
		/* returns 0 or greater for success */
	{"frame-visit-file-new-window", NULL, NULL, NULL, NULL, proctable_Long},
		/* returns 0 or greater for success */
	{"frame-change-directory", NULL, NULL, NULL, NULL, proctable_Boolean},
		/* returns TRUE for success */
	{"frame-current-directory", NULL, NULL, NULL, NULL, proctable_StaticString},
		/* returns a string with the current directory name */
	NULL
};

struct keymap *framecmds__InitKeymap(classID, menuOut)
struct classheader *classID;
struct menulist **menuOut;
{
    *menuOut = framecmdsMenus;
    return framecmdsKeymap;
}

boolean framecmds__InitializeClass(classID)
struct classheader *classID;
{
    struct classinfo *classInfo;

    framecmdsKeymap = keymap_New();
    framecmdsMenus = menulist_New();

    waitCursor = cursor_Create(NULL);
    cursor_SetStandard(waitCursor, Cursor_Wait);

    classInfo = class_Load("frame");
    bind_BindList(framecmdBindings, framecmdsKeymap, framecmdsMenus, classInfo);
    proctable_DefineProcsWithTypes(typetable);
    return TRUE;
}
