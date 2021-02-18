/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/



/******************** START OF MODULE SPECIFICATIONS *********************
*
* MODULE NAME:		imload.c
*
* DESCRIPTIVE NAME:	IMCORE
*
* FUNCTION:		This function is used for a stub of load() function
*			if the system doesn't support load() function.
*			See the description in the imloadable.c.
*
* MODULE TYPE:		C
*
* COMPILER:		AIX C
*
* CHANGE ACTIVITY:	0%G% - Modified
*
* STATUS:		Input Method Version %Q%
*
******************** END OF SPECIFICATIONS ******************************/

#ifdef	IMSTATICLOAD
#include <sys/types.h>
#include "im.h"

load(lang)
IMLanguage	lang;
{
	int	ret;

#include "imloadable.c"		/* this file includes C source flagments */

	return NULL;
}

/*ARGSUSED*/
unload(dummy)
int	dummy;
{
	/* does nothing */
}

#endif	IMSTATICLOAD
