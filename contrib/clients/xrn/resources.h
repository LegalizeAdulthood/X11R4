#ifndef RESOURCES_H
#define RESOURCES_H

/*
 * $Header: /users/ricks/xrn-11/R4/RCS/resources.h,v 1.11 89/11/17 12:51:55 ricks Rel $
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


#ifndef _XtIntrinsic_h
#include <X11/Intrinsic.h>
#endif

#ifndef AVL_H
#include "avl.h"
#endif

/*
 * resources.h: resources for xrn
 */

#define NAME_SIZE 1024

typedef struct {
    char progName[NAME_SIZE];
    avl_tree *headerTree;
    int headerMode;
    char *geometry;
    char *iconGeometry;
    char *saveDir;
    char *expandedSaveDir;
    char *newsrcFile;
    char *saveNewsrcFile;
    char *signatureFile;
    char *nntpServer;
    int topLines;
    int saveMode;
    char *leaveHeaders;
    char *stripHeaders;
    char *savePostings;
    char *deadLetters;
    int minLines;
    int maxLines;
    char *mailer;
    Boolean iconOnStart;
    Boolean subjectRead;
    char *tmpDir;
    char *confirm;
    int confirmMode;
    Boolean killFiles;
    char *userBitmap;
    char *editorCommand;
    char *strSaveMode;
    char *organization;
    char *replyTo;
    Boolean includeHeader;
    Boolean includeSep;
    Boolean updateNewsrc;
    int lineLength;
    int breakLength;
    Boolean pageArticles;
    Boolean sortedSubjects;
    char *addButtonList;
    char *ngButtonList;
    char *allButtonList;
    char *artButtonList;
    char *artSpecButtonList;
    char *printCommand;
    char *ngBindings;
    char *allBindings;
    char *addBindings;
    char *artBindings;
} app_resourceRec, *app_res;

extern app_resourceRec app_resources;

extern Widget Initialize(/* int argc, char **argv */);

/* article save options */

#define MAILBOX_SAVE   0x01
#define NORMAL_SAVE    0x02
#define HEADERS_SAVE   0x10
#define NOHEADERS_SAVE 0x20

#define ONEDIR_SAVE    0x100
#define SUBDIRS_SAVE   0x200

/* confirm box options */

#define NG_EXIT 0x01
#define NG_QUIT 0x02
#define NG_CATCHUP 0x04
#define NG_UNSUBSCRIBE 0x08
#define ART_CATCHUP 0x10
#define ART_PART_CATCHUP 0x20
#define ART_UNSUBSCRIBE 0x40


/* header options */

#define STRIP_HEADERS  0
#define LEAVE_HEADERS  1

#endif /* RESOURCES_H */
