
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/buttons.c,v 1.16 89/11/17 11:48:07 ricks Rel Locker: ricks $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * buttons.c: create and handle the buttons
 *
 */

#include "copyright.h"
#include <X11/Xos.h>
#include <stdio.h>
#include "config.h"
#include "utils.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Dialog.h>
#include "compose.h"
#include "cursor.h"
#include "mesg.h"
#include "dialogs.h"
#include "modes.h"
#include "resources.h"
#include "internals.h"
#include "save.h"
#include "xmisc.h"
#include "error_hnds.h"
#include "xthelper.h"
#include "xrn.h"
#include "buttons.h"

#define GROUP_NAME_SIZE 128

#ifdef VMS
#define getArticle getArticleFile
#include <descrip.h>
#endif

static Widget *AddButtons = NIL(Widget);
static Widget *NgButtons = NIL(Widget);
static Widget *AllButtons = NIL(Widget);
static Widget *ArtButtons = NIL(Widget);
static Widget *ArtSpecButtons = NIL(Widget);

static char *AddGroupsString = NIL(char);  /* new newsgroups list ...           */
static char *NewsGroupsString = NIL(char); /* newsgroups list that is displayed */
static char *AllGroupsString = NIL(char); /* list of all groups so the user */
				          /* can subscribe/unsubscribe to them */
static char *SubjectString = NIL(char); /* list of article numbers and subjects */

static XawTextPosition GroupPosition = (XawTextPosition) 0; /* cursor position */
				/* newsgroup window */
static XawTextPosition ArtPosition = (XawTextPosition) 0;	/* cursor position in */
				/* article subject window */
static XawTextPosition NewsgroupTop = (XawTextPosition) 0; /* top position in */
				/* newsgroup window */
static XawTextPosition ArticleTop = (XawTextPosition) 0; /* top position in */
				/* article subject window */

static char LastGroup[GROUP_NAME_SIZE];	/* last newsgroup accessed; used to */
				/* determine whether or not to move the */
				/* cursor in the newsgroup window */
static char *LastArticle;	/* the article currently displayed */
				/* in the article window */
static int CurrentArticle;	/* the number of the article currently */
				/* displayed, used for marking an article */
				/* as saved */

static long PrevArticle;	/* the number of the article displayed */
				/* before the current one */

static int ArtStatus = 0;	/* keeps track of what kind of article to */
				/* to search for: next, previous, or next */
				/* unread */
static int AllStatus = 0;	/* keeps track of which order to put the */
				/* groups in in all groups mode */
static char *LastRegexp;	/* last regular expression searched for */
static int LastSearch;		/* the direction of the last regular */
				/* expression search */

static XawTextPosition First;	/* keeps the beginning of the selected text */
				/* for the command move groups */
static XawTextPosition Last;	/* keeps the end of the selected text for */
				/* the command move groups */

static int Action;		/* action to take when a confirmation box */
				/* is clicked in */

static int ArtEntry = 1;

static XawTextSelectType lineSelectArray[] = {XawselectLine, XawselectNull};

static Arg lineSelArgs[] = {
    {XtNselectTypes, (XtArgVal) lineSelectArray},
};    

static XawTextSource AllSource = (XawTextSource) 0;
static XawTextSource SubjectSource = (XawTextSource) 0;
static XawTextSource ArtSource = (XawTextSource) 0;
static XawTextSource DummySource = (XawTextSource) 0;


/* article mode "modes" ... determines what to do: jump out of article */
/* mode, change the subject string, or do nothing */
#define art_DONE 0
#define art_CHANGE 1
#define art_NOCHANGE 2

/* keeps track of which type of article to search for: Next, Unread, or */
/* previous */
#define art_NEXT 0
#define art_UNREAD 1
#define art_PREV 2
#define art_NEXTWRAP 3


static int Mode = NO_MODE;            /* current mode                       */
static int PreviousMode = NO_MODE;    /* previous mode, what buttons to */
				       /* remove */

#define XRN_NO 0
#define XRN_YES 1


static struct _translations {
    Widget widget;
    XtTranslations tables[MAX_MODE];
    char *unparsed[MAX_MODE];
} Translations[6];


/*
 * The newsgroup and article command buttons
 *
 *    To add a button:
 *
 *      - add the appropriate static Arg[] for describing the button
 *        label and handler (define the return value of the handler)
 *      - add the variable and size to the apropriate *ButtonList
 *      - add it to the actions table and the keyactions table
 *
 */

static void doTheRightThing(); 
void abortSet();
static XtActionsRec TopActions[] = {
    {"doTheRightThing",	doTheRightThing},
    {"abortSearch", abortSet},
};


static void addQuitFunction();
static void addFirstFunction();
static void addLastFunction();
static void addAfterFunction();
static void addUnsubFunction();

static XtActionsRec AddActions[] = {
    {"addQuit",		addQuitFunction},
    {"addFirst",	addFirstFunction},
    {"addLast",		addLastFunction},
    {"addAfter",	addAfterFunction},
    {"addUnsub",	addUnsubFunction},
};

static void ngQuitFunction();
static void ngReadFunction();
static void ngNextFunction();
static void ngPrevFunction();
static void ngCatchUpFunction();
static void ngSubscribeFunction();
static void ngUnsubFunction();
static void ngGotoFunction();
static void ngAllGroupsFunction();
static void ngRescanFunction();
static void ngPrevGroupFunction();
static void ngSelectFunction();
static void ngMoveFunction();
static void ngExitFunction();
static void ngPostFunction();
static void ngGripeFunction();
static void ngCheckPointFunction();

static XtActionsRec NgActions[] = {
    {"ngQuit",		ngQuitFunction},
    {"ngRead",		ngReadFunction},
    {"ngNext",		ngNextFunction},
    {"ngPrev",		ngPrevFunction},
    {"ngCatchUp",	ngCatchUpFunction},
    {"ngSubscribe",	ngSubscribeFunction},
    {"ngUnsub",		ngUnsubFunction},
    {"ngGoto",		ngGotoFunction},
    {"ngRescan",	ngRescanFunction},
    {"ngAllGroups",	ngAllGroupsFunction},
    {"ngPrevGroup",	ngPrevGroupFunction},
    {"ngSelect",	ngSelectFunction},
    {"ngMove",		ngMoveFunction},
    {"ngExit",		ngExitFunction},
    {"ngGripe",		ngGripeFunction},
    {"ngPost",		ngPostFunction},
    {"ngCheckPoint",	ngCheckPointFunction},
};    

#ifndef NODOUBLECLICK
static char *NgClickTable = "<Btn1Down>(2):	ngRead()";
static XtTranslations NgClick = (XtTranslations) 0;

static char *ArtClickTable = "<Btn1Down>(2):	artNext()";
static XtTranslations ArtClick = (XtTranslations) 0;
#endif

static void artQuitFunction();
static void artNextFunction();
#ifdef XRN_TEXT_WIDGET
static void artScrollFunction();
#endif
static void artNextUnreadFunction();
static void artPrevFunction();
static void artLastFunction();
static void artNextGroupFunction();
static void artCatchUpFunction();
static void artGotoArticleFunction();
static void artMarkReadFunction();
static void artMarkUnreadFunction();
static void artUnsubFunction();
static void artSubNextFunction();
static void artSubPrevFunction();
static void artKillSessionFunction();
static void artKillLocalFunction();
static void artKillGlobalFunction();
static void artKillAuthorFunction();
static void artSubSearchFunction();
static void artContinueFunction();
static void artPostFunction();
static void artExitFunction();
static void artSaveFunction();
static void artReplyFunction();
static void artGripeFunction();
static void artForwardFunction();
static void artFollowupFunction();
static void artCancelFunction();
static void artRot13Function();
static void artHeaderFunction();
static void artPrintFunction();

static XtActionsRec ArtActions[] = {
    {"artQuit",		artQuitFunction},
    {"artNextUnread",	artNextUnreadFunction},
#ifdef XRN_TEXT_WIDGET
    {"artScroll",	artScrollFunction},
#endif
    {"artNext",		artNextFunction},
    {"artPrev",		artPrevFunction},
    {"artLast",		artLastFunction},
    {"artNextGroup",	artNextGroupFunction},
    {"artCatchUp",	artCatchUpFunction},
    {"artGotoArticle",	artGotoArticleFunction},
    {"artMarkRead",	artMarkReadFunction},
    {"artMarkUnread",	artMarkUnreadFunction},
    {"artUnsub",	artUnsubFunction},
    {"artSubNext",	artSubNextFunction},
    {"artSubPrev",	artSubPrevFunction},
    {"artKillSession",	artKillSessionFunction},
    {"artKillLocal",	artKillLocalFunction},
    {"artKillGlobal",	artKillGlobalFunction},
    {"artKillAuthor",	artKillAuthorFunction},
    {"artSubSearch",	artSubSearchFunction},
    {"artContinue",	artContinueFunction},
    {"artPost",		artPostFunction},
    {"artExit",		artExitFunction},
    {"artSave",		artSaveFunction},
    {"artReply",	artReplyFunction},
    {"artForward",	artForwardFunction},
    {"artFollowup",	artFollowupFunction},
    {"artCancel",	artCancelFunction},
    {"artRot13",	artRot13Function},
    {"artHeader",	artHeaderFunction},
    {"artPrint",	artPrintFunction},
    {"artGripe",	artGripeFunction},
};

static void allQuitFunction();
static void allSubFunction();
static void allFirstFunction();
static void allLastFunction();
static void allAfterFunction();
static void allUnsubFunction();
static void allGotoFunction();
static void allSelectFunction();
static void allMoveFunction();
static void allToggleFunction();

static XtActionsRec AllActions[] = {
    {"allQuit",		allQuitFunction},
    {"allSub",		allSubFunction},
    {"allFirst",	allFirstFunction},
    {"allLast",		allLastFunction},
    {"allAfter",	allAfterFunction},
    {"allUnsub",	allUnsubFunction},
    {"allGoto",		allGotoFunction},
    {"allSelect",	allSelectFunction},
    {"allMove",		allMoveFunction},
    {"allToggle",	allToggleFunction},
};

/*
 * definition of the buttons
 */

#ifdef lint
#define BUTTON(nm,lbl)
#else
#ifdef __STDC__	
#define BUTTON(nm,lbl)				\
static XtCallbackRec nm##Callbacks[] = {	\
    {busyCursor, NULL},				\
    {nm##Function, NULL},			\
    {unbusyCursor, NULL},			\
    {NULL, NULL}				\
};						\
static Arg nm##Args[] = {			\
    {XtNname, (XtArgVal) #nm},			\
    {XtNlabel, (XtArgVal) #lbl},		\
    {XtNcallback, (XtArgVal) nm##Callbacks}	\
};
#else
#define BUTTON(nm,lbl)				\
static XtCallbackRec nm/**/Callbacks[] = {	\
    {busyCursor, NULL},				\
    {nm/**/Function, NULL},			\
    {unbusyCursor, NULL},			\
    {NULL, NULL}				\
};						\
static Arg nm/**/Args[] = {			\
    {XtNname, (XtArgVal) "nm"},			\
    {XtNlabel, (XtArgVal) "lbl"},		\
    {XtNcallback, (XtArgVal) nm/**/Callbacks}	\
};
#endif
#endif

BUTTON(addQuit,quit)
BUTTON(addFirst,add first)
BUTTON(addLast,add last)
BUTTON(addAfter,add after group)
BUTTON(addUnsub,add unsubscribed)

BUTTON(ngQuit,quit)
BUTTON(ngRead,read group)
BUTTON(ngNext,next)
BUTTON(ngPrev,prev)
BUTTON(ngCatchUp,catch up)
BUTTON(ngSubscribe,subscribe)
BUTTON(ngUnsub,unsubscribe)
BUTTON(ngGoto,goto newsgroup)
BUTTON(ngAllGroups,all groups)
BUTTON(ngRescan,rescan)
BUTTON(ngPrevGroup,prev group)
BUTTON(ngSelect,select groups)
BUTTON(ngMove,move)
BUTTON(ngExit,exit)
BUTTON(ngCheckPoint,checkpoint)
BUTTON(ngPost,post)
BUTTON(ngGripe,gripe)

BUTTON(allQuit,quit)
BUTTON(allSub,subscribe)
BUTTON(allFirst,subscribe first)
BUTTON(allLast,subscribe last)
BUTTON(allAfter,subscribe after group)
BUTTON(allUnsub,unsubscribe)
BUTTON(allGoto,goto group)
BUTTON(allSelect,select groups)
BUTTON(allMove,move)
BUTTON(allToggle,toggle order)

BUTTON(artQuit,quit)
BUTTON(artNext,next)
#ifdef XRN_TEXT_WIDGET
BUTTON(artScroll,scroll)
#endif
BUTTON(artNextUnread,next unread)
BUTTON(artPrev,prev)
BUTTON(artLast,last)
BUTTON(artNextGroup,next newsgroup)
BUTTON(artGotoArticle,goto article)
BUTTON(artCatchUp,catch up)
BUTTON(artMarkRead,mark read)
BUTTON(artMarkUnread,mark unread)
BUTTON(artUnsub,unsubscribe)
BUTTON(artSubNext,subject next)
BUTTON(artSubPrev,subject prev)
BUTTON(artKillSession,session kill)
BUTTON(artKillLocal,local kill)
BUTTON(artKillGlobal,global kill)
BUTTON(artKillAuthor,author kill)
BUTTON(artSubSearch,subject search)
BUTTON(artContinue,continue)
BUTTON(artPost,post)
BUTTON(artExit,exit)
BUTTON(artGripe,gripe)

BUTTON(artSave,save)
BUTTON(artReply,reply)
BUTTON(artForward,forward)
BUTTON(artFollowup,followup)
BUTTON(artCancel,cancel)
BUTTON(artRot13,rot-13)
BUTTON(artHeader,toggle header)
BUTTON(artPrint,print)


typedef struct buttonList {
    Arg *buttonArgs;
    unsigned int size;
    char *message;
} ButtonList;

static ButtonList AddButtonList[] = {
#ifdef lint
    {NULL, NULL}
#else
    {addQuitArgs, XtNumber(addQuitArgs),
   "Quit add mode, unsubscribe all remaining groups"},
    {addFirstArgs, XtNumber(addFirstArgs),
   "Add the selected group(s) to the beginning of the .newsrc file"},
    {addLastArgs, XtNumber(addLastArgs),
   "Add the selected group(s) to the end of the .newsrc file"},
    {addAfterArgs, XtNumber(addAfterArgs),
   "Add the selected group(s) after a particular group in the .newsrc file"},
    {addUnsubArgs, XtNumber(addUnsubArgs),
   "Add the selected group(s) as unsubscribed"},
#endif /* lint */
};

static int AddButtonListCount = XtNumber(AddButtonList);

static ButtonList NgButtonList[] = {
#ifdef lint
    {NULL, NULL}
#else
    {ngQuitArgs, XtNumber(ngQuitArgs),
   "Quit XRN"},
    {ngReadArgs, XtNumber(ngReadArgs),
   "Read the articles in the current group"},
    {ngNextArgs, XtNumber(ngNextArgs),
   "Move the cursor to the next group"},
    {ngPrevArgs, XtNumber(ngPrevArgs),
   "Move the cursor to the previous group"},
    {ngCatchUpArgs, XtNumber(ngCatchUpArgs),
   "Mark all articles in the current group as read"},
    {ngSubscribeArgs, XtNumber(ngSubscribeArgs),
   "Subscribe to a group"},
    {ngUnsubArgs, XtNumber(ngUnsubArgs),
   "Unsubscribe to the current group"},
    {ngGotoArgs, XtNumber(ngGotoArgs),
   "Go to a newsgroup"},
    {ngAllGroupsArgs, XtNumber(ngAllGroupsArgs),
   "View all available groups, with option to subscribe"},
    {ngRescanArgs, XtNumber(ngRescanArgs),
   "Query the news server for new articles and groups"},
    {ngPrevGroupArgs, XtNumber(ngPrevGroupArgs),
   "Return to the group just visited"},
    {ngSelectArgs, XtNumber(ngSelectArgs),
   "Select groups to be moved"},
    {ngMoveArgs, XtNumber(ngMoveArgs),
   "Move previously selected groups in front of the current selection"},
    {ngExitArgs, XtNumber(ngExitArgs),
   "Quit XRN, leaving the .newsrc file unchanged since the last rescan"},
    {ngCheckPointArgs, XtNumber(ngCheckPointArgs),
   "Update the .newsrc file"},
    {ngGripeArgs, XtNumber(ngGripeArgs),
   "Mail a gripe to the XRN maintainers"},
    {ngPostArgs, XtNumber(ngPostArgs),
   "Post an article to a newsgroup"},
#endif /* lint */
};

static int NgButtonListCount = XtNumber(NgButtonList);

static ButtonList AllButtonList[] = {
#ifdef lint
    {NULL, NULL}
#else
    {allQuitArgs, XtNumber(allQuitArgs),
   "Return to newsgroup mode, saving changes"},
    {allSubArgs, XtNumber(allSubArgs),
   "Subscribe to current group, leaveing .newsrc position unchanged"},
    {allFirstArgs, XtNumber(allFirstArgs),
   "Subscribe to the selected group(s); put at the beginning of the .newsrc file"},
    {allLastArgs, XtNumber(allLastArgs),
   "Subscribe to the selected group(s); put at the end of the .newsrc file"},
    {allAfterArgs, XtNumber(allAfterArgs),
   "Subscribe to the selected group(s); put after a specified group in the .newsrc file"},
    {allUnsubArgs, XtNumber(allUnsubArgs),
   "Unsubscribe to the selected group(s)"},
    {allGotoArgs, XtNumber(allGotoArgs),
   "Go to the current newsgroup"},
    {allSelectArgs, XtNumber(allSelectArgs),
   "Select groups to be moved"},
    {allMoveArgs, XtNumber(allMoveArgs),
   "Move previously selected groups to the current position"},
    {allToggleArgs, XtNumber(allToggleArgs),
   "Change order of groups: alphabetical/.newsrc order"},
#endif /* lint */
};

static int AllButtonListCount = XtNumber(AllButtonList);

static ButtonList ArtButtonList[] = {
#ifdef lint
    {NULL, NULL}
#else
    {artQuitArgs, XtNumber(artQuitArgs),
   "Return to newsgroup mode"},
    {artNextUnreadArgs, XtNumber(artNextUnreadArgs),
   "Read the next unread article"},
    {artNextArgs, XtNumber(artNextArgs),
   "Read the next article"},
#ifdef XRN_TEXT_WIDGET
    {artScrollArgs, XtNumber(artScrollArgs),
   "Scroll current article"},
#endif
    {artPrevArgs, XtNumber(artPrevArgs),
   "Read the previous article"},
    {artLastArgs, XtNumber(artLastArgs),
   "Go back to the last article displayed"},
    {artNextGroupArgs, XtNumber(artNextGroupArgs),
   "Go to the next newsgroup, skipping newsgroup mode"},
    {artCatchUpArgs, XtNumber(artCatchUpArgs),
   "Mark all articles (up to the selected article) in the current group as read"},
    {artGotoArticleArgs, XtNumber(artGotoArticleArgs),
   "Go to the specified article number in the current group"},
    {artMarkReadArgs, XtNumber(artMarkReadArgs),
   "Mark selected article(s) as read"},
    {artMarkUnreadArgs, XtNumber(artMarkUnreadArgs),
   "Mark selected article(s) as unread"},
    {artUnsubArgs, XtNumber(artUnsubArgs),
   "Unsubscribe to the current group"},
    {artSubNextArgs, XtNumber(artSubNextArgs),
   "Search for the next article with the selected subject"},
    {artSubPrevArgs, XtNumber(artSubPrevArgs),
   "Search for the previous article with the selected subject"},
    {artKillSessionArgs, XtNumber(artKillSessionArgs),
   "Mark all articles with this subject as read, this session only"},
    {artKillLocalArgs, XtNumber(artKillLocalArgs),
   "Mark all articles with this subject as read in this group for this and all future sessions"},
    {artKillGlobalArgs, XtNumber(artKillGlobalArgs),
   "Mark all articles with this subject as read for all groups for this and all future sessions"},
    {artKillAuthorArgs, XtNumber(artKillAuthorArgs),
   "Mark all articles with this author as read, this session only"},
    {artSubSearchArgs, XtNumber(artSubSearchArgs),
   "Search the subject lines for a regular expression"},
    {artContinueArgs, XtNumber(artContinueArgs),
   "Continue the regular expression subject search"},
    {artPostArgs, XtNumber(artPostArgs),
   "Post an article to this newsgroup"},
    {artExitArgs, XtNumber(artExitArgs),
   "Return to newsgroup mode, marking all articles as unread"},
    {artGripeArgs, XtNumber(artGripeArgs),
   "Mail a gripe to the XRN maintainers"},
#endif /* lint */
};

static int ArtButtonListCount = XtNumber(ArtButtonList);

static ButtonList ArtSpecButtonList[] = {
#ifdef lint
    {NULL, NULL}
#else
    {artSaveArgs, XtNumber(artSaveArgs),
   "Save the current article in a file"},
    {artReplyArgs, XtNumber(artReplyArgs),
   "Mail a reply to the author of the current article"},
    {artForwardArgs, XtNumber(artForwardArgs),
   "Forward an article to a user(s)"},
    {artFollowupArgs, XtNumber(artFollowupArgs),
   "Post a followup to the current article"},
    {artCancelArgs, XtNumber(artCancelArgs),
   "Cancel the current article"},
    {artRot13Args, XtNumber(artRot13Args),
   "Decrypt an encrypted joke"},
    {artHeaderArgs, XtNumber(artHeaderArgs),
   "Display the complete/stripped header"},
    {artPrintArgs, XtNumber(artPrintArgs),
   "Print the article"},
#endif /* lint */
};

static int ArtSpecButtonListCount = XtNumber(ArtSpecButtonList);

static char TopNonButtonInfo[LABEL_SIZE];
static char BottomNonButtonInfo[LABEL_SIZE];


/*ARGSUSED*/
void
topInfoHandler(widget, client_data, event)
Widget widget;
caddr_t client_data;
XEvent *event;
/*
 * handle the Enter and Leave events for the buttons
 *
 * upon entering a button, get it's info string and put in the Question label
 * upon leaving a button, restore the old info string
 *
 */
{
    Arg infoLineArg[1];

    if (event->type == LeaveNotify) {
	XtSetArg(infoLineArg[0], XtNlabel, TopNonButtonInfo);
    } else if (event->type == EnterNotify) {
	XtSetArg(infoLineArg[0], XtNlabel, client_data);
    }
    XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
    return;
}


/*ARGSUSED*/
void
bottomInfoHandler(widget, client_data, event)
Widget widget;
caddr_t client_data;
XEvent *event;
/*
 * handle the Enter and Leave events for the buttons
 *
 * upon entering a button, get it's info string and put in the Question label
 * upon leaving a button, restore the old info string
 *
 */
{
    Arg infoLineArg[1];

    if (event->type == LeaveNotify) {
	XtSetArg(infoLineArg[0], XtNlabel, BottomNonButtonInfo);
    } else if (event->type == EnterNotify) {
	XtSetArg(infoLineArg[0], XtNlabel, client_data);
    }
    XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
    return;
}


static void
resetSelection()
/*
 * Reset First and Last to zero, so the user doesn't accidentally
 * move groups
 */
{
    First = 0;
    Last = 0;
    
    return;
}


static void
setTopInfoLineHandler(widget, message)
Widget widget;
char *message;
{
    XtAddEventHandler(widget,
		      (EventMask) (EnterWindowMask|LeaveWindowMask),
		      False,
		      topInfoHandler,
		      (caddr_t) message);
    return;
}


static void
setBottomInfoLineHandler(widget, message)
Widget widget;
char *message;
{
    XtAddEventHandler(widget,
		      (EventMask) (EnterWindowMask|LeaveWindowMask),
		      False,
		      bottomInfoHandler,
		      (caddr_t) message);
    return;
}


static void
setTopInfoLine(message)  
char *message;
{
    Arg infoLineArg[1];

    XtSetArg(infoLineArg[0], XtNlabel, message);
    (void) strcpy(TopNonButtonInfo, (char *) infoLineArg[0].value);
    XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
    return;
}


static void
setBottomInfoLine(message)  
char *message;
{
    Arg infoLineArg[1];

    XtSetArg(infoLineArg[0], XtNlabel, message);
    (void) strcpy(BottomNonButtonInfo, (char *) infoLineArg[0].value);
    XtSetValues(BottomInfoLine, infoLineArg, XtNumber(infoLineArg));
    return;
}

#define TOP	0
#define BOTTOM	1

static void
doButtons(resource, box, buttons, buttonList, size, infoLine)
char *resource;
Widget box;
Widget *buttons;
ButtonList *buttonList;
int *size;
int infoLine;
{
    char *ptr, *token;
    int j, i = 0;

    if (resource) {
	ptr = resource;

	while ((token = strtok(ptr, ", \t\n")) != NIL(char)) {
	    /* find name */
	    for (j = 0; j < *size; j++) {
		if (STREQ(token, (char *) buttonList[j].buttonArgs[0].value)) {
		    buttons[i] = XtCreateWidget((char *) buttonList[j].buttonArgs[0].value,
						  commandWidgetClass,
						  box,
						  buttonList[j].buttonArgs,
						  buttonList[j].size);
		    if (infoLine == TOP) {
			setTopInfoLineHandler(buttons[i], buttonList[j].message);
		    } else {
			setBottomInfoLineHandler(buttons[i], buttonList[j].message);
		    }
		    i++;
		    break;
		}
	    }
	    if (j == *size) {
		mesgPane("XRN error: bad button name: %s", token);
	    }
	    ptr = NIL(char);
	}
	*size = i;
	
    } else {
	for (i = 0; i < *size; i++) {
	    
	    buttons[i] = XtCreateWidget((char *) buttonList[i].buttonArgs[0].value,
					  commandWidgetClass,
					  box,
					  buttonList[i].buttonArgs,
					  buttonList[i].size);
	    if (infoLine == TOP) {
		setTopInfoLineHandler(buttons[i], buttonList[i].message);
	    } else {
		setBottomInfoLineHandler(buttons[i], buttonList[i].message);
	    }
	}
    }
    return;
}


static void
createButtons()  
{
#define SETTRANSLATIONS(w, index, mode, bind) \
    Translations[index].widget = w; \
    Translations[index].unparsed[mode] = bind;

    SETTRANSLATIONS(TopButtonBox, 0, ADD_MODE, app_resources.addBindings);
    SETTRANSLATIONS(BottomButtonBox, 1, ADD_MODE, app_resources.addBindings);
    SETTRANSLATIONS(TopInfoLine, 2, ADD_MODE, app_resources.addBindings);
    SETTRANSLATIONS(BottomInfoLine, 3, ADD_MODE, app_resources.addBindings);
    SETTRANSLATIONS(Text, 4, ADD_MODE, app_resources.addBindings);
    SETTRANSLATIONS(ArticleText, 5, ADD_MODE, app_resources.addBindings);

    SETTRANSLATIONS(TopButtonBox, 0, ALL_MODE, app_resources.allBindings);
    SETTRANSLATIONS(BottomButtonBox, 1, ALL_MODE, app_resources.allBindings);
    SETTRANSLATIONS(TopInfoLine, 2, ALL_MODE, app_resources.allBindings);
    SETTRANSLATIONS(BottomInfoLine, 3, ALL_MODE, app_resources.allBindings);
    SETTRANSLATIONS(Text, 4, ALL_MODE, app_resources.allBindings);
    SETTRANSLATIONS(ArticleText, 5, ALL_MODE, app_resources.allBindings);

    SETTRANSLATIONS(TopButtonBox, 0, NEWSGROUP_MODE, app_resources.ngBindings);
    SETTRANSLATIONS(BottomButtonBox, 1, NEWSGROUP_MODE, app_resources.ngBindings);
    SETTRANSLATIONS(TopInfoLine, 2, NEWSGROUP_MODE, app_resources.ngBindings);
    SETTRANSLATIONS(BottomInfoLine, 3, NEWSGROUP_MODE, app_resources.ngBindings);
    SETTRANSLATIONS(Text, 4, NEWSGROUP_MODE, app_resources.ngBindings);
    SETTRANSLATIONS(ArticleText, 5, NEWSGROUP_MODE, app_resources.ngBindings);

    SETTRANSLATIONS(TopButtonBox, 0, ARTICLE_MODE, app_resources.artBindings);
    SETTRANSLATIONS(BottomButtonBox, 1, ARTICLE_MODE, app_resources.artBindings);
    SETTRANSLATIONS(TopInfoLine, 2, ARTICLE_MODE, app_resources.artBindings);
    SETTRANSLATIONS(BottomInfoLine, 3, ARTICLE_MODE, app_resources.artBindings);
    SETTRANSLATIONS(Text, 4, ARTICLE_MODE, app_resources.artBindings);
    SETTRANSLATIONS(ArticleText, 5, ARTICLE_MODE, app_resources.artBindings);

    XtAddActions(TopActions, XtNumber(TopActions));
    
    AddButtons = ARRAYALLOC(Widget, XtNumber(AddButtonList));
    XtAddActions(AddActions, XtNumber(AddActions));

    doButtons(app_resources.addButtonList, TopButtonBox, AddButtons, AddButtonList, &AddButtonListCount, TOP);

    NgButtons = ARRAYALLOC(Widget, XtNumber(NgButtonList));
    XtAddActions(NgActions, XtNumber(NgActions));
    
#ifndef NODOUBLECLICK
    if (NgClick == (XtTranslations) 0) {
	NgClick = XtParseTranslationTable(NgClickTable);
    }
#endif

    doButtons(app_resources.ngButtonList, TopButtonBox, NgButtons, NgButtonList, &NgButtonListCount, TOP);

    AllButtons = ARRAYALLOC(Widget, XtNumber(AllButtonList));
    XtAddActions(AllActions, XtNumber(AllActions));
    
    doButtons(app_resources.allButtonList, BottomButtonBox, AllButtons, AllButtonList, &AllButtonListCount, BOTTOM);
    
    ArtButtons = ARRAYALLOC(Widget, XtNumber(ArtButtonList));
    XtAddActions(ArtActions, XtNumber(ArtActions));
    
#ifndef NODOUBLECLICK
    if (ArtClick == (XtTranslations) 0) {
	ArtClick = XtParseTranslationTable(ArtClickTable);
    }
#endif
    
    doButtons(app_resources.artButtonList, TopButtonBox, ArtButtons, ArtButtonList, &ArtButtonListCount, TOP);
    
    ArtSpecButtons = ARRAYALLOC(Widget, XtNumber(ArtSpecButtonList));
    
    doButtons(app_resources.artSpecButtonList, BottomButtonBox, ArtSpecButtons, ArtSpecButtonList, &ArtSpecButtonListCount, BOTTOM);

    return;
}


static void
setTranslations(mode)
int mode;
{
    Arg args[1];
    static int init[MAX_MODE] = {0,0,0,0};
    int i;

    if (!init[mode]) {
	/*
	 * first time:
	 *   parse table
	 *   override
	 *   get table back and store
	 */
	for (i = 0; i < sizeof(Translations) / sizeof(struct _translations); i++) {
	    XtTranslations table;

	    if (Translations[i].unparsed[mode] == NIL(char)) {
		table = 0;
	    } else {
		table = XtParseTranslationTable(Translations[i].unparsed[mode]);
	    }
#ifdef TRANSLATIONBUG
	    XtSetArg(args[0], XtNtranslations, 0);
	    XtGetValues(Translations[i].widget, args, XtNumber(args));
	    if (XtTranslations) args[0].value == (XtTranslations) 0) {
		if (table) {
		    XtSetArg(args[0], XtNtranslations, table);
		    XtSetValues(Translations[i].widget, args, XtNumber(args));
		}
	    }
#endif TRANSLATIONBUG
	    if (table) {
		XtOverrideTranslations(Translations[i].widget, table);
	    }
	    XtSetArg(args[0], XtNtranslations, 0);
	    XtGetValues(Translations[i].widget, args, XtNumber(args));
	    Translations[i].tables[mode] = (XtTranslations) args[0].value;
	}
	init[mode] = 1;
    } else {
	/*
	 * second and future times:
	 *   install translations
	 */
	for (i = 0; i < sizeof(Translations) / sizeof(struct _translations); i++) {
	    XtSetArg(args[0], XtNtranslations, Translations[i].tables[mode]);
	    XtSetValues(Translations[i].widget, args, XtNumber(args));
	}
    }
    return;
}


static void
swapMode()
/*
 * change the buttons displayed in the TopButtonBox (switch modes)
 */
{
    if (PreviousMode == Mode) {
	return;
    }
    
    /*
     * don't resize on the deletion, if the box is the same
     * size after adding the new buttons, don't want the extra
     * text window redraw
     *
     * if the box does change size, we are left with the double
     * draw of the text window
     *
     */
    XawPanedAllowResize(TopButtonBox, False);
    XawPanedAllowResize(BottomButtonBox, False);

    /*
     * NONE -> ADD
     *    manage add in top box
     *    manage art in bottom box
     *    desensitize bottom box
     *    install add actions in top box
     */
    if ((PreviousMode == NO_MODE) && (Mode == ADD_MODE)) {
	XtManageChildren(AddButtons, AddButtonListCount);
	XtManageChildren(ArtSpecButtons, ArtSpecButtonListCount);
	XtSetSensitive(BottomButtonBox, False);
	setTranslations(ADD_MODE);
    /*    
     * NONE -> NG
     *    manage ng in top box
     *    manage art in bottom box
     *    desensitize bottom box
     *    install ng actions in top box
     */
    } else if ((PreviousMode == NO_MODE) && (Mode == NEWSGROUP_MODE)) {
	XtManageChildren(NgButtons, NgButtonListCount);
	XtManageChildren(ArtSpecButtons, ArtSpecButtonListCount);
	XtSetSensitive(BottomButtonBox, False);
	setTranslations(NEWSGROUP_MODE);

#ifndef NODOUBLECLICK
	XtOverrideTranslations(Text, NgClick);
#endif
    /*
     * ADD -> NG
     *    unmanage add in top box
     *    manage ng in top box
     *    install ng actions in top box
     */
    } else if ((PreviousMode == ADD_MODE) && (Mode == NEWSGROUP_MODE)) {
	XtUnmanageChildren(AddButtons, AddButtonListCount);
	XtManageChildren(NgButtons, NgButtonListCount);
	setTranslations(NEWSGROUP_MODE);

#ifndef NODOUBLECLICK
	XtOverrideTranslations(Text, NgClick);
#endif
    /*
     * NG -> ART
     *    unmanage ng in top box
     *    manage art in top box
     *    sensitize bottom box
     *    install art actions in top box
     *    install art actions in bottom box
     */
    } else if ((PreviousMode == NEWSGROUP_MODE) && (Mode == ARTICLE_MODE)) {
	XtUnmanageChildren(NgButtons, NgButtonListCount);
	XtManageChildren(ArtButtons, ArtButtonListCount);
	XtSetSensitive(BottomButtonBox, True);
	setTranslations(ARTICLE_MODE);

#ifndef NODOUBLECLICK
	XtOverrideTranslations(Text, ArtClick);
#endif
    /*
     * NG -> ADD
     *    unmanage ng in top box
     *    manage add in top box
     *    install add actions in top box
     */
    } else if ((PreviousMode == NEWSGROUP_MODE) && (Mode == ADD_MODE)) {
	XtUnmanageChildren(NgButtons, NgButtonListCount);
	XtManageChildren(AddButtons, AddButtonListCount);
	setTranslations(ADD_MODE);
    /*
     * NG -> ALL
     *    desensitize top box
     *    unmanage art in bottom box
     *    manage all in bottom box
     *    sensitize bottom box
     *    install all actions in bottom box
     */
    } else if ((PreviousMode == NEWSGROUP_MODE) && (Mode == ALL_MODE)) {
	XtSetSensitive(TopButtonBox, False);
	XtUnmanageChildren(ArtSpecButtons, ArtSpecButtonListCount);
	XtManageChildren(AllButtons, AllButtonListCount);
	XtSetSensitive(BottomButtonBox, True);
	setTranslations(ALL_MODE);
    /*     
     * ART -> NG
     *    desensitize bottom box
     *    unmanage art in top box
     *    manage ng in top box
     *    install ng actions in top box
     */
    } else if ((PreviousMode == ARTICLE_MODE) && (Mode == NEWSGROUP_MODE)) {
	XtSetSensitive(BottomButtonBox, False);
	XtUnmanageChildren(ArtButtons, ArtButtonListCount);
	XtManageChildren(NgButtons, NgButtonListCount);
	setTranslations(NEWSGROUP_MODE);

#ifndef NODOUBLECLICK
	XtOverrideTranslations(Text, NgClick);
#endif
    /*
     * ALL -> NG
     *    sensitize top box
     *    unmanage all in bottom box
     *    manage art in bottom box
     *    desensitize bottom box
     */
    } else if ((PreviousMode == ALL_MODE) && (Mode == NEWSGROUP_MODE)) {
	XtSetSensitive(TopButtonBox, True);
	XtUnmanageChildren(AllButtons, AllButtonListCount);
	XtManageChildren(ArtSpecButtons, ArtSpecButtonListCount);
	XtSetSensitive(BottomButtonBox, False);
	setTranslations(NEWSGROUP_MODE);

#ifndef NODOUBLECLICK
	XtOverrideTranslations(Text, NgClick);
#endif
    /*	
     * ALL -> ART
     *    unmanage ng in top box
     *    sensitize top box
     *    manage art in top box
     *    unmanage all in bottom box
     *    manage art in bottom box
     *    install art actions in bottom box
     */
    } else if ((PreviousMode == ALL_MODE) && (Mode == ARTICLE_MODE)) {
	XtSetSensitive(TopButtonBox, True);
	XtUnmanageChildren(NgButtons, NgButtonListCount);
	XtManageChildren(ArtButtons, ArtButtonListCount);
	XtUnmanageChildren(AllButtons, AllButtonListCount);
	XtManageChildren(ArtSpecButtons, ArtSpecButtonListCount);
	setTranslations(ARTICLE_MODE);

#ifndef NODOUBLECLICK
	XtOverrideTranslations(Text, ArtClick);
#endif
    } else {
	(void) sprintf(error_buffer, "unsupported transition: %d to %d\n",
			       PreviousMode, Mode);
	ehErrorExitXRN(error_buffer);
    }
    
    XawPanedAllowResize(TopButtonBox, True);
    XawPanedAllowResize(BottomButtonBox, True);
    return;
}


static int XRNAbort = 0;

int
abortP()
{
    return XRNAbort;
}

void
abortSet()
{
    XRNAbort = 1;
}

void
abortClear()
{
    XRNAbort = 0;
}


/*ARGSUSED*/
static void
doTheRightThing(widget, event, string, count)
Widget widget;
XEvent *event;
String *string;
Cardinal count;
{
    busyCursor(widget, 0, 0);
    switch (Mode) {
	case ALL_MODE:
#ifdef XRN_TEXT_WIDGET
	XtScrollDownOnePage(ArticleText);
#endif
	break;

	case NEWSGROUP_MODE:
	ngReadFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);
	break;

	case ARTICLE_MODE:
#ifdef XRN_TEXT_WIDGET
	if (!app_resources.pageArticles || XawTextLastPage(ArticleText)) {
	    if (app_resources.subjectRead == False) {
		artNextUnreadFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);
	    } else {
		artSubNextFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);
	    }
	} else {
	    XtScrollDownOnePage(ArticleText);
	}
#else
	if (app_resources.subjectRead == False) {
	    artNextUnreadFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);
	} else {
	    artSubNextFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);
	}
#endif
	break;
    }
    unbusyCursor(widget, 0, 0);
    return;
}


void
updateSubjectWidget(left, right)
XawTextPosition left, right;
/*
 *  Redraw the text between left and right in the subject window;
 *  Adjust the window so the cursor is between min and max lines.
 */
{
    XawTextPosition currentPos;
    Arg arg[1], sargs[3];
    int numLines, count;

    if (SubjectSource != (XawTextSource) 0) {
	currentPos = ArticleTop = XawTextTopPosition(Text);
	XawTextInvalidate(Text, left - 1, right + 1);
	if ((app_resources.minLines >= 0) && (app_resources.maxLines >= 0)) {
	    if (currentPos <= ArtPosition) {
		for (numLines = 1; currentPos < ArtPosition; numLines++) {
		    if (!moveCursor(FORWARD, SubjectString, &currentPos)) {
			break;
		    }
		}
	    } else {
		numLines = 1;
		currentPos = ArtPosition;
	    }
	    if (numLines > app_resources.maxLines) {
		for (count = 1; count < app_resources.minLines; count++) {
		    if (!moveCursor(BACK, SubjectString, &currentPos)) {
			break;
		    }
		}
		XtSetArg(arg[0], XtNdisplayPosition, currentPos);
		XtSetValues(Text, arg, XtNumber(arg));
	    } else if (numLines < app_resources.minLines) {
		for (count = 1; count < app_resources.maxLines; count++) {
		    if (!moveCursor(BACK, SubjectString, &currentPos)) {
			break;
		    }
		}
		XtSetArg(arg[0], XtNdisplayPosition, currentPos);
		XtSetValues(Text, arg, XtNumber(arg));
	    }
	}
    } else {
	ArticleTop = (XawTextPosition) 0;
	XtSetArg(sargs[0], XtNstring, SubjectString);
	XtSetArg(sargs[1], XtNlength, utStrlen(SubjectString) + 1);
	XtSetArg(sargs[2], XtNeditType, XawtextRead);
	SubjectSource = XawStringSourceCreate(Text, sargs, XtNumber(sargs));
	XawTextSetSource(Text, SubjectSource, (XawTextPosition) ArticleTop);
    }
    XawTextSetInsertionPoint(Text, ArtPosition);
    
    return;
}


static int
getNearbyArticle(status, filename, question, artNum)
int status;
char **filename, **question;
long *artNum;
/*
 * Get the nearest article to the cursor.  If there is no article on the
 * current line, search forward or backwards for a valid article, depending
 * on the value of status.  Return the filename and question of the
 * article obtained.
 */
{
    XawTextPosition beginning;
    
    if (status == art_PREV) {
	if (SubjectString[ArtPosition] == '\0') {
	    if (ArtPosition == 0) {
		/* no articles remain, jump out of article mode */
		return art_DONE;
	    }
	    if (!moveCursor(BACK, SubjectString, &ArtPosition)) {
		return art_DONE;
	    }
	}
	*artNum = atol(&SubjectString[ArtPosition+2]);
	gotoArticle(*artNum);
	while (getArticle(filename, question) != XRN_OKAY) {
	    removeLine(SubjectString, Text, &SubjectSource, ArtPosition,
		       &ArticleTop);
	    if (!moveCursor(BACK, SubjectString, &ArtPosition)) {
		return art_DONE;
	    }
	    *artNum = atol(&SubjectString[ArtPosition+2]);
	    gotoArticle(*artNum);
	}
	return art_CHANGE;
    }

    if (status == art_NEXT) {
	if (SubjectString[ArtPosition] == '\0') {
	    return art_DONE;
	}
	*artNum = atol(&SubjectString[ArtPosition+2]);
	gotoArticle(*artNum);
	while (getArticle(filename, question) != XRN_OKAY) {
	    removeLine(SubjectString, Text, &SubjectSource, ArtPosition,
		       &ArticleTop);
	    if (SubjectString[ArtPosition] == '\0') {
		return art_DONE;
	    }
	    *artNum = atol(&SubjectString[ArtPosition+2]);
	    gotoArticle(*artNum); 
	}
	return art_CHANGE;
    }
    if (status == art_NEXTWRAP) {
	if (SubjectString[ArtPosition] == '\0') {
	    if (ArtPosition == 0) {
		return art_DONE;
	    }
	    ArtPosition = 0;
	}
	*artNum = atol(&SubjectString[ArtPosition+2]);
	gotoArticle(*artNum);
	while (getArticle(filename, question) != XRN_OKAY) {
	    removeLine(SubjectString, Text, &SubjectSource, ArtPosition,
		       &ArticleTop);
	    if (SubjectString[ArtPosition] == '\0') {
		if (ArtPosition == 0) {
		    return art_DONE;
		}
		ArtPosition = 0;
	    }
	    *artNum = atol(&SubjectString[ArtPosition+2]);
	    gotoArticle(*artNum); 
	}
	return art_CHANGE;
    }
    if (status == art_UNREAD) {
	if (SubjectString[ArtPosition] == '\0') {
	    if (ArtPosition == 0) {
		return art_DONE;
	    }
	    ArtPosition = 0;
	}
	beginning = ArtPosition;
	if (SubjectString[ArtPosition] != ' ') {
	    (void) moveUpWrap(SubjectString, &ArtPosition);
	    while ((SubjectString[ArtPosition] != ' ') &&
		   (ArtPosition != beginning)) {
		if (!moveUpWrap(SubjectString, &ArtPosition)) {
		    return art_DONE;
		}
	    }
	    if (ArtPosition == beginning) {
		return art_DONE;
	    }
	}
	/* we are at an unread article */
	*artNum = atol(&SubjectString[ArtPosition+2]);
	gotoArticle(*artNum);
	while (getArticle(filename, question) != XRN_OKAY) {
	    removeLine(SubjectString, Text, &SubjectSource, ArtPosition,
		       &ArticleTop);
	    if (SubjectString[ArtPosition] == '\0') {
		if (ArtPosition == 0) {
		    return art_DONE;
		}
		ArtPosition = 0;
	    }
	    while ((SubjectString[ArtPosition] != ' ') &&
		   (ArtPosition != beginning)) {
		if (!moveUpWrap(SubjectString, &ArtPosition)) {
		    return art_DONE;
		}
	    }
	    if (ArtPosition == beginning) {
		return art_DONE;
	    }
	    *artNum = atol(&SubjectString[ArtPosition+2]);
	    gotoArticle(*artNum); 
	}
	return art_CHANGE;
    }
    return art_CHANGE;
}


#define CHANGE 0		/* subject window has changed */
#define NOCHANGE 1		/* subject window has not changed */
#define DONE 2			/* no new article was found */
				/* EXIT is already defined, it implies */
				/* there are no articles left at all */

static int
isPrevSubject(subject, filename, question, artNum)
char *subject;
char **filename, **question;
long *artNum;
/*
 *
 */
{
    char *newsubject;
    char *newLine;
    char *newSubjectString;
    char *newString;
    char *oldString;
    XawTextPosition save;

    oldString = NIL(char);
    save = ArtPosition;
    startSearch();
    abortClear();
    
    for (;;) {
	if (abortP()) {
	    failedSearch();
	    ArtPosition = save;
	    return ABORT;
	}
	if (SubjectString[ArtPosition] == '\0') {
	    return EXIT;
	}
	if (ArtPosition != 0) {
	    (void) moveCursor(BACK, SubjectString, &ArtPosition);
	    *artNum = atol(&SubjectString[ArtPosition+2]);
	    newsubject = getSubject(*artNum);
	    if (utSubjectCompare(newsubject, subject) == 0) {
		gotoArticle(*artNum);
		if (getArticle(filename, question) != XRN_OKAY) {
		    removeLine(SubjectString, Text, &SubjectSource, ArtPosition,
			       &ArticleTop);
		    continue;
		}
		if (SubjectString[ArtPosition] == 'u') {
		    markArticleAsUnread(*artNum);
		}
		return NOCHANGE;
	    }
	    continue;
	} else {
	    if ((newLine = getPrevSubject()) == NIL(char)) {
		failedSearch();
		ArtPosition = save;
		return DONE;
	    }
	    newLine[0] = '+';
	    *artNum = atol(&newLine[2]);
	    newsubject = getSubject(*artNum);
	    if (oldString != NIL(char)) {
		newString = ARRAYALLOC(char, (utStrlen(oldString) + utStrlen(newLine) + 2));
		(void) strcpy(newString, newLine);
		(void) strcat(newString, "\n");
		(void) strcat(newString, oldString);
		FREE(oldString);
	    } else {
		newString = ARRAYALLOC(char, (utStrlen(newLine) + 2));
		(void) strcpy(newString, newLine);
		(void) strcat(newString, "\n");
	    }
	    if (utSubjectCompare(newsubject, subject) == 0) {
		/* found a match, go with it */

		newSubjectString = ARRAYALLOC(char, (utStrlen(newString) + utStrlen(SubjectString) + 1));
		(void) strcpy(newSubjectString, newString);
		(void) strcat(newSubjectString, SubjectString);
		FREE(SubjectString);
		SubjectString = newSubjectString;

		gotoArticle(*artNum);
		(void) getArticle(filename, question);
		if (SubjectString[ArtPosition] == 'u') {
		    markArticleAsUnread(*artNum);
		}
		return CHANGE;
	    }
	    oldString = newString;
	    continue;
	}
    }
}


static int
isNextSubject(subject, filename, question, artNum)
char *subject;
char **filename, **question;
long *artNum;
/*
 *
 */
{
    char *newsubject;
    XawTextPosition save = ArtPosition;

    abortClear();
    
    for (;;) {
	if (abortP()) {
	    failedSearch();
	    ArtPosition = save;
	    return ABORT;
	}
	if (SubjectString[ArtPosition] == '\0') {
	    if (ArtPosition == 0) {
		return EXIT;
	    }
	    ArtPosition = save;
	    return DONE;
	}
	*artNum = atol(&SubjectString[ArtPosition+2]);
	newsubject = getSubject(*artNum);
	if (utSubjectCompare(newsubject, subject) == 0) {
	    gotoArticle(*artNum);
	    if (getArticle(filename, question) != XRN_OKAY) {
		removeLine(SubjectString, Text, &SubjectSource, ArtPosition,
			   &ArticleTop);
		continue;
	    }
	    return NOCHANGE;
	} else {
	    if (!moveCursor(FORWARD, SubjectString, &ArtPosition)) {
		return EXIT;
	    }
	}
    }
}


static int
getPrevious(artNum)
XawTextPosition *artNum;
/*
 *
 */
{
    char *newLine;
    char *newString;
    Arg args[3];

    if ((newLine = getPrevSubject()) != NIL(char)) {
	newString = ARRAYALLOC(char, (utStrlen(SubjectString) + utStrlen(newLine) + 2));
	(void) strcpy(newString, newLine);
	(void) strcat(newString, "\n");
	(void) strcat(newString, SubjectString);
	FREE(SubjectString);
	SubjectString = newString;
	ArtPosition = ArticleTop = 0;
	if (SubjectSource != (XawTextSource) 0) {
	    XawStringSourceDestroy(SubjectSource);
	}

	XtSetArg(args[0], XtNstring, SubjectString);
	XtSetArg(args[1], XtNlength, utStrlen(SubjectString) + 1);
	XtSetArg(args[2], XtNeditType, XawtextRead);
	SubjectSource = XawStringSourceCreate(Text, args, XtNumber(args));
	XawTextSetSource(Text, SubjectSource, ArticleTop);
	XawTextSetInsertionPoint(Text, ArtPosition);
	*artNum = atol(&SubjectString[ArtPosition+2]);
	return TRUE;
    }
    
    return FALSE;
}

    
static void
selectedArticle(status)
int status;
/*
 */
{
    XawTextPosition left, right;
    
    XawTextGetSelectionPos(Text, &left, &right);
    XawTextUnsetSelection(Text);
    
    /* nothing was selected if left == right, so get article on NEXT or */
    /* PREVIOUS line */

    if (left == right) {
	ArtPosition = XawTextGetInsertionPoint(Text);

	if (ArtEntry == 1) {
	    return;
	}

	/* get article on NEXT line */

	if ((status == art_NEXT) || (status == art_UNREAD)) {
	    (void) moveCursor(FORWARD, SubjectString, &ArtPosition);
	} else if (status == art_PREV) {
	    (void) moveCursor(BACK, SubjectString, &ArtPosition);
	}
	return;
    }

    /* something was selected */

    /* make sure selection includes only whole groups */
    moveBeginning(SubjectString, &left);
    ArtPosition = left;
    
    return;
}


static void
adjustNewsgroupWidget()
/*
 * Adjust the top position in the newsgroup window such
 * that the cursor stays between min and max lines.
 */
{
    int numLines, count;
    XawTextPosition currentPos;

    moveBeginning(NewsGroupsString, &GroupPosition);
    currentPos = NewsgroupTop;
    if ((app_resources.minLines >= 0) && (app_resources.maxLines >= 0)) {
	if (currentPos <= GroupPosition) {
	    for (numLines = 1; currentPos < GroupPosition; numLines++) {
		if (!moveCursor(FORWARD, NewsGroupsString, &currentPos)) {
		    break;
		}
	    }
	} else {
	    numLines = 1;
	    currentPos = GroupPosition;
	}
	if (numLines > app_resources.maxLines) {
	    for (count = 1; count < app_resources.minLines; count++) {
		if (!moveCursor(BACK, NewsGroupsString, &currentPos)) {
		    break;
		}
	    }
	    NewsgroupTop = currentPos;
	} else if (numLines < app_resources.minLines) {
	    for (count = 1; count < app_resources.maxLines; count++) {
		if (!moveCursor(BACK, NewsGroupsString, &currentPos)) {
		    break;
		}
	    }
	    NewsgroupTop = currentPos;
	}
    }
    return;
}


static void
redrawAllWidget(position)
XawTextPosition position;
/*
 * Redraw the all groups window, assuming it has changed
 */
{
    Arg args[3];
    
    if (Mode != ALL_MODE) {
	return;
    }

    /* free source */
    if (AllSource != (XawTextSource) 0) {
	XawStringSourceDestroy(AllSource);
	AllSource = (XawTextSource) 0;
    }
    
    /* free string */
    if (AllGroupsString != NIL(char)) {
	FREE(AllGroupsString);
	AllGroupsString = NIL(char);
    }
    AllGroupsString = getStatusString(AllStatus);

    XtSetArg(args[0], XtNstring, AllGroupsString);
    XtSetArg(args[1], XtNlength, utStrlen(AllGroupsString) + 1);
    XtSetArg(args[2], XtNeditType, XawtextRead);
    AllSource = XawStringSourceCreate(ArticleText, args, XtNumber(args));
    XawTextSetSource(ArticleText, AllSource, position);
    XtSetValues(ArticleText, lineSelArgs, XtNumber(lineSelArgs));
    
    return;
}


static void
redrawNewsgroupTextWidget()
/*
 * Rebuild the newsgroup text window.
 * Find out what groups have articles to read and build up the string.
 * Create a string source and display it.
 */
{
    static XawTextSource NgSource = (XawTextSource) 0;
    char name[GROUP_NAME_SIZE];
    Arg args[3];

    if (Mode != NEWSGROUP_MODE) {
	return;
    }

    if (NewsGroupsString != NIL(char)) {
	FREE(NewsGroupsString);
    }
    
    NewsGroupsString = unreadGroups();

    /* update the info line */
    if (utStrlen(NewsGroupsString) == 0) {
	setTopInfoLine("No more unread articles in the subscribed to newsgroups");
    }
    if (NgSource != (XawTextSource) 0) {
	XawStringSourceDestroy(NgSource);
    }
    if (NewsGroupsString[GroupPosition] == '\0') {
	GroupPosition = 0;
    }
    currentGroup(Mode, NewsGroupsString, name, GroupPosition);
    if (STREQ(name, LastGroup)) {
	(void) moveUpWrap(NewsGroupsString, &GroupPosition);
    }
    
    adjustNewsgroupWidget();
    XtSetArg(args[0], XtNstring, NewsGroupsString);
    XtSetArg(args[1], XtNlength, utStrlen(NewsGroupsString) + 1);
    XtSetArg(args[2], XtNeditType, XawtextRead);
    NgSource = XawStringSourceCreate(Text, args, XtNumber(args));
    XawTextSetSource(Text, NgSource, (XawTextPosition) NewsgroupTop);
    
    return;
}


static void
updateNewsgroupMode()
/*
 * update the info line and update the newsgroup text window
 */
{
    if (PreviousMode != NEWSGROUP_MODE) {
	setTopInfoLine("Questions apply to current selection or cursor position");
    }
    redrawNewsgroupTextWidget();
    XawTextSetInsertionPoint(Text, GroupPosition);
    return;
}


static void
switchToNewsgroupMode()
/*
 * install the newsgroup mode buttons (and the delete the previous mode buttons)
 * and then go to newsgroup mode
 */
{
    PreviousMode = Mode;
    Mode = NEWSGROUP_MODE;

    resetSelection();

    /* switch buttons */
    swapMode();
    
    /* update the newsgroup mode windows */
    updateNewsgroupMode();
    
    return;
}


static XawTextSelectType allSelectArray[] =
    {XawselectPosition, XawselectChar, XawselectWord, XawselectLine,
     XawselectParagraph, XawselectAll, XawselectNull};


static void
redrawArticleWidget(filename, question)
char *filename, *question;
/*
 * If the article to be displayed has changed, update the article
 * window and redraw the mode line
 */
{
    Arg args[2];

    if (Mode != ARTICLE_MODE) {
	return;
    }

    if (filename != LastArticle) {
	LastArticle = filename;
	
	/* destroy the old text file window */
	if (ArtSource != (XawTextSource) 0) {
	    XawDiskSourceDestroy(ArtSource);
	}

	XtSetArg(args[0], XtNfile, filename);
	XtSetArg(args[1], XtNeditType, XawtextRead);
	ArtSource = XawDiskSourceCreate(ArticleText, args, XtNumber(args));
	
	XawTextSetSource(ArticleText, ArtSource, (XawTextPosition) 0);
	XawTextSetSelectionArray(ArticleText, allSelectArray);
	
	setBottomInfoLine(question);
#ifdef PREFETCH
	/* force the screen to update before prefetching */
	xthHandleAllPendingEvents();
	
	prefetchNextArticle();
	
#endif /* PREFETCH */
    }
    return;
}


static void
switchToArticleMode()
/*
 * install the article mode buttons (and delete the previous mode buttons),
 * build the subject line screen, and call ARTICLE_MODE function 'next unread'
 */
{
    /* change the text window */
    XawTextSetLastPos(Text, (XawTextPosition) 0);

    if (SubjectSource != (XawTextSource) 0) {
	XawStringSourceDestroy(SubjectSource);
	SubjectSource = (XawTextSource) 0;
    }

    if (SubjectString != NIL(char)) {
	FREE(SubjectString);
    }

    SubjectString = getSubjects(UNREAD);

    if ((SubjectString == NIL(char)) || (*SubjectString == '\0')) {
	bogusNewsgroup();
	/*
	 * the sources and strings have been destroyed at this point
	 * have to recreate them - the redraw routines check the mode
	 * so we can call all of them and only the one that is for the
	 * current mode will do something
	 */
	redrawAllWidget((XawTextPosition) 0);
	redrawNewsgroupTextWidget();
	return;
    }

    PreviousMode = Mode;
    Mode = ARTICLE_MODE;

    /* switch buttons */
    swapMode();

    /* get and display the article */
    ArtPosition = 0;
    updateSubjectWidget(0, 0);
    XawTextSetInsertionPoint(Text, 0);	/* source isn't around... */
    ArtEntry = 1;
    artNextUnreadFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);

    return;
}


static void
updateAllWidget(left, right)
XawTextPosition left, right;
/*
 *
 */
{
    XawTextPosition AllTop;
    XawTextPosition current;
    Arg args[3];
    
    if (AllSource != (XawTextSource) 0) {
	AllTop = XawTextTopPosition(ArticleText);
	XawTextInvalidate(ArticleText, left - 1, right + 1);
	current = right+1;
	(void) setCursorCurrent(AllGroupsString, &current);
	XawTextSetInsertionPoint(ArticleText, current);
    } else {
	AllTop = (XawTextPosition) 0;
	XtSetArg(args[0], XtNstring, AllGroupsString);
	XtSetArg(args[1], XtNlength, utStrlen(AllGroupsString) + 1);
	XtSetArg(args[2], XtNeditType, XawtextRead);
	AllSource = XawStringSourceCreate(ArticleText, args, XtNumber(args));
	XawTextSetSource(ArticleText, AllSource, (XawTextPosition) AllTop);
    }
    
    return;
}


static void
exitArticleMode()
/*
 * release the storage associated with article mode, unlink the article files,
 * and go to newsgroup mode
 */
{
    LastArticle = NIL(char);
    PrevArticle = CurrentArticle = 0;
    
    /* release storage and unlink files */
    if (SubjectSource != (XawTextSource) 0) {
	XawStringSourceDestroy(SubjectSource);
	SubjectSource = (XawTextSource) 0;
    }

    /* clear the article window */
    if (ArtSource != (XawTextSource) 0) {
	XawDiskSourceDestroy(ArtSource);
	ArtSource = (XawTextSource) 0;
    }

    /* dummy source - a placeholder until the new sources are installed */
    if (DummySource == (XawTextSource) 0) {
	static Arg args[3] = {
	    {XtNstring, (XtArgVal) ""},
	    {XtNlength, (XtArgVal) 2},
	    {XtNeditType, (XtArgVal) XawtextRead}
	};
	DummySource = XawStringSourceCreate(ArticleText, args, XtNumber(args));
    }
    XawTextSetSource(Text, DummySource, (XawTextPosition) 0);
    XawTextSetSource(ArticleText, DummySource, (XawTextPosition) 0);
    setBottomInfoLine("");
    if (SubjectString != NIL(char)) {
	FREE(SubjectString);
	SubjectString = NIL(char);
    }
    
    releaseNewsgroupResources();
    switchToNewsgroupMode();
    
    return;
}


static XawTextSource AddSource = (XawTextSource) 0;  /* source for the add mode text window */

    
static void
exitAddMode()
/*
 * release storage associated with add mode and go to newsgroup mode
 */
{
    if (AddSource != (XawTextSource) 0) {
	XawStringSourceDestroy(AddSource);
	AddSource = (XawTextSource) 0;
    }

    if (AddGroupsString != NIL(char)) {
	FREE(AddGroupsString);
	AddGroupsString = NIL(char);
    }

    switchToNewsgroupMode();
    
    return;
}


static void
redrawAddTextWidget(insertPoint)
XawTextPosition insertPoint;
/*
 * update the add mode text window to correspond to the new set of groups
 */
{
    Arg args[3];
    
    if (AddSource != (XawTextSource) 0) {
	XawStringSourceDestroy(AddSource);
    }
    (void) setCursorCurrent(AddGroupsString, &insertPoint);

    XtSetArg(args[0], XtNstring, AddGroupsString);
    XtSetArg(args[1], XtNlength, utStrlen(AddGroupsString) + 1);
    XtSetArg(args[2], XtNeditType, XawtextRead);
    
    AddSource = XawStringSourceCreate(Text, args, XtNumber(args));
    XawTextSetSource(Text, AddSource, (XawTextPosition) insertPoint);
    XawTextSetInsertionPoint(Text, insertPoint);
    
    return;
}


void
foundArticle(file, ques, artNum)
char *file, *ques;
long artNum;
/*
 * Display new article, mark as read.
 */
{
    PrevArticle = CurrentArticle;
    if (SubjectString[ArtPosition] == 'u') {
	markArticleAsUnread(artNum);
    } else {
	CurrentArticle = markStringRead(SubjectString, ArtPosition);
    }
    updateSubjectWidget(ArtPosition, ArtPosition);
    XawTextSetInsertionPoint(Text, ArtPosition);
    redrawArticleWidget(file, ques);
    
    return;
}


void
catchUpNG()
/*
 * used when the user has elected to catch
 * up newsgroups in newsgroup mode
 */
{
    char name[GROUP_NAME_SIZE];
    XawTextPosition left, right;
    
    if (getSelection(Text, NewsGroupsString, &left, &right)) {
	GroupPosition = left;
	while (left <= right) {
	    currentGroup(Mode, NewsGroupsString, name, left);
/*	    (void) strcpy(LastGroup, name);*/
	    if (gotoNewsgroup(name) == GOOD_GROUP) {
		catchUp();
	    }
	    if (!moveCursor(FORWARD, NewsGroupsString, &left)) {
		break;
	    }
	}
	updateNewsgroupMode();
	return;
    }
    (void) moveCursor(BACK, NewsGroupsString, &left);
    GroupPosition = left;
    XawTextSetInsertionPoint(Text, GroupPosition);
    
    return;
}


void
unsubscribeNG()
/*
 * Unsubscribe user from selected group(s)
 */
{
    char name[GROUP_NAME_SIZE];
    XawTextPosition left, right;
    
    if (getSelection(Text, NewsGroupsString, &left, &right)) {
	GroupPosition = left;
	while (left <= right) {
	    currentGroup(Mode, NewsGroupsString, name, left);
/*	    (void) strcpy(LastGroup, name);*/
	    if (gotoNewsgroup(name) == GOOD_GROUP) {
		unsubscribe();
	    }
	    if (!moveCursor(FORWARD, NewsGroupsString, &left)) {
		break;
	    }
	}
	updateNewsgroupMode();
	return;
    }
    (void) moveCursor(BACK, NewsGroupsString, &left);
    GroupPosition = left;
    XawTextSetInsertionPoint(Text, GroupPosition);
    
    return;
}


void
catchUpART()
/*
 * Catch up group, and exit article mode
 */
{
    catchUp();
    exitArticleMode();
    return;   
}


void
catchUpPartART()
/*
 * Mark all articles between 0 and ArtPosition as read.
 * Get the next unread article and display it, quit
 * if there are no more unread articles.
 */
{
    char *filename, *question;
    long artNum;
    
    markArticles(SubjectString, (XawTextPosition) 0, ArtPosition, '+');
    updateSubjectWidget((XawTextPosition) 0, ArtPosition);
    if (getNearbyArticle(art_UNREAD, &filename, &question, &artNum) == 
	art_DONE) {
	exitArticleMode();
	return;
    }
    foundArticle(filename, question, artNum);
    
    return;
}

void
unsubscribeART()
/*
 * Unsubscribe user from the current group;
 * exit article mode
 */
{
    unsubscribe();
    exitArticleMode();
    
    return;
}


static Widget ExitConfirmBox = (Widget) 0;
static Widget CatchUpConfirmBox = (Widget) 0;
static Widget PartCatchUpConfirmBox = (Widget) 0;
static Widget UnSubConfirmBox = (Widget) 0;

/*ARGSUSED*/
static void
generalHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    busyCursor(widget, 0, 0);
    switch(Action) {
	case NG_EXIT:
	if ((int) client_data == XRN_YES) {
	    ehNoUpdateExitXRN();
	}
	PopDownDialog(ExitConfirmBox);
	ExitConfirmBox = 0;
	break;

	case NG_QUIT:
	if ((int) client_data == XRN_YES) {
	    ehCleanExitXRN();
	}
	PopDownDialog(ExitConfirmBox);
	ExitConfirmBox = 0;
	break;

	case NG_CATCHUP:
	if ((int) client_data == XRN_YES) {
	    catchUpNG();
	}
	PopDownDialog(CatchUpConfirmBox);
	CatchUpConfirmBox = 0;
    	break;
	    
	case NG_UNSUBSCRIBE:
	if ((int) client_data == XRN_YES) {
	    unsubscribeNG();
	}
	PopDownDialog(UnSubConfirmBox);
	UnSubConfirmBox = 0;
	break;
	    
	case ART_CATCHUP:
	if ((int) client_data == XRN_YES) {
	    catchUpART();
	}	    
	PopDownDialog(CatchUpConfirmBox);
	CatchUpConfirmBox = 0;
	break;
	    
	case ART_PART_CATCHUP:
	if ((int) client_data == XRN_YES) {
	    catchUpPartART();
	}	    
	PopDownDialog(PartCatchUpConfirmBox);
	PartCatchUpConfirmBox = 0;
	break;
	    
	case ART_UNSUBSCRIBE:
	if ((int) client_data == XRN_YES) {
	    unsubscribeART();
	}	    
	PopDownDialog(UnSubConfirmBox);
	UnSubConfirmBox = 0;
	break;
    }
    unbusyCursor(widget, 0, 0);
    return;
}


/*ARGSUSED*/
static void
ngQuitFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to quit xrn
 *
 *  full update the newsrc file
 *  exit
 */
{
    static struct DialogArg args[] = {
        {"no",               generalHandler, (caddr_t) XRN_NO},
	{"yes",              generalHandler, (caddr_t) XRN_YES},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    if (app_resources.confirmMode & NG_QUIT) {
	Action = NG_QUIT;
	if (ExitConfirmBox == (Widget) 0) {
	    ExitConfirmBox = CreateDialog(topLevel, "Are you sure?",
					   DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(ExitConfirmBox);
	return;
    }
    ehCleanExitXRN();
}


/*ARGSUSED*/
static void
ngReadFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to read a new newsgroup
 *
 * get the selected group, set the internal pointers, and go to article mode
 *
 */
{
    char name[GROUP_NAME_SIZE];
    XawTextPosition left, right;

    if (Mode != NEWSGROUP_MODE) {
	return;
    }

    resetSelection();
    if (getSelection(Text, NewsGroupsString, &left, &right)) {
	currentGroup(Mode, NewsGroupsString, name, left);
	if (gotoNewsgroup(name) == GOOD_GROUP) {
	    if (getNewsgroup() == XRN_NOMORE) {
		mesgPane("Can not get the group, it may have been deleted");
		XawTextUnsetSelection(Text);
		return;
	    }
	    (void) strcpy(LastGroup, name);
	    GroupPosition = left;
	    switchToArticleMode();
	}
    } else {
	(void) moveCursor(BACK, NewsGroupsString, &left);
	GroupPosition = left;
	XawTextSetInsertionPoint(Text, GroupPosition);
    }
    XawTextUnsetSelection(Text);
    
    return;
}


/*ARGSUSED*/
static void
ngNextFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user does not want to read a newsgroup
 *
 * if selected group, set internal group
 * call updateNewsgroupMode
 */
{
    XawTextPosition left, right;
    Arg arg[1];

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();

    (void) getSelection(Text, NewsGroupsString, &left, &right);
    (void) moveUpWrap(NewsGroupsString, &left);
    GroupPosition = left;
    adjustNewsgroupWidget();
    XtSetArg(arg[0], XtNdisplayPosition, NewsgroupTop);
    XtSetValues(Text, arg, XtNumber(arg));
    XawTextSetInsertionPoint(Text, GroupPosition);

    return;
}


/*ARGSUSED*/
static void
ngPrevFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to move the cursor up in
 * the newsgroup window
 *
 * if selected group, set internal group
 * call updateNewsgroupMode
 */
{
    XawTextPosition left, right;
    Arg arg[1];

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    
    (void) getSelection(Text, NewsGroupsString, &left, &right);
    (void) moveCursor(BACK, NewsGroupsString, &left);
    GroupPosition = left;
    adjustNewsgroupWidget();
    XtSetArg(arg[0], XtNdisplayPosition, NewsgroupTop);
    XtSetValues(Text, arg, XtNumber(arg));
    XawTextSetInsertionPoint(Text, GroupPosition);

    return;
}


/*ARGSUSED*/
static void
ngCatchUpFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called to catch up on all unread articles in this newsgroup
 * use a confirmation box if the user has requested it
 * if selected group, set internal group
 */
{
    static struct DialogArg args[] = {
        {"no",               generalHandler, (caddr_t) XRN_NO},
	{"yes",              generalHandler, (caddr_t) XRN_YES},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    
    if (app_resources.confirmMode & NG_CATCHUP) {
	Action = NG_CATCHUP;
	if (CatchUpConfirmBox == (Widget) 0) {
	    CatchUpConfirmBox = CreateDialog(topLevel, "OK to catch up?",
					     DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(CatchUpConfirmBox);

	return;
    }
    catchUpNG();

    return;
}


/*ARGSUSED*/
static void
ngUnsubFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called to unsubscribe to a newsgroup
 *
 * if selected group, set internal group
 * do internals
 * call updateNewsgroupMode
 */
{
    static struct DialogArg args[] = {
        {"no",               generalHandler, (caddr_t) XRN_NO},
	{"yes",              generalHandler, (caddr_t) XRN_YES},
    };    

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    
    if (app_resources.confirmMode & NG_UNSUBSCRIBE) {
	Action = NG_UNSUBSCRIBE;
	if (UnSubConfirmBox == (Widget) 0) {
	    UnSubConfirmBox = CreateDialog(topLevel, "OK to unsubscribe?",
					   DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(UnSubConfirmBox);
	return;
    }
    unsubscribeNG();

    return;
}


#define XRNsub_ABORT 0
#define XRNsub_LASTGROUP 1
#define XRNsub_FIRST 2
#define XRNsub_LAST 3
#define XRNsub_CURRENT 4

static Widget SubscribeBox = (Widget) 0;

/*ARGSUSED*/
static void
subscribeHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    int status = SUBSCRIBE;
    char name[GROUP_NAME_SIZE];
    XawTextPosition left, right;

    busyCursor(widget, 0, 0);
    switch ((int) client_data) {
	case XRNsub_LASTGROUP:
	if (LastGroup[0] != '\0') {
	    if (gotoNewsgroup(LastGroup) != GOOD_GROUP) {
		mesgPane("Bad Group");
		PopDownDialog(SubscribeBox);
		SubscribeBox = 0;
		unbusyCursor(widget, 0, 0);
		return;
	    }
	    subscribe();
	    updateNewsgroupMode();
	}
	break;

	case XRNsub_FIRST:
	if (addToNewsrcBeginning(GetDialogValue(SubscribeBox),
				 status) == GOOD_GROUP) {
	    GroupPosition = 0;
	    updateNewsgroupMode();
	}
	break;

	case XRNsub_LAST:
	if (addToNewsrcEnd(GetDialogValue(SubscribeBox),
			   status) == GOOD_GROUP) {
	    updateNewsgroupMode();
	    endInsertionPoint(NewsGroupsString, &GroupPosition);
	    XawTextSetInsertionPoint(Text, GroupPosition);
	}
	break;

	case XRNsub_CURRENT:
	if (NewsGroupsString[XawTextGetInsertionPoint(Text)] == '\0') {
	    if (addToNewsrcEnd(GetDialogValue(SubscribeBox),
			       status) == GOOD_GROUP) {
		updateNewsgroupMode();
		endInsertionPoint(NewsGroupsString, &GroupPosition);
		XawTextSetInsertionPoint(Text, GroupPosition);
	    }
	} else {

	    /* don't need to check for the null group here, it would have */
	    /* been already handled above */
	    (void) getSelection(Text, NewsGroupsString, &left, &right);
	    GroupPosition = left;
	    if (GroupPosition == 0) {
	        if (addToNewsrcBeginning(GetDialogValue(
		    SubscribeBox),status) == GOOD_GROUP) {
		    updateNewsgroupMode();
		}
	    } else {
		(void) moveCursor(BACK, NewsGroupsString, &GroupPosition);
		currentGroup(Mode, NewsGroupsString, name, GroupPosition);
	        if (addToNewsrcAfterGroup(GetDialogValue(
		    SubscribeBox), name, status) == GOOD_GROUP) {
		    (void) moveUpWrap(NewsGroupsString, &GroupPosition);
		    updateNewsgroupMode();
		}
	    }
    	}
	break;
    }
    
    PopDownDialog(SubscribeBox);
    SubscribeBox = 0;
    XawTextUnsetSelection(Text);
    unbusyCursor(widget, 0, 0);
    return;
}


/*ARGSUSED*/
static void
ngSubscribeFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Subscribe to a group currently unsubscribed to
 */
{
    static struct DialogArg args[] = {
	{"abort",            subscribeHandler, (caddr_t) XRNsub_ABORT},
	{"last group",       subscribeHandler, (caddr_t) XRNsub_LASTGROUP},
	{"first",            subscribeHandler, (caddr_t) XRNsub_FIRST},
	{"last",             subscribeHandler, (caddr_t) XRNsub_LAST},
	{"cursor position",  subscribeHandler, (caddr_t) XRNsub_CURRENT},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    
    if (SubscribeBox == (Widget) 0) {
	SubscribeBox = CreateDialog(topLevel, "Group to subscribe to:",
				    DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(SubscribeBox);

    return;
}


#define XRNgoto_ABORT 0
#define XRNgoto_GOTO 1

static Widget GotoNewsgroupBox = (Widget) 0;

/*ARGSUSED*/
static void
gotoHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *name;

    busyCursor(widget, 0, 0);
    XawTextUnsetSelection(Text);
    if ((int) client_data == XRNgoto_GOTO) {
	name = GetDialogValue(GotoNewsgroupBox);
	if (name[0] == '\0') {
	    mesgPane("No Newsgroup name specified");
	} else if (jumpToNewsgroup(name) == GOOD_GROUP) {
	    (void) strcpy(LastGroup, name);
	    switchToArticleMode();
	}
    }
    PopDownDialog(GotoNewsgroupBox);
    GotoNewsgroupBox = 0;
    unbusyCursor(widget, 0, 0);
    return;
}


/*ARGSUSED*/
static void
ngGotoFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Jump to a newsgroup not displayed in newsgroup mode (either because
 * it's not subscribed to, or because all the articles have been read)
 *
 */
{
    static struct DialogArg args[] = {
	{"abort",           gotoHandler, (caddr_t) XRNgoto_ABORT},
	{"go to newsgroup", gotoHandler, (caddr_t) XRNgoto_GOTO},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    if (GotoNewsgroupBox == (Widget) 0) {
	GotoNewsgroupBox = CreateDialog(topLevel, "Group to go to:",
				    DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(GotoNewsgroupBox);
    return;
}



/*ARGSUSED*/
static void
ngAllGroupsFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Enter "all" mode.  Display all available groups to allow user to
 * subscribe/unsubscribe to them.
 */
{
    Arg args[3];
    
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();

    PreviousMode = Mode;
    Mode = ALL_MODE;

    /* switch buttons */
    swapMode();
    
    setBottomInfoLine("View all available groups, with option to subscribe");
    /* create the screen */
    AllGroupsString = getStatusString(AllStatus);

    XtSetArg(args[0], XtNstring, AllGroupsString);
    XtSetArg(args[1], XtNlength, utStrlen(AllGroupsString) + 1);
    XtSetArg(args[2], XtNeditType, XawtextRead);
    
    AllSource = XawStringSourceCreate(ArticleText, args, XtNumber(args));
    XawTextSetSource(ArticleText, AllSource, (XawTextPosition) 0);
    XtSetValues(ArticleText, lineSelArgs, XtNumber(lineSelArgs));
    
    return;
}


/*ARGSUSED*/
static void
ngRescanFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * query the server to see if there are any new articles and groups
 */
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    rescanServer();
    determineMode();
    
    return;
}


/*ARGSUSED*/
static void
ngPrevGroupFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * put the user in the previous newsgroup accessed
 */
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    resetSelection();
    if (LastGroup[0] != '\0') {
	if (jumpToNewsgroup(LastGroup) == GOOD_GROUP) {
	    switchToArticleMode();
	}
    }

    return;
}


/*ARGSUSED*/
static void
ngSelectFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/* 
 * save the user's selection of groups to be moved with the move
 * command
 */
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    (void) getSelection(Text, NewsGroupsString, &First, &Last);
    
    return;
}


/*ARGSUSED*/
static void
ngMoveFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Move the previously selected groups to the position before the
 * current selection
 */
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    char *newString;
    XawTextPosition left, right;
    XawTextPosition stringPoint;
    XawTextPosition cursorSpot;
    int direction = 0;
    int numGroups = 0;

    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    if (First == Last) {
	mesgPane("No groups were selected");
	return;	
    }
    buildString(&newString, First, Last, NewsGroupsString);
    stringPoint = 0;
    (void) getSelection(Text, NewsGroupsString, &left, &right);
    if ((left >= First) && (left <= Last+1)) {
	mesgPane("Selected groups have not been moved");
	resetSelection();
	return;
    }
    GroupPosition = cursorSpot= left;
    if (left > First) {
	direction = 1;
    }
    currentGroup(Mode, newString, newGroup, stringPoint);
    if (!moveCursor(BACK, NewsGroupsString, &left)) {
	(void) addToNewsrcBeginning(newGroup, status);
	(void) strcpy(oldGroup, newGroup);
	(void) moveCursor(FORWARD, newString, &stringPoint);
	numGroups++;
    } else {
	currentGroup(Mode, NewsGroupsString, oldGroup, left);
	(void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
	(void) strcpy(oldGroup, newGroup);
	(void) moveCursor(FORWARD, newString, &stringPoint);
	numGroups++;
    }
    while (newString[stringPoint] != '\0') {
	numGroups++;
	currentGroup(Mode, newString, newGroup, stringPoint);
	(void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
	(void) strcpy(oldGroup, newGroup);
	if (!moveCursor(FORWARD, newString, &stringPoint)) {
	    break;
	}
    }
    redrawNewsgroupTextWidget();
    if (direction) {
	while (numGroups > 0) {
	    GroupPosition = cursorSpot;
	    (void) moveCursor(BACK, NewsGroupsString, &GroupPosition);
	    numGroups--;
	}
	adjustNewsgroupWidget();
    }
    XawTextSetInsertionPoint(Text, GroupPosition);
    resetSelection();
    
    return;
}


/*ARGSUSED*/
static void
ngExitFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Quit xrn, leaving the newsrc in the state it was in at
 * the last invokation of rescan.
 */
{
    static struct DialogArg args[] = {
        {"no",               generalHandler, (caddr_t) XRN_NO},
	{"yes",              generalHandler, (caddr_t) XRN_YES},
    };

    if (Mode != NEWSGROUP_MODE) {
	return;
    }

    if (app_resources.confirmMode & NG_EXIT) {
	Action = NG_EXIT;
	if (ExitConfirmBox == (Widget) 0) {
	    ExitConfirmBox = CreateDialog(topLevel, "Are you sure?",
					   DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(ExitConfirmBox);
	return;
    }

    ehNoUpdateExitXRN();
}


/*ARGSUSED*/
static void
ngCheckPointFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * update the .newsrc file
 */
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }

    if (!updatenewsrc()) {
	ehErrorExitXRN("Can not update the newsrc file");
    }

    return;
}


/*ARGSUSED*/
static void
ngGripeFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Allow user to gripe
 */
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    gripe();
    return;
}


/*ARGSUSED*/
static void
ngPostFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * allow user to post an article
 */
{
    if (Mode != NEWSGROUP_MODE) {
	return;
    }
    post();
    
    return;
}


/*ARGSUSED*/
static void
artQuitFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to quit the current newsgroup and go to
 * the next one
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    exitArticleMode();
    
    return;
}

/*ARGSUSED*/
static void
artNextFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to read the next article
 */
{
    char *filename;		/* name of the article file */
    char *question;		/* question to put in the question box */
    long artNum;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    selectedArticle(ArtStatus);
    if (getNearbyArticle(ArtStatus, &filename, &question,
			 &artNum) == art_DONE) {
	exitArticleMode();
	return;
    }
    /* update the text window */
    foundArticle(filename, question, artNum);

    ArtStatus = art_NEXT;
    ArtEntry = 0;
    
    return;
}


#ifdef XRN_TEXT_WIDGET
/*ARGSUSED*/
static void
artScrollFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to scroll the current article
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (!app_resources.pageArticles || XawTextLastPage(ArticleText)) {
	if (app_resources.subjectRead == False) {
	    artNextUnreadFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);
	} else {
	    artSubNextFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);
	}
    } else {
	XtScrollDownOnePage(ArticleText);
    }
    return;
}
#endif


static void
artNextUnreadFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to go to the next unread news
 * article in the current newsgroup
 * 
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    ArtStatus = art_UNREAD;
    artNextFunction(widget, client_data, call_data);
    
    return;
}


/*ARGSUSED*/
static void
artPrevFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to read the previous article
 */
{
    XawTextPosition left, right;
    long artNum;
    char *filename, *question;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    ArtStatus = art_PREV;
    XawTextGetSelectionPos(Text, &left, &right);
    XawTextUnsetSelection(Text);
    ArtPosition = left;
    if (left == right) {
	ArtPosition = XawTextGetInsertionPoint(Text);
	if (ArtPosition == 0) {
	    if (getPrevious(&artNum)) {
		gotoArticle(artNum);
		(void) getArticle(&filename, &question);
		foundArticle(filename, question, artNum);
		ArtStatus = art_NEXT;
		return;
	    }
	    return;
	}
	(void) moveCursor(BACK, SubjectString, &ArtPosition);
    } else {
	moveBeginning(SubjectString, &ArtPosition);
    }
    if (getNearbyArticle(ArtStatus, &filename, &question, &artNum) == art_DONE) {
	exitArticleMode();
	return;
    }
    foundArticle(filename, question, artNum);
    ArtStatus = art_NEXT;
    
    return;
}


/*ARGSUSED*/
static void
artNextGroupFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char name[GROUP_NAME_SIZE];

    LastArticle = NIL(char);
    PrevArticle = CurrentArticle = 0;
		      
    releaseNewsgroupResources();
    if (moveCursor(FORWARD, NewsGroupsString, &GroupPosition) == FALSE) {
	artQuitFunction(widget, client_data, call_data);
	return;
    }
    currentGroup(NEWSGROUP_MODE, NewsGroupsString, name, GroupPosition);
    moveBeginning(NewsGroupsString, &GroupPosition);

    if ((name == NIL(char)) || (*name == '\0')) {
	artQuitFunction(widget, client_data, call_data);
	return;
    }

    if (gotoNewsgroup(name) == GOOD_GROUP) {
	if (getNewsgroup() == XRN_NOMORE) {
	    mesgPane("Can not get the group, it may have been deleted");
	    return;
	}
	(void) strcpy(LastGroup, name);
	switchToArticleMode();
    }
    return;
}


/*ARGSUSED*/
static void
artCatchUpFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to mark all articles in the current group as read
 */
{
    XawTextPosition left, right;

    static struct DialogArg args[] = {
        {"no",               generalHandler, (caddr_t) XRN_NO},
	{"yes",              generalHandler, (caddr_t) XRN_YES},
    };

    if (Mode != ARTICLE_MODE) {
	return;
    }
    XawTextGetSelectionPos(Text, &left, &right);
    if (left != right) {
	(void) getSelection(Text, SubjectString, &left, &right);
    }
    ArtPosition = right;
    if (left == right) {
	if (app_resources.confirmMode & ART_CATCHUP) {
	    Action = ART_CATCHUP;
	    if (CatchUpConfirmBox == (Widget) 0) {
		CatchUpConfirmBox = CreateDialog(topLevel, "OK to catch up?",
						 DIALOG_NOTEXT, args, XtNumber(args));
	    }
	    PopUpDialog(CatchUpConfirmBox);

	    return;
	}
	catchUpART();
	return;
    }
    if (moveCursor(FORWARD, SubjectString, &ArtPosition)) {
	if (app_resources.confirmMode & ART_CATCHUP) {
	    Action = ART_PART_CATCHUP;
	    if (PartCatchUpConfirmBox == (Widget) 0) {
		PartCatchUpConfirmBox = CreateDialog(topLevel,
						     "OK to catch up to current position?",
						     DIALOG_NOTEXT, args, XtNumber(args));
	    }
	    PopUpDialog(PartCatchUpConfirmBox);
	} else {
	    catchUpPartART();
	}
    }
    return;
}


/*ARGSUSED*/
static void
artUnsubFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * called when the user wants to unsubscribe to the current group
 */
{
    static struct DialogArg args[] = {
        {"no",               generalHandler, (caddr_t) XRN_NO},
	{"yes",              generalHandler, (caddr_t) XRN_YES},
    };

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (app_resources.confirmMode & ART_UNSUBSCRIBE) {
	Action = ART_UNSUBSCRIBE;
	if (UnSubConfirmBox == (Widget) 0) {
	    UnSubConfirmBox = CreateDialog(topLevel, "OK to unsubscribe?",
					   DIALOG_NOTEXT, args, XtNumber(args));
	}
	PopUpDialog(UnSubConfirmBox);
	return;
    }
    unsubscribeART();
    return;
}


static void
markFunction(marker)
char marker;
/*
 * Get selection region, mark articles, redisplay subject window.
 */
{
    XawTextPosition left, right;
    
    (void) getSelection(Text, SubjectString, &left, &right);
    markArticles(SubjectString, left, right, marker);
    updateSubjectWidget(left, right);
    
    return;
}


/*ARGSUSED*/
static void
artMarkReadFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Mark selected article(s) as read
 */
{
    char marker = '+';
    XawTextPosition save;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    save = ArtPosition;
    markFunction(marker);
    ArtPosition = save;
    return;
}


/*ARGSUSED*/
static void
artMarkUnreadFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Mark selected article(s) as unread
 */
{
    char marker = 'u';
    XawTextPosition save;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    save = ArtPosition;
    markFunction(marker);
    ArtPosition = save;
    return;
}


/*ARGSUSED*/
static void
artPostFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * allow user to post to the newsgroup currently being read
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    post();
    
    return;
}


/*ARGSUSED*/
static void
artSubNextFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 *
 */
{
    XawTextPosition left, right;
    char *filename, *question;
    char *subject;
    long artNum;
    int status;
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    XawTextGetSelectionPos(Text, &left, &right);
    XawTextUnsetSelection(Text);
    ArtPosition = left;
    if (left == right) {
	ArtPosition = XawTextGetInsertionPoint(Text);
	if (SubjectString[ArtPosition] == '\0') {
	    return;
	}
	artNum = atol(&SubjectString[ArtPosition+2]);
	subject = XtNewString(getSubject(artNum));
	(void) moveCursor(FORWARD, SubjectString, &ArtPosition);
	status = isNextSubject(subject, &filename, &question, &artNum);
	FREE(subject);
	switch (status) {
	  case ABORT:
	    infoNow("search has been aborted");
	    return;
	  case NOCHANGE:
	    (void) sprintf(error_buffer, "Subject search: %s", subject);
	    info(error_buffer);
	    foundArticle(filename, question, artNum);
	    return;
	  case DONE:
	    ArtPosition = 0;
	    infoNow("Subject has been exhausted, returning to first unread article");
	    if (getNearbyArticle(art_UNREAD,&filename,&question,&artNum) == art_DONE) {
		exitArticleMode();
		return;
	    }
	    foundArticle(filename, question, artNum);
	    return;
	  case EXIT:
	    exitArticleMode();
	    return;
	}
    }
    if (getNearbyArticle(art_NEXT, &filename, &question, &artNum) == art_DONE) {
	exitArticleMode();
	return;
    }
    foundArticle(filename, question, artNum);
    
    return;
}
	

/*ARGSUSED*/
static void
artSubPrevFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 *
 */
{
    XawTextPosition left, right;
    char *subject;
    long artNum;
    char *filename, *question;
    int status;
    Arg args[3];

    if (Mode != ARTICLE_MODE) {
	return;
    }
    XawTextGetSelectionPos(Text, &left, &right);
    XawTextUnsetSelection(Text);
    ArtPosition = left;
    if (left == right) {
	/* nothing selected, we should be on a valid article */
	ArtPosition = XawTextGetInsertionPoint(Text);
	if (SubjectString[ArtPosition] == '\0') {
	    return;
	}
	artNum = atol(&SubjectString[ArtPosition+2]);
	subject = XtNewString(getSubject(artNum));
	(void) sprintf(error_buffer, "Subject search: %s", subject);
	status = isPrevSubject(subject, &filename, &question, &artNum);
	FREE(subject);
	switch(status) {
	  case ABORT:
	    infoNow("search has been aborted");
	    return;
	  case NOCHANGE:
	    info(error_buffer);
	    foundArticle(filename, question, artNum);
	    return;
	  case CHANGE:
	    if (SubjectSource != (XawTextSource) 0) {
		ArticleTop = XawTextTopPosition(Text);
		XawStringSourceDestroy(SubjectSource);
	    }

	    XtSetArg(args[0], XtNstring, SubjectString);
	    XtSetArg(args[1], XtNlength, utStrlen(SubjectString) + 1);
	    XtSetArg(args[2], XtNeditType, XawtextRead);
    
	    SubjectSource = XawStringSourceCreate(Text, args, XtNumber(args));
	    XawTextSetSource(Text, SubjectSource, ArticleTop);
	    info(error_buffer);
	    foundArticle(filename, question, artNum);
	    return;
	  case DONE:
	    infoNow("Subject has been exhausted");
	    return;
	  case EXIT:
	    exitArticleMode();
	    return;
	}
    }
    moveBeginning(SubjectString, &left);
    ArtPosition = left;
    artNum = atol(&SubjectString[ArtPosition+2]);
    gotoArticle(artNum);
    if (getArticle(&filename, &question) != XRN_OKAY) {
	removeLine(SubjectString, Text, &SubjectSource, ArtPosition,
		   &ArticleTop);
	if (getNearbyArticle(art_NEXT, &filename, &question, &artNum) == art_DONE) {
	    exitArticleMode();
	    return;
	}
	infoNow(error_buffer);
	foundArticle(filename, question, artNum);
	return;
    }
    infoNow(error_buffer);
    foundArticle(filename, question, artNum);
    
    return;
}


char *SubjectKilled;

/*ARGSUSED*/
static void
artKillSessionFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Allow user to mark all articles with the current subject as read
 *
 * XXX get subject, kill using data structures, rebuild SubjectString
 */
{
    XawTextPosition left, right;
    char *subject;
    char *cursubject;
    char *filename, *question;
    long artNum;
    Arg args[3];
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (!getSelection(Text, SubjectString, &left, &right)) {
	return;
    }
    ArtPosition = left;
#ifdef ellen
    XawTextGetSelectionPos(Text, &left, &right);
    XawTextUnsetSelection(Text);
    if (left == right) {
	ArtPosition = XawTextGetInsertionPoint(Text);
	if (SubjectString[ArtPosition] == '\0') {
	    return;
	}
	left = ArtPosition;
    } else {
	ArtPosition = left;
    }
    moveBeginning(SubjectString, &ArtPosition);
#endif
    artNum = atol(&SubjectString[ArtPosition+2]);
    subject = XtNewString(getSubject(artNum));
    SubjectKilled = XtNewString(subject);
    ArtPosition = 0;
    while (SubjectString[ArtPosition] != '\0') {
	artNum = atol(&SubjectString[ArtPosition+2]);
	cursubject = getSubject(artNum);
	/* only kill those that have not been marked as unread */
	if ((STREQ(subject, cursubject)) &&
	    (SubjectString[ArtPosition] != 'u')) {
	    markArticleAsRead(artNum);
	    (void) markStringRead(SubjectString, ArtPosition);
	}
	if (!moveCursor(FORWARD, SubjectString, &ArtPosition)) {
	    break;
	}
    }
    /* set the cursor back to the beginning of the subject screen */
    ArtPosition = 0;
    FREE(subject);
    infoNow("Subject has been killed, returning to first unread article");
    if (getNearbyArticle(art_UNREAD, &filename, &question, &artNum)
	== art_DONE) {
	exitArticleMode();
	return;
    }
    if (SubjectSource != (XawTextSource) 0) {
	ArticleTop = XawTextTopPosition(Text);
	XawStringSourceDestroy(SubjectSource);
    }

    XtSetArg(args[0], XtNstring, SubjectString);
    XtSetArg(args[1], XtNlength, utStrlen(SubjectString) + 1);
    XtSetArg(args[2], XtNeditType, XawtextRead);
    
    SubjectSource = XawStringSourceCreate(Text, args, XtNumber(args));
    XawTextSetSource(Text, SubjectSource, ArticleTop);

    foundArticle(filename, question, artNum);
    
    return;
}


/*ARGSUSED*/
static void
artKillAuthorFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Allow user to mark all articles with the current author as read
 *
 * XXX get author, kill using data structures, rebuild SubjectString
 * XXX merge this with artKillSession
 */
{
    XawTextPosition left, right;
    char *author;
    char *curauthor;
    char *filename, *question;
    long artNum;
    Arg args[3];
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (!getSelection(Text, SubjectString, &left, &right)) {
	return;
    }
    ArtPosition = left;
#ifdef ellen
    XawTextGetSelectionPos(Text, &left, &right);
    XawTextUnsetSelection(Text);
    if (left == right) {
	ArtPosition = XawTextGetInsertionPoint(Text);
	if (SubjectString[ArtPosition] == '\0') {
	    return;
	}
	left = ArtPosition;
    } else {
	ArtPosition = left;
    }
    moveBeginning(SubjectString, &ArtPosition);
#endif
    artNum = atol(&SubjectString[ArtPosition+2]);
    author = XtNewString(getAuthor(artNum));
    ArtPosition = 0;
    while (SubjectString[ArtPosition] != '\0') {
	artNum = atol(&SubjectString[ArtPosition+2]);
	curauthor = getAuthor(artNum);
	/* only kill those that have not been marked as unread */
	if ((STREQ(author, curauthor)) &&
	    (SubjectString[ArtPosition] != 'u')) {
	    markArticleAsRead(artNum);
	    (void) markStringRead(SubjectString, ArtPosition);
	}
	if (!moveCursor(FORWARD, SubjectString, &ArtPosition)) {
	    break;
	}
    }
    /* set the cursor back to the beginning of the subject screen */
    ArtPosition = 0;
    FREE(author);
    infoNow("Author has been killed, returning to first unread article");
    if (getNearbyArticle(art_UNREAD, &filename, &question, &artNum)
	== art_DONE) {
	exitArticleMode();
	return;
    }
    if (SubjectSource != (XawTextSource) 0) {
	ArticleTop = XawTextTopPosition(Text);
	XawStringSourceDestroy(SubjectSource);
    }

    XtSetArg(args[0], XtNstring, SubjectString);
    XtSetArg(args[1], XtNlength, utStrlen(SubjectString) + 1);
    XtSetArg(args[2], XtNeditType, XawtextRead);
    
    SubjectSource = XawStringSourceCreate(Text, args, XtNumber(args));
    XawTextSetSource(Text, SubjectSource, ArticleTop);

    foundArticle(filename, question, artNum);
    
    return;
}


/*ARGSUSED*/
static void
artKillLocalFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    artKillSessionFunction(widget, client_data, call_data);
    killItem(SubjectKilled, KILL_LOCAL);
    FREE(SubjectKilled);
    return;
}


/*ARGSUSED*/
static void
artKillGlobalFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    artKillSessionFunction(widget, client_data, call_data);
    killItem(SubjectKilled, KILL_GLOBAL);
    FREE(SubjectKilled);
    return;
}


#define XRNgotoArticle_ABORT	0
#define XRNgotoArticle_DOIT	1

static Widget GotoArticleBox = (Widget) 0;


/*ARGSUSED*/
static void 
gotoArticleHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *numberstr;
    char *filename, *question;
    int SavePosition;
    int status;
    int direction;
    long artNum;
    Arg args[3];

    busyCursor(widget, 0, 0);
    XawTextUnsetSelection(Text);
    if ((int) client_data == XRNgotoArticle_ABORT) {
	PopDownDialog(GotoArticleBox);
	GotoArticleBox = 0;
	unbusyCursor(widget, 0, 0);
	return;
    }
    SavePosition = ArtPosition;
    numberstr = GetDialogValue(GotoArticleBox);
    if (numberstr == NIL(char)) {
	mesgPane("Bad article number");
	PopDownDialog(GotoArticleBox);
	GotoArticleBox = 0;
	unbusyCursor(widget, 0, 0);
	return;
    }

    artNum = atol(numberstr);
    if (artNum == 0) {
	mesgPane("Bad article number");
	PopDownDialog(GotoArticleBox);
	GotoArticleBox = 0;
	unbusyCursor(widget, 0, 0);
	return;
    }
    
    status = moveToArticle(artNum, &filename, &question);

    switch (status) {

      case NOMATCH:
	mesgPane("Article number %d not available", artNum);

      case ERROR:
	ArtPosition = SavePosition;
	break;

      case MATCH:
	if (SubjectSource != (XawTextSource) 0) {
	    XawStringSourceDestroy(SubjectSource);
	}

	if (SubjectString != NIL(char)) {
	    FREE(SubjectString);
	}

	SubjectString = getSubjects(ALL);
	XtSetArg(args[0], XtNstring, SubjectString);
	XtSetArg(args[1], XtNlength, utStrlen(SubjectString) + 1);
	XtSetArg(args[2], XtNeditType, XawtextRead);
    
	/* XXX not right...  only right if going back... */
	SubjectSource = XawStringSourceCreate(Text, args, XtNumber(args));
	XawTextSetSource(Text, SubjectSource, (XawTextPosition) 0);
	ArtPosition = 0;
	foundArticle(filename, question, artNum);
	break;
    }

    PopDownDialog(GotoArticleBox);
    GotoArticleBox = 0;
    unbusyCursor(widget, 0, 0);
    return;
}


/*ARGSUSED*/
static void
artGotoArticleFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    static struct DialogArg args[] = {
	{"abort",   gotoArticleHandler, (caddr_t) XRNgotoArticle_ABORT},
	{"doit", gotoArticleHandler, (caddr_t) XRNgotoArticle_DOIT},
    };
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (GotoArticleBox == (Widget) 0) {
	GotoArticleBox = CreateDialog(topLevel, "Article Number:",
				  DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(GotoArticleBox);
    return;
}


#define XRNsubSearch_ABORT 0
#define XRNsubSearch_FORWARD 1
#define XRNsubSearch_BACK 2

static Widget SubSearchBox = (Widget) 0;

static int SubInsideHandler = 0;

#define CLEANUP \
	PopDownDialog(SubSearchBox); \
	SubSearchBox = 0; \
	SubInsideHandler = 0; \
	unbusyCursor(widget, 0, 0);


/*ARGSUSED*/
static void 
subSearchHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *regexpr;
    char *filename, *question;
    int SavePosition;
    int status;
    int direction;
    long artNum;
    Arg args[3];

    if (SubInsideHandler) {
	return;
    }

    SubInsideHandler = 1;
    
    busyCursor(widget, 0, 0);
    XawTextUnsetSelection(Text);
    if ((int) client_data == XRNsubSearch_ABORT) {
	CLEANUP;
	return;
    }
    SavePosition = ArtPosition;
    regexpr = GetDialogValue(SubSearchBox);
    if (*regexpr == 0) {
	if (LastRegexp == NIL(char)) {
	    mesgPane("No previous regular expression");
	    CLEANUP;
	    return;	   
	}
	regexpr = LastRegexp;
    } else {
	if (LastRegexp != NIL(char)) {
	    FREE(LastRegexp);
	}
	LastRegexp = XtNewString(regexpr);
    }
    
    direction = ((int) client_data == XRNsubSearch_FORWARD) ? FORWARD : BACK;
    LastSearch = direction;
    status = subjectSearch(direction, &SubjectString, SubjectSource, 
			   &ArtPosition, ArticleTop, LastRegexp,
			   &filename, &question, &artNum);
    switch (status) {
      case ABORT:
	infoNow("search aborted");
	ArtPosition = SavePosition;
	break;

      case NOMATCH:
	(void) sprintf(error_buffer, "Search for expression %s: no match was found",
		       LastRegexp);
	infoNow(error_buffer);
      case ERROR:
	ArtPosition = SavePosition;
	break;

      case MATCH:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	foundArticle(filename, question, artNum);
	break;

      case WINDOWCHANGE:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	if (SubjectSource != (XawTextSource) 0) {
	    ArticleTop = XawTextTopPosition(Text);
	    XawStringSourceDestroy(SubjectSource);
	}
	XtSetArg(args[0], XtNstring, SubjectString);
	XtSetArg(args[1], XtNlength, utStrlen(SubjectString) + 1);
	XtSetArg(args[2], XtNeditType, XawtextRead);
    
	SubjectSource = XawStringSourceCreate(Text, args, XtNumber(args));
	XawTextSetSource(Text, SubjectSource, ArticleTop);
	foundArticle(filename, question, artNum);
	break;

      case EXIT:
	exitArticleMode();
	break;
    }
    
    CLEANUP;
    return;
}


/*ARGSUSED*/
static void
artSubSearchFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    static struct DialogArg args[] = {
	{"abort",   subSearchHandler, (caddr_t) XRNsubSearch_ABORT},
	{"forward", subSearchHandler, (caddr_t) XRNsubSearch_FORWARD},
	{"back",    subSearchHandler, (caddr_t) XRNsubSearch_BACK},
    };
    
    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (SubSearchBox == (Widget) 0) {
	SubSearchBox = CreateDialog(topLevel, "Regular Expression:",
				  DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(SubSearchBox);
    return;
}


/*ARGSUSED*/
static void
artContinueFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Continue a previously started regular expression
 * search of the subject lines.  Search for same
 * regular expression, in same direction.
 */
{
    char *filename, *question;
    XawTextPosition SavePosition;
    int status;
    long artNum;
    Arg args[3];

    if (LastRegexp == NULL) {
	mesgPane("No previous regular expression");
	return;
    }
    (void) sprintf(error_buffer, "Searching for expression %s", LastRegexp);
    info(error_buffer);
    XawTextUnsetSelection(Text);
    SavePosition = ArtPosition;
    status = subjectSearch(LastSearch, &SubjectString, SubjectSource,
			   &ArtPosition, ArticleTop, NIL(char),
			   &filename, &question, &artNum);
    switch (status) {
      case ABORT:
	infoNow("search aborted");
	ArtPosition = SavePosition;
	return;
      case NOMATCH:
	(void) sprintf(error_buffer, "Search for expression %s: no match was found", LastRegexp);
	infoNow(error_buffer);
      case ERROR:
	ArtPosition = SavePosition;
	return;
      case MATCH:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);    
	foundArticle(filename, question, artNum);
	return;
      case WINDOWCHANGE:
	(void) sprintf(error_buffer, "Search for expression %s", LastRegexp);
	infoNow(error_buffer);
	if (SubjectSource != (XawTextSource) 0) {
	    ArticleTop = XawTextTopPosition(Text);
	    XawStringSourceDestroy(SubjectSource);
	}
	XtSetArg(args[0], XtNstring, SubjectString);
	XtSetArg(args[1], XtNlength, utStrlen(SubjectString) + 1);
	XtSetArg(args[2], XtNeditType, XawtextRead);
    
	SubjectSource = XawStringSourceCreate(Text, args, XtNumber(args));
	XawTextSetSource(Text, SubjectSource, ArticleTop);
	foundArticle(filename, question, artNum);
	return;
      case EXIT:
	exitArticleMode();
	return;
    }
}

	
/*ARGSUSED*/
static void
artLastFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Display the article accessed before the current one
 */
{
    char *filename, *question;
    long artNum = 0;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (PrevArticle == 0) {
	mesgPane("No previous article");
	return;
    }
    ArtPosition = 0;
    findArticle(SubjectString, PrevArticle, &ArtPosition);
    gotoArticle(PrevArticle);
    if (getArticle(&filename, &question) != XRN_OKAY) {
	removeLine(SubjectString, Text, &SubjectSource, ArtPosition, &ArticleTop);
    } else {
	foundArticle(filename, question, artNum);
    }
    
    return;
}


/*ARGSUSED*/
static void
artExitFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Exit from the current newsgroup, marking all articles as
 * unread
 */
{
    XawTextPosition beg, end;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    beg = (XawTextPosition) 0;
    end = (XawTextPosition) 0;
    endInsertionPoint(SubjectString, &end);
    moveEnd(SubjectString, &end);
    markArticles(SubjectString, beg, end, ' ');
    exitArticleMode();
    
    return;
}


/*ARGSUSED*/
static void
addQuitFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * unsubscribe to the remaining groups and exit add mode
 */
{
    char newGroup[GROUP_NAME_SIZE];
    int left, right, nbytes;
    int status = UNSUBSCRIBE;

    if (Mode != ADD_MODE) {
	return;
    }
    left = 0;

    /*
     * go through the remaining groups, add them
     * to the end of the newsrc and unsubscribe them
     */
    while (AddGroupsString[left] != '\0') {
	for (right = left; AddGroupsString[right] != '\n'; right++);
	nbytes = right - left;
	(void) strncpy(newGroup, &AddGroupsString[left], nbytes);
	newGroup[nbytes] = '\0';
	(void) addToNewsrcEnd(newGroup, status);
	left = right+1;
    }

    exitAddMode();
    return;
}


/*ARGSUSED*/
static void
addFirstFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Find selected group(s) and add them to the .newsrc in the first position.
 * Move the cursor to the next group.
 * Update the AddGroupsString, going into newsgroup mode if it
 * is NULL.  Update the text window, update the insertion point.
 *
 */
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    XawTextPosition gbeg, left, right;

    if (Mode != ADD_MODE) {
	return;
    }
    if (getSelection(Text, AddGroupsString, &left, &right)) {
	gbeg = left;
	currentGroup(Mode, AddGroupsString, newGroup, gbeg);
	(void) addToNewsrcBeginning(newGroup, status);
	(void) strcpy(oldGroup, newGroup);
	(void) moveCursor(FORWARD, AddGroupsString, &gbeg);
	while (gbeg <= right) {
	    currentGroup(Mode, AddGroupsString, newGroup, gbeg);
	    (void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
	    (void) strcpy(oldGroup, newGroup);
	    if (!moveCursor(FORWARD, AddGroupsString, &gbeg)) {
		break;
	    }
	}
	(void) strcpy(&AddGroupsString[left], &AddGroupsString[right+1]);
	if (setCursorCurrent(AddGroupsString, &left)) {
	    /* update the text window */
	    redrawAddTextWidget(left);
	} else {
	    exitAddMode();
	}
    } else {
	(void) moveUpWrap(AddGroupsString, &left);
	XawTextSetInsertionPoint(Text, left);
    }
    
    return;
}


/*ARGSUSED*/
static void
addLastFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * add the currently selected group(s) to the end of the .newsrc file
 * and subscribe to them.
 */
{
    char newGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    XawTextPosition gbeg, left, right;

    if (Mode != ADD_MODE) {
	return;
    }
    if (getSelection(Text, AddGroupsString, &left, &right)) {
	gbeg = left;
	while (gbeg <= right) {
	    currentGroup(Mode, AddGroupsString, newGroup, gbeg);
	    (void) addToNewsrcEnd(newGroup, status);
	    if (!moveCursor(FORWARD, AddGroupsString, &gbeg)) {
		break;
	    }
	}
	(void) strcpy(&AddGroupsString[left], &AddGroupsString[right+1]);
	if (setCursorCurrent(AddGroupsString, &left)) {
	    redrawAddTextWidget(left);
	} else {
	    exitAddMode();
	}
    } else {
	(void) moveUpWrap(AddGroupsString, &left);
	XawTextSetInsertionPoint(Text, left);
    }
    
    return;
}


/* entering the name of a newsgroup to add after */

#define XRNadd_ADD 1
#define XRNadd_ABORT 0

static Widget AddBox = (Widget) 0;

/*ARGSUSED*/
static void
addHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * get the newsgroup to add a new newsgroup after in the .newsrc file
 */
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    XawTextPosition gbeg, left, right;

    busyCursor(widget, 0, 0);
    if (getSelection(Text, AddGroupsString, &left, &right)) {
	if ((int) client_data == XRNadd_ADD) {
	    gbeg = left;
	    currentGroup(Mode, AddGroupsString, newGroup, gbeg);
	    if (addToNewsrcAfterGroup(newGroup,
				      GetDialogValue(AddBox),
				      status) == GOOD_GROUP) {
		(void) moveCursor(FORWARD, AddGroupsString, &gbeg);
		while (gbeg <= right) {
		    (void) strcpy(oldGroup, newGroup);
		    currentGroup(Mode, AddGroupsString, newGroup, gbeg);
		    (void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
		    if (!moveCursor(FORWARD,AddGroupsString, &gbeg)) {
			break;
		    }
		}
		(void) strcpy(&AddGroupsString[left],
			      &AddGroupsString[right+1]);
	    }
	}
	if (setCursorCurrent(AddGroupsString, &left)) {
	    redrawAddTextWidget(left);
	} else {
	    exitAddMode();
	}
    } else {
	XawTextSetInsertionPoint(Text, (XawTextPosition) 0);
    }
    PopDownDialog(AddBox);
    AddBox = 0;
    unbusyCursor(widget, 0, 0);
    return;
}


/*ARGSUSED*/
static void
addAfterFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * subscribe to a new newsgroup, adding after a particular group in the
 * .newsrc file
 */
{
    static struct DialogArg args[] = {
	{"abort", addHandler, (caddr_t) XRNadd_ABORT},
	{"add",   addHandler, (caddr_t) XRNadd_ADD},
    };

    if (Mode != ADD_MODE) {
	return;
    }
    if (AddBox == (Widget) 0) {
	AddBox = CreateDialog(topLevel, "After which newsgroup?",
				  DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(AddBox);
    return;
}


/*ARGSUSED*/
static void
addUnsubFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * add a group to the end of the .newsrc file as unsubscribed
 */
{
    char newGroup[GROUP_NAME_SIZE];
    int status = UNSUBSCRIBE;
    XawTextPosition gbeg, left, right;

    if (Mode != ADD_MODE) {
	return;
    }
    if (getSelection(Text, AddGroupsString, &left, &right)) {
	gbeg = left;
	while (gbeg <= right) {
	    currentGroup(Mode, AddGroupsString, newGroup, gbeg);
	    (void) addToNewsrcEnd(newGroup, status);
	    if (!moveCursor(FORWARD, AddGroupsString, &gbeg)) {
		break;
	    }
	}
	(void) strcpy(&AddGroupsString[left], &AddGroupsString[right+1]);
	if (setCursorCurrent(AddGroupsString, &left)) {
	    redrawAddTextWidget(left);
	} else {
	    exitAddMode();
	}
    } else {
	(void) moveUpWrap(AddGroupsString, &left);
	XawTextSetInsertionPoint(Text, left);
    }
    return;
}


#define XRNsave_ABORT          0
#define XRNsave_SAVE           1

static Widget SaveBox = (Widget) 0;  /* box for typing in the name of a file */
static char *SaveString = NULL;	/* last input to save box */

/*ARGSUSED*/
static void
saveHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * handler for the save box
 */
{
    busyCursor(widget, 0, 0);
    if ((int) client_data != XRNsave_ABORT) {
	if (saveCurrentArticle(GetDialogValue(SaveBox))) {
	    SubjectString[ArtPosition+1] = 'S';
	}
	XtFree(SaveString);
	SaveString = XtNewString(GetDialogValue(SaveBox));
    }
    XawTextInvalidate(Text, ArtPosition, ArtPosition + 2);
    PopDownDialog(SaveBox);
    SaveBox = 0;
    unbusyCursor(widget, 0, 0);
    return;
}    

/*ARGSUSED*/
static void
artSaveFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * query the user about saving an article
 *
 *    brings up a dialog box
 *
 *    returns: void
 *
 */
{
    static struct DialogArg args[] = {
	{"abort", saveHandler, (caddr_t) XRNsave_ABORT},
	{"save",  saveHandler, (caddr_t) XRNsave_SAVE},
    };

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (SaveBox == (Widget) 0) {
	SaveBox = CreateDialog(topLevel, "    File Name?    ",
				  SaveString == NULL ? DIALOG_TEXT
				   : SaveString, args, XtNumber(args));
    }
    PopUpDialog(SaveBox);
    return;
}


/*ARGSUSED*/
static void
artPrintFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char buffer[1024];
#ifdef VMS
    int status;
    short msglen;
    struct dsc$descriptor_s buf_desc = { sizeof(buffer)-1,
	DSC$K_DTYPE_T, DSC$K_CLASS_S, buffer };

    (void) sprintf(buffer, "%sARTICLE-%u.LIS", app_resources.tmpDir,
	CurrentArticle);
    if (saveCurrentArticle(buffer))
    {
	(void) sprintf(buffer, "%s %sARTICLE-%u.LIS",
	    app_resources.printCommand, app_resources.tmpDir,
	    CurrentArticle);
	status = system(buffer);
	if (status & 1) {
	    info("Article sucessfully queued");
	} else {
	    status = SYS$GETMSG(status, &msglen, &buf_desc, 0, 0);
	    buffer[msglen] = NULL;
	    info(buffer);
	}
    }

#else /* Not VMS */
    (void) sprintf(buffer, "| %s", app_resources.printCommand);
    (void) saveCurrentArticle(buffer);
#endif /* VMS */

    return;
}


/*ARGSUSED*/
static void
artReplyFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Allow user to post a reply to the currently posted article
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    reply();
    return;
}



/*ARGSUSED*/
static void
artForwardFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Allow user to forward an article to a user(s)
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    forward();
    return;
}


/*ARGSUSED*/
static void
artGripeFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Allow user to gripe
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    gripe();
    return;
}


/*ARGSUSED*/
static void
artFollowupFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Allow user to post a followup to the currently displayed article
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    followup();
    return;
}

/*ARGSUSED*/
static void
artCancelFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Allow user to post a followup to the currently displayed article
 */
{
    if (Mode != ARTICLE_MODE) {
	return;
    }
    cancelArticle();
    return;
}


/*ARGSUSED*/
static void
artRot13Function(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * decrypt a joke
 */
{
    char *filename, *question;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (toggleRotation(&filename, &question) == XRN_OKAY) {
	LastArticle = NIL(char);
	redrawArticleWidget(filename, question);
    }
    return;
}


/*ARGSUSED*/
static void
artHeaderFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char *filename, *question;

    if (Mode != ARTICLE_MODE) {
	return;
    }
    if (toggleHeaders(&filename, &question) == XRN_OKAY) {
	LastArticle = NIL(char);
	redrawArticleWidget(filename, question);
    }
    return;
}


#define SUB_STRING   "subscribed  "
#define UNSUB_STRING "unsubscribed"

/*ARGSUSED*/
static void
allQuitFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Quit all groups mode.
 */
{
    if (Mode != ALL_MODE) {
	return;
    }
    /* destory string and source and stuff in the dummy source */
    
    if (AllSource != (XawTextSource) 0) {
	XawStringSourceDestroy(AllSource);
	AllSource = (XawTextSource) 0;
    }

    if (AllGroupsString != NIL(char)) {
	FREE(AllGroupsString);
	AllGroupsString = NIL(char);
    }

    if (DummySource == (XawTextSource) 0) {
	static Arg args[3] = {
	    {XtNstring, (XtArgVal) ""},
	    {XtNlength, (XtArgVal) 2},
	    {XtNeditType, (XtArgVal) XawtextRead}
	};
	
	DummySource = XawStringSourceCreate(ArticleText, args, XtNumber(args));
    }
    XawTextSetSource(ArticleText, DummySource, (XawTextPosition) 0);
    switchToNewsgroupMode();
    return;
}


/*ARGSUSED*/
static void
allSubFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Make the selected group(s) subscribed to, and leave them in
 * their current position in the newsrc file.
 */
{
    char newGroup[GROUP_NAME_SIZE];
    XawTextPosition gbeg, left, right;

    if (Mode != ALL_MODE) {
	return;
    }
    if (getSelection(ArticleText, AllGroupsString, &left, &right)) {
	gbeg = left;
	while (gbeg <= right) {
	    currentGroup(Mode, AllGroupsString, newGroup, gbeg);
	    if (gotoNewsgroup(newGroup) != GOOD_GROUP) {
		mesgPane("Bad group: %s\n", newGroup);
		return;
	    }
	    subscribe();
	    if (!moveCursor(FORWARD, AllGroupsString, &gbeg)) {
		break;
	    }
	}
	markAllString(AllGroupsString, left, right, SUB_STRING);
	updateAllWidget(left, right);
    } else {
	(void) moveUpWrap(AllGroupsString, &left);
	XawTextSetInsertionPoint(ArticleText, left);
    }
    return;
}


/*ARGSUSED*/
static void
allFirstFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Mark the selected group(s) as subscribed to, and move them to the
 * beginning of the newsrc file.
 */
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    XawTextPosition gbeg, left, right;

    if (Mode != ALL_MODE) {
	return;
    }
    if (getSelection(ArticleText, AllGroupsString, &left, &right)) {
	gbeg = left;
	currentGroup(Mode, AllGroupsString, newGroup, gbeg);
	(void) addToNewsrcBeginning(newGroup, SUBSCRIBE);
	(void) strcpy(oldGroup, newGroup);
	(void) moveCursor(FORWARD, AllGroupsString, &gbeg);
	while (gbeg <= right) {
	    currentGroup(Mode, AllGroupsString, newGroup, gbeg);
	    (void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
	    (void) strcpy(oldGroup, newGroup);
	    if (!moveCursor(FORWARD, AllGroupsString, &gbeg)) {
		break;
	    }
	}
	markAllString(AllGroupsString, left, right, SUB_STRING);
	if (AllStatus == 0) {
	    redrawAllWidget((XawTextPosition) 0);
	} else {
	    updateAllWidget(left, right);
	}
    } else {
	(void) moveUpWrap(AllGroupsString, &left);
	XawTextSetInsertionPoint(ArticleText, left);
	return;
    }
    return;
}


/*ARGSUSED*/
static void
allLastFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Mark the selected group(s) as subscribed to, and move them
 * to the end of the newsrc file.
 */
{
    char newGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    XawTextPosition gbeg, left, right;

    if (Mode != ALL_MODE) {
	return;
    }
    if (getSelection(ArticleText, AllGroupsString, &left, &right)) {
	gbeg = left;
	while (gbeg <= right) {
	    currentGroup(Mode, AllGroupsString, newGroup, gbeg);
	    (void) addToNewsrcEnd(newGroup, status);
	    if (!moveCursor(FORWARD, AllGroupsString, &gbeg)) {
		break;
	    }
	}
	markAllString(AllGroupsString, left, right, SUB_STRING);
	if (AllStatus == 0) {
	    redrawAllWidget(left);
	} else {
	    updateAllWidget(left, right);
	}
    } else {
	(void) moveUpWrap(AllGroupsString, &left);
	XawTextSetInsertionPoint(ArticleText, left);
	return;
    }
    return;
}


static Widget AllBox = (Widget) 0;

/*ARGSUSED*/
static void
allHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Mark the selected group(s) as subscribed to, and place them
 * after the group name (entered in the dialog box) in the newsrc file.
 */
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    XawTextPosition gbeg, left, right;
    int all = 0;

    busyCursor(widget, 0, 0);
    if (getSelection(ArticleText, AllGroupsString, &left, &right)) {
	if ((int) client_data == XRNadd_ADD) {
	    gbeg = left;
	    currentGroup(Mode, AllGroupsString, newGroup, gbeg);
	    if (addToNewsrcAfterGroup(newGroup,
				      GetDialogValue(AllBox),
				      SUBSCRIBE) == GOOD_GROUP) {
		(void) moveCursor(FORWARD, AllGroupsString, &gbeg);
		while (gbeg <= right) {
		    (void) strcpy(oldGroup, newGroup);
		    currentGroup(Mode, AllGroupsString, newGroup, gbeg);
		    (void) addToNewsrcAfterGroup(newGroup, oldGroup, SUBSCRIBE);
		    if (!moveCursor(FORWARD, AllGroupsString, &gbeg)) {
			break;
		    }
		}
		markAllString(AllGroupsString, left, right, SUB_STRING);
		if (AllStatus == 0) {
		    redrawAllWidget(left);
		} else {
		    updateAllWidget(left, right);
		}
		all = 1;
	    }
	}
    } else {
	(void) moveUpWrap(AllGroupsString, &left);
    }
    if (!all) {
	XawTextSetInsertionPoint(ArticleText, left);
    }
    PopDownDialog(AllBox);
    AllBox = 0;
    unbusyCursor(widget, 0, 0);
    return;
}


/*ARGSUSED*/
static void
allAfterFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Put up a dialog box for the user to enter a group name after which
 * the selected articles should be placed.
 */
{
    static struct DialogArg args[] = {
	{"abort",     allHandler, (caddr_t) XRNadd_ABORT},
	{"subscribe", allHandler, (caddr_t) XRNadd_ADD},
    };

    if (Mode != ALL_MODE) {
	return;
    }
    if (AllBox == (Widget) 0) {
	AllBox = CreateDialog(topLevel, "After which newsgroup?",
				  DIALOG_TEXT, args, XtNumber(args));
    }
    PopUpDialog(AllBox);
    return;
}


/*ARGSUSED*/
static void
allUnsubFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Mark the selected group(s) as unsubscribed, leaving their position
 * in the newsrc file unchanged.
 */
{
    char newGroup[GROUP_NAME_SIZE];
    XawTextPosition gbeg, left, right;

    if (Mode != ALL_MODE) {
	return;
    }
    if (getSelection(ArticleText, AllGroupsString, &left, &right)) {
	gbeg = left;
	while (gbeg <= right) {
	    currentGroup(Mode, AllGroupsString, newGroup, gbeg);
	    if (gotoNewsgroup(newGroup) != GOOD_GROUP) {
		mesgPane("Bad group: %s\n", newGroup);
		return;
	    }
	    unsubscribe();
	    if (!moveCursor(FORWARD, AllGroupsString, &gbeg)) {
		return;
	    }
	}
	markAllString(AllGroupsString, left, right, UNSUB_STRING);
	updateAllWidget(left, right);
    } else {
	(void) moveCursor(BACK, AllGroupsString, &left);
	XawTextSetInsertionPoint(ArticleText, left);
    }
    return;
}


/*ARGSUSED*/
static void
allGotoFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Go to the current newsgroup.  The current
 * group is either the first group of a selection,
 * or, if there is no selection, the group the cursor
 * is currently on (if any).
 */
{
    char newGroup[GROUP_NAME_SIZE];
    XawTextPosition left, right;

    if (Mode != ALL_MODE) {
	return;
    }

    /* get the current group name */

    if (getSelection(ArticleText, AllGroupsString, &left, &right)) {
	currentGroup(Mode, AllGroupsString, newGroup, left);
    } else {
	/* if at the end of the string, move to the beginning and quit */
	(void) moveUpWrap(AllGroupsString, &left);
	XawTextSetInsertionPoint(ArticleText, left);
	return;
    }

    /* jump to the newsgroup */

    if (jumpToNewsgroup(newGroup) == GOOD_GROUP) {
	(void) strcpy(LastGroup, newGroup);
	
	/* free source */

	if (AllSource != (XawTextSource) 0) {
	    XawStringSourceDestroy(AllSource);
	    AllSource = (XawTextSource) 0;
	}

	/* free string */

	if (AllGroupsString != NIL(char)) {
	    FREE(AllGroupsString);
	    AllGroupsString = NIL(char);
	}
	switchToArticleMode();
	return;
    }
    return;
}


/*ARGSUSED*/
static void
allSelectFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Make note of the groups that were selected
 * to be moved.
 */
{
    if (Mode != ALL_MODE) {
	return;
    }
    (void) getSelection(Text, AllGroupsString, &First, &Last);
    return;
}


/*ARGSUSED*/
static void
allMoveFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * Move the groups in the last selection to
 * the current cursor position (before the
 * current selection.
 */
{
    char newGroup[GROUP_NAME_SIZE];
    char oldGroup[GROUP_NAME_SIZE];
    int status = SUBSCRIBE;
    int dummy;
    char *newString;
    XawTextPosition left, right;
    XawTextPosition stringPoint;
    XawTextPosition cursorSpot;
    int direction = 0;
    int numGroups = 0;

    if (Mode != ALL_MODE) {
	return;
    }
    if (First == Last) {
	mesgPane("No groups were selected");
	return;
    }
    buildString(&newString, First, Last, AllGroupsString);
    stringPoint = 0;
    (void) getSelection(Text, AllGroupsString, &left, &right);
    if ((left >= First) &&(left <= Last+1)) {
	mesgPane("Selected groups have not been moved");
	resetSelection();
	return;
    }
    GroupPosition = cursorSpot = left;
    if (left > First) {
	direction = 1;
    }
    currentMode(newString, newGroup, &status, stringPoint);
    if (!moveCursor(BACK, AllGroupsString, &left)) {
	(void) addToNewsrcBeginning(newGroup, status);
	(void) strcpy(oldGroup, newGroup);
	(void) moveCursor(FORWARD, newString, &stringPoint);
	numGroups++;
    } else {
	currentMode(AllGroupsString, oldGroup, &dummy, left);
	(void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
	(void) strcpy(oldGroup, newGroup);
	(void) moveCursor(FORWARD, newString, &stringPoint);
	numGroups++;
    }
    while (newString[stringPoint] != '\0') {
	numGroups++;
	currentMode(newString, newGroup, &status, stringPoint);
	(void) addToNewsrcAfterGroup(newGroup, oldGroup, status);
	(void) strcpy(oldGroup, newGroup);
	if (!moveCursor(FORWARD, newString, &stringPoint)) {
	    break;
	}
    }
    redrawAllWidget(left);
    if (direction) {
	while (numGroups > 0) {
	    GroupPosition = cursorSpot;
	    (void) moveCursor(BACK, AllGroupsString, &GroupPosition);
	    numGroups--;
	}
	adjustNewsgroupWidget();
    }
    resetSelection();
    return;
}


/*ARGSUSED*/
static void
allToggleFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/* 
 * Change the order the groups appear on the screen.
 */
{
    Arg args[3];
    
    if (Mode != ALL_MODE) {
	return;
    }
    /* Destroy source */

    if (AllSource != (XawTextSource) 0) {
	XawStringSourceDestroy(AllSource);
	AllSource = (XawTextSource) 0;
    }

    /* Destroy string */

    if (AllGroupsString != NIL(char)) {
	FREE(AllGroupsString);
	AllGroupsString = NIL(char);
    }

    AllStatus = (AllStatus == 0) ? 1 : 0;

    /* make the new string and source */

    AllGroupsString = getStatusString(AllStatus);

    XtSetArg(args[0], XtNstring, AllGroupsString);
    XtSetArg(args[1], XtNlength, utStrlen(AllGroupsString) + 1);
    XtSetArg(args[2], XtNeditType, XawtextRead);
    
    AllSource = XawStringSourceCreate(ArticleText, args, XtNumber(args));
    XawTextSetSource(ArticleText, AllSource, (XawTextPosition) 0);
    XtSetValues(ArticleText, lineSelArgs, XtNumber(lineSelArgs));
    return;
}


/*
 * public routines
 */

void
determineMode()
/*
 * determine the initial mode and set up Text, TopButtonBox, and Question
 */
{
    /* set mode, handle text and question */
    PreviousMode = Mode;
    
    if ((AddGroupsString = newGroups()) != NIL(char)) {
	Mode = ADD_MODE;
	setTopInfoLine("Select groups to `add', `quit' unsubscribes remaining groups");
	redrawAddTextWidget((XawTextPosition) 0);
    } else {
	Mode = NEWSGROUP_MODE;

	redrawNewsgroupTextWidget();

	/* update the question */
	if (utStrlen(NewsGroupsString) == 0) {
	    setTopInfoLine("No more unread articles in the subscribed to newsgroups");
	} else {
	    setTopInfoLine("Operations apply to current selection or cursor position");
	}
    }
    setBottomInfoLine("");
    /*
     * article specific buttons, only create them if this is the first
     * call to 'determineMode'
     */
    if (PreviousMode == NO_MODE) {
	createButtons();
	XtManageChildren(ArtSpecButtons, XtNumber(ArtSpecButtonList));
    }
    
    /* switch buttons */
    swapMode();
    
    return;
}
