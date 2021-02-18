/*      $Header: /u/04c/mnt/integ/int/include/RCS/ubs.h,v 1.3 89/02/09 18:53:48 ejh Exp $ */
/*--------------------------------------------------------------------------*\
|
|  Name:  Bit String Utilities
|
|  Description:  
|     This utility contains general purpose routines to create, modify 
|     and destroy bitstrings.
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
|  Owner:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     Jan 6, 1988 : Module created.
|     $Log:	ubs.h,v $
 * Revision 1.3  89/02/09  18:53:48  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/11/02  15:12:02  rcs
 * moved debug variable from c file
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

#ifndef ubs

/*--------------------------------------------------------------------------*\
|  Include: ubs
|
|  Description:
|     Necessary includes for the ubs abstraction.
|
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include <errno.h>              /* Standard Ultrix error numbers            */

#include <memoryPack.h>         /* C Toolbox - memoryPack                   */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: ubs_debug
|
|  Description:
|     Debug variable for the abstraction.
|
\* Definition:                                                              */

#ifdef ubs_main
  int ubs_debug = 1;
#else
  extern int ubs_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Abstract Type:  ubs_bitstring
|
|  Description:
|     The bitstring abstract type consists of a string of "n" bits which can
|     be separately turned on and off.
|
\* Definition:                                                              */

	typedef private ubs_bitstring;

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ubs_create_bitstring
| 
|  Description:  
|     The ubs_create_bit_string routine creates a string of "n" bits and 
|     returns a bitstring handle.
|
\* Syntax:                                                                  */

	ubs_bitstring ubs_create_bitstring(
	  /* n : in int */
	);

/*
|  Parameters:
|     n - Length of bitstring to create.
|
|  Returns:
|     A bitstring handle.
|
|  Statuses:
|     ok, out_of_memory.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ubs_set_bitstring
| 
|  Description:  
|     The set bitstring routine sets each bit in a bitstring of "n" bits.
|
\* Syntax:                                                                  */

	void ubs_set_bitstring(
	  /* bitstring : in out ubs_bitstring */
	  /* n : in int */
	);

/*
|  Parameters:
|     bitstring - Bitstring to modified.
|     n - The number of bits in the bitstring.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ubs_set_bit
| 
|  Description:  
|     The set bit routine is used to set the "nth" bit in the 
|     bitstring.
|
\* Syntax:                                                                  */

	void ubs_set_bit(
	  /* bitstring : in out ubs_bitstring */
	  /* n : in int */
	);

/*
|  Parameters:
|     bitstring - Bitstring to be modified.
|     n - The bit in the bitstring to set.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ubs_reset_bit
| 
|  Description:  
|     The ubs_reset_bit routine is used to reset the "nth" bit in the 
|     bitstring.
|
\* Syntax:                                                                  */

	void ubs_reset_bit(
	  /* bitstring : in out ubs_bitstring */
	  /* n : in int */
	);

/*
|  Parameters:
|     bitstring - Bitstring to destroy.
|     n - the bit in the bitstring to reset.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ubs_test_bit
| 
|  Description:  
|     The ubs_test_bit routine is used to test the "nth" bit in the bitstring.
|
\* Syntax:                                                                  */

	boolean ubs_test_bit(
	  /* bitstring : in out ubs_bitstring */
	  /* n : in int */
	);

/*
|  Parameters:
|     bitstring - Bitstring to destroy.
|     n - the bit in the bitstring to test.
|
|  Returns:
|     Returns the value of the bit.
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: ubs_destroy_bitstring
| 
|  Description:  
|     The ubs_destroy routine is used to destroy an existing bitstring.
|
\* Syntax:                                                                  */

	void ubs_destroy_bitstring(
	  /* bitstring : in out ubs_bitstring */
	);

/*
|  Parameters:
|     bitstring - Bitstring to destroy.
|
|  Returns:
|     Void.
|
|  Statuses:
|     ok.
|
\*--------------------------------------------------------------------------*/
#define ubs
#endif

