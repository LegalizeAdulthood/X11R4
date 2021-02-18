/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/frame.ch,v 2.7 89/04/16 22:02:04 zs01 Exp $ */
/* $ACIS:frame.ch 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/frame.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidframe_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/frame.ch,v 2.7 89/04/16 22:02:04 zs01 Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* frame.H
 * Class definition for frame module.
 *
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/frame/RCS/frame.ch,v 2.7 89/04/16 22:02:04 zs01 Exp $
 * $ACIS:frame.ch 1.5$
 *
 */

#define frame_VERSION 1

#define frame_MUSTANSWER -1

#include <rect.h>

#include <message.ih>

#define frame_GetView(frame) (frame->targetView)
#define frame_GetChildView(frame) (frame->childView)
#define frame_GetBuffer(frame) (frame->buffer)
#define frame_GetCommandEnable(frame) (frame->commandEnable)
#define frame_GetTitle(frame) (frame->title)

class frame: lpair {
    methods:
        SetView(struct view *view);
        SetBuffer(struct buffer *buffer, boolean setInputFocus);
        SetCommandEnable(boolean enable);
        GetHelpBuffer() returns struct buffer *;
        SetHelpBuffer(struct buffer *buf, boolean setInputFocus) returns struct buffer *;
        SetTitle(char *title);
	Advice(enum message_Preference);
        DisplayString(int priority, char *string) returns int;
        AskForString(int priority, char *prompt, char *defaultString, char *buffer, int bufferSize) returns int;
        AskForStringCompleted(int priority, char *prompt, char *defaultString, char *buffer, int bufferSize, struct keystate *keystate,
                              procedure completionProc, procedure helpProc, long functionData, int flags) returns int;
        MultipleChoiceQuestion(int priority, char *prompt, long defaultChoice, long *result, char **choices, char *abbrevKeys) returns int;

    classprocedures:
        InitializeObject(struct frame *self) returns boolean;
        FinalizeObject(struct frame *self);
        Enumerate(procedure function, long functionData) returns struct frame *;
	Create(struct buffer *buffer) returns struct frame *;
	InitializeClass()returns boolean;
    overrides: /* Make it hard for people to access our lpair functions. */
        Update();
        FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
        WantHandler(char *handlerName) returns struct basicobject *;
        PostKeyState(struct keystate *keystate);
        PostMenus(struct menulist *menulist);
        ObservedChanged(struct observable *, long value);
	Hit(enum view_MouseAction action, long x, long y, long nclicks) returns struct view *;
	WantUpdate(struct view *v);

    data:
        struct frame *next;
        struct buffer *buffer;
        struct view *targetView;
        struct view *childView;
        char *title;
        struct text *messageText;
        struct frameview *messageView;
        struct framemessage *messageLine;
        int lineHeight; /* Height in pixels of the nominal one line message line. */
        struct frameview *dialogView;
        struct framemessage *dialogLine;
        struct keystate *keystate;
        struct menulist *menulist;
        char helpBuffer[16]; /* 16 is greater than sizeof("Help-Buffer-99") */
        boolean commandEnable;
	struct buffer *DialogBuffer;
	struct view *DialogTargetView;
	struct view *DialogBufferView;

    struct cursor *octcursor,*arrowcursor;
    struct fontdesc *myfontdesc;
    struct menulist *mymenus;
    struct keystate *mykeystate;
    enum message_Preference PositionalPreference;
    char **MultipleAnswers;
    struct rectangle AnswerBox;
    struct rectangle *HeightsOfAnswer;
    struct rectangle mesrec,bufferrec;
    struct pendingupdates *uplist;
    int IsAsking,
	IsBlocking,
	WildestAnswer, /* selected answer */
	DefaultWildestAnswer, /* default answer */
	StackPos, 
        hasDialogMessage,
	NumAnswerChoices,
        AwaitingFocus,
        UpdateRequested,
        buttonmaxwid;
        struct dataobject *object;
        boolean dataModified;
};
