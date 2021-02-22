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
 * Define file for :  polyline predefined bundles
 *
 */

#if !defined(XGKS_POLYLINES_H)
#define XGKS_POLYLINES_H

/* c1178:  Compiler glitch caused us to declare these globally and make     */
/*         them externs in the files they're used in                        */

#ifdef polylines_c                                              /* c1178 */
Glnbundl def_lnbundl[PDF_LINE_BNDLS] =
        {
           /* predefined line rep 1 */
           {
              GLN_SOLID,        /* type */
              1.0,              /* width */
              1                 /* colour */
           },
           /* predefined line rep 2 */
           {
              GLN_DASH,         /* type */
              1.0,              /* width */
              1                 /* colour */
           },
           /* predefined line rep 3 */
           {
              GLN_DOT,          /* type */
              1.0,              /* width */
              1                 /* colour */
           },
           /* predefined line rep 4 */
           {
              GLN_DOTDASH,      /* type */
              1.0,              /* width */
              1                 /* colour */
           },
           /* predefined line rep 5 */
           {
              GLN_SOLID,        /* type */
              5.0,              /* width */
              1                 /* colour */
           }
        };

#else                           /* c1178 */
extern                          /* c1178 */
Glnbundl def_lnbundl[];         /* c1178 */
#endif                          /* c1178 */

Gint gsetlineind(Gint idx);

#endif
