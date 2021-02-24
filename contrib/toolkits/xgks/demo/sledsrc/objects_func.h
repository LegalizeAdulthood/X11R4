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
 * object routines  
 */

#include "objects.h"

void draw_comb_ob(COMB_OB *comb_ob);
void erase_comb_ob(COMB_OB *comb_ob);
void draw_object(OBJECT *object);
void erase_object(OBJECT *object);
void draw_line(OBJECT *object);
void erase_line(OBJECT *object);
void draw_poly(OBJECT *object);
void erase_poly(OBJECT *object);
void draw_text(OBJECT *object);
void erase_text(OBJECT *object);
void display_text(OBJECT *object, int draw_erase);

extern int detect_line(OBJECT *object, Gpoint pt);
extern int get_line_extent(OBJECT *object, Gpoint extent[2]);
extern int copy_line(OBJECT *object1, OBJECT *duplicate);
extern void rotate_gen(OBJECT *object, Gpoint pt, Gfloat angle);
extern void read_line(FILE *fd, OBJECT *object, BOOLEAN *error);
extern int write_line(FILE *fd, OBJECT *object);
extern void free_line(OBJECT *);

extern int detect_poly(OBJECT *object, Gpoint pt);
extern int get_poly_extent(OBJECT *object, Gpoint extent[2]);
extern int copy_poly(OBJECT *object1, OBJECT *duplicate);
extern void read_poly(FILE *fd, OBJECT *object, BOOLEAN *error);
extern int write_poly(FILE *fd, OBJECT *object);
extern void free_poly(OBJECT *);

extern int detect_text(OBJECT *object, Gpoint pt);
extern int get_string_extent(OBJECT *object, Gpoint extent[2]);
extern int copy_text(OBJECT *object1, OBJECT *duplicate);
extern void rotate_text(OBJECT *object, Gpoint pt, Gfloat angle);
extern void read_text(FILE *fd, OBJECT *object, BOOLEAN *error);
extern int write_text(FILE *fd, OBJECT *object);
extern void free_text(OBJECT *);

void erase_object(OBJECT *object);
