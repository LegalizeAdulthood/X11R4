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
 *      detect_object
 *      detect_single_line
 *      detect_line
 *      detect_poly
 *      detect_text
 *      get_text_extent
 *      get_extent
 *      get_line_extent
 *      get_poly_extent
 *      get_string_extent
 *      is_object_inbounds
 */

#include <math.h>

#include "objects.h"
#include "key.h"
#include "ws.h"
#include "object_tbl.h"
#include "defs.h"
#include "trans.h"


detect_object(hit,comb_ob,object,pt)
BOOLEAN *hit;
COMB_OB **comb_ob;
OBJECT **object;
Gpoint pt;
{
        IDX idx;
        COMB_OB *comb_ob_ptr;
        OBJECT *object_ptr;

        *object = (OBJECT *) NULL;
        *comb_ob = (COMB_OB *) NULL;
        *hit = FALSE;

        /* search priority list from tail so that
           first object picked has highest display priority */

        for (comb_ob_ptr = tail; ((comb_ob_ptr != (COMB_OB *) NULL)
                && (*hit == FALSE)); comb_ob_ptr = comb_ob_ptr->prev)
        {
                if (comb_ob_ptr->visibility == GVISIBLE)
                {
                        /* last object in comb_ob has highest display
                        priority */

                        for (object_ptr = comb_ob_ptr->child;
                                object_ptr != (OBJECT *) NULL;
                                object_ptr = object_ptr->next)
                        {
                                if (object_ptr->visibility == GVISIBLE)
                                {
                                        idx = get_object_func_tbl_idx(object_ptr->key);
                                        if (object_func_tbl[idx].detect != NULL)
                                        {
                                                if ((*(object_func_tbl[idx].detect))
                                                        (object_ptr,pt) == TRUE)
                                                {
                                                        *object = object_ptr;
                                                        *comb_ob = comb_ob_ptr;
                                                        *hit = TRUE;
                                                }
                                        }
                                }
                        }
                }
        }

}  /* end detect_object */


/*
 *  detect_single_line
 *
 *  description:        test if line was detected
 *
 *  parameters:         line (Gpoint *) - endpoints of line
 *                      pt (Gpoint) - pt picked
 *
 *  returns:            (BOOLEAN) - TRUE if detected
 */

BOOLEAN
detect_single_line(line,pt)
Gpoint *line;
Gpoint pt;
{
        Gpoint extent[2];
        Gfloat distance;

        /* quick reject test */

        get_extent(line,2,extent);
        if (!in_bounds_delta(extent,pt,DELTA))
                return(FALSE);
        
        /* test distance from pt to line */

        if (eq_pts(line[0],line[1]))
        {
                if (dist(pt,line[0]) <= DELTA)
                        return(TRUE);
                else
                        return(FALSE);
        }

        distance = ((pt.x - line[0].x)
                * (line[1].y - line[0].y)
                - (line[1].x - line[0].x) 
                * (pt.y - line[0].y))
                / dist(line[0],line[1]);
        distance = abs(distance);

        if (distance <= DELTA)
                return(TRUE);
        else
                return(FALSE);
}  /* end detect_single_line */


/*
 *  detect_line
 *
 *  description:        tests if line object was detected
 *
 *  parameters:         object (OBJECT *) - line object
 *                      pt (Gpoint) - pt picked
 *
 *  returns:            (BOOLEAN) - true if detected
 */

BOOLEAN
detect_line(object,pt)
OBJECT *object;
Gpoint pt;
{
        BOOLEAN matched;
        Gpoint extent[2];
        int i;

        get_extent(object->lineob.pts,object->lineob.nopts,extent);
        if (!in_bounds_delta(extent,pt,DELTA))
        {
                return(FALSE);
        }
        else
        {
                matched = FALSE;
                for (i=0; i < (object->lineob.nopts - 1); i++)
                {
                        if (detect_single_line(&(object->lineob.pts[i]),
                                pt))
                        {
                                matched = TRUE;
                                break;
                        }
                }

        }
        return(matched);
}  /* end detect_line */


/*
 *  detect_poly
 *
 *  description:        tests if polygon has been detected
 *
 *  parameters:         object (OBJECT *) - line object
 *                      pt (Gpoint) - pt picked
 *
 *  returns:            (BOOLEAN) - true if detected
 */

BOOLEAN
detect_poly(object,pt)
OBJECT *object;
Gpoint pt;
{
        Gpoint extent[2];

        get_extent(object->polyob.pts,object->polyob.nopts,extent);
        if (in_bounds_delta(extent,pt,DELTA))
                return(TRUE);
        else
                return(FALSE);
}  /* end detect poly */


/*
 *  detect_text
 *
 *  description:        tests if text object was detected
 *
 *  parameters:         object (OBJECT *) - line object
 *                      pt (Gpoint) - pt picked
 *
 *  returns:            (BOOLEAN) - true if detected
 */

BOOLEAN
detect_text(object,pt)
OBJECT *object;
Gpoint pt;
{
        BOOLEAN hit;
        Gpoint rot_extent[4];
        CHAR_OB *ch_ptr;
        Gpoint extent[2];

        hit = FALSE;
        for (ch_ptr = object->textob.str;
                ((ch_ptr != (CHAR_OB *) NULL) && !hit);
                ch_ptr = ch_ptr->next)
        {
                /* find text extent */

                get_text_extent(ch_ptr->ch,ch_ptr->center,
                        ch_ptr->fontstyle,ch_ptr->height,
                        ch_ptr->expansion,object->textob.up_vec,
                        rot_extent);
                get_extent(rot_extent,4,extent);
                if (in_bounds_delta(extent,pt,DELTA))
                        hit = TRUE;
        }

        return(hit);

}  /* end detect_text */


/*
 *  get_text_extent
 *
 *  description:        calculates rotated text extent
 *                      for character
 *
 *  parameters:         ch (char) - text character
 *                      center (Gpoint) - char center
 *                      font (IDX) - char font
 *                      height (Gfloat) - char ehight
 *                      expansion (Gfloat) - char expansion
 *                      up_vec (Gpoint) - char up vector
 *                      extent (Gpoint *) - char extent (return val)
 *                      (storage must be preallocated)
 */

get_text_extent(ch,center,font,height,expansion,up_vec,extent)
Gchar ch;
Gpoint center;
IDX font;
Gfloat height, expansion;
Gpoint up_vec;
Gpoint *extent;
{
        Gpriattr prim_attr;
        Gindattr indiv_attr;
        Gextent ext;
        Gchar str[2];
        Gtxfp tpf;
        Gtxalign align;

        push_curr_trans();
        activate(PICT_AREA);
        ginqprimattr(&prim_attr);
        ginqindivattr(&indiv_attr);

        gsetcharheight(height);
        gsetcharexpan(expansion);
        gsetcharup(&up_vec);

        align.hor = GTH_CENTER;
        align.ver = GTV_HALF;
        gsettextalign(&align);

        tpf.font = font;
        tpf.prec = GSTROKE;
        gsettextfontprec(&tpf);

        str[0] = ch;
        str[1] = (char) NULL;

        ginqtextextent(ws_id,center,str,&ext);
        extent[0] = ext.ll;
        extent[1] = ext.lr;
        extent[2] = ext.ur;
        extent[3] = ext.ul;

        gsetcharheight(prim_attr.height);
        gsetcharexpan(indiv_attr.chexp);
        gsetcharup(&(prim_attr.up));
        gsettextalign(&(prim_attr.align));

        gsettextfontprec(&(indiv_attr.fp));

        pop_curr_trans();
}  /* end get_text_extent */


get_extent(pts,npts,extent)
Gpoint *pts;
int npts;
Gpoint *extent;
{
        int i;

        extent[MIN].x = pts[0].x;
        extent[MIN].y = pts[0].y;
        extent[MAX].x = pts[0].x;
        extent[MAX].y = pts[0].y;

        for (i=1; i<npts; i++)
        {
                if (pts[i].x < extent[MIN].x)
                        extent[MIN].x = pts[i].x;
                else if (pts[i].x > extent[MAX].x)
                        extent[MAX].x = pts[i].x;

                if (pts[i].y < extent[MIN].y)
                        extent[MIN].y = pts[i].y;
                else if (pts[i].y > extent[MAX].y)
                        extent[MAX].y = pts[i].y;
        }

}  /* end get_extent */


/*
 *  get_line_extent
 *
 *  description:        calculates upright rectangular extent for line
 *                      object
 *
 *  parameters:         object (OBJECT *) - line object
 *                      extent (Gpoint *) - array of two points
 *                      to hold min's and max's
 *                      (must be preallocated)
 */

get_line_extent(object,extent)
OBJECT *object;
Gpoint *extent;
{
        get_extent(object->lineob.pts,object->lineob.nopts,extent);
}  /* get_line_object */


/*
 *  get_poly_extent
 *
 *  description:        calculates upright rectangular extent for poly
 *                      object
 *
 *  parameters:         object (OBJECT *) - poly object
 *                      extent (Gpoint *) - array of two points
 *                      to hold min's and max's
 *                      (must be preallocated)
 */

get_poly_extent(object,extent)
OBJECT *object;
Gpoint *extent;
{
        get_extent(object->polyob.pts,object->polyob.nopts,extent);
}  /* get_poly_object */


/*
 *  get_string_extent
 *
 *  description:        calculates upright rectangular extent for text
 *                      object
 *
 *  parameters:         object (OBJECT *) - text object
 *                      extent (Gpoint *) - array of two points
 *                      to hold min's and max's
 *                      (must be preallocated)
 */

get_string_extent(object,extent)
OBJECT *object;
Gpoint *extent;
{
        Gpoint pts[6];
        CHAR_OB *ch_ptr;
        Gpoint up_vec;

        up_vec.x = 0.0;
        up_vec.y = 1.0;

        for (ch_ptr = object->textob.str; ch_ptr != (CHAR_OB *) NULL;
                ch_ptr = ch_ptr->next)
        {
                /* find rotated character extent */

                get_text_extent(ch_ptr->ch,ch_ptr->center,
                        ch_ptr->fontstyle,ch_ptr->height,
                        ch_ptr->expansion,up_vec,pts);

                if (ch_ptr == object->textob.str)
                {
                        get_extent(pts,4,extent);
                }
                else
                {
                        extent[MIN] = pts[4];
                        extent[MAX] = pts[5];
                        get_extent(pts,6,extent);
                }
        }
}  /* end get_string_extent */


/*
 * is_object_inbounds
 *
 *  description:        tests whether object lies entirely
 *                      within extent
 *
 *  parameters:         object (OBJECT *) - object to test
 *                      extent (Gpoint *) - extent to test against
 *
 *  returns:            (BOOLEAN) - true if object is inbounds, false
 *                              otherwise
 */

BOOLEAN
is_object_inbounds(object,extent)
OBJECT *object;
Gpoint *extent;
{
        Gpoint ob_extent[2];

        (*(object_func_tbl[get_object_func_tbl_idx(object->key)].get_extent))
                (object,ob_extent);

        if ((extent[MIN].x < ob_extent[MIN].x) &&
                (extent[MIN].y < ob_extent[MIN].y) &&
                (extent[MAX].x > ob_extent[MAX].x) &&
                (extent[MAX].y > ob_extent[MAX].y))
        {
                return(TRUE);
        }
        else    return(FALSE);
}  /* is_object_inbounds */

