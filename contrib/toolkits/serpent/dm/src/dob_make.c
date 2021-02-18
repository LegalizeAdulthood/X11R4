static char rcsid [] =
        "$Header: /u/04c/mnt/integ/dm/src/RCS/dob_make.c,v 1.9 89/05/25 17:37:09 little Exp $";

/*
$Log:	dob_make.c,v $
 * Revision 1.9  89/05/25  17:37:09  little
 * rearrange include order becasue of changes to d22_sne int d22.h
 * 
 * Revision 1.8  89/05/19  17:20:47  bmc
 * Adjusted to accomodate new definition of 'dro_atomic_data_item'.
 * See 'dro.h' for more information.
 * 
 * Revision 1.7  89/02/09  17:38:24  ejh
 * added copyright notice
 * 
 * Revision 1.6  89/01/24  16:58:54  rcs
 * added dob_make_id_constant_adi routine
 * 
 * Revision 1.4  88/12/21  19:17:48  rcs
 * fixed dob_make_real_constant_adi to create a real contant
 * and not a string
 * 
 * Revision 1.3  88/12/14  11:08:03  little
 * add dob_make_undefined_adi routine
 * 
 * Revision 1.2  88/10/07  15:40:14  little
 * not modified.
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

#include <stdio.h>
#include <strings.h>
#define memoryPack              /* use memoryPack rather than mallocPack    */
#include "listPack.h"
#include "isc.h"
#include "dob.h"
#include "dro.h"
/*
??? combine dob_make c & h with dob c & h
*/
#include "dob_make.h"
#include "dgr.h"
#include "d22.h"



/* --------------------------------------------------- dob_make_constant_adi */
dro_atomic_data_item dob_make_string_constant_adi (value)
string value;
{  dro_atomic_data_item adi;

   DEBUG2 (fprintf (stderr, "dob_make_string_constant_adi enter: value = %s\n",
	value);)

   adi = dob_make_temp_adi ();
   dob_variable_tell (adi.dob_handle, idd_string, value);
   DEBUG2 (fprintf (stderr, "dob_make_string_constant_adi exit: handle = %#x\n",
      adi.dob_handle);)

   return (adi);
}


/* ------------------------------------------ dob_make_boolean_constant_adi */
dro_atomic_data_item dob_make_boolean_constant_adi (value)
int value;
{  dro_atomic_data_item adi;

   DEBUG2 (fprintf(stderr, "dob_make_boolean_constant_adi enter: value = %d\n",
	value);)
   adi = dob_make_temp_adi ();
   dob_variable_tell (adi.dob_handle, idd_boolean, (char *) value);
    DEBUG2 (fprintf (stderr,
      "dob_make_boolean_constant_adi exit, handle = %#x\n", adi.dob_handle);
	fprintf (stderr, "component_name = %s\n", adi.component_name);)
   return (adi);
}


/* --------------------------------------------------- dob_make_integer_adi */
dro_atomic_data_item dob_make_integer_constant_adi (value)
int value;
{  dro_atomic_data_item adi;

   DEBUG2(fprintf(stderr, "dob_make_integer_constant_adi enter: value = %d\n",
	value);)

   adi = dob_make_temp_adi ();
   dob_variable_tell (adi.dob_handle, idd_integer, (char *) value);
   DEBUG2 (fprintf (stderr,
      "dob_make_integer_constant_adi exit, handle = %#x\n", adi.dob_handle);)

   return (adi);
}


/* -----------------------------------------dob_make_id_constant_adi */
dro_atomic_data_item dob_make_id_constant_adi (value)
iid_id_type value;
{  dro_atomic_data_item adi;

   DEBUG2 (fprintf(stderr, "dob_make_id_constant_adi enter: value = %d\n",
	value);)

   adi = dob_make_temp_adi ();
   dob_variable_tell (adi.dob_handle, idd_id, (char *) value);
   DEBUG2 (fprintf (stderr, "dob_make_id_constant_adi exit, handle = %#x\n",
      adi.dob_handle);)

   return (adi);
}


/* --------------------------------------------------- dob_make_real_adi */
dro_atomic_data_item dob_make_real_constant_adi (value)
double value;
{  dro_atomic_data_item adi;
   char *temp;

   DEBUG2 (fprintf (stderr, "dob_make_real_constant_adi enter: value = %f\n",
	value);)
   adi = dob_make_temp_adi ();
   temp = make_node (sizeof (double));
   *((double *) temp) = value;
   dob_variable_tell (adi.dob_handle, idd_real, temp);
   DEBUG2 (fprintf (stderr, "dob_make_real_constant_adi exit, handle = %#x\n",
      adi.dob_handle);)
   return (adi);
}


/* --------------------------------------------------- dob_make_temp_adi */
dro_atomic_data_item dob_make_temp_adi ()
{  dro_atomic_data_item ret_val;
   dob_data_object handle;

   DEBUG2 (fprintf (stderr, "dob_make_temp_adi enter\n");)
   handle = dob_temp_create ();
   if (get_status ())
      FATAL_ERROR (isc_print_status (
		"dob_make_temp_adi: problems with dob_temp_create status = ");)
   ret_val.dob_handle = handle;
   ret_val.component_name =  NULL;
   DEBUG2 (fprintf (stderr, "dob_make_temp_adi exit, handle = %#x\n",
      handle);)
   return (ret_val);
}


/* --------------------------------------------------- dob_make_undefined_adi */
dro_atomic_data_item dob_make_undefined_adi ()
{  dro_atomic_data_item ret_val;
   dob_data_object handle;

   DEBUG2 (fprintf (stderr, "dob_make_undefined_adi enter\n"););
				/* by temps are initailly undefined */
   ret_val = dob_make_temp_adi ();
   DEBUG2 (fprintf (stderr, "dob_make_undefined_adi exit, handle = %#x\n",
      ret_val.dob_handle););
   return (ret_val);
}


/* --------------------------------------------------- dob_make_adi */
dro_atomic_data_item dob_make_adi (the_handle, the_name)
dob_data_object the_handle;
string the_name;
{  dro_atomic_data_item ret_val;

   DEBUG2 (fprintf (stderr, "dob_make_adi enter\n");
	fprintf (stderr, "the_handle = %#x, component_name = %s\n",
	the_handle, the_name);)
   ret_val.dob_handle = the_handle;
   ret_val.component_name = the_name;
   DEBUG2 (fprintf (stderr, "dob_make_adi exit, handle = %#x\n",
      ret_val.dob_handle);)
   return (ret_val);
}


