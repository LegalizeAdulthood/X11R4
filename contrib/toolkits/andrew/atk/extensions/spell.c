/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: spell.c,v 1.2 89/12/01 12:21:09 xguest Exp $ */
/* $ACIS:spell.c 1.2$ */
/* $Source: /xsrc/contrib/toolkits/andrew/atk/extensions/RCS/spell.c,v $ */

#ifndef lint
static char *rcsid = "$Header: spell.c,v 1.2 89/12/01 12:21:09 xguest Exp $";
#endif /* lint */

/* xspell.c
 * Mega hack to get ispell capabilities in bx.
 * Perhaps will clean up later...
 */

#include <andrewos.h> /* sys/types.h sys/file.h */

#include <class.h>
#include <signal.h>
#include <ctype.h>

#include <text.ih>
#include <textv.ih>
#include <mark.ih>
#include <im.ih>
#include <message.ih>
#include <proctbl.ih>
#include <environ.ih>

#include <spell.eh>

#define Text(self) ((struct text *) self->header.view.dataobject)

static FILE *inFile, *outFile;
static char *wordArray[10];
static boolean expertMode;

/* Process handling stuff. Copied verbatim from compile.c */

#define READFD 0	/* The pipe index from which you can do a read. */
#define WRITEFD 1	/* The pipe index to which you can do a write. */

struct process {
    FILE *inFile, *outFile;
    short pid;
    boolean stopped;
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

        setpgrp(0, getpid());
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

/* Kill the speller process. Depends on the interact loop to cleanup the
 * zombie.
 */
static int KillSpeller()
{

    fclose(inFile);
    fclose(outFile);
    return(0);
/* We assume the EOF will terminate the speller. The code used to send a
 * SIGTERM, but this resulted in a race condition that would trash the user's
 * private dictionary if the last word in a document was inserted in said
 * dict.
 */
}

/* Makes up a prompt line and fills in the global word array. This will be
 * replaced by a multiple choice question call shortly.
 * Really should have a bounds test on buffer...
 */
static int BuildQuestionLine(buffer, choices)
    char *buffer, *choices;
{

    int i;

    while (*choices == ' ')
        choices++;
    for (i = 0; *choices != '\0' && (i < 10); i++) {
        sprintf(buffer, "%d) ", i);
        buffer += strlen(buffer);
        wordArray[i] = choices;
        while (*choices && *choices != ' ')
            *buffer++ = *choices++;
	if (!*choices) break;
        *choices++ = '\0';
        *buffer++ = ' ';
        while (*choices && *choices == ' ' || *choices == '\n')
            choices++;
    }
    *buffer = '\0';
    return i - 1;
}

/* Actually spell check an entire document starting at the current cursor
 * position.
 */
/* ARGUSED */
void spell__CheckDocument(classID, self, rock)
    struct classheader *classID;
    struct textview *self;
    long rock;
{

    int pos = textview_GetDotPosition(self), savePos = pos, saveLength = textview_GetDotLength(self);
    struct text *text;
    struct mark *tempMark;
    struct process *speller;
    char testBuf[100];

/* Post a wait cursor. */
    message_DisplayString(self, 0, "Starting spell check.");
    im_ForceUpdate();
    text = Text(self);
    tempMark = text_CreateMark(text, 0, 0);

/* Start up process and free process structure. */
    if ((speller = StartProcess("ispell -a", &inFile, &outFile)) != NULL)
        free(speller); /* Not used for now. */

/* Do an initial rendezvous to ensure that process actually started. */
    if (fputs("the\n", outFile) == EOF || fflush(outFile) == EOF ||
        fgets(testBuf, sizeof(testBuf), inFile) == NULL || *testBuf != '*') {
        message_DisplayString(self, 0, "Could not start ispell. Spell checking may not be supported at this site.");
        return;
    }

    while (text_GetChar(text, pos) != EOF) {

        int c;

        while (!isalpha(c = text_GetChar(text, pos)) && c != EOF) /* Skip to "word" */
            pos++;

        if (text_GetChar(text, pos) != EOF) {

            int c, startpos = pos;
            char buffer[4096], thisWord[100], *temp = thisWord; /* Lots of space... */

            for (c = text_GetChar(text, pos); isalpha(c); c = text_GetChar(text, ++pos)) {
                putc(c, outFile);
                *temp++ = c;
            }
            putc('\n', outFile);
            fflush(outFile);
            *temp = '\0';

            fgets(buffer, sizeof(buffer), inFile);
            temp = buffer;
            while (*temp == ' ' || *temp == '\n')
                temp++;
            fflush(stdout);
            switch (*temp) {

                int flag, maxWord;
                char messageBuf[200];

                case '*': /* Found it. */
                    break;
                case '+': /* Suffix removal. */
                    break;
                case '&': /* List of near misses given. */
                case '#': /* No near misses found. */
                    textview_SetDotPosition(self, startpos);
                    textview_SetDotLength(self, pos - startpos);
                    textview_FrameDot(self, startpos);
                    textview_WantUpdate(self, self);
                    im_ForceUpdate();

                    if (*temp == '&')
			maxWord = BuildQuestionLine(messageBuf, &temp[1]);
                    else
                        strcpy(messageBuf, "Word not found.");

                    sprintf(messageBuf + strlen(messageBuf), "   [<space>][A][R]%s[I][Q] ?", expertMode ? "[E]" : "");

                    do {

                        int response;
                        int i;
                        int len;
                        int max;
                        char wordBuf[200];
                        char *word;

                        i = textview_GetDotPosition(self);
			len = textview_GetDotLength(self);
                        if (len > sizeof(wordBuf) - 1)
                            max = i + sizeof(wordBuf) - 1;
			else
			    max = i + len;
                        for (word = wordBuf; i < max; ++i)
                            *word++ = text_GetChar(text, i);
                        *word = '\0';
                        flag = 0;
			message_DisplayString(self, 0, messageBuf);
			response = im_GetCharacter(textview_GetIM(self));

                        switch (response) {
                            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                                if ((response - '0') <= maxWord) {
                                    mark_SetPos(tempMark, pos);
                                    mark_SetLength(tempMark, 0);
                                    text_ReplaceCharacters(text, textview_GetDotPosition(self), textview_GetDotLength(self), wordArray[response - '0'], strlen(wordArray[response - '0']));
                                    pos = mark_GetPos(tempMark);
                                }
                                else {
                                    message_DisplayString(self, 0, "Number too large. Press any key to continue.");
                                    im_GetCharacter(textview_GetIM(self));
                                    flag = 1;
                                }
                                break;
                            case ' ':
                                message_DisplayString(self, 0, "Accepted.");
                                break;
                            case 'a':
                            case 'A':
                                fputs("@", outFile);
                                fputs(thisWord, outFile);
                                fputc('\n', outFile);
                                fflush(outFile);
                                fgets(buffer, sizeof(buffer), inFile); /* Snag garbage... */
                                message_DisplayString(self, 0, "Accepted for rest of session.");
                                break;
                            case 'r':
                            case 'R':
                                if (message_AskForString(self, 0, "Replacement word: ", wordBuf, wordBuf, sizeof(wordBuf)) >= 0) {
                                    mark_SetPos(tempMark, startpos);
                                    text_ReplaceCharacters(text, textview_GetDotPosition(self), textview_GetDotLength(self), wordBuf, strlen(wordBuf));
                                    pos = mark_GetPos(tempMark);
                                }
                                else {
                                    message_DisplayString(self, 0, "Canceled. Press any key to continue.");
                                    im_GetCharacter(textview_GetIM(self));
                                    flag = 1;
                                }
                                break;
                            case 'i':
                            case 'I':
                                putc('*', outFile);
                                fputs(thisWord, outFile);
                                fputc('\n', outFile);
                                fflush(outFile);
                                fgets(buffer, sizeof(buffer), inFile); /* Snag garbage... */
                                message_DisplayString(self, 0, "Entered in private dict.");
                                break;
                            case 'q':
                            case 'Q':
                            case EOF:
                                KillSpeller();
                                message_DisplayString(self, 0, "Spell check terminated.");
                                textview_SetDotPosition(self, savePos);
                                textview_SetDotLength(self, saveLength);
                                textview_WantUpdate(self, self);
                                text_RemoveMark(text, tempMark);
                                mark_Destroy(tempMark);
                                return; /* Get out of here. */
                            case 'e':
                            case 'E':
                                if (expertMode) {
				    mark_SetPos(tempMark, startpos);
				    textview_SetDotLength(self, 0);
				    textview_WantUpdate(self, self);
				    message_DisplayString(self, 0, "Recursive editing, ^C exits.");
				    im_KeyboardProcessor();
				    pos = mark_GetPos(tempMark); /* Recheck... */
				    break;
				}
                            default:
				if (expertMode)
				    message_DisplayString(self, 0, "<space>: Accept once, [A]: Accept, [R]: Replace, [E] Edit, [I]: Insert in private dictionary, [Q]: Quit, <number>: replace with that word. Press any key to continue.");
				else
				    message_DisplayString(self, 0, "<space>: Accept once, [A]: Accept, [R]: Replace, [I]: Insert in private dictionary, [Q]: Quit, <number>: replace with that word. Press any key to continue.");
                                im_GetCharacter(textview_GetIM(self));
                                flag = 1;
                                break;
                        }
                    } while (flag);
                    textview_WantUpdate(self, self);
                    im_ForceUpdate();
                    break;
              default: /* Ispell is broken. */
                    break;
            }
        }
    }
    KillSpeller();
    text_RemoveMark(text, tempMark);
    mark_Destroy(tempMark);
    textview_SetDotPosition(self, savePos);
    textview_SetDotLength(self, saveLength);
    message_DisplayString(self, 0, "Spell check completed.");
    textview_WantUpdate(self, self);
}

static void spell_CheckSpellingCmd(self, rock)
    struct textview *self;
    long rock;
{
    spell_CheckDocument(self, rock);
}
 
boolean spell__InitializeClass(classID)
    struct classheader *classID;
{
    proctable_DefineProc("spell-check-document", spell_CheckSpellingCmd, class_Load("textview"), NULL, "Checks spelling from the caret on.");

    expertMode = environ_GetProfileSwitch("expertmode", FALSE);
    return TRUE;
}
