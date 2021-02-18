	static char rcsid[]= "$Header: /u/04c/mnt/new_integ/externs/src/RCS/arith.c,v 1.1 89/10/08 23:15:35 serpent Exp $";

/*--------------------------------------------------------------------------*\
|   
|  Name:  arith.c
|
|  Description:  
|     This file defines extended arithmetic functions for Slang programs.
|
|  Component of:
|     Function libraries.
|
|  Related files:
|     arith.ext - Slang declarations of the functions.
|
|  Project:
|     Serpent
|     User Interface Project
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
|     Brian M. Clapper
|     bmc@sei.cmu.edu  
|
$Log:	arith.c,v $
 * Revision 1.1  89/10/08  23:15:35  serpent
 * Initial revision
 * 
 * Revision 1.1  89/09/12  18:40:11  bmc
 * Initial revision
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
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: make_integer
|
|  Description:
|     Converts a Slang expression to an integer, forcing truncation if
|     necessary.
|
|  Parameters:
|     operand - The expression or variable to be converted to integer.
|               While the above syntax implies that only integers are
|               accepted, in reality, any type of expression may be passed.
|               Conversion is handled by the Slang run-time system.
|
|  Result:
|     An integer version of the operand is passed back.  Real data is
|     truncated, not rounded.
|
|  Notes:
|     This function is basically just a no-op.  By the time it is called,
|     the generate Slang code has already converted the argument integer.
|     The function is only necessary because there has to be *something*
|     around to call.
\*                                                                          */

int make_integer (the_integer)
int the_integer;
{
    return the_integer;
}
