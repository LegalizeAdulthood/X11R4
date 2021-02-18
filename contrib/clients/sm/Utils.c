/***********************************************************

$Header: Utils.c,v 3.0 89/11/20 09:25:44 mikew Exp $

Copyright 1989 by Mike Wexler, Santa Clara, Ca.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Mike Wexler or not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

MIKE WEXLER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include "osdefs.h"
#include "PatchLevel.h"
#include <stdio.h>
#include <errno.h>

/*
 * Description: Print a fatal error message.
 * 
 * Side Effects: Exits
 *
 */

void 
Fatal(module, message)
    char           *module;	/* the name of the module */
    char           *message;	/* the message to print */
{
    fprintf(stderr, "sm v%f: %s(%s)\n", VERSION, message, module);
    exit(1);
}

/*
 * Description: Print a warning message
 * 
 */

void 
Warning(module, message)
    char           *module;	/* the name of the module */
    char           *message;	/* the error message */
{
    fprintf(stderr, "sm v%f: %s(%s)\n", VERSION, message, module);
    return;
}

/*
 * Description: Print warning message caused by failed system library calls.
 * 
 */

void 
PWarning(module)
    char           *module;	/* module in which the error occurred */
{
    char            text[128];
    sprintf(text, "sm v%f: %s", VERSION, module);
    perror(text);
    return;
}
