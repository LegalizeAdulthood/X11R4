
/*
 * Copyright 1989 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          120 Rand Hall, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@freedom.graphics.cornell.edu
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xcu/BmgrR.h>

/**
 *** Resource Converter Name
 **/

static void CvtStringToXcuBmgrType() ;	/* Resource Converter */

/**
 *** Resource Converter Quarks
 **/

static XrmQuark	XrmQEtransient,
		XrmQEsingle_toggle,
		XrmQEdouble_toggle,
		XrmQEone_of_many,
		XrmQEany_of_many,
		XrmQEhelp ;	/* XcuBmgrTypes */

/**
 *** Resource Converter AddConverter Routine
 **/

/*
 * e.g., one_of_many
 *       OneOfMany      } all match
 *       One_of_many
 */

void
XcuAddStringToBmgrTypeConverter ()
{
static Boolean first_time = True ;
if (first_time)
    {
    XrmQEtransient   = XrmStringToQuark("transient") ;
    XrmQEsingle_toggle = XrmStringToQuark("singletoggle") ;
    XrmQEdouble_toggle  = XrmStringToQuark("doubletoggle") ;
    XrmQEone_of_many  = XrmStringToQuark("oneofmany") ;
    XrmQEany_of_many  = XrmStringToQuark("anyofmany") ;
    XrmQEhelp  = XrmStringToQuark("help") ;
    XtAddConverter( XtRString, XtRXcuBmgrType, CvtStringToXcuBmgrType, NULL, 0);
    first_time = False ;
    }
return ;
}

/**
 *** Resource Converter Routine
 **/

#include <ctype.h>

/*
 * Convert String To XcuBmgrType
 */

static void CvtStringToXcuBmgrType(args, num_args, fromVal, toVal)
    XrmValuePtr *args ;		/* unused */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
XrmQuark	q ;
Cardinal	i, j ;
char	*s = (char *) fromVal->addr ;
char    lowerName[1000] ;
char	*name_ptr ;
static XcuBmgrType	e ; /* must be static! */

if (s == NULL) return ;

for (i=0,j=0; i<=strlen(s); i++)
    {
    char c = s[i] ;
    lowerName[j] = isupper(c) ? (char) tolower(c) : c ;
    /* squeeze out underscores */
    if (lowerName[j] != '_')
	j++ ;
    }

while (lowerName[j] == '_')
    lowerName[j--] = '\0' ;

/* Allow the name to be preceded with "XcuBmgr" */
name_ptr = lowerName ;
if (strncmp (lowerName, "xcubmgr", strlen("xcubmgr")) == 0)
    name_ptr += strlen("xcubmgr") ;

q = XrmStringToQuark(name_ptr) ;

toVal->size = sizeof(XcuBmgrType) ;
toVal->addr = (caddr_t) &e ;

if (q == XrmQEtransient)   { e = XcuBMGR_TRANSIENT; return; }
if (q == XrmQEsingle_toggle) { e = XcuBMGR_SINGLE_TOGGLE; return; }
if (q == XrmQEdouble_toggle)  { e = XcuBMGR_DOUBLE_TOGGLE; return; }
if (q == XrmQEone_of_many)  { e = XcuBMGR_ONE_OF_MANY;  return; }
if (q == XrmQEany_of_many)  { e = XcuBMGR_ANY_OF_MANY;  return; }
if (q == XrmQEhelp)  { e = XcuBMGR_HELP;  return; }

toVal->size = 0 ;
toVal->addr = NULL ;
return ;
}

