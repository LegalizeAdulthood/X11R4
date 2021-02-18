static char rcsid [] = "$Header: /u/04c/mnt/integ/int/src/RCS/ubs.c,v 1.3 89/02/09 19:14:22 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  Bit String Utilities
|
|  Description:  
|     This utility contains general purpose routines to create, modify 
|     and destroy bitstrings in shared memory.
|
|     Internally the bit string is represented by n bytes arranged in 
|     the following fashion:
|     
|     0                1                 2  
|     -----------------------------------------------------
|     |0 1 2 3 4 5 6 7 | 0 1 2 3 4 5 6 7 | 0 1 2 3 4 5 6 7|
|     -----------------------------------------------------
|
|  Component of:
|     Utilities Layer.
|
|  Related files:
|     ubs.c - contains the implementation for the hash utility package.
|     ubs.h - contains the external definition for hash utility package.
|     ubstest.c - test file.
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
|     Jan 6, 1988 : Module created.
|     $Log:	ubs.c,v $
 * Revision 1.3  89/02/09  19:14:22  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/11/02  15:12:55  rcs
 * moved debug variable to interface file
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

#define ubs_main
#include "ubs.h"                /* bitstring utility			    */

/*-------------------- Constants -------------------------------------------*/

#define DEBUG1(statement) SERPENT_DEBUG1(ubs_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(ubs_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(ubs_debug, statement)

#define BYTE_SIZE 8

/*-------------------- Type Definitions ------------------------------------*/


/*-------------------- Retained Data ---------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ubs_create_bitstring
| 
|  Description:  
|     The ubs_create_bit_string routine creates a string of "n" bits and 
|     returns a bitstring handle.
\*--------------------------------------------------------------------------*/
ubs_bitstring ubs_create_bitstring(n)
int n;
{                                       /* local variable definitions       */
ubs_bitstring bitstring;
int size, i;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "ubs_create_bitstring: call.\n");
    fprintf(stderr, "ubs_create_bitstring: n = %d.\n", n);
  );
/*
   Allocate the number of bytes of memory necessary to hold n bits.
*/
  size = n/BYTE_SIZE;
  if (n % BYTE_SIZE != 0) {		/* leftover bits		    */
    size++;
  }					/* end if leftover bits		    */
  bitstring = (ubs_bitstring)ism_get_shared_memory(size);
/*
   Initialize string to zero.
*/
  for (i = 0; i < size; i++) {		/* for each byte in bitstring	    */
    *(bitstring+i) = 0x00;
  }					/* end for each byte in bitstring   */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ubs_create_bitstring: bitstring = %x.\n", bitstring);
    fprintf(stderr, "ubs_create_bitstring: exiting.\n");
  );
  return bitstring;
}                                       /* end ubs_create_bitstring	    */

/*--------------------------------------------------------------------------*\
|  Name: ubs_set_bitstring
| 
|  Description:  
|     The ubs set bit string routine sets each bit in a bitstring of
|     "n" bits.
\*--------------------------------------------------------------------------*/
void ubs_set_bitstring(bitstring, n)
ubs_bitstring bitstring;
int n;
{                                       /* local variable definitions       */
  int size, i;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "ubs_set_bitstring: call.\n");
    fprintf(stderr, "ubs_set_bitstring: bitstring addr = %x.\n", bitstring);
    fprintf(stderr, "ubs_set_bitstring: bitstring = %x.\n", *bitstring);
    fprintf(stderr, "ubs_set_bitstring: n = %d.\n", n);
  );
/*
   Determine the size in bytes of the bit string.
*/
  size = n/BYTE_SIZE;
  if (n % BYTE_SIZE != 0) {		/* leftover bits		    */
    size++;
  }					/* end if leftover bits		    */

  DEBUG3(
    fprintf(stderr, "ubs_set_bitstring: size = %d.\n", size);
  );

/*
   Set bitstring.
*/
  for (i = 0; i < size; i++) {		/* for each byte in bitstring	    */
    *(bitstring+i) = 0xFF;
  }					/* end for each byte in bitstring   */
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ubs_set_bitstring: exiting.\n");
  );
  return;
}                                       /* end ubs_set_bitstring	    */

/*--------------------------------------------------------------------------*\
|  Name: ubs_set_bit
| 
|  Description:  
|     The ubs_set_bit routine is used to set the "nth" bit in the 
|     bitstring.
\*--------------------------------------------------------------------------*/
void ubs_set_bit(bitstring, n)
ubs_bitstring bitstring;
int n;
{                                       /* local variable definitions       */
  int byte, offset;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "ubs_set_bit: call.\n");
    fprintf(stderr, "ubs_set_bit: bitstring addr = %x.\n", bitstring);
    fprintf(stderr, "ubs_set_bit: bitstring = %x.\n", *bitstring);
    fprintf(stderr, "ubs_set_bit: n = %d.\n", n);
  );
/*
   Determine which byte is affected and the offset inside that byte.
*/
  byte = n / BYTE_SIZE;
  offset = n % BYTE_SIZE;
/*
   Create an OR flag by shifting the number one by offset positions
   and then or this with the correct byte in the bitstring.
*/
  *(bitstring+byte) = *(bitstring+byte) | (1 << offset);
/*
   Return.
*/
  DEBUG2(
    fprintf(
      stderr, 
      "ubs_set_bit: bitstring = %x.\n", *(bitstring+byte) | (1<<offset))
    ;
    fprintf(stderr, "ubs_set_bit: exiting.\n");
  );
  return;
}                                       /* end ubs_set_bit    */

/*--------------------------------------------------------------------------*\
|  Name: ubs_reset_bit
| 
|  Description:  
|     The ubs_reset_bit routine is used to reset the "nth" bit in the 
|     bitstring.
\*--------------------------------------------------------------------------*/
void ubs_reset_bit(bitstring, n)
ubs_bitstring bitstring;
int n;
{                                       /* local variable definitions       */
  int byte, offset;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "ubs_reset_bit: call.\n");
    fprintf(stderr, "ubs_reset_bit: bitstring = %x.\n", bitstring);
    fprintf(stderr, "ubs_reset_bit: n = %d.\n", n);
  );
/*
   Determine which byte is affected and the offset inside that byte.
*/
  byte = n / BYTE_SIZE;
  offset = n % BYTE_SIZE;
/*
   Create an AND flag by shifting the number one by offset positions
   and then or this with the correct byte in the bitstring.
*/
  *(bitstring+byte) & (1 << offset);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ubs_reset_bit: exiting.\n");
  );
  return;
}                                       /* end ubs_reset_bit    */

/*--------------------------------------------------------------------------*\
|  Name: ubs_test_bit
| 
|  Description:  
|     The ubs_test_bit routine is used to test the "nth" bit in the bitstring.
\*--------------------------------------------------------------------------*/
boolean ubs_test_bit(bitstring, n)
ubs_bitstring bitstring;
int n;
{                                       /* local variable definitions       */
  int byte, offset;
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "ubs_test_bit: call.\n");
    fprintf(stderr, "ubs_test_bit: bitstring addr = %x.\n", bitstring);
    fprintf(stderr, "ubs_test_bit: bitstring = %x.\n", *bitstring);
    fprintf(stderr, "ubs_test_bit: n = %d.\n", n);
  );
/*
   Determine which byte is affected and the offset inside that byte.
*/
  byte = n / BYTE_SIZE;
  DEBUG3(
    fprintf(stderr, "ubs_test_bit: byte = %d.\n", byte);
  );

  offset = n % BYTE_SIZE;
  DEBUG3(
    fprintf(stderr, "ubs_test_bit: offset = %d.\n", offset);
  );
/*
   Create an AND flag by shifting the number one by offset positions
   and then or this with the correct byte in the bitstring and return
   the value.
*/
  DEBUG2(
    fprintf(
       stderr, 
      "ubs_test_bit: ubs_test_bit = %d.\n", 
      (*(bitstring+byte) & (1 << offset)) != 0
    );
    fprintf(stderr, "ubs_test_bit: exiting.\n");
  );
  return(*(bitstring+byte) & (1 << offset)) != 0;
}                                       /* end ubs_test_bit		    */

/*--------------------------------------------------------------------------*\
|  Name: ubs_destroy_bitstring
| 
|  Description:  
|     The ubs_destroy routine is used to destroy an existing bitstring.
\*--------------------------------------------------------------------------*/
void ubs_destroy_bitstring(bitstring)
ubs_bitstring bitstring;
{                                       /* local variable definitions       */
/*
   Initialization.
*/
  set_status(ok);                       /* begin                            */
  DEBUG2(
    fprintf(stderr, "ubs_destroy_bitstring: call.\n");
    fprintf(stderr, "ubs_destroy_bitstring: bitstring = %x.\n", bitstring);
  );
/* 
   Free storage associated with bitstring.
*/
  ism_free_shared_memory(bitstring);
/*
   Return.
*/
  DEBUG2(
    fprintf(stderr, "ubs_destroy_bitstring: exiting.\n");
  );
  return;
}                                       /* end ubs_destroy_bitstring	    */

