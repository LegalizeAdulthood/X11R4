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
 */

#if !defined(POPUP_H)
#define POPUP_H

#include <xgks.h>

#include "screen_items.h"

#define MAX_NUM_POPUPS 9 /* max num of popups to be \
                            displayed at one time */

/*
 *  update states
 */

#define QUICK_UPDATE 0
#define FULL_UPDATE 1
#define FINAL_UPDATE 2

/*
 *  grid states
 */

#define GRID_OFF 0
#define GRID_ON 1
#define SNAP_ON 2

/*
 * verbose states
 */

#define VERBOSE_MODE_OFF 0
#define VERBOSE_MODE_ON 1

/*
 * grid color states
 */

#define WHITE_GRID 0
#define GREY_GRID 1
#define BLACK_GRID 2

extern Gint get_verbose_mode(void);
void display_popup(POPUP *popup, int nlns, char **lines);
void display_popups(POPUP *poplst);
void erase_popups(POPUP *poplst);
void display_number_popup(POPUP *popup, char *text, int state);
void switch_number_popup_state(Gpoint *extent, int *state, Gpoint pt, int mini, int maxi);
void switch_popup_state(int *state, int nostates, int nlns, char **lines, Gpoint extent[]);
void display_header_popup(POPUP *popup, char *header_text, char **text, int nlns);
void switch_header_popup_state(int *state, int nostates, int nlns, char **lines, Gpoint extent[]);
void display_popup_area(void);
void add_to_currpoptbl(POPUP *head_popup);
void rm_from_currpoptbl(POPUP *head_popup);

#endif
