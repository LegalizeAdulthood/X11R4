
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/compose.c,v 1.15 89/11/17 12:48:26 ricks Rel Locker: ricks $";
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
 * compose.c: routines for composing messages and articles
 */

#include "copyright.h"
extern char *getinfofromfile(/* char *filename */);
#include <X11/Xos.h>
#include <ctype.h>
#include <stdio.h>
#include "utils.h"
#include "config.h"
#include <sys/stat.h>
#ifndef VMS
#include <pwd.h>
#else
#define index strchr
#endif
#ifdef aiws
struct passwd *getpwuid();
struct passwd *getpwnam();
#endif /* aiws */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/cursorfont.h>
#include "avl.h"
#include "xthelper.h"
#include "resources.h"
#include "dialogs.h"
#include "server.h"
#include "mesg.h"
#include "news.h"
#include "internals.h"
#include "xrn.h"
#include "compose.h"


/* entire pane */
static Widget ComposeTopLevel = (Widget) 0;
/* text window */
static Widget ComposeText = (Widget) 0;
static XawTextPosition InsertionPoint = (XawTextPosition) 0;

static char CompositionString[COMPOSITION_SIZE];

#define BUFFER_SIZE 1024

#define POST     0
#define FOLLOWUP 1
#define REPLY    2
#define FORWARD  3
#define GRIPE    4
static int PostingMode = POST;

struct header {
    art_num article;
    char *artFile;
    char *newsgroups;
    char *subject;
    char *messageId;
    char *followupTo;
    char *references;
    char *from;
    char *replyTo;
    char *distribution;
    char *keywords;

    char *user;
    char *fullname;
    char *host;
    char *path;
};


/*
 * storage for the header information needed for building, checking,
 * and repairing the header.  Once this is created, the user can go
 * to another article or another group and still use the composition
 * window
 */

static struct header Header = {0, NIL(char), NIL(char), NIL(char), NIL(char), NIL(char),
                               NIL(char), NIL(char), NIL(char), NIL(char), NIL(char),
 NIL(char)};


#define HOST_NAME_SIZE 1024


static void
getHeader(article)
art_num article;
/*
 * get the header and other important information for an article
 *
 * if called with article equal to zero, will only set up the non-article
 * specific entries
 *
 */
{
    struct passwd *pwd;
    char host[HOST_NAME_SIZE], buffer[BUFFER_SIZE], *ptr;
    char path[HOST_NAME_SIZE];
    char *getenv();
    
    if (article > 0) {
	Header.article = article;
#ifdef PREFETCH
	/*
	 * handle the case where this is a request at the last
	 * article and the next newsgroup has been prefetched.
	 *
	 * must go back to the current newsgroup so the header information
	 * can be retrieved from the server
	 */
	if (!invalidatePrefetchedGroup(Newsrc[CurrentGroupNumber])) {
	    mesgPane("The newsgroup has been deleted out from underneath us");
	} else {
#endif /* PREFETCH */
	    xhdr(article, "newsgroups", &Header.newsgroups);
	    xhdr(article, "subject", &Header.subject);
	    xhdr(article, "message-id", &Header.messageId);
	    xhdr(article, "followup-to", &Header.followupTo);
	    xhdr(article, "references", &Header.references);
	    xhdr(article, "from", &Header.from);
	    xhdr(article, "reply-to", &Header.replyTo);
	    xhdr(article, "distribution", &Header.distribution);
	    xhdr(article, "keywords", &Header.keywords);

#ifdef PREFETCH
	}
#endif /* PREFETCH */

	/*
	 * since I'm lazy down below, I'm going to replace NIL pointers with ""
	 */
	if (Header.newsgroups == NIL(char)) {
	    Header.newsgroups = XtNewString("");
	}
	if (Header.subject == NIL(char)) {
	    Header.subject = XtNewString("");
	}
	if (Header.messageId == NIL(char)) {
	    Header.messageId = XtNewString("");
	}
	if (Header.followupTo == NIL(char)) {
	    Header.followupTo = XtNewString("");
	}
	if (Header.references == NIL(char)) {
	    Header.references = XtNewString("");
	}
	if (Header.from == NIL(char)) {
	    Header.from = XtNewString("");
	}
	if (Header.replyTo == NIL(char)) {
	    Header.replyTo = XtNewString("");
	}
	if (Header.distribution == NIL(char)) {
	    Header.distribution = XtNewString("");
	}
	if (Header.keywords == NIL(char)) {
	    Header.keywords = XtNewString("");
	}

    } else {
	/* information for 'post' */
	if (CurrentGroupNumber != NO_GROUP) {
	    Header.newsgroups = XtNewString(Newsrc[CurrentGroupNumber]->name);
	} else {
	    Header.newsgroups = XtNewString("");
	}
    }

    host[0] = 0;
#ifdef	UUCPNAME
    {
        FILE * uup;
    
        if ((uup = fopen(UUCPNAME, "r")) != NULL) {
    		char   *p;
    		char    xbuf[BUFSIZ];
    
    		while (fgets(xbuf, sizeof(xbuf), uup) != NULL) {
    		    if (*xbuf <= ' ' || *xbuf == '#')
    			continue;
    		    break;
    		}
    		if (p = index(xbuf, '\n'))
    		    *p = 0;
    		(void) strncpy(host, xbuf, sizeof(host) - 1);
    		(void) fclose(uup);
        }
    }
#endif
    if (host[0] == 0) {
        if ((ptr = getenv("HIDDENHOST")) != NIL(char)) {
	    (void) strncpy(host, ptr, sizeof(host)-1);
        } else if ((ptr = getinfofromfile(HIDDEN_FILE)) != NULL) {
	    (void) strncpy(host, ptr, sizeof(host) - 1);
	} else {
#ifndef VMS
	    (void) gethostname(host, sizeof(host));
#else
	    ptr = getenv("SYS$NODE");
	    (void) strcpy(host, ptr);
	    ptr = index(host, ':');
	    if (ptr != NIL(char)) {
		*ptr = '\0';
	    }
	    (void) strcat(host, DOMAIN_NAME);
	    XmuCopyISOLatin1Lowered(host, host);
#endif
	}
    }
    
    if ((ptr = getenv("HIDDENPATH")) != NIL(char)) {
	(void) strncpy(path, ptr, sizeof(path)-1);
    } else if ((ptr = getinfofromfile(PATH_FILE)) != NULL) {
	(void) strncpy(path, ptr, sizeof(path)-1);
    } else {
	(void) strncpy(path, host, sizeof(path)-1);
    }
    
    /* If the host name is not a full domain name, put in the domain */
    if (index (host, '.') == NIL(char)) {
        char   *domain;
    
        if ((domain = getenv ("DOMAIN")) != NIL (char)) {
	    (void) strcat(host, domain);
	} else if ((domain = getinfofromfile(DOMAIN_FILE)) != NULL) {
	    (void) strcat(host, domain);
        } else {
	    (void) strcat (host, DOMAIN_NAME);
	}
    }

    Header.host = XtNewString(host);
    Header.path = XtNewString(path);
#ifndef VMS
    pwd = getpwuid(getuid());
    Header.user = XtNewString(pwd->pw_name);
    Header.fullname = XtNewString(pwd->pw_gecos);
#else
    Header.user = XtNewString(getenv("USER"));
    XmuCopyISOLatin1Lowered(Header.user, Header.user);
    if ((Header.fullname = getenv("FULLNAME")) == NIL (char))
	Header.fullname = XtNewString("");
#endif
    ptr = index(Header.fullname, ',');
    if (ptr != NIL(char)) {
	*ptr = '\0';
    }
    
    /* & expansion */
    ptr = index(Header.fullname, '&');
    if (ptr != NIL(char)) {
	char *p = buffer + (ptr - Header.fullname);

	buffer[0] = '\0';
	*ptr = '\0';
	(void) strcpy(buffer, Header.fullname);
	(void) strcat(buffer, Header.user);
	if (isascii(*p)) {
	    *p = toupper(*p);
	}
	ptr++;
	(void) strcat(buffer, ptr);
	FREE(Header.fullname);
	Header.fullname = XtNewString(buffer);
    }
    
    return;
}


static int
fieldExists(string, fieldName)
char *string;
char *fieldName;
/*
 * see if a field exists in the header that begins with `fieldName'
 *
 * returns 0 for no, non-zero for yes
 */
{
    char *ptr;

    
    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0'); ptr++) {
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    /* end of message */
	    return(0);
	}
	if ((ptr[0] == '\n') && (ptr[1] == '\n')) {
	    /* end of header */
	    return(0);
	}

	/*
	 * see if the field name is in the string and at the beginning
	 * of the line and that the field name ends with a ':'
	 */
	if (((ptr == string) || (*(ptr-1) == '\n')) &&
	    STREQN(fieldName, ptr, utStrlen(fieldName)) &&
	    (ptr[utStrlen(fieldName)] == ':')) {
	    return(1);
	}
    }
    return(0);
}


static void
addField(string, field)
char *string;
char *field;
/*
 * add a header field to a message.
 * this is a destructive operation.
 */
{
    char buffer[COMPOSITION_SIZE];


    (void) strcpy(buffer, string);
    string[0] = '\0';
    (void) strcat(string, field);
    (void) strcat(string, buffer);

    return;
}    


static void
stripField(string, fieldName)
char *string;
char *fieldName;
/*
 * remove all fields from a header that begin with `fieldName'.
 * this is a destructive operation.
 */
{
    char *ptr, *nl;


    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0'); ptr = index(ptr, '\n')) {
	if (ptr != string) {
	    /* character after the newline */
	    ptr++;
	}
	
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    /* end of message */
	    return;
	}
	if ((ptr[0] == '\n') && (ptr[1] == '\n')) {
	    /* end of header */
	    return;
	}

	/*
	 * make sure a ':' is at the end of the field name
	 */
	if (STREQN(fieldName, ptr, utStrlen(fieldName)) && (ptr[utStrlen(fieldName)] == ':')) {
	    nl = index(ptr, '\n');
	    (void) strcpy(ptr, ++nl);
	    if (*ptr == '\n') {
		return;
	    }
	    ptr--;
	}
    }
    return;
}


#ifdef VMS
static void
returnField(string, fieldName, removed)
char *string;
char *fieldName;
char *removed;
/*
 * remove fields from a header that begin with `fieldName'
 * and return the removed characters in `removed'.
 * this is a destructive operation.
 */
{
    char *ptr, *nl, *copyPtr;

    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0');  ptr = index(++ptr, '\n')) {
	if (ptr != string) {
	    /* character after the newline */
	    ptr++;
	}
	
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    /* end of message */
	    return;
	}
	if ((ptr[0] == '\n') && (ptr[1] == '\n')) {
	    /* end of header */
	    return;
	}

	/*
	 * make sure a ':' is at the end of the field name
	 */
	if (STREQN(fieldName, ptr, utStrlen(fieldName)) && (ptr[utStrlen(fieldName)] == ':')) {
	    nl = index(ptr, '\n');
	    for (copyPtr = ptr + utStrlen(fieldName) + 1; copyPtr < nl; copyPtr++)
	         *removed++ = *copyPtr;
	    *removed = '\0';
	    (void) strcpy(ptr, ++nl);
	    if (*ptr == '\n') {
		return;
	    }
	    ptr--;
	}
    }
    return;
}
#endif /* VMS */

static void
destroyCompositionTopLevel()
{
    XtPopdown(ComposeTopLevel);
    XtDestroyWidget(ComposeTopLevel);
    ComposeTopLevel = (Widget) 0;

    return;
}

static void
freeHeader()
{
    FREE(Header.artFile);
    FREE(Header.newsgroups);
    FREE(Header.subject);
    FREE(Header.messageId);
    FREE(Header.followupTo);
    FREE(Header.references);
    FREE(Header.from);
    FREE(Header.replyTo);
    FREE(Header.distribution);
    FREE(Header.keywords);
    FREE(Header.user);
    FREE(Header.fullname);
    FREE(Header.host);
    FREE(Header.path);
    return;
}


static void
buildSubject(message)
char *message;
/*
 * add a subject field to the header of a message.
 * deal with supressing multiple '[rR][eE]: ' strings
 */
{
    if (STREQN(Header.subject, "Re: ", 4) ||
	STREQN(Header.subject, "RE: ", 4) ||
	STREQN(Header.subject, "re: ", 4)) {
	(void) strcat(message, "Subject: ");
    } else {
	(void) strcat(message, "Subject: Re: ");
    }
    (void) strcat(message, Header.subject);
    (void) strcat(message, "\n");

    return;
}


/*ARGSUSED*/
static void
abortFunction(data)
caddr_t data;
{
    destroyCompositionTopLevel();
    freeHeader();
    infoNow("Article/Message aborted");
    return;
}


/*ARGSUSED*/
static void
saveFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    FILE *fp;
    
    if ((fp = fopen(utTildeExpand(app_resources.savePostings), "a")) != NULL) {
	(void) sprintf(error_buffer, "Saving in %s", app_resources.savePostings);
	infoNow(error_buffer);
	
	(void) fwrite(CompositionString, sizeof(char), utStrlen(CompositionString), fp);
	(void) putc('\n', fp);
	(void) putc('\n', fp);
	(void) fclose(fp);
    } else {
	mesgPane("Can not save the article/message");
    }
    return;
}


static void
saveDeadLetter()
{
    FILE *fp;
    
    if ((fp = fopen(utTildeExpand(app_resources.deadLetters), "a")) != NULL) {
	(void) fwrite(CompositionString, sizeof(char), utStrlen(CompositionString), fp);
	(void) putc('\n', fp);
	(void) putc('\n', fp);
	(void) fclose(fp);
    } else {
	mesgPane("Can not save the dead article/message");
    }
    return;
}


/*ARGSUSED*/
static void
stripBlankFields(string)
char *string;
/*
 * go through the string and look for fields that have not
 * been filled in and remove them
 */
{
    char *ptr, *nl, *end;

    for (ptr = string; (ptr != NIL(char)) && (*ptr != '\0'); ptr = index(ptr, '\n')) {
	if ((ptr[0] == '\n') && (ptr[1] == '\0')) {
	    /* end of message */
	    return;
	}
	if ((ptr[0] == '\n') && (ptr[1] == '\n')) {
	    /* end of header */
	    return;
	}
	
	if (ptr != string) {
	    /* character after the newline */
	    ptr++;
	}
	
	/* get the line */
	if ((nl = index(ptr, '\n')) == NIL(char)) {
	    /* weird */
	    return;
	}

	/* ignore trailing spaces */
	end = nl;
	end--;
	for ( ; *end == ' '; end--) ;
	
	if ((*end == ':') && (end == index(ptr, ':'))) {
	    /* blank field */
	    (void) strcpy(ptr, ++nl);
	    if (*ptr == '\n') {
		return;
	    }
	    ptr--;
	}
    }
    return;
}    

#ifdef VMS
static Widget mailErrorDialog = NULL;

static void
popDownMail(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    PopDownDialog(mailErrorDialog);
    mailErrorDialog = NULL;
    return;
}


static void
mailError(status)
int status;
{
#include <descrip.h>
    static struct DialogArg args[] = {
	{"Click to continue", popDownMail, (caddr_t) -1},
    };
    char	VMSmessage[255];
    char	message[255];
    struct dsc$descriptor_s messageText;

    if (app_resources.editorCommand == NIL(char)) {
	destroyCompositionTopLevel();
    }
    freeHeader();

    messageText.dsc$b_class = DSC$K_CLASS_S;
    messageText.dsc$b_dtype = DSC$K_DTYPE_T;
    messageText.dsc$w_length = 255;
    messageText.dsc$a_pointer = &VMSmessage;

    sys$getmsg(status, &messageText, &messageText, 0, 0);
    VMSmessage[messageText.dsc$w_length] = '\0';
    (void) strcpy(message,"Error sending mail:");
    (void) strcat(message,VMSmessage);
    mailErrorDialog = CreateDialog(topLevel, message, DIALOG_NOTEXT, args, XtNumber(args));
    PopUpDialog(mailErrorDialog);
    return;
}
#endif /* VMS */

/*ARGSUSED*/
static void
sendFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    char buffer[BUFFER_SIZE];
#ifndef VMS
    FILE *fp;
    extern FILE *popen();	/* sequent */
#else
#include <maildef.h>
    struct _itemList {
	short item_size;
	short item_code;
	int   item_ptr;
	int   item_rtn_size;
	int   end_list;
    } itemList;
	 
    char toString[BUFFER_SIZE];
    char subjString[BUFFER_SIZE];
    int	context, status;
    char *textPtr, *nl;
#endif

    stripBlankFields(CompositionString);
    
    if ((PostingMode == POST) || (PostingMode == FOLLOWUP)) {
	if (PostingMode == FOLLOWUP) {
	    if (!fieldExists(CompositionString, "References")) {
		(void) sprintf(buffer, "References: %s\n", Header.messageId);
		addField(CompositionString, buffer);
	    }
	}

	if (!fieldExists(CompositionString, "From")) {
	    (void) sprintf(buffer, "From: %s@%s (%s)\n",
			   Header.user, Header.host, Header.fullname);
	    addField(CompositionString, buffer);
	}

	if (app_resources.editorCommand == NIL(char)) {
	    if (!fieldExists(CompositionString, "Subject")) {
		if (PostingMode == POST) {
		    XBell(XtDisplay(topLevel), 50);
		    mesgPane("missing Subject field!");
		    XawTextSetLastPos(ComposeText, (XawTextPosition) utStrlen(CompositionString));
		    XawTextSetInsertionPoint(ComposeText, (XawTextPosition) 0);
		    return;
		}
		buffer[0] = '\0';
		buildSubject(buffer);
		addField(CompositionString, buffer);
	    }
	}

	if (!fieldExists(CompositionString, "Newsgroups")) {
	    (void) sprintf(buffer, "Newsgroups: %s\n", Header.newsgroups);
	    addField(CompositionString, buffer);
	}

	if (!fieldExists(CompositionString, "Path")) {
	    (void) sprintf(buffer, "Path: %s!%s\n", Header.path, Header.user);
	    addField(CompositionString, buffer);
	}

	stripField(CompositionString, "Message-ID");
	stripField(CompositionString, "Relay-Version");
	stripField(CompositionString, "Posting-Version");

	switch (postArticle(CompositionString)) {
	    case POST_FAILED:
	    XBell(XtDisplay(topLevel), 50);
	    mesgPane("Could not post, saving to %s", app_resources.deadLetters);
	    saveDeadLetter();
	    return;

	    case POST_NOTALLOWED:
	    mesgPane("Posting not allowed from this machine, saving to %s",
			   app_resources.deadLetters);
	    break;
	    
	    case POST_OKAY:
	    infoNow("Article Posted");
	    break;
	}
	
    } else {
#ifndef VMS
	if ((fp = popen(app_resources.mailer, "w")) != NULL) {
	    /* XXX bug!  should do some line wrapping here ... */
	    (void) fwrite(CompositionString, sizeof(char), utStrlen(CompositionString), fp);
	    (void) pclose(fp);
	    infoNow("Message sent");
	} else {
	    mesgPane("Could not reply, saving to %s", app_resources.deadLetters);
	    saveDeadLetter();
	}
#else
	returnField(CompositionString, "To", toString);
	returnField(CompositionString, "Subject", subjString);
	context = 0;
	status = MAIL$SEND_BEGIN(&context, &0, &0);
	if ((status&1) != 1) {
	   mailError(status);
	   return;
	}
	itemList.item_code = MAIL$_SEND_TO_LINE;
	itemList.item_size = strlen(toString);
	itemList.item_ptr = &toString;
	itemList.item_rtn_size = 0;
	itemList.end_list = 0;
	status = MAIL$SEND_ADD_ATTRIBUTE(&context, &itemList, &0);
	if ((status&1) != 1) {
	   mailError(status);
	   return;
	}
	itemList.item_code = MAIL$_SEND_SUBJECT;
	itemList.item_size = strlen(subjString);
	itemList.item_ptr = &subjString;
	itemList.item_rtn_size = 0;
	itemList.end_list = 0;
	status = MAIL$SEND_ADD_ATTRIBUTE(&context, &itemList, &0);
	if ((status&1) != 1) {
	   mailError(status);
	   return;
	}
/*
 * Iterate over the composition string, extracting records
 * delimited by newlines and sending each as a record
 *
 */
	textPtr = CompositionString;
	if (*textPtr == '\n')
	    textPtr++;

	for (;;) {
/* Find the newline or end of string */
	    for (nl = textPtr; (*nl != '\0') && (*nl != '\n'); nl++);
	    itemList.item_code = MAIL$_SEND_RECORD;
	    itemList.item_size = nl - textPtr;
	    itemList.item_ptr = textPtr;
	    itemList.item_rtn_size = 0;
	    itemList.end_list = 0;
	    status = MAIL$SEND_ADD_BODYPART(&context, &itemList, &0);
	    if ((status&1) != 1) {
		mailError(status);
		return;
	    }
	
	    if (*nl == '\0')
		break;			/* end of string */
	    textPtr = ++nl;		/* skip the newline */
	}
	itemList.item_code = MAIL$_SEND_USERNAME;
	itemList.item_size = strlen(toString);
	itemList.item_ptr = &toString;
	itemList.item_rtn_size = 0;
	itemList.end_list = 0;
	status = MAIL$SEND_ADD_ADDRESS(&context, &itemList, &0);
	if ((status&1) != 1) {
	   mailError(status);
	   return;
	}
	status = MAIL$SEND_MESSAGE(&context, &0, &0);
	if ((status&1) != 1) {
	   mailError(status);
	   return;
	}
	infoNow("Message sent");
#endif
    }

    if (app_resources.editorCommand == NIL(char)) {
	destroyCompositionTopLevel();
    }
    freeHeader();

    return;
}


static void
includeArticleText()
{
    char line[256], *end, buffer[COMPOSITION_SIZE];
    FILE *artfp;

    (void) strcpy(buffer, &CompositionString[(int) InsertionPoint]);
    CompositionString[(int) InsertionPoint] = '\0';


    if (PostingMode == REPLY) {
       (void) sprintf(line, "In article %s you write:\n",
		Header.messageId );
    } else {
       (void) sprintf(line, "In article %s, %s writes:\n",
		   Header.messageId, Header.from);
    }

    (void) strcat(CompositionString, line);
    end = CompositionString + utStrlen(CompositionString);

    /* copy the article */
    if ((artfp = fopen(Header.artFile, "r")) != NULL) {

	/* skip header */
	if (app_resources.includeHeader != TRUE) {
	    while (fgets(line, 256, artfp) != NULL) {
		if (line[0] == '\n') {
		    break;
		}
	    }
	}

	/* copy article */
	while (fgets(line, 256, artfp) != NULL) {
	    if (app_resources.includeSep == TRUE) {
		(void) strcat(end, "> ");
		end += 2;
	    }
	    (void) strcat(end, line);
	    end += utStrlen(line);
	}
	(void) fclose(artfp);
    }

    /* new insertion point */
    InsertionPoint = end - CompositionString;

    /* copy back the text after the insertion point */
    (void) strcat(end, buffer);
    return;
}


/*ARGSUSED*/
static void
includeArticleFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    if (PostingMode == POST) {
	return;
    }
    
    InsertionPoint = XawTextGetInsertionPoint(ComposeText);
    includeArticleText();
    XawTextSetLastPos(ComposeText, (XawTextPosition) utStrlen(CompositionString));
    XawTextSetInsertionPoint(ComposeText, InsertionPoint);
    
    return;
}

#define XRNinclude_ABORT          0
#define XRNinclude_DOIT           1

static Widget IncludeBox = (Widget) 0;  /* box for typing in the name of a file */
static char *IncludeString = NULL; /* last input string */

/*ARGSUSED*/
static void
includeHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
/*
 * handler for the include box
 */
{
    char *end, buffer[COMPOSITION_SIZE], *file;
    int c;
    FILE *filefp;

    if ((int) client_data != XRNinclude_ABORT) {
	(void) strcpy(buffer, &CompositionString[(int) InsertionPoint]);
	CompositionString[(int) InsertionPoint] = '\0';
	end = CompositionString + InsertionPoint;

	/* copy the file */
	file = GetDialogValue(IncludeBox);
	if ((filefp = fopen(utTildeExpand(file), "r")) != NULL) {
	    while ((c = getc(filefp)) != EOF) {
		*end = c;
		end++;
	    }
	    *end = '\0';
	    (void) fclose(filefp);

	    /* new insertion point */
	    InsertionPoint = end - CompositionString;

	    /* copy back the text after the insertion point */
	    (void) strcat(end, buffer);

	    XawTextSetLastPos(ComposeText,
			     (XawTextPosition) utStrlen(CompositionString));
	    XawTextSetInsertionPoint(ComposeText, InsertionPoint);

	} else {
	    mesgPane("Can not open `%s'", file);
	}
	XtFree(IncludeString);
	IncludeString = XtNewString(file);
    }

    PopDownDialog(IncludeBox);
    IncludeBox = 0;

    return;
}    


/*ARGSUSED*/
static void
includeFileFunction(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    int len;
    static struct DialogArg args[] = {
	{"abort", includeHandler, (caddr_t) XRNinclude_ABORT},
	{"doit",  includeHandler, (caddr_t) XRNinclude_DOIT},
    };

    InsertionPoint = XawTextGetInsertionPoint(ComposeText);
    
    if (IncludeBox == (Widget) 0) {
	IncludeBox = CreateDialog(topLevel, "    File Name?    ",
				  IncludeString == NULL ? DIALOG_TEXT
				   : IncludeString, args, XtNumber(args));
    }
    PopUpDialog(IncludeBox);

    return;
}


#ifdef lint
#define BUTTON(nm,lbl)
#else
#ifdef __STDC__
#define BUTTON(nm,lbl)				\
static XtCallbackRec nm##Callbacks[] = {	\
    {nm##Function, NULL},			\
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
    {nm/**/Function, NULL},			\
    {NULL, NULL}				\
};						\
static Arg nm/**/Args[] = {			\
    {XtNname, (XtArgVal) "nm"},			\
    {XtNlabel, (XtArgVal) "lbl"},		\
    {XtNcallback, (XtArgVal) nm/**/Callbacks}	\
};
#endif
#endif


BUTTON(abort,abort);
BUTTON(send,send);
BUTTON(save,save);
BUTTON(includeArticle,include article);
BUTTON(includeFile,include file);


static void
composePane(titleString, header)
char *titleString;
char *header;
/*
 * brings up a new vertical pane, not moded, but maintains
 * enough state so that the current group and/or current
 * article can be changed
 *
 * only one compose pane at a time
 *
 * the pane consists of 3 parts: title bar, scrollable text window,
 * button box
 *
 * five functions:
 *    post article
 *    followup article
 *    reply to author
 *    include the text of the article (followup and reply)
 *    include the a file
 *    send a gripe
 *    forward a message
 */
{
    Widget pane, buttonBox, label;
    static char titleStorage[LABEL_SIZE];
    Arg fontArgs[1];
    Arg paneArgs[4];
    static Arg labelArgs[] = {
	{XtNlabel, (XtArgVal) titleStorage},
	{XtNskipAdjust, (XtArgVal) True},
    };
    static Arg boxArgs[] = {
	{XtNskipAdjust, (XtArgVal) True},
    };
    static Arg shellArgs[] = {
	{XtNinput, (XtArgVal) True},
	{XtNsaveUnder, (XtArgVal) False},
    };
    static Arg textArgs[] = {
	{XtNstring,  (XtArgVal) CompositionString},
	{XtNlength,  (XtArgVal) sizeof(CompositionString) + 1},
	{XtNtextOptions, (XtArgVal) (scrollVertical|wordBreak)},
	{XtNeditType,  (XtArgVal) XawtextEdit},
    };

    if (ComposeTopLevel != (Widget) 0) {
	mesgPane("Only one composition pane at a time");
	return;
    }

    if (app_resources.editorCommand != NIL(char)) {
	char buffer[1024], file[1024];
	FILE *filefp;
	struct stat buf, oldbuf;

#ifndef VMS
	(void) sprintf(file, "%s/xrnXXXXXXX", app_resources.tmpDir);
#else
	(void) sprintf(file, "%sxrnXXXXXXX.tmp", app_resources.tmpDir);
#endif
	(void) mktemp(file);
	
	if ((filefp = fopen(file, "w")) == NULL) {
	    mesgPane("Can not open the temp file");
	    return;
	}

	(void) strcpy(CompositionString, header);
	if ((PostingMode != POST) && (PostingMode != GRIPE)) {
	    includeArticleText();
	}	    
    
	(void) fwrite(CompositionString, sizeof(char), utStrlen(CompositionString), filefp);
	(void) fclose(filefp);

	if (stat(file, &oldbuf) == -1) {
	    mesgPane("Can not stat the temp file");
	    return;
	}
	
	/*
	 * app_resources.editorCommand is a sprintf'able string with a %s where the
	 * file name should be placed.  The result should be a command that
	 * handles all editing and windowing.
	 *
	 * Examples are:
	 *
	 *   emacsclient %s
	 *   xterm -e vi %s
	 *   xterm -e microEmacs %s
	 *
	 */
	(void) sprintf(buffer, app_resources.editorCommand, file);
#ifndef VMS
	if (system(buffer) != 0) {
	    mesgPane("Non-zero return from system, aborting");
	    return;
	}
#else
	system(buffer);
#endif
	
	if ((filefp = fopen(file, "r")) == NULL) {
	    mesgPane("Can not open the temp file");
	    return;
	}

	if (fstat(fileno(filefp), &buf) == -1) {
	    mesgPane("Can not stat the temp file");
	    return;
	}

	if (oldbuf.st_mtime == buf.st_mtime) {
	    mesgPane("No change, aborting");
	    return;
	}

	if (buf.st_size == 0) {
	    mesgPane("Zero size, aborting");
	    return;
	}

	(void) fread(CompositionString, sizeof(char), buf.st_size, filefp);
	CompositionString[buf.st_size] = '\0';
	(void) fclose(filefp);
	(void) unlink(file);

	/* pop up a confirm box */
	if (ConfirmationBox(topLevel, "Send the message?") == XRN_CB_ABORT) {
	    mesgPane("Aborting the message, saving to %s", app_resources.deadLetters);
	    saveDeadLetter();
	    return;
	}
	
	/* send to the proper processor */
	sendFunction((Widget) 0, (caddr_t) 0, (caddr_t) 0);
	
	return;
    }
    
    ComposeTopLevel = XtCreatePopupShell("Composition", topLevelShellWidgetClass,
					 topLevel, shellArgs, XtNumber(shellArgs));

    XtSetArg(paneArgs[0], XtNx, 0);
    XtSetArg(paneArgs[1], XtNy, 0);
    XtSetArg(paneArgs[2], XtNwidth, 0);
    XtSetArg(paneArgs[3], XtNheight, 0);
    XtGetValues(topLevel, paneArgs, XtNumber(paneArgs));
    pane = XtCreateManagedWidget("pane", panedWidgetClass, ComposeTopLevel, paneArgs, XtNumber(paneArgs));

    (void) strcpy(titleStorage, titleString);
    (void) strcpy(CompositionString, header);
    
    label = XtCreateManagedWidget("label", labelWidgetClass, pane,
				  labelArgs, XtNumber(labelArgs));

    /* XXX how to make a 80 column text window with scroll bar */
    ComposeText = XtCreateManagedWidget("text", asciiStringWidgetClass, 
					pane, textArgs, XtNumber(textArgs));

    buttonBox = XtCreateManagedWidget("box", boxWidgetClass, pane, boxArgs, XtNumber(boxArgs));

    XtCreateManagedWidget("abort", commandWidgetClass, buttonBox, abortArgs, XtNumber(abortArgs));
    
    XtCreateManagedWidget("send", commandWidgetClass, buttonBox, sendArgs, XtNumber(sendArgs));
    
    XtCreateManagedWidget("save", commandWidgetClass, buttonBox, saveArgs, XtNumber(saveArgs));

    if ((PostingMode != POST) && (PostingMode != GRIPE) && (PostingMode != FORWARD)) {
	XtCreateManagedWidget("includeArticle", commandWidgetClass, buttonBox, includeArticleArgs, XtNumber(includeArticleArgs));
    }
    XtCreateManagedWidget("includeFile", commandWidgetClass, buttonBox, includeFileArgs, XtNumber(includeFileArgs));

    XtRealizeWidget(ComposeTopLevel);

    if (PostingMode == FORWARD) {
	includeArticleText();
    }

    XawTextSetLastPos(ComposeText, (XawTextPosition) utStrlen(CompositionString));
    XawTextSetInsertionPoint(ComposeText, InsertionPoint);
    
    XtSetArg(fontArgs[0], XtNheight, 0);
    XtGetValues(label, fontArgs, XtNumber(fontArgs));
    XawPanedSetMinMax(label, (int) fontArgs[0].value, (int) fontArgs[0].value);
    XawPanedAllowResize(ComposeText, True);
    
    XDefineCursor(XtDisplay(ComposeTopLevel), XtWindow(ComposeTopLevel),
		  XCreateFontCursor(XtDisplay(ComposeTopLevel), XC_left_ptr));

    XtPopup(ComposeTopLevel, XtGrabNone);

    return;
}


static char *
signatureFile()
/*
 * return a string containing the contents of the users signature file
 *   (in a static buffer)
 *
 * if the signature file is bigger than MAX_SIGNATURE_SIZE, return NIL(char).
 */
{
    char *file;
    struct stat buf;
    FILE *infofp;
    long count;
    static char *info;
    static initialized = 0;

    if (!initialized) {

	info = NIL(char);
	
	if ((file = utTildeExpand(app_resources.signatureFile)) == NIL(char)) {
	    return(NIL(char));
	}
	if (stat(file, &buf) == -1) {
	    return(NIL(char));
	}
	if (buf.st_size > MAX_SIGNATURE_SIZE) {
	    return(NIL(char));
	}
	if ((infofp = fopen(file, "r")) == NULL) {
	    return(NIL(char));
	}
	info = ARRAYALLOC(char, (buf.st_size + 1));
	count = fread(info, sizeof(char), buf.st_size, infofp);
	info[count] = '\0';

	(void) fclose(infofp);
	initialized = 1;
    }
    
    return(info);
}


/* public functions 'reply', 'gripe', 'forward', 'followup', and 'post' */

void
reply()
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    art_num current = newsgroup->current;
    char title[LABEL_SIZE];
    char message[MAX_SIGNATURE_SIZE + BUFFER_SIZE], *signature;
    getHeader(current);
    Header.artFile = XtNewString(newsgroup->articles[INDEX(current)].filename);
    
    (void) sprintf(title, "Reply to article %ld in %s", current, newsgroup->name);
    
    (void) sprintf(message, "To: %s\n",
		   (*Header.replyTo != '\0') ? Header.replyTo : Header.from);
    buildSubject(message);
    (void) strcat(message, "\n");
    InsertionPoint = (XawTextPosition) utStrlen(message);
    (void) strcat(message, "\n");
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcat(message, signature);
    }

    PostingMode = REPLY;
    composePane(title, message);

    return;
}


void
gripe()
{
    char title[LABEL_SIZE];
    char message[MAX_SIGNATURE_SIZE + BUFFER_SIZE], *signature;
    
    (void) strcpy(title, "Gripe");
    (void) sprintf(message, "To: %s\nSubject: GRIPE about XRN %s\n\n",
		   GRIPES, XRN_VERSION);
    InsertionPoint = (XawTextPosition) utStrlen(message);
    (void) strcat(message, "\n");
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcat(message, signature);
    }

    PostingMode = GRIPE;
    composePane(title, message);

    return;
}


void
forward()
{
    char title[LABEL_SIZE];
    char message[MAX_SIGNATURE_SIZE + BUFFER_SIZE], *signature;
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    art_num current = newsgroup->current;
    
    getHeader(current);
    Header.artFile = XtNewString(newsgroup->articles[INDEX(current)].filename);
    
    (void) sprintf(title, "Forward article %ld in %s to a User", current,
		   newsgroup->name);
    (void) sprintf(message, "To:\nSubject:\n\n");
    InsertionPoint = (XawTextPosition) utStrlen(message);
    (void) strcat(message, "\n");
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcat(message, signature);
    }

    PostingMode = FORWARD;
    composePane(title, message);

    return;
}


void
followup()
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    art_num current = newsgroup->current;
    char buffer[BUFFER_SIZE], title[LABEL_SIZE];
    char message[MAX_SIGNATURE_SIZE + BUFFER_SIZE], *signature;

    if ((newsgroup->status & NG_UNPOSTABLE) == NG_UNPOSTABLE) {
	mesgPane("Can not post articles to this group");
	return;
    }

    getHeader(current);
    Header.artFile = XtNewString(newsgroup->articles[INDEX(current)].filename);
    
    (void) sprintf(title, "Followup to article %ld in %s", current, newsgroup->name);

    (void) sprintf(message, "Path: %s!%s\n", Header.path, Header.user);
    if (*Header.followupTo != '\0') {
	Header.newsgroups = XtNewString(Header.followupTo);
    }
    (void) sprintf(buffer, "Newsgroups: %s\n", Header.newsgroups);
    (void) strcat(message, buffer);
    (void) strcat(message, "Distribution: ");
    if (*Header.distribution != '\0') {
	(void) strcat(message, Header.distribution);
    } else {
	(void) strcat(message, "world");
    }
    (void) strcat(message, "\n");
    (void) strcat(message, "Followup-To: \n");
    (void) sprintf(buffer, "References: %s %s\n", Header.messageId, Header.references);
    (void) strcat(message, buffer);
    (void) sprintf(buffer, "From: %s@%s (%s)\n", Header.user, Header.host, Header.fullname);
    (void) strcat(message, buffer);
    (void) strcat(message, "Reply-To: ");
    if (app_resources.replyTo != NIL(char)) {
	(void) strcat(message, app_resources.replyTo);
    } else {
	(void) sprintf(buffer, "%s@%s (%s)", Header.user, Header.host, Header.fullname);
	(void) strcat(message, buffer);
    }
    (void) strcat(message, "\n");

    (void) strcat(message, "Organization: ");
    if (app_resources.organization != NIL(char)) {
	(void) strcat(message, app_resources.organization);
    } else {
	(void) strcat(message, ORG_NAME);
    }
    (void) strcat(message, "\n");

    buildSubject(message);
    (void) strcat(message, "Keywords: ");
    if (*Header.keywords != '\0') {
	(void) strcat(message, Header.keywords);
    }
    (void) strcat(message, "\n");
   
    (void) strcat(message, "\n");
    InsertionPoint = (XawTextPosition) utStrlen(message);
    (void) strcat(message, "\n");
    
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcat(message, signature);
    }

    PostingMode = FOLLOWUP;
    composePane(title, message);

    return;
}


void
post()
{
    struct newsgroup *newsgroup;
    char title[LABEL_SIZE], buffer[BUFFER_SIZE];
    char message[MAX_SIGNATURE_SIZE + BUFFER_SIZE], *signature;

    if (CurrentGroupNumber != NO_GROUP) {
	newsgroup = Newsrc[CurrentGroupNumber];

	if ((newsgroup->status & NG_UNPOSTABLE) == NG_UNPOSTABLE) {
	    mesgPane("Can not post articles to this group");
	    return;
	}
    }

    getHeader((art_num) 0);

    (void) sprintf(title, "Post article to %s",
		   (CurrentGroupNumber == NO_GROUP) ? "" : newsgroup->name);

    (void) sprintf(message, "Path: %s!%s\n", Header.path, Header.user);
    (void) sprintf(buffer, "Newsgroups: %s\n", Header.newsgroups);
    (void) strcat(message, buffer);
    (void) strcat(message, "Distribution: world\n");
    (void) strcat(message, "Followup-To: \n");
    (void) sprintf(buffer, "From: %s@%s (%s)\n", Header.user, Header.host, Header.fullname);
    (void) strcat(message, buffer);
    (void) strcat(message, "Reply-To: ");
    if (app_resources.replyTo != NIL(char)) {
	(void) strcat(message, app_resources.replyTo);
    } else {
	(void) sprintf(buffer, "%s@%s (%s)", Header.user, Header.host, Header.fullname);
	(void) strcat(message, buffer);
    }
    (void) strcat(message, "\n");

    (void) strcat(message, "Organization: ");
    if (app_resources.organization != NIL(char)) {
	(void) strcat(message, app_resources.organization);
    } else {
	(void) strcat(message, ORG_NAME);
    }
    (void) strcat(message, "\n");
    (void) strcat(message, "Subject: \n");
    (void) strcat(message, "Keywords: \n");

    (void) strcat(message, "\n");
    InsertionPoint = (XawTextPosition) utStrlen(message);
    (void) strcat(message, "\n");
    if ((signature = signatureFile()) != NIL(char)) {
	(void) strcat(message, signature);
    }

    PostingMode = POST;
    composePane(title, message);

    return;
}


void
cancelArticle()
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    art_num current = newsgroup->current;
    char buffer[BUFFER_SIZE];
    char message[MAX_SIGNATURE_SIZE + BUFFER_SIZE];

    if ((newsgroup->status & NG_UNPOSTABLE) == NG_UNPOSTABLE) {
	mesgPane("Can not post articles to this group");
	return;
    }

    getHeader(current);

    (void) sprintf(message, "Path: %s!%s\n", Header.path, Header.user);
    (void) sprintf(buffer, "From: %s@%s (%s)\n", Header.user, Header.host, Header.fullname);
    (void) strcat(message, buffer);
    buildSubject(message);
    if (*Header.followupTo != '\0') {
	Header.newsgroups = XtNewString(Header.followupTo);
    }
    (void) sprintf(buffer, "Newsgroups: %s\n", Header.newsgroups);
    (void) strcat(message, buffer);
    (void) sprintf(buffer, "References: %s %s\n", Header.messageId, Header.references);
    (void) strcat(message, buffer);

    (void) strcat(message, "Distribution: ");
    if (*Header.distribution != '\0') {
	(void) strcat(message, Header.distribution);
    } else {
	(void) strcat(message, "world");
    }
    (void) strcat(message, "\n");
    (void) sprintf(buffer, "Control: cancel %s\n", Header.messageId);
    (void) strcat(message, buffer);

    switch (postArticle(message)) {
	case POST_FAILED:
	mesgPane("Could not cancel the article");
	break;

	case POST_NOTALLOWED:
	mesgPane("Posting not allowed from this machine");
	break;
	    
	case POST_OKAY:
	infoNow("Canceled the article");
	break;
    }

    return;
}

