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
 * object routines  
 */

extern int draw_line();
extern int erase_line();
extern int detect_line();
extern int get_line_extent();
extern int copy_line();
extern int rotate_gen();
extern int read_line();
extern int write_line();
extern int free_line();

extern int draw_poly();
extern int erase_poly();
extern int detect_poly();
extern int get_poly_extent();
extern int copy_poly();
extern int read_poly();
extern int write_poly();
extern int free_poly();

extern int draw_text();
extern int erase_text();
extern int detect_text();
extern int get_string_extent();
extern int copy_text();
extern int rotate_text();
extern int read_text();
extern int write_text();
extern int free_text();

