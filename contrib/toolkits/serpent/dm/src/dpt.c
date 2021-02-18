static char rcsid [] =
	"$Header: /u/04c/mnt/integ/dm/src/RCS/dpt.c,v 1.7 89/05/25 17:36:39 little Exp $";

/*
$Log:	dpt.c,v $
 * Revision 1.7  89/05/25  17:36:39  little
 * rearrange include order becasue of changes to d22_sne int d22.h
 * 
 * Revision 1.6  89/05/19  17:22:26  bmc
 * Changed to accomodate modifications to 'dob.h'.  In particular, no longer
 * passes 'ids' to dob routines.  See 'dob.c', 'dob.h' and 'dpt.h' for more
 * information.
 * 
 * Revision 1.5  89/02/09  17:40:14  ejh
 * added copyright notice
 * 
 * Revision 1.4  89/02/01  11:52:42  rcs
 * no change
 * 
 * Revision 1.3  89/01/05  11:51:44  little
 * fix log should it is in a comment
 * 
 * Revision 1.2  89/01/04  17:01:57  rcs
 * modified dpt_rite_rule to return true when the parent vc does not
 * have a creating shared data instance
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
#define memoryPack              /* use memoryPack rather than mallocPack    */
#include "listPack.h"
#include "global.h"
#include "isc.h"
#include "iid.h"
#include "cus.h"
#include "dob.h"
#include "dpt.h"
#include "d22.h"


/* -------------------------------------------     dpt_vc_exists */
dob_data_object dpt_vc_exists (vc_template_name, potential_vce)
string vc_template_name;
LIST potential_vce;

{  cus_sym vc_template, vc_sym;
   dob_data_object vc_instance;
   LIST vce, instances;
   int vce_element;

   DEBUG2 (fprintf (stderr, "calling dpt_vc_exists\n");
	fprintf (stderr, "vc_template_name = %s\n", vc_template_name);
	fprintf (stderr, "potential_vce = ");
	loop_through_list (potential_vce, vce_element, int)
	   fprintf (stderr, "%d ", vce_element);
	fprintf (stderr, "\n"); );
   vc_template = cus_look_up (vc_template_name);
   if (!vc_template)
      FATAL_ERROR (fprintf (stderr, "cannot get vc template named %s\n",
	vc_template_name))
   instances = (LIST) cus_get_prop (vc_template, instances_prop);
   if (!instances)
      return (dob_null_object);

   loop_through_list (instances, vc_instance, dob_data_object)
      {vc_sym = dob_get_symbol_entry (vc_instance);
      vce = (LIST) cus_get_prop (vc_sym, vce_prop);
      DEBUG3 (fprintf (stderr, "vce for %#x = ", vc_instance);
            loop_through_list (vce, vce_element, int)
                 fprintf (stderr, "%d ", vce_element);
	   fprintf (stderr, "\n"); );

      if (list_equal (potential_vce, vce, NULL))
         return ((dob_data_object) cus_get_prop (vc_sym, id_prop));
      }
      
   return (dob_null_object);
}


/* -------------------------------------------     dpt_rite_rule */
boolean dpt_rite_rule (parent_vc, creating_sd)
dob_data_object parent_vc;
dob_data_object creating_sd;
{  cus_sym vc_sym;
   dob_data_object sd;

   DEBUG2 (fprintf (stderr,
	"entering dpt_rite_rule: parent_vc = %#x, creating_sd = %#x\n",
	parent_vc, creating_sd); );

   vc_sym = dob_get_symbol_entry (parent_vc);
/*
   Get the creating shared data property.  If there is no creating
   sd then this must be the one so return.  Otherwise return true
   if the creating sd matches.
*/
   sd = (dob_data_object) cus_get_prop (vc_sym, creating_sd_prop);
   if (sd == dob_null_object) {
     return true;
   }
   else {
     return (creating_sd == sd);
   }
}



