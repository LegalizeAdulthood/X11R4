/* -*-C-*-
********************************************************************************
*
* File:         xwebster.h
* RCS:          $Header: xwebster.h,v 1.5 89/03/06 01:42:04 mayer Exp $
* Description:  Miscellaneous macros
* Author:       Niels Mayer, HPLabs
* Created:      Wed Aug 31 14:09:08 1988
* Modified:     Tue Sep 13 15:33:32 1988 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       G-Job
*
* xwebster - dictionary browser
*
* Copyright 1988 Hewlett-Packard Company
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose and without fee is hereby granted, provided
* that the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of HP not be used in advertising or
* publicity pertaining to distribution of the software without specific,
* written prior permission.  HP makes no representations about the
* suitability of this software for any purpose.  It is provided "as is"
* without express or implied warranty.
*
* Please send any improvements, bug fixes, useful modifications, and comments
* to mayer@hplabs.hp.com.
********************************************************************************
*/
static char rcs_identity_xwebster[] = "@(#)$Header: xwebster.h,v 1.5 89/03/06 01:42:04 mayer Exp $";

#include <X11/Xos.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>


/*----------------------------------------------------------------------------
 * Kludges to sweeten Xt's ugly argument processing for widget creation.
 *---------------------------------------------------------------------------*/
static Arg _args[100];
static int _num_args;

#define ARGLIST_RESET() \
  _num_args = 0

#define ARGLIST_ADD(name, value) \
  XtSetArg(_args[_num_args], (name), (value)); _num_args++

#define ARGLIST_SET_LAST_ELT(name, value) \
  XtSetArg(_args[(_num_args - 1)], (name), (value))

#define ARGLIST() \
  _args, _num_args

static char temptext[1024]; /* local text storage, mostly used for sprintf */

#define FONTHEIGHT(f) ((f)->max_bounds.ascent + (f)->max_bounds.descent)
#define FONTWIDTH(f)  (f)->max_bounds.width

#define COPY_STRING(str) \
  (char *) strcpy(XtMalloc((strlen(str)+1) * sizeof(char)), (str))
