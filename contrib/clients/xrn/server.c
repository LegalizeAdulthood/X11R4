
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/server.c,v 1.14 89/11/17 12:52:01 ricks Rel $";
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
 * server.c: routines for communicating with the NNTP remote news server
 *
 */

#include "copyright.h"
#include <X11/Xos.h>
#include <ctype.h>
#include <stdio.h>
#include "utils.h"
#include <assert.h>
#ifndef VMS
#include <sys/param.h>
#else
#define MAXPATHLEN 512
#define index strchr
#endif
#include "config.h"
#include "avl.h"
#include "news.h"
#include "mesg.h"
#include "error_hnds.h"
#include "resources.h"
#include "server.h"

#define BUFFER_SIZE 1024
#define MESSAGE_SIZE 1024

int ServerDown = 0;
static char mybuf[MESSAGE_SIZE+100] = 
	"The news server is not responding correctly, aborting\n";

static void
get_data_from_server(str, size)
char *str;     /* string for message to be copied into */
int size;      /* size of string                       */
/*
 * get data from the server (active file, article)
 *
 *  on error, sets 'ServerDown'
 *
 *   returns: void
 */
{
    if (get_server(str, size) < 0) {
	ServerDown = 1;
    } else {
	ServerDown = 0;
    }
    return;
}


static void
check_time_out(command, response, size)
char *command;  /* command to resend           */
char *response; /* response from the command   */
int size;       /* size of the response buffer */
{
    /*
     * try to recover from a timeout
     *
     *   this assumes that the timeout message stays the same
     *   since the error number (503) is used for more than just
     *   timeout
     *
     *   Message is:
     *     503 Timeout ...
     */

    if (ServerDown || STREQN(response, "503 Timeout", 11)) {

	mesgPane("Lost connection to the NNTP server, attempting to reconnect");
	start_server(NIL(char));
	mesgPane("Reconnected to the NNTP server");
	
	/*
	 * if it was an ARTICLE for XHDR command, then you must get the
	 * server into the right state (GROUP mode), so resend the last
	 * group command
	 */
	if (STREQN(command, "ARTICLE", 7) || STREQN(command, "XHDR", 4)) {
	    if (getgroup(NIL(char), NIL(art_num), NIL(art_num), NIL(int)) == NO_GROUP) {
		return;
	    }
	    /* XXX should do some processing of changed first/last numbers */
	}
	
	put_server(command);
	get_data_from_server(response, size);
    }
    
    return;
}


char *
getarticle(artnumber, position, header, rotation)
art_num artnumber;  /* number of article in the current group to retrieve */
long *position;     /* byte position of header/body seperation            */
int header, rotation;
/*
 * retrieve article number 'artnumber' in the current group, update structure
 *
 *   returns:  filename that the article is stored in or NIL(char) if
 *             the article is not avaiable
 *
 */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE], *msg;
    FILE *articlefp;
    char *filename, *ptr;
#ifdef aiws
    char dummy[MAXPATH];
#else
    char dummy[MAXPATHLEN];
#endif /* aiws */
    char field[BUFFER_SIZE];
    int byteCount = 0;

    *position = 0;
    
    /* send ARTICLE */
    (void) sprintf(command, "ARTICLE %ld", artnumber);
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));

    if (*message != CHAR_OK) {
	/* can't get article */
	return(NIL(char));
    }

#ifndef VMS
    (void) sprintf(dummy, "%s/xrn%ld-XXXXXX", app_resources.tmpDir, artnumber);
#else
    (void) sprintf(dummy, "%sxrn%ld-XXXXXX", app_resources.tmpDir, artnumber);
#endif
    if ((filename = mktemp(dummy)) == NIL(char)) {
	mesgPane("can not create a file name for the article file");
	/* XXX read till end of article */
	return(NIL(char));
    }

    if ((articlefp = fopen(filename, "w")) == NULL) {
	mesgPane("can not open a temporary file for the article, `%s' may not be writable", app_resources.tmpDir);
	/* XXX read till end of article */
	return(NIL(char));
    }

    for (;;) {
	get_data_from_server(message, sizeof(message));

	/* the article is ended by a '.' on a line by itself */
	if ((message[0] == '.') && (message[1] == '\0')) {
	    /* check for a bogus message */
	    if (byteCount == 0) {
		(void) fclose(articlefp);
		(void) unlink(filename);
		return(NIL(char));
	    }
	    break;
	}

	msg = &message[0];

	/* find header/body seperation */
	if (*position == 0) {
	    if (*msg == '\0') {
		*position = byteCount;
	    }
	}
	      
	if (*msg == '.') {
	    msg++;
	}

	if (*msg != '\0') {
	    /* strip leading ^H */
	    while (*msg == '\b') {
		msg++;
	    }
	    /* strip '<character>^H' */
	    for (ptr = index(msg + 1, '\b'); ptr != NIL(char); ptr = index(ptr, '\b')) {
		if (ptr - 1 < msg) {
		    /* too many backspaces, kill all leading back spaces */
		    while (*ptr == '\b') {
		        (void) strcpy(ptr, ptr + 1);
			ptr++;
		    }
		    break;
		}
		(void) strcpy(ptr - 1, ptr + 1);
		ptr--;
	    }

	    /* strip the headers */
	    if ((*position == 0) && (header == NORMAL_HEADER)) {
		if ((ptr = index(msg, ':')) == NIL(char)) {
		    continue;    /* weird header line, skip */
		}
		if (*(ptr+1) == '\0') {
		    continue;    /* empty field, skip */
		}
		(void) strncpy(field, msg, (int) (ptr - msg));
		field[(int) (ptr - msg)] = '\0';
		utDowncase(field);
		if (avl_lookup(app_resources.headerTree, field, &ptr)) {
		    if (app_resources.headerMode == STRIP_HEADERS) {
			continue;
		    }
		} else {
		    if (app_resources.headerMode == LEAVE_HEADERS) {
			continue;
		    }
		}
	    }

	    /* handle rotation of the article body */
	    if ((rotation == ROTATED) && (*position != 0)) {
		for (ptr = msg; *ptr != '\0'; ptr++) {
		    if (isalpha(*ptr)) {
			if ((*ptr & 31) <= 13) {
			    *ptr = *ptr + 13;
			} else {
			    *ptr = *ptr - 13;
			}
		    }
		}
	    }

	    /* handle ^L (poorly) */
	    if (*msg == '\014') {
		int i;
		for (i = 0; i < 60; i++) {
		    (void) putc('\n', articlefp);
		}
		byteCount += 60;
		continue;
	    }
	    (void) fputs(msg, articlefp);
	}
	(void) putc('\n', articlefp);
	byteCount += utStrlen(msg) + 1;
    }

    (void) fclose(articlefp);

    return(XtNewString(filename));
}


int
getgroup(name, first, last, number)
char *name;     /* group name                 */
art_num *first; /* first article in the group */
art_num *last;  /* last article in the group  */
int *number;    /* number of articles in the group, if 0, first and last are bogus */
/*
 * enter a new group and get its statistics (and update the structure)
 *   allocate an array for the articles and process the .newsrc article
 *   info for this group
 *
 *   returns: NO_GROUP on failure, 0 on success
 *
 */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char group[GROUP_NAME_SIZE];
    long code, num, count, frst, lst;
    static char lastGroup[GROUP_NAME_SIZE];

    if (name == NIL(char)) {
	name = XtNewString(lastGroup);
    } else {
	(void) strcpy(lastGroup, name);
    }

    (void) sprintf(command, "GROUP %s", name);
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));
    
    if (*message != CHAR_OK) {
	if (atoi(message) != ERR_NOGROUP) {

		(void) strcat(mybuf, "\tRequest was: ");
		(void) strcat(mybuf, command);
		(void) strcat(mybuf, "\n");
		(void) strcat(mybuf, "\tFailing response was: ");
		(void) strcat(mybuf, message);
		ehErrorExitXRN(mybuf);
	}
	mesgPane("Can't get the group, looks like it was deleted out from under us");
	
	/* remove the group from active use ??? */
	
	return(0);
    }

    /* break up the message */
    count = sscanf(message, "%ld %ld %ld %ld %s", &code, &num, &frst, &lst, group);
    assert(count == 5);

    if (number != NIL(int)) {
	*number = num;
    }
    if (first != NIL(art_num)) {
	*first = frst;
    }
    if (last != NIL(art_num)) {
	*last = lst;
    }

    return(1);
}


void
getactive()
/*
 * get a list of all active newsgroups and create a structure for each one
 *
 *   returns: void
 */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE], group[GROUP_NAME_SIZE];
    char type;
    struct newsgroup *newsgroup;
    art_num first, last, art;
    char *ptr;
    int count = 0;

    (void) strcpy(command, "LIST");
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));
    
    if (*message != CHAR_OK) {

	(void) strcat(mybuf, "\tRequest was: ");
	(void) strcat(mybuf, command);
	(void) strcat(mybuf, "\n");
	(void) strcat(mybuf, "\tFailing response was: ");
	(void) strcat(mybuf, message);
	ehErrorExitXRN(mybuf);
    }

    for (;;) {
	get_data_from_server(message, sizeof(message));
	
	/* the list is ended by a '.' at the beginning of a line */
	if (*message == '.') {
	    break;
	}

	/* server returns: group last first y/m */

	if (sscanf(message, "%s %ld %ld %c", group, &last, &first, &type) != 4) {
	    mesgPane("bogus active file entry, skipping\n%s", message);
	    continue;
	}

	if (first == 0) {
	    first = 1;
	}

	if (!avl_lookup(NewsGroupTable, group, &ptr)) {

	    /* no entry, create a new group */
	    newsgroup = ALLOC(struct newsgroup);
/*	    fprintf(stderr,"allocate %d bytes for newsgroup %s\n", sizeof (struct newsgroup), group); /* debug */
	    
	    newsgroup->name = XtNewString(group);
	    newsgroup->newsrc = NOT_IN_NEWSRC;
	    newsgroup->status = NG_NOENTRY;
	    newsgroup->first = first;
	    newsgroup->last = last;
	    newsgroup->nglist = 0;
#ifdef notdef
	    if (last >= first) {
		    (void) fprintf(stderr,"allocate %d bytes for %d articles in %s\n", (newsgroup->last - newsgroup->first + 1) * sizeof (struct article),(newsgroup->last - newsgroup->first + 1), group);
		newsgroup->articles = ARRAYALLOC(struct article, newsgroup->last - newsgroup->first + 1);
		for (art = newsgroup->first; art <= newsgroup->last; art++) {
		    long indx = INDEX(art);
	
		    newsgroup->articles[indx].subject = NIL(char);
		    newsgroup->articles[indx].author = NIL(char);
		    newsgroup->articles[indx].filename = NIL(char);
		    newsgroup->articles[indx].status = ART_CLEAR;
		}
	    } else {
		newsgroup->articles = NIL(struct article);
	    }
#else
		newsgroup->articles = NIL(struct article);
#endif
	    
	    switch (type) {
		case 'y':
		newsgroup->status |= NG_POSTABLE;
		break;

		case 'm':
		newsgroup->status |= NG_MODERATED;
		break;

		case 'n':
		newsgroup->status |= NG_UNPOSTABLE;
		break;
	    }
	
	    (void) avl_insert(NewsGroupTable, newsgroup->name, (char *) newsgroup);

	    ActiveGroupsCount++;
	    
	} else {
	    
	    /*
	     * entry exists, use it; must be a rescanning call
	     *
	     * just update the first and last values and adjust the
	     * articles array
	     */
	    
	    newsgroup = (struct newsgroup *) ptr;

	    /*
	     * only allow last to increase or stay the same
	     * - don't allow bogus last values to trash a group
	     */
	    if (IS_SUBSCRIBED(newsgroup) && last >= newsgroup->last) {
		/* XXX really should save up the resync and use the GROUP info also */
		articleArrayResync(newsgroup, first, last, 1);
	    }
	}
    }

    return;
}


void
badActiveFileCheck()
/*
 * check the case where the first and last article numbers are equal
 * - unfortunately, this means two different things:
 *   1) there are no articles in the group
 *   2) there is one article in the group
 *
 * - so, to get rid of the ambiguity, we make a GROUP call
 *   and look at the 'number' of articles field to determine
 *   whether there are 0 or 1 articles
 */
{
    avl_generator *gen;
    char *key, *value;
    int number;

    /* check out first == last groups */
    gen = avl_init_gen(NewsGroupTable, AVL_FORWARD);

    while (avl_gen(gen, &key, &value)) {
	struct newsgroup *newsgroup = (struct newsgroup *) value;

	if (IS_SUBSCRIBED(newsgroup) &&
	    (newsgroup->first == newsgroup->last) &&
	    (newsgroup->first != 0)) {

	    (void) getgroup(newsgroup->name, NIL(art_num), NIL(art_num), &number);
	    /* fprintf(stderr, "call to getgroup for 0/1 check on %s (%d)\n", newsgroup->name, ++count); */
	    if (number == 0) {
		articleArrayResync(newsgroup, newsgroup->first, newsgroup->last, number);
	    }
	}
    }
    avl_free_gen(gen);

    return;
}


void
start_server(nntpserver)
char *nntpserver;
/*
 * initiate a connection to the news server
 *
 * nntpserver is the name of an alternate server (use the default if NULL)
 *
 * the server eventually used is remembered, so if this function is called
 * again (for restarting after a timeout), it will use it.
 *
 *   returns: void
 *
 */
{
    static char *server = NIL(char);   /* for restarting */
    int response, connected;
#ifndef VMS
    extern void sleep();
#endif

    if (server == NIL(char)) {
	
	if (nntpserver != NIL(char)) {
	    server = nntpserver;
	
	} else {
	    
	    if ((server = getserverbyfile(SERVER_FILE)) == NULL) {
		mesgPane("can't get the name of the news server from `%s'",
			       SERVER_FILE);
#ifndef VMS
		mesgPane("either fix this file, or put NNTPSERVER in your environment");
#else
		mesgPane("either fix this file, or define logical NNTPSERVER");
#endif
		ehErrorExitXRN("");
	    }
	}
    }

    do {
	if ((response = server_init(server)) < 0) {
	    connected = 0;
	    mesgPane("Failed to reconnect to the NNTP server (server_init), sleeping...");
	    sleep(60);
	    continue;
	}
	if (handle_server_response(response, server) < 0) {
	    connected = 0;
	    close_server();
	    mesgPane("Failed to reconnect to the NNTP server (handle_response), sleeping...");
	    sleep(60);
	    continue;
	}
	connected = 1;
    } while (!connected);
    
    return;
}
    

void
getsubjectlist(newsgroup, first, last)
struct newsgroup *newsgroup;
art_num first;
art_num last;
/*
 * get a list of subject lines for the current group in the range
 *  'first' to 'last'
 *
 *   returns: void
 *
 * Note that XHDR is not part of the rfc977 standard, but is implemented
 * by the Berkeley NNTP server
 *
 */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE], buffer[MESSAGE_SIZE];
    char *subjectline, *end;
    long number;

    (void) sprintf(command, "XHDR subject %ld-%ld", first, last);
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));

    /* check for errors */
    if (*message != CHAR_OK) {
	mesgPane(
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.\n");
	return;
    }

    for(;;) {

	get_data_from_server(message, sizeof(message));
	
	if (*message == '.') {
	    break;
	}

	/*
	 * message is of the form:
	 *
	 *    Number SubjectLine
	 *
	 *    203 Re: Gnumacs Bindings
	 *
	 * must get the number since not all subjects will be returned
	 */

	number = atol(message);
	subjectline = index(message, ' ');
	(void) sprintf(buffer, "  %5ld\t%s", number, ++subjectline);

	newsgroup->articles[INDEX(number)].subject = XtNewString(buffer);
    }
    return;
}


void
getauthorlist(newsgroup, first, last)
struct newsgroup *newsgroup;
art_num first;
art_num last;
/*
 * get a list of author lines for the current group in the range
 *  'first' to 'last'
 *
 *   returns: void
 *
 * Note that XHDR is not part of the rfc977 standard, but is implemented
 * by the Berkeley NNTP server
 *
 */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *author, *end;
    long number;

    
    (void) sprintf(command, "XHDR from %ld-%ld", first, last);
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));

    /* check for errors */
    if (*message != CHAR_OK) {
	mesgPane(
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.\n");
	return;
    }
    
    for(;;) {

	get_data_from_server(message, sizeof(message));
	
	if (*message == '.') {
	    break;
	}

	/*
	 * message is of the form:
	 *
	 *    Number Author
	 *
	 *    203 ricks@shambhala (Rick L. Spickelmier)
	 *
	 * must get the number since not all authors will be returned
	 */

	number = atol(message);
	author = index(message, ' ');
	assert(author != NIL(char));
	author++;
	if ((end = index(author, ' ')) != NIL(char)) {
	    *end = '\0';
	}
	newsgroup->articles[INDEX(number)].author = XtNewString(author);
    }
    return;
}


static void
sendLine(str)
char *str;
{
    int i;

    if (*str == '.') {
	for (i = utStrlen(str); i >= 0; i--) {
	    str[i+1] = str[i];
	}
	str[0] = '.';
    }
    put_server(str);
    return;
}


static char *
getLine(ptr)
char **ptr;
{
    static char line[512];
    char *end = index(*ptr, '\n');

    if (end) {
	(void) strncpy(line, *ptr, end - *ptr);
	line[end - *ptr] = '\0';
	*ptr = end + 1;
    } else {
	(void) strcpy(line, *ptr);
	*ptr = 0;
    }
    return line;
}


int
postArticle(article)
char *article;
/*
 * post an article
 *
 *   returns 1 for success, 0 for failure
 */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char *ptr, *line, *saveptr;
    int c, in_header;
    
    (void) strcpy(command, "POST");
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));

    if (*message != CHAR_CONT) {
	mesgPane("NNTP Serious Error: %s\n", message);
	if (atoi(message) == ERR_NOPOST) {
	    return(POST_NOTALLOWED);
	} else {
	    return(POST_FAILED);
	}
    }

    ptr = article;
    in_header = 1;

    while (in_header) {
	char *line;

	saveptr = ptr;

	line = getLine(&ptr);
	if (index(line, ':')) {
	    sendLine(line);
	    continue;
	}
	in_header = 0;
    }

    /* + 1 for \0, +2 for . */
    line = ARRAYALLOC(char, (app_resources.breakLength + 2));
    line[0] = '\0';
    c = 0;

    sendLine(line);		/* send a blank line */

    if (*saveptr != '\n') {	/* if the skipped line was not blank, point back to it */
	ptr = saveptr;
    }

    for (; *ptr != '\0'; ptr++) {
	if (c >= app_resources.breakLength) {
	    /* backoff to app_resources.lineLength */
	    ptr -= c - app_resources.lineLength;
	    c = app_resources.lineLength;
	    for (; c > 0 && *ptr != ' ' && *ptr != '\t'; ptr--) {
		/*
		if (*ptr == '\t') {
		}
		*/
		c--;
	    }

	    if (c == 0) {
		/* pathological, cut to app_resources.lineLength */
		c = app_resources.lineLength;
		ptr += app_resources.lineLength - 1;
	    }

	    /* output */
	    line[c] = '\0';
	    sendLine(line);
	    c = 0;
	    continue;
	}

	if (*ptr == '\n') {
	    line[c] = '\0';
	    sendLine(line);
	    c = 0;
	    continue;
	}

	/*
	if (*ptr == '\t') {
	    c += c % 8;
	    continue;
	}
	*/
	line[c++] = *ptr;
    }

    if (c != 0) {
	line[c] = '\0';
	sendLine(line);
    }

    FREE(line);

    put_server(".");

    get_data_from_server(message, sizeof(message));

    if (*message != CHAR_OK) {
	mesgPane("NNTP Serious Error: %s\n", message);
	return(POST_FAILED);
    }

    return(POST_OKAY);
}


void
xhdr(article, field, string)
art_num article;
char *field;
char **string;
/*
 * get header information about 'article'
 *
 *   the results are stored in 'string'
 */
{
    char buffer[BUFFER_SIZE], message[MESSAGE_SIZE], *ptr;
    
    (void) sprintf(buffer, "XHDR %s %ld", field, article);
    put_server(buffer);
    get_data_from_server(message, sizeof(message));
    
    check_time_out(buffer, message, sizeof(message));
    
    /* check for errors */
    if (*message != CHAR_OK) {
	*string = NIL(char);
	mesgPane(
"XRN: serious error, your NNTP server does not have XHDR support.\n\
Either you are running a pre-1.5 NNTP server or XHDR has\n\
not been defined in 'nntp/common/conf.h'\n\
XRN requires XHDR support to run.\n");
	return;
    }
    
    get_data_from_server(message, sizeof(message));

    /* no information */
    if (*message == '.') {
	*string = NIL(char);
	return;
    }

    ptr = index(message, ' ');

    /* malformed entry */
    if (ptr == NIL(char)) {
	mesgPane(
"XRN debugging message: malformed XHDR return\n\
command: %s, return: %s\n",
		       buffer, message);
	get_data_from_server(message, sizeof(message));
	return;
    }

    ptr++;

    /* no information */
    if (STREQ(ptr, "(none)")) {
	*string = NIL(char);
	/* ending '.' */
	do {
	    get_data_from_server(message, sizeof(message));
	} while (*message != '.');
	return;
    }

    *string = XtNewString(ptr);

    /* ending '.' */
    do {
	get_data_from_server(message, sizeof(message));
    } while (*message != '.');

    return;
}

struct article *
getarticles(newsgroup)
struct newsgroup *newsgroup;
{
    art_num first = newsgroup->first, last = newsgroup->last, art;

    if (last >= first && last != 0) {
	newsgroup->articles = ARRAYALLOC(struct article, last - first + 1);

	for (art = first; art <= last; art++) {
	    long indx = INDEX(art);
    
	    newsgroup->articles[indx].subject = NIL(char);
	    newsgroup->articles[indx].author = NIL(char);
	    newsgroup->articles[indx].filename = NIL(char);
	    newsgroup->articles[indx].status = ART_CLEAR;
	}
    }
    return(newsgroup->articles);
}

