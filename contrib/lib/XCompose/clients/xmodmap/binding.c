#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: binding.c,v 1.2 89/11/13 09:02:27 glennw Exp $";
#endif RCS_ID
#endif LINT

/*
 * Copyright 1989 by the Massachusetts Institute of Technology, 
 * Cambridge, Massachusetts, and Tektronix, Inc. Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 *
 *	NAME
 *		binding.c - reads multilingual keyboard binding files
 *
 *	DESCRIPTION
 *	    This file contains the code for parsing the (non-standard)
 *	    multilingual keyboard modifier mapping, and creating the 
 *	    binding properties.
 *
 *
 */

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <ctype.h>
#include "xmodmap.h"

/*
 * handle_binding_line - this routine parses a line from the bindings file
 * (which has had all leading and trailing whitespace removed)
 * and builds up the bindings table.
 */

int handle_binding_line (line, len)
    char *line;				/* string to parse */
    int len;				/* length of line */
{
    int n;
    struct dt *dtp;
    int status;

    n = skip_chars (line, len);
    if (n < 0) {
	badmsg ("input line '%s'", line);
	return (-1);
    }

	    n += skip_space (line+n, len-n);
	    line += n, len -= n;

	    return (status);

    fprintf (stderr, "%s:  unknown command on line %s:%d\n",
	     ProgramName, bindingFilename, lineno);
    return (-1);
}

/* 
 * Determine the default modifier pattern for a modifier not explicitly
 * defined.  All modifier bits are relevant.
 */

char default_modifier(modifier)
char modifier;
{
    return(0);
}

/*
 * fill in all undefined modifier patterns with the default.
 */

set_undefined_modifiers()
{
    
}
