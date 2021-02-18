#ifndef lint
static char *rcsid_Error_c = "$XConsortium: Error.c,v 1.5 88/09/06 17:27:51 jim Exp $";
#endif	lint

#include <X11/copyright.h>

/*
 * 		    Copyright 1989 TOSHIBA CORPORATION
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of TOSHIBA Corporation not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. TOSHIBA Corporation makes no 
 * representations about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 * TOSHIBA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
 * TOSHIBA BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
 * SOFTWARE.
 *
 * Author:  
 * 		Katsuhisa Yano
 *		Personal Computer Design Dept.
 *		TOSHIBA CORPORATION OME WORKS
 *		5-20-7 SORD BLD. MASAGO 
 *		CHIBA-CITY CHIBA 260 JAPAN
 *		(0472) 77-8670
 *
 *
 * Original Source: ./clients/uwm/Error.c
 *
 * Changes
 * Modified by TOSHIBA CORPORATION, to handle Japanese-Language.
 *
 */

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */



/*
 * MODIFICATION HISTORY
 *
 * 000 -- M. Gancarz, DEC Ultrix Engineering Group
 */

#ifndef lint
static char *sccsid = "@(#)Error.c	3.8	1/24/86";
#endif

#include "uwm.h"
/*
 * Default error reporting routine.  Called when a random uwm error
 * is encountered.
 */
Error(s)
char *s;	/* Error description string */
{
#ifdef XJP
    fprintf(stderr, "xjpuwm: %s\n", s);
#else
    fprintf(stderr, "uwm: %s\n", s);
#endif
    exit(1);
}

Warning(s)
char *s;	/* Error description string */
{
#ifdef XJP
    fprintf(stderr, "xjpuwm: warning: %s\n", s);
#else
    fprintf(stderr, "uwm: warning: %s\n", s);
#endif
}
