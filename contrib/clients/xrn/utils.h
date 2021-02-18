#ifndef UTILS_H
#define UTILS_H

/*
 * $Header: /users/ricks/xrn-11/R4/RCS/utils.h,v 1.11 89/11/17 12:52:42 ricks Rel Locker: ricks $
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
 * utils.h: random utility functions and macros for xrn
 */

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

extern char *mktemp(/* char *filename */);
extern char *strtok(/* char *string, *seperators */);
extern char *index(/* char *string, char character */);
extern char *rindex(/* char *string, char character */);
extern char *getenv(/* char *variable */);

#ifdef macII
extern int strcmp();
#endif

extern char *XtMalloc();
extern void XtFree();
extern char *XtNewString();

/* allocation macros */
#define ALLOC(type)           (type *) XtMalloc((unsigned) sizeof(type))
#define ARRAYALLOC(type, sz)  (type *) XtMalloc((unsigned) (sizeof(type) * (sz)))
#define NIL(type)             (type *) 0
#define FREE(item)            if ((char *) item != NIL(char)) XtFree((char *) item)
#ifdef VMS
extern int utGroupToVmsFilename(char *filename, char *group);
#endif
#define STREQ(a,b)            (strcmp(a, b) == 0)
#define STREQN(a,b,n)         (strncmp(a, b, n) == 0)

extern char *utTrimSpaces(/* char *string */);
extern char *utTildeExpand(/* char *filename */);
extern int utSubstring(/* char *string, *substring */);
extern void utDowncase(/* char *string */);

#define utStrlen(s)	((s) ? strlen(s) : 0)

extern int utSubjectCompare(/* char *str1, *str2 */);

#endif /* UTILS_H */
