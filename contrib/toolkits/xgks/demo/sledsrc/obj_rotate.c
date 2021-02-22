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
 *      rotate_comb_ob
 *      rotate_object
 *      rotate_gen
 *      rotate_text
 */

#include <xgks.h>

#include "objects.h"
#include "object_tbl.h"
#include "screen_items.h"


/*
 *  rotate_comb_ob
 *
 *  description:        rotates comb-ob about given pt
 *
 *  parameters:         comb_ob (COMB_OB *) - original comb_ob
 *                      pt (Gpoint) - pt of rotation
 *                      angle (Gfloat) - angle of rotation
 *
 */

rotate_comb_ob(comb_ob,pt,angle)
COMB_OB *comb_ob;
Gpoint pt;
Gfloat angle;
{
        OBJECT *object;

        for (object = comb_ob->child; object != (OBJECT *) NULL;
                object = object->next)
                
                rotate_object(object,pt,angle);
}  /* end rotate_comb_ob */


/*
 *  rotate_object
 *
 *  description:        rotates single object
 *
 *  parameters:         object (OBJECT *) - object to rotate
 *                      pt (Gpoint) - pt to rotate object about
 *                      angle (Gfloat) - angle of rotation
 */

rotate_object(object,pt,angle)
OBJECT *object;
Gpoint pt;
Gfloat angle;
{
        (*(object_func_tbl[get_object_func_tbl_idx(object->key)].rotate))
                (object,pt,angle);
}  /* end rotate_object */


/*
 *  rotate_gen
 *
 *  description:        rotates object other than text object
 *
 *  parameters:         object (OBJECT *) - object to rotate
 *                      pt (Gpoint) - pt to rotate object about
 *                      angle (Gfloat) - angle of rotation
 */

rotate_gen(object,pt,angle)
OBJECT *object;
Gpoint pt;
Gfloat angle;
{
                Gfloat trans[6];

                s_rot_mat(trans,pt,angle);
                mat_mult(object->trans,trans,object->trans);
}  /* end rotate_gen */


/*
 *  rotate_text
 *
 *  description:        rotates textob
 *
 *  parameters:         object (OBJECT *) - text object to rotate
 *                      pt (Gpoint) - pt to roate text about
 *                      angle (Gfloat) - angle of rotation
 */

rotate_text(object,pt,angle)
OBJECT *object;
Gpoint pt;
Gfloat angle;
{
                Gfloat trans[6];
                Gpoint orig;

                /* rotate view up vector */

                orig.x = 0.0;
                orig.y = 0.0;
                s_rot_mat(trans,orig,angle);
                pt_mult(&(object->textob.up_vec),1,trans);

                s_rot_mat(trans,pt,angle);
                mat_mult(object->trans,trans,object->trans);
}  /* end rotate_text */
