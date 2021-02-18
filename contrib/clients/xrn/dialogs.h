#ifndef DIALOGS_H
#define DIALOGS_H

/*
 * $Header: /users/ricks/xrn-11/R4/RCS/dialogs.h,v 1.5 89/11/17 12:51:16 ricks Rel $
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
 * dialogs.h: create simple moded dialog boxes
 *
 */


/* description of a button */
struct DialogArg {
    char *buttonName;
    void (*handler)();
    caddr_t data;
};

#define DIALOG_TEXT ""
#define DIALOG_NOTEXT (char *)0

extern Widget CreateDialog(/* Widget parent, char *title, char *text, struct DialogArgs *args, int count */);

extern void PopUpDialog(/* Widget dialog */);
extern void PopDownDialog(/* Widget dialog */);

extern char *GetDialogValue(/* Widget dialog */);

extern int ComfirmBox(/* Widget parent, char *message */);

#define XRN_CB_ABORT 0
#define XRN_CB_CONTINUE 1

#endif /* DIALOGS_H */

