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
 *  this file includes:
 *      free_comb_ob
 *      free_object
 *      free_line
 *      free_poly
 *      free_text
 */

#include "objects.h"
#include "object_tbl.h"


/*
 *  free_comb_ob
 *
 *  description:        frees storage associated with comb_ob
 *
 *  parameters:         comb_ob (COMB_OB *) - combination object
 */

free_comb_ob(comb_ob)
COMB_OB *comb_ob;
{
        OBJECT *object_ptr,*temp;

        object_ptr = (OBJECT *) comb_ob->child;
                
        while (object_ptr != (OBJECT *) NULL)
        {
                temp = object_ptr->next;
                free_object(object_ptr);
                object_ptr = temp;
        }
        free((char *) comb_ob);
}  /* end free_comb_ob */


/*
 *  free_object
 *
 *  description:        frees storage associated with object
 *
 *  parameters:         (OBJECT *) - object
 */

free_object(object)
OBJECT *object;
{
        (*(object_func_tbl[get_object_func_tbl_idx(object->key)].free))
                (object);
        free((char *) object->key);
        free((char *) object);
}  /* end free_object */


/*
 *  free_line
 *
 *  description:        frees all pointers in line object
 *
 *  parameters:         none
 */

free_line(object)
OBJECT *object;
{
        free((char *) object->lineob.pts);
}  /* end free_line */


/*
 *  free_poly
 *
 *  description:        frees all pointers in poly object
 *
 *  parameters:         none
 */

free_poly(object)
OBJECT *object;
{
        free((char *) object->polyob.pts);
}  /* end free_poly */


/*
 *  free_text
 *
 *  description:        frees all pointers in text object
 *
 *  parameters:         none
 */

free_text(object)
OBJECT *object;
{
        CHAR_OB *ch_ptr,*temp;

        ch_ptr = object->textob.str; 
        while (ch_ptr != (CHAR_OB *) NULL)
        {
                temp = ch_ptr->next;
                free((char *) ch_ptr);
                ch_ptr = temp;
        }
}  /* end free_text */
