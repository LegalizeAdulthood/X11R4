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
 *      read_menu_file
 *      read_menu
 */

#include <xgks.h>

#include <stdio.h>
#include <strings.h>

#include "screen_items.h"
#include "menu.h"
#include "key.h"


/*
 *  read_menu_file
 *
 *  description:        read in file containing menu tree
 *
 *  parameters:         filename (char *) - name of file
 *                                          containing menu info
 */

read_menu_file(filename)
char *filename;
{
        FILE *fd;                       /* fd for filename */

        /* open file for reading */

        fd = fopen(filename,"r");
        if (fd == (FILE *) NULL)
                exit_error("read_menu_file",2);
        
        /* read in and initialize menu tree */

        read_menu(fd,(MENU *) NULL,(MENU_ITEM *) NULL);
        set_menu(SEC_MENU_AREA,(MENU *) NULL);

        /* close file */

        fclose(fd);

}  /* end read_menu_file */


/*
 *  read_menu
 *
 *  description:        reads in and initializes data structures
 *                      for a single menu item.
 *
 *  parameters:         fd (FILE *) - fd for file containing menu tree
 *                      parent_menu (MENU *) - ptr to menu one level up
 *                      parent_menu_item (MENU_ITEM *) - ptr to menu
 *                              item called from
 */

read_menu(fd,parent_menu,parent_menu_item)
FILE *fd;                       /* menu file */
MENU *parent_menu;              /* menu called from */
MENU_ITEM *parent_menu_item;    /* menu item called from */
{
        MENU *menu_ptr;         /* menu being read */
        MENU_ITEM *menu_item_ptr;       /* menu item being read */
        POPUP *popup_ptr;       /* popups being read */
        char **name_ptr;        /* menu item name */
        char *temp;             /* temp for next char str */
        int i,j,n;

        extern char *get_char_str();

        /* set up parent and child links */

        menu_ptr = (MENU *) calloc(1,sizeof(MENU));
        if(parent_menu_item == (MENU_ITEM *) NULL)
                set_menu(PRI_MENU_AREA,menu_ptr);
        else
                parent_menu_item->child = menu_ptr;
        menu_ptr->parent = parent_menu;

        /* read in key */

        menu_ptr->key = get_char_str(fd,2);

        /* read in menu name */

        get_int(fd,&(menu_ptr->nlns),2);
        menu_ptr->name = (char **) calloc((unsigned) menu_ptr->nlns,
                sizeof(char *));
        for (i=0; i<(menu_ptr->nlns); i++)
        {
                menu_ptr->name[i] = get_char_str(fd,2);
        }

        /* read in popups */

        get_int(fd,&n,2);
        menu_ptr->poplst = (POPUP *) NULL;
        popup_ptr = (POPUP *) NULL;
        for (i=1; i<=n; i++)
        {
                /* allocate popup record */

                if (menu_ptr->poplst == (POPUP *) NULL)
                {
                        menu_ptr->poplst = (POPUP *) calloc(1,
                                sizeof(POPUP));
                        popup_ptr = menu_ptr->poplst;
                }
                else
                {
                        popup_ptr->next = (POPUP *) calloc(1,
                                sizeof(POPUP));
                        popup_ptr = popup_ptr->next;
                }

                /* fill in record */

                popup_ptr->key = get_char_str(fd,2);
                popup_ptr->is_active = FALSE;
        }

        /* read in menu items */

        get_int(fd,&(menu_ptr->noitems),2);
        if (max_num_menu_entries < (menu_ptr->noitems + 1))
                max_num_menu_entries = menu_ptr->noitems + 1;

        menu_ptr->item = (MENU_ITEM *) calloc((unsigned)
                menu_ptr->noitems, sizeof(MENU_ITEM));

        menu_item_ptr = menu_ptr->item;
        menu_item_ptr->key = get_char_str(fd,2);

        for (i=0; i<menu_ptr->noitems; i++)
        {

                /* read in no of lns in menu item name */

                get_int(fd,&(menu_item_ptr->nlns),2);

                /* read in menu item name */

                menu_item_ptr->name = (char **) calloc((unsigned)
                        menu_item_ptr->nlns,sizeof(char *));
                name_ptr = menu_item_ptr->name;

                for (j=0; j<menu_item_ptr->nlns; j++)
                {
                        *name_ptr = get_char_str(fd,2);
                        name_ptr++;
                }
                
                /* read in popups */

                get_int(fd,&n,2);
                menu_item_ptr->poplst = (POPUP *) NULL;
                for (j=1; j<=n; j++)
                {
                        /* allocate popup record */

                        if (menu_item_ptr->poplst == (POPUP *) NULL)
                        {
                                menu_item_ptr->poplst = (POPUP *) 
                                        calloc(1,sizeof(POPUP));
                                popup_ptr = menu_item_ptr->poplst;
                        }
                        else
                        {
                                popup_ptr->next = (POPUP *) calloc(1,
                                        sizeof(POPUP));
                                popup_ptr = popup_ptr->next;
                        }
        
                        /* read in popup key */

                        popup_ptr->key = get_char_str(fd,2);
                        popup_ptr->is_active = FALSE;
                }

                /* check for child menu */

                temp = (char *) NULL;
                temp = get_char_str(fd,2);

                if (temp[0] == '-')
                {
                        /* child menu exists */

                        read_menu(fd,menu_ptr,menu_item_ptr);

                        /* read in next menu item key, if any */

                        if (i < menu_ptr->noitems)
                        {
                                menu_item_ptr++;
                                menu_item_ptr->key = get_char_str(fd,2);
                        }
                }
                else if (i < menu_ptr->noitems)
                {
                        /* if no child menu and not last menu item
                           then temp =  key of next menu item */

                        menu_item_ptr++;
                        menu_item_ptr->key = (char *) calloc ((unsigned)
                                (strlen(temp) + 1), sizeof(char));
                        (void) strcpy(menu_item_ptr->key,temp);
                }
                free((char *) temp);

        }  /* end read in menu items loop */

}  /* end read_menu */


