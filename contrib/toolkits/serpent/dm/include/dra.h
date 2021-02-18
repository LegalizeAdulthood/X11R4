/*
$Header: /u/04c/mnt/integ/dm/include/RCS/dra.h,v 1.8 89/08/31 19:02:05 bmc Exp $
*/

/*--------------------------------------------------------------------------*\
|
|  Name:  Run Time Data Access Utility (dra)
|
|  Description:  
|     The run time data access utility provides run time access to various
|     data components within the object base.
|
|  Component of:
|     Dialogue Manager Layer.
|
|  Related files:
|     dra.c - contains the implementation for the run time data access 
|             utility.
|     dra.h - contains the external definition for the run time data
|             access utility.
|
|  Project:
|     SERPENT System
|     User Interface Prototyping
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
$Log:	dra.h,v $
 * Revision 1.8  89/08/31  19:02:05  bmc
 * Added dra_id_exists() function.
 * 
 * Revision 1.7  89/07/24  14:59:26  little
 * ops removal
 * 
 * Revision 1.6  89/02/09  16:49:51  ejh
 * added copyright notice
 * 
 * Revision 1.5  89/02/01  14:59:02  rcs
 * random interations with dra_new
 * 
 * Revision 1.4  89/01/30  14:24:37  rcs
 * added header for dra_get_name
 * removed header for dra_get_element_name, dra_get_template_name
 *    and dra_get_object_name
 * 
 * Revision 1.3  89/01/17  14:30:16  rcs
 * added header for routine dra_get_name
 * 
 * Revision 1.2  88/12/09  17:32:30  little
 * no changes, this is stupid.  i have to fix serpent_source_checkint someday.
 * 
|     July 16, 1988 : Module created.
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

#ifndef dra
#define dra


#include <stdio.h>              /* C Standard I/O Package                   */
#include <errno.h>              /* Standard Ultrix error numbers            */

#include <sys/types.h>          /* System types                             */

#define memoryPack              /* use memoryPack rather than mallocPack    */
#include <listPack.h>           /* C Toolbox - listPack                     */
#include <hashPack.h>           /* C Toolbox - hashPack                     */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

#include "idd.h"		/* interface data definitition abstraction  */
#include "isd.h"		/* interface shared data abstraction	    */
#include "cus.h"		/* compiler symbol table management package */

#include "ubo.h"		/* buffer operations			    */
#include "dtt.h"		/* transaction table			    */
#include "dob.h"		/* object base				    */
#include "dro.h"		/* run time arithmetic operations	    */

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: dra_debug
|
|  Description:
|     Debug variable for the run time conversions utility.
|
\* Definition:                                                              */

#ifdef dra_main
  int dra_debug = 3;
#else
  extern int dra_debug;
#endif

/*
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dra_find_thingie
|
|  Description:
|     finds a particular thingie (vc,var,object) from a thingie instances list
| 
\* Syntax:                                                                  */

	dob_data_object dra_find_thingie (
	  /* parent: in dob_data_object */
	  /* thingie_type: in cus_sym_type */
	  /* thingie_name: in string */
	);
/* 
|  Parameters:
|	dob_data_object parent;
|	cus_sym_type thingie_type;
|	string thingie_name;
|
|  Returns:  
|     the things found
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dra_get_parent_vc
| 
|  Description:  
|     Gets the id of the parent vc for the specified vc or object.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_get_parent_vc(
          /* id : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     id - vc or object id.
|
|  Returns:  
|     The id of the parent vc.
|  
|  Statuses:  
|     invalid_symbol, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_creating_sd
| 
|  Description:  
|     Gets the id of the shared data element which causes the specified
|     vc to be created.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_get_creating_sd(
          /* vc_id : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     vc_id - Created vc.
|
|  Returns:  
|     The id of the creating shared data element for the specified vc.
|  
|  Statuses:  
|     invalid_symbol, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_id_exists
| 
|  Description:  
|     Determines whether an id specifies a valid (i.e., non-removed, defined)
|     thingie.
| 
\* Syntax:                                                                  */

	dro_atomic_data_item dra_id_exists(
	  /* id_parm : in dro_atomic_data_item */
	);
/*
|  Parameters:
|     id_parm - ID of thingie to test.
|
|  Returns:
|     false - Does not exist.
|     true  - Exists.
|
|  Statuses:  
|     removed_data_object, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_vc_id
| 
|  Description:  
|     Gets the id of the shared data element which causes the specified
|     vc to be created.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_get_vc_id(
          /* sd_id : in dro_atomic_data_item			*/
          /* vc_template_name : in dro_atomic_data_item	*/
        );

/* 
|  Parameters:
|     sd_id - ID of shared data element.
|     vc_template_name - String containing the name of the vc template.
|
|  Returns:  
|     The id of a named vc creatingd off this shared data element.
|  
|  Statuses:  
|     invalid_shared_data, symbol_not_found, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_name
| 
|  Description:  
|     Gets the name for the specified data item.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_get_name(
          /* id : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     id - Data item ID.
|
|  Returns:  
|     A string containing the name of the specified data item.
|  
|  Statuses:  
|     invalid_shared_data, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_type
| 
|  Description:  
|     Gets the type of the id.  Valid types are vc, object and sd.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_get_type(
          /* id : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     id - VC instance ID.
|
|  Returns:  
|     The data type for the specified object.
|  
|  Statuses:  
|     invalid_symbol, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_object
| 
|  Description:  
|     Gets the id of the named object within a vc instance.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_get_object(
          /* vc_id : in dro_atomic_data_item */
          /* object_name : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     vc_id - VC instance ID.
|
|  Returns:  
|     The template name for the specified vc instance.
|  
|  Statuses:  
|     invalid_symbol, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_component_value
| 
|  Description:  
|     Gets the value of the specified component.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_get_component_value(
          /* sd_id : in dro_atomic_data_item */
          /* component_name : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     sd_id - VC instance ID.
|     component_name - A string containing the name of the component.
|
|  Returns:  
|     The component value.
|  
|  Statuses:  
|     invalid_shared_data, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_put_component_value
| 
|  Description:  
|     Puts a specified value into a component of a shared data element.
| 
\* Syntax:                                                                  */

        void dra_put_component_value(
          /* sd_id          : in dro_atomic_data_item */
          /* component_name : in dro_atomic_data_item */
          /* value          : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     sd_id - VC instance ID.
|     value - String containing the name of the component.
|     value - The new component value.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     invalid_shared_data, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_get_variable_value
| 
|  Description:  
|     Gets the value of the specified variable.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_get_variable_value(
          /* vc_id : in dro_atomic_data_item */
          /* variable_name : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     vc_id - VC instance ID.
|     vc_id - A string containing the name of the variable.
|
|  Returns:  
|     The variable value.
|  
|  Statuses:  
|     invalid_symbol, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_put_variable_value
| 
|  Description:  
|     Puts a specified value into a variable of a shared data element.
| 
\* Syntax:                                                                  */

        void dra_put_variable_value(
          /* vc_id         : in dro_atomic_data_item */
          /* variable_name : in dro_atomic_data_item */
          /* value         : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     vc_id - VC instance ID.
|     vc_id - A string containing the name of the variable.
|     value - The new variable value.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     invalid_symbol, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_create_element
| 
|  Description:  
|     Creates a shared data element.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_create_element(
          /* element_name : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     element_name - Element name to be created.
|
|  Returns:  
|     The created shared data element.
|  
|  Statuses:  
|     null_element_name, not_found, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_new
| 
|  Description:  
|     Determines if a view controller should be instantiated
|     for the given combination of shared data instance, vc template,
|     and parent vc.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dra_new(
          /* element 		: in dro_atomic_data_item */
          /* vc_template_name	: in dro_atomic_data_item */
          /* parent_vc		: in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     element - Shared data instance.
|     vc_template_name - Name of the vc template.
|     parent_vc - Parent vc containing vc_template_name.
|
|  Returns:  
|     True if the vc should be created, false if it should not.
|  
|  Statuses:  
|     invalid_id, ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dra_delete_temp
| 
|  Description:  
|     deletes the parameter, if it is a temp
| 
\* Syntax:                                                                  */

        void dra_delete_temp(
          /* candidate : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     candidate - thing to potentially delete
|
|  Returns:  
|     Nothing
|  
|  Statuses:  
|     various
|  
\*--------------------------------------------------------------------------*/


#endif
