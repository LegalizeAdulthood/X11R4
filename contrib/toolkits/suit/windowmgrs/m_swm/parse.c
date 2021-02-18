/*
 *	Original Source: ./contrib/windowmgrs/twm/parse.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*****************************************************************************/
/**       Copyright 1988 by Evans & Sutherland Computer Corporation,        **/
/**                          Salt Lake City, Utah                           **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    name  of Evans & Sutherland  not be used in advertising or publi-    **/
/**    city pertaining to distribution  of the software without  specif-    **/
/**    ic, written prior permission.                                        **/
/**                                                                         **/
/**    EVANS  & SUTHERLAND  DISCLAIMS  ALL  WARRANTIES  WITH  REGARD  TO    **/
/**    THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILI-    **/
/**    TY AND FITNESS, IN NO EVENT SHALL EVANS &  SUTHERLAND  BE  LIABLE    **/
/**    FOR  ANY  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY  DAM-    **/
/**    AGES  WHATSOEVER RESULTING FROM  LOSS OF USE,  DATA  OR  PROFITS,    **/
/**    WHETHER   IN  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS    **/
/**    ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE  OR PER-    **/
/**    FORMANCE OF THIS SOFTWARE.                                           **/
/*****************************************************************************/

/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/***********************************************************************
 *
 * $Header: parse.c,v 1.16 88/07/19 13:49:08 toml Exp $
 *
 * parse the .swmrc file
 *
 * 17-Nov-87 Thomas E. LaStrange		File created
 *
 ***********************************************************************/

#ifndef lint
static char RCSinfo[]=
"$Header: parse.c,v 1.16 88/07/19 13:49:08 toml Exp $";
#endif

#include <stdio.h>
#include "swm.h"
#include "menus.h"
#include "util.h"

#define BUF_LEN 300

static FILE *swmrc;
static int ptr = 0;
static int len = 0;
static char buff[BUF_LEN+1];
extern int yylineno;

/***********************************************************************
 *
 *  Procedure:
 *	ParseSwmrc - parse the .swmrc file
 *
 *  Inputs:
 *	filename  - the filename to parse.  A NULL indicates $HOME/.swmrc
 *
 ***********************************************************************
 */

void
ParseSwmrc(filename)
char *filename;
{
    char *home;
    char init_file[200];

    InitMenus();

    if (filename == NULL)
    {
	home = (char *)getenv("HOME");
	strcpy(init_file, home);
	strcat(init_file, "/.swmrc");
    }
    else
	strcpy(init_file, filename);

    if ((swmrc = fopen(init_file, "r")) == NULL)
    {
	fprintf(stderr, "swm: couldn't open \"%s\"\n", init_file);
    	return;
    }

    ptr = 0;
    len = 0;
    yylineno = 0;
    ParseError = FALSE;

    yyparse();

    fclose(swmrc);

    if (ParseError)
    {
	fprintf(stderr, "swm: errors found in \"%s\", swm aborting\n",
	    init_file);
	Done();
    }
}

/***********************************************************************
 *
 *  Procedure:
 *	SwmInput - redefinition of the lex input routine
 *
 *  Returned Value:
 *	the next input character
 *
 ***********************************************************************
 */

int
SwmInput()
{
    while (ptr == len)
    {
	if (fgets(buff, BUF_LEN, swmrc) == NULL)
	    return NULL;

	yylineno++;

	ptr = 0;
	len = strlen(buff);
    }
    return ((int)buff[ptr++]);
}

/***********************************************************************
 *
 *  Procedure:
 *	SwmUnput - redefinition of the lex unput routine
 *
 *  Inputs:
 *	c	- the character to push back onto the input stream
 *
 ***********************************************************************
 */

void
SwmUnput(c)
{
    buff[--ptr] = c;
}

/***********************************************************************
 *
 *  Procedure:
 *	SwmOutput - redefinition of the lex output routine
 *
 *  Inputs:
 *	c	- the character to print
 *
 ***********************************************************************
 */

void
SwmOutput(c)
{
    putchar(c);
}
