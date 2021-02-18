#ifndef CURSOR_H
#define CURSOR_H

/*
 * $Header: /users/ricks/xrn-11/R4/RCS/cursor.h,v 1.5 89/11/17 12:51:11 ricks Rel $
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
 * cursor.h: functions for manipulating the cursor and/or text in a
 *           text window
 */

#define BACK 0
#define FORWARD 1

#define NEWS_GROUP_LINE "Unread news in %s"

extern void moveBeginning();
extern void moveEnd();
extern int moveCursor();
extern int moveUpWrap();
extern void endInsertionPoint();
extern int getSelection();
extern int getArtSelection();
extern void removeLine();
extern int setCursorCurrent();
extern void currentGroup();
extern void currentMode();
extern int markStringRead();
extern void markAllString();
extern void markArticles();
extern void buildString();
extern void findArticle();
extern int subjectSearch();

#endif /* CURSOR_H */
