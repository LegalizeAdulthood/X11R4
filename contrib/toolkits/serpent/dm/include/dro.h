/*
 $Header: /u/04c/mnt/integ/dm/include/RCS/dro.h,v 1.7 89/05/22 11:15:08 bmc Exp $
*/

/*--------------------------------------------------------------------------*\
|
|  Name:  Run Time Operations Utility (dro)
|
|  Description:  
|     The run time operations utility provides various run time arithmetic
|     operations for the run time portion of the dialogue manager.  The
|     run time operations utility works on data objects stored in the
|     object base.  These may be either symbols or shared data elements.
|
|  Component of:
|     Dialogue Manager Layer.
|
|  Related files:
|     dro.c - contains the implementation for the transactionmanager.
|     dro.h - contains the external definition for the transaction manager.
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
$Log:	dro.h,v $
 * Revision 1.7  89/05/22  11:15:08  bmc
 * No longer includes 'uer.h'.  Instead, 'uer.h' is explicitly included by
 * those files that need it.  Change was necessary to accomodate changes
 * to 'uer.h'.          
 * 
 * Revision 1.6  89/05/19  17:12:40  bmc
 * Changed definition of 'dro_atomic_data_item': Instead of containing an
 * id, it now contains an object handle.
 * 
 * Revision 1.5  89/02/09  16:52:18  ejh
 * added copyright notice
 * 
 * Revision 1.4  88/12/19  17:53:02  rcs
 * moved dro_boolean and dro_dot in from dro_more
 * 
 * Revision 1.3  88/12/16  15:26:38  rcs
 * eliminated dro_base_undefined
 * changed dro_undefined to return boolean
 * 
 * Revision 1.2  88/10/25  16:46:10  little
 * add the sp11 value to dro_handling enmu
 * 
|     June 14, 1988 : Module created.
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

#ifndef dro
#define dro

/*--------------------------------------------------------------------------*\
|  Includes: isd
|
|  Description:
|     The following is a list of all the necessary includes for the 
|     run time operations utility.
|
\* Definition:                                                              */

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
#include "drc.h"		/* runtime conversions			    */

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: dro_debug
|
|  Description:
|     Debug variable for the object base abstraction.
|
\* Definition:                                                              */

#ifdef dro_main
  int dro_debug = 3;
#else
  extern int dro_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Type: dro_atomic_data_item
|
|  Description:
|     The operand data structure is used to uniquely identify
|     operands in either shared data or the symbol table and the
|     results of arithmetic operations.
|
\* Definition:                                                              */

	typedef struct {
	  dob_data_object dob_handle;
	  string component_name;
	} dro_atomic_data_item;

        typedef dro_atomic_data_item *dro_adi_handle;

/*
|  Components:
|     dob_handle     - The handle of the operand in the object base (dob).
|     component_name - The component name of the operand (if shared data).
|
\*--------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------
|    typedef dro_handling:
*/
typedef enum handling {
  bool	=0,		/* convert both operands to this type		    */
  integ	=1,
  real	=2,
  str	=3,
  rcd	=4,
  id	=5,
  buff	=6,
  undef	=7,
  no_sp, 		/* no special processing			    */
  sp1, 			/* special processing as defined for operation	    */
  sp2, 
  sp3, 
  sp4, 
  sp5, 
  sp6, 
  sp7,
  sp8,
  sp9,
  sp10,
  sp11
} dro_handling;
/*
--------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------ *\
|  Routine: dro_check_string
|
|  Description:
|	Check if string operand type is reducible to integer or real and
|	return the new type.
\* Syntax:								    */
	idd_data_types dro_check_string(
		/* op : dro_handling; */
	);

/* 
|  Parameters:
|     op - First operand.
|
|  Returns:  
|     new idd_data_types if operand is string type and reducible to real
|	or integer.  Same operand type otherwise.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dro_select_type
| 
|  Description:  
|     Select most appropriate type (of string, integer, and real)
| 
\* Syntax:                                                                  */

	idd_data_types dro_select_type(
		/* t1 : idd_data_types */
		/* t2 : idd_data_types */
	);
/* 
|  Parameters:
|     t1 - first idd_data type
|     t2 - second idd_data type
|
|  Returns:  
|	iddstring, iddreal, or iddinteger (highest priority to lowest)
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dro_ask2
| 
|  Description:  
|     Performs the same function as dro_ask except it accepts a 
|     dro_atomic_data_item as a paramter.
| 
\* Syntax:                                                                  */

        caddr_t dro_ask2(
          /* data_object : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     data_object - ADI for object being queried.
|
|  Returns:  
|     Pointer to data.
|  
|  Statuses:  
|     ok, invalid_symbol, invalid_data_object, out_of_memory.
|  
\*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
|  Routine: dro_assign
| 
|  Description:  
|     Assigns the value of the second operand to the first operand.
| 
\* Syntax:                                                                  */

        void dro_assign(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, invalid_data_type,
|     illegal_assignnment, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_add
| 
|  Description:  
|     Adds the values of two data objects and returns a temp.  Data objects
|     may be either symbol table entries or shared data elements.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_add(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     The id of the temp containing the sum of the operands.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_subtract
| 
|  Description:  
|     Subtracts the second operand from the first operand, i.e. op1 - op2.
|     and returns the difference.  Data objects
|     may be either symbol table entries or shared data elements.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_subtract(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     The id of the temp containing the difference of the operands.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_multiply
| 
|  Description:  
|     Multiplies the operands.  Data objects may be either symbol table 
|     entries or shared data elements.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_multiply(
          /* op1 : in dro_atomic_dat_item */
          /* op2 : in dro_atomic_dat_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     The id of the temp containing the product of the operands.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_divide
| 
|  Description:  
|     Divides the first operand by the second operand, i.e. op1/op2.
|     and returns the quotient.  Data objects
|     may be either symbol table entries or shared data elements.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_divide(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     The id of the temp containing the quotient of the operands.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_exponentiate
| 
|  Description:  
|     Raises the first operand to the power of the second operand, i.e.
|     op1**op2.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_exponentiate(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     The id of the temp containing the result of the exponentiation.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_equal
| 
|  Description:  
|     Tests for equality between the two operands.  
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_equal(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Returns true if equal, false if not equal.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_not_equal
| 
|  Description:  
|     Tests for inequality between the two operands.
| 
\* Syntax:                                                                  */

#define dro_not_equal(op1, op2) dro_not(dro_equal(op1, op2))

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Returns true if not equal, false if equal.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_greater
| 
|  Description:  
|     Tests to see if the first operand is greater than the second, i.e.
|     op1 > op2.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_greater(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Returns true if greater, false if not greater.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_less
| 
|  Description:  
|     Tests to see if the first operand is less than the second, i.e.
|     op1 < op2.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_less(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Returns true if less, false if not less.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_less_than_or_equal
| 
|  Description:  
|     Tests to see if the first operand is less than or equal than the second,
|     i.e. op1 < op2.
| 
\* Syntax:                                                                  */

#define dro_less_than_or_equal(op1, op2) dro_not(dro_greater(op1, op2))

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Returns true if less than or equal, false if not less than or equal.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_greater_than_or_equal
| 
|  Description:  
|     Tests to see if the first operand is greater than or equal than 
|     the second, i.e. op1 < op2.
| 
\* Syntax:                                                                  */

#define dro_greater_than_or_equal(op1, op2) dro_not(dro_less(op1, op2))

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Returns true if greater than or equal, false if less than.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_and
| 
|  Description:  
|     Performs an and operation on the operands.
| 
\* Syntax:                                                                  */

         dro_atomic_data_item dro_and(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Returns the result of an and operation on the two operands.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_or
| 
|  Description:  
|     Performs an or operation on the operands.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_or(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|     op2 - Second operand.
|
|  Returns:  
|     Returns the result of an or operation on the two operands.
|  
|  Statuses:  
|     ok, first_operand_invalid, second_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_not
| 
|  Description:  
|     Performs a not operation on the operand.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_not(
          /* op : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|
|  Returns:  
|     Returns the result of an not operation on the operand.
|  
|  Statuses:  
|     ok, first_operand_invalid, illegal_boolean_operation, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_negate
| 
|  Description:  
|     Performs a negation of the operand.
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_negate(
          /* op : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - First operand.
|
|  Returns:  
|     Returns the result of the negation of the operand.
|  
|  Statuses:  
|     ok, first_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_undefined
| 
|  Description:  
|     Returns true if undefined.  This should probably be a dob function
|     rather than a dro function.
| 
\* Syntax:                                                                  */

        boolean dro_undefined(
          /* op1 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - Operand.
|
|  Returns:  
|     Returns true if specified operand is undefind; false if defined.
|  
|  Statuses:  
|     ok, first_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_boolean
| 
|  Description:  
|     Returns value of a boolean adi.  Undefined causes a run time error.
| 
\* Syntax:                                                                  */

        boolean dro_boolean(
          /* op1 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - Operand.
|
|  Returns:  
|     Returns value of a boolean adi.
|  
|  Statuses:  
|     ok, first_operand_invalid, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: dro_dot
| 
|  Description:  
|     ???
| 
\* Syntax:                                                                  */

        dro_atomic_data_item dro_dot(
          /* op1 : in dro_atomic_data_item */
          /* op2 : in dro_atomic_data_item */
        );

/* 
|  Parameters:
|     op1 - Operand.
|     op2 - Operand.
|
|  Returns:  
|     ???
|  
|  Statuses:  
|     ???
|  
\*--------------------------------------------------------------------------*/
#endif
