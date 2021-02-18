/*      $Header: /u/04c/mnt/integ/int/include/RCS/idd.h,v 1.8 89/09/30 11:20:22 bmc Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  Interface Data Definition abstraction (idd)
|
|  Description:  
|     The interface data definition abstraction provides basic operations for
|     creating data definitions to be used by an object base.
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     idd.c - contains the implementation for the interface data definition
|	      abstraction.
|     idd.h - contains the external definition for the interface data 
|	      definition abstraction.
|     iddtest.c - test file.
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
|  Owner:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     Oct. 19, 1987 : Module created.
|     17 may 1988: reed -- added definitions for:
|	 idd_walk_data_definition, idd_get_name, and
|	 idd_get_element_number_of_components 
|     Jun. 20, 1988 : Added idd_undefined idd_data_type - rCs
|     $Log:	idd.h,v $
 * Revision 1.8  89/09/30  11:20:22  bmc
 * Moved 'type' field in 'idd_buffer_type' to end of structure.
 * 
 * Revision 1.7  89/09/29  15:36:56  rcs
 * added support for typed buffers
 * 
 * Revision 1.6  89/09/20  12:52:10  little
 * add idd_validate_component
 * 
 * Revision 1.5  89/02/09  18:37:09  ejh
 * added copyright notice
 * 
 * Revision 1.4  89/01/30  14:43:07  rcs
 * changed the order of types in idd_data_type enum
 * add idd_get_type_string routine header
 * 
 * Revision 1.3  88/11/03  18:46:13  little
 * add idd_validate_element definition
 * 
 * Revision 1.2  88/11/02  14:19:41  rcs
 * moved debug variable from c file - rCs
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

#ifndef idd
#define idd

/*--------------------------------------------------------------------------*\
|  Includes: idd
| 
|  Description:  
|     Necessary includes for the idd abstraction.
| 
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include <errno.h>              /* Standard Ultrix error numbers            */

#include <sys/types.h>		/* System Types				    */

#ifndef memoryPack
#define memoryPack              /* use memoryPack rather than mallocPack    */
#endif
#include <listPack.h>           /* C Toolbox - listPack                     */
#include <hashPack.h>           /* C Toolbox - hashPack                     */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: idd_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef idd_main
  int idd_debug = 1;
#else
  extern int idd_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Type: idd_data_types
| 
|  Description:  
|     The idd data types type is an enumeration of defined data types.
| 
\* Definition:                                                              */

	typedef enum data_types {
	  idd_boolean 		= 0,
	  idd_integer		= 1,
	  idd_real		= 2,
	  idd_string		= 3,
	  idd_record		= 4,
	  idd_id		= 5,
	  idd_buffer		= 6,
	  idd_undefined		= 7,
	  idd_no_data_type      = 8,
	} idd_data_types;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Type: idd_buffer_type
| 
|  Description:  
|     The idd buffer type is used to define the structure of a buffer
|     within shared data.
| 
\* Definition:                                                              */

	typedef struct {
	  int     length;
	  caddr_t body;
	  idd_data_types type;
	} idd_buffer_type;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: idd_data_definition
| 
|  Description:  
|     The idd data definition abstract type defines the underlying data 
|     definition for an object base.
| 
\* Definition:                                                              */

	typedef private idd_data_definition;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: idd_element
| 
|  Description:  
|     The idd element abstract type provides a handle to an element 
|     description structure.
| 
\* Definition:                                                              */

	typedef private idd_element;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type: idd_component
| 
|  Description:  
|     The idd component abstract type provides a handle to an component 
|     description structure.
| 
\* Definition:                                                              */

	typedef private idd_component;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_create_data_definition
| 
|  Description:  
|     The create data definition routine is used to create a data definition.
| 
\* Syntax:                                                                  */

	idd_data_definition idd_create_data_definition();

/* 
|  Parameters:
|     none.
|
|  Returns:  
|      A handle to a newly created data definition.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_create_element
| 
|  Description:  
|     The create element routine is used to add an element to an existing 
|     data definition.
| 
\* Syntax:                                                                  */

	idd_element idd_create_element(
          /* data_definition : in idd_data_definition */
          /* name 	     : in string  	      */
          /* type 	     : in idd_data_type       */
          /* length          : in int                 */
	);

/* 
|  Parameters:
|     data_definition - Data definition in which to add element.
|     name - Element name.
|     type - Element type.
|     length - Length of data element.
|
|  Returns:  
|     A handle to a newly created element.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: idd_validate_component
|
|  Description:
|     determines whethere a shared data component is in an element in a
|	 data definition
\* Syntax:                                                                  */

	boolean idd_validate_component (
	  /* data_definition : in data_definition */
	  /* element_name : in string */
	  /* component_name : in string */
	);

/* 
|  Parameters:
|     data_definition - Data definition in which to search for shared data
|       element.
|     element_name - Name of shared data element to search for.
|     component_name - Name of shared data component to search for.
|
|  Returns:  
|     true if component_name is in element in data_definition, false otherwise
|
|  Statuses:  
|     none
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: idd_validate_element
|
|  Description:
|     determines whethere a shared data element is in a data definition
\* Syntax:                                                                  */

	boolean idd_validate_element (
	  /* data_definition : in data_definition */
	  /* element_name : in string */
	);

/* 
|  Parameters:
|     data_definition - Data definition in which to search for shared data
|       element.
|     element_name - Name of shared data element to search for.
|
|  Returns:  
|     true if element_name is in data_definition, false otherwise
|
|  Statuses:  
|     none
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: idd_create_component
| 
|  Description:  
|     The create component routine is used to add a component to an existing 
|     element.
| 
\* Syntax:                                                                  */

	idd_component idd_create_component(
          /* element : in idd_element   */
          /* name    : in string        */
          /* type    : in idd_data_type */
          /* offset  : in int           */
          /* length  : in int           */
	);

/* 
|  Parameters:
|     element - Element in which to add component.
|     name - Element name.
|     type - Element type.
|     offset  - Offset of data component in element.
|     length - Length of data component.
|
|  Returns:  
|     A handle to a newly created component.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_shared_data_type
|
|  Description:  
|     The get shared data type routine is used to get the type associated with
|     a shared data element.
| 
\* Syntax:                                                                  */

        idd_data_types idd_get_shared_data_type(
          /* data_definition : in idd_data_definition */
          /* element_name    : in string */
          /* component_name  : in string */
        );

/* 
|  Parameters:
|     data_definition - Data definition in which to search for shared data
|       element.
|     element_name - The name of the shared data element.
|     component_name - The name of the shared data component or NULL. 
|
|  Returns:  
|     The type of the shared data element or record component.
|  
|  Statuses:  
|     ok, undefined_element_name.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_create_component_list
| 
|  Description:  
|     The create component name list routine accepts an instance id as a 
|     parameter and returns a list of component names.
| 
\* Syntax:                                                                  */

        LIST idd_create_component_list(
          /* data_definition : in idd_data_definition */
          /* element_name : in string */
        );

/* 
|  Parameters:
|     data_definition - Data definition in which to search for shared data
|       element.
|     element_name - The name of the shared data element.
|
|  Returns:  
|     The list of changed components for the element.
|  
|  Statuses:  
|     ok, invalid_id, element_not_a_record.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_destroy_component_list
| 
|  Description:  
|     The destroy component name list routine frees all storage associated
|     with the specified component list structure.
| 
\* Syntax:                                                                  */

        void idd_destroy_component_list(
          /* component_list : in out LIST */
        );

/* 
|  Parameters:
|     component_list - List of component names to be destroyed.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_handle.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_component_name
| 
|  Description:  
|     The get component name routine returns the "n"th component in a
|     shared data element record. 
| 
\* Syntax:                                                                  */

	string idd_get_component_name(
	  /* data_definition : in idd_data_definition */
	  /* element_name    : in string              */
	  /* n               : in int                 */
	);
/* 
|  Parameters:
|     data_definition - Data definition in which to search for shared data
|       element.
|     element_name - Name of shared data element to search for.
|     n - nth component in record.
|
|  Returns:  
|     The nth component name.
|  
|  Statuses:  
|     ok, not_found.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_length
| 
|  Description:  
|     The get length routine returns the length of a specified shared data
|     element. 
| 
\* Syntax:                                                                  */

	int idd_get_length(
	  /* data_definition : in idd_data_definition */
	  /* element_name : in string */
	);
/* 
|  Parameters:
|     data_definition - Data definition in which to search for shared data
|       element.
|     element_name - Name of shared data element to search for.
|
|  Returns:  
|     The length of the shared data element in bytes or 0 if not found.
|  
|  Statuses:  
|     ok, not_found.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_component_lop
| 
|  Description:  
|     The get component lop routine returns the length, offset and position
|     of a component within a shared data element.  Component positions are
|     numbered from 0 to n-1.
| 
\* Syntax:                                                                  */

	void idd_get_component_lop(
	  /* data_definition : in idd_data_definition */
	  /* element_name    : in string */
	  /* component_name  : in string */
	  /* length          : out int */
	  /* offset          : out int */
	  /* position        : out int */
	);
/* 
|  Parameters:
|     data_definition - Data definition in which to search for shared data
|       element.
|     element_name - Name of the shared data element containing the component.
|     component_name - Component for which to return length and offset.
|     length - The length of the component (in bytes).
|     offset - The offset of the component within the element (in bytes).
|     position - The position of the component within the shared data record.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, not_found.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_number_of_components
| 
|  Description:  
|     The get number of components returns the number of components defined
|     within a shared data element.
| 
\* Syntax:                                                                  */

	int idd_get_number_of_components(
	  /* data_definition : in idd_data_definition */
	  /* element_name    : in string */
	);
/* 
|  Parameters:
|     data_definition - Data definition in which to search for shared data
|       element.
|     element_name - Name of the shared data element.
|
|  Returns:  
|     The number of components in the shared data element.
|  
|  Statuses:  
|     ok, not_found.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_destroy_component
| 
|  Description:  
|     The destroy component routine is used to destroy a component.
| 
\* Syntax:                                                                  */

	void  idd_destroy_component(
	  /* component : in out idd_component */
	);

/* 
|  Parameters:
|     component - Component to be destroyed.
|
|  Returns:  
|     void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_destroy_element
| 
|  Description:  
|     The destroy element routine is used to destroy a element.
| 
\* Syntax:                                                                  */

	void  idd_destroy_element(
	  /* element : in out idd_element */
	);

/* 
|  Parameters:
|     element - Element to be destroyed.
|
|  Returns:  
|     void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_print_data_definition
| 
|  Description:  
|     The print data definition routine is used to print the element names
|     of each element in a specified data definition.  This routine is
|     mainly for debug purposes.
| 
\* Syntax:                                                                  */

	void idd_print_data_definition(
	  /* data_definition : in out idd_data_definition */
	);

/* 
|  Parameters:
|     data_definition - Data definition to be printed.
|
|  Returns:  
|     void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_destroy_data_definition
| 
|  Description:  
|     The destroy data definition routine is used to destroy a data 
|     definition.
| 
\* Syntax:                                                                  */

	void idd_destroy_data_definition(
	  /* data_definition : in out idd_data_definition */
	);

/* 
|  Parameters:
|     data_definition - Data definition to be destroyed.
|
|  Returns:  
|     void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_walk_data_definition
| 
|  Description:  
|     "walks" the data definition, executing the user defined function "on"
|	each element
| 
\* Syntax:                                                                  */

	void idd_walk_data_definition(
	  /* data_definition : in idd_data_definition */
	  /* the_funct : in int (*the_funct) () */
	  /* the_thing : in char* */
	);

/* 
|  Parameters:
|     data_definition - Data definition to be walked
|     the_funct - a user supplied int valued function.
|     the_thing - the magic free parameter to the_funct.
|
|  Returns:  
|     void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_name
| 
|  Description:  
|     return the name of the input element
| 
\* Syntax:                                                                  */

	string idd_get_name(
	  /* element : in element_record */
	);

/* 
|  Parameters:
|     element - the element record in question
|
|  Returns:  
|     the name corresponding to the input element record
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_element_number_of_components
| 
|  Description:  
|     return the number of components in an element record
| 
\* Syntax:                                                                  */

	int idd_get_element_number_of_components(
	  /* element : in element_record */
	);

/* 
|  Parameters:
|     element - the element record in question
|
|  Returns:  
|     the number of components associated with the particular element record
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_type_string
| 
|  Description:  
|     The idd_get_type_string returns the string corresponding to the 
|     specified type.
| 
\* Syntax:                                                                  */

	string idd_get_type_string(
	  /* data_type : in idd_data_type */
	);

/* 
|  Parameters:
|     data_type - the data type.
|
|  Returns:  
|      The string corresponding to the specified type.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/
#endif


