	/* $Header: /u/04c/mnt/integ/utilities/include/RCS/ubo.h,v 1.6 89/11/01 17:25:14 little Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:   Buffer Ops Utility  (ubo)
|
|  Description:  
|     The buffer ops utility provides basic buffer operations on buffers.
|
|  Component of:
|     Dialogue Manager Layer.
|
|  Related files:
|     ubo.c - contains the implementation for the transactionmanager.
|     ubo.h - contains the external definition for the transaction manager.
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
$Log:	ubo.h,v $
 * Revision 1.6  89/11/01  17:25:14  little
 * change the default debug level to 1 and protect thee definition of
 * memorypack
 * 
 * Revision 1.5  89/10/03  18:10:33  bmc
 * Added ubo_init_buffer (a.k.a. init_buffer) macro.
 * 
 * Revision 1.4  89/09/27  19:31:07  bmc
 * Added NULL_BUFFER constant
 * 
 * Revision 1.3  89/09/27  16:27:53  bmc
 *  Added ubo_create_buffer().
 * 
 * Revision 1.2  89/02/10  14:33:37  ejh
 * added copyright notice
 * 
|     May 24, 1988 : Module created.
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

#ifndef ubo
#define ubo

/*--------------------------------------------------------------------------*\
|  Includes: isd
|
|  Description:
|     The following is a list of all the necessary includes for the isd
|     abstraction.
|
\* Definition:                                                              */

#include <stdio.h>              /* C Standard I/O Package                   */
#include <errno.h>              /* Standard Ultrix error numbers            */

#include <sys/types.h>          /* System types                             */

#ifndef memoryPack
#define memoryPack              /* use memoryPack rather than mallocPack    */
#endif
#include <listPack.h>           /* C Toolbox - listPack                     */
#include <hashPack.h>           /* C Toolbox - hashPack                     */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */

#include "isd.h"		/* interface shared data abstraction	    */
#include "cus.h"		/* compiler symbol table management package */

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Global Variable: ubo_debug
|
|  Description:
|     This is the debug variable for the buffer operations utility.
|
\* Definition:                                                              */

#ifdef ubo_main
  int ubo_debug = 1;
#else
  extern int ubo_debug;
#endif

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constants: NULL_BUFFER, null_buffer
|
|  Description:
|     These constants provide a convenient notation for expressing the
|     concept of a NULL buffer pointer.
|
\* Definition:                                                              */

#define NULL_BUFFER	((idd_buffer_type *) NULL)
#define null_buffer	NULL_BUFFER


/*--------------------------------------------------------------------------*\
|  Macro: ubo_init_buffer or init_buffer
|
|  Description:
|     Initializes the contents of a buffer structure.  WARNING:  If the
|     buffer body pointer points to an allocated block of memory, the
|     pointer will be lost, as this macro simply zeros the body pointer.
|
\* Definition:                                                              */

#define ubo_init_buffer(the_buffer) \
    the_buffer.body   = UNDEFINED_BUFFER_BODY; \
    the_buffer.length = UNDEFINED_BUFFER_LENGTH; \
    the_buffer.type   = idd_undefined

#define init_buffer(the_buffer) ubo_init_buffer(the_buffer)

/*--------------------------------------------------------------------------*\
|  Constants: buffcmp, buffcpy
|
|  Description:
|     These constants provide short external names for the buffer ops.
|
\* Definition:                                                              */

#define buffcmp ubo_buffcmp
#define buffcpy ubo_buffcpy

/*
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ubo_buffcmp
| 
|  Description:  
|     Compares two existing buffers and returns true if equal or false if
|     not equal.
| 
\* Syntax:                                                                  */

        boolean ubo_buffcmp(
          /* buff1 : in idd_buffer_type *  */
          /* buff2 : in idd_buffer_type *  */
        );

/* 
|  Parameters:
|     buff1 - Pointer to first buffer.
|     buff2 - Pointer to second buffer.
|
|  Returns:  
|     True if buffers equal; false otherwise.
|  
|  Statuses:  
|     ok.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ubo_buffcpy
| 
|  Description:  
|     Copies the contents of buff2 to buff1.  The buffer structure is
|     assumed to exist but storage for the buffer body will be malloced
|     and any existing pointer overwritten.
| 
\* Syntax:                                                                  */

        void ubo_buffcpy(
          /* buff1 : in idd_buffer_type *  */
          /* buff2 : in idd_buffer_type *  */
        );

/* 
|  Parameters:
|     buff1 - Pointer to buffer to copy to.
|     buff2 - Pointer to buffer to copy.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ubo_create_buffer
| 
|  Description:  
|     Creates a buffer of undefined length, suitable for assignment via
|     ubo_buffcpy.
| 
\* Syntax:                                                                  */

        idd_buffer_type *ubo_create_buffer ();

/* 
|  Parameters:
|     None.
|
|  Returns:  
|     Address of allocated buffer.  Returns NULL if an error occurs.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: ubo_destroy_buffer
| 
|  Description:  
|     Destroys an existing buffer.
| 
\* Syntax:                                                                  */

        void ubo_destroy_buffer(
          /* buff : in idd_buffer_type *  */
        );

/* 
|  Parameters:
|     buff - Pointer to buffer to copy to.
|
|  Returns:  
|     Void.
|  
|  Statuses:  
|     ok, out_of_memory.
|  
\*--------------------------------------------------------------------------*/
#endif
