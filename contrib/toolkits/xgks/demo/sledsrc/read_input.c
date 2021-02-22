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
 *      read_picture
 *      read_palette
 *      read_object
 *      read_line
 *      read_poly
 *      read_text
 */

#include <xgks.h>

#include <stdio.h>

#include "io.h"
#include "defs.h"
#include "screen_items.h"
#include "color.h"
#include "palette.h"
#include "objects.h"
#include "object_tbl.h"
#include "object_list.h"
#include "conversions.h"

extern char *read_char_str();
extern OBJECT *read_object();
extern char *calloc();


/*
 *  read_picture
 *
 *  description:        read in picture description
 *
 *  parameters:         filename (char *) - name of file to read
 *
 *  returns:            (BOOLEAN) - true if input file successfully
 *                      read in, false otherwise
 */

BOOLEAN
read_picture(filename)
char *filename;
{
        FILE *fd;
        OBJECT *object;
        COMB_OB *comb_ob,*temp;
        int i,j;
        BOOLEAN error;
        char *string;
        Gcobundl *clrs;
        Gint no_clrs;
        COMB_OB *new_head, *new_tail;
        IDX fgd,bgd,fill;

        /* open file for reading */

        fd = fopen(filename,"r");
        if (fd == (FILE *) NULL)
        {
                full_prompt(22,2);
                return(FALSE);
        }

        /* check that file was written out by current version of
           program */

        string = read_char_str(fd,&error);
        if (error == TRUE)
        {
                fclose(fd);
                full_prompt(28,2);
                return(FALSE);
        }
        if (!eq(string,VERSION_NUM))
        {
                fclose(fd);
                full_prompt(43,2);
                return(FALSE);
        }

        /* read in fgd, bgd and fill colors and palette colors */

        read_int(fd,&fgd,&error);
        if (error == TRUE)
        {
                fclose(fd);
                full_prompt(28,2);
                return(FALSE);
        }

        read_int(fd,&bgd,&error);
        if (error == TRUE)
        {
                fclose(fd);
                full_prompt(28,2);
                return(FALSE);
        }

        read_int(fd,&fill,&error);
        if (error == TRUE)
        {
                fclose(fd);
                full_prompt(28,2);
                return(FALSE);
        }

        read_palette_clrs(fd,&clrs,&no_clrs,&error);
        if (error == TRUE)
        {
                fclose(fd);
                full_prompt(28,2);
                return(FALSE);
        }

        /* read in object list */

        read_int(fd,&i,&error);
        if (error == TRUE)
        {
                fclose(fd);
                full_prompt(28,2);
                return(FALSE);
        }

        new_head = (COMB_OB *) NULL;
        new_tail = (COMB_OB *) NULL;
        comb_ob = new_head;
        object = (OBJECT *) NULL;
        while (i > 0)
        {
                if (new_head == (COMB_OB *) NULL)
                {
                        new_head = alloc_comb_ob();
                        new_head->prev = (COMB_OB *) NULL;
                        comb_ob = new_head;
                }
                else
                {
                        comb_ob->next = alloc_comb_ob();
                        comb_ob->next->prev = comb_ob;
                        comb_ob = comb_ob->next;
                }

                read_int(fd,&j,&error);
                if (error == TRUE)
                {
                        fclose(fd);
                        full_prompt(28,2);
                        return(FALSE);
                }

                while (j > 0)
                {
                        if (comb_ob->child == (OBJECT *) NULL)
                        {
                                comb_ob->child = read_object(fd,&error);
                                object = comb_ob->child;
                        }
                        else
                        {
                                object->next = read_object(fd,&error);
                                object = object->next;
                        }
                        object->parent = comb_ob;

                        j--;
                } /* end while */

                i--;
        }  /* end while */

        if (new_head != (COMB_OB *) NULL)
                new_tail = comb_ob;

        /* since file has been successfully read in, get rid of
           free up old linked list of objects */

        comb_ob = head;
        while (comb_ob != (COMB_OB *) NULL)
        {
                temp = comb_ob->next;
                free((char *) comb_ob);
                comb_ob = temp;
        }

        head = new_head;
        tail = new_tail;

        /* update color table, fgd, bgd and fill colors */

        for (i= 0; i < no_clrs; i++)
                set_color((i + palstartclr),&(clrs[i]));
        free((char *) clrs);

        set_fgdclr(fgd);
        set_bgdclr(bgd);
        set_fillclr(fill);

        /* close file */

        fclose(fd);
        prompt(2);
        return(TRUE);
}  /* end read_picture */


/*
 *  read_palette_clrs
 *
 *  description:        allocates color table space for palette
 *                      colors in color table and initializes colors
 *                      in color table as well as initializing fgdclr
 *                      to first entry in color table (top row, 
 *                      leftmost column)
 *
 *  parameters:         fd (FILE *) - file descriptor
 *                      clrs (Gcobundl **) - array to hold list of colors
 *                              (allocated by read_palette_clrs)
 *                      no_clrs (int *) - number of colors in palette
 *                      error (BOOLEAN *) - true if error reading input file
 */

read_palette_clrs(fd,clrs,no_clrs,error)
FILE *fd;
Gcobundl **clrs;
Gint *no_clrs;
BOOLEAN *error;
{
        int i;

        /* read in number of rows and columns */

        read_int(fd,&nrows,error);
        if (*error == TRUE)
                return;

        read_int(fd,&ncols,error);
        if (*error == TRUE)
                return;

        *no_clrs = nrows * ncols;

        /* read in colors and enter into color table */

        *clrs = (Gcobundl *) calloc((unsigned) *no_clrs, sizeof(Gcobundl));

        if (palstartclr == NOT_ALLOCATED)
                palstartclr = alloc_clr_tbl_entries(MAX_PAL_ENTRIES);
        for (i = 0; i < (nrows * ncols); i++)
        {
                read_real3(fd,&((*clrs)[i].red),&((*clrs)[i].green),
                        &((*clrs)[i].blue),error);
                if (*error == TRUE)
                        return;
        }

        *error = FALSE;
}  /* end read_palette_clrs */


/*
 *  read_object
 *
 *  description:        reads in object description
 *
 *  parameters:         fd (FILE *) - file descriptor
 *                      error (BOOLEAN *) - ture if error reading
 *                              input file
 *
 *  returns:            (OBJECT *) - object
 */

OBJECT *
read_object(fd,error)
FILE *fd;
BOOLEAN *error;
{
        OBJECT *object;
        KEY key;
        int i;
        int v;

        key = read_char_str(fd,error);
        if (*error == TRUE)
                return((OBJECT *) NULL);
        object = alloc_single_ob(key);
        free((char *) key);

        /* read_int(fd,&(object->visibility),error); */
        read_int(fd,&v,error);
        object->visibility = num_to_vis(v);

        if (*error == TRUE)
                return((OBJECT *) NULL);
        for (i=0; i<6; i++)
        {
                read_real(fd,&(object->trans[i]),error);
                if (*error == TRUE)
                        return((OBJECT *) NULL);
        }

        i = get_object_func_tbl_idx(object->key);
        (*(object_func_tbl[i].read))(fd,object,error);

        return(object);
}  /* end read_object */


/*
 *  read_line
 *
 *  description:        reads in fields of struct line object
 *
 *  parameters:         fd (FILE *) - file descriptor
 *                      object (OBJECT *) - line object
 *                      error (BOOLEAN *) - ture if error reading
 *                              input file
 */

read_line(fd,object,error)
FILE *fd;
OBJECT *object;
BOOLEAN *error;
{
        int i;

        read_int(fd,&(object->lineob.color),error);
        read_int(fd,&(object->lineob.style),error);
        read_int(fd,&(object->lineob.width),error);
        read_int(fd,&(object->lineob.nopts),error);
        object->lineob.pts = (Gpoint *) calloc((unsigned)
                object->lineob.nopts,sizeof(Gpoint));

        for (i=0; i<object->lineob.nopts; i++)
        {
                read_real2(fd,&(object->lineob.pts[i].x),
                        &(object->lineob.pts[i].y),error);
                if (*error == TRUE)
                        return;
        }
}  /* end read_line */


/*
 *  read_poly
 *
 *  description:        reads in fields of struct poly object
 *
 *  parameters:         fd (FILE *) - file descriptor
 *                      object (OBJECT *) - poly object
 *                      error (BOOLEAN *) - true if error reading
 *                              input file
 */

read_poly(fd,object,error)
FILE *fd;
OBJECT *object;
BOOLEAN *error;
{
        int i;
        int prov;

        read_int(fd,&prov,error);
        if (*error == TRUE)
                return;
        object->polyob.fillstyle = num_to_style(prov);

        read_int(fd,&(object->polyob.fillcolor),error);
        if (*error == TRUE)
                return;
        read_int(fd,&(object->polyob.linecolor),error);
        if (*error == TRUE)
                return;
        read_int(fd,&(object->polyob.style),error);
        if (*error == TRUE)
                return;
        read_int(fd,&(object->polyob.width),error);
        if (*error == TRUE)
                return;
        read_int(fd,&(object->polyob.nopts),error);
        if (*error == TRUE)
                return;
        object->polyob.pts = (Gpoint *) calloc((unsigned)
                object->polyob.nopts,sizeof(Gpoint));

        for (i=0; i<object->polyob.nopts; i++)
        {
                read_real2(fd,&(object->polyob.pts[i].x),
                        &(object->polyob.pts[i].y),error);
                if (*error == TRUE)
                        return;
        }
}  /* end read_poly */


/*
 *  read_text
 *
 *  description:        reads in fields of struct text object
 *
 *  parameters:         fd (FILE *) - file descriptor
 *                      object (OBJECT *) - text object
 *                      error (BOOLEAN *) - ture if error reading
 *                              input file
 */

read_text(fd,object,error)
FILE *fd;
OBJECT *object;
BOOLEAN *error;
{
        CHAR_OB *ch_ptr;
        int i;
        int prov;

        /* read_int(fd,&(object->textob.path),error); */
        read_int(fd,&prov,error);
        if (*error == TRUE)
                return;
        object->textob.path = num_to_path(prov);

        /* read_int(fd,&(object->textob.vert_just),error); */
        read_int(fd,&prov,error);
        if (*error == TRUE)
                return;
        object->textob.vert_just = num_to_vert(prov);

        /* read_int(fd,&(object->textob.horz_just),error); */
        read_int(fd,&prov,error);
        if (*error == TRUE)
                return;
        object->textob.horz_just = num_to_horz(prov);

        read_real2(fd,&(object->textob.pos.x),
                &(object->textob.pos.y),error);
        if (*error == TRUE)
                return;
        read_real2(fd,&(object->textob.up_vec.x),
                &(object->textob.up_vec.y),error);
        if (*error == TRUE)
                return;
        read_int(fd,&i,error);
        if (*error == TRUE)
                return;

        ch_ptr = (CHAR_OB *) NULL;
        while (i > 0)
        {
                if (ch_ptr == (CHAR_OB *) NULL)
                {
                        object->textob.str = (CHAR_OB *) 
                                calloc((unsigned) 1, sizeof(CHAR_OB));
                        ch_ptr = object->textob.str;
                }
                else
                {
                        ch_ptr->next = (CHAR_OB *) calloc((unsigned) 1,
                                sizeof(CHAR_OB));
                        ch_ptr = ch_ptr->next;
                }

                read_real2(fd,&(ch_ptr->center.x),
                        &(ch_ptr->center.y),error);
                if (*error == TRUE)
                        return;
                read_int(fd,&(ch_ptr->color),error);
                if (*error == TRUE)
                        return;
                read_real(fd,&(ch_ptr->height),error);
                if (*error == TRUE)
                        return;
                read_int(fd,&(ch_ptr->fontstyle),error);
                if (*error == TRUE)
                        return;
                read_real(fd,&(ch_ptr->expansion),error);
                if (*error == TRUE)
                        return;
                read_real(fd,&(ch_ptr->width),error);
                if (*error == TRUE)
                        return;
                read_char(fd,&(ch_ptr->ch),error);
                if (*error == TRUE)
                        return;
                ch_ptr->next = (CHAR_OB *) NULL;

                i--;
        }  /* end while */
}  /* end read_text */


