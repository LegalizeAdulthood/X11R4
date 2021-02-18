/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/msghndlr.ch,v 2.7 89/06/16 11:57:51 wjh Exp $ */
/* $ACIS:msghndlr.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/msghndlr.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidmsghandler_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/msghndlr.ch,v 2.7 89/06/16 11:57:51 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* msghndlr.H
 * Definition of a msghandler object for use by message handlers.
 *
 */

#define msghandler_VERSION 1

#ifndef message_DEFINED /* Hack, hack, hack... */
enum message_CompletionCode { /* Called message for simplicity. */
    message_Complete, /* The string returned is a unique completion of the user input. */
    message_CompleteValid, /* The string returned is a completion and a valid substring of another complete. */
    message_Valid, /* The string returned is a valid substring of a complete entry. Must be at least as long a user's input. */
    message_Invalid, /* The string returned is the longest viable prefix of the user's input. */
};

/* Values for help procedure calls. */
enum message_HelpItem {
    message_HelpListItem, /* This call is an item to be added to the list of possible completions. */
    message_HelpGenericItem, /* This call contains text with no notable features. Presumably for the user to read. */
};

enum message_Preference {
    message_NoBlock = 3,
    message_OnTop = 2, 
    message_OnBottom = 1, 
    message_Center = 0
};


#define message_MustMatch 1 /* Can only return valid completions if this flag is set. */
#define message_InitialHelp 2 /* Display help at beggining if this flag is set. */
#define message_NoInitialString 4 /* Do not use default as initial string. */
#define message_Mandatory 8 /* Indicates that question must be answered. It cannot be punted. */
#endif /* message_DEFINED  */

class msghandler[msghndlr] {
    methods:
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
	Advice(enum message_Preference pref) ;
};
