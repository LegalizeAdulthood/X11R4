/* -*-C-*-
********************************************************************************
*
* File:         winterp.h
* RCS:          $Header: winterp.h,v 1.2 89/11/25 05:00:18 mayer Exp $
* Description:  Miscellaneous macros
* Author:       Niels Mayer, HPLabs
* Created:      Wed Aug 31 14:09:08 1988
* Modified:     Sat Nov 25 04:59:56 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r4 contrib tape release
*
* WINTERP 1.0 Copyright 1989 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
**
********************************************************************************
*/

/*----------------------------------------------------------------------------
 * Kludges to sweeten Xt's ugly argument processing for widget creation.
 *---------------------------------------------------------------------------*/
extern Arg _args[];		/* global in winterp.c */
extern int _num_args;		/* global in winterp.c */

#define ARGLIST_RESET() \
  _num_args = 0

#define ARGLIST_ADD(name, value) \
  XtSetArg(_args[_num_args], (name), (value)); _num_args++

#define ARGLIST_SET_LAST_ELT(name, value) \
  XtSetArg(_args[(_num_args - 1)], (name), (value))

#define ARGLIST() \
  _args, _num_args

