/*
$Header: /u/04c/mnt/integ/dm/include/RCS/dob.h,v 1.11 89/10/23 22:55:22 serpent Exp $
*/

/*--------------------------------------------------------------------------*\
|
|  Name:  Object Base Manager  (dob)
|
|  Description:  
|     The object base manager provides a common means for accessing data 
|     elements within the dialogue manager.  The object base manager provides
|     a front end to both the symbol table and the shared data components
|     of the dialogue manager.
|
|  Component of:
|     Dialogue Manager Layer.
|
|  Related files:
|     dob.c - contains the implementation for the transactionmanager.
|     dob.h - contains the external definition for the transaction manager.
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
$Log:	dob.h,v $
 * Revision 1.11  89/10/23  22:55:22  serpent
 * put ifndef around define memoryPack
 * 
 * Revision 1.10  89/10/05  21:37:19  rcs
 * typeless buffers
 * 
 * Revision 1.9  89/07/24  14:58:42  little
 * ops removal
 * 
 * Revision 1.8  89/05/22  11:13:35  bmc
 * No longer includes 'uer.h'.  Instead, 'uer.h' is explicitly included by
 * those files that need it.  Change was necessary to accomodate changes
 * to 'uer.h'.
 * 
 * Revision 1.7  89/05/19  17:09:19  bmc
 * Defined abstract data type 'dob_data_object'.  Changed function
 * definitions to be consistent with new version of 'dob.c'.
 * 
 * Revision 1.6  89/02/09  16:44:09  ejh
 * added copyright notice
 * 
 * Revision 1.5  89/01/30  14:26:26  rcs
 * added header for dob_get_real_name procedure
 * 
 * Revision 1.4  89/01/05  12:48:19  rcs
 * added uer.h to the list of include files
 * 
 * Revision 1.3  88/10/27  19:08:01  little
 * add definition for dob_temp_release
 * 
 * Revision 1.2  88/10/26  11:05:25  little
 * add description of dob_validate_object routine
 * 
|     May 24, 1988 : Module created.
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

#ifndef dob
#define dob
/*--------------------------------------------------------------------------*\
|  Includes: isd
|
|  Description:
|     The following is a list of all the necessary includes for the isd
|     abstraction.
|
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include <errno.h>              /* Standard Ultrix error numbers            */

#include <sys/types.h>          /* System types                             */

#ifndef memoryPack
#define memoryPack              /* use memoryPack rather than mallocPack    */
#endif
#include <listPack.h>           /* C Toolbox - listPack                     */
#include <hashPack.h>           /* C Toolbox - hashPack                     */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

#include "idd.h"                /* interface data definitition abstraction  */

#include "isd.h"		/* interface shared data abstraction	    */
#include "cus.h"		/* compiler symbol table management package */

#include "ubo.h"		/* buffer operations			    */
#include "dtt.h"		/* transaction table			    */
#include "dpg.h"		/* snippet execution abstraction	    */
#include "dgr.h"		/* snippet execution abstraction	    */

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: dob_debug
|
|  Description:
|     Debug variable for the object base abstraction.
|
\* Definition:                                                              */

#ifdef dob_main
  int dob_debug = 2;
#else
  extern int dob_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: dob_data_object
|
|  Description:
|     Provides a handle for an (private) object in the object base.
|
\* Syntax:                                                                  */

	typedef private dob_data_object;
/*
??? Change the following to  ((dob_data_object) NULL_PRIVATE)
??? when it's clear nobody cares what the value is.
*/
#	define dob_null_object ((dob_data_object) -1)

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_init
| 
|  Description:  
|     Performs necessary intialization of the object base manager.
| 
\* Syntax:                                                                  */

        void dob_init();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dob_get_object_id
|
|  Description:
|     Converts a dob_data_object into its corresponding id.  Not to be
|       used lightly!
|
|
\* Syntax:                                                                  */

        iid_id_type dob_get_object_id (
          /* object : in dob_data_object   */
        );

/*
|  Parameters:
|     object - the handle of the object for which the id is to be returned
|
|  Returns:
|     iid_id_type - returns the id associated with the object.  If a 
|        null object is specifed, it returns "iid_null_id".
|
|  Statuses:
|     ok
|     invalid_data_object
|
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dob_validate_object
| 
|  Description:  
|     Checks to see if an object is still in the data base; returns true
|       if it is, else false.
| 
\* Syntax:                                                                  */

        boolean dob_validate_object (
          /* object : in dob_data_object   */
        );

/* 
|  Parameters:
|     id - the id of the object to be checked
|
|  Returns:  
|     boolean - true if object exists, false otherwise
|  
|  Statuses:  
|     ok
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dob_transaction_update
| 
|  Description:  
|     Updates the object base according to changes specified by the 
|     transaction.
| 
\* Syntax:                                                                  */

        void dob_transaction_update(
          /* transaction : in isd_trans  */
        );

/* 
|  Parameters:
|     transaction - Transction with which to update object base.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_transaction_handle, out_of_memory.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dob_create_symbol_table_entry
| 
|  Description:  
|     Creates an id in the object base for an existing symbol table
|     handle.
| 
\* Syntax:                                                                  */

        dob_data_object dob_create_symbol_table_entry(
          /* symbol_table_handle : in cus_sym */
        );

/* 
|  Parameters:
|     transaction - Transction with which to update object base.
|
|  Returns:  
|     Handle for the specified symbol table data object.
|  
|  Statuses:  
|     ok, invalid_transaction_handle, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_slang_obj_create
| 
|  Description:  
|     Creates a shared data instance.
| 
\* Syntax:                                                                  */

        dob_data_object dob_slang_obj_create(
          /* element_name	: in string	     */
          /* mailbox		: in ipc_mailbox     */
	  /* template		: in dob_data_object */
	  /* parent_vc		: in dob_data_object */
        );

/* 
|  Parameters:
|     shared_data - Handle of shared data instance.
|     component_name - Name of component to modify.
|     template - Handle of shared data object template
|
|  Returns:  
|     Handle of created shared data instance.
|  
|  Statuses:  
|     ok, null_element_name, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_shared_data_tell
| 
|  Description:  
|     Updates a shared data instance and notifies the appropriate
|     application/technology.
| 
\* Syntax:                                                                  */

        void dob_shared_data_tell(
          /* shared_data	: in dob_data_object */
          /* component_name	: in string	     */
          /* data		: in caddr_t	     */
        );

/* 
|  Parameters:
|     shared_data - Handle of shared data instance.
|     component_name - Name of component to modify
|     data - Pointer to the new data.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_data_object, null_component_name, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_variable_create
| 
|  Description:  
|     Creates a variable instance in the the symbol table and the appropriate
|     object base mapping and returns a unique object id.
| 
\* Syntax:                                                                  */

        dob_data_object dob_variable_create(
          /* parent_vc : in dob_data_object */
          /* variable_template : in dob_data_object */
        );

/* 
|  Parameters:
|     parent_vc - Handle of the parent vc of the variable to be created.
|     variable_template - Handle of the variable template.
|
|  Returns:  
|     Handle of created symbol table entry.
|  
|  Statuses:  
|     ok, invalid_parent_vc, invalid_variable_template, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_variable_init
| 
|  Description:  
|     Runs the initialization snippet for a specified variable
| 
\* Syntax:                                                                  */

        void dob_variable_init(
          /* variable : in dob_data_object */
        );

/* 
|  Parameters:
|     variable - Handle of the variable to initialize
|     variable_template - Handle of the variable template.
|
|  Returns:  
|     Handle of created symbol table entry.
|  
|  Statuses:  
|     ok, invalid_parent_vc, invalid_variable_template, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_temp_create
| 
|  Description:  
|     Creates a temp instance in the the symbol table and the appropriate
|     object base mapping and returns a unique object handle.
| 
\* Syntax:                                                                  */

        dob_data_object dob_temp_create();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Handle of created symbol table entry.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_temp_release
|
|  Description:
|     Releases a temp instance and add it to the free list

\* Syntax:                                                                  */

	void dob_temp_release (
	  /* temp_object : in dob_data_object */
	);
/* 
|  Parameters:
|	temp_objct - handle of temporary to be released
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_data_object
|  
\*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*\
|  Routine: dob_variable_tell
| 
|  Description:  
|     Updates a variable instance in the symbol table if different from
|     existing data.
| 
\* Syntax:                                                                  */

        void dob_variable_tell(
          /* variable : in dob_data_object */
          /* type : in cus_sym_type  */
          /* data : in caddr_t  */
        );

/* 
|  Parameters:
|     variable - Handle of variable to be updated.
|     type - New type.
|     data - New data.
|     
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_symbol_type, invalid_data_object, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_object_create
| 
|  Description:  
|     Creates an object in the symbol table.
| 
\* Syntax:                                                                  */

        dob_data_object dob_object_create(
          /* parent_vc 		: in dob_data_object */
          /* object_template	: in dob_data_object */
        );

/* 
|  Parameters:
|     parent_vc - Handle of the parent vc.
|     object_template - Handle of the template to be used to define the object.
|
|  Returns:  
|     Handle of created objecdt.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_object_tell
| 
|  Description:  
|     Updates an object instance attribute in shared data if different
|     from existing data.  See shared data tell.
| 
\* Syntax:                                                                  */

#define dob_object_tell dob_shared_data_tell

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_vc_create
| 
|  Description:  
|     Creates a vc instance in the the symbol table and the appropriate
|     object base mapping and returns a unique id.
| 
\* Syntax:                                                                  */

        dob_data_object dob_vc_create(
          /* parent_vc : in dob_data_object */
          /* vc_template : in dob_data_object */
          /* creating_sd : in dob_data_object */
        );

/* 
|  Parameters:
|     parent_vc - Handle of the parent vc of the vc to be created.
|     vc_template - Handle of the vc template.
|     creating_sd - Handle of the creating shared data instance or null if 
|                   none.
|
|  Returns:  
|     Handle of created symbol table entry.
|  
|  Statuses:  
|     ok, invalid_parent_vc, invalid_vc_template, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_base_ask
| 
|  Description:  
|     Gets data out of the object base.  The data object may be either a
|     shared data element or component or a variable in the symbol table.
|     Status is set to invalid_symbol for all other symbols.
| 
\* Syntax:                                                                  */

        caddr_t dob_base_ask(
          /* data_object : in dob_data_object  */
          /* component_name : in string  */
	  /* propagateL in boolean */
        );

/* 
|  Parameters:
|     data_object - Handle of data object for which to get the data type.
|     component_name - Name of specific component if data object is a
|                      shared data element or object.
|     propagate - ask will propagate references if true
|
|  Returns:  
|     Pointer to data.
|  
|  Statuses:  
|     ok, invalid_symbol, invalid_data_object, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_ask
| 
|  Description:  
|     Gets data out of the object base.  The data object may be either a
|     shared data element or component or a variable in the symbol table.
|     Status is set to invalid_symbol for all other symbols.
| 
\* Syntax:                                                                  */

        caddr_t dob_ask(
          /* data_object : in dob_data_object  */
          /* component_name : in string  */
        );

/* 
|  Parameters:
|     data_object - Handle of data object for which to get the data type.
|     component_name - Name of specific component if data object is a
|                      shared data element or object.
|
|  Returns:  
|     Pointer to data.
|  
|  Statuses:  
|     ok, invalid_symbol, invalid_data_object, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_data_value
| 
|  Description:  
|     Just like dob_ask except dob_get_data_value evaluates through typed
|     buffers and returns the actual type.
| 
\* Syntax:                                                                  */

        caddr_t dob_get_data_value(
          /* data_object : in dob_data_object  */
          /* component_name : in string  */
        );

/* 
|  Parameters:
|     data_object - Handle of data object for which to get the data type.
|     component_name - Name of specific component if data object is a
|                      shared data element or object.
|
|  Returns:  
|     Pointer to data.
|  
|  Statuses:  
|     ok, invalid_symbol, invalid_data_object, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_symbol_type
| 
|  Description:  
|     Returns the type of the specified symbol.
| 
\* Syntax:                                                                  */

        cus_sym_type dob_get_symbol_type(
          /* symbol : in dob_data_object  */
        );

/* 
|  Parameters:
|     symbol - Handle of symbol for which to return symbol type.
|
|  Returns:  
|     Symbol type of specified symbol.
|  
|  Statuses:  
|     ok, invalid_symbol, symbol_not_found, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_symbol_entry
| 
|  Description:  
|     Returns the symbol table handle corresponding to the id or null if
|     one does not exist.
| 
\* Syntax:                                                                  */

        cus_sym dob_get_symbol_entry(
          /* symbol : in dob_data_object  */
        );

/* 
|  Parameters:
|     symbol - Handle of symbol for which to return symbol handle.
|
|  Returns:  
|     Symbol handle of specified symbol.
|  
|  Statuses:  
|     ok, invalid_symbol, symbol_not_found, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_name
| 
|  Description:  
|     Returns the name of the vc, object, variable or shared data element
|     represented by the Handle.
| 
\* Syntax:                                                                  */

        string dob_get_name(
          /* data_object : in dob_data_object  */
        );

/* 
|  Parameters:
|     data_object - Handle of data object for which to get the name.
|
|  Returns:  
|     Thingee name.
|  
|  Statuses:  
|     ok, invalid_data_object, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_real_name
| 
|  Description:  
|     Returns the real name of the vc, object, variable or shared data element
|     represented by the Handle.  This is the name specified by the slang
|     writer and not the name generated by the compiler.
| 
\* Syntax:                                                                  */

        string dob_get_real_name(
          /* data_object : in dob_data_object  */
        );

/* 
|  Parameters:
|     data_object - Handle of data object for which to get the name.
|
|  Returns:  
|     Thingee name.
|  
|  Statuses:  
|     ok, invalid_data_object, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_base_data_type
| 
|  Description:  
|     Gets the data type of the specified data object.  This means
|     if the data item is actually a buffer this routine returns a buffer.
| 
\* Syntax:                                                                  */

        idd_data_types dob_get_base_data_type(
          /* data_object : in dob_data_object  */
          /* component_name : in string  */
        );

/* 
|  Parameters:
|     data_object - Handle of data object for which to get the data type.
|     component_name - Name of specific component if data object is a
|                      shared data element or object.
|
|  Returns:  
|     Data type associated with specified data object.
|  
|  Statuses:  
|     ok, invalid_shared_data, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_get_data_type
| 
|  Description:  
|     Gets the data type of the specified data object.
| 
\* Syntax:                                                                  */

        idd_data_types dob_get_data_type(
          /* data_object : in dob_data_object  */
          /* component_name : in string  */
        );

/* 
|  Parameters:
|     data_object - Handle of data object for which to get the data type.
|     component_name - Name of specific component if data object is a
|                      shared data element or object.
|
|  Returns:  
|     Data type associated with specified data object.
|  
|  Statuses:  
|     ok, invalid_shared_data, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_destroy_data_object
| 
|  Description:  
|     Destroys the data object associated with the specified id and frees
|     all associated storage.
|     
\* Syntax:                                                                  */

        void dob_destroy_data_object(
          /* data_object : in out dob_data_object */
        );

/* 
|  Parameters:
|     data_object - Handle of data object to be destroyed.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, invalid_data_object.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dob_cleanup
| 
|  Description:  
|      Performs necessary cleanup of the object base manager.
|     
\* Syntax:                                                                  */

        void dob_cleanup();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/
#endif
