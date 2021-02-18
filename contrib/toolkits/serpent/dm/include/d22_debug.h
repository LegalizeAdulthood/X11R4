/*
$Header: /u/04c/mnt/integ/dm/include/RCS/d22_debug.h,v 1.5 89/11/02 17:22:48 little Exp $
*/

/*
$Log:	d22_debug.h,v $
 * Revision 1.5  89/11/02  17:22:48  little
 * put the debug macros under NDEBUG
 * 
 * Revision 1.4  89/09/20  12:09:25  little
 * no changes
 * 
 * Revision 1.3  89/08/29  17:41:44  little
 * change definition of d22_slang_source so the addition rt error print
 * works with the new way the compiler is generating the string
 * 
 * Revision 1.2  89/02/09  16:40:29  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/04  16:58:13  little
 * Initial revision
 * 
*/

/*--------------------------------------------------------------------------*\
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

#ifndef d22_debug
#define d22_debug

extern int d22_slang_line_num;
extern caddr_t d22_slang_source;

#ifndef NDEBUG
#define DM_RT_DEBUG1(flag, statement)\
  if(flag > 0) {\
    statement;\
    fprintf (stderr, "\tfile %s, line %d, ", __FILE__, __LINE__);\
    fprintf (stderr, "at or near SLANG line %d\n", d22_slang_line_num);\
    fprintf (stderr, "\t  in %s\n", d22_slang_source);\
    fflush(stderr);\
  }
#else
#define DM_RT_DEBUG1(flag, statement)
#endif

#endif

