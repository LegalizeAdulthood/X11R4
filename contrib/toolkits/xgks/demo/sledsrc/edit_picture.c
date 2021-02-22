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
 *      edit_picture_init
 *      move_init
 *      move_restart
 *      move_exec
 *      move_cleanup
 *      copy_init
 *      copy_restart
 *      copy_exec
 *      copy_cleanup
 *      delete_init
 *      delete_restart
 *      delete_exec
 *      delete_cleanup
 *      erase
 *      rotate_init
 *      rotate_restart
 *      rotate_exec
 *      rotate_cleanup
 *      combine_init
 *      combine_restart
 *      combine_exec
 *      combine_cleanup
 *      split_init
 *      split_restart
 *      split_exec
 *      split_cleanup
 *      enq_comb_ob_list
 *      edit_objects_undo
 *      free_lists
 *      init_angle_popup
 *      switch_angle_state
 *      get_rot_angle
 */

#include <xgks.h>

#include "draw.h"
#include "objects.h"
#include "popup.h"
#include "trans.h"
#include "functbl.h"
#include "key.h"
#include "defs.h"
#include "screen_items.h"

#define EDIT_PICTURE_UNDO       "edit picture undo menu item"

extern Gfloat get_rot_angle();

static COMB_OB_PTR *undo_list = (COMB_OB_PTR *) NULL;
static COMB_OB_PTR *new_list = (COMB_OB_PTR *) NULL;
static MENU_ITEM *mitem_in_progress;
static BOOLEAN object_picked;
static COMB_OB *new_comb_ob;
static COMB_OB *prev_new_comb_ob;
static BOOLEAN first_pt_picked;


/*
 *  edit_picture_init
 *
 *  description:        intializes menu, popups and undo variables
 *
 *  parameters:         menu_item (MENU_ITEM *) - edit picture menu item
 *
 */

edit_picture_init(menu_item)
MENU_ITEM *menu_item;
{
        if (no_objects() == TRUE)
                reprompt(20);
        else
        {
                display_child_menu(menu_item);
                free_lists();
                mitem_in_progress = (MENU_ITEM *) NULL;
        }
}  /* end edit_picture_init */


/*
 *  move_init
 *
 *  description:        initializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - move menu item
 */

move_init(menu_item)
MENU_ITEM *menu_item;
{
        set_currmitem(menu_item);
        hilite(menu_item->key);
        push_curr_trans();
        activate(PICT_AREA);
        object_picked = FALSE;
        prompt(18);
}  /* end move_init */  


/*
 *  move_restart
 *
 *  description:        reintializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - move menu item
 */

move_restart(menu_item)
MENU_ITEM *menu_item;
{
        if (eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                set_currmitem(menu_item);
        }
        else
        {
                object_picked = FALSE;
                prompt(18);
        }
}  /* end move_restart */       


/*
 *  move_exec
 *
 *  description:        moves selected comb_ob
 *
 *  parameters:         pt (Gpoint) - pt selected
 */

move_exec(pt)
Gpoint pt;
{
        BOOLEAN hit;
        static COMB_OB *comb_ob;
        static COMB_OB *dup;
        static Gpoint first_pt;
        static Gfloat trans[6];
        OBJECT *object;

        if (object_picked == FALSE)
        {
                detect_object(&hit,&comb_ob,&object,pt);
                if (hit)
                {
                        object_picked = TRUE;
                        first_pt = pt;
                        prompt(19);
                }
                else
                        reprompt(15);
        }
        else  /* object_picked */
        {
                free_lists();
                dup = copy_comb_ob(comb_ob);
                dup->visibility = GVISIBLE;
                enq_comb_ob_list(dup,&new_list);
                comb_ob->visibility = GINVISIBLE;
                enq_comb_ob_list(comb_ob,&undo_list);
                s_trans_mat(trans,(pt.x - first_pt.x),
                        (pt.y - first_pt.y));
                update_trans(dup,trans);
                enq_comb_ob(dup);
                erase_comb_ob(comb_ob);
                if (get_update_mode() != QUICK_UPDATE)
                        redraw_objects();
                else
                        draw_comb_ob(dup);
                comb_ob = dup;
                first_pt = pt;
                prompt(19);
        }
}  /* end move_exec */


/*
 *  move_cleanup
 *
 *  description:        cleans up move menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - move menu item
 */

move_cleanup(menu_item)
MENU_ITEM *menu_item;
{
        if (!eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                pop_curr_trans();
                unhilite(menu_item->key);
                mitem_in_progress = (MENU_ITEM *) NULL;
        }
        else
        {
                mitem_in_progress = menu_item;
        }
        set_currmitem((MENU_ITEM *) NULL);
}  /* end move_cleanup */


/*
 *  copy_init
 *
 *  description:        initializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - copy menu item
 */

copy_init(menu_item)
MENU_ITEM *menu_item;
{
        set_currmitem(menu_item);
        hilite(menu_item->key);
        push_curr_trans();
        activate(PICT_AREA);
        object_picked = FALSE;
        prompt(16);
}  /* end copy_init */  


/*
 *  copy_restart
 *
 *  description:        reintializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - copy menu item
 */

copy_restart(menu_item)
MENU_ITEM *menu_item;
{
        if (eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                set_currmitem(menu_item);
        }
        else
        {
                object_picked = FALSE;
                prompt(16);
        }
}  /* end copy_restart */       


/*
 *  copy_exec
 *
 *  description:        copys selected comb_ob
 *
 *  parameters:         pt (Gpoint) - pt selected
 */

copy_exec(pt)
Gpoint pt;
{
        BOOLEAN hit;
        static COMB_OB *comb_ob;
        static COMB_OB *dup;
        static Gpoint first_pt;
        static Gfloat trans[6];
        OBJECT *object;

        if (object_picked == FALSE)
        {
                detect_object(&hit,&comb_ob,&object,pt);
                if (hit)
                {
                        object_picked = TRUE;
                        first_pt = pt;
                        prompt(17);
                }
                else
                        reprompt(15);
        }
        else  /* object_picked */
        {
                free_lists();
                dup = copy_comb_ob(comb_ob);
                dup->visibility = GVISIBLE;
                enq_comb_ob_list(dup,&new_list);
                s_trans_mat(trans,(pt.x - first_pt.x),
                        (pt.y - first_pt.y));
                update_trans(dup,trans);
                enq_comb_ob(dup);
                draw_comb_ob(dup);
                prompt(17);
        }
}  /* end copy_exec */


/*
 *  copy_cleanup
 *
 *  description:        cleans up copy menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - copy menu item
 */

copy_cleanup(menu_item)
MENU_ITEM *menu_item;
{
        if (!eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                pop_curr_trans();
                unhilite(menu_item->key);
                mitem_in_progress = (MENU_ITEM *) NULL;
        }
        else
        {
                mitem_in_progress = menu_item;
        }
        set_currmitem((MENU_ITEM *) NULL);
}  /* end copy_cleanup */


/*
 *  delete_init
 *
 *  description:        initializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - delete menu item
 */

delete_init(menu_item)
MENU_ITEM *menu_item;
{
        set_currmitem(menu_item);
        hilite(menu_item->key);
        push_curr_trans();
        activate(PICT_AREA);
        prompt(14);
}  /* end delete_init */        


/*
 *  delete_restart
 *
 *  description:        reintializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - delete menu item
 */

delete_restart(menu_item)
MENU_ITEM *menu_item;
{
        if (eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                set_currmitem(menu_item);
        }
        prompt(14);
}  /* end delete_restart */     


/*
 *  delete_exec
 *
 *  description:        deletes selected comb_ob, if any
 *
 *  parameters:         pt (Gpoint) - pt selected
 */

delete_exec(pt)
Gpoint pt;
{
        BOOLEAN hit;
        COMB_OB *comb_ob;
        OBJECT *object;

        detect_object(&hit,&comb_ob,&object,pt);
        if (hit)
        {
                free_lists();
                comb_ob->visibility = GINVISIBLE;
                enq_comb_ob_list(comb_ob,&undo_list);
                erase_comb_ob(comb_ob);
                if (get_update_mode() != QUICK_UPDATE)
                        redraw_objects();
        }
        else
                reprompt(15);
}  /* end delete_exec */


/*
 *  delete_cleanup
 *
 *  description:        cleans up delete menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - delete menu item
 */

delete_cleanup(menu_item)
MENU_ITEM *menu_item;
{
        if (!eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                pop_curr_trans();
                unhilite(menu_item->key);
                mitem_in_progress = (MENU_ITEM *) NULL;
        }
        else
        {
                mitem_in_progress = menu_item;
        }
        set_currmitem((MENU_ITEM *) NULL);
}  /* end delete_cleanup */


/*
 *  erase
 *
 *  description:        delete all objects on screen
 *
 *  parameters:         menu_item (MENU_ITEM *) - erase menu item
 *
 */

erase(menu_item)
MENU_ITEM *menu_item;
{
        COMB_OB *comb_ob;

        hilite(menu_item->key);
        free_lists();
        for (comb_ob = head; comb_ob != (COMB_OB *) NULL;
                comb_ob = comb_ob->next)
        {
                comb_ob->visibility = GINVISIBLE;
                enq_comb_ob_list(comb_ob,&undo_list);
        }

        erase_area(PICT_AREA);
        (*(screen_tbl[find_screen_tbl_idx(PICT_AREA)].draw))();
        unhilite(menu_item->key);
}  /* end erase */


/*
 *  rotate_init
 *
 *  description:        initializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - rotate menu item
 */

rotate_init(menu_item)
MENU_ITEM *menu_item;
{
        set_currmitem(menu_item);
        hilite(menu_item->key);
        display_popups(menu_item->poplst);
        push_curr_trans();
        activate(PICT_AREA);
        prompt(34);
}  /* end rotate_init */        


/*
 *  rotate_restart
 *
 *  description:        reintializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - rotate menu item
 */

rotate_restart(menu_item)
MENU_ITEM *menu_item;
{
        if (eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                set_currmitem(menu_item);
        }
        else
        {
                prompt(34);
        }
}  /* end rotate_restart */     


/*
 *  rotate_exec
 *
 *  description:        rotates selected comb_ob
 *
 *  parameters:         pt (Gpoint) - pt selected
 */

rotate_exec(pt)
Gpoint pt;
{
        BOOLEAN hit;
        COMB_OB *comb_ob;
        COMB_OB *dup;
        OBJECT *object;

        detect_object(&hit,&comb_ob,&object,pt);
        if (hit)
        {
                free_lists();
                dup = copy_comb_ob(comb_ob);
                dup->visibility = GVISIBLE;
                enq_comb_ob_list(dup,&new_list);
                comb_ob->visibility = GINVISIBLE;
                enq_comb_ob_list(comb_ob,&undo_list);
                rotate_comb_ob(dup,pt,get_rot_angle());
                enq_comb_ob(dup);
                erase_comb_ob(comb_ob);
                if (get_update_mode() != QUICK_UPDATE)
                        redraw_objects();
                else
                        draw_comb_ob(dup);
                prompt(34);
        }
        else
                reprompt(15);
}  /* end rotate_exec */


/*
 *  rotate_cleanup
 *
 *  description:        cleans up rotate menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - rotate menu item
 */

rotate_cleanup(menu_item)
MENU_ITEM *menu_item;
{
        if (!eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                erase_popups(menu_item->poplst);
                pop_curr_trans();
                unhilite(menu_item->key);
                mitem_in_progress = (MENU_ITEM *) NULL;
        }
        else
        {
                mitem_in_progress = menu_item;
        }
        set_currmitem((MENU_ITEM *) NULL);
}  /* end rotate_cleanup */


/*
 *  combine_init
 *
 *  description:        initializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - combine menu item
 */

combine_init(menu_item)
MENU_ITEM *menu_item;
{
        set_currmitem(menu_item);
        hilite(menu_item->key);
        push_curr_trans();
        activate(PICT_AREA);
        first_pt_picked = FALSE;
        new_comb_ob = (COMB_OB *) NULL;
        prev_new_comb_ob = (COMB_OB *) NULL;
        prompt(24);
}  /* end combine_init */       


/*
 *  combine_restart
 *
 *  description:        reintializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - combine menu item
 */

combine_restart(menu_item)
MENU_ITEM *menu_item;
{
        COMB_OB *temp_comb_ob;

        if (eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                set_currmitem(menu_item);
                temp_comb_ob = new_comb_ob;
                new_comb_ob = prev_new_comb_ob;
                prev_new_comb_ob = temp_comb_ob;
        }
        else
                new_comb_ob = (COMB_OB *) NULL;

        first_pt_picked = FALSE;
        prompt(24);
}  /* end combine_restart */    


/*
 *  combine_exec
 *
 *  description:        combines selected objects
 *
 *  parameters:         pt (Gpoint) - pt selected
 */

combine_exec(pt)
Gpoint pt;
{
        COMB_OB_PTR *comb_ob_ptr, *temp_comb_ob_ptr;
        COMB_OB *prev_comb_ob;
        COMB_OB *comb_ob;
        COMB_OB *dup_comb_ob = NULL;
        static Gpoint first_pt;
        OBJECT *object;
        OBJECT *dup_object;
        Gpoint extent[2];
        COMB_OB_PTR *list;

        extern COMB_OB_PTR *get_object_list();

        if (first_pt_picked == FALSE)
        {
                first_pt = pt;
                first_pt_picked = TRUE;
                prompt(25);
        }
        else  /* extent defined so combine objects within extent */
        {
                if (pt.x < first_pt.x)
                {
                        extent[MIN].x = pt.x;
                        extent[MAX].x = first_pt.x;
                }
                else
                {
                        extent[MIN].x = first_pt.x;
                        extent[MAX].x = pt.x;
                }

                if (pt.y < first_pt.y)
                {
                        extent[MIN].y = pt.y;
                        extent[MAX].y = first_pt.y;
                }
                else
                {
                        extent[MIN].y = first_pt.y;
                        extent[MAX].y = pt.y;
                }

                list = get_object_list(extent);
                if (list == (COMB_OB_PTR *) NULL)
                {
                        first_pt_picked = FALSE;
                        full_prompt(26,10);
                        return;
                }

                free_lists();
                collect_garbage();

                /* if not allocated previously, allocate
                   new combination object to store group of
                   objects in list */

                if (new_comb_ob == (COMB_OB *) NULL)
                {
                        new_comb_ob = alloc_comb_ob();
                        enq_comb_ob(new_comb_ob);
                        enq_comb_ob_list(new_comb_ob,&new_list);
                }
                else
                {
                        prev_new_comb_ob = new_comb_ob;
                        new_comb_ob = copy_comb_ob(prev_new_comb_ob);
                        prev_new_comb_ob->visibility = GINVISIBLE;
                        enq_comb_ob(new_comb_ob);
                        enq_comb_ob_list(prev_new_comb_ob,&undo_list);
                        enq_comb_ob_list(new_comb_ob,&new_list);
                }

                /*  make copy of each comb_ob in list 
                    (i.e. dup_comb_ob).  Put all obejcts except 
                    those picked in dup_comb_ob.
                    Put picked objects in new_comb_ob */

                prev_comb_ob = (COMB_OB *) NULL;
                comb_ob_ptr = list;
                while (comb_ob_ptr != (COMB_OB_PTR *) NULL)
                {
                        /* dup_comb_ob is an exact duplicate
                           of comb_ob except that objects from list are
                           set to be invisible */

                        comb_ob = comb_ob_ptr->comb_ob;
                        if (prev_comb_ob != comb_ob)
                        {
                                dup_comb_ob = copy_comb_ob(comb_ob);
                                comb_ob->visibility = GINVISIBLE;
                                enq_after(dup_comb_ob,comb_ob);
                                enq_comb_ob_list(comb_ob,&undo_list);
                                enq_comb_ob_list(dup_comb_ob,&new_list);
                        }

                        /* find object in list from among chldren
                           of both comb_ob and dup_comb_ob */

                        object = comb_ob->child;
                        dup_object = dup_comb_ob->child;
                        while (object != comb_ob_ptr->object)
                        {
                                object = object->next;
                                dup_object = dup_object->next;
                        }
                        dup_object->visibility = GINVISIBLE;

                        dup_object = copy_object(object);
                        add_to_comb_ob(new_comb_ob,dup_object,TAIL);

                        prev_comb_ob = comb_ob;
                        temp_comb_ob_ptr = comb_ob_ptr->next;
                        free((char *) comb_ob_ptr);
                        comb_ob_ptr = temp_comb_ob_ptr;
                }

                draw_comb_ob(new_comb_ob);

                first_pt_picked = FALSE;
                prompt(24);
        }
}  /* end combine_exec */


/*
 *  combine_cleanup
 *
 *  description:        cleans up combine menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - combine menu item
 */

combine_cleanup(menu_item)
MENU_ITEM *menu_item;
{
        if (!eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                pop_curr_trans();
                unhilite(menu_item->key);
                mitem_in_progress = (MENU_ITEM *) NULL;
        }
        else
        {
                mitem_in_progress = menu_item;
        }
        set_currmitem((MENU_ITEM *) NULL);
}  /* end combine_cleanup */


/*
 *  split_init
 *
 *  description:        initializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - split menu item
 */

split_init(menu_item)
MENU_ITEM *menu_item;
{
        set_currmitem(menu_item);
        hilite(menu_item->key);
        push_curr_trans();
        activate(PICT_AREA);
        prompt(27);
}  /* end split_init */ 


/*
 *  split_restart
 *
 *  description:        reintializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - split menu item
 */

split_restart(menu_item)
MENU_ITEM *menu_item;
{
        if (eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                set_currmitem(menu_item);
        }
        else
        {
                prompt(27);
        }
}  /* end split_restart */      


/*
 *  split_exec
 *
 *  description:        splits selected comb_ob
 *
 *  parameters:         pt (Gpoint) - pt selected
 */

split_exec(pt)
Gpoint pt;
{
        BOOLEAN hit;
        COMB_OB *comb_ob;
        COMB_OB *new_comb_ob;
        COMB_OB *prev;
        OBJECT *object;
        OBJECT *dup_object;

        detect_object(&hit,&comb_ob,&object,pt);
        if (hit == TRUE)
        {
                free_lists();

                comb_ob->visibility = GINVISIBLE;
                enq_comb_ob_list(comb_ob,&undo_list);

                prev = comb_ob;
                for (object = comb_ob->child;
                        object != (OBJECT *) NULL;
                        object = object->next)
                {
                        if (object->visibility == GINVISIBLE)
                                continue;

                        dup_object = copy_object(object);
                        new_comb_ob = alloc_comb_ob();
                        add_to_comb_ob(new_comb_ob,dup_object,TAIL);
                        enq_after(new_comb_ob,prev);
                        enq_comb_ob_list(new_comb_ob,&new_list);
                        prev = new_comb_ob;
                }

                prompt(27);
        }
        else
                reprompt(15);
}  /* end split_exec */


/*
 *  split_cleanup
 *
 *  description:        cleans up split menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - split menu item
 */

split_cleanup(menu_item)
MENU_ITEM *menu_item;
{
        if (!eq(get_newmitem(),EDIT_PICTURE_UNDO))
        {
                pop_curr_trans();
                unhilite(menu_item->key);
                mitem_in_progress = (MENU_ITEM *) NULL;
        }
        else
        {
                mitem_in_progress = menu_item;
        }
        set_currmitem((MENU_ITEM *) NULL);
}  /* end split_cleanup */


/*
 *  enq_comb_ob_list
 *
 *  description:        enques comb_ob on list
 *                      (either undo_List or new_list)
 *
 *  parameters:         comb_ob (COMB_OB *) - combination object
 *                      list (COMB_OB_PTR *) - undo_list or new_list
 */

enq_comb_ob_list(comb_ob,list)
COMB_OB *comb_ob;
COMB_OB_PTR **list;
{
        COMB_OB_PTR *comb_ob_ptr;

        comb_ob_ptr = (COMB_OB_PTR *) calloc((unsigned) 1,
                sizeof(COMB_OB_PTR));

        comb_ob_ptr->next = *list;
        *list = comb_ob_ptr;
        comb_ob_ptr->comb_ob = comb_ob;
}  /* end enq_comb_ob_list */


/*
 *  edit_picture_undo
 *
 *  description:        undoes last action (delete, erase copy etc.)
 *                      everything in undo_list is made visible
 *                      everyting in new_list is made invisible
 *                      then lists are swapped
 *
 *  parameters:         menu_item (MENU_ITEM *) - undo menu item
 */

edit_picture_undo(menu_item)
MENU_ITEM *menu_item;
{
        COMB_OB_PTR *comb_ob_ptr;

        /* if there is something to undo */

        if ((new_list != (COMB_OB_PTR *) NULL) ||
                (undo_list != (COMB_OB_PTR *) NULL))
        {
                hilite(menu_item->key);

                /* make most recently added changes invisible */

                for (comb_ob_ptr = new_list;
                        comb_ob_ptr != (COMB_OB_PTR *) NULL;
                        comb_ob_ptr = comb_ob_ptr->next)
                {
                        comb_ob_ptr->comb_ob->visibility = GINVISIBLE;
                        erase_comb_ob(comb_ob_ptr->comb_ob);
                }

                /* make most recently added changes visible */

                for (comb_ob_ptr = undo_list;
                        comb_ob_ptr != (COMB_OB_PTR *) NULL;
                        comb_ob_ptr = comb_ob_ptr->next)
                {
                        comb_ob_ptr->comb_ob->visibility = GVISIBLE;
                }

                if ((new_list != (COMB_OB_PTR *) NULL) ||
                        ((undo_list != (COMB_OB_PTR *) NULL) &&
                        (get_update_mode() != QUICK_UPDATE)))
                {
                        redraw_objects();
                }
                else if ((undo_list != (COMB_OB_PTR *) NULL) &&
                        (get_update_mode() == QUICK_UPDATE))

                        for (comb_ob_ptr = undo_list;
                                comb_ob_ptr != (COMB_OB_PTR *) NULL;
                                comb_ob_ptr = comb_ob_ptr->next)
                        {
                                draw_comb_ob(comb_ob_ptr->comb_ob);
                        }

                /* swap lists so newly added changes are in new_list
                   etc. */

                comb_ob_ptr = new_list;
                new_list = undo_list;
                undo_list = comb_ob_ptr;

                unhilite(menu_item->key);
        }

        /* restart menu item which was in progress when undo was
           picked */

        if (mitem_in_progress != (MENU_ITEM *) NULL)
        {
                menu_item_func_tbl[find_menu_item_func_tbl_idx(
                        mitem_in_progress->key)].restart(
                        mitem_in_progress);

        }
        else
                set_currmitem((MENU_ITEM *) NULL);
}  /* end edit_picture_undo */


/*
 *  free_lists
 *
 *  description:        frees lists but not comb_obs being pointed to
 *
 *  parameters:         none
 */

free_lists()
{
        COMB_OB_PTR *comb_ob_ptr, *temp;

        /* free undo list */

        comb_ob_ptr = undo_list;
        while (comb_ob_ptr != (COMB_OB_PTR *) NULL)
        {
                temp = comb_ob_ptr->next;
                free((char *) comb_ob_ptr);
                comb_ob_ptr = temp;
        }

        undo_list = (COMB_OB_PTR *) NULL;

        comb_ob_ptr = new_list;
        while (comb_ob_ptr != (COMB_OB_PTR *) NULL)
        {
                temp = comb_ob_ptr->next;
                free((char *) comb_ob_ptr);
                comb_ob_ptr = temp;
        }

        new_list = (COMB_OB_PTR *) NULL;

}  /* free lists */


/* popup routines */

static int angle = 90;
static int min_angle = 0;
static int max_angle = 90;

/*
 *  init_angle_popup
 *
 *  description:        draws angle popup on screen
 *
 *  parameters:         popup (POPUP *) - line width popup
 */

init_angle_popup(popup)
POPUP *popup;
{
        display_number_popup(popup,"ANGLE",angle);
}  /* end init_angle popup */


/*
 *  switch_angle_state
 *
 *  description:        updates angle
 *
 *  parameters:         popup (POPUP *) - line width popup
 */

switch_angle_state(popup,pt)
POPUP *popup;
Gpoint pt;
{
        switch_number_popup_state(popup->extent,&angle,pt,
                min_angle,max_angle);
}  /* end switch_angle popup */


/*
 *  get_rot_angle
 *
 *  description:        returns current value of angle in radians
 *
 *  parameters:         none
 *
 *  returns:            (Gfloat) - angle in radians
 */


Gfloat
get_rot_angle()
{
        return(angle * (2 * PI) / 360);
}

