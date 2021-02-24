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
 * this file includes
 *      copy_comb_ob
 *      copy_object
 *      copy_line
 *      copy_poly
 *      copy_text
 */

#include <stdlib.h>

#include "objects_copy.h"

#include "objects.h"
#include "object_tbl.h"
#include "screen_items.h"

/*
 *  copy_comb_ob
 *
 *  description:        copies one comb_ob to another
 *                      does not copy prev and next links
 *
 *  parameters:         comb_ob (COMB_OB *) - original comb_ob
 *
 *  returns:            (COMB_OB *) - duplicate comb_ob
 */

COMB_OB *copy_comb_ob( COMB_OB *comb_ob)
{
        COMB_OB *duplicate;
        OBJECT *object_ptr, *dup_ob_ptr;

        duplicate = alloc_comb_ob();
        duplicate->visibility = comb_ob->visibility;

        duplicate->child = (OBJECT *) NULL;
        dup_ob_ptr = comb_ob->child;
        for (object_ptr = comb_ob->child; object_ptr != (OBJECT *) NULL;
                object_ptr = object_ptr->next)
        {
                if (duplicate->child == (OBJECT *) NULL)
                {
                        dup_ob_ptr = copy_object(object_ptr);
                        duplicate->child = dup_ob_ptr;
                }
                else
                {
                        dup_ob_ptr->next = copy_object(object_ptr);
                        dup_ob_ptr = dup_ob_ptr->next;
                }
                dup_ob_ptr->parent = duplicate;
        }
        return(duplicate);
}  /* end copy_comb_ob */


/*
 *  copy_object
 *
 *  description:        copies one object to another
 *
 *  parameters:         object (OBJECT *) - object to copy
 *
 *  returns:            (OBJECT *) - duplicate
 */

OBJECT * copy_object( OBJECT *object)
{
        OBJECT *duplicate;
        int i;

        duplicate = alloc_single_ob(object->key);
        duplicate->visibility = object->visibility;
        
        for (i=0; i<6; i++)
                duplicate->trans[i] = object->trans[i];

        (*(object_func_tbl[get_object_func_tbl_idx(object->key)].copy))(object, duplicate);
        return(duplicate);
}  /* end copy_object */


/*
 *  copy_line
 *
 *  description:        copies struct lineob part of object1
 *                      to duplicate
 *
 *  parameters:         object1 (OBJECT *) - line object to copy
 *                      duplicate (OBJECT *) -  dupilcate
 *                              (must be allocated previously)
 */

void copy_line(OBJECT *object1, OBJECT *duplicate)
{
        int i;

        duplicate->lineob.color = object1->lineob.color;
        duplicate->lineob.style = object1->lineob.style;
        duplicate->lineob.width = object1->lineob.width;
        duplicate->lineob.nopts = object1->lineob.nopts;
        duplicate->lineob.pts = (Gpoint *) calloc((unsigned)
                duplicate->lineob.nopts, sizeof(Gpoint));
        
        for (i=0; i < object1->lineob.nopts; i++)
                duplicate->lineob.pts[i] = object1->lineob.pts[i];
}  /* end copy_line */


/*
 *  copy_poly
 *
 *  description:        copies struct polyob part of object1
 *                      to duplicate
 *
 *  parameters:         object1 (OBJECT *) - poly object to copy
 *                      duplicate (OBJECT *) -  dupilcate
 *                              (must be allocated previously)
 */

void copy_poly(OBJECT *object1, OBJECT *duplicate)
{
        int i;

        duplicate->polyob.linecolor = object1->polyob.linecolor;
        duplicate->polyob.fillcolor = object1->polyob.fillcolor;
        duplicate->polyob.fillstyle = object1->polyob.fillstyle;
        duplicate->polyob.width = object1->polyob.width;
        duplicate->polyob.style = object1->polyob.style;
        duplicate->polyob.nopts = object1->polyob.nopts;
        duplicate->polyob.pts = (Gpoint *) calloc((unsigned)
                duplicate->polyob.nopts, sizeof(Gpoint));
        
        for (i=0; i < object1->polyob.nopts; i++)
                duplicate->polyob.pts[i] = object1->polyob.pts[i];
}  /* end copy_poly */


/*
 *  copy_text
 *
 *  description:        copies struct textob part of object1
 *                      to duplicate
 *
 *  parameters:         object1 (OBJECT *) - text object to copy
 *                      duplicate (OBJECT *) -  dupilcate
 *                              (must be allocated previously)
 */

void copy_text(OBJECT *object1, OBJECT *duplicate)
{
        CHAR_OB *ch_ptr1, *ch_ptr2;

        duplicate->textob.path = object1->textob.path;
        duplicate->textob.vert_just = object1->textob.vert_just;
        duplicate->textob.horz_just = object1->textob.horz_just;
        duplicate->textob.pos = object1->textob.pos;
        duplicate->textob.up_vec = object1->textob.up_vec;

        ch_ptr2 = (CHAR_OB *) NULL;
        for (ch_ptr1 = object1->textob.str; 
                ch_ptr1 != (CHAR_OB *) NULL;
                ch_ptr1 = ch_ptr1->next)
        {
                if (ch_ptr2 == (CHAR_OB *) NULL)
                {
                        duplicate->textob.str = ch_ptr2 = (CHAR_OB *)
                                calloc ((unsigned) 1, sizeof(CHAR_OB));
                }
                else
                {
                        ch_ptr2->next = (CHAR_OB *) calloc((unsigned)
                                1, sizeof(CHAR_OB));
                        ch_ptr2 = ch_ptr2->next;
                }

                ch_ptr2->center = ch_ptr1->center;
                ch_ptr2->color = ch_ptr1->color;
                ch_ptr2->height = ch_ptr1->height;
                ch_ptr2->expansion = ch_ptr1->expansion;
                ch_ptr2->width = ch_ptr1->width;
                ch_ptr2->fontstyle = ch_ptr1->fontstyle;
                ch_ptr2->ch = ch_ptr1->ch;
        }

        ch_ptr2->next = (CHAR_OB *) NULL;
}  /* end copy_text */
