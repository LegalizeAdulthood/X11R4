static char rcsid [] =
        "$Header: /u/04c/mnt/integ/dm/src/RCS/dro1.c,v 1.24 89/11/02 16:17:25 little Exp $";

/*
??? propagate undefined in all run-time functions
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
|     Stephen T. Chou
|     stc@sei.cmu.edu
|
|  Change History:
|     June 14, 1988 : Module created.
|     Oct 3, 1988: change conversion table to allow string <--> buffer  ljb
|
$Log:	dro1.c,v $
 * Revision 1.24  89/11/02  16:17:25  little
 * no changes
 * 
 * Revision 1.23  89/10/05  16:58:27  rcs
 * various mods to support typed buffers
 * 
 * Revision 1.22  89/09/01  14:10:23  bmc
 * Removed error_msg buffers in individual routines; they now use uer_error_msg
 * supplied by uer.h
 * 
 * Revision 1.21  89/07/18  09:20:28  ljb
 * no changes
 * 
 * Revision 1.20  89/06/19  13:05:44  ljb
 * allow for equality tests involving boolean and other types
 * 
 * Revision 1.19  89/06/15  15:27:35  ljb
 * allow conversions and comparisions between boolean and other types
 * allow conversions and comparisions between buffer and other types
 * 
 * Revision 1.18  89/05/22  11:11:56  bmc
 * Now explicitly include 'uer.h'.  Related to changes to 'dob.c', 'dob.h'
 * and 'uer.h'.
 * 
 * Revision 1.17  89/05/19  17:24:13  bmc
 * Changed to accomodate new definition of 'dro_atomic_data_item'.  (See
 * 'dro.h' for more information.)  Also, changed calls to dob routines
 * to be consistent with modifications in 'dob.h'.
 * 
 * Revision 1.16  89/04/11  14:42:59  rcs
 * added support for string to id and id to string conversions
 * 
 * Revision 1.15  89/02/09  17:44:14  ejh
 * added copyright notice
 * 
 * Revision 1.14  89/02/06  17:44:38  rcs
 * added debug print
 * added buffer compare, mmaybe
 * 
 * Revision 1.13  89/01/30  13:40:49  rcs
 * added run time error checking
 * added support for object_sym
 * 
 * Revision 1.12  89/01/09  14:52:30  little
 * change the error message in illegal assignment from the incorrect
 * UER_ILLEGAL_RECORD_EQUATE to the correct UER_ILLEGAL_ASSIGNMENT
 * 
 * Revision 1.11  88/12/19  17:49:22  rcs
 * upgraded run time error handling
 * 
 * Revision 1.8  88/12/14  11:15:46  little
 * fix a bad switch statement in dro_assign
 * 
 * Revision 1.7  88/11/04  17:03:52  little
 * change DEBUG1 to print d22_slang_source
 * 
 * Revision 1.6  88/10/25  16:48:09  little
 * add comparison of buffers and strings using sp11 special processing
 * flag
 * 
 * Revision 1.5  88/10/12  09:44:39  ljb
 * allow conversion between buffer and integer
 * 
 * Revision 1.3  88/10/04  17:49:19  little
 * take the  free_node(lhs_data) out of dro_assign string case
 * 
 * Revision 1.2  88/10/03  14:52:15  ljb
 * allow conversion from buffer to strings and vice versa
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

#define dro_main
#include "dro.h"                          /* own header file		    */
#include "dra.h"
#include "d22_debug.h"
#include <signal.h>
#include "dob.h"
#include "uer.h"			/* error reporting facility	    */

#define DEBUG1(statement) DM_RT_DEBUG1(dro_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(dro_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(dro_debug, statement)


/*  ??? this should be in isc.h	*/

#define FATAL_ERROR(statement)\
   {  long pid;\
      char command [256];\
   fprintf(stderr, "!!!!!     DM FATAL ERROR, at or near SLANG line %d\n",\
        d22_slang_line_num);\
   fprintf (stderr, "\t  in %s\n", d22_slang_source);\
   fprintf(stderr, "   file %s, line %d\n", __FILE__, __LINE__);\
   statement;\
   fflush(stderr);\
   pid = getpid();\
   sprintf(command, "gcore %d", pid);\
   system(command);\
   sprintf(command, "mv core.%d core", pid);\
   system(command);\
   exit(1);\
   }


/*
??? d22_slang_line_num should be in an include file, but there are collisions
*/
extern int d22_slang_line_num;



#define NO_OF_TYPES 8

/*
???  integ <- id and id <- integ, should be sp7
*/
/*			-- FROM --
 bool	integ	real	string	record	id	buff	undef	-- TO --
*/
dro_handling assignment[NO_OF_TYPES][NO_OF_TYPES] = {
  bool, sp1,    sp1,    sp1,    sp7,    sp7,    sp7,    sp10,   /* bool     */
  sp1,  integ,  sp1,    sp2,    sp7,    integ,  sp7,    sp10,   /* integ    */
  sp1,  sp3,    real,   sp4,    sp7,    sp7,    sp7,    sp10,   /* real     */
  sp1,  sp5,    sp5,    str,    sp7,    sp11,   sp7,    sp10,   /* string   */
  sp7,  sp7,    sp7,    sp7,    rcd,    sp7,    sp7,    sp10,   /* record   */
  sp7,  integ,  sp7,    sp11,   sp7,    id,     sp7,    sp10,   /* id       */
  buff, buff,   buff,   buff,   sp7,    buff,   buff,   sp10,   /* buffer   */
  sp6,  sp6,    sp6,    sp6,    sp6,    sp6,    sp6,    sp6     /* undef    */
};
/*			-- FROM --
 bool	integ	real	string	record	id	buff	undef	-- TO --
*/
dro_handling binary_arithmetic_ops[NO_OF_TYPES][NO_OF_TYPES] = {
  sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	/* bool	    */
  sp4,	integ,	sp3,	sp1,	sp4,	sp4,	sp4,	undef,	/* integ    */
  sp4,	sp3,	real,	sp2,	sp4,	sp4,	sp4,	undef,	/* real     */
  sp4,	sp1,	sp2,	sp5,	sp4,	sp4,	sp4,	undef,	/* string   */
  sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	/* record   */
  sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	/* id       */
  sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	sp4,	/* buffer   */
  sp4,	undef,	undef,	undef,	sp4,	sp4,	sp4,	undef	/* undef    */
};

/*			-- FROM --
 bool	integ	real	string	record	id	buff	undef	-- TO --
*/
dro_handling binary_boolean_ops[NO_OF_TYPES][NO_OF_TYPES] = {
  bool,	bool,	bool,	bool,	sp4,	sp4,	sp7,	sp6,	/* bool	    */
  bool,	integ,	sp1,	sp1,	sp4,	sp4,	sp7,	sp6,	/* integ    */
  bool,	sp3,	real,	sp2,	sp4,	sp4,	sp7,	sp6,	/* real	    */
  bool,	sp1,	sp2,	str,	sp4,	sp4,	sp7,	sp6,	/* string   */
  rcd,	rcd,	rcd,	rcd,	rcd,	rcd,	rcd,	sp6, 	/* record   */
  sp4,	sp4,	sp4,	sp4,	sp4,	id,	sp7,	sp6, 	/* id	    */
  sp7,	sp7,	sp7,	sp7,	sp7,	sp7,	buff,	sp6, 	/* buffer   */
  sp6,	sp6,	sp6,	sp6,	sp6,	sp6,	sp7,	undef	/* undef    */
};

/* ----------------------------------------------------------------------- *\
|  Routine: dro_check_string
|  
|  Description:
|    Check if a string is convertible to integer or real and return
|    the new type.
\* ----------------------------------------------------------------------- */
idd_data_types dro_check_string(op)
dro_atomic_data_item op;
{
  idd_data_types op_type;
  caddr_t op_data;
  int stat;
/*
   Initialization.
*/
  set_status(ok);                         /* begin                          */

  DEBUG2(
    fprintf(stderr, "dro_check_string: call.\n");
    fprintf(stderr, "dro_check_string: op.dob_handle = %#x\n", op.dob_handle);
    fprintf(
      stderr, 
      "dro_check_string: op.component_name = %d\n", 
      op.component_name
    );
  );

  op_type = dob_get_data_type(op.dob_handle, op.component_name);
  if (op_type == idd_string) {
    op_data =(caddr_t)dob_ask(op.dob_handle, op.component_name);
    if (stat = isinteger(op_data)) {
      DEBUG3(
        fprintf(stderr, "dro_check_string: integer ? %d\n", stat);
      );
      return idd_integer;
    }

    if (stat = isreal(op_data)) {
      DEBUG3(
        fprintf(stderr, "dro_check_string: real ? %d\n", stat);
      );
      return idd_real;
    }

  }

  DEBUG2(
    fprintf(stderr, "dro_check_string: exit.\n");
  );

  return op_type;
}

/* ----------------------------------------------------------------------- *\
|  Routine: dro_select_type
|  
|  Description:
|    get most appropriate idd_data_type of the two
|    ???? bad, bad software engineering
\* ----------------------------------------------------------------------- */
idd_data_types dro_select_type(t1, t2)
idd_data_types t1, t2;
{
/*
   Initialization.
*/
  set_status(ok);                         /* begin                          */

  DEBUG2(
    fprintf(stderr, "dro_select_type: call.\n");
  );

  DEBUG2(
    fprintf(stderr, "dro_select_type: exit.\n");
  );

  if (t1 == t2) return t1;
  if ((int)t1 >(int)t2) return t1;
  else return t2;
}

/* ----------------------------------------------------------------------- *\
|  Routine: dro_power
|  
|  Description:
|    find integer exponent of an integer
|
\* ----------------------------------------------------------------------- */
int dro_power(a, b)
int a, b;
{
  int i, product = 1;
/*
   Initialization.
*/
  set_status(ok);                         /* begin                          */

  DEBUG2(
    fprintf(stderr, "dro_power: call.\n");
  );

  for(i = 0; i < b; ++i)
        product = product * a;

  DEBUG2(
    fprintf(stderr, "dro_power: exit.\n");
  );

  return product;
}

/* ----------------------------------------------------------------------- *\
|  Routine: dro_rpow
|  
|  Description:
|    find exponent of real
|
\* ----------------------------------------------------------------------- */
double dro_rpow(a, b)
double a, b;
{
  double result;
/*
   Initialization.
*/
  set_status(ok);                         /* begin                          */

  DEBUG2(
    fprintf(stderr, "dro_rpow: call.\n");
  );

  result = pow(a, b);

  /* check if erron is set to EDOM or ERANGE */
  if ((errno == 33) || (errno == 34)) {
	set_status(failed_conversion);
	return NULL;
  }

  DEBUG2(
    fprintf(stderr, "dro_rpow: exit.\n");
  );

  return result;
}

/*--------------------------------------------------------------------------*\
|  Routine: dro_ask2
| 
|  Description:  
|     Gets data out of the object base.
\*--------------------------------------------------------------------------*/
caddr_t dro_ask2(data_object)
	dro_atomic_data_item data_object;
{                                         /* local type definitions         */
  caddr_t data;
/*
   Initialization.
*/
  set_status(ok);                         /* begin                          */

  DEBUG2(
    fprintf(stderr, "dro_ask2: call.\n");
    fprintf(
      stderr, 
      "dro_ask2: data_object.dob_handle = %#x\n",
      data_object.dob_handle
    );
  );
/*
   Call ask.
*/
  data = dob_ask(data_object.dob_handle, data_object.component_name);
  rcheck_status("dro_ask2: bad status get dob_ask.", NULL);
/*
   Return.
*/
  DEBUG2(fprintf(stderr, "dro_ask2: exiting.\n"););

  return data;
}                                       /* end dro_ask2                     */

/*--------------------------------------------------------------------------*\
|  Routine: dro_assign
|
|  Description:
|     Assigns the value of the second operand to the first operand.
\*--------------------------------------------------------------------------*/
void dro_assign(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  idd_data_types op1_type, op2_type;
  caddr_t op2_data;
  dro_handling handling;
  cus_sym_type symbol_type;
  char answer[256];
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(
    fprintf(stderr, "dro_assign: call.\n");
    fprintf(stderr, "dro_assign: op1.dob_handle = %#x.\n", op1.dob_handle);
    fprintf(
      stderr, "dro_assign: op1.component_name = %s.\n", op1.component_name
    );
    fprintf(stderr, "dro_assign: op2.dob_handle = %#x.\n", op2.dob_handle);
    fprintf(
      stderr, "dro_assign: op2.component_name = %s.\n", op2.component_name
    );
  );
/* 
   Determine the type and value of the second operand.
*/
  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_check_status_binop(
    "dro_assign: bad status from dob_get_data_type op2",
    ":="
  );

  op2_data = (caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_check_status_binop(
    "dro_assign: bad status from dob_base_ask of op2",
    ":="
  );
/*
   Get the symbol type of the target.  
*/
  symbol_type = dob_get_symbol_type(op1.dob_handle);
  uer_check_status_binop(
    "dro_assign: bad status from dob_get_symbol_type",
    ":="
  );
/*
   If the symbol is a variable then assign the type and value of the 
   second operand to the first operand.
*/
  if (symbol_type == variable_sym) {	/* symbol is a variable		    */

    dob_variable_tell(op1.dob_handle, op2_type, op2_data);
    uer_check_status_binop(
      "dro_assign: bad status from dob_variable_tell",
      ":="
    );
      
  }					/* end if symbol is a variable	    */
/*
   Else the type is shared data.  In this case use the conversion table in
   order to determine the appropriate handling.
*/
  else {				/* shared data			    */
/* 
   Determine the base data type of the first operand.  The value or
   actual data type is not needed since we are just going to cream it.
*/
    op1_type = dob_get_base_data_type(op1.dob_handle, op1.component_name);
    uer_check_status_binop(
      "dro_assign: bad status from dob_get_data_base_type of op1",
      ":="
    );

    op2_data = drc_get_atv(op2, op1_type);
    uer_check_status_binop(
      "dro_assign: bad status from drc_get_atv",
      ":="
    );

    handling = assignment[(int)op1_type][(int)op2_type];

    DEBUG3(
      fprintf(stderr, "assign: handling = %d\n", handling);
    );

    switch(handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Integer, boolean, id
\*--------------------------------------------------------------------------*/
/*   
   Assign the value of the second operand to the first operand.  The 
   first operand's type is assumed to be the same.  The space for the
   value is assumed to exist for these types.
*/

      case integ:
      case bool:
      case id:

        dob_shared_data_tell(op1.dob_handle, op1.component_name, op2_data);
        uer_check_status_binop(
          "dro_assign: bad status from dob_shared_data_tell",
          ":="
        );

        break;

/*--------------------------------------------------------------------------*\
|  Real.
\*--------------------------------------------------------------------------*/
/*
   Copy the real number into the previously allocated space, then call
   variable tell to cause the propogate.
*/
      case real:

        dob_shared_data_tell(op1.dob_handle, op1.component_name, op2_data);
        uer_check_status_binop(
          "dro_assign: bad status from dob_shared_data_tell",
          ":="
        );

        break;

/*--------------------------------------------------------------------------*\
|  String.
\*--------------------------------------------------------------------------*/
/*
   If the length of the op1 string is less then the length of the op2 string
   free the space occupied by the existing op1 string and allocate space the
   size of the op2 string.
*/
      case str:

        dob_shared_data_tell(op1.dob_handle, op1.component_name, op2_data);
        uer_check_status_binop(
          "dro_assign: bad status from dob_shared_data_tell",
          ":="
        );


        break;

/*--------------------------------------------------------------------------*\
|  Record.
\*--------------------------------------------------------------------------*/
      case rcd:

        dob_shared_data_tell(op1.dob_handle, op1.component_name, op2_data);
        uer_check_status_binop(
          "dro_assign: bad status from dob_shared_data_tell",
          ":="
        );

        break;

/*--------------------------------------------------------------------------*\
|  Buffer.  Anything besides a record can be represented as a buffer.
\*--------------------------------------------------------------------------*/
      case buff:

        dob_shared_data_tell(op1.dob_handle, op1.component_name, op2_data);
        uer_check_status_binop(
          "dro_assign: bad status from dob_shared_data_tell",
          ":="
        );

        break;

/*--------------------------------------------------------------------------*\
|  sp1:  convert the source(op2) value to the type of the target and 
|        assign to the target value(op1).
\*--------------------------------------------------------------------------*/
      case sp1:
      case sp2:
      case sp3:
      case sp4:
      case sp5:
      case sp11:  /* string to id and id to string */

#if 0

        op2_data = drc_get_atv(op2, op1_type);
        uer_check_status_binop(
          "dro_assign: bad status from drc_get_atv",
          ":="
        );
#endif
        dob_shared_data_tell(op1.dob_handle, op1.component_name, op2_data);
        uer_check_status_binop(
          "dro_assign: bad status from dob_shared_data_tell of op2_data",
          ":="
        );

        break;

/*--------------------------------------------------------------------------*\
|  sp7: run time error
\*--------------------------------------------------------------------------*/

      case sp7:

        DEBUG1(
          fprintf(stderr, "dro_assign: illegal assignment.\n");
        );

        sprintf(uer_error_msg, UER_ILLEGAL_ASSIGNMENT);

        uer_report_binary_op_error(
          uer_error_msg,
          op1,
          op2
        );

	return;

/*--------------------------------------------------------------------------*\
|  sp10: undefined to something
\*--------------------------------------------------------------------------*/
      case sp10:
/*
	 case on type of op1, and put the correct undefined value
	 into the sd component, via dob_shared_data_tell
*/
        switch(op1_type) {			/* switch on op1 type	    */

          case idd_boolean:
            dob_shared_data_tell(op1.dob_handle, op1.component_name, 
				UNDEFINED_BOOLEAN);
            break;

          case idd_integer:
            dob_shared_data_tell(op1.dob_handle, op1.component_name,
             			    UNDEFINED_INTEGER);
            break;

          case idd_real:
            dob_shared_data_tell(op1.dob_handle,
				 op1.component_name,
				 UNDEFINED_REAL);
            break;

          case idd_string:
            dob_shared_data_tell(op1.dob_handle,
				 op1.component_name,
                		 UNDEFINED_STRING);
            break;

          case idd_id:
            dob_shared_data_tell(op1.dob_handle,
				 op1.component_name,
				 UNDEFINED_ID);
            break;

          case idd_buffer:
             dob_shared_data_tell(
               op1.dob_handle, 
               op1.component_name,
               UNDEFINED_BUFFER_LENGTH
             );
             break;

          case idd_record:

            DEBUG1(
              fprintf(
                stderr, 
                "dro_assign: can not assign record to undefined.\n"
              );
            );

            sprintf(uer_error_msg, UER_RECORD_UNDEFINED);

            uer_report_binary_op_error(
              uer_error_msg,
              op1,
              op2
            );

            break;

          default:

            FATAL_ERROR(
              fprintf(
                stderr,
                "unknown type of %d in dro_assign for sp10\n", 
                op1_type
              );
            );

        }				/* end switch data type		    */

        uer_check_status_binop(
          "dro_assign: bad status from dob_shared_data_tell",
          ":="
        );

        break;

/*--------------------------------------------------------------------------*\
|  default:  Fatal Error
\*--------------------------------------------------------------------------*/

      sp6:  /* it should be impossible to assign to an undefined sd element */
      default:                          /* unknown handling type            */

        FATAL_ERROR(
          fprintf(
            stderr, 
            "dro_assign: unknown handling = %d.\n",
            handling
          );
        );

        break;
    }				/* end switch on specific handling	    */
  }				/* end else symbol 			    */
/*
   Delete the symbol, if it is a temp, and destroy the atomic data item.
*/
  dra_delete_temp(op2);
  uer_check_status_binop(
    "dro_assign: bad status from dra_delete_temp for op2",
    ":="
  );
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_assign: exiting.\n");
  );
  
  return;

}					/* end dro_assign		    */

/*--------------------------------------------------------------------------*\
|  Routine: dro_add
|
|  Description:
|     Adds the values of two data objects and returns a temp.  Data objects
|     may be either symbol table entries or shared data elements.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_add(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type, t1, t2, t3;
  dob_data_object new_handle;
  caddr_t op1_data, op2_data;
  dro_handling handling;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(fprintf(stderr, "dro_add: call.\n");
    fprintf(stderr, "dro_add: op1.dob_handle = %#x.\n", op1.dob_handle);
    fprintf(stderr, 
      "dro_add: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_add: op2.dob_handle = %#x.\n", op2.dob_handle);
    fprintf(stderr, 
      "dro_add: op2.component_name = %s.\n", 
      op2.component_name);
  );

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;
/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_add: bad status from dob_temp_create",
    "+", 
    temp
  );
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_add: bad status from dob_get_data_type of op1",
    "+", temp);

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_add: bad status from dob_get_data_type of op2",
    "+", temp);

  op1_data =(caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_add: bad status from dob_get_data_value of op1",
    "+", temp);

  op2_data =(caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_add: bad status from dob_get_data_value of op2",
    "+", temp);
/*
   Set the current type and value properties for the temp as appropriate.
*/
  handling = binary_arithmetic_ops[(int)op1_type][(int)op2_type];

  switch(handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Integer
\*--------------------------------------------------------------------------*/

    case integ:

      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_add: bad status from drc_get_atv of op1",
        "+", temp);

      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_add: bad status from drc_get_atv of op2",
        "+", temp);
/*
   Update temp with sum of values and set type to integer.
*/
      dob_variable_tell(new_handle, idd_integer,
		(char *)((int)op1_data + (int)op2_data));
      uer_rcheck_status_binop(
        "dro_add: bad status from dob_variable_tell",
        "+", temp);

      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/

    case real:
/*
   Convert both operands values to reals.
*/
      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_add: bad status from drc_get_atv",
        "+", temp);

      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_add: bad status from drc_get_atv",
        "+", temp);
/*
   Update temp with sum of values and set type to real.
*/
      DEBUG3(
        fprintf(stderr, "dro: %f + %f = ",
          *((double *)op1_data), *((double *)op2_data));
      );

      *((double *)op1_data) = *((double *)op1_data) + *((double *)op2_data);

      DEBUG3(
        fprintf(stderr, "%f\n", *((double *)op1_data));
      );

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_add: bad status from dob_variable_tell",
        "+", temp);

      break;

/*--------------------------------------------------------------------------*\
|  Operation undefined types.
\*--------------------------------------------------------------------------*/

    case bool:
    case str:
    case rcd:
    case id:
    case buff:
      sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "addition");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  Undefined.
\*--------------------------------------------------------------------------*/

    case undef: 
/*
   Set temp type to undefined.
*/
      dob_variable_tell(new_handle, idd_undefined, NULL);
      uer_rcheck_status_binop(
        "dro_add: bad status from dob_variable_tell",
        "+", temp);

      break;

/*--------------------------------------------------------------------------*\
|  sp1, sp2, sp3:  convert the operands to reals and sum.
\*--------------------------------------------------------------------------*/

    case sp1:
    case sp2:
    case sp5:

      t1 =(idd_data_types)dro_check_string(op1);
      uer_rcheck_status_binop(
        "dro_add: bad status from dro_check_string op1",
        "+", temp);

      t2 =(idd_data_types)dro_check_string(op2);
      uer_rcheck_status_binop(
        "dro_add: bad status from dro_check_string of op2",
        "+", temp);

      t3 = dro_select_type(t1, t2);
      uer_rcheck_status_binop(
        "dro_add: bad status from dro_select_type",
        "+", temp);

      if (t3 == idd_string) {		/* string type			    */

        sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "addition");

        uer_report_binary_op_error(
          uer_error_msg,
          op1,
          op2
        );

	return temp;
      }					/* end if string type		    */
      else {				/* else not string type		    */

        op1_data = drc_get_atv(op1, t3);
        uer_rcheck_status_binop(
          "dro_add: bad stateus from drc_get_atv of op1",
          "+", temp);

        op2_data = drc_get_atv(op2, t3);
        uer_rcheck_status_binop(
          "dro_add: bad status from drc_get_atv of op2",
          "+", temp);


        if (t3 == idd_real) {
          *((double *)op1_data) = *((double *)op1_data)+*((double *)op2_data);
	}
        else {
          op1_data =(caddr_t)((int)op1_data + (int)op2_data);
	}

        dob_variable_tell(new_handle, t3, op1_data);
        uer_rcheck_status_binop(
          "dro_add: bad status from dob_variable_tell",
          "+", temp);

      }					/* end else not string type	    */

      break;

    case sp3:
/*
   Convert operands to reals.
*/
      op1_data = drc_get_atv(op1, idd_real);
      uer_rcheck_status_binop(
        "dro_add: bad status from drc_get_atv of op1",
        "+", temp);

      op2_data = drc_get_atv(op2, idd_real);
      uer_rcheck_status_binop(
        "dro_add: bad status from drc_get_atv of op2",
        "+", temp);
/*
   Perform add operation and update type and value of temp.
*/
      *((double *)op1_data) = *((double *)op1_data) + *((double *)op2_data);

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_add: bad status from dob_variable_tell",
        "+", temp);

      break;

/*--------------------------------------------------------------------------*\
|  sp4: run time error
\*--------------------------------------------------------------------------*/
    case sp4:

      sprintf(uer_error_msg, UER_ILLEGAL_ARITH_CONVERSION, "addition");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  default:  fatal error
\*--------------------------------------------------------------------------*/
    default:                          /* unknown handling type              */

      FATAL_ERROR(
        fprintf(
          stderr, 
          "dro_add: unknown handling = %d.\n",
          handling
        );
      );

      break;
  }				/* end switch on specific handling	    */
/* 
   Delete parameters if they are temps 
*/
   dra_delete_temp(op1);
   uer_rcheck_status_binop(
     "dro_add: bad status from dra_delete_temp for op1",
     "+", temp);

   dra_delete_temp(op2);
   uer_rcheck_status_binop(
     "dro_add: bad status from dra_delete_temp for op2",
     "+", temp);
/* 
   Everything ok, set returns handle value 
*/
   temp.dob_handle = new_handle;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_add: temp.dob_handle = %#x.\n", temp.dob_handle);
    fprintf(stderr, "dro_add: exiting.\n");
  );

  return temp;
}					/* end dro_add			    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_subtract
| 
|  Description:  
|     Subtracts the second operand from the first operand, i.e. op1 - op2.
|     and returns the difference.  Data objects
|     may be either symbol table entries or shared data elements.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_subtract(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type, t1, t2, t3;
  caddr_t op1_data, op2_data;
  dob_data_object new_handle;
  dro_handling handling;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(fprintf(stderr, "dro_subtract: call.\n");
    fprintf(stderr, "dro_subtract: op1.dob_handle = %#x.\n", op1.dob_handle);
    fprintf(stderr, "dro_subtract: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_subtract: op2.dob_handle = %#x.\n", op2.dob_handle);
    fprintf(stderr, "dro_subtract: op2.component_name = %s.\n", 
      op2.component_name);
  );

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;

/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_subtract: bad status from dob_temp_create",
    "-", temp);
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_subtract: bad status from dob_get_data_type for op1",
    "-", temp);

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_subtract: bad status from dob_get_data_type for op2",
    "-", temp);

  op1_data =(caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_subtract: bad status from dob_get_data_value for op1",
    "-", temp);

  op2_data =(caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_subtract: bad status from dob_get_data_value for op2",
    "-", temp);
/*
   Set the current type and value properties for the temp as appropriate.
*/
  handling = binary_arithmetic_ops[(int)op1_type][(int)op2_type];

  switch(handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Integer
\*--------------------------------------------------------------------------*/

    case integ:

      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from drc_get_atv for op1",
        "-", temp);

      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from drc_get_atv for op1",
        "-", temp);
/*
   Update temp with sum of values and set type to integer.
*/
      dob_variable_tell(
        new_handle, 
        idd_integer, 
        (char *)((int)op1_data - (int)op2_data)
      );
      uer_rcheck_status_binop(
        "dro_subtract: bad status from dob_variable_tell",
        "-", temp);

      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/

    case real:
/*
   Convert both operands values to reals.
*/
      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from drc_get_atv of op1_data",
        "-", temp);

      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from drc_get_atv of op2_data",
        "-", temp);
/*
   Update temp with sum of values and set type to real.
*/
      DEBUG3(fprintf(stderr, "dro: %f + %f = ",
          *((double *)op1_data), *((double *)op2_data)););

      *((double *)op1_data) = *((double *)op1_data) - *((double *)op2_data);

      DEBUG3(fprintf(stderr, "%f\n", *((double *)op1_data)););

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from dob_variable_tell of op1_data",
        "-", temp);

      break;

/*--------------------------------------------------------------------------*\
|  Operation undefined types.
\*--------------------------------------------------------------------------*/

    case bool:
    case str:
    case rcd:
    case id:
    case buff:

      sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "subtraction");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  Undefined.
\*--------------------------------------------------------------------------*/

    case undef: 
/*
   Set temp type to undefined.
*/
      dob_variable_tell(new_handle, idd_undefined, NULL);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from dob_variable_tell",
        "-", 
        temp
      );

      break;

/*--------------------------------------------------------------------------*\
|  sp1, sp2, sp3:  convert the operands to reals and sum.
\*--------------------------------------------------------------------------*/

    case sp1:
    case sp2:
    case sp5:
      t1 =(idd_data_types)dro_check_string(op1);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from dro_check_strg for op1",
        "-", 
        temp
      );

      t2 =(idd_data_types)dro_check_string(op2);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from dro_chck_str for op2",
        "-", 
        temp
      );

      t3 = dro_select_type(t1, t2);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from dro_select_type",
        "-", 
        temp 
      );

      if (t3 == idd_string) {

        sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "subtraction");

        uer_report_binary_op_error(
          uer_error_msg,
          op1,
          op2
        );

	return temp;
      }
      else {

        op1_data = drc_get_atv(op1, t3);
        uer_rcheck_status_binop(
          "dro_subtract: bad status from drc_get_atv of op1",
          "-", temp);
 
        op2_data = drc_get_atv(op2, t3);
        uer_rcheck_status_binop(
          "dro_subtract: bad status from drc_get_atv of op2",
          "-", temp);

        if (t3 == idd_real) {
          *((double *)op1_data) = *((double *)op1_data)-*((double *)op2_data);
        }
        else {
          op1_data =(caddr_t)((int)op1_data - (int)op2_data);
        }

        dob_variable_tell(new_handle, t3, op1_data);
        uer_rcheck_status_binop(
          "dro_subtract: bad status from dob_variable_tell",
          "-", temp);
      }

      break;

    case sp3:
/*
   Convert operands to reals.
*/
      op1_data = drc_get_atv(op1, idd_real);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from drc_get_atv of op1",
        "-", temp);

      op2_data = drc_get_atv(op2, idd_real);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from drc_get_atv of op2",
        "-", temp);
/*
   Perform add operation and update type and value of temp.
*/
      *((double *)op1_data) = *((double *)op1_data) - *((double *)op2_data);

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_subtract: bad status from dob_variable_tell",
        "-", temp);

      break;

/*--------------------------------------------------------------------------*\
|  sp4: run time error
\*--------------------------------------------------------------------------*/
    case sp4:

      sprintf(uer_error_msg, UER_ILLEGAL_ARITH_CONVERSION, "subtraction");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  default:  Fatal Error
\*--------------------------------------------------------------------------*/
    default:                          /* unknown handling type              */

        FATAL_ERROR(
          fprintf(
            stderr, 
            "dro_subtract: unknown handling = %d.\n",
            handling
          );
        );

      break;
  }				/* end switch on specific handling	    */
/* 
   Delete parameters if they are temps 
*/
  dra_delete_temp(op1);
  uer_rcheck_status_binop(
    "dro_subtract: bad status from dra_delete_temp of op1",
    "-", temp);

  dra_delete_temp(op2);
  uer_rcheck_status_binop(
    "dro_subtract: bad status from dra_delete_temp of op2",
    "-", temp);
/* 
   Everything ok, place handle in return structure 
*/
  temp.dob_handle = new_handle;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_subtract: temp.dob_handle = %#x\n", temp.dob_handle);
    fprintf(stderr, "dro_subtract: exiting.\n");
  );

  return temp;
}					/* end dro_subtract		    */

/*--------------------------------------------------------------------------*\
|  Routine: dro_multiply
| 
|  Description:  
|     Multiplies the operands.  Data objects may be either symbol table 
|     entries or shared data elements.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_multiply(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type, t1, t2, t3;
  caddr_t op1_data, op2_data;
  dro_handling handling;
  dob_data_object new_handle;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(fprintf(stderr, "dro_multiply: call.\n");
    fprintf(stderr, "dro_multiply: op1.dob_handle = %#x.\n", op1.dob_handle);
    fprintf(stderr, "dro_multiply: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_multiply: op2.dob_handle = %#x.\n", op2.dob_handle);
    fprintf(stderr, "dro_multiply: op2.component_name = %s.\n", 
      op2.component_name);
  );

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;
/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_multiply: bad status from dob_temp_create",
    "*", temp);  
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_multiply: bad status from dob_get_data_type",
    "*", temp);  

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_multiply: bad status from dob_get_data_type",
    "*", temp);  

  op1_data =(caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_multiply: bad status from dob_get_data_value",
    "*", temp);  

  op2_data =(caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_multiply: bad status from dob_get_data_value",
    "*", temp);  

/*
   Set the current type and value properties for the temp as appropriate.
*/
  handling = binary_arithmetic_ops[(int)op1_type][(int)op2_type];

  switch(handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Integer
\*--------------------------------------------------------------------------*/

    case integ:

      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from drc_get_atv",
        "*", temp);  

      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from drc_get_atv",
        "*", temp);  
/*
   Update temp with sum of values and set type to integer.
*/
      dob_variable_tell(new_handle, idd_integer,
			(char *)((int)op1_data *(int)op2_data));
      uer_rcheck_status_binop(
        "dro_multiply: bad status from dob_variable_tell",
        "*", 
        temp
      );  

      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/

    case real:
/*
   Convert both operands values to reals.
*/
      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from drc_get_atv",
        "*", 
        temp
      );  

      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop("dro_multiply: bad status from drc_get_atv",
        "*", temp);  
/*
   Update temp with sum of values and set type to real.
*/
      DEBUG3(
        fprintf(
          stderr, 
          "dro_multiply: %f * %f = ",
          *((double *)op1_data), *((double *)op2_data)
        );
      );

      *((double *)op1_data) = *((double *)op1_data) * *((double *)op2_data);

      DEBUG3(fprintf(stderr, "%f\n", *((double *)op1_data)););

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from dob_variable_tell",
        "*", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  Operation undefined types.
\*--------------------------------------------------------------------------*/

    case bool:
    case str:
    case rcd:
    case id:
    case buff:

      sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "multiplication");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  Undefined.
\*--------------------------------------------------------------------------*/

    case undef: 
/*
   Set temp type to undefined.
*/
      dob_variable_tell(new_handle, idd_undefined, NULL);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from dob_variable_tell",
        "*", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  sp1, sp2, sp3:  convert the operands to reals and sum.
\*--------------------------------------------------------------------------*/

    case sp1:
    case sp2:
    case sp5:
      t1 =(idd_data_types)dro_check_string(op1);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from dob_variable_tell",
        "*", temp);  

      t2 =(idd_data_types)dro_check_string(op2);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from dob_variable_tell",
        "*", temp);  

      t3 = dro_select_type(t1, t2);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from dob_variable_tell",
        "*", temp);  

      if (t3 == idd_string) {

        sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "subtraction");

        uer_report_binary_op_error(
          uer_error_msg,
          op1,
          op2
        );

	return temp;

      }
      else {

        op1_data = drc_get_atv(op1, t3);
        uer_rcheck_status_binop(
          "dro_multiply: bad status from drc_get_atv of op1",
          "*", temp);

        op2_data = drc_get_atv(op2, t3);
        uer_rcheck_status_binop(
          "dro_multiply: bad status from drc_get_atv of op2",
          "*", temp);  

        if (t3 == idd_real) {
          *((double *)op1_data) = 
	  *((double *)op1_data) * *((double *)op2_data);
	}
        else {
          op1_data =(caddr_t)((int)op1_data *(int)op2_data);
	}

        dob_variable_tell(new_handle, t3, op1_data);
        uer_rcheck_status_binop(
          "dro_multiply: bad status from dob_variable_tell",
          "*", temp);  
      }

      break;

    case sp3:
/*
   Convert operands to reals.
*/
      op1_data = drc_get_atv(op1, idd_real);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from drc_get_atv",
        "*", temp);  

      op2_data = drc_get_atv(op2, idd_real);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from drc_get_atv",
        "*", temp);  
/*
   Perform add operation and update type and value of temp.
*/
      *((double *)op1_data) = *((double *)op1_data) * *((double *)op2_data);

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_multiply: bad status from dob_variable_tell",
        "*", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  sp4: run time error
\*--------------------------------------------------------------------------*/
    case sp4:

      sprintf(uer_error_msg, UER_ILLEGAL_ARITH_CONVERSION, "multiplication");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  default:  Fatal Error
\*--------------------------------------------------------------------------*/
    default:                          /* unknown handling type              */

        FATAL_ERROR(
          fprintf(
            stderr, 
            "dro_multiply: unknown handling = %d.\n",
            handling
          );
        );

      break;
  }				/* end switch on specific handling	    */
/* 
   Delete parameters if they are temps 
*/
   dra_delete_temp(op1);
   uer_rcheck_status_binop(
     "dro_multiply: bad status from dra_delete_temp for op1",
     "*", temp);  

   dra_delete_temp(op2);
   uer_rcheck_status_binop(
     "dro_multiply: bad status from dra_delete_temp for op2",
     "*", temp);  
/* 
   everything worked ok, put handle in return value 
*/
   temp.dob_handle = new_handle;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_multiply: temp.dob_handle = %#x\n", temp.dob_handle);
    fprintf(stderr, "dro_multiply: exiting.\n");
  );

  return temp;
}					/* end dro_multiply		    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_divide
| 
|  Description:  
|     Divides the first operand by the second operand, i.e. op1/op2.
|     and returns the quotient.  Data objects
|     may be either symbol table entries or shared data elements.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_divide(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type;
  caddr_t op1_data, op2_data;
  dro_handling handling;
  dob_data_object new_handle;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(fprintf(stderr, "dro_divide: call.\n");
    fprintf(stderr, "dro_divide: op1.dob_handle = %#x.\n", op1.dob_handle);
    fprintf(stderr, "dro_divide: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_divide: op2.dob_handle = %#x.\n", op2.dob_handle);
    fprintf(stderr, "dro_divide: op2.component_name = %s.\n", 
      op2.component_name);
  );

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;
/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_divide: bad status from dob_temp_create",
    "/", temp);  
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_divide: bad status from dob_get_data_type",
    "/", temp);  

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_divide: bad status from dob_get_data_type",
    "/", temp);  

  op1_data =(caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_divide: bad status from dob_get_data_value",
    "/", temp);  

  op2_data =(caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_divide: bad status from dob_get_data_value",
    "/", temp);  
/*
   Set the current type and value properties for the temp as appropriate.
*/
  handling = binary_arithmetic_ops[(int)op1_type][(int)op2_type];

  switch(handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Integer
\*--------------------------------------------------------------------------*/

    case integ:

      op1_data = drc_get_atv(op1, idd_real);
      uer_rcheck_status_binop(
        "dro_divide: bad status from drc_get_atv",
        "/", temp);  

      op2_data = drc_get_atv(op2, idd_real);
      uer_rcheck_status_binop(
        "dro_divide: bad status from drc_get_atv",
        "/", temp);  
/*
   Update temp with sum of values and set type to integer.
*/
      *((double *)op1_data) = *((double *)op1_data) / *((double *)op2_data);

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_divide: bad status from dob_variable_tell",
        "/", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/

    case real:
/*
   Convert both operands values to reals.
*/
      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_divide: bad status from drc_get_atv",
        "/", temp);  

      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_divide: bad status from drc_get_atv",
        "/", temp);  
/*
   Update temp with sum of values and set type to real.
*/
      DEBUG3(fprintf(stderr, "dro: %f + %f = ",
          *((double *)op1_data), *((double *)op2_data)););

      *((double *)op1_data) = *((double *)op1_data) / *((double *)op2_data);

      DEBUG3(fprintf(stderr, "%f\n", *((double *)op1_data)););

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_divide: bad status from dob_variable_tell",
        "/", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  Operation undefined types.
\*--------------------------------------------------------------------------*/

    case bool:
    case str:
    case rcd:
    case id:
    case buff:

      sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "division");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  Undefined.
\*--------------------------------------------------------------------------*/

    case undef: 
/*
   Set temp type to undefined.
*/
      dob_variable_tell(new_handle, idd_undefined, NULL);
      uer_rcheck_status_binop(
        "dro_divide: bad status from dob_variable_tell",
        "/", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  sp1, sp2, sp3:  convert the operands to reals and sum.
\*--------------------------------------------------------------------------*/

    case sp1:
    case sp2:
    case sp5:
    case sp3:
/*
   Convert operands to reals.
*/
      op1_data = drc_get_atv(op1, idd_real);
      uer_rcheck_status_binop(
        "dro_divide: bad status from drc_get_atv",
        "/", temp);  

      op2_data = drc_get_atv(op2, idd_real);
      uer_rcheck_status_binop(
        "dro_divide: bad status from drc_get_atv",
        "/", temp);  
/*
   Perform add operation and update type and value of temp.
*/
      *((double *)op1_data) = *((double *)op1_data) / *((double *)op2_data);

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_divide: bad status from dob_variable_tell",
        "/", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  sp4: run time error
\*--------------------------------------------------------------------------*/
    case sp4:

      sprintf(uer_error_msg, UER_ILLEGAL_ARITH_CONVERSION, "division");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  default:  Fatal Error
\*--------------------------------------------------------------------------*/
    default:                          /* unknown handling type              */

        FATAL_ERROR(
          fprintf(
            stderr, 
            "dro_divide: unknown handling = %d.\n",
            handling
          );
        );

      break;
  }				/* end switch on specific handling	    */
/* 
   Delete parameters if they are temps 
*/
   dra_delete_temp(op1);
   uer_rcheck_status_binop(
     "dro_divide: bad status from dra_delete_temp for op1",
     "/", temp);  

   dra_delete_temp(op2);
   uer_rcheck_status_binop(
     "dro_divide: bad status from dra_delete_temp for op2",
     "/", temp);  
/* 
   Everythinig worked ok, put in in ret val 
*/
   temp.dob_handle = new_handle;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_divide: temp.dob_handle = %#x\n", temp.dob_handle);
    fprintf(stderr, "dro_divide: exiting.\n");
  );

  return temp;
}					/* end dro_divide		    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_exponentiate
| 
|  Description:  
|     Raises the first operand to the power of the second operand, i.e.
|     op1**op2.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_exponentiate(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type, t1, t2, t3;
  caddr_t op1_data, op2_data;
  dro_handling handling;
  dob_data_object new_handle;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(fprintf(stderr, "dro_exponentiate: call.\n");
    fprintf(stderr, "dro_exponentiate: op1.dob_handle = %#x.\n", op1.dob_handle);
    fprintf(stderr, "dro_exponentiate: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_exponentiate: op2.dob_handle = %#x.\n", op2.dob_handle);
    fprintf(stderr, "dro_exponentiate: op2.component_name = %s.\n", 
      op2.component_name);
  );

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;

/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_exponentiate: bad status from dob_temp_create",
    "**", temp);  
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_exponentiate: bad status from dob_get_data_type",
    "**", temp);  

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_exponentiate: bad status from dob_get_data_type",
    "**", temp);  

  op1_data =(caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_exponentiate: bad status from dob_get_data_value",
    "**", temp);  

  op2_data =(caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_exponentiate: bad status from dob_get_data_value",
    "**", temp);  
/*
   Set the current type and value properties for the temp as appropriate.
*/
  handling = binary_arithmetic_ops[(int)op1_type][(int)op2_type];

  switch(handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Integer
\*--------------------------------------------------------------------------*/

    case integ:

      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from drc_get_atv",
        "**", temp);  

      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from drc_get_atv",
        "**", temp);  
/*
   Update temp with sum of values and set type to integer.
*/
      op1_data =(caddr_t)dro_power((int)op1_data,(int)op2_data);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from dro_power",
        "**", temp);  

      dob_variable_tell(new_handle, idd_integer, op1_data);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from dob_variable_tell",
        "**", temp);  


      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/

    case real:
/*
   Convert both operands values to reals.
*/
      op1_data = drc_get_atv(op1,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from drc_get_atv",
        "**", temp);  


      op2_data = drc_get_atv(op2,(idd_data_types)handling);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from drc_get_atv",
        "**", temp);  
/*
   Update temp with sum of values and set type to real.
*/
      DEBUG3(fprintf(stderr, "dro: %f + %f = ",
          *((double *)op1_data), *((double *)op2_data)););

      *((double *)op1_data) = 
		dro_rpow(*((double *)op1_data), *((double *)op2_data));

      DEBUG3(fprintf(stderr, "%f\n", *((double *)op1_data)););

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
         "dro_exponentiate: bad status from dob_variable_tell",
        "**", temp);  
      break;

/*--------------------------------------------------------------------------*\
|  Operation undefined types.
\*--------------------------------------------------------------------------*/

    case bool:
    case str:
    case rcd:
    case id:
    case buff:

      sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "exponentiation");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  Undefined.
\*--------------------------------------------------------------------------*/

    case undef: 
/*
   Set temp type to undefined.
*/
      dob_variable_tell(new_handle, idd_undefined, NULL);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from dob_variable_tell",
        "**", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  sp1, sp2, sp3:  convert the operands to reals and sum.
\*--------------------------------------------------------------------------*/

    case sp1:
    case sp2:
    case sp5:
      t1 =(idd_data_types)dro_check_string(op1);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from dro_check_string",
        "**", temp);  

      t2 =(idd_data_types)dro_check_string(op2);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from dro_check_string",
        "**", temp);  

      t3 = dro_select_type(t1, t2);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from dro_select_type",
        "**", temp);  


      if (t3 == idd_string) {

        sprintf(uer_error_msg, UER_OP_UNDEF_TYPES, "subtraction");

        uer_report_binary_op_error(
          uer_error_msg,
          op1,
          op2
        );

	return temp;

      }
      else {

        op1_data = drc_get_atv(op1, t3);
        uer_rcheck_status_binop(
          "dro_exponentiate: bad status from drc_get_atv",
          "**", temp);  

        op2_data = drc_get_atv(op2, t3);
        uer_rcheck_status_binop(
          "dro_exponentiate: bad status from drc_get_atv",
          "**", temp);  

	if (t3 == idd_real) {
          *((double *)op1_data) = dro_rpow(
            *((double *)op1_data), 
            *((double *)op2_data)
          );

          dob_variable_tell(new_handle, idd_real, op1_data);
	  uer_rcheck_status_binop(
            "dro_exponentiate: bad status from dob_var_tell",
            "**", temp);  
	}
	else {
          op1_data =(caddr_t)dro_power((int)op1_data,(int)op2_data);

	  dob_variable_tell(new_handle, idd_integer, op1_data);
	  uer_rcheck_status_binop(
            "dro_exponentiate: bad status from dob_var_tell",
            "**", temp);  
        }
      }

      break;

    case sp3:
/*
   Convert operands to reals.
*/
      op1_data = drc_get_atv(op1, idd_real);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from drc_get_atv",
        "**", temp);  

      op2_data = drc_get_atv(op2, idd_real);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from drc_get_atv",
        "**", temp);  
/*
   Perform add operation and update type and value of temp.
*/
      *((double *)op1_data) = 
		dro_rpow(*((double *)op1_data), *((double *)op2_data));

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_binop(
        "dro_exponentiate: bad status from dob_variable_tell", 
        "**", temp);  

      break;

/*--------------------------------------------------------------------------*\
|  sp4: run time error
\*--------------------------------------------------------------------------*/
    case sp4:

      sprintf(uer_error_msg, UER_ILLEGAL_ARITH_CONVERSION, "exponentiation");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  default:  Fatal Error
\*--------------------------------------------------------------------------*/
    default:                          /* unknown handling type              */

        FATAL_ERROR(
          fprintf(
            stderr, 
            "dro_exponentiate: unknown handling = %d.\n",
            handling
          );
        );

      break;
  }				/* end switch on specific handling	    */

/* 
   Delete parameters if they are temps 
*/
   dra_delete_temp(op1);
   uer_rcheck_status_binop(
     "dro_exponentiate: bad status from dra_delete_temp",
     "**", temp);  

   dra_delete_temp(op2);
   uer_rcheck_status_binop(
     "dro_exponentiate: bad status from dra_delete_temp",
     "**", temp);  
/* 
   Everything ok, place handle in returned structure 
*/
   temp.dob_handle = new_handle;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_exponentiate: temp.dob_handle = %#x\n", temp.dob_handle);
    fprintf(stderr, "dro_exponentiate: exiting.\n");
  );

  return temp;
}					/* end dro_exponentiate		    */

