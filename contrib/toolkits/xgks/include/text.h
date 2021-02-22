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
 * Define file for :  predefined text bundles
 * 
 *
 */

#if !defined(XGKS_TEXT_H)
#define XGKS_TEXT_H

void XgksComputeVec(Gpoint *up_vec, Gpoint *base_vec);

/* c1178:  Compiler glitch caused us to declare these globally and make     */
/*         them externs in the files they're used in                        */

#define   DEFCHRSPACE    0.0 /* c1154 */

#ifdef text_c                                                   /* c1178 */
Gtxbundl def_txbundl[PDF_TEXT_BNDLS] =
        {
           /* predefined text rep 1 */
           {
            {                                                   /* d1 */
              1,                /* font */
              GSTROKE           /* precision */
            },                                                  /* d1 */
              1.0,              /* expansion */
              0.15,             /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 2 */
           {
            {                                                   /* d1 */
              2,                /* font */
              GSTROKE           /* precision */
            },                                                  /* d1 */
              1.0,              /* expansion */
              0.15,             /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 3 */
           {
            {                                                   /* d1 */
              3,                /* font */
              GSTROKE           /* precision */
            },                                                  /* d1 */
              1.0,              /* expansion */
              0.2,              /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 4 */
           {
            {                                                   /* d1 */
              4,                /* font */
              GSTROKE           /* precision */
            },                                                  /* d1 */
              1.0,              /* expansion */
              0.15,             /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 5 */
           {
            {                                                   /* d1 */
              5,                /* font */
              GSTROKE           /* precision */
            },                                                  /* d1 */
              1.0,              /* expansion */
              0.1,              /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 6 */
           {
            {                                                   /* d1 */
              6,                /* font */
              GSTROKE           /* precision */
            },                                                  /* d1 */
              1.0,              /* expansion */
              0.1,              /* spacing */
              1                 /* colour */
           }
        };

#else                           /* c1178 */
extern                          /* c1178 */
Gtxbundl def_txbundl[];         /* c1178 */
#endif                          /* c1178 */

#endif
