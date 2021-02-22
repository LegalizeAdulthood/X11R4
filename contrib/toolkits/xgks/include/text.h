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

#define   DEFCHRSPACE    0.0

#ifdef text_c
Gtxbundl def_txbundl[PDF_TEXT_BNDLS] =
        {
           /* predefined text rep 1 */
           {
            {
              1,                /* font */
              GSTROKE           /* precision */
            },
              1.0,              /* expansion */
              0.15,             /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 2 */
           {
            {
              2,                /* font */
              GSTROKE           /* precision */
            },
              1.0,              /* expansion */
              0.15,             /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 3 */
           {
            {
              3,                /* font */
              GSTROKE           /* precision */
            },
              1.0,              /* expansion */
              0.2,              /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 4 */
           {
            {
              4,                /* font */
              GSTROKE           /* precision */
            },
              1.0,              /* expansion */
              0.15,             /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 5 */
           {
            {
              5,                /* font */
              GSTROKE           /* precision */
            },
              1.0,              /* expansion */
              0.1,              /* spacing */
              1                 /* colour */
           },
           /* predefined text rep 6 */
           {
            {
              6,                /* font */
              GSTROKE           /* precision */
            },
              1.0,              /* expansion */
              0.1,              /* spacing */
              1                 /* colour */
           }
        };

#else
extern
Gtxbundl def_txbundl[];
#endif

#endif
