/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * Tool: X 11 Graphical Kernel System
 * Author:  Michael Wojcik
 * 
 * Define file for :  predefined text bundles
 * 
 * $Header: text.h,v 4.0 89/08/31 16:35:32 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/text.h,v $
 *
 * $Log:	text.h,v $
 * Revision 4.0  89/08/31  16:35:32  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.5  89/06/05  10:29:29  bruce
 * DCR# d1:	Changed variable initialization of default text bundles
 * 		putting sub-structures inside brackets for the AIX compiler.
 * 
 * Revision 1.4  89/06/01  14:05:51  bruce
 * PTR# c1178:	Changed default structures to accomodate compiler glitch.
 * 
 * Revision 1.3  89/03/03  17:12:30  amy
 * PTR c2027	For each predefined bundle, set character spacing to a value 
 * 		that looks like a "good" default value for that font.
 * 
 * Revision 1.2  89/02/13  17:01:02  amy
 * No change.
 * 
 * Revision 1.1  89/02/04  15:33:30  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 1.3  89/01/30  10:09:19  mike
 * PTR c1143	Changed predefined representation #6 to use font #6.
 * 
 * Revision 1.2  89/01/27  13:17:12  bruce
 * PTR c1154	Added constant with which to initialize default character spacing. 
 * 
 * Revision 1.1  89/01/11  08:58:59  mike
 * Initial revision
 *   
 *
 */


/* c1147:  Moved these here from text.c and made them static */
/* c1178:  Compiler glitch caused us to declare these globally and make     */
/*         them externs in the files they're used in                        */

#define   DEFCHRSPACE    0.0 /* c1154 */

#ifdef text_c                                                   /* c1178 */
Gtxbundl def_txbundl[PDF_TEXT_BNDLS] =				/* c1143 */
	{							/* c1143 */
	   /* predefined text rep 1 */				/* c1143 */
	   {							/* c1143 */
	    {							/* d1 */
              1,		/* font */			/* c1143 */
              GSTROKE		/* precision */			/* c1143 d1 */
	    },							/* d1 */
              1.0,		/* expansion */			/* c1143 */
              0.15,		/* spacing */			/* c1143  c2027 */
              1			/* colour */			/* c1143 */
	   },							/* c1143 */
	   /* predefined text rep 2 */				/* c1143 */
	   {							/* c1143 */
	    {							/* d1 */
              2,		/* font */			/* c1143 */
              GSTROKE		/* precision */			/* c1143 d1 */
	    },							/* d1 */
              1.0,		/* expansion */			/* c1143 */
              0.15,		/* spacing */			/* c1143  c2027 */
              1			/* colour */			/* c1143 */
	   },							/* c1143 */
	   /* predefined text rep 3 */				/* c1143 */
	   {							/* c1143 */
	    {							/* d1 */
              3,		/* font */			/* c1143 */
              GSTROKE		/* precision */			/* c1143 d1 */
	    },							/* d1 */
              1.0,		/* expansion */			/* c1143 */
              0.2,		/* spacing */			/*c1143 c2027*/
              1			/* colour */			/* c1143 */
	   },							/* c1143 */
	   /* predefined text rep 4 */				/* c1143 */
	   {							/* c1143 */
	    {							/* d1 */
              4,		/* font */			/* c1143 */
              GSTROKE		/* precision */			/* c1143 d1 */
	    },							/* d1 */
              1.0,		/* expansion */			/* c1143 */
              0.15,		/* spacing */			/*c1143 c2027*/
              1			/* colour */			/* c1143 */
	   },							/* c1143 */
	   /* predefined text rep 5 */				/* c1143 */
	   {							/* c1143 */
	    {							/* d1 */
              5,		/* font */			/* c1143 */
              GSTROKE		/* precision */			/* c1143 d1 */
	    },							/* d1 */
              1.0,		/* expansion */			/* c1143 */
              0.1,		/* spacing */			/*c1143 c2027*/
              1			/* colour */			/* c1143 */
	   },							/* c1143 */
	   /* predefined text rep 6 */				/* c1143 */
	   {							/* c1143 */
	    {							/* d1 */
              6,		/* font */			/* c1143 */
              GSTROKE		/* precision */			/* c1143 d1 */
	    },							/* d1 */
              1.0,		/* expansion */			/* c1143 */
              0.1,		/* spacing */			/*c1143 c2027*/
              1			/* colour */			/* c1143 */
	   }							/* c1143 */
	};							/* c1143 */

#else                           /* c1178 */
extern                          /* c1178 */
Gtxbundl def_txbundl[];         /* c1178 */
#endif                          /* c1178 */
