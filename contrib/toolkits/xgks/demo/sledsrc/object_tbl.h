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

#if !defined(OBJECT_TBL_H)
#define OBJECT_TBL_H

#include <xgks.h>

#include "objects.h"
#include "screen_items.h"

struct object_func_tbl_entry
{
    KEY key;                                                 /* object type key */
    void (*draw)(OBJECT *object);                            /* draw routine for object */
    void (*erase)(OBJECT *object);                           /* draw routine for object */
    int (*detect)(OBJECT *object, Gpoint pt);                /* routine to determine if
                                           object was selected */
    int (*get_extent)(OBJECT *object, Gpoint extent[2]);     /* routine which calculates
                                           upright rectangular extent
                                           for object */
    int (*copy)(OBJECT *object, OBJECT *duplicate);          /* routine to one object of
                                           of type key to another */
    void (*rotate)(OBJECT *object, Gpoint pt, Gfloat angle); /* routine to rotate object */
    void (*free)(OBJECT *object);                            /* routine for freeing
                                           object structure
                                           (such as lineob) */
    void (*read)(FILE *fd, OBJECT *object, int *error);      /* routine to read in object
                                           of type key */
    int (*write)(FILE *fd, OBJECT *object);                  /* routine to write out object
                                           of type key */
};

extern int object_func_tbl_sz;
extern struct object_func_tbl_entry object_func_tbl[];
IDX get_object_func_tbl_idx(KEY key);

#endif
