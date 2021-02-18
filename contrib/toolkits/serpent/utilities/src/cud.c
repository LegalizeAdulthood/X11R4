static char rcsid [] =
	"$Header: /u/04c/mnt/new_integ/utilities/src/RCS/cud.c,v 1.1 89/10/08 00:11:09 serpent Exp $";

/*
$Log:	cud.c,v $
 * Revision 1.1  89/10/08  00:11:09  serpent
 * Initial revision
 * 
 * Revision 1.5  89/02/26  10:41:49  little
 * add the symbol printout to sym_write
 * 
 * Revision 1.4  89/02/22  16:27:17  little
 * 
 * no changes
 * 
 * Revision 1.3  89/02/10  14:46:45  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/12/21  11:50:55  little
 * take out unused comment type props
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
#include "cus.h"
#include "cud.h"
#include "idd.h"

void cud_print_symbol (stream, sym, indent)
FILE *stream;
cus_sym sym;
char *indent;

{  double *real_pointer;

   if (sym)
      {fprintf (stream,
		"%ssymbol - %s, %x", indent, cus_sym_representation (sym),sym);
      if (cus_get_prop (sym, is_constant_prop))
         {switch (cus_get_prop (sym, current_type_prop))
            {case idd_integer:
               fprintf (stream, " - integer constant, value = %d",
			cus_get_prop (sym, current_value_prop));
               break;
            case idd_real:
               real_pointer = (double *)cus_get_prop (sym, current_value_prop);
               fprintf (stream, " - real constant, value = %f", *real_pointer);
               break;
            case idd_string:
               fprintf (stream, " - string constant, value = %s",
			cus_get_prop (sym, current_value_prop));
               break;
            case idd_boolean:
               fprintf (stream, " - boolean constant, value = %d",
			cus_get_prop (sym, current_value_prop));
               break;
            default:
               fprintf (stream,
		 "UNKNOWN TYPE OF %d", cus_get_prop (sym, current_type_prop));
               break;
            }
         }
      }
   fprintf (stream, "\n");
   fflush (stream);
   return;
}


void cud_print_tree (stream, the_tree, indent)
FILE *stream;
LIST the_tree;
char *indent;

{  char *temp;
   char nu_indent [256];
   int first;

   if (the_tree)
      {strcat (strcpy (nu_indent, indent), "   ");
      if (cus_is_sym (the_tree))
         cud_print_symbol (stream, the_tree, nu_indent);
       else
         {fprintf (stream,"%s(\n", indent);
         first = TRUE;
         loop_through_list (the_tree, temp, char *)
            {if (cus_is_sym (temp))
                  cud_print_symbol (stream, temp, nu_indent);
             else
               cud_print_tree (stream, temp, nu_indent);
            }
         fprintf (stream, "%s)\n", indent);
         }
      }
   fflush (stream);
   return;
}


void cud_print_variable (stream, variable)
FILE *stream;
cus_sym variable;
{  char *value;

   fprintf (stream, "~~~~~~~~~~~~~~~~~~~~~~     variable %s  ~~~~~~~~~~~~~\n",
	cus_sym_representation (variable));
   fprintf (stream, "name:\t");
   fprintf (stream, "%s\n", cus_get_prop (variable, u_name_prop));
   fflush (stream);

   if (value = cus_get_prop (variable, parent_vc_prop))
      {fprintf (stream, "parent:\t");
      cud_print_symbol (stream, value, "");
      }

   if (value =  cus_get_prop (variable, variable_comments_prop))
      {fprintf (stream, "comments:\n");
      cud_print_tree (stream, value, "\t");
      }

   if (value = cus_get_prop (variable, initial_value_snippet_prop))
      {fprintf (stream, "snippet:\n");
      cud_print_tree (stream, value, "\t");
      }
   fprintf (stream, "~~~~~~~~~~~~~~~~~~~   end  %s     ~~~~~~~~~~~~~~~~~~~~\n",
	cus_sym_representation (variable));
   fflush (stream);
   return;
}


void cud_print_method (stream, method)
FILE *stream;
cus_sym method;
{  char *value;

   fprintf (stream, "^^^^^^^^^^^^^^^^^^^^^^     method %s     ^^^^^^^^^^^^^\n",
	cus_sym_representation (method));
   fprintf (stream, "name:\t");
   fprintf (stream, "%s\n", cus_get_prop (method, u_name_prop));

   if (value = cus_get_prop (method, parent_object_prop))
      {fprintf (stream, "parent:\t");
      cud_print_symbol (stream, value, "");
      }

   if (value = cus_get_prop (method, value_snippet_prop))
      {fprintf (stream, "value snippet:\n");
      cud_print_tree (stream, value, "\t");
      }

   fprintf (stream, "^^^^^^^^^^^^^^^^^^^  end   %s     ^^^^^^^^^^^^^^^^^^^^\n",
	cus_sym_representation (method));

   fflush (stream);
   return;
}


void cud_print_attribute (stream, attribute)
FILE *stream;
cus_sym attribute;
{  char *value;

   fprintf (stream, "----------------------     attribute %s     ----------\n",
	cus_sym_representation (attribute));
   fprintf (stream, "name:\t");
   fprintf (stream, "%s\n", cus_get_prop (attribute, u_name_prop));

   if (value = cus_get_prop (attribute, parent_object_prop))
      {fprintf (stream, "parent:\t");
      cud_print_symbol (stream, value, "");
      }

   if (value = cus_get_prop (attribute, value_snippet_prop))
      {fprintf (stream, "value snippet:\n");
      cud_print_tree (stream, value, "\t");
      }

   if (value = cus_get_prop (attribute, modified_snippet_prop))
      {fprintf (stream, "modified snippet:\n");
      cud_print_tree (stream, value, "\t");
      }

   fprintf (stream, "-------------------  end   %s     --------------------\n",
	cus_sym_representation (attribute));

   fflush (stream);
   return;
}


/* ------------------------------------------   cus_print_data_definition */
void cus_print_data_definition (stream, def)
char *def;
{

/*	?? fix up to print the correct things, when if the internal
	form of a data_definition & assorted structures
	settles down.
*/

   fflush (stream);
   return;
}


/* -----------------------------------------------   cud_print_object */
void cud_print_object (stream, object) 
FILE *stream;
cus_sym object;

{  char *attribute;
   char *value;

   fprintf (stream, "======================     object %s     =============\n",
	cus_sym_representation (object));

   fprintf (stream, "context & name:\t");
   fprintf (stream, "%d, ", cus_get_prop (object, owned_context_prop));
   fprintf (stream, "%s\n", cus_get_prop (object, u_name_prop));

   if (value = cus_get_prop (object, parent_vc_prop))
      {fprintf (stream, "parent:\t");
      cud_print_symbol (stream, value, "");
      }

   if (value = cus_get_prop (object, object_comments_prop))
      {fprintf (stream, "object comments:\n");
      cud_print_tree (stream, value, "\t");
      }

   if (value = cus_get_prop (object, data_definition_prop))
      cus_print_data_definition (stream, value);

   if (value = cus_get_prop (object, object_type_prop))
      {fprintf (stream, "type:\t");
      cud_print_symbol (stream, value, "");
      }

   if (value = cus_get_prop (object, attributes_prop))
      {loop_through_list ((LIST) cus_get_prop (object, attributes_prop),
		         attribute, char *)
         {cud_print_attribute (stream, attribute);
         }
      }

   if (value = cus_get_prop (object, methods_prop))
      {loop_through_list ((LIST) cus_get_prop (object, methods_prop),
		         attribute, char *)
         {cud_print_method (stream, attribute);
         }
      }

   fprintf (stream, "===================  end   %s     ====================\n",
	cus_sym_representation (object));

   fflush (stream);
   return;
}


void cud_print_vc (stream, vc)
FILE *stream;
cus_sym vc;

{  char *object;
   char *sub_vc;
   char *value;

   fprintf (stream,
	"++++++++++++++++++++++     %s     ++++++++++++++++++++\n",
	cus_sym_representation (vc));
   fprintf (stream,
	"++++++++++++++++++++++     %s     ++++++++++++++++++++\n\n",
	cus_sym_representation (vc));

   fprintf (stream, "context & name:\t");
   fprintf (stream, "%d, ", cus_get_prop (vc, owned_context_prop));
   fprintf (stream, "%s\n", cus_get_prop (vc, u_name_prop));

   if (value = cus_get_prop (vc, parent_vc_prop))
      {fprintf (stream, "parent:\t");
      cud_print_symbol (stream, value, "");
      }

   if (value = cus_get_prop (vc, vc_comments_prop))
      {fprintf (stream, "comment:\n");
      cud_print_tree (stream, value, "\t");
      }

   if (value = cus_get_prop (vc, cc_used_vars_prop))
      {fprintf (stream, "cc used vars:\n");
      cud_print_tree (stream, value, "\t");
      }

   if (value = cus_get_prop (vc, creation_condition_prop))
      {fprintf (stream, "creation condition:\n");
      cud_print_tree (stream, value, "\t");
      }

   if (value = cus_get_prop (vc, variables_prop))
      {loop_through_list ((LIST) value, object, char *)
         {cud_print_variable (stream, object);
         }
      }

   if (value = cus_get_prop (vc, objects_prop))
      {loop_through_list ((LIST) value, object, char *)
         {cud_print_object (stream, object);
         }
      }

   if (value = cus_get_prop (vc, create_actions_snippet_prop))
      {fprintf (stream, "create actions snippet:\n");
      cud_print_tree (stream, value, "\t");
      }

   if (value = cus_get_prop (vc, destroy_actions_snippet_prop))
      {fprintf (stream, "destroy actions snippet:\n");
      cud_print_tree (stream, value, "\t");
      }

   if (value = cus_get_prop (vc, sub_vcs_prop))
      {loop_through_list ((LIST) value, sub_vc, char *)
         {cud_print_vc (stream, sub_vc);
         }
      }

   fprintf (stream,
	"+++++++++++++++++++  end   %s     ++++++++++++++++++++\n",
	cus_sym_representation (vc));
   fprintf (stream, "\n\n");

   fflush (stream);
   return;
}

