static char rcsid [] =
	"$Header: /u/04c/mnt/integ/int/src/RCS/idd.c,v 1.9 89/09/20 12:51:17 little Exp $";

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
|     17 may 1988: reed -- added definitions for:
|        idd_walk_data_definition, idd_get_name, and
|        idd_get_element_number_of_components
|
|     Oct. 19, 1987 : Module created.
|     $Log:	idd.c,v $
 * Revision 1.9  89/09/20  12:51:17  little
 * add idd_validate_component
 * 
 * Revision 1.8  89/04/05  15:54:47  trm
 * just checking out checkin. no changes made.  trm
 * 
 * Revision 1.7  89/02/09  19:00:36  ejh
 * added copyright notice
 * 
 * Revision 1.6  89/01/30  14:40:24  rcs
 * modified order of type name strings in idd_data_type_strings
 * 
 * Revision 1.5  88/12/19  17:35:46  rcs
 * add idd_get_data_string utility
 * 
 * Revision 1.4  88/11/03  18:45:53  little
 * add idd_validate_element routine
 * 
 * Revision 1.3  88/11/02  15:41:54  rcs
 * *** empty log message ***
 * 
 * Revision 1.2  88/11/02  14:18:09  rcs
 * moved debug variable to interface file - rCs
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

#define idd_main
#include "idd.h"		/* data definition abstraction	    	    */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(idd_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(idd_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(idd_debug, statement)

#define MAX_HASH 64

/*-------------------- Type Definitions ------------------------------------*/

typedef struct {
  string name;
  idd_data_types type;
  int offset, length, position;
  char *parent;
} component_record;

typedef struct {
  string name;
  idd_data_types type;
  int components;
  int length;
  HASH component_names;
  HASH component_table;
} element_record;

/*-------------------- Retained Data ---------------------------------------*/

static char *idd_data_type_strings[] = {
  "boolean",
  "integer",
  "real",
  "string",
  "record",
  "id",
  "buffer",
  "undefined",
  "no data type"
};

/*--------------------------------------------------------------------------*\
|  Routine: match_element_name
| 
|  Description:  
|     Routine to compare the identifiers with user elements.
|   
\*--------------------------------------------------------------------------*/
int match_element_name(identifier, element)
string identifier;
element_record *element;
{ 					/* local type definitions	    */
  set_status(ok);			/* begin 			    */

  DEBUG2(
    fprintf(stderr, "match_element_name: call.\n");
    fprintf(stderr, "identifier = %s.\n", identifier);
    fprintf(stderr, "element = %x.\n", element);
  );

  DEBUG3(
    fprintf(stderr, "element->name = %s.\n", element->name);
  );

  DEBUG2(
    fprintf(stderr, 
      "match_element_name:  match_element_name = %d.\n", 
      strcmp(identifier, element->name) == 0
    );
    fprintf(stderr, "match_element_name:  exiting.\n");
  );

  return(strcmp(identifier, element->name) == 0);
} 					/* end match_element_name	    */

/*--------------------------------------------------------------------------*\
|  Routine: match_component_name
| 
|  Description:  
|     Routine to compare the identifiers with user components.
|   
\*--------------------------------------------------------------------------*/
int match_component_name(identifier, component)
string identifier;
component_record *component;
{ 					/* local type definitions	    */
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "match_component_name: call.\n");
  );

  DEBUG2(
    fprintf(stderr, 
      "match_component_name:  match_component_name = %d.\n",
      strcmp(identifier, component->name) == 0
    );
    fprintf(stderr, "match_component_name:  exiting.\n");
  );
  return(strcmp(identifier, component->name) == 0);
} 					/* end match_component_name	    */

/*--------------------------------------------------------------------------*\
|  Routine: hash_string
| 
|  Description:  
|     Internal function which will convert a string identifiers into indecies
|     in the array of hash lists.
|   
\*--------------------------------------------------------------------------*/
int hash_string(name)
string name;
{ 					/* local type definitions	    */
  register int hash_value = 0;
  register string pointer;

  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "hash_string: call.\n");
  );
/*
   Loop through each character in the name string and add the character value
   to the accumulator.
*/
  for (pointer = name; *pointer != '\0'; pointer++) {
    hash_value += *pointer;
  }
/*
   Return a value in the right range.
*/
  DEBUG2(
    fprintf(stderr, "hash_string: exiting.\n");
  );
  return ((hash_value & 077777) % MAX_HASH);
} 					/* end hash_string		    */

/*--------------------------------------------------------------------------*\
|  Routine: match_position
| 
|  Description:  
|     Routine to compare the identifiers with user elements.
|   
\*--------------------------------------------------------------------------*/
int match_position(identifier, component)
int identifier;
component_record *component;
{ 					/* local type definitions	    */
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "match_position: call.\n");
  );

  DEBUG2(
    fprintf(stderr, "match_position: exiting.\n");
    fprintf(stderr, 
      "match_position: match position = %d.\n", 
      identifier == component->position
    );
  );
  return(identifier == component->position);
} 					/* end match_position		    */

/*--------------------------------------------------------------------------*\
|  Routine: hash_int
| 
|  Description:  
|     Internal function which will convert an integer into indecies
|     in the array of hash lists.
|   
\*--------------------------------------------------------------------------*/
int hash_int(n)
int n;
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "hash_int: call.\n");
  );
/*
   Return a value in the right range.
*/
  DEBUG2(
    fprintf(stderr, "hash_int: exiting.\n");
  );
  return ((n & 077777) % MAX_HASH);
} 					/* end hash_int		    */

/*--------------------------------------------------------------------------*\
|  Routine: idd_create_data_definition
| 
|  Description:  
|     The create data definition routine is used to create a data definition.
\*--------------------------------------------------------------------------*/
idd_data_definition idd_create_data_definition()
{ 					/* local type definitions	    */
  HASH data_definition;
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_create_data_definition: call.\n");
  );

  data_definition = make_hashtable(
    MAX_HASH, 
    hash_string, 			/* element name			    */
    match_element_name
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, 
      "idd_create_data_definition:  data_definition = %x.\n",
      (idd_data_definition)data_definition
    );
    fprintf(stderr, "idd_create_data_definition:  exiting.\n");
  );
  return((idd_data_definition)data_definition);
} 					/* end idd_create_data_definition   */

/*--------------------------------------------------------------------------*\
|  Routine: idd_create_element
| 
|  Description:  
|     The create element routine is used to create an element for a given
|     data definition.
\*--------------------------------------------------------------------------*/
idd_element idd_create_element(data_definition, name, type, length)
idd_data_definition data_definition;
string name;
idd_data_types type;
int length;
{ 					/* local type definitions	    */
  element_record *element;
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_create_element: call.\n");
    fprintf(stderr, "data_definition = %x.\n", data_definition);
    fprintf(stderr, "name = %s.\n", name);
    fprintf(stderr, "type = %d.\n", type);
    fprintf(stderr, "length = %d.\n", length);
  );
/*
   Create Element.
*/
  element = (element_record *)make_node(sizeof(element_record));
  rcheck_null(
    element, 
    "idd_create_element:  out of memory during make_node.\n",
    out_of_memory,
    NULL
  );
/*
   Initialize element fields.
*/
  element->name = (string)make_node(strlen(name)+1);
  rcheck_null(
    element->name, 
    "idd_create_element:  out of memory during make_node.\n",
    out_of_memory,
    NULL
  );
  strcpy(element->name, name);

  element->length = length;
  element->components = -1;
  element->type = type;
/*
   If element is a record create two hash tables for the components,
   one that is indexed by the component name and another which is
   indexed by the position of the component in the element.
*/
  if (type == idd_record) {		/* record type			    */

    element->component_table = make_hashtable(
      MAX_HASH, 
      hash_string, 
      match_component_name
    );

    element->component_names = make_hashtable(
      MAX_HASH, 
      hash_int, 
      match_position
    );
  }					/* end record type		    */
/*
   Add the element to the data definition hash table using the element
   name as the hashing index.
*/
  add_to_hashtable(
    data_definition, 			/* hash table			    */
    element, 				/* element record		    */
    name				/* element name			    */
  );
/*
   Return.
*/

  DEBUG3(
    fprintf(stderr, "element_name = %s.\n", (idd_element)element->name);
  );

  DEBUG2(
    fprintf(stderr, "element = %x.\n", (idd_element)element);
    fprintf(stderr, "idd_create_element:  exiting.\n");
  );
  return((idd_element)element);
} 					/* end idd_create_element   */

/*--------------------------------------------------------------------------*\
|  Routine: idd_create_component
| 
|  Description:  
|     The create component routine is used to create a component of a 
|     particular element.
\*--------------------------------------------------------------------------*/
idd_component idd_create_component(element, name, type, offset, length)
idd_element element;
string name;
idd_data_types type;
int offset, length;
{ 					/* local type definitions	    */
  component_record *component;
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */

  DEBUG2(
    fprintf(stderr, "idd_create_component:  call.\n");
    fprintf(stderr, "idd_create_component:  element = %x.\n", element);
    fprintf(stderr, "idd_create_component:  name = %s.\n", name);
    fprintf(stderr, "idd_create_component:  type = %d.\n", type);
    fprintf(stderr, "idd_create_component:  offset = %d.\n", offset);
    fprintf(stderr, "idd_create_component:  length = %d.\n", length);
  );

/*
   Allocate storage for component record.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "idd_create_component:  about to make component record node.\n"
    );
  );

  component = (component_record *)make_node(sizeof(component_record));
  rcheck_null(
    component, 
    "idd_create_component:  out of memory during make_node.\n",
    out_of_memory,
    NULL
  );
/*
   Initialize component fields.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "idd_create_component:  about to make component name node.\n"
    );
    fprintf(stderr, "idd_create_component:  name = %s.\n", name);
    fprintf(
      stderr, 
      "idd_create_component:  strlen(name) = %d.\n", 
      strlen(name)
    );
  );

  component->name = (string)make_node(strlen(name)+1);
  rcheck_null(
    component->name, 
    "idd_create_component:  out of memory during make_node.\n",
    out_of_memory,
    NULL
  );

  strcpy(component->name, name);

  component->type = type;
  component->offset = offset;
  component->length = length;
  component->parent = element;		/* back pointer			    */
/*
   Add component to hash of components referenced by element name.
*/

  DEBUG3(
    fprintf(
      stderr, 
      "idd_create_comp:  about to add comp to comp_table hashtble.\n"
    );
  );

  add_to_hashtable(
    ((element_record *)element)->component_table, 
    component,
    name
  );
/*
   Increment component number in element record and set position field
   in component record accordingly.  Add component to hash referenced by 
   component number.
*/
  component->position = ++(((element_record *)element)->components);

  DEBUG3(
    fprintf(stderr, "idd_create_comp:  about to add comp to comp_names hashtble.\n");
    fprintf(stderr, 
      "idd_create_comp:  component_names = %x.\n",
      ((element_record *)element)->component_names
    );
    fprintf(stderr, 
      "idd_create_comp:  component = %x.\n",
      component
    );
    fprintf(stderr, 
      "idd_create_comp:  component position = %d.\n",
      component->position
    );
  );

  add_to_hashtable(
    ((element_record *)element)->component_names, 
    component,
    component->position
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, 
      "idd_create_component: component = %x.\n", 
      (idd_component)component
    );
    fprintf(stderr, "idd_create_component:  exiting.\n");
  );
  return((idd_component)component);
} 					/* end idd_create_component  	    */

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_shared_data_type
|
|  Description:
|     The get shared data type routine is used to get the type associated with
|     a shared data element or component if not nullstring.
\*--------------------------------------------------------------------------*/
idd_data_types idd_get_shared_data_type(data_definition,
				element_name, component_name)
idd_data_definition data_definition;
string element_name;
string component_name;
{                                       /* local variable definitions       */
  idd_data_types data_type;
  element_record *element;
  component_record *component;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(
      stderr, 
      "idd_get_shared_data_type: call.\n"
    );
    fprintf(
      stderr, 
      "idd_get_shared_data_type: data definition = %x.\n",
      data_definition
    );
    fprintf(
      stderr, 
      "idd_get_shared_data_type: element name = %s.\n", 
      element_name
    );
    fprintf(
      stderr, 
      "idd_get_shared_data_type: component name = %s.\n", 
      component_name
    );
  );
/*
  Find element name in hash table.
*/
  element = (element_record *)get_from_hashtable(
    data_definition, 
    element_name
  );
  rcheck_null(
    element, 
    "idd_get_shared_data_type: element not found.\n", 
    not_found,
    idd_no_data_type
  );
/* 
   If component name not specified return element type.
*/
  if (component_name == NULL) {		/* element specified		    */
    data_type = element->type;
  }					/* end if element specified	    */
/* 
   Else if component name specified return component type.
*/
  else {				/* component specified		    */
/*
   Find component name in component table.
*/
    component = (component_record *)get_from_hashtable(
      element->component_table, 
      component_name
    );
    rcheck_null(
      component, 
      "idd_get_shared_data_type: component not found.\n",
      not_found,
      idd_no_data_type
    );

    data_type = component->type;  

  }					/* end else component specified	    */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "idd_get_shared_data_type: data type = %d.\n", data_type);
    fprintf(stderr, "idd_get_shared_data_type: exiting.\n");
  );
  return data_type;
}                                       /* end idd_get_shared_data_type     */

/*--------------------------------------------------------------------------*\
|  Routine: build_component_list
|
|  Description:
|     The build component list routine adds a component name to the tail
|     of a specified list.
\*--------------------------------------------------------------------------*/
void build_component_list(component_list, component)
LIST component_list;
component_record *component;
{                                       /* local variable definitions       */
  
  (void)add_to_tail(component_list, component->name);

  return;
}                                       /* end build_component_list         */

/*--------------------------------------------------------------------------*\
|  Routine: idd_create_component_list
|
|  Description:
|     The get component name list routine accepts an element name as a
|     parameter and returns a list of component names.
\*--------------------------------------------------------------------------*/
LIST idd_create_component_list(data_definition, element_name)
idd_data_definition data_definition;
string element_name;
{                                       /* local variable definitions       */
  LIST component_list;
  element_record *element;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "idd_create_component_list: call.\n");
    fprintf(
      stderr, 
      "idd_create_component_list: element name = %s.\n", 
      element_name
    );
  );
/*
  Find element name in hash table.
*/
  element = (element_record *)get_from_hashtable(
    data_definition, 
    element_name
  );
  rcheck_null(
    element, 
    "idd_create_component_list: element not found.\n",
    not_found,
    NULL
  );
/* 
   Validate that the element is a record.
*/
  if (element->components == -1) {	/* not a record			    */
    set_status(element_not_a_record);
    DEBUG1(
      isc_print_status("idd_create_component_list");
    );
    return NULL;
  }					/* end if no components		    */
/*
   Create a list in process memory to return the list of component names.
*/
  component_list = make_list();
  rcheck_null(
    component_list,
    "idd_create_component_list:  make_list failed.\n",
    out_of_memory,
    NULL
  );
/*
   Convert list structure of hash elements by applying build list function
   to each element in the change table.
*/
  for_all_hashtable(
    element->component_table,
    component_list,
    build_component_list
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(
      stderr, 
      "idd_create_component_list: list = %x.\n", 
      component_list
    );
    fprintf(stderr, "idd_create_component_list: exiting.\n");
  );
  return component_list;
}                                       /* end idd_create_component_list    */

/*--------------------------------------------------------------------------*\
|  Routine: idd_destroy_component_list
|
|  Description:
|     The destroy component list routine frees all storage associated
|     with the specified component list.
\*--------------------------------------------------------------------------*/
void idd_destroy_component_list(component_list)
LIST component_list;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "idd_destroy_component_list: call.\n");
    fprintf(stderr, 
      "idd_destroy_component_list: component list = %x.\n", 
      component_list
    );
  );
/*
   Destroy the list not including the nodes (which are actually pointers
   to data in the data definition.
*/
  free_list(component_list, NOT_NODES);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "idd_destroy_component_list: exiting.\n");
  );
  return;
}                                       /* end idd_destroy_component_list       */

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_component_name
| 
|  Description:  
|     The get component name routine returns the nth component in a 
|     shared data element record.
\*--------------------------------------------------------------------------*/
string idd_get_component_name(data_definition, element_name, position)
idd_data_definition data_definition;
string element_name;
int position;
{ 					/* local type definitions	    */
  element_record *element;
  component_record *component;
  string component_name;
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */

  DEBUG2(
    fprintf(stderr, "idd_get_component_name: call.\n");
    fprintf(stderr, "data definition = %x.\n", data_definition);
    fprintf(stderr, "element = %s.\n", element_name);
    fprintf(stderr, "position = %d.\n", position);
  );
/*
  Find element name in hash table.
*/
  element = (element_record *)get_from_hashtable(
    data_definition, 
    element_name
  );
  rcheck_null(
    element, 
    "idd_get_component_name: element not found.\n", 
    not_found,
    NULL
  );
/* 
   Validate that the element is a record.
*/
  if (element->components == -1) {	/* not a record			    */
    set_status(element_not_a_record);
    DEBUG1(
      isc_print_status("idd_create_component_list");
    );
    return NULL;
  }					/* end if no components		    */
/*
   Find component name in component name hashtable.
*/
  component = (component_record *)get_from_hashtable(
    element->component_names,
    position
  );
  rcheck_null(
    component, 
    "idd_get_component_name: component not found.\n",
    not_found,
    NULL
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(
      stderr, 
      "idd_get_component_name: component name = %s.\n", 
      component->name
    );
    fprintf(stderr, "idd_get_component_name: exiting.\n");
  );
  return component->name;
} 					/* end idd_get_component_name	    */


/*--------------------------------------------------------------------------*\
|  Routine: idd_get_length
| 
|  Description:  
|     The get length routine is used to returns the length of a specified 
|     shared data element.
\*--------------------------------------------------------------------------*/
int idd_get_length(data_definition, element_name)
idd_data_definition data_definition;
string element_name;
{ 					/* local type definitions	    */
  element_record *element;
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_get_length: call.\n");
  );

/*
  Find element name in hash table.
*/
  element = (element_record *)get_from_hashtable(
    data_definition, 
    element_name
  );
  rcheck_null(
    element, 
    "idd_get_length: element not found.\n", 
    not_found,
    -1
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "idd_get_length: length = %d.\n", element->length);
    fprintf(stderr, "idd_get_length: exiting.\n");
  );
  return element->length;
} 					/* end idd_get_length	    */


/*--------------------------------------------------------------------------*\
|  Routine: idd_validate_element
| 
|  Description:  
|     determines whethere a shared data element is in a data definition
\*--------------------------------------------------------------------------*/
boolean idd_validate_element (data_definition, element_name)
idd_data_definition data_definition;
string element_name;
{  element_record *element;

   set_status(ok);			/* begin 			    */
   DEBUG2 (fprintf(stderr, "idd_validate_element: call.\n"););
 
 					/* Find element name in hash table.  */
   element = (element_record *) get_from_hashtable (
				data_definition, element_name);

   if (element)
      return (TRUE);
    else
      return (FALSE);
} 					/* end idd_validate_element */
 
 
/*--------------------------------------------------------------------------*\
|  Routine: idd_validate_component
| 
|  Description:  
|     determines whethere a shared data component is in an element in a data
|	definition
\*--------------------------------------------------------------------------*/
boolean idd_validate_component (data_definition, element_name, component_name)
idd_data_definition data_definition;
string element_name;
string component_name;
{  element_record *element;
   component_record *component;

   set_status(ok);			/* begin 			    */
   DEBUG2 (fprintf (stderr, "idd_validate_component: call.\n"););
 
 					/* Find element name in hash table  */
   element = (element_record *) get_from_hashtable (
				data_definition, element_name);
   if (element)
				/* Find component name in component table */
      {component = (component_record *) get_from_hashtable (
    					element->component_table, 
    					component_name);
      if (component)
         return (TRUE);
        else
         return (FALSE);
      }
    else
      return (FALSE);
} 					/* end idd_validate_component */
 
 
 /*--------------------------------------------------------------------------*\
 |  Routine: idd_get_component_lop
| 
|  Description:  
|     The get offset routine returns the size and offset of a component 
|     within a shared data element.
\*--------------------------------------------------------------------------*/
void idd_get_component_lop(data_definition, element_name, component_name,
	 		      length, offset, position)
idd_data_definition data_definition;
string element_name, component_name;
int *length;
int *offset;
int *position;
{ 					/* local type definitions	    */
  element_record *element;
  component_record *component;
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_get_component_lop: call.\n");
    fprintf(
      stderr, 
      "idd_get_component_lop: data definition = %x.\n", 
      data_definition
    );
    fprintf(
      stderr, 
      "idd_get_component_lop: element name = %s.\n", 
      element_name
    );
    fprintf(
      stderr, 
      "idd_get_component_lop: component name = %s.\n", 
      component_name
    );
  );
/*
  Find element name in hash table.
*/
  element = (element_record *)get_from_hashtable(
    data_definition, 
    element_name
  );

  check_null(
    element, 
    "idd_get_component_lop: element not found.\n",
    not_found
  );
/* 
   Validate that the element is a record.
*/
  if (element->components == -1) {	/* not a record			    */
    set_status(element_not_a_record);
    DEBUG1(
      isc_print_status("idd_create_component_list");
    );
    return;
  }					/* end if no components		    */
/*
   Find component name in component table.
*/
  component = (component_record *)get_from_hashtable(
    element->component_table, 
    component_name
  );
  check_null(
    component, 
    "idd_get_component_lop: component not found.\n",
    not_found
  );
/*
   Set return values.
*/
  *length = component->length;
  *offset = component->offset;
  *position = component->position;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "idd_get_component_lop: exiting.\n");
    fprintf(stderr, "idd_get_component_lop: length = %d.\n", *length);
    fprintf(stderr, "idd_get_component_lop: offset = %d.\n", *offset);
    fprintf(stderr, "idd_get_component_lop: position = %d.\n", *position);
  );
  return;
} 					/* end idd_get_component_lop    */

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_number_of_components
| 
|  Description:  
|     The get number of components returns the number of components in 
|     a shared data record.
\*--------------------------------------------------------------------------*/
int idd_get_number_of_components(data_definition, element_name)
idd_data_definition data_definition;
string element_name;
{ 					/* local type definitions	    */
  element_record *element;
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_get_number_of_components: call.\n");
  );
/*
  Find element name in hash table.
*/
  element = (element_record *)get_from_hashtable(
    data_definition, 
    element_name
  );

  rcheck_null(
    element, 
    "idd_get_number_of_components: element not found.\n",
    not_found,
    -1
  );
/*
   Return number of components.
*/
  DEBUG2(
    fprintf(stderr, 
      "idd_get_number_of_components:  # of components = %d.\n",
      element->components
    );
    fprintf(stderr, "idd_get_number_of_components:  exiting.\n");
  );
  return element->components;
} 					/* end idd_get_number_of_components */

/*--------------------------------------------------------------------------*\
|  Routine: idd_destroy_component
| 
|  Description:  
|     The destroy component is used to destroy an existing component.
\*--------------------------------------------------------------------------*/
void idd_destroy_component(component)
idd_component component;
{ 					/* local type definitions	    */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_destroy_component: call.\n");
  );
/*
   Free associated storage.
*/  
  free_node(((component_record *)component)->name);  
  free_node(component); 
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "idd_destroy_component: exiting.\n");
  );
  return;
} 					/* end idd_destroy_component	    */

/*--------------------------------------------------------------------------*\
|  Routine: destroy_components
| 
|  Description:  
|     Prints the element name field of an element record.
\*--------------------------------------------------------------------------*/
void destroy_components(nada, component)
component_record *nada;			/* dummy parameter		    */
component_record *component;		/* component to destroy 	    */
{ 					/* local type definitions	    */

  idd_destroy_component(component);  

  return;

} 					/* end destroy_components  */

/*--------------------------------------------------------------------------*\
|  Routine: idd_destroy_element
| 
|  Description:  
|     The destroy element is used to destroy an existing element.
\*--------------------------------------------------------------------------*/
void idd_destroy_element(element)
idd_element element;
{ 					/* local type definitions	    */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_destroy_element: call.\n");
  );
/*
   If element is a record free component structures.
*/
  if (((element_record *)element)->type == idd_record) {  /* record type    */
/*
    Destroy each component in the record.
*/
    for_all_hashtable(
      ((element_record *)element)->component_table,
      NULL,
      destroy_components
    );
/*
   Free all storage associated with both component hash tables.
*/
    free_hashtable(((element_record *)element)->component_table, NOT_NODES);
    free_hashtable(((element_record *)element)->component_names, NOT_NODES);

  }					/* end if record type		    */
/*
   Free storage associated with element name.
*/
  free_node(((element_record *)element)->name);
/*
   Free storage associated with element record.
*/
  free_node(element);  
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "idd_destroy_element: exiting.\n");
  );
  return;
} 					/* end idd_destroy_element	    */

/*--------------------------------------------------------------------------*\
|  Routine: print_element_name
| 
|  Description:  
|     Prints the element name field of an element record.

\*--------------------------------------------------------------------------*/
void print_element_name(nada, element)
element_record *nada;			/* dummy parameter		    */
element_record *element;		/* element to print name of	    */
{ 					/* local type definitions	    */

  fprintf(stderr, "element_name = %s.\n", element->name);

  return;

} 					/* end print_element_name  */

/*--------------------------------------------------------------------------*\
|  Routine: idd_print_data_definition
| 
|  Description:  
|     The print data definition is used to print the element name of
|     each element in the specified data definition.  This routine is
|     mainly for debug purposes.
\*--------------------------------------------------------------------------*/
void idd_print_data_definition(data_definition)
idd_data_definition data_definition;
{ 					/* local type definitions	    */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_print_data_definition: call.\n");
  );
/*
   Print the element name for each element in the hash table.
*/  
  for_all_hashtable(data_definition, NULL, print_element_name);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "idd_print_data_definition: exiting.\n");
  );
  return;
} 					/* end idd_print_data_definition  */

/*--------------------------------------------------------------------------*\
|  Routine: destroy_elements
| 
|  Description:  
|     Destroys an element in the hash table.
\*--------------------------------------------------------------------------*/
void destroy_elements(nada, element)
element_record *nada;			/* dummy parameter		    */
element_record *element;		/* element to destroy	 	    */
{ 					/* local type definitions	    */

  idd_destroy_element(element);  

  return;

} 					/* end destroy_elements 	    */

/*--------------------------------------------------------------------------*\
|  Routine: idd_destroy_data_definition
| 
|  Description:  
|     The destroy data definition is used to destroy an existing data 
|     definition including all user specified elements within the data
|     definition.
\*--------------------------------------------------------------------------*/
void idd_destroy_data_definition(data_definition)
idd_data_definition data_definition;
{ 					/* local type definitions	    */
/*
   Initialization.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_destroy_data_definition: call.\n");
    fprintf(
      stderr, 
      "idd_destroy_data_definition: data definition = %x.\n", 
      data_definition
    );
  );
/*
   Destroy all elements in the hash table.
*/
  for_all_hashtable(data_definition, NULL, destroy_elements);
/*
   Free storage associated with hash table structure.
*/
  free_hashtable((HASH)data_definition, NOT_NODES);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "idd_destroy_data_definition: exiting.\n");
  );
  return;
} 					/* end idd_destroy_data_definition  */

/*--------------------------------------------------------------------------*\
|  Routine: idd_get_name
| 
|  Description:  
|     return the name of the input element
\*--------------------------------------------------------------------------*/
string idd_get_name (element)
element_record *element;		/* element in question */
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_get_name: call.\n");
  );

/*
  return the element name
*/
  DEBUG2(
    fprintf(stderr, 
      "idd_get_name:  name = %s.\n",
      element->name
    );
    fprintf(stderr, "idd_get_name:  exiting.\n");
  );
  return (element->name);
} 					/* end idd_get_name */


/*--------------------------------------------------------------------------*\
|  Routine: idd_walk_data_definition
| 
|  Description:  
|     call the passed though call-back function for every element of the
|	data definition
\*--------------------------------------------------------------------------*/
void idd_walk_data_definition (data_definition, the_funct, the_thing)
idd_data_definition data_definition;
int (*the_funct) ();
char *the_thing;
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_walk_data_definition: call.\n");
  );

  for_all_hashtable (data_definition, the_thing, the_funct);

  DEBUG2(
    fprintf(stderr, "idd_walk_data_definition:  exiting.\n");
  );

  return;
} 					/* end idd_walk_data_definition */


/*--------------------------------------------------------------------------*\
|  Routine: idd_get_element_number_of_components
| 
|  Description:  
|     return the number of components in an element record
\*--------------------------------------------------------------------------*/
int idd_get_element_number_of_components (element)
element_record *element;		/* element in question */
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_get_element_number_of_components: call.\n");
  );

/*
  return the element's number of components
*/
  DEBUG2(
    fprintf(stderr, 
      "idd_get_element_number_of_components:  number of components = %d.\n",
      element->components
    );
    fprintf(stderr, "idd_get_element_number_of_components:  exiting.\n");
  );
  return (element->components + 1);
}				/* end idd_get_element_number_of_components */



/*--------------------------------------------------------------------------*\
|  Routine: idd_get_type_string
| 
|  Description:  
|     The idd_get_type_string returns the string corresponding to the
|     specified type.
\*--------------------------------------------------------------------------*/
string idd_get_type_string(data_type)
idd_data_types data_type;
{ 					/* local type definitions	    */
/*
   Initialize.
*/
  set_status(ok);			/* begin 			    */
  DEBUG2(
    fprintf(stderr, "idd_get_type_string: call.\n");
  );
/*
   Return the appropriate string.
*/
  DEBUG2(
    fprintf(
      stderr, 
      "idd_get_type_string:  data type = %s.\n",
      idd_data_type_strings[(int)data_type]
    );
    fprintf(stderr, "idd_get_type_string:  exiting.\n");
  );
  return(idd_data_type_strings[(int)data_type]);
}				/* end idd_get_type_string */


