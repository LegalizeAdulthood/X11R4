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
 *  this file contains:
 *      get_font
 *      set_font
 *      get_text_bndl_tbl_idx
 *      set_aspect_flags
 *      load_box
 *      center_text
 *      curve
 *      circle
 *      conic
 *      draw_conic
 *      set_free_fill_rep
 */

#include <xgks.h>

#include "screen_items.h"
#include "color.h"
#include "draw.h"
#include "ws.h"
#include "trans.h"
#include "defs.h"
#include "math.h"

TEXT_BNDL *text_bndl_tbl;


/*
 *  get_font
 *
 *  description:        finds font corresponding to text bundle
 *                      table entry associated with key
 *
 *  parameters:         key (KEY) - key to text bundle table entry
 *                      font (IDX *) - ptr to font (return value)
 *                      precision (Gtxprec*) - ptr to precision
 *                              (return value)
 */

get_font(key,font,precision)
KEY key;
IDX *font;
Gtxprec *precision;
{
        IDX idx;
        Gtxbundl text_bundle;

        idx = get_text_bndl_tbl_idx(key);

        ginqtextrep(ws_id,text_bndl_tbl[idx].idx,GSET,&text_bundle);

        *font = text_bundle.fp.font;
        *precision = text_bundle.fp.prec;
}  /* end get_font */


/*
 *  set_font
 *
 *  description:        sets font corresponding to text bundle
 *                      table entry associated with key
 *
 *  parameters:         key (KEY) - key to text bundle table entry
 *                      font (IDX) - font style index
 *                      precision (Gtxprec) - font precision
 */

set_font(key,font,precision)
KEY key;
IDX font;
Gtxprec precision;
{
        IDX idx;
        Gtxbundl text_bundle;

        idx = get_text_bndl_tbl_idx(key);

        ginqtextrep(ws_id,text_bndl_tbl[idx].idx,GSET,&text_bundle);

        text_bundle.fp.font = font;
        text_bundle.fp.prec = precision;

        gsettextrep(ws_id,text_bndl_tbl[idx].idx,&text_bundle);
}  /* end set_font */


/*
 *  get_text_bndl_tbl_idx
 *
 *  description:        finds index in text_bndl tbl corresonding
 *                      to key
 *
 *  parameters:         key (KEY) - key corresponding to bndl tbl index
 */

get_text_bndl_tbl_idx(key)
KEY key;                                /* key corresponding to index */
{
        IDX idx;                        /* text_bndl_tbl index */

        idx = 0;
        while (!eq(text_bndl_tbl[idx].key,key)) 
        {
                idx++;
        }

        return(idx);
};  /* end get_text_bndl_tbl_idx */


/*
 *  set_aspect_flags
 *
 *  description:  set all aspect flags to same setting
 *
 *  parameters: setting (Gasf) - GINDIVIDUAL | GBUNDLED
 */

/* set_aspect_flags(setting)    Old GKS version 
ENUM setting;
{
        ENUM flags[13];
        int i;

        for (i=0; i<13; i++)
                flags[i] = setting;

        s_aspect_flags(flags);
} */

set_aspect_flags(setting) /* New GKS version. */
Gasf setting;
{
        Gasfs newfs;

        newfs.ln_type = setting;
        newfs.ln_width = setting;
        newfs.ln_color = setting;
        newfs.mk_type = setting;
        newfs.mk_size = setting;
        newfs.mk_color = setting;
        newfs.tx_fp = setting;
        newfs.tx_exp = setting;
        newfs.tx_space = setting;
        newfs.tx_color = setting;
        newfs.fl_inter = setting;
        newfs.fl_style = setting;
        newfs.fl_color = setting;

        gsetasf(&newfs);
} /* end set_aspect_flags */


/*
 *  load_box
 *
 *  description:        loads box array with pts to display rectangle
 *
 *  parameters:         box (Gpoint * or NDCPT *) - rectangle
 *                      xmin,xmax,ymin,ymax (Gpoint) - rectangle
 *                              boundary
 */

load_box(box,xmin,xmax,ymin,ymax)
Gpoint *box;                            /* array of pts to be filled */
Gfloat xmin,xmax,ymin,ymax;             /* could also be NDCPT */
{
        box[0].x = xmin;        box[0].y = ymin;
        box[1].x = xmax;        box[1].y = ymin;
        box[2].x = xmax;        box[2].y = ymax;
        box[3].x = xmin;        box[3].y = ymax;

}  /* end load_box */


/*
 *  center_text
 *
 *  description:        draws multiple lines of text
 *                      centered about a point
 *
 *  parameters: nolines (int) - no of text lines
 *                      lines (char **) - lines of text
 *                      center (Gpoint *) - center pt
 *                      key (KEY) - text bundle table key
 */

center_text(nolines,lines,center,clrno,key)
int nolines;                            /* no of lines to display */
char **lines;                           /* array of lines */
Gpoint *center;                         /* center pt */
IDX clrno;                              /* text color */
KEY key;                                /* text bundle table key */
{
        IDX idx;                        /* text bundle tbl index
                                           containing key */
        Gtxalign align;                 /* text alignment */
        Gpoint up_vector;               /* up direction for text */
        Gpriattr attr;
        Gpoint ln_center;               /* center pt for curr line */
        Gtxbundl text_bundle;           /* bundle tbl entry for idx */
        Gfloat vert_spacing;
        int i;

        /* find index corresponding to key */

        idx = get_text_bndl_tbl_idx(key);

        /* since not all text parameters are bundled,
           the ones that aren't must be saved and restored */

        ginqprimattr(&attr);

        /* set text parameters */

        align.hor = GTH_CENTER;
        align.ver = GTV_HALF;
        gsettextalign(&align);

        up_vector.x = 0.0;
        up_vector.y = 1.0;
        gsetcharup(&up_vector);
        gsettextpath(GTP_RIGHT);
        gsetcharheight(text_bndl_tbl[idx].char_ht);

        /* set text color */

        ginqtextrep(ws_id,text_bndl_tbl[idx].idx,GSET,&text_bundle);

        text_bundle.color = clrno;

        gsettextrep(ws_id,text_bndl_tbl[idx].idx,&text_bundle);

        gsettextind(text_bndl_tbl[idx].idx);

        /* display text */

        vert_spacing = text_bndl_tbl[idx].char_ht * (3.0 / 5.0);
        ln_center.x = center->x;
        ln_center.y = center->y + (nolines - 1) / 2.0 *
                (text_bndl_tbl[idx].char_ht + vert_spacing);

                /*
                printf("ht = %f, expansion = %f, spacing = %f\n",
                        text_bndl_tbl[idx].char_ht,
                        text_bundle.exp,
                        text_bundle.space);
                */


        for (i=0; i<nolines; i++)
        {

                gtext(&ln_center,*lines);
                lines++;
                ln_center.y = ln_center.y - 
                        (text_bndl_tbl[idx].char_ht + vert_spacing);

        };

        /* restore old text parameters */

        
        gsettextalign(&attr.align);

        gsetcharup(&(attr.up));
        gsettextpath(attr.path);
        gsetcharheight(attr.height);
        gsettextind(attr.txindex);

}  /* end center_text */


/*
 *  center_text_vert
 *
 *  description:        draws a single of text vertically
 *                      centered about a point
 *
 *  parameters:         line (char *) - line of text
 *                      center (Gpoint *) - center pt
 *                      clrno (IDX) - text color
 *                      key (KEY) - text bundle table key
 */

center_text_vert(line,center,clrno,key)
char *line;                             /* text line */
Gpoint *center;                         /* center pt */
IDX clrno;                              /* text color */
KEY key;                                /* text bundle table key */
{
        IDX idx;                        /* text bundle tbl index
                                           containing key */
        Gpoint up_vector;               /* up direction for text */
        Gpriattr attr;                  /* current attributes */
        Gtxbundl text_bundle;   /* bundle tbl entry for idx */
        Gtxalign align;

        /* find index corresponding to key */

        idx = get_text_bndl_tbl_idx(key);

        /* since not all text parameters are bundled,
           the ones that aren't must be saved and restored */

        ginqprimattr(&attr);

        /* set text parameters */

        align.ver = GTV_HALF;
        align.hor = GTH_CENTER;
        gsettextalign(&align);

        up_vector.x = 0.0;
        up_vector.y = 1.0;
        gsetcharup(&up_vector);
        gsettextpath(GTP_DOWN);
        gsetcharheight(text_bndl_tbl[idx].char_ht);
        gsettextind(text_bndl_tbl[idx].idx);

        /* set text color */

        ginqtextrep(ws_id,text_bndl_tbl[idx].idx,GSET,&text_bundle);

        text_bundle.color = clrno;

        gsettextrep(ws_id,text_bndl_tbl[idx].idx,&text_bundle);

        /* display text */

        gtext(center,line);

        /* restore old text parameters */

        /* s_text_alignment(attr.text_align_horz,attr.text_align_vert); */
        gsettextalign(&attr.align);

        gsetcharup(&(attr.up));
        gsettextpath(attr.path);
        gsetcharheight(attr.height);
        gsettextind(attr.txindex);

}  /* end center_text_vert */


/*
 *  curve
 *
 *  description:        calculates pts of curve
 *                      based on 4 control points.
 *                      the curve is a bezier curve
 *                      (see pgs. 519-521 of Foley and VanDam)
 *
 *  parameters:         c (Gpoint *) - array of 4 control points
 *                      nopts (int) - no of pts to calculate
 *                      pts (Gpoint *) - list of pts
 */


curve(c,nopts,pts)
Gpoint *c;
int nopts;
Gpoint *pts;
{
        Gfloat t;
        Gfloat incr;
        int i;

        t = 0.0;
        incr = 1.0 / ((Gfloat) (nopts-1));

        /* for (i=0; i<4; i++)
                printf("c[i] = (%.2f, %.2f)\n",c[i].x,c[i].y); */

        for (i=0; i<nopts; i++)
        {
                pts[i].x = (1-t) * (1-t) * (1-t) * c[0].x
                        + 3 * t * (t-1) * (t-1) * c[1].x
                        + 3 * t * t * (1-t) * c[2].x
                        + t * t * t * c[3].x;

                pts[i].y = (1-t) * (1-t) * (1-t) * c[0].y
                        + 3 * t * (t-1) * (t-1) * c[1].y
                        + 3 * t * t * (1-t) * c[2].y
                        + t * t * t * c[3].y;
                
                t += incr;
        }

}  /* end curve */


/*
 *  circle
 *
 *  description:        calculates pts and draws circle
 *                      using fillarea command. No
 *                      attributes are set.
 *
 *  parameters:         center (Gpoint) - center of circle
 *                      rad (Gfloat) - radius of circle
 *                      nopts (int) - no of pts to calculate
 *                      pts (Gpoint *) - list of pts
 */

circle(center,rad,nopts,pts)
Gpoint center;
Gfloat rad;
int nopts;
Gpoint *pts;
{
        conic(center, 2 * rad, 2 * rad, nopts, pts);


}  /* end circle */


/*
 *  conic
 *
 *  description:        calculates pts and draws conic
 *                      using fillarea command. No
 *                      attributes are set.
 *
 *  parameters:         center (Gpoint) - center of conic
 *                      wd (Gfloat) - width of conic
 *                      ht (Gfloat) - height of conic
 *                      nopts (int) - no of pts to calculate
 *                      pts (Gpoint *) - list of pts
 */


conic(center,wd,ht,nopts,pts)
Gpoint center;
Gfloat wd;
Gfloat ht;
int nopts;
Gpoint *pts;
{
        Gfloat incr;
        int i;

        wd /= 2.0;
        ht /= 2.0;
        incr = (2 * PI) / (nopts - 1);

        for (i=0; i<(nopts - 1); i++)
        {
                pts[i].x = center.x + wd * cos(i * incr);
                pts[i].y = center.y + ht * sin(i * incr);
        }
        pts[(nopts - 1)].x = pts[0].x;
        pts[(nopts - 1)].y = pts[0].y;

}  /* end conic */


/*
 *  draw_conic
 *
 *  description:        calculates pts and draws conic
 *                      using fillarea command. No
 *                      attributes are set.
 *
 *  parameters:         center (Gpoint) - center of conic
 *                      wd (Gfloat) - width of conic
 *                      ht (Gfloat) - height of conic
 */


draw_conic(center,wd,ht)
Gpoint center;
Gfloat wd;
Gfloat ht;
{
        Gpoint pts[NO_CONIC_LNS];

        conic(center,wd,ht,NO_CONIC_LNS,pts);
        gfillarea(NO_CONIC_LNS,pts);
}  /* end draw_conic */


/*
 *  set_free_fill_rep
 *
 *  description:        sets fill_area bundle table entry
 *                      index FREE_FILL_AREA_ENTRY to
 *                      color and style
 */

set_free_fill_rep(color,style)
IDX color;                      /* bundle_entry color */
Gflinter style;                 /* GHOLLOW or GSOLID */
{
        Gflbundl repr;

        repr.inter = style;
        repr.style = 1;
        repr.color = color;

        gsetfillrep(ws_id,FREE_FILL_AREA_ENTRY,&repr);
}  /* end set_free_fill_rep */
