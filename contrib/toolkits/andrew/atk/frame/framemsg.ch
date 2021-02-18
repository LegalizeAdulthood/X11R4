/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framemsg.ch,v 1.7 89/09/08 16:29:22 ghoti Exp $ */
/* $ACIS:framemsg.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framemsg.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidframemessage_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/framemsg.ch,v 1.7 89/09/08 16:29:22 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* framemsg.H
 * Class for the frame's message line abstraction.
 *
 */

/* All buffer size arguments in this module are as returned by sizeof(buffer).
 * That is, they include the NUL at the end of the string.
 */
class framemessage[framemsg]: msghandler[msghndlr] {
    overrides:
        DisplayString(int priority, char *string) returns int;
        AskForString(int priority, char *prompt, char *defaultString, char *buffer, int bufferSize) returns int;
        AskForStringCompleted(int priority, char *prompt, char *defaultString, char *buffer, int bufferSize, struct keystate *keystate,
                              procedure completionProc, procedure helpProc, long functionData, int flags) returns int;
        MultipleChoiceQuestion(int priority, char *prompt, long defaultChoice, long *result, char **choices, char *abbrevKeys) returns int;
        CancelQuestion();
/* These next calls are only applicable during an AskFor... operation. */
        GetCurrentString(char *buffer, int bufferSize) returns int;
        InsertCharacters(int pos, char *string, int len) returns int;
        DeleteCharacters(int pos, int len) returns int;
        GetCursorPos() returns int;
        SetCursorPos(int pos) returns int;
        Asking() returns boolean;
	Advice(enum message_Preference);

	methods:
	SetCompanion(struct msghandler *companion);

    classprocedures:
        FinalizeObject();
        Create(struct frame *myFrame) returns struct framemessage *;
        InitializeClass() returns boolean;
	InitializeObject(struct framemessage *self) returns boolean;    

    data:
        struct msghandler *companion;
        struct frame *frame;
        struct frameview *messageView;
        struct text *messageText;
        struct view *oldInputFocus;
        struct buffer *realBuffer;
        long messageLen; /* Length of message displayed during an AskFor operation. */
        enum message_CompletionCode (*completionProc)();
        int (*helpProc)();
        long completionData;
        char *textBuffer;
        int maxTextSize;
        struct keystate *keystate;
        struct event *erasureEvent;
        char flags;
        char asking;
        char punt;
        char hasDefault; /* TRUE if defaultString in nonNULL. */
};
