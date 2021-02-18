static char rcsid [] =
	"$Header: /u/04c/mnt/new_integ/utilities/src/RCS/cus.c,v 1.15 89/10/08 00:16:22 serpent Exp $";


/*--------------------------------------------------------------------------*\
|
|  Name:  Compiler symbol table package (cus)
|
|  Version: 1.0
|
|  Change History:
|	9 jun 88 - change property "name" print to use the name array instead
|		of a print routine
|	8 jun 88 - add the cus_sym_delete routine (it is a do nothing for now)
|	10 may 1988 - module created
|
|  Description:  
|     Provides the routines necessary to manage and use the symbol table
|
|  Component of:
|     compiler
|
|  Related files:
|     cus.h - the user accessable header file necessary to use this package
|     cus-private.h - local header file used only by the symbol table routines
|
|  Project:
|     SERPENT System
|     User Interface Prototyping
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Author:  
|     Reed  Little
|     little@sei.cmu.edu  
|
$Log:	cus.c,v $
 * Revision 1.15  89/10/08  00:16:22  serpent
 * short names: cus_private.h => cus_priv.h
 * 
 * Revision 1.14  89/09/26  18:10:34  bmc
 * Change cus_create_sym_tab so that it doesn't create symbol table if it
 * already exists.
 * 
 * Revision 1.13  89/06/19  11:41:05  little
 * fix some errors in the reinstallation of redefined processing
 * 
 * Revision 1.12  89/06/14  10:59:01  little
 * add redefined processing back and make its use selectable via a parameter
 *   to the sym tba init routine
 * 
 * Revision 1.11  89/06/09  13:40:54  little
 * take out use of redefined_prop and associated mechanism
 * reorder case statements for props which are kept in the "symbol" itself
 *   to take into account frequency of use (most frequent first).
 * add prop usage count mechanism
 * add template,original_name, current_value, & current_type props to
 *   the symbol structure (for speed of access)
 * 
 * Revision 1.10  89/05/19  09:49:14  little
 * fix up some comments
 * 
 * Revision 1.9  89/05/15  11:41:14  little
 * add some comments to make len happy
 * 
 * Revision 1.8  89/05/11  15:41:44  little
 * take the redefined_prop out of the property list and put into the
 * symbol structure.  this is to speed-up get prop calls.
 * 
 * Revision 1.7  89/02/10  14:47:53  ejh
 * added copyright notice
 * 
 * Revision 1.6  89/02/01  21:44:18  rcs
 * fixed bug in cus_sym_representation of incorrectly setting bad status
 * 
 * Revision 1.5  89/02/01  16:20:23  little
 * change cus_sym_representation to work correctly with the
 * new property named alternate_print_string_prop, if the property
 * is present, it's value is the sym_representation
 * 
 * Revision 1.4  89/01/17  16:05:26  rcs
 * added cus_get_sym_type_string routine and supporting data structure
 * 
 * Revision 1.3  89/01/09  15:51:45  little
 * add the concept of the redefined_prop.  when the prop is set for a symbol,
 * use the value of the prop (redefined_prop) as a symbol and get the
 * requested prop from that "new" symbol.
 * 
 * Revision 1.2  88/10/06  13:37:57  little
 * add log comments
 * 

|
\*--------------------------------------------------------------------------*\
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

/*-------------------- Includes --------------------------------------------*/

#include <strings.h>
#include <stdio.h>
#define memoryPack              /* use memoryPack rather than mallocPack    */
#include "hashPack.h"           /* C Toolbox - listPack                     */
#include "global.h"
#include "idd.h"
#include "isc.h"
#include "c22.h"
#include "cus.h"
#define cus_top
#include "cus_priv.h"


/*-------------------- Symbolic Constants ---------------------------------*/

/*-------------------- Type Definitions ------------------------------------*/

/*-------------------- Static Data ---------------------------------------*/

static char cus_no_sym_message [] = "NOT IN SYMBOL TABLE";

 				/* used to determined whether redefine
				   processing is to be peformed
				*/

static boolean cus_do_redefine;

/*--------------------------------------------------------------------------*\
|  Routine: cus_get_sym_type_string
|
|  Description:
|     The cus_get_sym_type_string function returns the string corresponding
|	to the specified symbol type.  this function is here in addition to the
|	cus_type_print_val function, because the person who added this function
|	(cus_get_sym_type_string) did not know about cus_type_print_val.  since
|	this function (cus_get_sym_type_string) is used in a lot of places
|	i (reed) do not want to look into, i just implemented it using
|	cus_type_print_val.
|
\*--------------------------------------------------------------------------*/
string cus_get_sym_type_string(symbol_type)
cus_sym_type symbol_type;
{
  return (cus_type_print_val (symbol_type));
}                                         /* end cus_get_sym_type_string    */


/*--------------------------------------------------------------------------*\
|  Routine: cus_int_get_prop
|
|  Description:
|     the internal version of the get property routine, this is the guy
|	that does most of the work.   it is seperate because there are time
|	when we do not want to go through the redefined prop (which is not
|	being used at this time to speed things up)
| 
\*--------------------------------------------------------------------------*/
char *cus_int_get_prop (sym, property_name)
cus_sym sym;
cus_property_type property_name;

{  cus_priv_property_ptr prop_ptr;
   char *ret_prop_val;

   set_status (ok);
   cus_prop_gets [(int) property_name] ++;
   
   if (sym)
      switch (property_name)
         {case type_prop:
            ret_prop_val = (char *) sym -> type;
            break;
         case template_prop:
            ret_prop_val = (char *) sym -> template;
            break;
         case original_name_prop:
            ret_prop_val = (char *) sym -> original_name;
            break;
         case current_value_prop:
            ret_prop_val = (char *) sym -> current_value;
            break;
         case current_type_prop:
            ret_prop_val = (char *) sym -> current_type;
            break;
         case redefined_prop:
            if (cus_do_redefine)
               ret_prop_val = (char *) sym -> redefined;
              else
					/* should be an error it someone asks
					   for redefined_prop and not enabled
					*/
               FATAL_ERROR (fprintf (stderr,
				  "cus_int_get_prop got a redefined_prop\n"););
            break;
         default:
            {prop_ptr = (cus_priv_property_ptr) get_list_val (sym-> properties,
      						              property_name);
            if (!prop_ptr)
               {set_status (cus_no_property);
               ret_prop_val = NULL;
               }
             else
               ret_prop_val = prop_ptr-> the_value;
            }
         }
    else
      {set_status (cus_null_symbol);
      ret_prop_val = NULL;
      }
   return (ret_prop_val);
}					/* end cus_int_get_prop */


/*--------------------------------------------------------------------------*\
|  Routine: cus_get_redefined
|
|  Description:
|     this routine "runs down" all redefined links, until a "gounded" symbol
|	is found.  the redefined prop allows for a user to temporarily
|	(or permentaly if desired) redefine one symbol to be another.  this way
|	one user routine can "fool" other user routines as to what a symbol
|	really is.
\*--------------------------------------------------------------------------*/
static cus_sym cus_get_redefined (sym)
cus_sym sym;

{  cus_sym test, redefined_sym;

   set_status (ok);
   if (!cus_do_redefine)
      return (sym);
   if (cus_int_is_sym (sym))
      {test = sym;
      redefined_sym = test;
      while (test = (cus_sym) test-> redefined)
         redefined_sym = test;
      set_status (ok);
      return (redefined_sym);
      }
    else
      return (NULL);

}                                       /* end cus_get_redefined */


/*--------------------------------------------------------------------------*\
|  Routine: cus_genname
|
|  Description:
|     generate a unique string
|     
\*--------------------------------------------------------------------------*/
char *cus_genname (prefix)
string prefix;
{  char buf [256];			/* WARNING: assumes generated strings
					   are no longer than 255 characters,
					   and does no testing for same
					*/
   char *ptr;

   sprintf (buf, "%s_%d", prefix, cus_gensym_seed);
   cus_gensym_seed += 1;
   ptr = strcpy (make_node (strlen (buf) + 1), buf);
   return (ptr);
}					/* end cus_genname */


/*--------------------------------------------------------------------------*\
|  Routine: cus_push_context
|
|  Description:
|     internal routine to "restructure" sym tab to a new context
|
\*--------------------------------------------------------------------------*/
static int cus_push_context (context, the_bucket)
cus_context_type context;
cus_name_bucket_ptr the_bucket;
{
   cus_sym look_sym;

   look_sym = (cus_sym) get_list_head (the_bucket-> inner_contexts);
   if (look_sym)
      {if (look_sym-> context == context)
         {if (the_bucket-> sym)
            add_to_head (the_bucket-> outer_contexts, the_bucket-> sym);
		/* no need to check if something to pop off inner_contexts
		   we would not be here if that was so,
		   due to previous if (look_sym) statement */
         the_bucket-> sym = (cus_sym)get_from_head(the_bucket->inner_contexts);
         }
      }
   return (0);
}					/* end cus_push_context */


/*--------------------------------------------------------------------------*\
|  Routine: cus_activate_context
|
|  Description:
|     user visible routine to "place" the sym tab into the desired context
| 
\*--------------------------------------------------------------------------*/
void cus_activate_context (context)
cus_context_type context;
{
   for_all_hashtable (cus_sym_tab-> the_table, context, cus_push_context);
   add_to_head (cus_sym_tab -> old_contexts, cus_sym_tab-> current_context);
   cus_sym_tab -> current_context = context;

   return;
}					/* end cus_activate_context */


/*--------------------------------------------------------------------------*\
|  Routine: cus_pop_context
|
|  Description:
|     internal routine used to put sym tab in parent context
|     
\*--------------------------------------------------------------------------*/
static int cus_pop_context (context, the_bucket)
cus_context_type context;
cus_name_bucket_ptr the_bucket;
{cus_context_type check;

   if (the_bucket->sym != NULL)
      check = the_bucket->sym->context;
    else
      check = -1;
   if (check == context)
      {add_to_head (the_bucket-> inner_contexts, the_bucket-> sym);
      if (list_length (the_bucket-> outer_contexts) < 0)
         FATAL_ERROR (fprintf (stderr, "no more outer contexts in bucket %s\n",
		      the_bucket-> name))

      the_bucket-> sym = (cus_sym) get_from_head (the_bucket-> outer_contexts);
      }
   return (0);
}					/* end cus_pop_context */


/*--------------------------------------------------------------------------*\
|  Routine: cus_current_context
|
|  Description:
|     returns the context in which the sym tab is currently
|     
\*--------------------------------------------------------------------------*/
cus_context_type cus_current_context ()
{
   return (cus_sym_tab-> current_context);
}					/* end cus_current_context */


/*--------------------------------------------------------------------------*\
|  Routine: cus_deactivate_context
|
|  Description:
|     user visible routine to place the sym tab into a parent context
\*--------------------------------------------------------------------------*/
cus_context_type cus_deactivate_context ()

{   cus_context_type old_context;

   old_context = cus_sym_tab -> current_context;

   if (old_context == BASE_CONTEXT)
      FATAL_ERROR (fprintf (stderr,
	  "cus_deactivate_context: trying to deactivate base context\n"))
   if (list_length (cus_sym_tab -> old_contexts) < 1)
      FATAL_ERROR (fprintf (stderr, "no more contexts in symbol table\n"))

   for_all_hashtable (cus_sym_tab-> the_table, old_context,
		      cus_pop_context);
   cus_sym_tab -> current_context = (cus_context_type) get_from_head
					(cus_sym_tab -> old_contexts);
   return (cus_sym_tab -> current_context);
}					/* end cus_deactivate_context */


/*--------------------------------------------------------------------------*\
|  Routine: cus_create_nu_context
|
|  Description:
|     user visible routine to create a nu context and go to same
|     
\*--------------------------------------------------------------------------*/
cus_context_type cus_create_nu_context ()
{
   add_to_head (cus_sym_tab -> old_contexts, cus_sym_tab-> current_context);
   cus_sym_tab -> current_context = ++cus_context_seed;
   return (cus_context_seed);
}					/* end cus_create_context */


/*--------------------------------------------------------------------------*\
|  Routine: cus_internal_walk
|
|  Description:
|     intercal routine to actually do the sym tab walk
|     
\*--------------------------------------------------------------------------*/
static void cus_internal_walk (their_funct, their_thing, the_bucket)
int (*their_funct) ();
char *their_thing;
cus_name_bucket_ptr the_bucket;

{
   DEBUG2 (fprintf (stderr, "entering cus_internal_walk\n"))
   if (the_bucket-> sym)
      their_funct (their_thing, the_bucket-> sym);
   for_all_list (the_bucket-> inner_contexts, their_thing, their_funct);
   for_all_list (the_bucket-> outer_contexts, their_thing, their_funct);
   return;
}					/* end cus_internal_walk */


/*--------------------------------------------------------------------------*\
|  Routine: cus_walk_sym_tab
|
|  Description:
|     user visible routine to do the symbol table walk
|     
\*--------------------------------------------------------------------------*/
void cus_walk_sym_tab (the_funct, the_thing)
int (*the_funct) ();
char *the_thing;

{
   for_all_2hashtable (cus_sym_tab-> the_table, the_funct, the_thing,
		       cus_internal_walk);
   return;
}					/* end cus_walk_sym_tab */


/*--------------------------------------------------------------------------*\
|  Routine: cus_hash_fcn
|
|  Description:
|     internal sym tab hash function
|     
\*--------------------------------------------------------------------------*/
static int cus_hash_fcn (name)
char* name;

{  register char *cp;
   register int i;
   int sh;

   cp = name;
   i = 0;
   while (*cp)
      i = i*2 + *cp++;
   sh = (i & 077777) % HASH_SIZE;
   return (sh);
}					/* end cus_hash_fcn */


/*--------------------------------------------------------------------------*\
|  Routine: cus_match_fcn
|
|  Description:
|     internal sym tab match function
|     
\*--------------------------------------------------------------------------*/
static int cus_match_fcn (candidate_name, bucket)
char* candidate_name;
cus_name_bucket_ptr bucket;

{
   return (strcmp (candidate_name, bucket-> name) == 0);
}					/* end cus_match_fcn */


/*--------------------------------------------------------------------------*\
|  Routine: cus_create_sym_tab
|
|  Description:
|     sym tab structure creater and initializer.  if it ever turns out more
|	than one sym tab is needed, this routine and most others will need
|	to have an additional parameter, which is a sym tab instead of using
|	the global cus_sym_tab.
|     
\*--------------------------------------------------------------------------*/
void cus_create_sym_tab (do_redefine_flag)
boolean do_redefine_flag;
{
   DEBUG2 (fprintf (stderr, "entering cus_create_sym_tab\n"))

    if (cus_sym_tab == NULLst)
	{
	cus_sym_tab = (cus_table_type*) make_node (sizeof (cus_table_type));
	DEBUG3 (fprintf (stderr, "cus_create_sym_tab: cus_sym_tab = %x\n",
			cus_sym_tab))
	cus_sym_tab->the_table = make_hashtable (HASH_SIZE, cus_hash_fcn,
					    cus_match_fcn);
	cus_sym_tab-> current_context = BASE_CONTEXT;
	cus_sym_tab-> tag = 0;
	cus_sym_tab-> old_contexts = make_list ();
	cus_do_redefine = do_redefine_flag;
	}

   return;
}					/* end cus_create_sym_tab */

/*--------------------------------------------------------------------------*\
|  Routine: cus_gensym
|
|  Description:
|     generate and inter a symbol with a unique name
|     
\*--------------------------------------------------------------------------*/
cus_sym cus_gensym (prefix, type)
string prefix;
cus_sym_type type;

{  int temp;

   return (cus_add_sym (cus_genname (prefix), type, &temp, NO_CONTEXT));
}					/* end cus_gensym */


/*--------------------------------------------------------------------------*\
|  Routine: cus_sym_context
|
|  Description:
|     return the context of the passed in symbol
|     
\*--------------------------------------------------------------------------*/
cus_context_type cus_sym_context (sym)
cus_sym sym;

{
   if (sym = cus_get_redefined (sym))
      return (sym-> context);
    else
      return (-1);
}					/* end cus_sym_context */


/*--------------------------------------------------------------------------*\
|  Routine: cus_sym_representation
|
|  Description:
|     return the print representation of a symbol
|     
\*--------------------------------------------------------------------------*/
char *cus_sym_representation (sym)
cus_sym sym;

{  caddr_t alt, temp;

   set_status (ok);
   if (sym = cus_get_redefined (sym))
     {temp = cus_int_get_prop (sym, alternate_print_string_prop);
     set_status (ok);			     /* mask any possible error	    */
     if (alt = temp)
         {return (alt);
         }
       else
         {if ((sym-> name) == NULL)
            return (cus_no_sym_message);
          else
            return (sym-> name);
         }
      }
    else
      return (cus_no_sym_message);
}					/* end cus_sym_representation */


/*--------------------------------------------------------------------------*\
|  Routine: cus_make_nu_name_bucket
|
|  Description:
|     internal routine which makes a nu sym tab bucket
|     
\*--------------------------------------------------------------------------*/
static cus_name_bucket_ptr cus_make_nu_name_bucket (name)
char *name;

{  cus_name_bucket_ptr a_bucket;

   a_bucket = (cus_name_bucket_ptr) make_node (sizeof (cus_name_bucket));
   a_bucket->name = make_node ((strlen (name) + 1));
   strcpy (a_bucket->name, name);
   a_bucket->sym = NULL;
   a_bucket->outer_contexts = make_list ();
   a_bucket->inner_contexts = make_list ();
   if (!add_unique_to_hashtable (cus_sym_tab->the_table, a_bucket, name))
      return (NULL);
   return (a_bucket);
}					/* end cus_make_nu_name_bucket */


/*--------------------------------------------------------------------------*\
|  Routine: cus_int_is_sym
|
|  Description:
|     internal version of routine which returns wheether a thing is a symbol.
|   	there is an internal version to allow bypassing the redefined shit
|	(which is not being used right now)
|     
\*--------------------------------------------------------------------------*/
static int cus_int_is_sym (sym)
cus_sym sym;

{
   if (sym)
      return ((sym-> ident == SYM_IDENTIFIER));
    else
      return (NULL);
}					/* end cus_int_is_sym */


/*--------------------------------------------------------------------------*\
|  Routine: cus_is_sym
|
|  Description:
|     returns whether the passed in thing is a symbol or not.
|     
\*--------------------------------------------------------------------------*/
int cus_is_sym (sym)
cus_sym sym;

{
   sym = (cus_sym) cus_get_redefined (sym);
   return (cus_int_is_sym (sym));
}					/* end cus_is_sym */


/*--------------------------------------------------------------------------*\
|  Routine: cus_prop_print_val
|
|  Description:
|     returns the print string corresponding to the passed in prop
|     
\*--------------------------------------------------------------------------*/
string cus_prop_print_val (property)
cus_property_type property;
{
   return (cus_property_type_print_value [(int) property]);
}					/* end cus_prop_print_val */


/*--------------------------------------------------------------------------*\
|  Routine: cus_type_print_val
|
|  Description:
|     returns the print string corresponding to the passed in type
|     
\*--------------------------------------------------------------------------*/
string cus_type_print_val (type)
cus_sym_type type;

{ 
   return (cus_symbol_type_print_value [(int) type]);
}					/* end cus_type_print_val */


/*--------------------------------------------------------------------------*\
|  Routine: cus_make_nu_sym_entry
|
|  Description:
|     internal routine to put a new symbol in the tab1
|     
\*--------------------------------------------------------------------------*/
static cus_sym cus_make_nu_sym_entry (type, the_bucket, worry_about_context)
cus_sym_type type;
cus_name_bucket_ptr the_bucket;
int worry_about_context;

{  cus_sym the_sym;

   the_sym = (cus_sym) make_node (sizeof (cus_sym_entry));
   the_sym->ident = SYM_IDENTIFIER;
   the_sym->name = the_bucket-> name;
   the_sym->bucket = the_bucket;
   the_sym->type = type;
   the_sym->redefined = (caddr_t) NULL;
   the_sym->template = (caddr_t) NULL;
   the_sym->current_type = (caddr_t) NULL;
   the_sym->current_value = (caddr_t) NULL;
   the_sym->original_name = (caddr_t) NULL;
   if (worry_about_context)
   	the_sym->context = cus_sym_tab->current_context;
    else
   	the_sym->context = (cus_context_type)  NO_CONTEXT_CONTEXT;
   the_sym->properties = make_list ();
   DEBUG3 (fprintf (stderr,"cus_make_nu_sym_entry:\n");
      fprintf (stderr,"\sym=%x,ident=%d,name=%s,type=%s,context=%d\n",
         the_sym,the_sym->ident,the_sym->name,
	 cus_type_print_val(the_sym->type),the_sym->context))
   return (the_sym);
}					/* end cus_make_nu_sym_entry */


/*--------------------------------------------------------------------------*\
|  Routine: cus_look_up
|
|  Description:
|     user visible routine to check and see if a string has been interred
|	in the sym tab
|     
\*--------------------------------------------------------------------------*/
cus_sym cus_look_up (name)
char* name;

{  cus_name_bucket_ptr the_bucket;
   cus_sym the_sym;

   if (cus_sym_tab == NULLst)
      FATAL_ERROR (fprintf (stderr, "no symbol table about\n"))

   the_sym = (cus_sym) NULL;
   the_bucket = (cus_name_bucket_ptr) get_from_hashtable
                                        (cus_sym_tab->the_table, name);
   DEBUG3 (fprintf (stderr,"cus_look_up: the_bucket = %x\n", the_bucket))
   if (the_bucket)
/*	this was originally here, but now i think it was not needed
	&&
       ((the_bucket-> sym-> context == NO_CONTEXT_CONTEXT) ||
        (the_bucket-> sym-> context == cus_sym_tab-> current_context)))
*/
   	the_sym = the_bucket-> sym;
   return (cus_get_redefined (the_sym));
}					/* end cus_look_up */


/*--------------------------------------------------------------------------*\
|  Routine: cus_add_sym
|
|  Description:
|     inter a string in the tab, creating new symbol if necessary
|     
\*--------------------------------------------------------------------------*/
cus_sym cus_add_sym (name, type, new, worry_about_context)
char* name;
cus_sym_type type;
int* new;
int worry_about_context;

{  cus_sym the_sym;
   cus_name_bucket_ptr the_bucket;

   DEBUG2 (fprintf (stderr,
		"entering cus_add_sym: name=%s,worry_about_context=%d\n",
                name, worry_about_context))

   *new = 0;

   if (cus_sym_tab == NULLst)
      FATAL_ERROR (fprintf (stderr, "no symbol table about\n"))

   the_bucket = (cus_name_bucket_ptr)get_from_hashtable
					(cus_sym_tab->the_table, name);
   if (!the_bucket)
      if (!(the_bucket = cus_make_nu_name_bucket (name)))
         {DEBUG3 (fprintf (stderr,"couldn't add bucket %s to symbol table\n",
			   name))
         return (NULL);
         }

   the_sym = the_bucket-> sym;
   if (!the_sym)
      {*new = 1;
      the_sym = cus_make_nu_sym_entry (type, the_bucket, worry_about_context);
      the_bucket-> sym = the_sym;
      }
   else
      if (worry_about_context &&
   	  (the_sym->context != (cus_context_type)  NO_CONTEXT_CONTEXT) &&
          (the_sym->context != cus_sym_tab->current_context))
         {*new = 1;
         add_to_head (the_bucket->outer_contexts, the_sym);
         the_sym = cus_make_nu_sym_entry (type, the_bucket,
					  worry_about_context);
         the_bucket-> sym = the_sym;
         }

   DEBUG2 (fprintf (stderr, "cus_add_sym exit, the_sym = %x\n", the_sym););
   return (the_sym);
}					/* end cus_add_sym */


/*--------------------------------------------------------------------------*\
|  Routine: cus_put_prop
|
|  Description:
|     user visible routine to place a new property value into a prop for a sym
|     
\*--------------------------------------------------------------------------*/
void cus_put_prop (sym, property_name, property_value)
cus_sym sym;
cus_property_type property_name;
char *property_value;

{  cus_priv_property_ptr prop_ptr;

   cus_prop_sets [(int) property_name] ++;

   sym = cus_get_redefined (sym);
   if (sym)
      {switch (property_name)
         {case type_prop:
            sym -> type = (cus_sym_type) property_value;
            break;
         case template_prop:
            sym -> template = (caddr_t) property_value;
            break;
         case original_name_prop:
            sym -> original_name = (caddr_t) property_value;
            break;
         case current_value_prop:
            sym -> current_value = (caddr_t) property_value;
            break;
         case current_type_prop:
            sym -> current_type = (caddr_t) property_value;
            break;
         case redefined_prop:
            if (cus_do_redefine)
               sym -> redefined = (caddr_t) property_value;
              else
					/* if someone asks for redefined_prop
					   and not enabled it is an error */
               FATAL_ERROR (fprintf (stderr,
				  "cus_put_prop got a redefined_prop\n"););
            break;
         default:
            {prop_ptr = (cus_priv_property_ptr )
				get_list_val (sym-> properties, property_name);
            if (!prop_ptr)
               {prop_ptr = (cus_priv_property_ptr)
      				make_node (sizeof (cus_priv_property));
                add_to_head_val (sym-> properties, prop_ptr, property_name);
               }
            prop_ptr -> the_value = property_value;
            }
         }
      DEBUG3 (fprintf (stderr,"new value for property %s = %x\n",
             cus_property_type_print_value [(int)property_name],
					   property_value););
      }
   return;
}					/* end cus_put_prop */


/*--------------------------------------------------------------------------*\
|  Routine: cus_get_prop
|
|  Description:
|     user visible routine to get the current value for a prop for a sym
|     
\*--------------------------------------------------------------------------*/
char *cus_get_prop (sym, property_name)
cus_sym sym;
cus_property_type property_name;

{  cus_sym test;

   return (cus_int_get_prop (cus_get_redefined (sym), property_name));

}					/* end cus_get_prop */


/*--------------------------------------------------------------------------*\
|  Routine: cus_print_property_list
|
|  Description:
|     user visible routine to print the property list for the specified
|	sym.  if the user passed in a prop value print routine, use it,
|	otherwise just the print thje prop names of all the props which the
|	sym has.
|     
\*--------------------------------------------------------------------------*/
void cus_print_property_list (stream, sym, indent, their_funct)
FILE *stream;
cus_sym sym;
string indent;
int (*their_funct) ();

{  LIST prop_list;

   DEBUG2 (fprintf (stderr, "entering cus_print_property_list\n"))

   sym = cus_get_redefined (sym);
   if (sym)
      prop_list =  LISThead (sym-> properties);
      {if (their_funct)
         {their_funct (stream, indent, sym, type_prop);
         if (sym-> redefined)
            their_funct (stream, indent, sym, redefined_prop);
         if (sym-> template)
            their_funct (stream, indent, sym, template_prop);
         if (sym-> current_type)
            their_funct (stream, indent, sym, current_type_prop);
         if (sym-> current_value)
            their_funct (stream, indent, sym, current_value_prop);
         if (sym-> original_name)
            their_funct (stream, indent, sym, original_name_prop);
         while (prop_list != NULL)
            {their_funct (stream, indent, sym, (int) LISTvalue (prop_list));
            prop_list = LISThead (prop_list);
            }
         }
       else
         {fprintf (stream, "%s%s = %s\n", indent,
   		   cus_property_type_print_value [(int) type_prop],
   	 	   cus_type_print_val (sym -> type));
         if (sym -> redefined)
            fprintf (stream, "%s%s = %s\n", indent,
   		   cus_property_type_print_value [(int) redefined_prop],
   	 	   cus_type_print_val (sym -> redefined));
         if (sym -> template)
            fprintf (stream, "%s%s = %s\n", indent,
   		   cus_property_type_print_value [(int) template_prop],
   	 	   cus_type_print_val (sym -> template));
         if (sym -> current_type)
            fprintf (stream, "%s%s = %s\n", indent,
   		   cus_property_type_print_value [(int) current_type_prop],
   	 	   cus_type_print_val (sym -> current_type));
         if (sym -> current_value)
            fprintf (stream, "%s%s = %s\n", indent,
   		   cus_property_type_print_value [(int) current_value_prop],
   	 	   cus_type_print_val (sym -> current_value));
         if (sym -> original_name)
            fprintf (stream, "%s%s = %s\n", indent,
   		   cus_property_type_print_value [(int) original_name_prop],
   	 	   cus_type_print_val (sym -> original_name));
         while (prop_list != NULL)
            {fprintf (stream, "%s%s\n",
   		      indent,
                      cus_property_type_print_value
				[(int)LISTvalue (prop_list)]);
            prop_list = LISThead (prop_list);
            }
         }
      }
   return;
}					/* end cus_print_property_list */


/*--------------------------------------------------------------------------*\
|  Routine: cus_delete_sym
|
|  Description:
|     delete the passed in sym from the tab
|     
\*--------------------------------------------------------------------------*/
/*
??? need to make cus_delete_sym "work", make sure "works" w/redefined_prop. if
    we ever use it.
*/
boolean cus_delete_sym (sym)
cus_sym sym;

{
   return (1);
}					/* end cus_delete_sym */


/*--------------------------------------------------------------------------*\
|  Routine: cus_string_constant
|
|  Description:
|     inter passed in string and make a string constant, perhaps this should
|     not be in the sym tab set of routines, but so it so goes.
|     
\*--------------------------------------------------------------------------*/
cus_sym cus_string_constant (the_string)
string the_string;

{  cus_sym the_sym;

   the_sym = cus_gensym ("string", constant_sym);
   cus_put_prop (the_sym, is_constant_prop, 1);
   cus_put_prop (the_sym, current_type_prop, idd_string);
   cus_put_prop (the_sym, current_value_prop, the_string);
   return (the_sym);
}						/* end cus_make_string_sym */


/*--------------------------------------------------------------------------*\
|  Routine: cus_print_prop_nums
|
|  Description:
|     prints the property set/use info
|     
\*--------------------------------------------------------------------------*/
void cus_print_prop_nums (stream, indent)
{  int i;

   fprintf (stream, "\n\n%s PROPERTY USAGE\n", indent);
   fprintf (stream, "%s\t\tSET\tUSE\n", indent);
   for (i = 0; i < (int) last_prop_DO_NOT_USE; i++)
      {fprintf (stream, "%s%s\t%d\t%d\n", indent, cus_prop_print_val (i),
		cus_prop_sets [i], cus_prop_gets [i]);
      }
   fprintf (stream, "\n\n");

   return;
}					/* end cus_print_prop_nums */


