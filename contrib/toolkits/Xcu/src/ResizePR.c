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

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xcu/ResizePR.h>

/**
 *** Resource Converter Name
 **/

static void CvtStringToResizeParticipants() ;	/* Resource Converter */

/**
 *** Resource Converter Quarks
 **/

/* ResizeParticipants */
static XrmQuark	XrmQEnone, XrmQEinters, XrmQEinternals, XrmQEchildren ;

/**
 *** Resource Converter AddConverter Routine
 **/

void
XcuAddStringToResizeParticipantsConverter ()
{
static Boolean first_time = True ;
if (first_time)
    {
    XrmQEnone   = XrmStringToQuark("none") ;
    XrmQEinters = XrmStringToQuark("inters") ;
    XrmQEinternals = XrmStringToQuark("internals") ;
    XrmQEchildren  = XrmStringToQuark("children") ;
    XtAddConverter (XtRString, XtRResizeParticipants,
		    CvtStringToResizeParticipants, NULL, 0 ) ;
    first_time = False ;
    }
return ;
}

/**
 *** Resource Converter Routine
 **/

#include <ctype.h>

/*
 * Convert String To ResizeParticipants
 */

static void CvtStringToResizeParticipants(args, num_args, fromVal, toVal)
    XrmValuePtr *args ;		/* unused */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
static XtArgVal	e ; /* must be static! */
XrmQuark	q ;
Cardinal	i ;
Cardinal	j ;
Cardinal	iptr ;
char	*s = (char *) fromVal->addr ;
char    lowerName[1000] ;

if (s == NULL) return ;

for (i=0,j=0; i<=strlen(s); i++)
    {
    char c = s[i] ;
    if (isspace(c))
	continue ;
    lowerName[j] = isupper(c) ? (char) tolower(c) : c ;
    j++ ;
    }
lowerName[j] = '\0' ;

e = 0 ;

toVal->size = sizeof(XcuResizeParticipants) ;
toVal->addr = (caddr_t) &e ;

iptr = 0 ;
for (i=0;  i < strlen(lowerName)+1;  i++)
    {
    if (lowerName[i] == '|' || lowerName[i] == '+' || lowerName[i] == '\0')
	{
	lowerName[i] = '\0' ;
	q = XrmStringToQuark(&lowerName[iptr]) ;
	if      (q == XrmQEnone)      e |= (XtArgVal)XcuRESIZE_NONE ;
	else if (q == XrmQEinters)    e |= (XtArgVal)XcuRESIZE_INTERS ;
	else if (q == XrmQEinternals) e |= (XtArgVal)XcuRESIZE_INTERNALS ;
	else if (q == XrmQEchildren)  e |= (XtArgVal)XcuRESIZE_CHILDREN ;
	else
	    {
	    char text[80] ;
	    sprintf (text, "(%s) is not a valid ResizeParticipants item\n",
		     lowerName[i]) ;
	    XtError (text) ;
	    }
	iptr = i+1 ;
	}
    }

return ;
}

