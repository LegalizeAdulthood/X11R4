static char rcsid [] =
        "$Header: /u/04c/mnt/integ/dm/src/RCS/dro2.c,v 1.19 89/11/02 16:18:06 little Exp $";
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
$Log:	dro2.c,v $
 * Revision 1.19  89/11/02  16:18:06  little
 * *** empty log message ***
 * 
 * Revision 1.18  89/11/02  15:08:19  ljb
 * remove debug output
 * 
 * Revision 1.17  89/10/05  16:58:57  rcs
 * various mods to support typed buffers
 * 
 * Revision 1.16  89/09/01  14:11:40  bmc
 * Removed error_msg buffers in individual routines; they now use uer_error_msg
 * supplied by uer.h
 * 
 * Revision 1.15  89/07/24  15:05:54  little
 * change dro_undefined to make use of dob_undefined
 * 
 * Revision 1.13  89/06/19  13:06:34  ljb
 * allow for equality tests involving boolean and other types
 * 
 * Revision 1.12  89/06/15  15:28:33  ljb
 * allow comparisions between buffer and other types
 * 
 * Revision 1.11  89/05/22  11:12:32  bmc
 * Now explicitly include 'uer.h'.  Related to changes to 'dob.c', 'dob.h'
 * and 'uer.h'.
 * 
 * Revision 1.10  89/05/19  17:25:48  bmc
 * Changed to accomodate new definition of 'dro_atomic_data_item'.  (See
 * 'dro.h' for more information.)  Also, changed calls to dob routines
 * to be consistent with modifications in 'dob.h'.
 * 
 * Revision 1.9  89/02/09  18:22:45  ejh
 * added copyright notice
 * 
 * Revision 1.8  89/02/07  16:21:22  rcs
 * fixed error with debug print
 * 
 * Revision 1.7  89/01/30  13:43:13  rcs
 * added run time error checking
 * added support for object_sym
 * 
 * Revision 1.6  88/12/19  18:14:54  rcs
 * added dro_boolean and dro_dot from dro_more.c
 * upgraded run time error checking
 * 
 * Revision 1.5  88/12/14  11:08:40  little
 * in compare, whenever either operand is undefined (but not both) the result
 * is NOT equal
 * this is for special processing sp6
 * 
 * Revision 1.4  88/11/04  17:04:26  little
 * change DEBUG1 to print d22_slang_source
 * 
 * Revision 1.3  88/10/25  16:49:54  little
 * add comparison of buffers and strings using sp11 special processing
 * flag
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

#include "global.h"
#include "dro.h"                          /* own header file		    */
#include "dra.h"
#include "d22_debug.h"
#include "uer.h"			/* error reporting facility	    */

#define DEBUG1(statement) SERPENT_DEBUG1(dro_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(dro_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(dro_debug, statement)

#define NO_OF_TYPES 8

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

extern dro_handling assignment[NO_OF_TYPES][NO_OF_TYPES];
extern dro_handling binary_arithmetic_ops[NO_OF_TYPES][NO_OF_TYPES];
extern dro_handling binary_boolean_ops[NO_OF_TYPES][NO_OF_TYPES];

/*--------------------------------------------------------------------------*\
|  Routine: dro_equal
| 
|  Description:  
|     Tests for equality between the two operands.  
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_equal(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type, t1, t2, t3;
  caddr_t op1_data, op2_data;
  dro_handling handling;
  dob_data_object new_handle;
  boolean they_equal;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(
    fprintf(stderr, "dro_equal: call.\n");
    fprintf(stderr, "dro_equal: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(stderr, "dro_equal: op1.component_name = %s.\n",
	op1.component_name);
    fprintf(stderr, "dro_equal: op2.dob_handle = %d.\n", op2.dob_handle);
    fprintf(stderr, "dro_equal: op2.component_name = %s.\n", 
      op2.component_name);
  );

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;
/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_equal: bad status from dob_temp_create.\n",
    "=", 
    temp
  );
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_equal: bad status from dob_get_data_type of op1.\n",
    "=", 
    temp
  );

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_equal: bad status from dob_get_data_type of op2",
    "=", 
    temp
  );

  op1_data = (caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_equal: bad status from dob_get_data_value of op1.",
    "=", 
    temp
  );

  op2_data = (caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_equal: bad status from dob_get_data_value of op2.\n",
    "=", 
    temp
  );
/*
   Set the current type and value properties for the temp as appropriate.
*/
  handling = binary_boolean_ops[(int)op1_type][(int)op2_type];

  switch (handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Boolean
\*--------------------------------------------------------------------------*/
/*
   Set temp to true if boolean values are equal.
*/
    case bool:
/*  
    we do the comparision in boolean by converting both types
    to boolean.  If the conversion fails then the comparision fails

    ???? this shit is really wierd - 10/2/89 rCs 
*/
      op1_data = drc_get_atv (op1, idd_boolean);
      if (get_status() == failed_conversion) op1_data = (caddr_t)(-1);

      op2_data = drc_get_atv (op2, idd_boolean);
      if (get_status() == failed_conversion) op2_data = (caddr_t)(-2);

      dob_variable_tell(new_handle, idd_boolean, 
        (char *) ((boolean)op1_data == (boolean)op2_data));
      uer_rcheck_status_binop(
        "dro_equal: bad status from dob_variable_tell",
        "=", 
        temp
      );

      break;

/*--------------------------------------------------------------------------*\
|  Integer
\*--------------------------------------------------------------------------*/
/*
   Set temp to true if integer values are equal.
*/
    case integ:

      dob_variable_tell(new_handle, idd_boolean, 
        (char *) ((int)op1_data == (int)op2_data));
      uer_rcheck_status_binop(
        "dro_equal: bad status from dob_variable_tell",
        "=", 
        temp
      );

      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/
/*
   Set temp to true if real values are equal.
*/
    case real:

      dob_variable_tell(new_handle, idd_boolean, 
        (char *) (*((double *)op1_data) == *((double *)op2_data)));
      uer_rcheck_status_binop(
        "dro_equal: bad status from dob_variable_tell",
        "=", 
        temp
      );

      break;

/*--------------------------------------------------------------------------*\
|  ID
\*--------------------------------------------------------------------------*/
/*
   Set temp to true if real values are equal.
*/
    case id:

      dob_variable_tell(
        new_handle, 
        idd_boolean, 
        (char *)(iid_equal((iid_id_type)op1_data, (iid_id_type)op2_data))
      );
      uer_rcheck_status_binop(
        "dro_equal: bad status from dob_variable_tell",
        "=", 
        temp
      );

      break;

/*--------------------------------------------------------------------------*\
|  Record.
\*--------------------------------------------------------------------------*/
/*
   Not currently supported.
*/
    case rcd:

      DEBUG1(
        fprintf(stderr, "dob_equal: idd_record type invalid.\n");
      );

      sprintf(uer_error_msg, UER_ILLEGAL_RECORD_EQUATE);

      uer_report_binary_op_error(
        uer_error_msg,
        op1, 
        op2
      );

      return(temp);

/*--------------------------------------------------------------------------*\
|  Buffer
\*--------------------------------------------------------------------------*/
/*
   Set temp to true if buffers compare equal.
*/
    case buff:

      dob_variable_tell(
        new_handle, 
        idd_boolean, 
        (char *)(ubo_buffcmp(
          (idd_buffer_type *)op1_data, 
          (idd_buffer_type *)op2_data)
        )
      );
      uer_rcheck_status_binop(
        "dro_equal: bad status from dob_variable_tell",
        "=", 
        temp
      );

      break;

/*--------------------------------------------------------------------------*\
|  Undefined.
\*--------------------------------------------------------------------------*/
/*
   If both operands are of undefined type (and therefore variables) they
   are automatically equal.
*/
    case undef: 

      dob_variable_tell(new_handle, idd_boolean, (char *) true);
      uer_rcheck_status_binop(
        "dro_equal: bad status from dob_variable_tell",
        "=", 
        temp
      );

      break;

/*--------------------------------------------------------------------------*\
|  sp11:
|        compare a string and a buffer
\*--------------------------------------------------------------------------*/
   case sp11:

      op1_data = drc_get_atv (op1, idd_string);
      uer_rcheck_status_binop(
        "dro_equal: bad status from drc_get_atv for op1",
        "=", 
        temp
      );

      op2_data = drc_get_atv (op2, idd_string);
      uer_rcheck_status_binop(
        "dro_equal: bad status from for op2",
        "=", 
        temp
      );

      DEBUG3(
        fprintf (stderr, "comparing string and buffer\n");
      );

      they_equal =  !strcmp ((string) op1_data, (string) op2_data);

      DEBUG3(
        fprintf (stderr, "they_equal = %d\n", they_equal);
      );

      dob_variable_tell (new_handle, idd_boolean, they_equal);
      uer_rcheck_status_binop(
        "dro_equal: bad status from dob_variable_tell",
        "=", 
        temp
      );

      break;

/*--------------------------------------------------------------------------*\
|        convert both operands to integers.  if either conversion fails
|        convert both operands to reals.  if either conversion fails 
|        then run time conversion error.
\*--------------------------------------------------------------------------*/
    case str:
    case sp1:
    case sp2:
    case sp3:
    case sp5:

      t1 = dro_check_string(op1);
      t2 = dro_check_string(op2);
      t3 = dro_select_type(t1, t2);

      DEBUG3(
        fprintf(stderr, "dro_equal: type = %d\n", (int)t3);
      );

      op1_data = drc_get_atv(op1, t3);
      uer_rcheck_status_binop(
        "dro_equal: bad status from drc_get_atv of op1_data",
        "=", 
        temp
      );

      op2_data = drc_get_atv(op2, t3);
      uer_rcheck_status_binop(
        "dro_equal: bad status from drc_get_atv of op2_data",
        "=", 
        temp
      );

      switch (t3) {
        case idd_string:
          dob_variable_tell(new_handle, idd_boolean, 
	    (char *)(strcmp((string)op1_data, (string)op2_data) == 0));
	  uer_rcheck_status_binop(
            "dro_equal: bad status from dob_variable_tell",
	    "=", 
            temp
          );
	  break;

        case idd_real:
	  dob_variable_tell(new_handle, idd_boolean,
            (char *) (*((double *)op1_data) == *((double *)op2_data)));
	  uer_rcheck_status_binop(
            "dro_equal: bad status from dob_variable_tell",
            "=", temp);
	  break;

        case idd_integer:
	  dob_variable_tell(new_handle, idd_boolean,
	    (char *) ((int)op1_data == (int)op2_data));
	  uer_rcheck_status_binop(
            "dro_equal: bad status from dob_variable_tell",
            "=", temp);
	  break;
      }

      break;

/*--------------------------------------------------------------------------*\
|  sp4, sp7:  buffer type buffers can not be compared to anything.
\*--------------------------------------------------------------------------*/

    case sp7:
    case sp4:

      sprintf(
        uer_error_msg, 
        UER_UNDEFINED_COMPARISON, 
        "="
      );

      uer_report_binary_op_error(
        uer_error_msg, 
        op1, 
        op2
      );

      return(temp);

/*--------------------------------------------------------------------------*\
|  sp6:  one side is of undefined type, the other is not but may be of
|        undefined value.
\*--------------------------------------------------------------------------*/

    case sp6: 

      if (dro_undefined(op1) && dro_undefined(op2)) {

        dob_variable_tell(new_handle, idd_boolean, (char *)true);
        uer_rcheck_status_binop(
          "dro_equal: bad status from dob_variable_tell",
          "=", temp);

      }
      else {
        
        dob_variable_tell(new_handle, idd_boolean, (char *) false);
        uer_rcheck_status_binop(
          "dro_equal: bad status from dob_variable_tell",
          "=", temp);
      }

      break;


/*--------------------------------------------------------------------------*\
|  default:  run time error
\*--------------------------------------------------------------------------*/

    default:                          /* unknown handling type              */

      FATAL_ERROR(
        fprintf(stderr, "dro_equal: unknown handling of %d.\n", handling);
      );

  }				/* end switch on specific handling	    */
/* 
   Delete parameters if they are temps 
*/
   dra_delete_temp (op1);
   uer_rcheck_status_binop(
     "dro_equal: bad status from dra_del_temp for op1",
     "=", temp);

   dra_delete_temp (op2);
   uer_rcheck_status_binop(
     "dro_equal: bad status from dra_del_temp for op2",
     "=", temp);
/* 
   Everything worked ok, put handle in structure. 
*/
   temp.dob_handle = new_handle;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_equal: temp.dob_handle = %d.\n", temp.dob_handle);
    fprintf(stderr, "dro_equal: exiting.\n");
  );
  return temp;
}					/* end dro_equal		    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_greater
| 
|  Description:  
|     Tests to see if the first operand is greater than the second, i.e.
|     op1 > op2.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_greater(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type;
  idd_data_types t1, t2, t3;
  caddr_t op1_data, op2_data;
  dro_handling handling;
  dob_data_object new_handle;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */

  DEBUG2(fprintf(stderr, "dro_greater: call.\n");
    fprintf(stderr, "dro_greater: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(stderr, "dro_greater: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_greater: op2.dob_handle = %d.\n", op2.dob_handle);
    fprintf(stderr, "dro_greater: op2.component_name = %s.\n", 
      op2.component_name);
  );

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;
/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_greater: bad status from dob_temp_create",
    ">", temp);
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_greater: bad status from dob_get_data_type",
    ">", temp);

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_greater: bad status from dob_get_data_type",
    ">", temp);

  op1_data = (caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_greater: bad status from dob_get_data_value",
    ">", temp);

  op2_data = (caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_greater: bad status from dob_get_data_value",
    ">", temp);
/*
   Set the current type and value properties for the temp as appropriate.
*/
  handling = binary_arithmetic_ops[(int)op1_type][(int)op2_type];

  switch (handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Integer
\*--------------------------------------------------------------------------*/
/*
   Set temp to true if op1_data is greater than op2_data.
*/
    case integ:

      dob_variable_tell(new_handle, idd_boolean, 
        (char *) ((int)op1_data > (int)op2_data));
      uer_rcheck_status_binop(
        "dro_greater: bad status from dob_variable_tell",
        ">", temp);

      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/
/*
   Set temp to true op1_data is greater than op2_data.
*/
    case real:

      dob_variable_tell(new_handle, idd_boolean, 
        (char *) (*((double *)op1_data) > *((double *)op2_data)));
      uer_rcheck_status_binop(
        "dro_greater: bad status from dob_variable_tell",
        ">", temp);

      break;

/*-----------------------------------------------------------------------*\
|   Other undefined types 
\*-----------------------------------------------------------------------*/
    case bool:
    case rcd:
    case id:
    case buff:

      sprintf(
        uer_error_msg, 
        UER_UNDEFINED_COMPARISON, 
        ">"
      );

      uer_report_binary_op_error(
        uer_error_msg,
        op1, 
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  Undefined.
\*--------------------------------------------------------------------------*/
/*
   If both values are undefined then the result is undefined.
*/
    case undef:
 
      dob_variable_tell( new_handle, idd_undefined, NULL);
      uer_rcheck_status_binop(
        "dro_greater: bad status from dob_variable_tell",
        ">", temp);

      break;

/*--------------------------------------------------------------------------*\
|  convert both operands to integers.  if either conversion fails
|  convert both operands to reals.  if either conversion fails 
|  then run time conversion error.
\*--------------------------------------------------------------------------*/
    case str:
    case sp1:
    case sp2:
    case sp3:
    case sp5:

      t1 = dro_check_string(op1);
      uer_rcheck_status_binop(
        "dro_greater: bad status from dro_check_string of op1",
        ">", temp);

      t2 = dro_check_string(op2);
      uer_rcheck_status_binop(
        "dro_greater: bad status from dro_check_string of op2",
        ">", temp);

      t3 = dro_select_type(t1, t2);
      uer_rcheck_status_binop(
        "dro_greater: bad status from dro_select_type",
        ">", temp);

      DEBUG3(
        fprintf(stderr, "dro_greater: type = %d\n", (int)t3);
      );

      op1_data = drc_get_atv(op1, t3);
      uer_rcheck_status_binop(
        "dro_greater: bad status from drc_get_atv of op1",
        ">", temp);

      op2_data = drc_get_atv(op2, t3);
      uer_rcheck_status_binop(
        "dro_greater: bad status from drc_get_atv of op2",
        ">", temp);

      switch (t3) {

        case idd_string:

          sprintf(uer_error_msg, UER_UNDEFINED_COMPARISON, ">");

          uer_report_binary_op_error(
            uer_error_msg,
            op1,
            op2
          );

          return temp;
 
        case idd_real:
	  dob_variable_tell(
            new_handle, 
            idd_boolean,
            (char *) (*((double *)op1_data) > *((double *)op2_data))
          );
          uer_rcheck_status_binop(
            "dro_greater: bad status from dob_variable_tell",
            ">", temp);

	  break;

        case idd_integer:
	  dob_variable_tell(
            new_handle, 
            idd_boolean,
	    (char *)((int)op1_data > (int)op2_data)
          );
	  uer_rcheck_status_binop(
            "dro_greater: bad status from dob_variable_tell",
            ">", temp);

	  break;
	}
      break;

/*--------------------------------------------------------------------------*\
|  sp4: run time error
\*--------------------------------------------------------------------------*/

    case sp4:

      sprintf(uer_error_msg, UER_UNDEFINED_COMPARISON, ">");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  sp6:  undefined operand
\*--------------------------------------------------------------------------*/
    case sp6: 

      sprintf(uer_error_msg, UER_ILLEGAL_UNDEF_COMPARISON, ">");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  sp7:  run time error
\*--------------------------------------------------------------------------*/
    case sp7: 

      sprintf(uer_error_msg, UER_ILLEGAL_BUFF_COMPARISON, ">");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  default:  run time error
\*--------------------------------------------------------------------------*/
    case sp11:

      default:                        /* unknown handling type              */

        FATAL_ERROR(
          fprintf(stderr, "dro_greater: unknown handling of %d.\n", handling);
        );

        break;
  }				/* end switch on specific handling	    */
/* 
   delete parameters if they are temps 
*/
   dra_delete_temp (op1);
   uer_rcheck_status_binop(
     "dro_greater: bad status from dra_delete_temp for op1",
     ">", temp);

   dra_delete_temp (op2);
   uer_rcheck_status_binop(
     "dro_greater: bad status from dra_delete_temp for op2",
     ">", 
     temp
   );
/* 
   Everything worked ok, put handle in structure 
*/
   temp.dob_handle = new_handle;
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_greater: temp.dob_handle = %d.\n", temp.dob_handle);
    fprintf(stderr, "dro_greater: exiting.\n");
  );
  return temp;
}					/* end dro_greater		    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_less
| 
|  Description:  
|     Tests to see if the first operand is less than the second, i.e.
|     op1 < op2.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_less(op1, op2)
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
  DEBUG2(fprintf(stderr, "dro_less: call.\n");
    fprintf(stderr, "dro_less: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(stderr, "dro_less: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_less: op2.dob_handle = %d.\n", op2.dob_handle);
    fprintf(stderr, "dro_less: op2.component_name = %s.\n", 
      op2.component_name););

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;

/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_less: bad status from dob_temp_create",
    "<", 
    temp
  );    
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_less: bad status from dob_get_data_type",
    "<", 
    temp
  );    

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_less: bad status from dob_get_data_type",
    "<", 
    temp
  );    

  op1_data = (caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_less: bad status from dob_get_data_value",
    "<", 
    temp
  );    

  op2_data = (caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_less: bad status from dob_get_data_value",
    "<", 
    temp
  );    
/*
   Set the current type and value properties for the temp as appropriate.
*/
  handling = binary_arithmetic_ops[(int)op1_type][(int)op2_type];

  switch (handling) {		/* switch on specific handling		    */

/*--------------------------------------------------------------------------*\
|  Integer
\*--------------------------------------------------------------------------*/
/*
   Set temp to true if op1_data is less than op2_data.
*/
    case integ:

      dob_variable_tell(new_handle, idd_boolean, 
        (char *) ((int)op1_data < (int)op2_data));
      uer_rcheck_status_binop(
        "dro_less: bad status from dob_variable_tell",
        "<", 
        temp
      );    

      break;

/*--------------------------------------------------------------------------*\
|  Real
\*--------------------------------------------------------------------------*/
/*
   Set temp to true if op1_data is less than op2_data.
*/
    case real:

      dob_variable_tell(new_handle, idd_boolean, 
        (char *) (*((double *)op1_data) < *((double *)op2_data)));
      uer_rcheck_status_binop(
        "dro_less: bad status from dob_variable_tell",
        "<", 
        temp
      );    

      break;

/* --------------------------------------------------------------------- *\
    Other undefined types 
\*---------------------------------------------------------------------- */
    case bool:
    case rcd:
    case id:
    case buff:

      DEBUG1(
        fprintf(stderr, "dro_less: error, undefined type.\n");
      );

      sprintf(uer_error_msg, UER_UNDEFINED_COMPARISON, "<");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      break;

/*--------------------------------------------------------------------------*\
|  Undefined.
\*--------------------------------------------------------------------------*/
/*
   If both values are undefined then the result is undefined.
*/
    case undef: 

      dob_variable_tell(new_handle, idd_undefined, NULL);
      uer_rcheck_status_binop(
        "dro_less: bad status from dob_variable_tell",
        "<", 
        temp
      );    

      break;

/*--------------------------------------------------------------------------*\
|  convert both operands to integers.  if either conversion fails
|  convert both operands to reals.  if either conversion fails 
|  then run time conversion error.
\*--------------------------------------------------------------------------*/
    case str:
    case sp1:
    case sp2:
    case sp3:
    case sp5:

      t1 = dro_check_string(op1);
      t2 = dro_check_string(op2);
      t3 = dro_select_type(t1, t2);

      op1_data = drc_get_atv(op1, t3);
      uer_rcheck_status_binop(
        "dro_less: bad status from drc_get_atv of op1_data",
        "<", 
        temp
      );    

      op2_data = drc_get_atv(op2, t3);
      uer_rcheck_status_binop(
        "dro_less: bad status from drc_get_atv of op2_data",
        "<", 
        temp
      );    

      switch (t3) {
        case idd_string:
          set_status(illegal_conversion);
          break;

        case idd_real:
          dob_variable_tell(new_handle, idd_boolean,
            (char *) (*((double *)op1_data) < *((double *)op2_data)));
          uer_rcheck_status_binop(
            "dro_less: bad status from dob_variable_tell",
            "<", 
            temp
          );    
          break;

        case idd_integer:
          dob_variable_tell(new_handle, idd_boolean,
            (char *) ((int)op1_data < (int)op2_data));
          uer_rcheck_status_binop(
            "dro_less: bad status from dob_variable_tell",
            "<", 
            temp
          );    
          break;
      }
      break;

/*--------------------------------------------------------------------------*\
|  sp4: run time error
\*--------------------------------------------------------------------------*/
    case sp4:

      sprintf(uer_error_msg, UER_UNDEFINED_COMPARISON, ">");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  sp6:  undefined operand
\*--------------------------------------------------------------------------*/
    case sp6: 

      sprintf(uer_error_msg, UER_ILLEGAL_UNDEF_COMPARISON, "<");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  sp7:  run time error
\*--------------------------------------------------------------------------*/
    case sp7: 

      sprintf(uer_error_msg, UER_ILLEGAL_BUFF_COMPARISON, "<");

      uer_report_binary_op_error(
        uer_error_msg,
        op1,
        op2
      );

      return temp;

/*--------------------------------------------------------------------------*\
|  default:  run time error
\*--------------------------------------------------------------------------*/
    case sp11:

      default:                        /* unknown handling type              */

        FATAL_ERROR(
          fprintf(stderr, "dro_greater: unknown handling of %d.\n", handling);
        );

        break;
  }				/* end switch on specific handling	    */

			/* delete parameters if they are temps */
  dra_delete_temp (op1);
  uer_rcheck_status_binop(
    "dro_less: bad status from dra_delete_temp for op1",
    "<", 
    temp
  );    

  dra_delete_temp (op2);
  uer_rcheck_status_binop(
    "dro_less: bad status from dra_delete_temp for op2",
    "<", 
    temp
  );    

			/* everything worked ok, put handle in structure */
  temp.dob_handle = new_handle;

/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_less: temp.dob_handle = %d.\n", temp.dob_handle);
    fprintf(stderr, "dro_less: exiting.\n");
  );
  return temp;
}					/* end dro_less		    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_and
| 
|  Description:  
|     Performs an and operation on the operands.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_and(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type;
  caddr_t op1_data, op2_data;
  dob_data_object new_handle;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(fprintf(stderr, "dro_and: call.\n");
    fprintf(stderr, "dro_and: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(stderr, "dro_and: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_and: op2.dob_handle = %d.\n", op2.dob_handle);
    fprintf(stderr, "dro_and: op2.component_name = %s.\n", 
      op2.component_name););

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;

/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop("dro_and: bad status from dob_temp_create",
    "AND", 
    temp
  );    
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_and: bad status from dob_get_data_type",
    "AND", 
    temp
  );    

  DEBUG3(
    fprintf(stderr, "dro_and: op1_type = %d.\n", op1_type);
  );

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_and: bad status from dob_get_data_type",
    "AND", 
    temp
  );    

  DEBUG3(
    fprintf(stderr, "dro_and: op2_type = %d.\n", op2_type);
  );

  op1_data = (caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_and: bad status from dob_get_data_value",
    "AND", 
    temp
  );    

  DEBUG3(
    fprintf(stderr, "dro_and: op1_data = %d.\n", op1_data);
  );

  op2_data = (caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_and: bad status from dob_get_data_value",
    "AND", 
    temp
  );    

  DEBUG3(
    fprintf(stderr, "dro_and: op2_data = %d.\n", op2_data);
  );
/*
   Set the current type and value properties for the temp as appropriate.
*/
  if ((op1_type == idd_boolean) && (op2_type == idd_boolean)) {
				/* both operands are of boolean type 	    */

    dob_variable_tell(new_handle, idd_boolean, 
      (char *) ((boolean)op1_data & (boolean)op2_data));
    uer_rcheck_status_binop(
      "dro_and: bad status from dob_variable_tell",
      "AND", 
      temp
    );    

  }				/* end if both operands are of boolean type */
  else if ((op1_type == idd_undefined) && (op2_type == idd_undefined)
         || (op1_type == idd_boolean) && (op2_type == idd_undefined) 
         || (op1_type == idd_undefined) && (op2_type == idd_boolean) 
       ) { 				/* undefined case	 	    */

    dob_variable_tell(new_handle, idd_undefined, NULL);
    uer_rcheck_status_binop(
      "dro_and: bad status from dob_variable_tell",
      "AND", 
      temp
    );    


  }					/* end else undefined case 	    */
  else {				/* run time error		    */

    DEBUG1(
      fprintf(stderr, "dro_and: error, undefined type.\n");
    );

    sprintf(uer_error_msg, UER_UNDEFINED_COMPARISON, "AND");

    uer_report_binary_op_error(
      uer_error_msg,
      op1,
      op2
    );

    return temp;

  }					/* end else run time error	    */

			/* delete parameters if they are temps */
  dra_delete_temp (op1);
  uer_rcheck_status_binop(
    "dro_and: bad status from dra_delete_temp for op1",
    "AND", 
    temp
  );    

  dra_delete_temp (op2);
  uer_rcheck_status_binop(
    "dro_and: bad status from dra_delete_temp for op2",
    "AND", 
    temp
  );    

			/* everything worked ok, put handle in structure */
   temp.dob_handle = new_handle;

/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_and: temp.dob_handle = %d.\n", temp.dob_handle);
    fprintf(stderr, "dro_and: exiting.\n");
  );
  return temp;
}					/* end dro_and			    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_or
| 
|  Description:  
|     Performs an or operation on the operands.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_or(op1, op2)
dro_atomic_data_item op1, op2;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type, op2_type;
  caddr_t op1_data, op2_data;
  dob_data_object new_handle;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(fprintf(stderr, "dro_or: call.\n");
    fprintf(stderr, "dro_or: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(stderr, "dro_or: op1.component_name = %s.\n", 
      op1.component_name);
    fprintf(stderr, "dro_or: op2.dob_handle = %d.\n", op2.dob_handle);
    fprintf(stderr, "dro_or: op2.component_name = %s.\n", 
      op2.component_name););

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;

/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_binop(
    "dro_or: bad status from dob_temp_create",
    "OR", 
    temp
  );    
/* 
   Determine the data types and values for both operands.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_or: bad status from dob_get_data_type",
    "OR", 
    temp
  );    

  op2_type = dob_get_data_type(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_or: bad status from dob_get_data_type",
    "OR", 
    temp
  );    

  op1_data = (caddr_t)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_binop(
    "dro_or: bad status from dob_get_data_value",
    "OR", 
    temp
  );    

  op2_data = (caddr_t)dob_get_data_value(op2.dob_handle, op2.component_name);
  uer_rcheck_status_binop(
    "dro_or: bad status from dob_get_data_value",
    "OR", 
    temp
  );    
/*
   Set the current type and value properties for the temp as appropriate.
*/
  if ((op1_type == idd_boolean) && (op2_type == idd_boolean)) {
				/* both operands are of boolean type 	    */

    dob_variable_tell(new_handle, idd_boolean, 
      (char *) ((boolean)op1_data | (boolean)op2_data));
    uer_rcheck_status_binop(
      "dro_or: bad status from dob_variable_tell",
      "OR", 
      temp
    );    

  }				/* end if both operands are of boolean type */
  else if ((op1_type == idd_undefined) && (op2_type == idd_undefined)
         || (op1_type == idd_boolean) && (op2_type == idd_undefined) 
         || (op1_type == idd_undefined) && (op2_type == idd_boolean) 
       ) { 				/* undefined case	 	    */

    dob_variable_tell(new_handle, idd_undefined, NULL);
    uer_rcheck_status_binop(
      "dro_or: bad status from dob_variable_tell",
      "OR", 
      temp
    );    


  }					/* end else undefined case 	    */
  else {				/* run time error		    */

    set_status(illegal_boolean_operation);
    DEBUG1(isc_print_status("dro_or:");
      fflush(stderr););

    return temp;

  }					/* end else run time error	    */

			/* delete parameters if they are temps */
  dra_delete_temp (op1);
  uer_rcheck_status_binop(
    "dro_or: bad status from dra_delete_temp for op1",
    "OR", 
    temp
  );    

  dra_delete_temp (op2);
  uer_rcheck_status_binop(
    "dro_or: bad status from dra_delete_temp for op2",
    "OR", 
    temp
  );    

			/* everything worked ok, put handle in structure */
   temp.dob_handle = new_handle;

/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "dro_or: temp.dob_handle = %d.\n", temp.dob_handle);
    fprintf(stderr, "dro_or: exiting.\n");
  );
  return temp;
}					/* end dro_or			    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_not
| 
|  Description:  
|     Performs a not operation on the operand.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_not(op1)
dro_atomic_data_item op1;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type;
  boolean op1_data;
  dob_data_object new_handle;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(fprintf(stderr, "dro_not: call.\n");
    fprintf(stderr, "dro_not: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(stderr, "dro_not: op1.component_name = %s.\n", 
      op1.component_name););

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;

/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_unop("dro_not: bad status from dob_temp_create", "NOT", temp);

/* 
   Determine the data types and values for the operand.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_unop("dro_not: bad status from dob_get_data_type", "NOT", temp);

  op1_data = (boolean)dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_unop("dro_not: bad status from dob_get_data_value", "NOT", temp);
/*
   Set the current type and value properties for the temp.
*/
  switch (op1_type) {			/* switch on op1 data type	    */

    case idd_boolean:

      dob_variable_tell(new_handle, idd_boolean, (char *) (!op1_data));
      uer_rcheck_status_unop("dro_not: bad status from dob_variable_tell", "NOT", temp);

      break;

    case idd_integer:
    case idd_real:
    case idd_string:
    case idd_record:
    case idd_id:
    case idd_buffer:

      set_status(illegal_boolean_operation);

      break;

    case idd_undefined:

      dob_variable_tell(new_handle, idd_boolean, UNDEFINED_BOOLEAN);
      uer_rcheck_status_unop("dro_not: bad status from dob_variable_tell", "NOT", temp);

      break;
      
  }					/* end switch on op1 data type	    */
			/* delete parameters if they are temps */
   dra_delete_temp (op1);
   uer_rcheck_status_unop("dro_not: bad status from dra_delete_temp for op1", "NOT", temp);

			/* everything worked ok, put handle in structure */
   temp.dob_handle = new_handle;

/*
   Return.
*/
  DEBUG2(fprintf(stderr, "dro_not: temp.dob_handle = %d.\n", temp.dob_handle);
    fprintf(stderr, "dro_not: temp.component_name = %s.\n", 
      temp.component_name);
    fprintf(stderr, "dro_not: exiting.\n"););

  return temp;
}					/* end dro_not			    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_negate
| 
|  Description:  
|     Performs a negation of the operand.
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_negate(op1)
dro_atomic_data_item op1;
{					/* local type definition	    */
  dro_atomic_data_item temp;
  idd_data_types op1_type;
  caddr_t op1_data;
  dob_data_object new_handle;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(fprintf(stderr, "dro_negate: call.\n");
    fprintf(stderr, "dro_negate: op1.dob_handle = %d.\n", op1.dob_handle);
    fprintf(stderr, "dro_negate: op1.component_name = %s.\n", 
      op1.component_name););

  temp.dob_handle = dob_null_object;
  temp.component_name = NULL;

/*
   Create a temp to return the result in.
*/
  new_handle = dob_temp_create();
  uer_rcheck_status_unop("dro_negate: bad status from dob_temp_create", "NEGATE", temp);

/* 
   Determine the data types and values for the operand.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_unop("dro_negate: bad status from dob_get_data_type", "NEGATE", temp);

  op1_data = dob_get_data_value(op1.dob_handle, op1.component_name);
  uer_rcheck_status_unop("dro_negate: bad status from dob_get_data_value", "NEGATE", temp);
/*
   Set the current type and value properties for the temp.
*/
  switch (op1_type) {			/* switch on op1 data type	    */

    case idd_integer:

      dob_variable_tell(new_handle, idd_integer, (char *) (-1 * (int)op1_data));
      uer_rcheck_status_unop("dro_negate: bad status from dob_variable_tell", "NEGATE", temp);

      break;

    case idd_real:

      *((double *)op1_data) = -1 * (*((double *)op1_data));

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_unop("dro_negate: bad status from dob_variable_tell", "NEGATE", temp);

      break;

    case idd_string:

      op1_data = drc_get_atv(op1, idd_real);
      uer_rcheck_status_unop(
        "dro_negate: bad status from drc_get_atv", 
         "NEGATE", 
         temp
      );

      *((double *)op1_data) = -1 * (*((double *)op1_data));

      dob_variable_tell(new_handle, idd_real, op1_data);
      uer_rcheck_status_unop("dro_negate: bad status from dob_variable_tell", "NEGATE", temp);

      break;

    case idd_boolean:
    case idd_record:
    case idd_id:
    case idd_buffer:

      set_status(illegal_arithmetic_operation);

      break;

    case idd_undefined:

      dob_variable_tell(new_handle, idd_boolean, UNDEFINED_BOOLEAN);
      uer_rcheck_status_unop("dro_negate: bad status from dob_variable_tell", "NEGATE", temp);

      break;
      
  }					/* end switch on op1 data type	    */
			/* delete parameters if they are temps */
   dra_delete_temp (op1);
   uer_rcheck_status_unop("dro_negate: bad status from dra_delete_temp for op1", "NEGATE", temp);

			/* everything worked ok, put handle in structure */
   temp.dob_handle = new_handle;

/*
   Return.
*/
  DEBUG2(fprintf(stderr, "dro_negate: temp.dob_handle = %d.\n", temp.dob_handle);
    fprintf(stderr, "dro_negate: temp.component_name = %s.\n", 
      temp.component_name);
    fprintf(stderr, "dro_negate: exiting.\n"););

  return temp;
}					/* end dro_negate		    */


/*--------------------------------------------------------------------------*\
|  Routine: dro_undefined
| 
|  Description:  
|     Returns true if undefined.  This is and adi version of dob_undefined.
\*--------------------------------------------------------------------------*/
boolean dro_undefined (op1)
dro_atomic_data_item op1;
{					/* local type definition	    */
  boolean value;
  idd_data_types op1_type;
  dob_data_object new_handle;
  caddr_t op1_data;

  set_status (ok);

  DEBUG2 (fprintf (stderr, "enter dro_undefined: ");
    	  fprintf (stderr, "op1.dob_handle = %d.\n", op1.dob_handle);
    	  fprintf (stderr, "\top1.component_name = %s.\n",
		  op1.component_name););

				/* Determine the data types and values for the
				   operand.
				*/
  op1_type = dob_get_data_type (op1.dob_handle, op1.component_name);
  rcheck_status ("dro_undefined: bad status from dob_get_data_type", value);

  op1_data = dob_base_ask(op1.dob_handle, op1.component_name, false);
  rcheck_status("dro_undefined: bad status from dob_ask", value);

  value = dob_undefined(op1_type, op1_data);

  DEBUG2(fprintf(stderr, "exit dro_undefined: value = %d.\n", value););

  return value;
}					/* end dro_undefined		    */


/* ?? the error checking in this routine should be different. */
/*--------------------------------------------------------------------------*\
|  Routine: dro_boolean
| 
|  Description:  
|     Returns value of a boolean adi.  Undefined causes a run time error.
\*--------------------------------------------------------------------------*/
boolean dro_boolean(op1)
dro_atomic_data_item op1;
{  
  idd_data_types op1_type;
  caddr_t op1_data;
  int ret_val;
/*
   Initialize.
*/
  set_status(ok);

  DEBUG2(
    fprintf(stderr, "dro_boolean: call\n");
    fprintf(
      stderr, 
      "dro_boolean: op1.dob_handle = %d, op1.component_name = %s\n",
      op1.dob_handle, op1.component_name
    );
  );
/*
   Get the data item type.
*/
  op1_type = dob_get_data_type(op1.dob_handle, op1.component_name);
  uer_rcheck_status_unop(
    "dro_boolean: bad status from dob_get_data_type.",
    "boolean test", 
    false
  );
/*
   If the type is boolean, get the value;  otherwise run time error.
*/
  if (op1_type == idd_boolean) {	/* operand is of boolean type	    */

    ret_val = (int)dro_ask2(op1);
    uer_rcheck_status_unop(
      "dro_boolean: bad status from dro_ask2",
      "boolean test", 
      ret_val
    );

  }					/* end if operand is of bool type   */
  else {				/* else if operand not boolean	    */

    sprintf(uer_error_msg, UER_ILLEGAL_TEST);

    uer_report_unary_op_error(
      uer_error_msg,
      op1
    );

    return ret_val;

  }					/* end else operand not boolean	    */
/*
   If the boolean expression is undefined generate a run time error.
*/
  if (dro_undefined(op1)) {		/* if operand undefined		    */
     
    DEBUG1(
      fprintf(stderr, "dro_boolean: idd_record type invalid.\n");
    );

    sprintf(uer_error_msg, UER_UNDEFINED_TEST);

    uer_report_unary_op_error(
      uer_error_msg,
      op1
    );

  }				/* end if operand is undefined		    */
/*
   Return with value.
*/
  DEBUG2(
    fprintf(stderr, "dro_boolean:  value = %d\n", ret_val);
    fprintf(stderr, "dro_boolean:  exiting");
  );
  return(ret_val);
}

/*--------------------------------------------------------------------------*\
|  Routine: dro_dot
| 
|  Description:  
|     ????
\*--------------------------------------------------------------------------*/
dro_atomic_data_item dro_dot(left, right)
dro_atomic_data_item left, right;
/*
?? write dro_dot, csc_dot_write is "doing all the work" now
*/
{  
  dro_atomic_data_item new_adi;

  return(new_adi);
}
