/*LIBS: -lframe -lbasics
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ez/RCS/eza.c,v 2.9 89/11/03 17:33:33 tpn Exp $ */
/* $ACIS:eza.c 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ez/RCS/eza.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ez/RCS/eza.c,v 2.9 89/11/03 17:33:33 tpn Exp $";
#endif /* lint */

#include <sys/errno.h>
#include <sys/param.h> /* For MAXPATHLEN. */
#include <andrewos.h> /* sys/types.h sys/file.h */

#include <andyenv.h>
#include <class.h>

#include <im.ih>
#include <graphic.ih>
#include <view.ih>
#include <frame.ih>
#include <dataobj.ih>
#include <buffer.ih>
#include <text.ih>
#include <message.ih>
#include <cursor.ih>
#include <event.ih>
#include <rect.h>
#include <init.ih>
#include <environ.ih>
#include <app.ih>
#include <proctbl.ih>

#include <eza.eh>

static struct cursor *waitCursor;

#define INITIALHELP "EZ version 2.12.\n\n"

extern int errno;

boolean ezapp__InitializeObject(classID,self)
struct classheader *classID;
struct ezapp *self;
{
    self->initFile=TRUE;
    self->files=NULL;
    self->fileLink= &self->files;
    self->haveBufferInWindow = FALSE;
    self->im = NULL;
    self->frame = NULL;
    self->buffer = NULL;
    self->errorBuffer = NULL;
    self->defaultObject = NULL;
    ezapp_SetMajorVersion(self, 7);
    ezapp_SetMinorVersion(self, 0);
    return TRUE;
}

/* The checkpointing algorithm used here is based on the following notions:
 *
 * 1) At Most, one buffer will be checkpointed every T seconds.
 * 2) At most, a given buffer will be checkpointed once in an (a * T) second
 *    interval.
 * 3) Only buffers which have been modified since the last time they were
 *    checkpointed (or written) are checkpointed.
 * 4) All buffers that meet criterion 3 will be checkpointed before any buffer is
 *    checkpointed twice.
 * 5) Scratch buffers are not checkpointed.
 *
 * The algorithm is implemented using two fields in the buffer structure. The
 * first is the ckpVersion field, which holds the dataobject version number as of
 * the last checkpoint (or file write). The second is the ckpClock field which is
 * reset on each checkpoint (or file write), and is incremented every T seconds.
 * To find which buffer to checkpoint, take the one with the highest ckpClock
 * greater than a. If there are none greater than a, don't checkpoint any.
 *
 * A third field in the buffer structure is used to allow the user to specify
 * the checkpointing behavior on a per buffer basis. This field is called the
 * latency and is basically the number of clock intervals above (or below if
 * negative) the system should wait before checkpointing this buffer. The code
 * is somewhat hairier than it might be since the buffer package initializes
 * both the ckpClock and ckpLatency  fields to 0 so we bias it by the value of
 * CkpLatency. This is the price paid for generality in the buffer package.
 *
 * Currently T (CkpInterval) is 30 seconds and a (CkpLatency) is 4 (i.e. 2
 * minutes checkpoint latency). However, a can be changed on a per
 * buffer basis.
 */

long CkpInterval; /* How often to run Checkpoint routine. */
#define DEFAULTCKPINTERVAL 30 /* Default for CkpInterval. */
long CkpLatency; /* The minimum amount of time to wait to checkpoint a buffer. */
#define DEFAULTCKPLATENCY 4 /* Default fo CkpLatency. */


static void StartupError(errorDoc, string)
    struct text *errorDoc;
    char *string;
{

    text_InsertCharacters(errorDoc, text_GetLength(errorDoc), string, strlen(string));
}

struct bestbuffer {
    struct buffer *buffer;
    long bufferclock;
};

boolean FindCkpBuffer(buffer, best)
    struct buffer *buffer;
    struct bestbuffer *best;
{

    if (!buffer_GetScratch(buffer) && dataobject_GetModified(buffer_GetData(buffer)) > buffer_GetCkpVersion(buffer)) {
        if (buffer_GetCkpClock(buffer) > best->bufferclock) {
            best->buffer = buffer;
            best->bufferclock = buffer_GetCkpClock(buffer);
        }
        buffer_SetCkpClock(buffer, buffer_GetCkpClock(buffer) + 1);
    }
    return FALSE;
}

#define view_Visible(view) (!rectangle_IsEmptyRect(&(((struct graphic *) view)->visualBounds)))

boolean CkpMessage(applicationView, targetView, inputFocusView, message)
    struct view *applicationView, *targetView, *inputFocusView;
    char *message;
{
    if (inputFocusView == NULL) /* Handles case where input focus is not set... */
        inputFocusView = targetView;

    if (view_Visible(inputFocusView)) {
        message_DisplayString(inputFocusView, 0, message);
        return TRUE;
    }
    return FALSE;
}

void Checkpoint(dummyData)
    long dummyData;
{
    struct bestbuffer result;

    result.buffer = NULL;
    result.bufferclock = CkpLatency - 1; /* (number + 1) * CKPINTERVAL seconds is how often a given buffer can be checkpointed. */

    buffer_Enumerate(FindCkpBuffer, &result);
    if (result.buffer != NULL) {

        int closeCode;

        im_SetProcessCursor(waitCursor);
        if (buffer_Visible(result.buffer))
            buffer_EnumerateViews(result.buffer, CkpMessage, "Checkpointing...");
        im_ForceUpdate();

        if ((closeCode = buffer_WriteToFile(result.buffer, buffer_GetCkpFilename(result.buffer), 0)) >= 0) {
            buffer_SetCkpVersion(result.buffer, dataobject_GetModified(buffer_GetData(result.buffer)));
            buffer_SetCkpClock(result.buffer, buffer_GetCkpLatency(result.buffer));
        }

        if (buffer_Visible(result.buffer))
            buffer_EnumerateViews(result.buffer, CkpMessage, closeCode ? "Checkpoint Failed." : "Checkpointed.");
        im_SetProcessCursor(NULL);
    }
    im_EnqueueEvent(Checkpoint, 0, event_SECtoTU(CkpInterval));
}

void SetBufferCkpLatency(frame, key)
    struct frame *frame;
    long key;
{

    struct buffer *buffer;
    char answer[20];

    if ((buffer = frame_GetBuffer(frame)) == NULL)
        return;

/* This stuff is a little hairy. Basically, it is converting a latency number to
 * seconds and back again. A latency number is shifted from 0, or specifically,
 * the code waits (latency + CkpLatency) * CkpInterval seconds before
 * considering checkpointing this buffer. 
 */
    sprintf(answer, "%d", (CkpLatency - buffer_GetCkpLatency(buffer)) * CkpInterval);
    if (message_AskForString(frame, 0, "Minimum checkpoint time in seconds: ", answer, answer, sizeof(answer)) != -1) {

        long latencyIntervals = atoi(answer) / CkpInterval;

        buffer_SetCkpLatency(buffer, CkpLatency - latencyIntervals);
        buffer_SetCkpClock(buffer, CkpLatency - latencyIntervals); /* Force new checkpoint time to take effect now. */
    }
}

void Startup(frame)
    struct frame *frame;
{
    struct buffer *buffer;
    long count = 0;
    int cc;
/* Loop until we have a file in a buffer. */
    while (frame_VisitFilePrompting(frame, "Please enter the name of an initial file: ", FALSE) < 0) {

        boolean first = TRUE;

/* This next code handles a mouse (or menu) hit.
 * We must process it to get rid of it, and then make the view
 * give up the input focus it may have aquired. The loop is
 * so the error message doesn't time out and get erased.
 */
        do {

            if (!first) {
	        im_Interact(FALSE);
                view_LoseInputFocus(frame_GetView(frame));
            }
            else
                first = FALSE;

            switch (errno) {
                case EACCES:
                    message_DisplayString(frame, 0, "File not found; could not create. Permission denied. Press any key to continue.");
                    break;
#ifdef AFS_ENV
                case ETIMEDOUT:
                    message_DisplayString(frame, 0, "File not found; could not create. A server is down. Press any key to continue. ");
                    break;
#endif /* AFS_ENV */
                case 0: /* Message line was punted. */
                    message_DisplayString(frame, 0, "You must specify a file. Press any key to continue. ");
                    break;
                default:
                    message_DisplayString(frame, 0, "File not found; could not create. Press any key to continue.");
            }
	    if(count++ > 10) /* the user probably quit */ exit(0);
        } while ((cc = im_GetCharacter(view_GetIM((struct view *) frame))) == EOF);
	if(cc == 3) exit(0);
	count = 0;
    }
    buffer = frame_GetBuffer(frame);
    if (buffer->readOnly)
        message_DisplayString(frame, 0, "File is read only.");
    else if (access(buffer_GetFilename(buffer), F_OK) >= 0)
        message_DisplayString(frame, 0, "Done.");
    else
        message_DisplayString(frame, 0, "New file.");
}

static void addFile(self,name,newWin,ro)
struct ezapp *self;
char *name;
boolean newWin,ro;
{
    /* Its a file right? */
    struct ezapp_fileList *fileEntry=
      (struct ezapp_fileList *) malloc(sizeof(struct ezapp_fileList));

    fileEntry->filename=name;
    fileEntry->ObjectName=self->defaultObject;
    fileEntry->newWindow=newWin;
    fileEntry->readOnly=ro;
    fileEntry->next=NULL;
    *self->fileLink=fileEntry;
    self->fileLink=(&(fileEntry->next));
}

boolean ezapp__ParseArgs(self,argc,argv)
struct ezapp *self;
int argc;
char **argv;
{
    int maxInitWindows=environ_GetProfileInt("MaxInitWindows", 2);
    boolean useNewWindow = FALSE;
    boolean pendingReadOnly = FALSE;
    boolean pendingObject=FALSE;
    char *name=ezapp_GetName(self);

    if(!super_ParseArgs(self,argc,argv))
	return FALSE;

    if(name!=NULL && strcmp(name,"ez")!=0 && class_IsTypeByName(name,"dataobject"))
	self->defaultObject=name;

    if (maxInitWindows < 2)
	maxInitWindows = 1;

    while(*++argv!=NULL){
	if(**argv=='-')
	    switch((*argv)[1]){
		case 'o': /* Create a buffer on object */
		    pendingObject=TRUE;
		    break;
		case 'r': /* Next file should be readonly. */
		    pendingReadOnly = TRUE;
		    break;
		case 'w': /* New window. */
		    useNewWindow = TRUE;
		    break;
		default:
		    fprintf(stderr,"%s: unrecognized switch: %s\n", ezapp_GetName(self), *argv);
		    return FALSE;
	    }
	else{
	    if(pendingObject){
		self->defaultObject= *argv;
		pendingObject=FALSE;
	    }else{
		addFile(self,
			*argv,
			(useNewWindow || maxInitWindows-->0),
			pendingReadOnly);
		useNewWindow=FALSE;
		pendingReadOnly=FALSE;
	    }
	}
    }

    return TRUE;
}

static void makeErrorBuf(self)
struct ezapp *self;
{
    self->errorBuffer = buffer_Create("Startup-Errors", NULL, "text", NULL);
    buffer_SetScratch(self->errorBuffer, TRUE);
}

void ezapp__ReadInitFile(self)
struct ezapp *self;
{
    makeErrorBuf(self);

    ezapp_SetErrorProc(self,StartupError);
    ezapp_SetErrorRock(self,(pointer)buffer_GetData(self->errorBuffer));

    super_ReadInitFile(self);
}

boolean ezapp__Start(self)
struct ezapp *self;
{
    struct ezapp_fileList *fileEntry, *next;
    struct text *errtext;

    if(!super_Start(self))
	return FALSE;

    if(self->errorBuffer==NULL) /* ? */
	makeErrorBuf(self);

    errtext=(struct text *)buffer_GetData(self->errorBuffer);

    buffer_SetDefaultObject(self->defaultObject);

    if(self->files==NULL)  {
	char *defFile;

	if ((defFile = environ_GetProfile("DefaultStartUpFile")) != NULL && *defFile != '\0')  {
	    addFile(self, defFile, TRUE, FALSE);
	}
    }

    for (fileEntry = self->files; fileEntry != NULL; fileEntry = next) {
        if(fileEntry->ObjectName != NULL){
            if(class_IsTypeByName(fileEntry->ObjectName,"dataobject"))
                buffer_SetDefaultObject(fileEntry->ObjectName);
            else {
                char errorMessage[200];
                sprintf(errorMessage,"%s is not a known dataobject\n",fileEntry->ObjectName);
                StartupError(errtext,errorMessage);
                self->buffer = NULL;
                next = fileEntry->next;
                free(fileEntry);
                continue;
            }
        }
        self->buffer = buffer_GetBufferOnFile(fileEntry->filename, fileEntry->readOnly ? buffer_ReadOnly : 0);

        if(fileEntry->ObjectName != NULL)
            buffer_SetDefaultObject(self->defaultObject);

        if (self->buffer != NULL) {
            long version = dataobject_GetModified(buffer_GetData(self->buffer));

            buffer_SetCkpClock(self->buffer, 0);
            buffer_SetCkpVersion(self->buffer, version);
            buffer_SetWriteVersion(self->buffer, version);
            if (fileEntry->newWindow) {
                self->frame = frame_New();
                frame_SetCommandEnable(self->frame, TRUE);
                self->im = im_Create(NULL);
                im_SetView(self->im, self->frame);
                frame_PostDefaultHandler(self->frame, "message", frame_WantHandler(self->frame, "message"));
                frame_SetBuffer(self->frame, self->buffer, TRUE);
                self->haveBufferInWindow = TRUE;
            }
        }
        else {
            char errorMessage[200];
            sprintf(errorMessage, "File %s does not exist and could not be created.", fileEntry->filename);
            StartupError(errtext,errorMessage);
        }
        next = fileEntry->next;
        free(fileEntry);
    }

    if (text_GetLength(errtext) != 0) {
        text_InsertCharacters(errtext, 0, INITIALHELP, sizeof(INITIALHELP) - 1);
        text_InsertCharacters(errtext, sizeof(INITIALHELP) - 1, "Errors encountered during startup:\n", sizeof("Errors encountered during startup:\n") - 1);
    }
    else {
        buffer_Destroy(self->errorBuffer);
        self->errorBuffer = NULL;
    }

    if (self->frame == NULL || self->errorBuffer != NULL)  {
	/*
	    Dont have a window yet, or need one for error buffer.
	*/

	self->frame = frame_New();
	frame_SetCommandEnable(self->frame, TRUE);
        if (self->haveBufferInWindow)
            frame_SetTitle(self->frame, "Startup Errors.");
        else
            frame_SetTitle(self->frame, "No files specified."); /* This will get reset below. */
	self->im = im_Create(NULL);
	im_SetView(self->im, self->frame);
	frame_PostDefaultHandler(self->frame, "message", frame_WantHandler(self->frame, "message"));
    }

    if (self->errorBuffer)
	frame_SetBuffer(self->frame, self->errorBuffer,	TRUE);
    waitCursor = cursor_Create(NULL);
    cursor_SetStandard(waitCursor, Cursor_Wait);
    return TRUE;
}

int ezapp__Run(self)
struct ezapp *self;
{
 
    if(!ezapp_Fork(self))
	return -1;

    if (! self->haveBufferInWindow)  {
        struct text *text;
        struct view *inputFocus;

        if (self->errorBuffer == NULL) {
            self->buffer = buffer_Create("", NULL, "text", NULL);
            text = (struct text *) buffer_GetData(self->buffer);
            frame_SetBuffer(self->frame, self->buffer, TRUE);
            text_InsertCharacters(text, 0, INITIALHELP, sizeof(INITIALHELP) - 1);
        }
        inputFocus = im_GetInputFocus(self->im);
        view_LoseInputFocus(inputFocus);
        Startup(self->frame);
        if (self->errorBuffer == NULL) /* If we had to create a buffer to hold the error text. */
            buffer_Destroy(self->buffer);
        frame_SetTitle(self->frame, NULL);
    }

/* A CheckPointInterval of 0 means don't checkpoint. */
    if ((CkpInterval = environ_GetProfileInt("CheckpointInterval", DEFAULTCKPINTERVAL)) != 0) {
        CkpLatency = environ_GetProfileInt("CheckpointMinimum", DEFAULTCKPLATENCY * CkpInterval) / CkpInterval;
        im_EnqueueEvent(Checkpoint, 0, event_SECtoTU(CkpInterval));
    }

    im_KeyboardProcessor();

    return 0;
}

boolean ezapp__InitializeClass()
{

    proctable_DefineProc("ezapp-set-buffer-checkpoint-latency", SetBufferCkpLatency, class_Load("frame"), NULL, "Set the number of checkpoint intervals to wait before checkpointing the current buffer.");

    return TRUE;
}
