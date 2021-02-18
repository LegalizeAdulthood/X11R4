#ifndef INTERNALS_H
#define INTERNALS_H

/*
 * $Header: /users/ricks/xrn-11/R4/RCS/internals.h,v 1.8 89/11/17 12:51:28 ricks Rel $
 */

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
 * internals.h: news system interface
 */

#define XRN_ERROR 0
#define XRN_NOMORE 0
#define XRN_OKAY   1

/* 
 * kill file stuff
 */
#define KILL_GLOBAL 0
#define KILL_LOCAL 1

/*
 * routines for adding newsgroups to the newsrc file
 */
 
/* 'status' values for the 'add' functions */
#define SUBSCRIBE 1
#define UNSUBSCRIBE 0

extern int addToNewsrcAfterGroup(/* char *newGroup, *afterGroup, int status  */);
extern int addToNewsrcEnd(/* char *newGroup, int status */);
extern int addToNewsrcBeginning(/* char *newGroup, int status */);


/*
 * routines for doing newsgroup management
 */

/* jump/goto/add newsgroup codes */
#define BAD_GROUP -1
#define GOOD_GROUP 1

/* set the internal group to 'name' */
extern int gotoNewsgroup(/* char *name */);

/* set the internal group to 'name', and if all of the articles are read, unmark the last one */
extern int jumpToNewsgroup(/* char *name */);

/* getNewsgroup returns 0 if a bad group, 1 if okay */
extern int getNewsgroup();

#ifdef PREFETCH
/*
 * invalidatePrefetchedGroup sees if a group has been prefetched,
 * and if so, goes back to the old group and invalidates the prefetch
 *
 * returns 0 on failure, non-zero on success
 */
extern int invalidatePrefetchedGroup(/* struct newsgroup *newsgroup */);
#endif /* PREFETCH */

/* subscribe to the current newsgroup */
extern void subscribe();

/* unsubscribe to the current newsgroup */
extern void unsubscribe();

/* updates the .newsrc file so that the current newsgroup is marked as all read */
extern void catchUp();


/*
 * routines for doing article management
 */

extern void gotoArticle(/* long article */);

#ifndef VMS
extern int getArticle(/* char **filename, char **question */);
#else
extern int getArticleFile(/* char **filename, char **question */);
#endif
extern int toggleHeaders(/* char **filename, char **question */);
extern int toggleRotation(/* char **filename, char **question */);

#ifdef PREFETCH
extern void prefetchNextArticle();
#endif /* PREFETCH */

/* mark articles */
extern void markArticleAsRead(/* long article */);
extern void markArticleAsUnread(/* long article */);

/* get a single subject line, stripped of leading/trailing spaces, Re: */
extern char *getSubject(/* long article */);
extern char *getAuthor(/* long article */);

/* get the subject line for the previous subject (and get the article too) */
/* only called when going off the top of the subject string */
extern char *getPrevSubject();

extern void startSearch();
extern void failedSearch();

/*
 * information gathering routines
 */

extern void checkLock();
extern void removeLock();
  
/* read the .newsrc file, find out what to read */
extern void initializeNews(/* char *newsrcfile */);

/* query the server for new information */
extern void rescanServer();

/* return the new newsgroups string */
extern char *newGroups();

/* return the unread newsgroups string */
extern char *unreadGroups();

/* return the subject string */
#define ALL 0
#define UNREAD 1
extern char *getSubjects(/* int mode */);

/* build and return the status string */
extern char *getStatusString();

/*
 * clean up routines
 */

extern void bogusNewsgroup();

extern void releaseNewsgroupResources();
extern void cleanUpNewsrc();
#ifdef PREFETCH
extern void freePrefetchedGroupArticle();
#endif /* PREFETCH */

#endif /* INTERNALS_H */
