/*
 *              Copyright IBM Corporation 1989
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
 * Define file for :  polymarker predefined bundles
 * 
 * $Header: polymarkers.h,v 4.0 89/08/31 16:35:20 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/polymarkers.h,v $
 *
 * $Log:        polymarkers.h,v $
 * Revision 4.0  89/08/31  16:35:20  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.3  89/06/01  14:06:04  bruce
 * PTR# c1178:  Changed default structures to accomodate compiler glitch.
 * 
 * Revision 1.2  89/02/13  16:59:10  amy
 * No change.
 * 
 * Revision 1.1  89/02/04  15:33:16  amy
 * PTR c1147    Make global vars. and functions private, or static where possible.
 * 
 * Revision 1.2  89/01/27  13:17:59  bruce
 * PTR c1154    Added constant with which to initialize default polymarker size 
 *              scale factor.
 * 
 * Revision 1.1  89/01/11  08:58:43  mike
 * Initial revision
 *   
 *
 */


/* c1147:  Moved bundles here from polymarkers.c and made them static */
/* c1178:  Compiler glitch caused us to declare these globally and make     */
/*         them externs in the files they're used in                        */

#define  DEFMRKRSIZE  1.0       /* c1154 */

#ifdef polymarkers_c                                            /* c1178 */
Gmkbundl def_mkbundl[PDF_MARK_BNDLS] =                          /* c1143 */
        {                                                       /* c1143 */
           /* predefined marker rep 1 */                        /* c1143 */
           {                                                    /* c1143 */
              GMK_STAR,         /* type */                      /* c1143 */
              6.0,              /* size */                      /* c1143 */
              1                 /* colour */                    /* c1143 */
           },                                                   /* c1143 */
           /* predefined marker rep 2 */                        /* c1143 */
           {                                                    /* c1143 */
              GMK_POINT,        /* type */                      /* c1143 */
              6.0,              /* size */                      /* c1143 */
              1                 /* colour */                    /* c1143 */
           },                                                   /* c1143 */
           /* predefined marker rep 3 */                        /* c1143 */
           {                                                    /* c1143 */
              GMK_PLUS,         /* type */                      /* c1143 */
              6.0,              /* size */                      /* c1143 */
              1                 /* colour */                    /* c1143 */
           },                                                   /* c1143 */
           /* predefined marker rep 4 */                        /* c1143 */
           {                                                    /* c1143 */
              GMK_O,            /* type */                      /* c1143 */
              6.0,              /* size */                      /* c1143 */
              1                 /* colour */                    /* c1143 */
           },                                                   /* c1143 */
           /* predefined marker rep 5 */                        /* c1143 */
           {                                                    /* c1143 */
              GMK_X,            /* type */                      /* c1143 */
              6.0,              /* size */                      /* c1143 */
              1                 /* colour */                    /* c1143 */
           }                                                    /* c1143 */
        };                                                      /* c1143 */

#else                           /* c1178 */
extern                          /* c1178 */
Gmkbundl def_mkbundl[];         /* c1178 */
#endif                          /* c1178 */
