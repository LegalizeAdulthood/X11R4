/* -*-C-*-
********************************************************************************
*
* File:         w_XmString.c
* RCS:          $Header: w_XmString.c,v 1.1 89/11/25 04:19:10 mayer Exp $
* Description:  Interfaces to XmString abominations (sounds of puking deleted)
* Author:       Niels Mayer, HPLabs
* Created:      Sun Nov  5 14:46:20 1989
* Modified:     Sat Nov 25 04:19:01 1989 (Niels Mayer) mayer@hplnpm
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
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_XmString.c,v 1.1 89/11/25 04:19:10 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

/******************************************************************************
 *
 ******************************************************************************/
void Wxms_Garbage_Collect_XmString(lval_xmstring)
     LVAL lval_xmstring;
{
  XmStringFree(get_xmstring(lval_xmstring));
}


/******************************************************************************
 * the XmStrings created here will be freed when the lisp vector holding
 * the XmStrings gets garbage collected and  there are no more references
 * to the XmStrings.
 ******************************************************************************/
LVAL Wxms_XmStringTable_To_Lisp_Vector(xmstrtab, size)
     XmStringTable xmstrtab;
     int           size;
{
  LVAL result;
  int i;
  
  if (!xmstrtab || !size)
    return(NIL);

  xlsave1(result);
  result = newvector(size);
  for (i = 0; i < size; i++)
    setelement(result, i, cv_xmstring(XmStringCopy(xmstrtab[i])));
  xlpop();
  return (result);
}


/******************************************************************************
 * This is equivalent to an xlga*() function, in that it retrieves an argument
 * of a certain type from the argument stack. This particular xlga*() function
 * will retrieve a String OR XmString argument to an XLISP function. If the
 * argument was a string, it will convert it to an XmString and return that,
 * otherwise it returns the XmString-value of the argument.
 *
 * In calling this function, you must pass it a pointer to an LVAL in which
 * this function will store a lisp-node of type XLTYPE_XmString. If this 
 * function ended up converting a string to an XmString, that XmString will be
 * garbage collected unless it is explicitly saved by code in the calling function.
 * In many cases, this fn. is used in methods that return the XLTYPE_XmString
 * node, in which case the XLTYPE_XmString won't be garbage collected if it
 * is referenced in the user's lisp code.
 ******************************************************************************/
XmString Get_String_or_XmString_Arg_Returning_XmString(item)
     LVAL *item;		/* param to return an LVAL_XmString */
{
  *item = xlgetarg();

  if (xmstring_p(*item))
    return (get_xmstring(*item));
  else if (stringp(*item)) {
    XmString result = XmStringCreate((char*) getstring(*item), XmSTRING_DEFAULT_CHARSET);
    *item = cv_xmstring(result); /* note that the XmString created  will get g.c'd if not ref'd */
    return (result);
  }
  else
    xlerror("Bad type: expected either a XmString or a string.", item);
}
