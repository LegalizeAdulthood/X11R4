/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/dsearch.c,v 2.6 89/02/24 09:02:07 ghoti Exp $ */
/* $ACIS:dsearch.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/dsearch.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/dsearch.c,v 2.6 89/02/24 09:02:07 ghoti Exp $";
#endif /* lint */

#include <class.h>
#include <text.ih>
#include <textv.ih>
#include <im.ih>
#include <search.ih>
#include <message.ih>
#include <proctbl.ih>

#include <dsearch.eh>

#define MAXSTRING	256
static char lastString[MAXSTRING] = "";
static char *LastPattern;
static int lcSearch = 0;
static int failures = 0;

/* Describe how the current seach string has failed. */
#define FORWARD		1
#define REVERSE		2

static void SearchForward(view)
    register struct textview *view;
{

    int pos = 0, argument;
    register int count;
    register struct text *text;

    text = (struct text *) view->header.view.dataobject;
    argument = im_Argument(textview_GetIM(view));
    if (GetPattern(view, text, FORWARD) < 0)
	return;
    textview_SetDotPosition(view, textview_GetDotPosition(view) + textview_GetDotLength(view));
    for (count = 0; count < argument; count++) {
	pos = search_MatchPattern (text, textview_GetDotPosition(view), LastPattern);
	if (pos < 0) {
	    textview_SetDotLength(view,0);
	    message_DisplayString(view, 0, "Search failed.");
	    failures |= FORWARD;
	    textview_WantUpdate(view, view);
	    return;
	}
	failures = 0;
	textview_SetDotPosition(view, pos);
	textview_SetDotLength(view, search_GetMatchLength());
    }
    textview_FrameDot(view, pos);
    textview_WantUpdate(view, view);
    message_DisplayString(view, 0, "Done.");
    return;
}

static void SearchReverse(view)
    register struct textview *view;
{

    int argument, originalPos, pos = 0;
    register int count;
    register struct text *text;

    text = (struct text *) view->header.view.dataobject;
    originalPos = textview_GetDotPosition(view);
    argument = im_Argument(textview_GetIM(view));
    if (GetPattern(view, text, REVERSE) < 0)
	return;
    textview_SetDotLength(view, 0);
    if (originalPos > 0)
        textview_SetDotPosition(view, originalPos - 1);
    for (count = 0; count < argument; count++) {
	pos = search_MatchPatternReverse(text, textview_GetDotPosition(view), LastPattern);
	if (pos < 0) {
	    textview_SetDotPosition(view, originalPos);
            message_DisplayString(view, 0, "Reverse search failed.");
	    failures |= REVERSE;
	    textview_WantUpdate(view, view);
	    return;
	}
	failures = 0;
	textview_SetDotPosition(view, pos);
	textview_SetDotLength(view, search_GetMatchLength());
    }
    textview_FrameDot(view, pos);
    textview_WantUpdate(view, view);
    message_DisplayString(view, 0, "Done.");
    return;
}


static int GetPattern(view, text, direction)
    struct textview *view;		/* textview we're using */
    struct text *text;		/* and doc */
    int direction;			/* direction of the search */
{

    char string[MAXSTRING];
    int pos, len;
    int useLast = FALSE;
    int lastCmdWasSearch;
    int lastSearchFailed = FALSE;

    pos = textview_GetDotPosition(view);
    len = textview_GetDotLength(view);
    lastCmdWasSearch = im_GetLastCmd(textview_GetIM(view)) == lcSearch;
    if (lastCmdWasSearch)
	lastSearchFailed = (failures & direction);
    /* Now figure out what pattern to use. */
    if (lastCmdWasSearch && !lastSearchFailed)
	useLast = TRUE;
    else if (lastCmdWasSearch || len == 0) {

        char prompt[MAXSTRING + sizeof("Reverse search for : ")];

        if (*lastString != '\0')
            sprintf(prompt, "%s for [%s] : ", (direction == FORWARD) ? "Search" : "Reverse search", lastString);
        else
            sprintf(prompt, "%s for: ", (direction == FORWARD) ? "Search" : "Reverse search");
	if (message_AskForString(view, 0, prompt, NULL, string, 100) < 0)
	    return -1;
	if (string[0] == 0)
	    useLast = TRUE;
    }
    else {

        int c, to, from;

	/* Use the current selection. */
	for (to = 0, from = 0; from < len && to+2 < MAXSTRING; ++from) {
	    c = text_GetChar(text, pos + from);
	    if (c == '\\' || c == '[' || c == ']' || c == '*')
		string[to++] = '\\';
	    string[to++] = c;
	}
	string[to++] = 0;
	if (from < len) {
	    message_DisplayString(view, 0, "Search string too long.");
	    return -1;
	}
	if (strcmp(string, lastString) == 0)
	    useLast = TRUE;
    }
    if (!useLast) {

        char *errorMessage;

	failures = 0;
	errorMessage = search_CompilePattern(string, &LastPattern);
	if (errorMessage != NULL) {
	    message_DisplayString(view, 0, errorMessage);
	    return -1;
	}
	strcpy(lastString, string);
    }
    im_SetLastCmd(textview_GetIM(view), lcSearch);
    return 0;
}

boolean dynsearch__InitializeClass(classID)
    struct classheader *classID;
{
    struct classinfo *textviewClassinfo;

    lcSearch = im_AllocLastCmd();
    LastPattern = NULL;
    lastString[0] = 0;

    textviewClassinfo = class_Load("textview");
    proctable_DefineProc("dynsearch-search-forward", SearchForward, textviewClassinfo, NULL,
                         "Search forward for a pattern; uses selection.");
    proctable_DefineProc("dynsearch-search-reverse", SearchReverse, textviewClassinfo, NULL,
                         "Search backwards for a pattern; uses selection.");
    return TRUE;
}
