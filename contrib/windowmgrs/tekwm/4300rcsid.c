/*
 * Copyright 1989 by Tektronix, Inc. Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of Tektronix not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written prior
 * permission.  Tektronix makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 *
 *	NAME
 *		$(FAMILY)rcsid.c -- version control file
 *
 *	DESCRIPTION
 *		Contains the Tektronix software copyright boilerplate, and
 *		the routine to print the version number and copyright.
 *		This file's RCS version number is updated whenever a UBE
 *		buid is done.  Its RCS version number is
 *		therefore the closest thing to an RCS version number for
 *		the entire program.  The file is checked in on a branch so that
 *		the fourth number is rolled if we are doing an update release.
 *
 *	INTERNAL INCLUDES
 */

#include "Xos.h"

#define NIL(type)   ((type) -511)
/*
 *	EXTERNALS
 */

/*
 *	CONSTANT
 *		Prog_Version, Trade_Secret
 *
 *	DECLARATION
 */

static	char *	Prog_Version = "$Revision: 1.13 $";
static	char *	Trade_Secret[] = {
	"Copyright (c) $Date: 89/11/22 15:15:27 $, Tektronix, Inc.",
	"Confidential -- All proprietary rights reserved.",
	"Licensed Material -- Property of Tektronix, Inc.",
	" ",
	"This program is the property of Tektronix, Inc.  or  others  from",
	"whom  Tektronix has obtained a licensing right, and is considered",
	"by Tektronix to be confidential.  It is protected by  U.S.  copy-",
	"right  law  as an unpublished work and is furnished pursuant to a",
	"written license agreement.  It may not be used, copied or  other-",
	"wise reproduced, or disclosed to others except in accordance with",
	"the terms and conditions of that agreement.",
	NIL(char *)
};
static	char *	License_note[] = {
	"(C) Copyright 1988 Tektronix, Inc.,",
	"        P.O. Box 4600, Beaverton, Oregon, 97076.",
	"Licensed material - program property of Tektronix.  All rights reserved.",
	NIL(char *)
};

/*
 *	INTERNALS
 */

char *	rcsid = "$Header: 4300rcsid.c,v 1.13 89/11/22 15:15:27 glennw Exp $";
#define STARTVERS 11	/* start of number in Prog_Version */

char *version(argc, argv)
int argc;
char *argv[];
{ 
    char * argvzero, * endvers;
    char vers[15];	/* to extract revision number from $Revision: 1.13 $ */
    int i;
    char * dot;

    argvzero = argv[0];

    while (argc-- > 1) {
        argv++;
        if (strcmp(*argv, "-v") == 0 ||
            strcmp(*argv, "-V") == 0) {
	    endvers=rindex(Prog_Version, ' ');	/* at space after number */
	    strncpy(vers, Prog_Version + STARTVERS, endvers - Prog_Version - STARTVERS);
	    vers[endvers - Prog_Version - STARTVERS] = 0;
	    dot = index(vers, '.');
	    if (index(dot, '.'))   /* there are two dots */
		printf("%s update or engineering release version %s\n\n", argvzero, vers);
	    else
		printf("%s version %s\n\n", argvzero, vers);
	    for (i= -1; Trade_Secret[++i] != NIL(char *);) {
	        printf(Trade_Secret[i]);
		putchar('\n');
	    }
            exit(0);
	}
    }
    return(Prog_Version);
   }
