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
 * Define file for :  fillarea predefined bundles
 * 
 */

#ifdef fillarea_c
Gflbundl def_flbundl[PDF_FILL_BNDLS] =
        {
           /* predefined fill rep 1 */
           {
              GHOLLOW,          /* interior style */
              1,                /* style index */
              1                 /* colour index */
           },
           /* predefined fill rep 2 */
           {
              GSOLID,           /* interior style */
              1,                /* style index */
              1                 /* colour index */
           },
           /* predefined fill rep 3 */
           {
              GPATTERN,         /* interior style */
              1,                /* style index */
              1                 /* colour index */
           },
           /* predefined fill rep 4 */
           {
              GHATCH,           /* interior style */
              -1,               /* style index */
              1                 /* colour index */
           },
           /* predefined fill rep 5 */
           {
              GHATCH,           /* interior style */
              -10,              /* style index */
              1                 /* colour index */
           }
        };

PTATTR  def_ptattr =
        {
           { 1.0, 0.0 },   /* pattern width vector */
           { 0.0, 1.0 },   /* pattern height vector */
           { 0.0, 0.0 }    /* pattern reference point */
        };

#else
extern
Gflbundl def_flbundl[];

extern
PTATTR  def_ptattr;
#endif
