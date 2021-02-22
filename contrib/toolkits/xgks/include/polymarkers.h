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
 *
 */


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
