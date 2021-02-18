/*LIBS: -lbasics
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/compile.c,v 2.7 89/04/12 05:32:51 pgc Exp $ */
/* $ACIS:compile.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/compile.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/compile.c,v 2.7 89/04/12 05:32:51 pgc Exp $";
#endif /* lint */

/* compile.c
 * compilation package for be2 based editor.
 */

#include <class.h>
#include <andrewos.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>

#include <view.ih>
#include <dataobj.ih>
#include <im.ih>
#include <message.ih>
#include <text.ih>
#include <textv.ih>
#include <frame.ih>
#include <buffer.ih>
#include <proctbl.ih>
#include <search.ih>
#include <filetype.ih>
#include <style.ih>
#include <rect.h>
#include <mark.ih>
#include <envrment.ih>
#include <environ.ih>
#include <stylesht.ih>

#include <compile.eh>

#ifdef hpux
#define killpg(id,sig) kill(-(id),sig)
#endif /* hpux */

static struct style *boldStyle = NULL;

#define Text(thisView) ((struct text *) thisView->header.view.dataobject)

struct lengthPair {
    long first, second;
};

boolean SetDotToEnd(applicationView, targetView, inputFocus, lengths)
    struct view *applicationView, *targetView, *inputFocus;
    struct lengthPair *lengths;
{

    if (class_IsType(targetView, class_Load("textview"))) {

        struct textview *thisView = (struct textview *) targetView;

        if (textview_GetDotPosition(thisView) == lengths->first) {
            textview_SetDotPosition(thisView, lengths->second);
            textview_FrameDot(thisView, lengths->second);
        }
    }
    return FALSE; /* Keep on enumerating. */
}

static void InsertMessage(buffer, pos, string, len)
    struct buffer *buffer;
    long pos;
    char *string;
    long len;
{

    struct environment *tempEnv;
    struct text *doc = (struct text *) buffer_GetData(buffer);
    struct lengthPair lengths;

    text_InsertCharacters(doc, pos, string, len);
    text_InsertCharacters(doc, pos + len, "\n", 1);
    if (boldStyle != NULL) {
        tempEnv = text_AddStyle(doc, pos, len + 1, boldStyle);
        environment_SetStyle(tempEnv, FALSE, FALSE);
    }
    lengths.first = pos;
    lengths.second = pos + len + 1;
    buffer_EnumerateViews(buffer, SetDotToEnd, &lengths);
    text_NotifyObservers(doc, 0);
}

/* Process handling stuff. */

#define READFD 0	/* The pipe index from which you can do a read. */
#define WRITEFD 1	/* The pipe index to which you can do a write. */

struct process {
    FILE *inFile, *outFile;
    short pid;
    boolean stopped;
};

struct processbuffer {
    struct process *process;
    struct buffer *buffer;
};

static struct process *StartProcess(command, inputFile, outputFile)
    char *command;
    FILE **inputFile, **outputFile;
{

    int inpipe[2], outpipe[2], pid;
    struct process *thisProc;

/* Create pipe for parent reading from child. */
    if (inputFile != NULL) {
        if (pipe(inpipe) < 0)
            return 0;
        *inputFile = fdopen(inpipe[READFD], "r");
        fcntl(inpipe[READFD], F_SETFD, 1); /* Set close on exec flag. */
    }
    else
        if ((inpipe[WRITEFD] = open("/dev/null", O_WRONLY) < 0))
            return 0;

/* Create pipe for parent writing to child. */
    if (outputFile != NULL) {
        if (pipe(outpipe) < 0)
            return 0;
        *outputFile = fdopen(outpipe[WRITEFD], "w");
        fcntl(outpipe[WRITEFD], F_SETFD, 1); /* Set close on exec flag. */
    }
    else
        if ((outpipe[READFD] = open("/dev/null", O_RDONLY)) < 0)
            return 0;

    if ((pid = osi_vfork()) == 0) { /* In child */

        int numfds = getdtablesize();
        int fd;

        dup2(outpipe[READFD], 0);
        dup2(inpipe[WRITEFD], 1);
        dup2(1, 2); /* Should have more control over this. */

        /* Don't leave any open file descriptors for child. */
        for (fd = 3; fd < numfds; fd++)
            close(fd);

#ifdef hpux
	setpgrp();
#else /* hpux */
        setpgrp(0, getpid());
#endif /* hpux */
        execlp("/bin/csh", "csh", "-cf", command, (char *)NULL);
        return NULL;
    }

    close(outpipe[READFD]);
    close(inpipe[WRITEFD]);

    thisProc = (struct process *) malloc(sizeof(struct process));
    if (inputFile != NULL)
        thisProc->inFile = *inputFile;
    else
        thisProc->inFile = NULL;
    if (outputFile != NULL)
        thisProc->outFile = *outputFile;
    else
        thisProc->outFile = NULL;
    thisProc->pid = pid;
    thisProc->stopped = FALSE;
    return thisProc;
}

/* Kills process with terminate signal. Trys to cleanup everything. */
static int FinishProcess(process)
    struct process *process;
{

    if (process->pid > 0) /* If we have a process running. */
        killpg(process->pid, SIGTERM);
#ifdef SIGCONT	    /* SIGCONT not currently supported on hp9000s300 */
    if (process->stopped) /* I think this works. */
        killpg(process->pid, SIGCONT);
#endif /* SIGCONT	     */
    if (process->inFile != NULL)
        fclose(process->inFile);
    if (process->outFile != NULL)
        fclose(process->outFile);
    free(process);
    return 0;
}

/* Process and comman global state. */
static struct process *currentProcess = NULL; /* pid of compilation process, is 0 if none currently executing. */
static char *defaultCommand;
static char *compileCommand;

/* Starts a process dumping output into a buffer. */
static struct buffer *MakeCommandBuffer(command, buffername, handler)
    char *command, *buffername;
    int (*handler)();
{

    struct text *commandLogDoc;
    struct buffer *commandLog;
    FILE *inputFile;

    if ((commandLog = buffer_FindBufferByName(buffername)) == NULL) {
        commandLog = buffer_Create(buffername, NULL, "text", NULL);
        buffer_SetScratch(commandLog, TRUE);
        commandLogDoc = (struct text *) buffer_GetData(commandLog);
    }
    else {
        commandLogDoc = (struct text *) buffer_GetData(commandLog);
        text_Clear(commandLogDoc);
        text_NotifyObservers(commandLogDoc, 0);
    }

    if (text_ReadTemplate(commandLogDoc, "compile", FALSE) < 0)
        boldStyle = NULL;
    else
        boldStyle = stylesheet_Find(commandLogDoc->styleSheet, "bold");

    if ((currentProcess = StartProcess(command, &inputFile, NULL)) != 0) {
        
        struct processbuffer *processBuffer = (struct processbuffer *) malloc(sizeof(struct processbuffer));

        processBuffer->buffer = commandLog;
        processBuffer->process = currentProcess;
        im_AddFileHandler(inputFile, handler, processBuffer, 3); /* Priority chosen at random! Should be checked and replaced. */
        return commandLog;
    }
    else
        return NULL;
}

static void compile_BuildHandler(inputFile, processBuffer)
    FILE *inputFile;
    struct processbuffer *processBuffer;
{

    char buffer[BUFSIZ];
    struct text *logDoc = (struct text *) buffer_GetData(processBuffer->buffer);

    if (fgets(buffer, sizeof(buffer), inputFile)) {

        long length = text_GetLength(logDoc), len;
        struct lengthPair lengths;
        
        text_InsertCharacters(logDoc, length, buffer, len = strlen(buffer));
        lengths.first = length;
        lengths.second = length + len;
        buffer_EnumerateViews(processBuffer->buffer, SetDotToEnd, &lengths);
        text_NotifyObservers(logDoc, 0);
    }
    else {
        if (FinishProcess(currentProcess) != -1) {
            currentProcess = NULL;
            InsertMessage(processBuffer->buffer, text_GetLength(logDoc), "End", sizeof("End") - 1);
        }
        free(processBuffer);
        im_RemoveFileHandler(inputFile);
    }
}

static void compile_KillBuild(view, key)
    struct view *view;
    long key;
{

    if (currentProcess != NULL) {
        if (im_ArgProvided(view_GetIM((struct view *) view))) {
            if (currentProcess->stopped)
                message_DisplayString(view, 0, "Build is already stopped.");
            else {
#ifdef SIGSTOP	 /* not currently supported on HP9000S300 */
                currentProcess->stopped = TRUE;
                killpg(currentProcess->pid, SIGSTOP);
                message_DisplayString(view, 0, "Build stopped.");
#else /* SIGSTOP	  */
		message_DisplayString(view, 0, "Build not stopped (job control not supported).");
#endif /* SIGSTOP	  */
            }
            return;
        }
        else {

            struct buffer *logBuffer = buffer_FindBufferByName("Error-Log");
            struct text *logDoc = (struct text *) buffer_GetData(logBuffer);

            im_RemoveFileHandler(currentProcess->inFile);
            FinishProcess(currentProcess);
            InsertMessage(logBuffer, text_GetLength(logDoc), "Killed!", sizeof("Killed!") - 1);
            currentProcess = NULL;
            message_DisplayString(view, 0, "Dead!");
        }
    }
    else
        message_DisplayString(view, 0, "You don't have a subprocess to kill (or stop).");
}

static void compile_SetCommand(command)
    char *command;
{

    if (compileCommand != defaultCommand)
        free(compileCommand);
    if (command == NULL) {
        compileCommand = defaultCommand;
        return;
    }
    compileCommand = (char *) malloc(strlen(command) + 1);
    strcpy(compileCommand, command);
}

static boolean SaveModifiedBuffer(buffer, messageView)
    struct buffer *buffer;
    struct view *messageView;
{

    if ((buffer_GetWriteVersion(buffer) < dataobject_GetModified(buffer_GetData(buffer))) && (buffer_GetFilename(buffer) != NULL))
        while (TRUE)
            if (buffer_WriteToFile(buffer, NULL, buffer_ReliableWrite) < 0) { /* Save to buffer filename */

                char message[150], response[2];

                sprintf(message, "Couldn't save \"%.100s\", Try again, Quit, Ignore [t]? ", buffer_GetFilename(buffer));
                if ((message_AskForString(messageView, 0, message, NULL, response, sizeof(response)) == -1) || *response == 'q')
                    return TRUE; /* Stop enumerating here... */
                else if (*response == 'i')
                    return FALSE; /* Oh well, continue anyway... */
            }
            else {

                long version = dataobject_GetModified(buffer_GetData(buffer));

                unlink(buffer_GetCkpFilename(buffer));
                buffer_SetCkpClock(buffer, 0);
                buffer_SetCkpVersion(buffer, version);
                buffer_SetWriteVersion(buffer, version);
                return FALSE; /* Keep on enumerating. */
            }
    return FALSE;
}

/* Save all modified buffers giving appropriate user feedback... */
static boolean SaveAllBuffers(view)
    struct view *view;
{

    return (buffer_Enumerate(SaveModifiedBuffer, view) == NULL);
}

static void compile_Build(view, key)
    struct view *view;
    long key;
{

    char buffer[BUFSIZ];
    int askForCommand;
    struct buffer *commandBuffer;
    struct view *PutInAnotherWindow();

    askForCommand = im_ArgProvided(view_GetIM(view));
    im_ClearArg(view_GetIM(view));

    if (currentProcess != NULL) {

        int needAnswer;

        if (currentProcess->stopped) {
            currentProcess->stopped = FALSE;
#ifdef SIGCONT
            killpg(currentProcess->pid, SIGCONT);
#endif /* SIGCONT */
            message_DisplayString(view, 0, "Build continuing");
            return;
        }

        do {
            needAnswer = 0;
            if (message_AskForString(view, 0, "You already have a compilation running, kill it [n]? ", NULL, buffer, 2) < 0)
                return;
            if (*buffer == '\0')
                *buffer = 'n';
            switch (*buffer) {
                case 'n':
                case 'N':
                    return;
                    /* break; */
                case 'y':
                case 'Y':
                    compile_KillBuild(view, key);
                    break;
                default:
                    message_DisplayString(view, 0, "You must answer 'y' or 'n'. (Press any key to continue).");
                    im_GetCharacter(view_GetIM(view));
                    needAnswer = 1;
                    break;
            }
        } while (needAnswer);
    }
    resetErrors();
    if (!SaveAllBuffers(view))
        return;

    if (askForCommand) {
        if (message_AskForString(view, 0, "Compile command: ", compileCommand, buffer, sizeof(buffer)) != -1)
            compile_SetCommand(buffer);
        else
            return;
    }
    commandBuffer = MakeCommandBuffer(compileCommand, "Error-Log", (int (*)()) compile_BuildHandler);
    if (view->dataobject != buffer_GetData(commandBuffer)) /* If not already looking at the error log. */
        PutInAnotherWindow(view, commandBuffer, FALSE);
    InsertMessage(commandBuffer, 0, compileCommand, strlen(compileCommand));
    message_DisplayString(view, 0, "Started compile, output is in buffer \"Error-Log\".");
}

/* Error parsing stuff. */

/* Convert an entire doc full of error messages into an error list. */

/* Data structure for holding the list of errors. */
struct errorList {
    struct mark *messageMark;
    struct mark *offendingCodeMark;
    struct errorList *next;
    struct errorList *previous;
};

/* Globals for maintaining error state. Probably shouldn't be so singular. */
static int errorPos = 0; /* Position of last error parsed. */
static char currentDirectory[400] = ".";
static struct errorList *allErrors = NULL, *currentError = NULL, *lastParsed = NULL, **lastParsedLink = &allErrors;

/* Remove all pending errors from the error list. */
static resetErrors()
{

    struct errorList *tempList;

    errorPos = 0;
    while (allErrors != NULL) {
        tempList = allErrors->next;
        free(allErrors);
        allErrors = tempList;
    }
    currentError = NULL;
    im_GetDirectory(currentDirectory);
    lastParsed = NULL;
    lastParsedLink = &allErrors;
}

static int getlinepos(doc, line)
    register struct text *doc;
    int line;
{

    register int pos, len, i = 1;

    len = text_GetLength(doc);
    for (pos = 0; (i < line) && (pos < len); pos++)
	if (text_GetChar(doc, pos) == '\n')
	    ++i;
    return pos;
}

static long nextlinepos(doc, pos)
    register struct text *doc;
    register long pos;
{

    register int tempChar;

    while ((tempChar = text_GetChar(doc, pos)) != '\n' && tempChar != EOF)
        pos++;
    if (tempChar == '\n')
        pos++;
    return pos;
}

 /* "foo.c".*line.*123:... */
static int ParseCCError(doc, startPos, fileName, maxSize)
    struct text *doc;
    long *startPos;
    char *fileName;
    int maxSize;
{

    long docPos = *startPos, currentChar;
    char lineNumberBuffer[32], *lineNumberChars = lineNumberBuffer, *origFilename = fileName;

    if (text_GetChar(doc, docPos) == '"') {
        while (((currentChar = text_GetChar(doc, ++docPos)) != '"') && (currentChar != '\n') &&
               (currentChar != EOF) && (fileName - origFilename < maxSize))
            *fileName++ = currentChar;
        *fileName = '\0';
        if (currentChar == '"') {
            while (((currentChar = text_GetChar(doc, ++docPos)) != 'l') && (currentChar != '\n') &&
                   (currentChar != EOF))
                ;
            if (text_GetChar(doc, docPos) == 'l' && text_GetChar(doc, ++docPos) == 'i' &&
                text_GetChar(doc, ++docPos) == 'n' && text_GetChar(doc, ++docPos) == 'e') {

                while (text_GetChar(doc, ++docPos) == ' ')
                    ;
                while (isdigit(*lineNumberChars = text_GetChar(doc, docPos))) {
                    lineNumberChars++;
                    docPos++;
                }
                *lineNumberChars = '\0';
                *startPos = nextlinepos(doc, docPos);
                return atoi(lineNumberBuffer);
            }
        }
        return -1; /* I don't know what this is... */
    }
    return -1;
}

/* foo.c:123:... */
static int ParseEgrepError(doc, startPos, fileName, maxSize)
    struct text *doc;
    long *startPos;
    char *fileName;
    int maxSize;
{

    long docPos = *startPos, currentChar;
    char lineNumberBuffer[32], *lineNumberChars = lineNumberBuffer, *origFilename = fileName;

    while (((currentChar = text_GetChar(doc, docPos++)) != ':') && (currentChar != ' ') &&
           (currentChar != '\n') && (currentChar != EOF) && (fileName - origFilename < maxSize))
        *fileName++ = currentChar;
    *fileName = '\0';
    if (currentChar == ':') {
        while (!isdigit(currentChar = text_GetChar(doc, docPos)) && (currentChar != '\n') &&
               (currentChar != EOF))
            docPos++;
        if (!isdigit(currentChar))
            return -1;
        while (isdigit(*lineNumberChars = text_GetChar(doc, docPos))) {
            lineNumberChars++;
            docPos++;
        }
        *lineNumberChars = '\0';
        *startPos = nextlinepos(doc, docPos);
        return atoi(lineNumberBuffer);
    }
    return -1;
}

/* <'E' or 'w'> "foo.c", L<line>,<character>... */
static int ParseHCError(doc, startPos, fileName, maxSize)
    struct text *doc;
    long *startPos;
    char *fileName;
    int maxSize;
{

    long docPos = *startPos, currentChar;
    char lineNumberBuffer[32], *lineNumberChars = lineNumberBuffer, *origFilename = fileName;

    if (((currentChar = text_GetChar(doc, docPos++)) == 'E' ||
        currentChar == 'w') && text_GetChar(doc, docPos++) == ' ' &&
        text_GetChar(doc, docPos++) == '"') {

        while ((currentChar = text_GetChar(doc, docPos++)) != '\n' &&
               currentChar != '"' && (fileName - origFilename < maxSize))
            *fileName++ = currentChar;
        *fileName = '\0';
        if (currentChar != '"' || text_GetChar(doc, docPos++) != ',' ||
	    text_GetChar(doc, docPos++) != 'L')
	    return -1;
        while (isdigit(*lineNumberChars = text_GetChar(doc, docPos++)))
            lineNumberChars++;
	*lineNumberChars = '\0';
        do {
            docPos = nextlinepos(doc, docPos);
        } while (text_GetChar(doc, docPos) == '|'); /* While we have a continuation lines. */
        *startPos = docPos;
        return atoi(lineNumberBuffer);
    }
    return -1;
}

static int ParseCD(doc, startPos, directory, maxSize)
    struct text *doc;
    long *startPos;
    char *directory;
    int maxSize;
{

    char *origDirectory = directory;
    long docPos = *startPos, currentChar;

    if ((text_GetChar(doc, docPos++) == 'c') && (text_GetChar(doc, docPos++) == 'd')) { /* cd directory */
        while ((currentChar = text_GetChar(doc, docPos)) == ' ')
            docPos++;
        while ((currentChar = text_GetChar(doc, docPos)) != ' ' && currentChar != '\n' && (currentChar != EOF) && (directory - origDirectory < maxSize)) {
            *directory++ = currentChar;
            docPos++;
        }
        *directory = '\0';
        *startPos = nextlinepos(doc, docPos);
        return 0;
    }
    return -1;
}

/* Parse input file text into line number and filename (or cd command). */
static int ParseEntry(buffer, doc, startPos, maxSize)
    char *buffer;
    struct text *doc;
    long *startPos;
    int maxSize;
{

    int line;

    if ((line = ParseCCError(doc, startPos, buffer, maxSize)) > -1)
        return line;
    else if ((line = ParseEgrepError(doc, startPos, buffer, maxSize)) > -1)
        return line;
    else if ((line = ParseHCError(doc, startPos, buffer, maxSize)) > -1)
        return line;
    else if ((line = ParseCD(doc, startPos, buffer, maxSize)) > -1)
        return line;
    else {
        *startPos = nextlinepos(doc, *startPos);
        return -1;
    }
}


static struct errorList *MakeErrorList(errorBuffer)
    struct buffer *errorBuffer;
{

    long searchPos, startPos, lineNumber, lastLine = -1;
    char filenameBuffer[100], realFilename[200], lastFilename[200];
    struct text *errorDoc = (struct text *) buffer_GetData(errorBuffer);
    struct buffer *buffer;
    struct errorList *firstError = NULL;

    lastFilename[0] = '\0'; /* lastFileName should be part of state. */
    searchPos = errorPos;
    while (searchPos < text_GetLength(errorDoc)) {
        startPos = searchPos; /* For making mark. */
        lineNumber = ParseEntry(filenameBuffer, errorDoc, &searchPos, sizeof(filenameBuffer) - 1);
        if (lineNumber > 0) {
            if (*filenameBuffer != '/') {
                strcpy(realFilename, currentDirectory);
                strcat(realFilename, "/");
            }
            else
                *realFilename = '\0';
            strcat(realFilename, filenameBuffer);
            if (strcmp(realFilename, lastFilename) || lineNumber != lastLine) {
                buffer = (struct buffer *) buffer_GetBufferOnFile(realFilename, buffer_MustExist);
                if (buffer != NULL) {

                    int tempPos;
                    struct errorList *thisError = (struct errorList *) malloc(sizeof(struct errorList));

                    thisError->messageMark = text_CreateMark(errorDoc, startPos, searchPos - startPos);
                    mark_SetStyle(thisError->messageMark, FALSE, FALSE);
                    tempPos = getlinepos((struct text *) buffer_GetData(buffer), lineNumber);
                    thisError->offendingCodeMark = text_CreateMark((struct text *) buffer_GetData(buffer),
                                                                   tempPos,
                                                                   nextlinepos((struct text *) buffer_GetData(buffer), tempPos) - tempPos);
                    thisError->previous = lastParsed;
                    thisError->next = NULL;
                    lastParsed = thisError;
                    *lastParsedLink = thisError;
                    lastParsedLink = &(thisError->next);

                    if (firstError == NULL)
                        firstError = thisError;

                    strcpy(lastFilename, realFilename);
                    lastLine = lineNumber;
                }
                else {

                    char errorMessageBuffer[100];

                    sprintf(errorMessageBuffer, "Couldn't find file: %s", filenameBuffer);
                    InsertMessage(errorBuffer, searchPos, errorMessageBuffer, strlen(errorMessageBuffer));
                    searchPos = nextlinepos(errorDoc, searchPos);
                }
            }
            else {
                mark_SetLength(lastParsed->messageMark, searchPos - mark_GetPos(lastParsed->messageMark));
            }
        }
        else if (lineNumber == 0) {

            char tempBuffer[400];

            if (*filenameBuffer != '/') {
                strcat(currentDirectory, "/");
                strcat(currentDirectory, filenameBuffer);
                filetype_CanonicalizeFilename(tempBuffer, currentDirectory, sizeof(tempBuffer));
                strcpy(currentDirectory, tempBuffer);
                if (*currentDirectory == '\0') { /* Keeps appending a slash from starting at root. */
                    currentDirectory[0] = '.';
                    currentDirectory[1] = '\0';
                }
            }
            else
                filetype_CanonicalizeFilename(currentDirectory, filenameBuffer, sizeof(currentDirectory));
        }
        errorPos = searchPos;
    }
    return firstError;
}

static struct view *PopToMark();

/* Pop to the next set of marks on the error list. */
compile_NextError(view, key)
    struct view *view;
    int key;
{

    if (currentError != NULL)
            currentError = currentError->next;
    if (currentError == NULL) {

        struct buffer *errorBuffer = buffer_FindBufferByName("Error-Log");

        if (errorBuffer == NULL) {
            message_DisplayString(view, 0, "Can't find compilation buffer.");
            return;
        }

        message_DisplayString(view, 0, "Parsing errors...");
        im_ForceUpdate();
        currentError = MakeErrorList(errorBuffer);
    }

    if (currentError != NULL) {
        PopToMark(currentError->messageMark, FALSE, view);
        PopToMark(currentError->offendingCodeMark, TRUE, view);
    }
    else
        if (currentProcess != NULL)
            message_DisplayString(view, 0, "Not done finding all your mistakes yet.");
        else
            message_DisplayString(view, 0, "I find no more errors!");
}

/* Pop to the previous set of marks on the error list. */
compile_PreviousError(view, key)
    struct view *view;
    int key;
{

    if (currentError == NULL) {

        struct buffer *errorBuffer = buffer_FindBufferByName("Error-Log");

        if (errorBuffer == NULL) {
            message_DisplayString(view, 0,  "Can't find compilation buffer.");
            return;
        }

        message_DisplayString(view, 0, "Parsing errors...");
        im_ForceUpdate();
        currentError = MakeErrorList(errorBuffer);
    }

    if (currentError == NULL)
        currentError = lastParsed;

    if (currentError != NULL) {
        if (currentError->previous != NULL)
            currentError = currentError->previous;
        PopToMark(currentError->messageMark, FALSE, view);
        PopToMark(currentError->offendingCodeMark, TRUE, view);
    }
    else
        message_DisplayString(view, 0, "There appears to be no errors here!");
}

struct finderInfo {
    struct frame *myFrame, *otherFrame, *bestFrame;
    struct buffer *myBuffer;
};

static boolean FrameFinder(frame, info)
    struct frame *frame;
    struct finderInfo *info;
{

    struct rectangle bogus;

    if (info->myFrame == frame)
        return FALSE;
    if (info->otherFrame != NULL && info->bestFrame != NULL)
	return TRUE;
    frame_GetVisualBounds(frame, &bogus);
    if (!rectangle_IsEmptyRect(&bogus))
        if (frame_GetBuffer(frame) == info->myBuffer) {
            info->bestFrame = frame;
            return FALSE;
        }
        else {
            info->otherFrame = frame;
            return FALSE;
        }
    return FALSE;
}

ViewEqual(frame, view)
    struct frame *frame;
    struct view *view;
{

#if 1
    return (frame_GetView(frame) == view);
#else /* 1 */
    return (view_GetIM((struct view *) frame) == view_GetIM(view))
#endif /* 1 */
}

struct frame *FindByView(view)
    struct view *view;
{

    return frame_Enumerate(ViewEqual, view);
}

/* Find a window other that the one that contains this inset.  Create one if we have to. */
static struct view *PutInAnotherWindow(view, buffer, forceWindow)
    struct view *view;
    struct buffer *buffer;
    int forceWindow;
{

    boolean FrameFinder();
    struct frame *frame;
    struct finderInfo myInfo;

    myInfo.myFrame = FindByView(view);
    myInfo.otherFrame = NULL;
    myInfo.bestFrame = NULL;
    myInfo.myBuffer = buffer;
    frame_Enumerate(FrameFinder, &myInfo);
    frame = (myInfo.bestFrame != NULL) ? myInfo.bestFrame : ((myInfo.otherFrame != NULL) ? myInfo.otherFrame : NULL);
    if (frame == NULL) {

        struct im *newIM;

        if (!forceWindow)
            return NULL;
        newIM = im_Create(NULL);
        frame = frame_Create(buffer);
        im_SetView(newIM, frame);

/* This is here because the frame_Create call can't set the input focus
 * because the frame didn't have a parent when it called view_WantInputFocus.
 * This is bogus but hard to fix...
 */
        view_WantInputFocus(frame_GetView(frame), frame_GetView(frame));
    }
    else if (frame_GetBuffer(frame) != buffer)
	frame_SetBuffer(frame, buffer, TRUE);
    return frame_GetView(frame);
}

/* A sort of kluge-o-matic routine.
 * Takes a mark, a flag and a window. The flag says whether to use the window,
 * or to not use the window if this mark's buffer is not already displayed.
 */
static struct view *PopToMark(mark, useWindowFlag, textview)
    struct mark *mark;
    int useWindowFlag;
    struct textview *textview;
{

    struct buffer *buffer = buffer_FindBufferByData(mark->object);

    if (useWindowFlag) {
        if (Text(textview) != (struct text *) mark->object) {

            struct frame *frame = FindByView((struct view *) textview);

            frame_SetBuffer(frame, buffer, TRUE);
            textview = (struct textview *) frame_GetView(frame);
        }
    }
    else {
        textview = (struct textview *) PutInAnotherWindow((struct view *) textview, buffer, TRUE); /* Make sure it gets in a window */
    }

    textview_SetDotPosition(textview, mark_GetPos(mark));
    textview_SetDotLength(textview, mark_GetLength(mark));
    textview_FrameDot(textview, mark_GetPos(mark));
    textview_WantUpdate(textview, textview);
    return (struct view *) textview;
}

boolean compile__InitializeClass(classID)
    struct classheader *classID;
{

    struct classinfo *textviewType = class_Load("textview");
    char *command;

    proctable_DefineProc("compile-build", compile_Build, textviewType, NULL, "Start a compilation.");
    proctable_DefineProc("compile-kill-build", compile_KillBuild, textviewType, NULL, "Terminate a compilation.");
    proctable_DefineProc("compile-next-error", compile_NextError, textviewType, NULL, "Step forward through compilation errors.");
    proctable_DefineProc("compile-previous-error", compile_PreviousError, textviewType, NULL, "Step backward through compilation errors.");

    if ((command = environ_GetProfile("CompileCommand")) == NULL)
        defaultCommand = "build -k";
    else {
        defaultCommand = (char *) malloc(strlen(command) + 1);
        strcpy(defaultCommand, command);
    }
    compileCommand = defaultCommand;
    return TRUE;
}
