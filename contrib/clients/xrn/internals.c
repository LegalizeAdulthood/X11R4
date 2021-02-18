
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/internals.c,v 1.13 89/11/17 12:51:22 ricks Rel $";
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
 * internals.c: routines for handling the internal data structures
 *    calls server routines, is calls by the user interface code
 *
 */

#include "copyright.h"
#include <X11/Xos.h>
#include <stdio.h>
#include "config.h"
#include "utils.h"
#include <assert.h>
#ifndef VMS
#include <sys/param.h>
#else
#ifndef R_OK
#define F_OK            0       /* does file exist */
#define X_OK            1       /* is it executable by caller */
#define W_OK            2       /* writable by caller */
#define R_OK            4       /* readable by caller */
#endif /* R_OK */
#define getArticle getArticleFile
#define index strchr
#include <ctype.h>
#endif /* VMS */
#ifdef aiws
#include <unistd.h>
#endif /* aiws */
#include "avl.h"
#include "news.h"
#include "newsrcfile.h"
#include "resources.h"
#include "server.h"
#include "mesg.h"
#include "error_hnds.h"
#include "save.h"
#include "internals.h"

#define GETARTICLES(newsgroup) (newsgroup->articles) ? (newsgroup->articles) : getarticles(newsgroup)
#define SETARTICLES(newsgroup) if (newsgroup->articles == 0) (void) getarticles(newsgroup)

struct article *getarticles();
static char *strip();

#define BUFFER_SIZE 1024
#define LABEL_SIZE 128
#define FATAL 0
#define OKAY  1
/*
 * length of lines in the top text window
 */
#define LINE_LENGTH 80

#ifdef PREFETCH
/* don't prefetch the next group if you got to a group via 'goto newsgroup' */
#define OKAY_TO_PREFETCH_GROUP 1
#define DO_NOT_PREFETCH_GROUP  0

/*
 * this flag is set to DO_NOT_PREFETCH_GROUP if you got to the current
 * group by 'goto newsgroup' (jumpToNewsgroup).  You do not want to prefetch
 * the next group if you are randomly jumping around.  The next group
 * after the one jumped to is probably not the next one you will go to.
 */
static int PrefetchGroupMode = OKAY_TO_PREFETCH_GROUP;
#endif /* PREFETCH */

/*
 * next article to get when faulting in articles that are of the top
 * of the subject screen.
 */
static art_num NextPreviousArticle;
  
/* storage for all newsgroups, key is the name of the group */
avl_tree *NewsGroupTable = NIL(avl_tree);

/* number of groups in the active file */
int ActiveGroupsCount = 0;

/* number of the group currently being processed */
ng_num CurrentGroupNumber = NO_GROUP;

/* number of groups in the .newsrc file */
ng_num MaxGroupNumber = 0;

#ifdef PREFETCH
/* set to the value of a group if that group has been prefetched */
static ng_num PrefetchedGroup = NO_GROUP;
#endif /* PREFETCH */

void
checkLock()
/*
 * see if another xrn is running
 */
{
#ifndef VMS
    char *buffer = utTildeExpand("~/.xrnlock");
#else
    char *buffer = "SYS$LOGIN:XRN.LOCK";
    char *ptr;
#endif /* VMS */
    char host[64];
    char myhost[64];
    int pid;
    FILE *fp;

    if (!buffer) {
	/* silently ignore this condition */
	return;
    }

    if ((fp = fopen(buffer, "r")) == NULL) {
	if ((fp = fopen(buffer, "w")) == NULL) {
	    /* silently ignore this condition */
	    return;
	}
#ifndef VMS
	if (gethostname(myhost, sizeof(myhost)) == -1) {
	    (void) strcpy(myhost, "bogusHost");
	}
#else
	ptr = getenv("SYS$NODE");
	(void) strcpy(myhost, ptr);
	ptr = index(myhost, ':');
	if (ptr != NIL(char)) {
	    *ptr = '\0';
	}

	if (*myhost == NIL(char)) {
	    (void) strcpy(myhost, "bogusHost");
	}
#endif /* VMS */

	(void) fprintf(fp, "%s %d", myhost, getpid());
	(void) fclose(fp);
	return;
    }

    (void) fscanf(fp, "%s %d", host, &pid);
    (void) fprintf(stderr, "An XRN of yours is running on %s as process %d.\n",
		   host, pid);
    (void) fprintf(stderr, "If it is no longer running, remove %s\n",
		   buffer);
    exit(-1);

    return;
}


void
removeLock()
{
#ifndef VMS
    char *buffer = utTildeExpand("~/.xrnlock");
#else
    char *buffer = "SYS$LOGIN:XRN.LOCK";
#endif /* VMS */

    if (buffer) {
	(void) unlink(buffer);
    }
    return;
}


void
initializeNews()
/*
 * initialize the program and the news system
 *   read newsrc, etc
 *
 *   returns: void
 *
 */
{
    start_server(app_resources.nntpServer);

    NewsGroupTable = avl_init_table(strcmp);
    
    getactive();
    if (!readnewsrc(app_resources.newsrcFile, app_resources.saveNewsrcFile)) {
	ehErrorExitXRN("Can not read the newsrc file");
    }
    badActiveFileCheck();
    return;
}


void
rescanServer()
/*
 * get the active file again and grow the Newsrc array if necessary
 */
{
    int old = ActiveGroupsCount;

    /* update the newsrc file */
    if (!updatenewsrc()) {
	ehErrorExitXRN("Can not update the newsrc file");
    }

    /* close and reopen the connection - till the server is fixed */
    close_server();
    start_server(NIL(char));
    
    getactive();

    if (ActiveGroupsCount > old) {
	/* new newsgroups were found, allocate a bigger array */
	Newsrc = (struct newsgroup **) realloc((char *) Newsrc, (unsigned) (sizeof(struct newsgroup *) * ActiveGroupsCount));
    }

    badActiveFileCheck();

    return;
}


int
unreadArticleCount(newsgroup)
struct newsgroup *newsgroup;
/*
 * accurately count the number of unread articles in a group
 *
 *   returns: the number of unread articles in the group
 */
{
    art_num i;
    struct article *articles = GETARTICLES(newsgroup);
    int count = 0;

    if (EMPTY_GROUP(newsgroup)) {
	return 0;
    }
    
    if ((newsgroup->first == 0) && (newsgroup->last == 0)) {
	return 0;
    }
    
    for (i = newsgroup->first; i <= newsgroup->last; i++) {
	long indx = INDEX(i);
	if (IS_UNREAD(articles[indx]) && !IS_UNAVAIL(articles[indx])) {
	    count++;
	}
    }

    return count;
}


static void
setCurrentArticle(newsgroup)
struct newsgroup *newsgroup;
/*
 * find the first unread article in a group and set 'current' to it
 *
 * returns: void
 *
 */
{
    struct article *articles = GETARTICLES(newsgroup);
    art_num i;
    

    newsgroup->current = newsgroup->last + 1;
    
    for (i = newsgroup->first; i<= newsgroup->last; i++) {
	long indx = INDEX(i);
	if (IS_UNREAD(articles[indx]) && !IS_UNAVAIL(articles[indx])) {
	    newsgroup->current = i;
	    return;
	}
    }
    return;
}


static char *
arrayToString(array, count, bytes)
char **array;
int count;
int bytes;
/*
 * convert an array of strings (of 'count' items) into one big string
 * with each substring seperated by newlines
 *
 * bytes is the sum of the string lengths of the strings
 *
 * this routine takes care of allocating the big string
 *
 */
{
    char *bigString, *end;
    int i;

    /* bytes for the strings, count for the newlines, 1 for the zero byte */
    bigString = ARRAYALLOC(char, (bytes + count + 1));
    bigString[0] = '\0';

    end = bigString;
    for (i = 0; i < count; i++) {
	(void) strcpy(end, array[i]);
	end += utStrlen(array[i]);
	*end = '\n';
	end++;
    }

    *end = '\0';

    return bigString;
}


char *
unreadGroups()
/*
 * build and return a string of information about groups that need to be read
 */
{
    char dummy[LINE_LENGTH];
    struct newsgroup *newsgroup;
    static int lineLength = 15 + 40 + 5 + 10;
    ng_num i;
    int unread, j;
    int bytes = 0, subscribedGroups = 0;
    char *string, **ar;

    ar = ARRAYALLOC(char *, MaxGroupNumber);

    for (i = 0; i < MaxGroupNumber; i++) {
	newsgroup = Newsrc[i];

	if (IS_SUBSCRIBED(newsgroup) && ((unread = unreadArticleCount(newsgroup)) > 0)) {
	    (void) sprintf(dummy, "Unread news in %-40.40s%5d article%s",
			   newsgroup->name, unread,
			   (unread > 1 ? "s" : " "),
			   newsgroup->first, newsgroup->last);

	    ar[subscribedGroups++] = XtNewString(dummy);
	    bytes += utStrlen(dummy);
	}
    }

    string = arrayToString(ar, subscribedGroups, bytes);
    for (j = 0; j < subscribedGroups; j++) {
	FREE(ar[j]);
    }
    FREE(ar);
    return string;
}


char *
newGroups()
/*
 * determine the newsgroups that are not in the .newsrc file
 *
 *   returns a string representing the information
 */
{
    int count = 0, bytes = 0;
    avl_generator *gen;
    char *key, *value;
    char **ar;
    char *string;

    ar = ARRAYALLOC(char *, ActiveGroupsCount);

    gen = avl_init_gen(NewsGroupTable, AVL_FORWARD);

    while (avl_gen(gen, &key, &value)) {
	struct newsgroup *newsgroup = (struct newsgroup *) value;

	if (IS_NOENTRY(newsgroup)) {
	    ar[count++] = newsgroup->name;
	    bytes += utStrlen(newsgroup->name);
	}
    }

    avl_free_gen(gen);

    /* no new groups return */
    if (count == 0) {
	FREE(ar);	
	return NIL(char);
    }

    string = arrayToString(ar, count, bytes);
    FREE(ar);
    
    return string;
}


#ifdef PREFETCH

void
freePrefetchedGroupArticle()
{
    struct newsgroup *pfgroup;
    struct article *articles;
    long indx;

    if (PrefetchedGroup != NO_GROUP) {
	pfgroup = Newsrc[PrefetchedGroup];
	articles = GETARTICLES(pfgroup);
	indx = pfgroup->current - pfgroup->first;
    
	/* free the first article if it has been prefetched */
	if (IS_FETCHED(articles[indx])) {
	    CLEAR_FILE(articles[indx]);
	}
    }
    
    return;
}

#endif /* PREFETCH */


void
releaseNewsgroupResources()
/*
 *   release some resources
 *
 *   returns: void
 */
{
    /*
     * CurrentGroupNumber can be invalid when called from the
     * signal catcher
     */
    if (CurrentGroupNumber != NO_GROUP) {
	struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
	struct article *articles = GETARTICLES(newsgroup);
	art_num art;

	if ((newsgroup->last == 0) || EMPTY_GROUP(newsgroup)) {
	    return;
	}

	for (art = newsgroup->first; art <= newsgroup->last; art++) {
	    long indx = INDEX(art);
	    
	    CLEAR_SUBJECT(articles[indx]);
	    CLEAR_AUTHOR(articles[indx]);
	    CLEAR_FILE(articles[indx]);
	    SET_STRIPPED_HEADERS(articles[indx]);
	    SET_UNROTATED(articles[indx]);
	}
    }

    if (app_resources.updateNewsrc == TRUE) {
	if (!updatenewsrc()) {
	    ehErrorExitXRN("Can not update the newsrc file");
	}
    }
    
    return;
}


void
articleArrayResync(newsgroup, first, last, number)
struct newsgroup *newsgroup;
art_num first, last;
int number;
/*
 * update an article array if the first and/or last article numbers change
 *
 *  returns: void
 */
{
    struct article *articles = GETARTICLES(newsgroup);
    struct article *newarticles;
    int i;


    /*
     * if there are actually no articles in the group, free up the
     * article array and set the first/last values
     */
       
    if (number == 0) {
	CLEAR_ARTICLES(newsgroup);
	newsgroup->first = newsgroup->last + 1;
	return;
    }
    
    /* refuse to resync if last == 0 */
    if (last == 0) {
	return;
    }

    if (first > last) {
	/* all articles have been removed */
	CLEAR_ARTICLES(newsgroup);
	newsgroup->first = newsgroup->last + 1;
	return;
    }

    /* don't allow last to go backwards!!! */
    if (last < newsgroup->last) {
	last = newsgroup->last;
    }
	
    if ((first != newsgroup->first) || (last != newsgroup->last)) {
	/* the first/last values have changed, resync */
	newarticles = ARRAYALLOC(struct article, last - first + 1);

	/*
	 * initialize the new article structures
	 */
	
	for (i = first; i <= last; i++) {
	    newarticles[i - first].subject = NIL(char);
	    newarticles[i - first].author = NIL(char);
	    newarticles[i - first].filename = NIL(char);
	    if (i < newsgroup->first) {
		/* handle first decreasing... mark them as read */
		newarticles[i - first].status = ART_CLEAR_READ;
	    } else {
		newarticles[i - first].status = ART_CLEAR;
	    }
	}

	if (!EMPTY_GROUP(newsgroup)) {
	    for (i = first; i <= last; i++) {
		if ((i >= newsgroup->first) && (i <= newsgroup->last)) {
		    newarticles[i - first] = articles[INDEX(i)];
		}
	    }

	    /* free up the old resources - before the new first */
	    for (i = newsgroup->first; i < first; i++) {
		long indx = INDEX(i);
		CLEAR_SUBJECT(articles[indx]);
		CLEAR_AUTHOR(articles[indx]);
		CLEAR_FILE(articles[indx]);
	    }
	    /* after the new last */
	    for (i = last + 1; i < newsgroup->last; i++) {
		long indx = INDEX(i);
		CLEAR_SUBJECT(articles[indx]);
		CLEAR_AUTHOR(articles[indx]);
		CLEAR_FILE(articles[indx]);
	    }
	}

	if (articles != NIL(struct article)) {
	    FREE(articles);
	}
	newsgroup->articles = newarticles;

	newsgroup->first = first;
	newsgroup->last = last;
    }

    return;
}


static char *
localKillFile(newsgroup, mode)
struct newsgroup *newsgroup;
int mode;
{
    static char buffer[BUFFER_SIZE];
    char *ptr;
    int i;

    if (!createNewsDir()) {
	return NIL(char);
    }

    (void) strcpy(buffer, app_resources.expandedSaveDir);
    i = strlen(buffer);

#ifndef VMS
    buffer[i++] = '/';

    for (ptr = newsgroup->name; *ptr != 0; ptr++) {
	if (*ptr == '.') {
	    if (mode) {
		buffer[i] = '\0';
		(void) mkdir(buffer, 0777);
	    }
	    buffer[i] = '/';
	    i++;
	} else {
	    buffer[i++] = *ptr;
	}
    }
    buffer[i] = '\0';
    if (mode) {
	(void) mkdir(buffer, 0777);
    }
    (void) strcpy(&buffer[i], "/KILL");
#else
    i = i + utGroupToVmsFilename(&buffer[i], newsgroup->name);
    (void) strcpy(&buffer[i], ".KILL");
#endif

    return buffer;
}


char *
globalKillFile(newsgroup)
struct newsgroup *newsgroup;
{
    static char buffer[BUFFER_SIZE];
    int i;

    if (!createNewsDir()) {
	return NIL(char);
    }
    (void) strcpy(buffer, app_resources.expandedSaveDir);
#ifndef VMS
    (void) strcat(buffer, "/KILL");
#else
    (void) strcat(buffer, "NEWS$GLOBAL.KILL");
#endif
    return buffer;
}


void
killItem(item, type)
char *item;
int type;
/*
 * add a kill subject/author entry to a kill file
 */
{
    char input[BUFFER_SIZE], *iptr;
    char output[BUFFER_SIZE], *optr;
    FILE *fp;
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    char *file;

    if (type == KILL_LOCAL) {
	file = localKillFile(newsgroup, 1);
    } else {
	file = globalKillFile(newsgroup);
    }

    if ((fp = fopen(file, "a")) == NULL) {
	mesgPane("Can not open the %s kill file for %s", 
		       (type == KILL_LOCAL) ? "local" : "global",
		       newsgroup->name);
	return;
    }

    /* get rid of all magic characters */
    (void) strncpy(input, item, sizeof(input));
    iptr = input;
    optr = output;
    while (*iptr) {
	if ((*iptr == '/')  ||
	    (*iptr == '\\') ||
	    (*iptr == '(')  ||
	    (*iptr == '[')  ||
	    (*iptr == '.')  ||
	    (*iptr == '^')  ||
	    (*iptr == '*')  ||
	    (*iptr == '$')  ||
	    (*iptr == '\'') ||
	    (*iptr == '\"')) {
	    *iptr = '.';
	}
	*optr++ = *iptr++;
    }
    *optr = '\0';

    fprintf(fp, "/%.24s/:j\n", output);
    (void) fclose(fp);
    return;
}


/* XXX  THIS ROUTINE REALLY NEEDS TO BE CLEANED UP */
static void
killArticles(newsgroup, fp)
struct newsgroup *newsgroup;
FILE *fp;
/*
 * kill all subjects in the newsgroup that match the kill
 * orders in fp.
 */
{
    char string[BUFFER_SIZE], pattern[BUFFER_SIZE], commands[BUFFER_SIZE];
    char dummy[BUFFER_SIZE];
    art_num i, start = newsgroup->current;
    char *subject, *author, *subj, *ptr, *pptr;
    int scount = 0, kcount = 0, ucount = 0;
#ifdef SYSV_REGEX
    extern char *regcmp();
    extern char *regex();
#else
    extern char *re_comp();
#endif
    char *reRet;
    int len;
    char type;

    info("processing KILL file");

    /* XXX don't reprocess global each time, keep in persistent hash table */

    while (fgets(string, sizeof(string), fp) != NULL) {
	/*
	 * see if there is a 'THRU artnum' line, if so,
	 * only compare subjects from that article on
	 * XXX need to update THRU
	 */
	if (STREQN(string, "THRU", 4)) {
	    i = atol(&string[5]);
	    /* if THRU is less than current, ignore it */
	    start = MAX(i + 1, newsgroup->current);
	    continue;
	}

	if (*string == '&') {
	    /* 'rn' switch setting, ignore */
	    continue;
	}

	/*
	 * process kill file request should be in the form
	 */
	ptr = string + 1;
	pptr = pattern;

	while (*ptr && (*ptr != '/' || ((*ptr == '/') && *(ptr - 1) == '\\'))) {
	   *pptr++ = *ptr;
	   ptr++;
	}
	*pptr = '\0';

	if (!*ptr) {
	    mesgPane("Malformed KILL file entry (pattern): %s (%s)",
		     string, pattern);
	    continue;
	}

	/* rn puts ': *' in front of patterns, xrn doesn't */
	if (strncmp(pattern, ": *", 3) == 0) {
	    /* deal with overlapping strings */
	    (void) strcpy(dummy, pattern + 3);
	    (void) strcpy(pattern, dummy);
	}

#ifdef SYSV_REGEX
	if ((reRet = regcmp(pattern, NULL)) == NULL) {
	    mesgPane("Bad KILL file pattern: %s", string);
#else
	if ((reRet = re_comp(pattern)) != NIL(char)) {
	    mesgPane("Bad KILL file pattern: %s\n%s", string, reRet);
#endif
	    continue;
	}

	ptr++;	/* skip past the slash */
	(void) strcpy(commands, ptr);
	if ((ptr = index(commands, ':')) == NIL(char)) {
	    mesgPane("Malformed KILL file entry (command): %s (%s)",
		     string, commands);
	    continue;
	}
	ptr++;	/* skip past the colon */
	type = *ptr;

	for (i = start; i <= newsgroup->last; i++) {

	    /* short cut */
	    if (IS_UNAVAIL(newsgroup->articles[INDEX(i)]) ||
		((type == 'j') && IS_READ(newsgroup->articles[INDEX(i)])) ||
		((type == 'm') && IS_UNREAD(newsgroup->articles[INDEX(i)]))) {
		continue;
	    }

	    if (newsgroup->articles[INDEX(i)].subject ||
	        newsgroup->articles[INDEX(i)].author) {

		subject = newsgroup->articles[INDEX(i)].subject;
		author = newsgroup->articles[INDEX(i)].author;

		if (subject) {
		    subj = strip(subject);
		}

#ifdef SYSV_REGEX
		if ((subject && (regex(reRet, subj) != NULL)) ||
		    (author  && (regex(reRet, author) != NULL))) {
#else
		if ((subject && re_exec(subj)) ||
		    (author  && re_exec(author))) {
#endif
		    switch (type) {
			case 'j':
			    SET_READ(newsgroup->articles[INDEX(i)]);
			    kcount++;
			    break;

			case 'm':
			    SET_UNREAD(newsgroup->articles[INDEX(i)]);
			    break;

			case 's':
			    (void) saveArticle(NIL(char), newsgroup, i);
			    scount++;
			    break;

			default:
			    mesgPane("unknown KILL file option: %s\n", string);
			    ucount++;
			    break;
		    }
		}
	    }
	}
    }

    if (kcount)
	mesgPane("killed %d articles in %s\n",
		       kcount, newsgroup->name);

    if (scount)
	mesgPane("saved %d articles in %s\n",
		       scount, newsgroup->name);

    if (ucount)
	mesgPane("matched %d articles with unknown option in %s\n",
		       ucount, newsgroup->name);

    return;
}


static void
checkKillFiles(newsgroup)
struct newsgroup *newsgroup;
/*
 * mark articles as read if in the kill files
 */
{
    FILE *fp;

    if ((fp = fopen(globalKillFile(newsgroup), "r")) != NULL) {
	killArticles(newsgroup, fp);
	(void) fclose(fp);
    }

    if ((fp = fopen(localKillFile(newsgroup, 0), "r")) != NULL) {
	killArticles(newsgroup, fp);
	(void) fclose(fp);
    }

    return;
}


static void
setUpGroup(newsgroup, killfilesp)
struct newsgroup *newsgroup;
int killfilesp;		/* check kill files? no for jumpToNewsgroup */
{
    art_num first, last;
    int number;
    
    /* get the latest group information */
    if (!getgroup(newsgroup->name, &first, &last, &number)) {
	return;
    }
    if (!EMPTY_GROUP(newsgroup)) {
	articleArrayResync(newsgroup, first, last, number);
	setCurrentArticle(newsgroup);
	getsubjectlist(newsgroup, newsgroup->current, newsgroup->last);
	getauthorlist(newsgroup, newsgroup->current, newsgroup->last);
	if (killfilesp && (app_resources.killFiles == TRUE)) {
	    checkKillFiles(newsgroup);
	}
    }
    return;
}

  
int
gotoNewsgroup(name)
char *name;
/*
 * goto a particular newsgroup
 *   invalidate the prefetched group if it is not the new one
 */
{
    char *ptr;
    struct newsgroup *newsgroup;

#ifdef PREFETCH
    PrefetchGroupMode = OKAY_TO_PREFETCH_GROUP;
#endif /* PREFETCH */
    
    if (!avl_lookup(NewsGroupTable, name, &ptr)) {
	mesgPane("Newsgroup `%s' does not exist", name);
	return BAD_GROUP;
    }

    newsgroup = (struct newsgroup *) ptr;
    CurrentGroupNumber = newsgroup->newsrc;
#ifdef PREFETCH
    if (CurrentGroupNumber != PrefetchedGroup) {
	freePrefetchedGroupArticle();
	PrefetchedGroup = NO_GROUP;
    }
#endif /* PREFETCH */
    return GOOD_GROUP;
}
    

int
jumpToNewsgroup(name)
char *name;
/*
 * goto a particular newsgroup
 *   if all of the articles have been read, unmark the last one
 */
{
    char *ptr;
    struct newsgroup *newsgroup;
    art_num i;
    
#ifdef PREFETCH    
    PrefetchGroupMode = DO_NOT_PREFETCH_GROUP;
#endif /* PREFETCH */
    if (!avl_lookup(NewsGroupTable, name, &ptr)) {
	mesgPane("Newsgroup `%s' does not exist", name);
	return BAD_GROUP;
    }

    newsgroup = (struct newsgroup *) ptr;
    CurrentGroupNumber = newsgroup->newsrc;

    if (EMPTY_GROUP(newsgroup)) {
	mesgPane("Cannot go to the newsgroup, no articles available");
	return BAD_GROUP;
    }

    if (!IS_SUBSCRIBED(newsgroup)) {
	updateArticleArray(newsgroup);
    }

    /*
     * if there are no unread articles, find the last available one
     * and mark it as unread
     *
     * XXX bug if the last article becomes unavailable
     */
    if (unreadArticleCount(newsgroup) == 0) {
	assert(newsgroup->articles != NIL(struct article));
	for (i = newsgroup->last; i >= newsgroup->first; i--) {
	    if (!IS_UNAVAIL(newsgroup->articles[INDEX(i)])) {
		SET_UNREAD(newsgroup->articles[INDEX(i)]);
		break;
	    }
	}
    }
#ifdef PREFETCH
    if (CurrentGroupNumber != PrefetchedGroup) {
	freePrefetchedGroupArticle();
	PrefetchedGroup = NO_GROUP;
    }

    if (PrefetchedGroup != NO_GROUP) {
	CurrentGroupNumber = PrefetchedGroup;
	PrefetchedGroup = NO_GROUP;
	newsgroup = Newsrc[CurrentGroupNumber];
	return GOOD_GROUP;
    }
#endif /* PREFETCH */
    setUpGroup(newsgroup, 0);
    return GOOD_GROUP;
}
    

int
getNewsgroup()
/*
 * get the newsgroup
 *
 * returns:  XRN_NOMORE - bad group
 *           XRN_OKAY   - okay
 */
{
    struct newsgroup *newsgroup;

#ifdef PREFETCH    
    if (PrefetchedGroup != NO_GROUP) {
	CurrentGroupNumber = PrefetchedGroup;
	PrefetchedGroup = NO_GROUP;
	newsgroup = Newsrc[CurrentGroupNumber];

	return XRN_OKAY;
    }
#endif /* PREFETCH */
    newsgroup = Newsrc[CurrentGroupNumber];

    if (IS_SUBSCRIBED(newsgroup) && (unreadArticleCount(newsgroup) > 0)) {
	setUpGroup(newsgroup, 1);
	return XRN_OKAY;
    }
    
    return XRN_NOMORE;
}


void
catchUp()
/*
 * mark all articles in a newsgroup as read
 *
 *   other functions will take care of releasing resources
 *
 *   returns: void
 *
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    art_num art;

    SETARTICLES(newsgroup);
    for (art = newsgroup->first; art <= newsgroup->last; art++) {
	SET_READ(newsgroup->articles[INDEX(art)]);
    }
    return;
}


void
subscribe()
/*
 * subscribe to a newsgroup
 *
 *   returns: void
 *
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    
    if (!IS_SUBSCRIBED(newsgroup)) {
	SET_SUB(newsgroup);
	updateArticleArray(newsgroup);
    }
    return;
}


void
unsubscribe()
/*
 * unsubscribe to a newsgroup
 *
 *   returns: void
 *
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    
    SET_UNSUB(newsgroup);
    return;
}


static char *
buildQuestion(newsgroup)
struct newsgroup *newsgroup;
/*
 * build a string that is used as the question for what
 * to do at the end of an article
 *
 *   returns: the question in a static area
 *
 */
{
    static char dummy[LABEL_SIZE];
    art_num i;
    struct article *articles = GETARTICLES(newsgroup);
    long unread = 0;

    
    for (i = newsgroup->first; i <= newsgroup->last; i++) {
	long indx = INDEX(i);
	if (IS_UNREAD(articles[indx]) && !IS_UNAVAIL(articles[indx])) {
	    unread++;
	}
    }
	    
    if (unread <= 1) {
	(void) sprintf(dummy, "Article %ld in %s",
		       newsgroup->current, newsgroup->name);
    } else {
	(void) sprintf(dummy, "Article %ld in %s (%ld remaining)",
		       newsgroup->current, newsgroup->name, unread - 1);
    }
    return dummy;
}


#ifdef PREFETCH

int
invalidatePrefetchedGroup(newsgroup)
struct newsgroup *newsgroup;
/*
 * invalidatePrefetchedGroup sees if a group has been prefetched,
 * and if so, goes back to the old group and invalidates the prefetch
 *
 * returns 0 on failure, non-zero on success
 */
{
    if (PrefetchedGroup != NO_GROUP) {
	freePrefetchedGroupArticle();
	if (!getgroup(newsgroup->name, NIL(art_num), NIL(art_num), NIL(int))) {
	    return FATAL;
	}
	PrefetchedGroup = NO_GROUP;
    }
    return OKAY;
}
#endif /* PREFETCH */

static void
handleXref(article)
art_num article;
{
    char *string, *ptr, *token, group[GROUP_NAME_SIZE], *gptr;
    int count, number;
    struct newsgroup *newsgroup;

    
    xhdr(article, "xref", &string);

    if (string == NIL(char)) {
	/* no xrefs */
	return;
    }

    /*
     * an xrefs line is of the form:
     *
     *   host group:number group:number .... group:number
     */

    if ((ptr = index(string, ' ')) == NIL(char)) {
	FREE(string);
	return;
    }

    while ((token = strtok(ptr, " ")) != NIL(char)) {
	ptr = NIL(char);
	
	count = sscanf(token, "%[^: ]:%d", group, &number);
	if (count != 2) {
	    /* bogus entry */
	    continue;
	}

	if (!avl_lookup(NewsGroupTable, group, &gptr)) {
	    /* bogus group */
	    continue;
	}

	/* only Xref groups that are subscribed to */
	
	newsgroup = (struct newsgroup *) gptr;
	SETARTICLES(newsgroup);

	if (IS_SUBSCRIBED(newsgroup) &&
	    (number >= newsgroup->first) && (number <= newsgroup->last)) {

	    SET_READ(newsgroup->articles[INDEX(number)]);
	}
    }
    FREE(string);
    return;
}

int
getArticle(filename, question)
char **filename;
char **question;
/*
 * get the next article
 *
 * returns: XRN_ERROR - article has been canceled
 *          XRN_OKAY  - article returned
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    struct article *articles = GETARTICLES(newsgroup);
    long indx = CURRENT;
    int header, rotation;


    if (IS_UNFETCHED(articles[indx])) {
#ifdef PREFETCH
	/*
	 * handle the case where this is a 'prev' request at the last
	 * article and the next newsgroup has been prefetched.
	 *
	 * must go back to the current newsgroup so the article can be
	 * retrieved from the server
	 */
	if (!invalidatePrefetchedGroup(newsgroup)) {
	    mesgPane("The newsgroup `%s' has been deleted out from underneath us", newsgroup->name);
	    return XRN_ERROR;
	}
#endif /* PREFETCH */
	
	/* get the article and handle unavailable ones.... */
	header = (IS_ALL_HEADERS(articles[indx]) ? FULL_HEADER : NORMAL_HEADER);
	rotation = (IS_ROTATED(articles[indx]) ? ROTATED : NOT_ROTATED);
	
	if ((articles[indx].filename = getarticle(newsgroup->current, &articles[indx].position, header, rotation)) == NIL(char)) {
	    SET_UNAVAIL(articles[indx]);
	    mesgPane("Can not get the next article, it was canceled by the author or the news file system filled");
	    return XRN_ERROR;
	}
	SET_FETCHED(articles[indx]);
	
    } else {
	/* verify that the file still exists */
	if (access(articles[indx].filename, R_OK) == -1) {
	    /* refetch the file */
	    SET_UNFETCHED(articles[indx]);
	    return getArticle(filename, question);
	}	    
    }

    *filename = articles[indx].filename;
    *question = buildQuestion(newsgroup);
    SET_READ(articles[indx]);
    handleXref(newsgroup->current);
    return XRN_OKAY;
}


int
toggleHeaders(filename, question)
char **filename;
char **question;
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    struct article *articles = GETARTICLES(newsgroup);

    if (IS_ALL_HEADERS(articles[CURRENT])) {
	SET_STRIPPED_HEADERS(articles[CURRENT]);
    } else {
	SET_ALL_HEADERS(articles[CURRENT]);
    }	
    CLEAR_FILE(articles[CURRENT]);
    return getArticle(filename, question);
}


int
toggleRotation(filename, question)
char **filename;
char **question;
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    struct article *articles = GETARTICLES(newsgroup);

    if (IS_ROTATED(articles[CURRENT])) {
	SET_UNROTATED(articles[CURRENT]);
    } else {
	SET_ROTATED(articles[CURRENT]);
    }	
    CLEAR_FILE(articles[CURRENT]);
    return getArticle(filename, question);
}


#ifdef PREFETCH

void
prefetchNextGroup()
/*
 * prefetch the next group
 *
 *   sequence of events:
 *    - put the server into the next group
 *    - get the first unread article
 *    - mark that we are in a new group (but don't change CurrentGroupNumber)
 *
 *    returns: void
 */
{
    struct newsgroup *newsgroup;
    struct article *articles;
    long indx;
    ng_num number;


    if (PrefetchGroupMode == DO_NOT_PREFETCH_GROUP) {
	return;
    }

    for (number = CurrentGroupNumber + 1; number < MaxGroupNumber; number++) {
	newsgroup = Newsrc[number];
	/* find a group that is subscribed to and has unread articles */
	if (IS_SUBSCRIBED(newsgroup) && (unreadArticleCount(newsgroup) > 0)) {
	    break;
	}
    }

    if (number == MaxGroupNumber) {
	return;
    }
    if (unreadArticleCount(newsgroup) > PREFETCH_MAX) {
	return;
    }

    setUpGroup(newsgroup, 1);
    indx = CURRENT;
    articles = GETARTICLES(newsgroup);
    
    /* if the current article is unfetched, fetch it */
    if (IS_UNFETCHED(articles[indx])) {
	/* if the article can be fetched, mark it so */
	if ((articles[indx].filename = getarticle(newsgroup->current, &articles[indx].position, NORMAL_HEADER, NOT_ROTATED)) != NIL(char)) {
	    SET_FETCHED(articles[indx]);
	}
    }
    
    PrefetchedGroup = number;

    return;
}


void
prefetchNextArticle()
/*
 * prefetch the next article, will prefetch the next unread article in
 *   the next group at the end of the current group
 *
 * returns: void
 *
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    struct article *articles = GETARTICLES(newsgroup);
    long indx;
    

    /*
     * if this is the last article, prefetch the next group, and the
     * first article of the next group
     */
    if (newsgroup->current == newsgroup->last) {
	prefetchNextGroup();
	return;
    }

    indx = INDEX(newsgroup->current + 1);

    if (IS_UNFETCHED(articles[indx])) {
	/*
	 * XXX HP Global optimizer did the wrong thing when the
	 * assignment and the if where in the same statement 
	 */
	articles[indx].filename = getarticle(newsgroup->current + 1, &articles[indx].position, NORMAL_HEADER, NOT_ROTATED);
	if (articles[indx].filename == NIL(char)) {
	    return;
	}
	SET_FETCHED(articles[indx]);
	SET_STRIPPED_HEADERS(articles[indx]);
	SET_UNROTATED(articles[indx]);
	return;
    }
    return;
}

#endif /* PREFETCH */


#define lsDestroy(lst) \
    if (lst != NIL(struct list)) { \
	struct list *_next; \
	do { \
	    _next = lst->next; \
	    FREE(lst); \
	    lst = _next; \
	} while (lst != NIL(struct list)); \
    }


void
updateArticleArray(newsgroup)
struct newsgroup *newsgroup;    /* newsgroup to update article array for   */
/*
 * mark the articles in a group that have been read
 *
 *   returns: void
 *
 */
{
    struct list *item;
    art_num art;

    if (newsgroup->last == 0) {
	return;
    }

    if (EMPTY_GROUP(newsgroup)) {
	newsgroup->articles = NIL(struct article);
	return;
    }

    if (newsgroup->nglist == NIL(struct list)) {
	return;
    }

    if (!IS_SUBSCRIBED(newsgroup)) {
	newsgroup->articles = NIL(struct article);
	return;
    }

    SETARTICLES(newsgroup);

    /* process the .newsrc line */

    for (item = newsgroup->nglist; item != NIL(struct list); item = item->next) {
	switch (item->type) {
	    case SINGLE:
	    if (item->contents.single > newsgroup->last) {
		/* something really bad has happened, reset */
		mesgPane("Article numbering problem, marking all articles in %s as unread",
			       newsgroup->name);
		for (art = newsgroup->first; art <= newsgroup->last; art++) {
		    newsgroup->articles[INDEX(art)].status = ART_CLEAR;
		}
		lsDestroy(newsgroup->nglist);
		newsgroup->nglist = NIL(struct list);
		return;
	    }
	    if (item->contents.single >= newsgroup->first) {
		newsgroup->articles[INDEX(item->contents.single)].status = ART_CLEAR_READ;
	    }
	    break;

	    case RANGE:
	    if ((item->contents.range.start > newsgroup->last) ||
		(item->contents.range.end > newsgroup->last)) {
		/* something really bad has happened, reset */
		mesgPane("Article numbering problem, marking all articles in %s as unread\n",
			       newsgroup->name);
		for (art = newsgroup->first; art <= newsgroup->last; art++) {
		    newsgroup->articles[INDEX(art)].status = ART_CLEAR;
		}
		lsDestroy(newsgroup->nglist);
		newsgroup->nglist = NIL(struct list);
		return;
	    }
	    if (item->contents.range.start < newsgroup->first) {
		item->contents.range.start = newsgroup->first;
	    }
	    
	    if (item->contents.range.end < newsgroup->first) {
		break;
	    }
	    for (art = item->contents.range.start; art <= item->contents.range.end; art++) {
		newsgroup->articles[INDEX(art)].status = ART_CLEAR_READ;
	    }
	}
    }

    lsDestroy(newsgroup->nglist);
    newsgroup->nglist = NIL(struct list);
    
    return;
}


void
markArticleAsRead(article)
long article;
/*
 * mark an article as read
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];

    SETARTICLES(newsgroup);
    SET_READ(newsgroup->articles[INDEX((art_num) article)]);
    return;
}

    
void
markArticleAsUnread(article)
long article;
/*
 * mark an article as unread
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];

    SETARTICLES(newsgroup);
    SET_UNREAD(newsgroup->articles[INDEX((art_num) article)]);
    return;
}
      


/*
 * handle adding items to the newsrc
 *
 *   3 cases
 *
 *   1. add to the beginning
 *        move 0 to MaxGroupNumber-1 down 1, update newsrc fields
 *        update 0
 *        inc MaxGroupNumber
 *   2. add to the end
 *        inc MaxGroupNumber
 *        update MaxGroupNumber-1
 *   3. add after a group (newloc is the current location of the group)
 *      move newloc+1 to end down 1, update newsrc fields
 *      update newloc
 *      in MaxGroupNumber
 *
 *   And the case of 'subscribe' (assumes it moves)
 *
 *   1. add to the beginning
 *        move 0 to oldloc-1 down 1, update newsrc fields
 *        update 0
 *   2. add to the end
 *        move oldloc+1 to MaxGroupNumber-1 update 1, update newsrc fields
 *        upadte MaxGroupNumber-1
 *   3. add after a group
 *
 */

int
addToNewsrcBeginning(newGroup, status)
char *newGroup;
int status;
{
    char *ptr;
    struct newsgroup *newsgroup;
    ng_num i;
    
    if (!avl_lookup(NewsGroupTable, newGroup, &ptr)) {
	mesgPane("Newsgroup `%s' does not exist", newGroup);
	return BAD_GROUP;
    }
    
    newsgroup = (struct newsgroup *) ptr;

    CLEAR_NOENTRY(newsgroup);
    if (status == SUBSCRIBE) {
	SET_SUB(newsgroup);
    } else {
	SET_UNSUB(newsgroup);
    }
    if (newsgroup->newsrc == NOT_IN_NEWSRC) {
	for (i = MaxGroupNumber - 1; i >= 0; i--) {
	    Newsrc[i + 1] = Newsrc[i];
	    Newsrc[i + 1]->newsrc = i + 1;
	}
    
	MaxGroupNumber++;
	
    } else {
	for (i = newsgroup->newsrc - 1; i >= 0; i--) {
	    Newsrc[i + 1] = Newsrc[i];
	    Newsrc[i + 1]->newsrc = i + 1;
	}
    }
    
    newsgroup->newsrc = 0;
    Newsrc[0] = newsgroup;
    
    return GOOD_GROUP;
}


int
addToNewsrcEnd(newGroup, status)
char *newGroup;
int status;
{
    char *ptr;
    struct newsgroup *newsgroup;
    ng_num i;
    
    if (!avl_lookup(NewsGroupTable, newGroup, &ptr)) {
	mesgPane("Newsgroup `%s' does not exist", newGroup);
	return BAD_GROUP;
    }
    
    newsgroup = (struct newsgroup *) ptr;

    CLEAR_NOENTRY(newsgroup);
    if (status == SUBSCRIBE) {
	SET_SUB(newsgroup);
    } else {
	SET_UNSUB(newsgroup);
    }
    if (newsgroup->newsrc == NOT_IN_NEWSRC) {
	MaxGroupNumber++;
    } else {
	for (i = newsgroup->newsrc + 1; i < MaxGroupNumber; i++) {
	    Newsrc[i - 1] = Newsrc[i];
	    Newsrc[i - 1]->newsrc = i - 1;
	}
    }
    
    newsgroup->newsrc = MaxGroupNumber - 1;
    Newsrc[MaxGroupNumber - 1] = newsgroup;
    
    return GOOD_GROUP;
}


int
addToNewsrcAfterGroup(newGroup, afterGroup, status)
char *newGroup;
char *afterGroup;
int status;
{
    char *ptr;
    struct newsgroup *newsgroup, *ng;
    ng_num newloc, i;
    
    if (!avl_lookup(NewsGroupTable, newGroup, &ptr)) {
	mesgPane("Newsgroup `%s' does not exist", newGroup);
	return BAD_GROUP;
    }
    
    newsgroup = (struct newsgroup *) ptr;

    CLEAR_NOENTRY(newsgroup);
    if (status == SUBSCRIBE) {
	SET_SUB(newsgroup);
    } else {
	SET_UNSUB(newsgroup);
    }
    if (!avl_lookup(NewsGroupTable, afterGroup, &ptr)) {
	mesgPane("Newsgroup `%s' does not exist", afterGroup);
	return BAD_GROUP;
    }
    
    ng = (struct newsgroup *) ptr;
    newloc = ng->newsrc;

    if (newloc == NOT_IN_NEWSRC) {
	mesgPane("Newsgroup `%s' is not in your .newsrc file", afterGroup);
	return BAD_GROUP;
    }

    if (newsgroup->newsrc == NOT_IN_NEWSRC) {
	for (i = MaxGroupNumber - 1; i >= newloc + 1; i--) {
	    Newsrc[i + 1] = Newsrc[i];
	    Newsrc[i + 1]->newsrc = i + 1;
	}

	MaxGroupNumber++;
	newsgroup->newsrc = newloc + 1;
	Newsrc[newloc + 1] = newsgroup;

    } else {
	
	if (newloc + 1 < newsgroup->newsrc) {
	    for (i = newsgroup->newsrc - 1; i >= newloc + 1; i--) {
		Newsrc[i + 1] = Newsrc[i];
		Newsrc[i + 1]->newsrc = i + 1;
	    }
	    newsgroup->newsrc = newloc + 1;
	    Newsrc[newloc + 1] = newsgroup;
	    
	} else if (newsgroup->newsrc < newloc + 1) {
	    for (i = newsgroup->newsrc + 1; i < newloc + 1; i++) {
		Newsrc[i - 1] = Newsrc[i];
		Newsrc[i - 1]->newsrc = i - 1;
	    }
	    newsgroup->newsrc = newloc;
	    Newsrc[newloc] = newsgroup;
	}
	/* if its in the correct location already, don't touch it */
    }
    
    return GOOD_GROUP;
}

	
char *
getStatusString(sorted)
int sorted;
/*
 * build and return a string that shows the subscription status
 * of all newsgroups; assumes all groups have been subscribed or
 * unsubscribed to by this time.
 *
 *   if sorted is non-zero, the list is sorted alphabetically, if
 *    zero, the list is returned as it exists in the newsrc file
 */
{
    int i, count = 0, bytes = 0;
    char buffer[1024];
    avl_generator *gen;
    char *key, *value;
    char **ar;
    char *string;

    ar = ARRAYALLOC(char *, ActiveGroupsCount);

    if (sorted) {
	gen = avl_init_gen(NewsGroupTable, AVL_FORWARD);

	while (avl_gen(gen, &key, &value)) {
	    struct newsgroup *newsgroup = (struct newsgroup *) value;
	    
	    (void) sprintf(buffer, "%-60s%s",
			   newsgroup->name,
			   IS_SUBSCRIBED(newsgroup) ? "subscribed  " : "unsubscribed");
	    ar[count++] = XtNewString(buffer);
	    bytes += utStrlen(buffer);
	}
	avl_free_gen(gen);
	
    } else {
	for (i = 0; i < MaxGroupNumber; i++) {
	    struct newsgroup *newsgroup = (struct newsgroup *) Newsrc[i];
	    
	    (void) sprintf(buffer, "%-60s%s",
			   newsgroup->name,
			   IS_SUBSCRIBED(newsgroup) ? "subscribed  " : "unsubscribed");
	    ar[count++] = XtNewString(buffer);
	    bytes += utStrlen(buffer);
	}
    }	    
    
    string = arrayToString(ar, count, bytes);
    for (i = 0; i < count; i++) {
	FREE(ar[i]);
    }
    FREE(ar);
    
    return string;
}

static char *
getUnSortedSubjects(mode)
int mode;
/*
 * build and return the subjects string
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    struct article *articles = GETARTICLES(newsgroup);
    static lineLength = 52 + 2 + 24 + 1;
    art_num i;
    char *start, *end;

    if (EMPTY_GROUP(newsgroup)) {
	return XtNewString("");
    }

    NextPreviousArticle = newsgroup->current - 1;

    if ((newsgroup->last - newsgroup->current + 1) < 0) {
	(void) sprintf(error_buffer, "Active File Error: last - current + 1 < 0 (%s)\n",
			       newsgroup->name);
	ehErrorExitXRN(error_buffer);
    }
	
    start = ARRAYALLOC(char, ((newsgroup->last - newsgroup->current + 1) * lineLength + 1));
    end = start;
    *start = '\0';

    for (i = newsgroup->current; i <= newsgroup->last; i++) {
	long indx = INDEX(i);

	/* canceled and empty articles will not have a subject entry */
	if (articles[indx].subject != NIL(char)) {
	    char buffer[LINE_LENGTH];
	    
	    /* don't put articles in the string if already read ... */
	    if ((mode == ALL) || IS_UNREAD(articles[indx])) {

		(void) sprintf(buffer, "%-52.52s  %-24.24s\n",
			       articles[indx].subject, articles[indx].author);
		(void) strcpy(end, buffer);
		
		/* mark articles if they have already been read */
		if (IS_READ(articles[indx])) {
		    *end = '+';
		} else {
		    *end = ' ';
		}
		if (IS_SAVED(articles[indx])) {
		    *(end + 1)  = 'S';
		} else {
		    *(end + 1)  = ' ';
		}

		end += lineLength;
		*end = '\0';
	    }
	} else {
	    /* if you can't get the subject, mark the article as unavailable */
	    SET_UNAVAIL(articles[indx]);
	}
    }

    return start;
}


struct entry {
    char *beginning;
    char *end;
    int left;
    int size;
    int startingArticle;
};


#define CHUNK (4 * (52 + 2 + 24 + 1 + 1))


static void
valfree(val)
struct entry *val;
{
    XtFree(val->beginning);
    return;
}


static int
pteCompare(a, b)
char *a, *b;
{
    struct entry **pa = (struct entry **) a;
    struct entry **pb = (struct entry **) b;

    return (*pa)->startingArticle - (*pb)->startingArticle;
}


/*
 * XXX AVL TREE is the wrong data structure here, st table would be
 * better.... no need for ordering based on subject string
 */
static char *
getSortedSubjects(mode)
int mode;
/*
 * build and return the subjects string
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    struct article *articles = GETARTICLES(newsgroup);
    static lineLength = 52 + 2 + 24 + 1;
    art_num i;
    char *start, *end;

    struct entry *pte;
    struct entry **pteArray;

    avl_generator *gen;
    char *key, *ptr;
    avl_tree *tree;
    char curSub[30];
    int treeSize, sz, len;

    if (EMPTY_GROUP(newsgroup)) {
	return XtNewString("");
    }

    tree = avl_init_table(utSubjectCompare);

    NextPreviousArticle = newsgroup->current - 1;

    if ((newsgroup->last - newsgroup->current + 1) < 0) {
	(void) sprintf(error_buffer, "Active File Error: last - current + 1 < 0 (%s)\n",
			       newsgroup->name);
	ehErrorExitXRN(error_buffer);
    }
	
    /* 
     * build the subject groups 
     */
    for (i = newsgroup->current; i <= newsgroup->last; i++) {
	long indx = INDEX(i);

	/* canceled and empty articles will not have a subject entry */
	if (articles[indx].subject != NIL(char)) {
	    char buffer[LINE_LENGTH];

	    /* don't put articles in the string if already read ... */
	    if ((mode == ALL) || IS_UNREAD(articles[indx])) {

		(void) sprintf(buffer, "%-52.52s  %-24.24s\n",
			       articles[indx].subject, articles[indx].author);
		(void) strncpy(curSub, getSubject(i), sizeof(curSub));

		if (avl_lookup(tree, curSub, &ptr)) {
		    /* add to the end */
		    pte = (struct entry *) ptr;
		    if (lineLength > pte->left) {
			/* grow the string */
			pte->size += CHUNK;
			pte->left += CHUNK;
			sz = pte->end - pte->beginning;
			pte->beginning = XtRealloc(pte->beginning, pte->size);
			pte->end = pte->beginning + sz;
			*(pte->end) = '\0';
		    }
		    (void) strcpy(pte->end, buffer);
		    pte->end += lineLength;
		    pte->left -= lineLength;
		    *(pte->end) = '\0';
		} else {
		    /* create new */
		    pte = ALLOC(struct entry);
		    pte->startingArticle = i;
		    pte->beginning = ARRAYALLOC(char, CHUNK);
		    (void) strcpy(pte->beginning, buffer);
		    pte->size = CHUNK;
		    pte->left = pte->size - lineLength;
		    pte->end = pte->beginning + lineLength;
		    *(pte->end) = '\0';
		    avl_insert(tree, XtNewString(curSub), (char *) pte);
		}
	    }
	} else {
	    /* if you can't get the subject, mark the article as unavailable */
	    SET_UNAVAIL(articles[indx]);
	}
    }

    i = 0;
    treeSize = avl_count(tree);
    pteArray = ARRAYALLOC(struct entry *, treeSize);
    gen = avl_init_gen(tree, AVL_FORWARD);
    while (avl_gen(gen, &key, &ptr)) {
	pteArray[i++] = (struct entry *) ptr;
    }
    avl_free_gen(gen);

    /* sort by article number */
    qsort((char *) pteArray, treeSize, sizeof(struct pte *), pteCompare);

    start = ARRAYALLOC(char, ((newsgroup->last - newsgroup->current + 1) * lineLength + 1));
    end = start;
    *start = '\0';

    for (i = 0; i < treeSize; i++) {
	(void) strcpy(end, pteArray[i]->beginning);
	end += utStrlen(pteArray[i]->beginning);
	*end = '\0';
    }

    avl_free_table(tree, XtFree, valfree);
    FREE(pteArray);

    return start;
}


char *
getSubjects(mode)
int mode;
{
    if (app_resources.sortedSubjects) {
	return getSortedSubjects(mode);
    } else {
	return getUnSortedSubjects(mode);
    }
}


void
gotoArticle(article)
long article;
/*
 * set the internal pointers to a particular article
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    newsgroup->current = (art_num) article;
    return;
}


int
checkArticle(art)
art_num art;
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];

    /* Check if requested article is not available */
    if (art < newsgroup->first || art > newsgroup->last) {
        return XRN_ERROR;
    }
    return XRN_OKAY;
}    


void
bogusNewsgroup()
/*
 * first and last are the same and there are no articles
 *
 * representation bug in the news system active file
 *
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];


    if (newsgroup->articles != NIL(struct article)) {
	SET_READ(newsgroup->articles[LAST]);
	SET_UNAVAIL(newsgroup->articles[LAST]);
    }

    mesgPane("No articles in `%s', probably killed or canceled", newsgroup->name);
    
    return;
}


#define STRIPLEADINGSPACES   for (; *start == ' '; start++);
#define STRIPENDINGSPACES  for ( ; *end == ' '; *end = '\0', end--);

static char *
strip(str)
char *str;
{
    register char *start, *end, *ptr;
    static char work[BUFFER_SIZE];

    (void) strncpy(work, str, BUFFER_SIZE);
    start = work;
    work[BUFFER_SIZE - 1] = '\0';
    end = index(start, '\0') - 1;

    /* a tab separates the article number from the subject line */
    start = index(start, '\t');
    assert(start != NIL(char));
    start++;

    STRIPLEADINGSPACES;

    /* strip leading '[rR][eE]: ' and 'Re^N: ' */
    while (STREQN(start, "Re: ", 4) ||
	   STREQN(start, "RE: ", 4) ||
	   STREQN(start, "re: ", 4) ||
	   STREQN(start, "Re: ", 4)) {
	start += 4;
	
	/* strip leading spaces after '[rR]e: ' */
	STRIPLEADINGSPACES;
    }

    while (STREQN(start, "Re^", 3)) {
	start += 3;
	ptr = index(start, ':');
	start = ptr + 1;
	STRIPLEADINGSPACES;
    }

    STRIPENDINGSPACES;

    return start;
}


char *
getSubject(article)
long article;
/*
 * return the subject of an article with trailing/leading spaces stripped,
 * leading '[rR]e: ' stripped, and trailing ' ([wW]as: ' stripped
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    art_num artnum = (art_num) article;

    SETARTICLES(newsgroup);

    return strip(newsgroup->articles[INDEX(artnum)].subject);
}


char *
getAuthor(article)
long article;
/*
 * return the author of an article
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    struct article *articles = GETARTICLES(newsgroup);

    return articles[INDEX((art_num) article)].author;
}


char *
getPrevSubject()
/*
 * get the previous subject (article number is NextPreviousArticle).
 * only called when going off the top of the subject string
 *
 *   returns a point to a static area
 *
 *  NextPreviousArticle is set to current-1 on building the subject string.
 *  NextPreviousArticle is decremented by this routine.
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    struct article *articles = GETARTICLES(newsgroup);
    static char buffer[BUFFER_SIZE];

#ifdef PREFETCH
    if (!invalidatePrefetchedGroup(newsgroup)) {
	mesgPane("The newsgroup has been deleted out from underneath us");
	return NIL(char);
    }
#endif /* PREFETCH */

    /* search for the next available article in the reverse direction */
    for ( ; NextPreviousArticle >= newsgroup->first; NextPreviousArticle--) {
	long indx = INDEX(NextPreviousArticle);
	
	(void) sprintf(error_buffer, "searching article %ld", NextPreviousArticle);
	infoNow(error_buffer);

	/* get the subject (and author) if it does not already exist */
	if (articles[indx].subject == NIL(char)) {

	    /* get the subject and a few more */
	    getsubjectlist(newsgroup,
			   MAX(newsgroup->first, NextPreviousArticle - SUBJECTS),
			   NextPreviousArticle);
	}

	if (articles[indx].author == NIL(char)) {
	    getauthorlist(newsgroup,
			  MAX(newsgroup->first, NextPreviousArticle - SUBJECTS),
			   NextPreviousArticle);

	}

	NextPreviousArticle--;

	if (articles[indx].subject) {
	    (void) sprintf(buffer, "%-52.52s  %-24.24s",
	                   articles[indx].subject,
		           (articles[indx].author == NIL(char) ?
				      "(none)" : articles[indx].author));
	    return buffer;
	}
	/* continue on */
    }

    return NIL(char);
}


static art_num justInCase;   /* old NextPreviousArticle, just in case the search fails */

void
startSearch()
/* the front-end is about to do an article search, save the starting point */
{
    justInCase = NextPreviousArticle;
    return;
}


void
failedSearch()
/* the article search failed, restore the original point */
{
    NextPreviousArticle = justInCase;
    return;
}


void
fillUpArray(art)
art_num art;
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];

    if (art < newsgroup->current) {
	getsubjectlist(newsgroup, art, newsgroup->current - 1);
	getauthorlist(newsgroup, art, newsgroup->current - 1);
    }
    return;
}


/*
 * getinfofromfile	Get a string from a named file
 *			Handle white space and comments.
 *
 *	Parameters:	"file" is the name of the file to read.
 *
 *	Returns:	Pointer to static data area containing the
 *			first non-ws/comment line in the file.
 *			NULL on error (or lack of entry in file).
 *
 *	Side effects:	None.
 */

char *
getinfofromfile(file)
char	*file;
{
	register FILE	*fp;
	register char	*cp;
	static char	buf[256];
	char		*getenv();

	if (file == NULL)
		return (NULL);

	fp = fopen(file, "r");
	if (fp == NULL)
		return (NULL);

	while (fgets(buf, sizeof (buf), fp) != NULL) {
		if (*buf == '\n' || *buf == '#')
			continue;
		cp = index(buf, '\n');
		if (cp)
			*cp = '\0';
		(void) fclose(fp);
		return (buf);
	}

	(void) fclose(fp);
	return (NULL);			 /* No entry */
}

