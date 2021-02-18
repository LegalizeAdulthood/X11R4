#include "serpent.h"
#include "dra.h"
#include "dro.h"
#include "dob.h"
#include "dob_make.h"
#include "dpt.h"
#include "d22.h"

static char rcsid [] =
        "$Header: /u/04c/mnt/new_integ/utilities/src/RCS/u22_rt.c,v 1.1 89/10/08 00:13:34 serpent Exp $";

/*
$Log:	u22_rt.c,v $
 * Revision 1.1  89/10/08  00:13:34  serpent
 * Initial revision
 * 
 * Revision 1.3  89/05/26  10:50:39  little
 * change order of includes so everything compiles cleanly.  the change was
 * necessetatied due the change of what d22_sne's type is (in d22.h).
 * 
 * Revision 1.2  89/05/25  13:27:41  little
 * add #include for dpt.h
 * 
 * Revision 1.1  89/05/19  10:44:50  little
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

/*
	This file is used purely to build the c22_runtime.h header file.
	c22_runtime.h exists so the compiles of the slang compiler produced
	c code will compile faster.  c22_runtime.h contains a preprocessed
	version of all the headers necessary to compile the slang compiler
	produced code and nothing more (for the most part).
*/

