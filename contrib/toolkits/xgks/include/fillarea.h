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
 * Define file for :  fillarea predefined bundles
 * 
 * $Header: fillarea.h,v 4.0 89/08/31 16:33:58 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/fillarea.h,v $
 *
 * $Log:	fillarea.h,v $
 * Revision 4.0  89/08/31  16:33:58  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.2  89/06/01  14:06:11  bruce
 * PTR# c1178:	Changed default structures to accomodate compiler glitch.
 * 
 * Revision 1.1  89/02/04  15:31:45  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 1.2  89/01/30  10:07:22  mike
 * PTR c1143:  Changed fill area bundles to include one pattern.
 * 
 * Revision 1.1  89/01/11  08:57:43  mike
 * Initial revision
 *   
 *
 */

/* c1147:  Moved these predefined reps from fillarea.c and made them static */
/* c1178:  Compiler glitch caused us to declare these globally and make     */
/*         them externs in the files they're used in                        */

#ifdef fillarea_c                                               /* c1178 */
Gflbundl def_flbundl[PDF_FILL_BNDLS] =				/* c1143 */
	{							/* c1143 */
	   /* predefined fill rep 1 */				/* c1143 */
           {							/* c1143 */
              GHOLLOW,		/* interior style */		/* c1143 */
              1,		/* style index */		/* c1143 */
              1			/* colour index */		/* c1143 */
	   },							/* c1143 */
	   /* predefined fill rep 2 */				/* c1143 */
           {							/* c1143 */
              GSOLID,		/* interior style */		/* c1143 */
              1,		/* style index */		/* c1143 */
              1			/* colour index */		/* c1143 */
           },							/* c1143 */
	   /* predefined fill rep 3 */				/* c1143 */
           {							/* c1143 */
              GPATTERN,		/* interior style */		/* c1143 */
              1,		/* style index */		/* c1143 */
              1			/* colour index */		/* c1143 */
           },							/* c1143 */
	   /* predefined fill rep 4 */				/* c1143 */
           {							/* c1143 */
              GHATCH,		/* interior style */		/* c1143 */
              -1,		/* style index */		/* c1143 c1174 */
              1			/* colour index */		/* c1143 */
           },							/* c1143 */
	   /* predefined fill rep 5 */				/* c1143 */
           {							/* c1143 */
              GHATCH,		/* interior style */		/* c1143 */
              -10,		/* style index */		/* c1143 c1174 */
              1			/* colour index */		/* c1143 */
           }							/* c1143 */
	};							/* c1143 */

/* c1144:  Created predefined pattern attributes */
PTATTR  def_ptattr =
        {
           { 1.0, 0.0 },   /* pattern width vector */       /* c1144 */
           { 0.0, 1.0 },   /* pattern height vector */      /* c1144 */
           { 0.0, 0.0 }    /* pattern reference point */    /* c1144 */
        };

#else                           /* c1178 */
extern                          /* c1178 */
Gflbundl def_flbundl[];         /* c1178 */

extern                          /* c1178 */
PTATTR  def_ptattr;             /* c1178 */
#endif                          /* c1178 */
