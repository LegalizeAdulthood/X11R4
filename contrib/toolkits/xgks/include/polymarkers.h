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


#define  DEFMRKRSIZE  1.0       /* c1154 */

#ifdef polymarkers_c
Gmkbundl def_mkbundl[PDF_MARK_BNDLS] =
        {
           /* predefined marker rep 1 */
           {
              GMK_STAR,         /* type */
              6.0,              /* size */
              1                 /* colour */
           },
           /* predefined marker rep 2 */
           {
              GMK_POINT,        /* type */
              6.0,              /* size */
              1                 /* colour */
           },
           /* predefined marker rep 3 */
           {
              GMK_PLUS,         /* type */
              6.0,              /* size */
              1                 /* colour */
           },
           /* predefined marker rep 4 */
           {
              GMK_O,            /* type */
              6.0,              /* size */
              1                 /* colour */
           },
           /* predefined marker rep 5 */
           {
              GMK_X,            /* type */
              6.0,              /* size */
              1                 /* colour */
           }
        };

#else
extern
Gmkbundl def_mkbundl[];
#endif
