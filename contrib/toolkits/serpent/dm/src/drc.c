static char rcsid [] =
        "$Header: /u/04c/mnt/integ/dm/src/RCS/drc.c,v 1.24 89/11/02 16:17:01 little Exp $";
/*
???? propagate undefined in all run-time functions
*/
/*--------------------------------------------------------------------------*\
|
|  Name:  Run Time Conversion Utility (drc)
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
|     Stephen T. Chou
|     stc@sei.cmu.edu
|
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
$Log:	drc.c,v $
 * Revision 1.24  89/11/02  16:17:01  little
 * 
 * no changes
 * 
 * Revision 1.23  89/10/23  19:42:21  ejh
 * switched over to u22_copy_string
 * 
 * Revision 1.22  89/10/05  17:06:22  rcs
 * checked in wrong file last time
 * 
 * Revision 1.20  89/08/29  10:56:41  bmc
 * Conditionally compiled code no longer defaults to VAX architecture if
 * '#ifdef mc68000' test fails.  Instead, explicit tests are included for
 * each supported architecture.  Unsupported architectures will cause
 * compilation errors.  (SPR #109)
 * 
 * Revision 1.19  89/08/14  16:35:46  ljb
 * try again to fix up isreal.  added test for special case of single
 * + or single -.  Also ifdefed depending upon architecture to test
 * for Sun bug in returning one past last character scanned.
 * 
 * Revision 1.18  89/08/08  12:01:58  ljb
 * change isinteger and isreal to use unix functions strtol and strtod rather than 
 * roll our own.
 * 
 * 
 * Revision 1.17  89/06/15  15:26:21  ljb
 * add conversion routines for boolean to other things and
 * virtually anything to buffer
 * make buffer universal type which holds values in binary rather
 * than ascii
 * 
 * Revision 1.16  89/05/19  17:21:46  bmc
 * Adjusted to accomodate new definition of 'dro_atomic_data_item'.
 * See 'dro.h' for more information.
 * 
 * Revision 1.15  89/04/25  18:11:05  rnk
 * fixed isinteger so that it doesn't barf on trailing blanks
 * 
 * Revision 1.14  89/04/11  14:41:43  rcs
 * added string to id and id to string conversions
 * 
 * Revision 1.13  89/02/09  17:42:50  ejh
 * added copyright notice
 * 
 * Revision 1.12  89/02/07  20:29:11  ejh
 * rebuilt rtoi and blstr; general portability fixes
 * 
 * Revision 1.11  89/02/07  19:02:07  ejh
 * fixed rtoi
 * 
 * Revision 1.10  89/01/30  13:48:59  rcs
 * modified strtobuff and bufftostr to use and support
 *   NULL body buffers
 * added debug print for calls, parameters, exit and return values
 * 
 * Revision 1.9  89/01/12  14:37:36  little
 * add #ifndef mc68000 so things work correctly on sun
 * 
 * Revision 1.8  89/01/04  15:39:12  little
 * add printing of d22_slang_source to error printout
 * 
 * Revision 1.7  88/12/21  19:16:27  rcs
 * modified rtoa and itoa
 * 
 * Revision 1.6  88/12/21  14:24:22  little
 * add drc_get_undefined function
 * add debugs print to funcall & itoa
 * 
 * Revision 1.5  88/11/04  17:03:24  little
 * change DEBUG1 to print d22_slang_source
 * 
|     June 14, 1988 : Module created.
|     Oct 3, 1988: add conversion string <--> buffer       ljb
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

#define drc_main

#include <math.h>
#include "u22.h"
#include "drc.h"                          /* own header file		    */
#include "d22_debug.h"

#define DEBUG1(statement) SERPENT_DEBUG1(drc_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(drc_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(drc_debug, statement)

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

/*
 bool	integ	real	string	record	id	buff	undef
*/

/*--------------------------------------------------------------------------*\
|  Routine: funcall
| 
|  Description:  
|     call proper type conversion function
|???  current choice of routine is based on having 8 types.  Should be
|      changed to make more general.
\*--------------------------------------------------------------------------*/
static caddr_t funcall(to_type, from_type, arg)
idd_data_types to_type, from_type;
caddr_t arg;
{
  caddr_t result;
  int sum;

  DEBUG2(
    fprintf(stderr, "funcall: call.\n");
    fprintf(stderr, "funcall: to_type = %s.\n", idd_get_type_string(to_type));
    fprintf(
      stderr, 
      "funcall: from_type = %s.\n", 
      idd_get_type_string(from_type)
    );
    fprintf(stderr, "funcall: arg = %d.\n", arg);
  );
/* 
   If the operand type is the same as the conversion type return the 
   existing value unmodified.  
*/
  if (from_type == to_type) {
    result = (caddr_t)arg;
  }
/* 
   If undefined, get new type of undefined.
*/
  else if (dob_undefined(from_type, arg)) {
    result = drc_get_undefined(to_type);
  }
/* 
   Perform conversion.
*/
  else {

    sum = ((int)NO_OF_TYPES * (int)to_type) + (int)from_type;

    DEBUG3(
      fprintf(stderr, "funcall: sum = %d\n", sum);
    );

    switch (sum) {
      case 1:  		/* convert integer to boolean*/
        result = inttobool(arg); break;

      case 8:		/* convert boolean to integer*/
        result = booltoint(arg); 
        break;

      case 2: 		/* convert real to boolean*/
        result = realtobool(arg); 
	break;

      case 10:
        result = rtoi(arg); 
	break;

      case 3: 		/* convert string to boolean*/
        result = strtobool(arg); 
	break;

      case 6: 		/* convert buffer to boolean */
      case 14: 		/* convert buffer to integer */
      case 22:		/* convert buffer to real */
      case 30: 		/* convert buffer to string */
      case 46: 		/* convert buffer to id*/
        result = convert_from_buffer(to_type, arg); 
        break;

      case 11:  
      case 43: /* convert id to string */
        result = astoi(arg); 
        break;

      case 16: 		/* convert boolean to real*/
        result = booltoreal(arg); 
	break;

      case 17: 
        result = itor(arg); 
        break;

      case 19: 
        result = astor(arg); 
        break;

      case 24: 		/* convert boolean to string*/
        result = booltostr(arg); 
        break;

      case 29: /* convert id to string */
      case 25: 
        result = itoa(arg); 
        break;

      case 26: 
        result = rtoa(arg); 
        break;

      case 7: 
      case 15: 
      case 23: 
      case 31: 
      case 39: 
      case 47: 
      case 55: 
        result = UNDEF; 
        break;

      case 48:		/*convert boolean to buffer*/
        result = booltobuff(arg); break;

      case 49: 
        result = inttobuff(arg); 
        break;

      case 50:		/* convert real to buffer*/
        result = realtobuff(arg);
        break;

      case 51: 
        result = strtobuff(arg); 
        break;

      case 53:		/* convert id to buffer */
        result = idtobuff(arg);
        break;

      default: 

        DEBUG1(
          fprintf(stderr, "funcall: failed conversion.\n");
          fprintf(stderr, "funcall: sum = %d.\n", sum);
        );

        set_status(failed_conversion);
        return (NULL);
    }				/* end switch on type combination	*/
  }				/* end else provide conversion		*/
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "funcall: result = %d.\n", result);
    fprintf(stderr, "funcall: exit.\n");
  );
  return (result);
}				/* end funcall 				*/
				

/*--------------------------------------------------------------------------*\
|  Routine: drc_get_undefined
| 
|  Description:  
|     Returns the "correct" undefined value
\*--------------------------------------------------------------------------*/
caddr_t drc_get_undefined (desired_type)
idd_data_types desired_type;

{  caddr_t ret_val;
   idd_buffer_type *ret_buf;

   set_status(ok);
   DEBUG2 (fprintf (stderr, "drc_get_undefined: call, ");
           fprintf (stderr, "desired_type = %d\n", desired_type););

   switch (desired_type)
      {case idd_boolean:
         ret_val = (caddr_t) UNDEFINED_BOOLEAN;
         break;

      case idd_integer:
         ret_val = (caddr_t) UNDEFINED_INTEGER;
         break;

      case idd_real:
         ret_val = make_node (sizeof (UNDEFINED_REAL));
         *((double *) ret_val) = UNDEFINED_REAL;
         break;

      case idd_string:
         ret_val = make_node (sizeof (UNDEFINED_STRING));
         ret_val = UNDEFINED_STRING;
         break;

      case idd_id:
         ret_val = (caddr_t) UNDEFINED_ID;
         break;

      case idd_buffer:
         ret_buf = (idd_buffer_type *) make_node (sizeof (idd_buffer_type));
         ret_buf-> length = UNDEFINED_BUFFER_LENGTH;
         ret_val = (caddr_t) ret_buf;
         break;

      case idd_undefined:
      case idd_record:
         FATAL_ERROR (fprintf (stderr,
	   "drc_get_undefined: got an illegal type = %d\n", desired_type););
         break;
      default:
         FATAL_ERROR (fprintf (stderr,
	   "drc_get_undefined: got an unknown type = %d\n", desired_type););
         break;
     
   }

  DEBUG2(
    fprintf(stderr, "drc_get_undefined: (int)ret_val = %d.\n", ret_val);
    fprintf(stderr, "drc_get_undefined: (string)ret_val = %s.\n", ret_val);
    fprintf(stderr, "drc_get_undefined: (hex)ret_val = %x.\n", ret_val);
    fprintf(stderr, "drc_get_undefined: exit\n");
  );

  return (ret_val);
}					/* end drc_get_undefined */


/*--------------------------------------------------------------------------*\
|  Routine: drc_get_atv
| 
|  Description:
|     The drc get appropriately typed value routine accepts an operand and
|     a conversion type and returns a value of the conversion type or sets
|     status to invalid_conversion.
\*--------------------------------------------------------------------------*/
caddr_t drc_get_atv(operand, data_type)
dro_atomic_data_item operand;
idd_data_types data_type;
{					/* local type definition	    */
  idd_data_types op_type;
  caddr_t op_data, result, ret_val;
/*
   Initialization.
*/
  set_status(ok);			/* begin			    */
  DEBUG2(
    fprintf(stderr, "drc_get_atv: call.\n");
    fprintf(stderr, "drc_get_atv: operand.dob_handle = %#x.\n",
	operand.dob_handle);
    fprintf(
      stderr, 
      "drc_get_atv: operand.component_name = %s.\n", 
      operand.component_name
    );
    fprintf(stderr, "data type = %s.\n", idd_get_type_string(data_type));
  );
/*
   If the data type is undefined force a fatal error.
*/
  if (data_type == idd_undefined) {
    FATAL_ERROR(
      fprintf(
        stderr, 
        "drc_get_atv: attempt to convert to the undefined type\n"
      );
    );
  }
/*
   Get the type and value of the operand.
*/
  op_type = dob_get_base_data_type(
    operand.dob_handle,
    operand.component_name
  );
  rcheck_status("drc_get_atv:  bad status from dob_get_base_data_type", NULL);

  op_data = (caddr_t)dob_ask(operand.dob_handle, operand.component_name);
  rcheck_status("drc_get_atv:bad status from dob_ask", NULL);

  DEBUG3(
    fprintf(
      stderr, 
      "drc_get_atv: converting from %s.\n", 
       idd_get_type_string(op_type)
    );
    fprintf(
      stderr, 
      "drc_get_atv:  value = %d, %f, %x; result = %d, %f, %x.\n",
      op_data, op_data, op_data,
      result, result, result
    );
  ); 
/*
   Get the actual value.
*/
  ret_val = (caddr_t) funcall(data_type, op_type, op_data);
/*
   Return.
*/
  DEBUG2( 
    fprintf(stderr, "drc_get_atv: exiting.\n");
    fprintf(stderr, "\tret_val = %x\n", ret_val);
  );
  return (ret_val);
}					/* end drc_get_atv		    */


/*--------------------------------------------------------------------------*\
|  Routine: checkchar
| 
|  Description:  
|     Check if space, sign, period, or E/e appear in appropriate place
\*--------------------------------------------------------------------------*/
static int checkchar(ch, sp, sign, period, exp)
char ch;
int sp, sign, period, exp;
{
  DEBUG2(
    fprintf(stderr, "checkchar: call.\n");
  );

  DEBUG2(
    fprintf(stderr, "checkchar: exit.\n");
  );

  if (isdigit(ch)) return (1);
  if ((sp == 1) && isspace(ch)) return (1);
  if ((sign == 1) && issign(ch)) return (1);
  if ((period == 1) && isperiod(ch)) return (1);
  if ((exp == 1) && isexp(ch)) return (1);
  else return (NULL);
}


/*--------------------------------------------------------------------------*\
|  Routine: isinteger
| 
|  Description:  
|     Check if an ascii string is a integer string
\*--------------------------------------------------------------------------*/
int isinteger(str)
char *str;
{
   long num;
   char *dummy;
   long strtol();
   int return_val;

    DEBUG2(fprintf(stderr, "isinteger: call.\n"); );

   num = strtol(str	, &dummy, 10);

   if (*dummy == '\0')
      return_val = 1;
    else
      return_val = NULL;

  DEBUG2(
    fprintf(stderr, "isinteger: exit.\n");
  );

  return (return_val);
}

/*--------------------------------------------------------------------------*\
|  Routine: is real
| 
|  Description:  
|     check if an ascii string is a real string.
\*--------------------------------------------------------------------------*/
int isreal(str)
char *str;
{

   double num;
   char *dummy;
   double strtod();
   int return_val;

  DEBUG2(
    fprintf(stderr, "isreal: call.\n");
  );

   num = strtod(str, &dummy);

#ifdef mc68000
/* ????????  dummy is supposed to point to the last byte scanned.  This test
   whether it is a valid number asks is the last byte the end of the string.
   on the Sun, it seems that it returns one past the last byte.  Hence, the
   test for *(dummy-1).  This is very definitely flakey ?????????*/

   if (*(dummy-1) == '\0')
#endif

#ifdef vax
   if (*dummy == '\0')
#endif

      return_val = 1;
    else
      return_val = 0;

   if (strcmp(str,"+") == 0) return_val = 0;
   if (strcmp(str,"-") == 0) return_val = 0;

  DEBUG2(
    fprintf(stderr, "isreal: exit.\n");
  );

  return (return_val);
}


/*--------------------------------------------------------------------------*\
|  Routine: astoi
| 
|  Description:  
|     Takes in ascii and return integer.
\*--------------------------------------------------------------------------*/
static caddr_t astoi(num)
char *num;
{

  int result;

  DEBUG2(
    fprintf(stderr, "astoi: call.\n");
  );

  if (isinteger(num) == NULL) {

    DEBUG1(
      fprintf(stderr, "astoi: failed conversion.\n");
    );

	set_status(failed_conversion);
	return (NULL);
  }

  result = atoi(num);

  DEBUG2(
    fprintf(stderr, "astoi: exit.\n");
  );

  return ((caddr_t) result);
}

/*--------------------------------------------------------------------------*\
|  Routine: astor
| 
|  Description:  
|     Takes in ascii and return real.
\*--------------------------------------------------------------------------*/
static caddr_t astor(num)
char *num;
{

  double *result;

  DEBUG2(
    fprintf(stderr, "astor: call.\n");
  );

  result = (double *)make_node(sizeof(double));

  if (isreal(num) == NULL) {

    DEBUG1(
      fprintf(stderr, "astor: failed conversion.\n");
    );

	set_status(failed_conversion);
	return (NULL);
  }

  *result = atof(num);

  DEBUG2(
	fprintf(stderr, "astor: %e, at %d\n", *result, (int)result);
  );

  /* If *result = HUGE or 0, error! */

  DEBUG2(
    fprintf(stderr, "astor: exit.\n");
  );

  if ((*result == HUGE) || (errno == 34)) {

    DEBUG1(
      fprintf(stderr, "astor: failed conversion.\n");
    );

    set_status(failed_conversion);
    return (NULL); 
  }
  else
  	return ((caddr_t) result);
}

/*--------------------------------------------------------------------------*\
|  Routine: rtoi
| 
|  Description:  
|     Takes in address of real and return integer (rounded real).
\*--------------------------------------------------------------------------*/
static caddr_t rtoi(addr)
caddr_t addr;
{
  int result;
  double f;

  DEBUG2(
    fprintf(stderr, "rtoi: call.\n");
    fprintf (stderr, "rtoi: value to be converted: %e\n", *((double *) addr));
  );

  f = *((double *) addr);
  result = f;
  if ((result - f) >= 0.5)
	result++;
  DEBUG2 (fprintf (stderr, "rtoi: result: %d\n", result);
	fprintf(stderr, "rtoi: exit.\n");
  );

  return ((caddr_t) result);
}

/*--------------------------------------------------------------------------*\
|  Routine: itor
| 
|  Description:  
|     Converts an integer to a real number.
\*--------------------------------------------------------------------------*/
/* takes in an integer and return real */
static caddr_t itor(num)
int num;
{
  double *result;

  DEBUG2(
    fprintf(stderr, "itor: call.\n");
  );

  result = (double *)make_node(sizeof(double));
  *result = num;

  DEBUG2(
    fprintf(stderr, "itor: %d to %f\n", result, *result);
    fprintf(stderr, "itor: exit.\n");
  );

  return ((caddr_t) result);
}

/*--------------------------------------------------------------------------*\
|  Routine: rtoa
| 
|  Description:  
|     converts a real number to an ascii string
\*--------------------------------------------------------------------------*/
static caddr_t rtoa(num1)
caddr_t num1;
{
  char s[BUFSIZ];
  char *str;

  DEBUG2(
    fprintf(stderr, "rtoa: called.\n");
    fprintf(stderr, "rtoa: real number = %f.\n", num1);
  );

  sprintf(s, "%f", *((double *)num1));

  str = u22_copy_string (s);

  DEBUG2(
    fprintf(stderr, "rtoa: string = %s.\n", str);
    fprintf(stderr, "rtoa: exiting.\n");
  );

  return ((caddr_t) str);
}

/*--------------------------------------------------------------------------*\
|  Routine: itoa
| 
|  Description:  
|     converts an integer to an ascii string
\*--------------------------------------------------------------------------*/
static caddr_t itoa(num)
int num;
{
  char s[BUFSIZ];
  char *str;

  DEBUG2(
    fprintf(stderr, "itoa: called.\n");
    fprintf(stderr, "itoa: integer number = %d.\n", num);
  );

  sprintf(s, "%d", num);

  str = u22_copy_string (s);

  DEBUG2(
    fprintf(stderr, "itoa: string = %s.\n", str);
    fprintf(stderr, "itoa: exiting.\n");
  );

  return ((caddr_t) str);
}
	
/*--------------------------------------------------------------------------*\
|  Routine: booltostr
| 
|  Description:  
|     converts a boolean to an ascii string of "true" or "false"
\*--------------------------------------------------------------------------*/
static caddr_t booltostr(bool)
boolean bool;
{
  char *result;

  if (bool != 0)
    result = u22_copy_string ("true");
  else
    result = u22_copy_string ("false");

  return ((caddr_t) result);
}

/*------------------------------------------------------------*\
|  Routine: strtobuff
|
|  Description: 
|   This routine takes as input a string and returns a buffer with
|   the same length and body
\*------------------------------------------------------------*/
static caddr_t strtobuff (arg)
string arg;
{
 idd_buffer_type *result;

  DEBUG2(
    fprintf(stderr, "strtobuff: call.\n");
    fprintf(stderr, "strtobuff: arg = %s.\n", arg);
  );
/* 
   The buffer is the same size as the strlen and does not include
   a null termination character.  In case of zero length strings
   no memory is allocated.
*/
  result = (idd_buffer_type *)make_node(sizeof(idd_buffer_type));
  result->length = strlen(arg);
  result->type = idd_string;
  result->body = NULL;
/*
   If the string has a positive length allocate memory and copy the 
   contents of the string, not including the null termination char.
*/
  if (result->length > 0) {
    result->body = make_node(result->length);
    strncpy(result->body, arg, result->length);
  }

  return ((caddr_t) result);
}

/*------------------------------------------------------------*\
|  Routine: inttobuff
|
|  Description: 
|   This routine takes as input an integer and returns a buffer with
|   the buffer string representation
\*------------------------------------------------------------*/
static caddr_t inttobuff (arg)
int arg;
{
  idd_buffer_type *result;

  DEBUG2(
    fprintf(stderr, "inttobuff: call.\n");
    fprintf(stderr, "inttobuff: arg = %d.\n", arg);
  );

   result = (idd_buffer_type *)ubo_create_buffer();
   result->length = sizeof(arg);
   result->type = idd_integer;
   result->body = make_node(result->length);
   memcpy(result->body, &arg, result->length);

  DEBUG2(
    fprintf(stderr, "inttobuff: result = %x.\n", result);
    fprintf(stderr, "inttobuff: result->length = %d.\n", result->length);
    fprintf(
      stderr,
      "inttobuff:  result->type = %s.\n",
      idd_get_type_string(result->type)
    );
    fprintf(stderr, "inttobuff: result->body = %x.\n", result->body);
    fprintf(stderr, "inttobuff: exit.\n");
  );

  return ((caddr_t) result);
}

/*------------------------------------------------------------*\
|  Routine: idtobuff
|
|  Description: 
|   This routine takes as input an id and returns a buffer with
|   the buffer string representation
\*------------------------------------------------------------*/
static caddr_t idtobuff (arg)
iid_id_type arg;
{
  idd_buffer_type *result;


  DEBUG2(
    fprintf(stderr, "idtobuff: call.\n");
    fprintf(stderr, "idtobuff: arg = %d.\n", arg);
  );

  result = (idd_buffer_type *)make_node(sizeof(idd_buffer_type));
  result->length = sizeof(arg);
  result->type = idd_id;
  result->body = make_node(result->length);
  memcpy(result->body, &arg, result->length);

  DEBUG2(
    fprintf(stderr,"idtobuff: result = %x.\n", result);
    fprintf(stderr,"idtobuff: result->length = %d.\n", result->length);
    fprintf(
      stderr,
      "idtobuff:  result->type = %s.\n",
      idd_get_type_string(result->type)
    );
    fprintf(stderr,"idtobuff: result->body (addr)= %x.\n", result->body);
    fprintf(stderr,"idtobuff: result->body (content)= %x.\n",*(result->body));
    fprintf(stderr,"idtobuff: exit.\n");
  );

  return ((caddr_t) result);
}

/*------------------------------------------------------------*\
|  Routine: realtobuff
|
|  Description: 
|   This routine takes as input the address of a real and returns 
|   a buffer with the same binary representation
\*------------------------------------------------------------*/
static caddr_t realtobuff (arg)
double *arg;
{
  idd_buffer_type *result;


  DEBUG2(
    fprintf(stderr, "realtobuff: call.\n");
    fprintf(stderr, "realtobuff: arg = %d.\n", arg);
  );

   result = (idd_buffer_type *)make_node(sizeof(idd_buffer_type));
   result->length = sizeof(double);
   result->type = idd_real;
   result->body = make_node(result->length);
   memcpy(result->body, &arg, result->length);

  DEBUG2(
    fprintf(stderr, "realtobuff: result = %x.\n", result);
    fprintf(stderr, "realtobuff: result->length = %d.\n", result->length);
    fprintf(
      stderr,
      "realtobuff:  result->type = %s.\n",
      idd_get_type_string(result->type)
    );
    fprintf(stderr, "realtobuff: result->body = %x.\n", result->body);
    fprintf(stderr, "realtobuff: exit.\n");
  );

  return ((caddr_t) result);
}

/*-------------------------------------------------------------------------*\
|  Routine: inttobool
|
|  Description:
|    Converts an integer to a boolean.  Valid integer values are 0
|    (converted to false) and 1 (converted to true)
*\-------------------------------------------------------------------------*/
static caddr_t inttobool(num)
int num;
{
  boolean result;

  DEBUG2(
    fprintf(stderr, "inttobool: call.\n");
    fprintf(stderr, "inttobool: num = %d\n", num);
  );

  if (num == 1 | num == 0) {
      result = num;
  DEBUG2(
    fprintf(stderr, "inttobool: result = %d.\n", result);
    fprintf(stderr, "inttobool: exit.\n");
  );

      return ((caddr_t)result);
  }
  else {
      set_status(failed_conversion);

  DEBUG2(
    fprintf(stderr, "inttobool: exit.failed conversion\n");
  );

      return (NULL);
  }
}

/*---------------------------------------------------------------------------*\
|  Routine: realtobool
|
|  Description:
|    Converts a real to a boolean.  Valid  values are 0
|    (converted to false) and 1 (converted to true)
*\--------------------------------------------------------------------------*/
static caddr_t realtobool(num)
caddr_t num;
{
  int result;

  DEBUG2(
    fprintf(stderr, "realtobool: call.\n");
    fprintf(stderr, "realtobool: num = %f\n", num);
  );

  result = (int)itor(num);

  if ( result |= 1 && result != 0){
     result = NULL;
     set_status(failed_conversion);
  }
  DEBUG2(
    fprintf(stderr, "realtobool: result = %d.\n", result);
    fprintf(stderr, "realtobool: exit.\n");
  );

   return ((caddr_t)result);
}

/*---------------------------------------------------------------------------*\
|  Routine: strtobool
|
|  Description:
|    Converts a string to a boolean.  Valid  values are "false"
|    (converted to false) and "true" (converted to true)
*\--------------------------------------------------------------------------*/
static caddr_t strtobool(arg)
string arg;
{
  int result;

  DEBUG2(
    fprintf(stderr, "strtobool: call.\n");
    fprintf(stderr, "strtobool: arg = %s\n", arg);
  );

  if (strcmp("true",arg) == 0) result = 1;
   else if (strcmp("false",arg) == 0) result = 0;
   else {
     result = NULL;
     set_status(failed_conversion);
   }

  DEBUG2(
    fprintf(stderr, "strtobool: result = %d.\n", result);
    fprintf(stderr, "strtobool: exit.\n");
  );

   return ((caddr_t)result);
}

/*--------------------------------------------------------------------------*\
|  Routine: convert_from_buffer
|
|  Description:
|    Converts a buffer to anything.  Buffers of buffer type can not be
|    converted to anything.  Everything else is handled by a recursive
|    call to funcall.
\*--------------------------------------------------------------------------*/
static caddr_t convert_from_buffer(to_type, arg)
idd_data_types to_type;
idd_buffer_type *arg;
{
  caddr_t result;
  long dummy;

  DEBUG2(
    fprintf(stderr, "convert_from_buffer: call.\n");
    fprintf(stderr, "convert_from_buffer: arg = %x.\n", arg);
    fprintf(stderr, "convert_from_buffer: arg->length = %d.\n", arg->length);
    fprintf(
      stderr, 
      "convert_from_buffer: arg->type = %s.\n", 
      idd_get_type_string(arg->type)
    );
    fprintf(stderr, "convert_from_buffer: arg->body = %x.\n", arg->body);
  );
/*
   If buffer is of buffer type set status to failed conversion.
*/
  if (arg->type == idd_buffer) {
    result = NULL;
    set_status (failed_conversion);

  }
/*
    If not buffer type reinvoke funcall with the actual type 
    and value of the buffer.
*/
  else {
/*
   If the argument is immediate than cast it into a long and then
   reinvoke funcall.
*/
    if (arg->type == idd_boolean || 
        arg->type == idd_integer ||
        arg->type == idd_id
     ) {
      dummy = *((long *)(arg->body));
      result = funcall(to_type, arg->type, (caddr_t)dummy);
    }
/*
   If the argument is not immediate cast it as a pointer.
*/
    else {
      result = funcall(to_type, arg->type, (caddr_t)(arg->body));
    }
  }
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "convert_from_buffer: result = %d.\n", result);
    fprintf(stderr, "convert_from_buffer: exit.\n");
  );

  return (result);
}

/*--------------------------------------------------------------------------*\
|  Routine: booltoint
|
|  Description:
|    Converts a boolean to an integer.  This is easy
\*--------------------------------------------------------------------------*/
static caddr_t booltoint(arg)
int arg;
{
  int result;

  DEBUG2(
    fprintf(stderr, "booltoint: call.\n");
    fprintf(stderr, "booltoint: arg = %d.\n", arg);
  );

  result = arg;

  DEBUG2(
    fprintf(stderr, "booltoint: result = %d.\n", result);
    fprintf(stderr, "booltoint: exit.\n");
  );

   return ((caddr_t)result);
}

/*--------------------------------------------------------------------------*\
|  Routine: booltoreal
|
|  Description:
|    Converts a boolean to an real.  This is easy
\*--------------------------------------------------------------------------*/
static caddr_t booltoreal(arg)
int arg;
{
  double *result;

  DEBUG2(
    fprintf(stderr, "booltoreal: call.\n");
    fprintf(stderr, "booltoreal: arg = %d.\n", arg);
  );

  result = (double *)itor(booltoint(arg));

  DEBUG2(
    fprintf(stderr, "booltoreal: exit.\n");
  );

   return ((caddr_t)result);
}


/*--------------------------------------------------------------------------*\
|  Routine: booltobuff
|
|  Description:
|    Converts a boolean to a buffer.
\*--------------------------------------------------------------------------*/
static caddr_t booltobuff(arg)
boolean arg;
{
  idd_buffer_type *result;

  DEBUG2(
    fprintf(stderr, "booltobuff: call.\n");
    fprintf(stderr, "booltobuff: arg = %d.\n", arg);
  );

  result = (idd_buffer_type *)make_node(sizeof(idd_buffer_type));
  result->length = sizeof(arg);
  result->type = idd_boolean;
  result->body = make_node(result->length);
  memcpy(result->body, &arg, result->length);

  DEBUG2(
    fprintf(stderr, "booltobuff: result = %x.\n", result);
    fprintf(stderr, "booltobuff: result->length = %d.\n", result->length);
    fprintf(
      stderr,
      "booltobuff:  result->type = %s.\n",
      idd_get_type_string(result->type)
    );
    fprintf(stderr, "booltobuff: result->body = %x.\n", result->body);
    fprintf(stderr, "booltobuff: exit.\n");
  );

   return ((caddr_t)result);
}
