#ifndef NEWS_H
#define NEWS_H

/*
 * $Header: /users/ricks/xrn-11/R4/RCS/news.h,v 1.7 89/11/17 12:51:39 ricks Rel $
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


#ifndef AVL_H
#include "avl.h"
#endif

/*
 * news.h: internal data structures
 *
 */

/* handle first and/or last article number changes */
extern void articleArrayResync(/* struct newsgroup *newsgroup, int first, int list */);
extern void updateArticleArray(/* struct newsgroup *newsgroup */);

typedef long art_num;    /* easy way to pick out variables refering to articles */
typedef long ng_num;     /* easy way to pick out newsgroup variables            */

extern avl_tree *NewsGroupTable;
extern int ActiveGroupsCount;

/* this is indexed via 'current - first' */
struct article {
    unsigned long status;
        /* ART_READ, ART_UNREAD; ART_FETCHED, ART_UNFETCHED; ART_UNAVAIL */
    char *subject;        /* subject line                         */
    char *author;         /* author name                          */
    char *filename;       /* name of the article file             */
    long position;        /* header/body seperation point (bytes) */
};


struct newsgroup {
    char *name;        /* name of the group                                 */
    ng_num newsrc;     /* index of this group into Newsrc                   */
    unsigned long status;
        /* NG_SUB, NG_UNSUB, NG_NOENTRY */
        /* NG_POSTABLE, NG_MODERATED, NG_UNPOSTABLE */
    art_num first;     /* first available article number                    */
    art_num last;      /* last article number                               */
    art_num current;   /* current article number                            */
    struct article *articles;
    struct list *nglist;  /* newsgroup entry for unsubscribed groups        */
};

/*
 * NOTE: if first > last, that means that there are no articles available for
 * the group and no articles array will exist
 */


/* article has been read / is unread */
#define ART_READ      0x0001
#define ART_UNREAD    0x0002
/* article has been fetched / is unfetched */
#define ART_FETCHED   0x0010
#define ART_UNFETCHED 0x0020
/* article is not available */
#define ART_UNAVAIL   0x0040

/* headers */
#define ART_ALL_HEADERS      0x0100
#define ART_STRIPPED_HEADERS 0x0200

/* rotation */
#define ART_ROTATED   0x0400
#define ART_UNROTATED 0x0800

/* saved */
#define ART_SAVED   0x4000
#define ART_UNSAVED 0x8000

#define ART_CLEAR       (ART_UNREAD | ART_UNFETCHED | ART_STRIPPED_HEADERS | ART_UNROTATED)
#define ART_CLEAR_READ  (ART_READ | ART_UNFETCHED | ART_STRIPPED_HEADERS | ART_UNROTATED)

/* helper macros */
#define IS_READ(art)       (((art).status & ART_READ) == ART_READ)
#define IS_UNREAD(art)     (((art).status & ART_UNREAD) == ART_UNREAD)
#define IS_FETCHED(art)    (((art).status & ART_FETCHED) == ART_FETCHED)
#define IS_UNFETCHED(art)  (((art).status & ART_UNFETCHED) == ART_UNFETCHED)
#define IS_UNAVAIL(art)    (((art).status & ART_UNAVAIL) == ART_UNAVAIL)
#define IS_ALL_HEADERS(art) (((art).status & ART_ALL_HEADERS) == ART_ALL_HEADERS)
#define IS_ROTATED(art)    (((art).status & ART_ROTATED) == ART_ROTATED)

#define IS_SAVED(art)    (((art).status & ART_SAVED) == ART_SAVED)
  
  
#define SET_READ(art) ((art).status &= ~ART_UNREAD, (art).status |= ART_READ)
#define SET_UNREAD(art) ((art).status &= ~ART_READ, (art).status |= ART_UNREAD)
#define SET_FETCHED(art) ((art).status &= ~ART_UNFETCHED, (art).status |= ART_FETCHED)
#define SET_UNFETCHED(art) ((art).status &= ~ART_FETCHED, (art).status |= ART_UNFETCHED)
#define SET_STRIPPED_HEADERS(art) ((art).status &= ~ART_ALL_HEADERS, (art).status |= ART_STRIPPED_HEADERS)
#define SET_ALL_HEADERS(art) ((art).status &= ~ART_STRIPPED_HEADERS, (art).status |= ART_ALL_HEADERS)
#define SET_UNROTATED(art) ((art).status &= ~ART_ROTATED, (art).status |= ART_UNROTATED)
#define SET_ROTATED(art) ((art).status &= ~ART_UNROTATED, (art).status |= ART_ROTATED)
#define SET_UNAVAIL(art) ((art).status |= ART_UNAVAIL)

#define SET_SAVED(art) ((art).status &= ~ART_UNSAVED, (art).status |= ART_SAVED)

#define CLEAR_FILE(art) \
  if ((art).filename != NIL(char)) { \
      (void) unlink((art).filename); \
      SET_UNFETCHED(art); \
      FREE((art).filename); \
      (art).filename = NIL(char); \
  }	

#define CLEAR_SUBJECT(art) \
  if ((art).subject != NIL(char)) { \
      FREE((art).subject); \
      (art).subject = NIL(char); \
  }	

#define CLEAR_AUTHOR(art) \
  if ((art).author != NIL(char)) { \
      FREE((art).author); \
      (art).author = NIL(char); \
  }	

/* newsgroup is subscribed/unsubscribed to */
#define NG_SUB      0x0010
#define NG_UNSUB    0x0020
/* no entry in the .newsrc for this group */
#define NG_NOENTRY  0x0040
/* newsgroup can be posted to / can not be posted to / is moderated */
#define NG_POSTABLE 0x1000
#define NG_UNPOSTABLE 0x2000
#define NG_MODERATED 0x4000

#define IS_SUBSCRIBED(ng) (((ng)->status & NG_SUB) == NG_SUB)
#define IS_NOENTRY(ng)    (((ng)->status & NG_NOENTRY) == NG_NOENTRY)
  
#define SET_SUB(ng)    ((ng)->status &= ~NG_UNSUB, (ng)->status |= NG_SUB)
#define SET_UNSUB(ng)  ((ng)->status &= ~NG_SUB, (ng)->status |= NG_UNSUB)
#define CLEAR_NOENTRY(ng) ((ng)->status &= ~NG_NOENTRY)

#define EMPTY_GROUP(ng) ((ng)->last < (ng)->first)
  
#define CLEAR_ARTICLES(ng) \
  if ((ng)->articles != NIL(struct article)) { \
      FREE((ng)->articles); \
      (ng)->articles = NIL(struct article); \
  }	

#define INDEX(artnum)  (artnum - newsgroup->first)
#define CURRENT        INDEX(newsgroup->current)
#define LAST           INDEX(newsgroup->last)

extern ng_num CurrentGroupNumber;   /* current index into the newsrc array       */
extern ng_num MaxGroupNumber;       /* size of the newsrc array                  */

struct newsgroup **Newsrc;          /* sequence list for .newsrc file            */

#define NOT_IN_NEWSRC -1            /* must be less than 0 */

/* not a valid group (must be less than 0) */
#define NO_GROUP -1


#define GROUP_NAME_SIZE 128

#endif /* NEWS_H */
