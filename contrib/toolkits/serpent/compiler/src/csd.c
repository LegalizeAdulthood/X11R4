static char rcsid [] = "$Header: /u/04c/mnt/integ/compiler/src/RCS/csd.c,v 1.13 89/10/23 20:04:05 ejh Exp $";


/*
$Log:	csd.c,v $
 * Revision 1.13  89/10/23  20:04:05  ejh
 * defined memoryPack
 * 
 * Revision 1.12  89/07/24  14:54:39  little
 * ops removal
 * 
 * Revision 1.11  89/05/09  11:06:33  little
 * no changes
 * 
 * Revision 1.10  89/02/27  17:09:21  little
 * add some debug print
 * 
 * Revision 1.9  89/02/15  17:38:30  little
 * write the dialogue name onto the "exec" file for the command name
 * for the dm
 * 
 * Revision 1.8  89/02/09  16:32:45  ejh
 * added copyright notice
 * 
 * Revision 1.7  89/01/26  17:34:32  little
 * add some debug statements
 * 
 * Revision 1.6  89/01/18  14:25:49  ejh
 * changed the writing of .dial.init and .exec.dm_rt to reflect the new ill
 * file preamble
 * 
 * Revision 1.5  89/01/09  16:30:57  little
 * add some debug print.  add the mailbox_name_prop to sym tab infor
 * passed to run-time for objects.  
 * 
 * Revision 1.4  88/10/31  15:18:09  little
 * a couple of castes to make the compiler happy
 * 
 * Revision 1.3  88/10/24  17:09:09  little
 * put out a new property for objects (fixed_method_snippet_prop) which
 * at run-time is the snippet number of the method associated with the object.
 * note, this is different from the compile-time use of
 * fixed_method_snippet_prop, which astually holds the snippet tree.
 * 
 * Revision 1.2  88/10/03  08:59:16  little
 * add processing for dm shared data and add some casts to make saber happy
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

#define memoryPack

#include <strings.h>
#include <stdio.h>
#include "listPack.h"
#include "global.h"
#include "isc.h"
#include "c22.h"
#include "cs2.h"
#include "csc.h"
#include "csd.h"
#include "cus.h"
#include "cam.h"
#include "dgr.h"
#include "din.h"

static void csd_string_write ();
static void csd_prop_write ();
static void csd_symbol_write ();
static void csd_integer_write ();

static char great_big_string [2048];

#define smart_loop_through_list(list, item_name, statement)\
   {LIST value; char *item_name;\
   if (value = (LIST) list)\
      loop_through_list (value, item, char *)\
         {statement;}\
   }

#define csd_list_write(stream, statement)\
   fwrite (OPEN_LIST_ESCAPE, 1, strlen (OPEN_LIST_ESCAPE), stream);\
   statement;\
   fwrite (CLOSE_LIST_ESCAPE,1, strlen(CLOSE_LIST_ESCAPE), stream);

/*--------------------------------------------------------------------------*\
|  Routine: csd_dm_rt_write
|
|  Description:
|     writes the run-time file which contains information about which tap
|	processess to fire up and their parameters (also includes the name of
|	dm process)
\*--------------------------------------------------------------------------*/
void csd_dm_rt_write (dialogue_name)
string dialogue_name;
{

    smart_loop_through_list (cus_get_prop (car_god_vc, ill_files_prop), item,
	{fprintf (cs2_dm_init_stream, "%s %s\n", cus_get_prop (item,
	    mailbox_name_prop), cus_get_prop (item, ill_file_name_prop));
	if (!strcmp (cus_get_prop (item, mailbox_name_prop), DM_BOX_NAME))
	    {fprintf (cs2_dm_exec_stream, "%s %s\n", cus_get_prop (item,
		execution_name_prop), dialogue_name);
	    c22_got_dm_box = true;
	    }
	else
	    fprintf (cs2_dm_exec_stream, "%s %s\n", cus_get_prop (item,
		execution_name_prop), cus_get_prop (item, exec_command_prop));
	})
    return;
}					/* end csd_dm_rt_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_var_write
|
|  Description:
|     writes all info wrt vars onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
void csd_var_write (var)
cus_sym var;
{   LIST value;

    fwrite (BEGIN_SYMBOL_ENTRY_ESCAPE, 1, strlen (BEGIN_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);

    csd_symbol_write (cs2_symtab_stream, cus_get_prop (var, u_name_prop),
	variable_sym);
    csd_prop_write (cs2_symtab_stream, original_name_prop);
    csd_string_write (cs2_symtab_stream, cus_sym_representation (var));
    csd_prop_write (cs2_symtab_stream, instances_prop);
    csd_list_write (cs2_symtab_stream, 1)
    csd_prop_write (cs2_symtab_stream, initial_value_snippet_prop);
    if (value = (LIST) cus_get_prop (var, initial_value_snippet_prop))
	csd_integer_write (cs2_symtab_stream,
	    (int) cus_get_prop (get_list_head (value), c_proc_number_prop));
    else
	csd_integer_write (cs2_symtab_stream, NO_GENERIC_ROUTINE);

    fwrite (END_SYMBOL_ENTRY_ESCAPE, 1, strlen (END_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);

    return;
}					/* end csd_var_write*/

/*--------------------------------------------------------------------------*\
|  Routine: csd_att_write
|
|  Description:
|     writes all info wrt atts onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
void csd_att_write (att)
cus_sym att;
{   LIST value;

    fwrite (BEGIN_SYMBOL_ENTRY_ESCAPE, 1, strlen (BEGIN_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);
    csd_symbol_write (cs2_symtab_stream, cus_get_prop (att, u_name_prop),
	attribute_sym);
    csd_prop_write (cs2_symtab_stream, value_snippet_prop);
    if (value = (LIST) cus_get_prop (att, value_snippet_prop))
	csd_integer_write (cs2_symtab_stream,
	    (int) cus_get_prop (get_list_head (value), c_proc_number_prop));
    else
	csd_integer_write (cs2_symtab_stream, NO_GENERIC_ROUTINE);
    fwrite (END_SYMBOL_ENTRY_ESCAPE, 1, strlen (END_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);

    return;
}					/* end csd_att_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_vc_write
|
|  Description:
|     writes all info wrt vcs onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
void csd_vc_write (vc)
cus_sym vc;
{   LIST value;
    string vc_name;
    cus_sym creating_sd;

    vc_name = cus_sym_representation (vc);

    DEBUG2 (fprintf (stderr, "csd_vc_write enter: vc = %s\n", vc_name););

    fwrite (BEGIN_SYMBOL_ENTRY_ESCAPE, 1, strlen (BEGIN_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);
    csd_symbol_write (cs2_symtab_stream, cus_get_prop (vc, u_name_prop),
	vc_sym);
    csd_prop_write (cs2_symtab_stream, original_name_prop);
    csd_string_write (cs2_symtab_stream, cus_sym_representation (vc));

    if (creating_sd = (cus_sym) cus_get_prop (vc, creating_sd_prop))
	{
	csd_prop_write (cs2_symtab_stream, creating_sd_prop);
	strcpy (great_big_string, SD_QUALIFIER_STRING);
	strcat (great_big_string, cus_sym_representation (creating_sd));
	csd_symbol_write (cs2_symtab_stream, great_big_string, sd_sym);
	}
    csd_prop_write (cs2_symtab_stream, variables_prop);
    csd_list_write (cs2_symtab_stream, smart_loop_through_list
	(cus_get_prop (vc, variables_prop), item, csd_symbol_write
	    (cs2_symtab_stream, cus_get_prop (item, u_name_prop),
	    variable_sym)));
    csd_prop_write (cs2_symtab_stream, objects_prop);
    csd_list_write (cs2_symtab_stream, smart_loop_through_list
	(cus_get_prop (vc, objects_prop), item, csd_symbol_write
	    (cs2_symtab_stream, cus_get_prop (item, u_name_prop),
	    object_sym)));
    csd_prop_write (cs2_symtab_stream, sub_vcs_prop);
    csd_list_write (cs2_symtab_stream, smart_loop_through_list
	(cus_get_prop (vc, sub_vcs_prop), item, csd_symbol_write
	    (cs2_symtab_stream, cus_get_prop (item, u_name_prop),
	    object_sym)));
    csd_prop_write (cs2_symtab_stream, instances_prop);
    csd_list_write (cs2_symtab_stream, 1)
    csd_prop_write (cs2_symtab_stream, value_snippet_prop);
    csd_integer_write (cs2_symtab_stream, cus_get_prop (vc,
	c_proc_number_prop));
    csd_prop_write (cs2_symtab_stream, create_actions_snippet_prop);
    if (value = (LIST) cus_get_prop (vc, create_actions_snippet_prop))
	csd_integer_write (cs2_symtab_stream,
	    (int) cus_get_prop (get_list_head (value), c_proc_number_prop));
    else
	csd_integer_write (cs2_symtab_stream, NO_GENERIC_ROUTINE);

    csd_prop_write (cs2_symtab_stream, destroy_actions_snippet_prop);
    if (value = (LIST) cus_get_prop (vc, destroy_actions_snippet_prop))
	csd_integer_write (cs2_symtab_stream,
	    (int) cus_get_prop (get_list_head (value), c_proc_number_prop));
    else
	csd_integer_write (cs2_symtab_stream, NO_GENERIC_ROUTINE);

    fwrite (END_SYMBOL_ENTRY_ESCAPE, 1, strlen (END_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);

    DEBUG2 (fprintf (stderr, "csd_vc_write exit:\n"););
    return;
}					/* end csd_vc_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_obj_write
|
|  Description:
|     writes all info wrt objs onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
void csd_obj_write (obj)
cus_sym obj;
{   cus_sym m_proc_name, dd_def;
    string mailbox, object_type;
    LIST atts, method_prop;

    DEBUG2 (fprintf (stderr, "enter csd_obj_write: obj = %s\n",
	cus_sym_representation (obj)););

    fwrite (BEGIN_SYMBOL_ENTRY_ESCAPE, 1, strlen (BEGIN_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);

    csd_symbol_write (cs2_symtab_stream, cus_get_prop (obj, u_name_prop),
	object_sym);

    csd_prop_write (cs2_symtab_stream, original_name_prop);
    csd_string_write (cs2_symtab_stream, cus_sym_representation (obj));

    csd_prop_write (cs2_symtab_stream, mailbox_name_prop);
    dd_def = (cus_sym) cus_get_prop (obj, data_definition_prop);
    mailbox = (string) cus_get_prop (dd_def, mailbox_name_prop);
    DEBUG3 (fprintf (stderr, "\tdd_def = %s, mailbox = %s\n",
	cus_sym_representation ( dd_def), mailbox););
    csd_string_write (cs2_symtab_stream, mailbox);

    csd_prop_write (cs2_symtab_stream, object_type_prop);
    object_type = cus_sym_representation (cus_get_prop (obj,
	object_type_prop));
    DEBUG3 (fprintf (stderr, "\tobject_type_prop = %s\n", object_type););

    csd_string_write (cs2_symtab_stream, object_type);

    csd_prop_write (cs2_symtab_stream, instances_prop);
    csd_list_write (cs2_symtab_stream, 1)

    if (method_prop = (LIST) cus_get_prop (obj, fixed_method_snippet_prop))
	{
	m_proc_name = (cus_sym) get_list_next (method_prop, NULL);
	DEBUG3 (fprintf (stderr, "method proc name = %s\n",
	    cus_sym_representation (m_proc_name)););
	DEBUG3 (fprintf (stderr, "method proc number = %d\n",
	    (int) cus_get_prop (m_proc_name, c_proc_number_prop)););
	csd_prop_write (cs2_symtab_stream, fixed_method_snippet_prop);
	csd_integer_write (cs2_symtab_stream, (int) cus_get_prop (m_proc_name,
	    c_proc_number_prop));
	}

    csd_prop_write (cs2_symtab_stream, attributes_prop);
    atts = (LIST) cus_get_prop (obj, attributes_prop);
    DEBUG3 (fprintf (stderr, "\t# atts = %d\n", list_length (atts)););
    csd_list_write (cs2_symtab_stream, smart_loop_through_list (atts, item,
	csd_symbol_write (cs2_symtab_stream, cus_get_prop (item, u_name_prop),
	attribute_sym)));

    fwrite (END_SYMBOL_ENTRY_ESCAPE, 1, strlen (END_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);

    DEBUG2 (fprintf (stderr, "csd_obj_write: exit\n"););
    return;
}					/* end csd_obj_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_cc_write
|
|  Description:
|     writes all info wrt ccs onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
void csd_cc_write (vc)
cus_sym vc;
{
   return;
}					/* end csd_cc_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_action_write
|
|  Description:
|     writes all info wrt creationd and destruction actions into the dm
|	run-time sym tab init file
\*--------------------------------------------------------------------------*/
void csd_action_write (action, vc)
string action;
cus_sym vc;
{
   return;
}					/* end csd_action_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_integer_write
|
|  Description:
|     writes an integer onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
static void csd_integer_write (stream, the_int)
FILE *stream;
int the_int;
{
   DEBUG3 (fprintf (stderr, "integer = %d\n", the_int))
   fwrite (INTEGER_ESCAPE, 1, strlen (INTEGER_ESCAPE), stream);
   fwrite (&the_int, 1, sizeof (int), stream);
   return;
}					/* end csd_integer_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_symbol_write
|
|  Description:
|     writes a symbol onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
static void csd_symbol_write (stream, name, type)
FILE *stream;
string name;
cus_sym_type type;

{  int len;

   len = strlen (name);
   fwrite (SYMBOL_ESCAPE, 1, strlen (SYMBOL_ESCAPE), stream);
   fwrite (&len, 1, sizeof (int), stream);
   fwrite (name, 1, len, stream);
   fwrite (&type, 1, sizeof (cus_sym_type), stream);
   return;
}					/* end csd_symbol_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_prop_write
|
|  Description:
|     writes a property id onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
static void csd_prop_write (stream, prop)
FILE *stream;
cus_property_type prop;
{
    DEBUG2 (fprintf (stderr, "csd_prop_write enter: prop = %d, %s\n",
	prop, cus_type_print_val (prop)););
    fwrite (PROPERTY_ESCAPE, 1, strlen (PROPERTY_ESCAPE), stream);
    fwrite (&prop, sizeof (cus_property_type), 1, stream);
    DEBUG2 (fprintf (stderr, "csd_prop_write exit\n"););
    return;
}					/* end csd_prop_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_string_write
|
|  Description:
|     writes a string onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
static void csd_string_write (stream, the_string)
FILE *stream;
string the_string;
{  int len;

   len = strlen (the_string);
   fwrite (STRING_ESCAPE, 1, strlen (STRING_ESCAPE), stream);
   fwrite (&len, 1, sizeof (int), stream);
   fwrite (the_string, 1, len, stream);
   return;
}					/* end csd_string_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_sdd_element_write
|
|  Description:
|     writes all info (sd template) wrt a particular sd element onto the dm
|	run-time sym tab init file
\*--------------------------------------------------------------------------*/
static void csd_sdd_element_write (dummy, element)
char *dummy;
char *element;
{   string element_name;
    LIST value;
    int new;

    set_status (ok);
    element_name = idd_get_name (element);

    DEBUG2 (fprintf (stderr, "entering csd_sdd_element_write: element = %s\n",
	element_name););

    fwrite (BEGIN_SYMBOL_ENTRY_ESCAPE, 1, strlen (BEGIN_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);
	/* create a unique symbol for the element and write it out */
    strcpy (great_big_string, SD_QUALIFIER_STRING);
    strcat (great_big_string, element_name);
    csd_symbol_write (cs2_symtab_stream, great_big_string, sd_sym);

    csd_prop_write (cs2_symtab_stream, potential_vcs_prop);
    csd_list_write (cs2_symtab_stream, 1)

    csd_prop_write (cs2_symtab_stream, instances_prop);
    csd_list_write (cs2_symtab_stream, 1)

    fwrite (END_SYMBOL_ENTRY_ESCAPE, 1, strlen (END_SYMBOL_ENTRY_ESCAPE),
	cs2_symtab_stream);

    DEBUG2 (fprintf (stderr, "leaving csd_sdd_element_write\n"););
    return;
}                                              /* end csd_sdd_element_write */

/*--------------------------------------------------------------------------*\
|  Routine: csd_ill_files_write
|
|  Description:
|     writes all info (sd templates) wrt all sd descriptions which were used
|	in the slang program onto the dm run-time sym tab init file
\*--------------------------------------------------------------------------*/
void csd_ill_files_write ()
{   LIST ills;
    cus_sym ill_sym;
    idd_data_definition current_data_definition;

    DEBUG2 (fprintf (stderr, "entering csd_ill_files_write\n"););

    ills = (LIST) cus_get_prop (car_god_vc, ill_files_prop);
    if (list_length (ills) > 0)
	loop_through_list (ills, ill_sym, cus_sym)
	    {
	    DEBUG3 (fprintf (stderr, "processing ill file = %s\n",
		cus_get_prop (ill_sym, ill_file_name_prop)))
	    current_data_definition =
		(idd_data_definition) cus_get_prop (ill_sym, idd_handle_prop);
	    idd_walk_data_definition (current_data_definition,
		csd_sdd_element_write, NULL);
	    }
    DEBUG2 (fprintf (stderr, "leaving csd_ill_files_write\n"););
    return;
}					/* end csd_ill_files_write */
