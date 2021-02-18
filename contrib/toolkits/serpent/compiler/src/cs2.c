static char rcsid [] = "$Header: /u/04c/mnt/integ/compiler/src/RCS/cs2.c,v 1.27 89/10/23 20:01:20 ejh Exp $";

/*
$Log:	cs2.c,v $
 * Revision 1.27  89/10/23  20:01:20  ejh
 * declared fopen; renamed generated files; simplified make_full_name
 * 
 * Revision 1.26  89/10/08  00:45:07  serpent
 * short names: u22_runtime.h => u22_rt.h
 * 
 * Revision 1.25  89/08/29  17:30:00  little
 * change god to eve
 * 
 * Revision 1.24  89/08/23  11:21:48  bmc
 * Fixed erroneous printf statement.
 * 
 * Revision 1.23  89/08/02  13:12:53  little
 * fix up some debug stuff in the generated code for dgr_run_snippet
 * 
 * Revision 1.22  89/08/01  14:29:12  little
 * fix the location of the debug output on the cs2_c_snip_stream
 * 
 * Revision 1.21  89/07/24  14:52:55  little
 * ops removal
 * 
 * Revision 1.20  89/05/22  17:22:36  bmc
 * Changed definition of dgr_run_snippet (generated code) due to changes
 * in DM run-time routines.
 * 
 * Revision 1.19  89/05/19  10:43:27  little
 * change generated c files to use the "fast" header
 * 
 * Revision 1.18  89/04/14  11:21:27  little
 * add open & close of cs2_ops_find2_stream, which is used to hold
 * find rules for vcs and vars.
 * 
 * Revision 1.17  89/04/07  17:50:47  ljb
 * restore shared data information from vc
 * 
 * Revision 1.16  89/04/03  13:54:14  little
 * no changes
 * 
 * Revision 1.15  89/03/28  12:55:25  serpent
 * changed  #ifdef 0  to  #if 0  to allow sun compile.
 * 
 * Revision 1.14  89/03/15  17:29:20  little
 * add the find & remove ops files.
 * modify the include file names which appear in the output c files.
 * 
 * Revision 1.13  89/03/15  13:57:11  rcs
 * no changes
 * 
 * Revision 1.12  89/02/27  17:06:07  little
 * cahnge csc_is_sd from being defined as static, so outside routines can
 * call it. change fatal_error for call to new and no bound sd into
 * being a compile error.
 * 
 * Revision 1.11  89/02/20  16:20:33  little
 * no changes
 * 
 * Revision 1.10  89/02/15  17:36:44  little
 * make output file (c & ops) names base on the dialogue name
 * 
 * Revision 1.9  89/02/03  15:58:37  little
 * change to put out correct info on the serpent "exec" file when there
 * is not a dm ill file
 * 
 * Revision 1.8  89/01/09  16:29:40  little
 * add some debug print
 * 
 * Revision 1.7  88/12/19  17:29:15  little
 * take out the write of dro_more.h to the oputput c files.
 * 
 * Revision 1.6  88/10/31  15:12:39  little
 * add call to cam_fix_atts in cs2_object.
 * 
 * Revision 1.5  88/10/24  17:04:42  little
 * change methods to not be handled by ops.  do not need to do any ops
 * code generation for methods.
 * 
 * Revision 1.4  88/10/04  17:32:20  little
 * add dialogue_name parameter to a couple of functions
 * 
 * Revision 1.3  88/10/03  09:00:08  little
 * add processing for dm shared data
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

#define cs2_top
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
#include "u22.h"

#define NON_EXISTANT_DM_ILL_FILE_NAME "HELLO_JACK"

/*--------------------------------------------------------------------------*\
|  Routine: cs2_fflush
|
|  Description:
|     flushes the input stream
\*--------------------------------------------------------------------------*/
void cs2_fflush (stream)
FILE *stream;
{
   set_status (ok);
   fflush (stream);
   return;
}					/* end cs2_fflush */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_cc_gen
|
|  Description:
|     generates code for the creation cond associated with the input vc
\*--------------------------------------------------------------------------*/
void cs2_cc_gen (vc)
cus_sym vc;
{
   set_status (ok);
   cam_fix_cc (vc);
   csc_cc_write (vc);
   csd_cc_write (vc);
   return;
}					/* end cs2_cc_gen */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_vars_gen
|
|  Description:
|     generates code for the vars associated with the input vc
\*--------------------------------------------------------------------------*/
int cs2_vars_gen (vc)
cus_sym vc;
{
   char *value, *var;
   set_status (ok);
   if (value = cus_get_prop (vc, variables_prop))
      {loop_through_list ((LIST) value, var, char *)
	{csc_var_write (var);
	csd_var_write (var);
	}
      }
   return;
}					/* end cs2_vars_gen */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_objs_gen
|
|  Description:
|     generates code for the objs associated with the input vc
\*--------------------------------------------------------------------------*/
int cs2_objs_gen (vc)
cus_sym vc;
{  char *obj, *att;
   LIST obj_list, att_list;

   set_status (ok);
   DEBUG2 (fprintf (stderr, "cs2_objs_gen: vc = %s\n",
		    cus_sym_representation (vc)););
   if (obj_list = (LIST) cus_get_prop (vc, objects_prop))
      {DEBUG3 (fprintf (stderr, "\t# objects = %d\n",
	list_length (obj_list)););
      obj_list = copy_list (obj_list, NOT_NODES);
      loop_through_list (obj_list, obj, char *)
         {cus_activate_context (cus_get_prop (obj, owned_context_prop));
         DEBUG3 (fprintf (stderr, "cs2_objs_gen: processing object = %s\n",
 	    cus_sym_representation (obj)););
         cam_fix_atts (obj);
         if (att_list = (LIST) cus_get_prop (obj, attributes_prop))
            {loop_through_list (att_list, att, char *)
               {csc_att_write (att);
               csd_att_write (att);
               }
            }
         if (cus_get_prop (obj, methods_prop))
            {cam_fix_methods (obj);
            csc_meth_write (obj);
            }
         csd_obj_write (obj);
         cus_deactivate_context ();
         }
      }
   return;
}					/* end cs2_objs_gen */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_vc_gen
|
|  Description:
|     generates code for the input vc
\*--------------------------------------------------------------------------*/
int cs2_vc_gen (vc)
cus_sym vc;
{  char *value, *sub_vc;
   char *parm;

   set_status (ok);
	/* cc is in vc's "parents's" context */
   cs2_cc_gen (vc);
   DEBUG3 (fprintf (stderr, "before activate vc name = %s\n",
	cus_sym_representation (vc)))
   cus_activate_context (cus_get_prop (vc, owned_context_prop));
   DEBUG3 (fprintf (stderr, "after activate vc name = %s\n",
	cus_sym_representation (vc)))

   cs2_vars_gen (vc);
   cs2_objs_gen (vc);
   csc_action_write ("create", vc);
   csd_action_write ("create", vc);
   csc_action_write ("destroy", vc);
   csd_action_write ("destroy", vc);
   if (value = cus_get_prop (vc, sub_vcs_prop))
      {loop_through_list ((LIST) value, sub_vc, char *)
         {cs2_vc_gen ((cus_sym) sub_vc);
         }
      }
   csd_vc_write (vc);
/*
   Now we restore properties which may have been overwritten by the
   creation condition
*/
   if ((parm = cus_get_prop (vc, creating_sd_prop)) && csc_is_sd (parm))
      {
      cus_put_prop (parm, created_vcs_prop,
	cus_get_prop (vc,old_created_vcs_prop));
      cus_put_prop (parm, parent_vc_prop,
	cus_get_prop (vc,old_parent_vc_prop));
      cus_put_prop (parm, owned_context_prop,
	cus_get_prop (vc, old_owned_context_prop));
      }
   cus_deactivate_context ();
   return;
}					/* end cs2_vc_gen */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_open_output_file
|
|  Description:
|     open a file on which to put generated code
\*--------------------------------------------------------------------------*/
FILE *cs2_open_output_file (file_name)
string file_name;
{  FILE *fopen (), *stream;

   set_status (ok);
   stream = fopen (file_name, "w");
   if (!stream)
      FATAL_ERROR (fprintf (stderr, "cannot open file %s\n", file_name))
   return (stream);
}					/* end cs2_open_output_file */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_open_c_file
|
|  Description:
|     open a file on which to put generated c code
\*--------------------------------------------------------------------------*/
FILE *cs2_open_c_file (name)
string name;
{  FILE *stream;

   stream = cs2_open_output_file (name);
   fprintf (stream, "#include \"u22_rt.h\"\n");
   fprintf (stream, "\n");
   cs2_fflush (stream);
   return (stream);
}					/* end cs2_open_c_file */

#define make_full_name(ext) \
   strcat (strcat (strcpy (full_name, "."), dialogue_name), "ext")

/*--------------------------------------------------------------------------*\
|  Routine: cs2_initialize_output_files
|
|  Description:
|     initialize the files on which generated code is placed
\*--------------------------------------------------------------------------*/
void cs2_initialize_output_files (dialogue_name)
string dialogue_name;
{  char full_name [1024];

   set_status (ok);
   cs2_c_snip_stream = cs2_open_c_file (make_full_name (s.c));
   fprintf (cs2_c_snip_stream, "#include \"%s\"\n\n", make_full_name (1.h));
   fprintf (cs2_c_snip_stream, "void dgr_run_snippet (sne)\n");
   fprintf (cs2_c_snip_stream, "dgr_sne *sne;\n");
   fprintf (cs2_c_snip_stream, "{set_status (ok);\n\n");
   if (c22_output_debug_code)
      {fprintf (cs2_c_snip_stream,
	"DEBUG2 (fprintf (stderr, \"dgr_run_snippet:");
      fprintf (cs2_c_snip_stream, " sne = %%x, routine = %%d\\n\",\n");
      fprintf (cs2_c_snip_stream, "\tsne, sne-> routine); );\n\n");
      }
   fprintf (cs2_c_snip_stream, "if ((sne-> routine > NO_GENERIC_ROUTINE) &&\n");
   fprintf (cs2_c_snip_stream, "   (sne-> routine < d22_max_snippets) &&\n");
   fprintf (cs2_c_snip_stream, "    dob_validate_object (sne-> parm_1))\n");
   fprintf (cs2_c_snip_stream,
	    "      dgr_routine_array [sne-> routine] (sne);\n\n");
   fprintf (cs2_c_snip_stream, "return;\n}\n\n");

   cs2_c_pet_stream = cs2_open_c_file (make_full_name (p.c));
   cs2_c_att_stream = cs2_open_c_file (make_full_name (a.c));
   cs2_c_var_stream = cs2_open_c_file (make_full_name (v.c));
   cs2_c_meth_stream = cs2_open_c_file (make_full_name (m.c));

	/* creation & destructions are now written to the same file.  we are
	   keeping the "other" variable around to make it easier to go back
	   to two files if the need arises. */

   cs2_c_cact_stream = cs2_open_c_file (make_full_name (c.c));
   cs2_c_dact_stream = cs2_c_cact_stream;
#if  0
   cs2_c_dact_stream = cs2_open_c_file (make_full_name (d.c));
#endif

   cs2_c_proc_header0_stream = cs2_open_output_file (make_full_name (0.h));
   fprintf (cs2_c_proc_header0_stream,
     "static char DM_NAME [] = \"DIALOGUE_NAME = %s\";\n\n", dialogue_name);
   fprintf (cs2_c_proc_header0_stream, "char * din_get_dm_name ()\n");
   fprintf (cs2_c_proc_header0_stream, "{return (\"%s\");}\n\n",
	dialogue_name);

   cs2_c_proc_header1_stream = cs2_open_output_file (make_full_name (1.h));
   fprintf (cs2_c_proc_header1_stream, "#include \"%s\"\n\n",
	make_full_name (0.h));
   fprintf (cs2_c_proc_header1_stream,
	"/* example use: dgr_routine_array [5] (parameters) */\n");
   fprintf (cs2_c_proc_header1_stream, "void (*dgr_routine_array []) () =\n");
   fprintf (cs2_c_proc_header1_stream, "{\n");
   cs2_fflush (cs2_c_proc_header1_stream);

   cs2_symtab_stream = cs2_open_output_file (make_full_name (t.d));
   cs2_dm_init_stream = cs2_open_output_file (make_full_name (i.d));
   c22_got_dm_box = false;
   cs2_dm_exec_stream = cs2_open_output_file (make_full_name (e.d));

   return;
}					/* end cs2_initialize_output_files */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_cleanup_output_files
|
|  Description:
|     add termination infor and close the files on which generated code is
|	placed
\*--------------------------------------------------------------------------*/
void cs2_cleanup_output_files (dialogue_name)
string dialogue_name;
{
   set_status (ok);
   fclose (cs2_c_snip_stream);
   fclose (cs2_c_pet_stream);
   fclose (cs2_c_att_stream);
   fclose (cs2_c_var_stream);
   fclose (cs2_c_meth_stream);

	/* creations & destruction actions are now written on a single file.
	   see the open code for more info. */

   fclose (cs2_c_cact_stream);
#if  0
   fclose (cs2_c_dact_stream);
#endif

	/* dgr_dummy_snippet is added because each "real" entry is added w/a
	   trailing comma, so we need a final entry to "finish" the list.
	   since it will never be used, it does not matter what routine it is.
	*/

   fprintf (cs2_c_proc_header0_stream, "void dgr_dummy_snippet ();\n");
   fclose (cs2_c_proc_header0_stream);
   fprintf (cs2_c_proc_header1_stream, "\tdgr_dummy_snippet\n");
   fprintf (cs2_c_proc_header1_stream, "};\n\n");
   fprintf (cs2_c_proc_header1_stream, "int d22_max_snippets = %d;\n\n",
	cs2_current_c_proc_number); 
   fclose (cs2_c_proc_header1_stream);
   fclose (cs2_symtab_stream);

   if (!c22_got_dm_box)
      {fprintf (cs2_dm_init_stream, "%s %s\n", DM_BOX_NAME,
	NON_EXISTANT_DM_ILL_FILE_NAME);
      fprintf (cs2_dm_exec_stream, "%s_EXE %s\n",
	u22_convert_string (dialogue_name, UPPERCASE), dialogue_name);
      }

   fclose (cs2_dm_init_stream);
   fclose (cs2_dm_exec_stream);
   return;
}					/* end cs2_cleanup_output_files */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_cleanup
|
|  Description:
|     wrapup the the code generation phase
\*--------------------------------------------------------------------------*/
void cs2_cleanup (dialogue_name)
string dialogue_name;
{
   set_status (ok);
   cs2_cleanup_output_files (dialogue_name);
   return;
}					/* end cs2_cleanup */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_initialize
|
|  Description:
|     initialize the code generation phase
\*--------------------------------------------------------------------------*/
void cs2_initialize (dialogue_name)
string dialogue_name;
{
   set_status (ok);
   cs2_initialize_output_files (dialogue_name);
   cs2_current_c_proc_number = 0;
   return;
}					/* end cs2_initialize */

/*--------------------------------------------------------------------------*\
|  Routine: cs2_gen
|
|  Description:
|     perfrom the code generation phase
\*--------------------------------------------------------------------------*/
void cs2_gen (dialogue_name)
string dialogue_name;
{  cus_sym god_vc;

   set_status (ok);
   cs2_initialize (dialogue_name);
   god_vc = cus_look_up (c22_god_text);
   if (! god_vc)
      {DEBUG1 (fprintf (stderr, "cs2_gen CANNOT FIND HONCHO VC NAMED %s\n",
	c22_god_text))
      return;
      }
   cs2_vc_gen (god_vc);
   csd_dm_rt_write (dialogue_name);
   csd_ill_files_write ();
   cs2_cleanup (dialogue_name);
   return;
}					/* end cs2_gen */
