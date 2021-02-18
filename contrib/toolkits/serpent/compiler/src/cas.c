static char rcsid [] =
	"$Header: /u/04c/mnt/integ/compiler/src/RCS/cas.c,v 1.8 89/02/20 16:27:18 little Exp $";


/*
$Log:	cas.c,v $
 * Revision 1.8  89/02/20  16:27:18  little
 * no changes
 * 
 * Revision 1.7  89/02/20  16:21:01  little
 * no changes
 * 
 * Revision 1.6  89/02/13  16:24:54  little
 * no changes
 * 
 * Revision 1.5  89/02/10  09:26:40  little
 * no changes
 * 
 * Revision 1.4  89/02/10  09:12:08  little
 * no changes
 * 
 * Revision 1.3  89/02/09  16:27:07  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/12/14  10:43:24  little
 * changes some debug1's to debug2
 * 
 * Revision 1.1  88/12/05  17:36:43  little
 * Initial revision
 * 
*/

/*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#include <strings.h>
#include <stdio.h>
#include "listPack.h"
#include "global.h"
#include "c22.h"
#include "cas.h"

static LIST cas_new_tree;

/* --------------------------------------------     cas_walk_dot_tree */
static void cas_walk_dot_tree (old_tree)
char *old_tree;
{
   if (cus_is_sym (old_tree))
      add_to_tail (cas_new_tree, old_tree);
    else
      {cas_walk_dot_tree (get_list_pos (old_tree, 2));
      cas_walk_dot_tree (get_list_pos (old_tree, 3));
      }
   return;
}					/* cas_walk_dot_tree */


/* --------------------------------------------     cas_fix_dot_tree */
char * cas_fix_dot_tree (old_tree)
char * old_tree;
{

   DEBUG2 (fprintf (stderr, "entering cas_fix_dot_tree: old_tree =\n");
           cud_print_tree (stderr, old_tree, "\t"););

   cas_new_tree = make_list ();
   if (cus_is_sym (old_tree))
      add_to_head (cas_new_tree, old_tree);
    else
      cas_walk_dot_tree (old_tree);

   DEBUG2 (fprintf (stderr, "leaving cas_fix_dot_tree: cas_new_tree =\n");
           cud_print_tree (stderr, cas_new_tree, "\t"););
   return ((char *) cas_new_tree);
}					/* end cas_fix_dot_tree */

