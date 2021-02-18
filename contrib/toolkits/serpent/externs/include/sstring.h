/* $Header: /u/04c/mnt/new_integ/externs/include/RCS/sstring.h,v 1.1 89/10/08 23:12:00 serpent Exp $ */

/*--------------------------------------------------------------------------*\
|   
|  Name:  sstring.h
|
|  Description:  
|     This file defines string macros for use with Slang programs.  It is
|     a separate file from "sstring.ext" because the macros (and constants)
|     need to be shared by Slang programs and the C implementation of the
|     other functions.  Because of the difference in Slang and C external
|     function definitions, only the macro definitions can be shared.  The
|     function definitions must reside in separate files.
|
|  Component of:
|     External function libraries.
|
|  Related files:
|     sstring.c   - contains the C implementation of the functions.
|     sstring.ext - contains external Slang binding for these functions.
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
$Log:	sstring.h,v $
 * Revision 1.1  89/10/08  23:12:00  serpent
 * Initial revision
 * 
 * Revision 1.1  89/09/07  10:04:02  bmc
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

#ifndef _STRING_MACROS_H_
#define _STRING_MACROS_H_

/*--------------------------------------------------------------------------*\
|  Constant: to_string_end
|
|  Description:
|     Used as a place-holder in cases where you need to specify end of string,
|     but you don't need to know exactly how long it is.  (E.g., you want to
|     delete from position 2 to the end of the string, but you don't want to
|     waste time calling string_length()).
\*                                                                          */
#define to_string_end	(-1)
#define TO_STRING_END	to_string_end

/*--------------------------------------------------------------------------*\
|  Macro: string_append
|
|  Description:
|     The string_append macro concatentates the second string to the 
|     end of the first string and returns the concatentated string.  
|
\*                                                                          */

#define string_append(string_1,string_2) \
	string_insert(string_1,string_2,string_length(string_1) + 1)

/*
|  Parameters:
|     string_1 - The base string to which to append.
|     string_2 - The string to be appended to string_1.
|
|  Result:
|     A new string is returned containing the result.  If an error occurs,
|     "" is returned.  Space for the new string is allocated via a call
|     to make_node().
|
\*--------------------------------------------------------------------------*/

#endif  _STRING_MACROS_H_
