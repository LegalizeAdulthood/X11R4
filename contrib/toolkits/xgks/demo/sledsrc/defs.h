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
 */

#include <strings.h>
#include <xgks.h>

#define sqr(a)		((a) * (a))
#define abs(a)		((a) < 0 ? (-(a)) : (a))
#define max2(a,b)	((a) < (b) ? (b) : (a))
#define min2(a,b)	((a) > (b) ? (b) : (a))
#define eq(a,b)		((strlen(a) == strlen(b)) && 		\
			 (strncmp((a),(b),			\
			 min2(strlen(a),strlen(b))) == 0))
#define max3(a,b,c)	(max2(max2((a),(b)),(c)))
#define min3(a,b,c)	(min2(min2((a),(b)),(c)))

#define dist(a,b)	(sqrt(sqr((a.x) - (b.x)) + sqr((a.y) - (b.y))))

#define eq_pts(a,b)	(((a.x) == (b.x)) && ((a.y) == (b.y)))

#define DELTA		.005

#define LINE_FAC	4.0
