/* $Header: input.c,v 1.5 89/07/02 15:58:24 moraes Exp $ */
#include <ctype.h>

#include "xpic.h"
#include "windows.h"
#include "Minibuf.h"

static char *input;
static BOOL done;

/*
 *  This routine focuses all input on the inputWindow, and starts up a
 *  loop which forms a secondary dispatcher, ignoring all events except
 *  ExposeWindow events on windows other than the inputWindow. This
 *  effectively forces the user to use the inputWindow. The loop
 *  terminates when input is completed, either by inputting a string, in
 *  which case finished_input will be invoked, or by aborting.  Both set
 *  the completion flag and the loop quits, focussing input back to the
 *  previous holder. No harm is done to this application if focus is not
 *  given to the minibuffer, or removed from it - it's just more
 *  convenient for the user who doesn't have to move a mouse around too
 *  much.  The routine returns the input string, or a NULL if the input
 *  was aborted.
 */
char *get_input(prompt, default_string, complete)
char *prompt, *default_string;
{
	XEvent ev;
	extern void SetWorkingCursor(), SetTextCursor();
	
	MinibufGetInput(inputWidget, prompt, default_string, complete);
	AcquireFocus(inputWidget);
	XtAddGrab(inputWidget, False, False);
	done = FALSE;
	SetTextCursor();
	while (!done) {
		XtNextEvent(&ev);
		(void) XtDispatchEvent(&ev);
	}
	SetWorkingCursor();
	ReleaseFocus(inputWidget);
	XtRemoveGrab(inputWidget);
	return(input);
}


/*
 *  Callback, invoked when user hits RETURN in the inputWidget (or whatever
 *  the user has bound 'newline' to) in which case inp_string points to
 *  an alloc'ed string. If the user aborts input (^G, ^C) then
 *  inp_string is NULL
 */
/*ARGSUSED*/
void finished_input(w, tag, inp_string)
Window w;
caddr_t tag;
char *inp_string;
{
	input = inp_string;
	done = TRUE;
}


/* 
 *  Asks to confirm something - If they reply "y", returns YES, if they
 *  reply "n", returns NO, if they abort, returns ABORT. You are given
 *  IMPATIENCE tries to answer
 */
confirm(query, default_answer)
char *default_answer;
char *query;
{
	char *answer;
	char c;
	char *mesg = "Answer yes or no, please (y/n)";
	int count = 0;
#define IMPATIENCE	5

	do {
		if ((answer = get_input(query, default_answer, FALSE)) == NULL)
			return (ABORT);
		c = (isupper(answer[0])) ? tolower(answer[0]) : answer[0];
		if (c == 'y')
			return(YES);
		else if (c == 'n')
			return(NO);
		message(mesg);
		/* Let's get really explicit next time */
		mesg = "Type 'y' and RETURN if you want to answer YES, 'n' for NO, CTRL-G to abort";
	} while (count++ < IMPATIENCE);
	message("Forget it - Aborting");
	return(ABORT);
}


/*
 *  Message is printed on the communication line if the windows are
 *  mapped, otherwise fprintf'ed to stderr
 */
message(s)
char *s;
{
	if (inputWidget != 0)
		MinibufDisplayMessage(inputWidget, s, TRUE);
	else
		(void) fprintf(stderr, "%s\n", s);
}
