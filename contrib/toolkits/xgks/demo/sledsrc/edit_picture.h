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

extern int edit_picture_init();

/* menu item functions */

extern int edit_picture_undo();
extern int erase();

extern int rotate_init();
extern int rotate_restart();
extern int rotate_exec();
extern int rotate_cleanup();

extern int move_init();
extern int move_restart();
extern int move_exec();
extern int move_cleanup();

extern int copy_init();
extern int copy_restart();
extern int copy_exec();
extern int copy_cleanup();

extern int delete_init();
extern int delete_restart();
extern int delete_exec();
extern int delete_cleanup();

extern int combine_init();
extern int combine_restart();
extern int combine_exec();
extern int combine_cleanup();

extern int split_init();
extern int split_restart();
extern int split_exec();
extern int split_cleanup();

/* popup functions */

extern int init_angle_popup();
extern int switch_angle_state();
