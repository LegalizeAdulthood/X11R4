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

#include <xgks.h>

#define	MAX_NUM_POPUPS		9	/* max num of popups to be
					   displayed at one time */


/*
 *  update states
 */

#define	QUICK_UPDATE		0
#define	FULL_UPDATE		1
#define	FINAL_UPDATE		2

/*
 *  grid states
 */

#define	GRID_OFF		0
#define	GRID_ON			1
#define	SNAP_ON			2

/*
 * verbose states
 */

#define	VERBOSE_MODE_OFF	0
#define	VERBOSE_MODE_ON		1


extern Gint get_verbose_mode();

/*
 * grid color states
 */

#define	WHITE_GRID		0
#define	GREY_GRID		1
#define	BLACK_GRID		2
