/*	$Header: /u/04c/mnt/integ/int/include/RCS/global.h,v 1.6 89/10/05 17:20:43 rcs Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  Global
|
|  Description:  
|     This header file includes standard type definitions
|     used within the SERPENT system.
|
|  Component of:
|     Initialization and Cleanup Layer.
|
|  Related files:
|     global.h - Standard SERPENT definitions.
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
|     Oct. 19, 1987 : Module created.
|     Jun. 20, 1988 : Added undefined constant values. - rCs
$Log:	global.h,v $
 * Revision 1.6  89/10/05  17:20:43  rcs
 * no change
 * 
 * Revision 1.5  89/06/01  10:57:56  little
 * change definition of boolean from chort to int
 * 
 * Revision 1.4  89/02/17  03:44:41  ejh
 * true and false are now only defined if they haven't previously been
 * 
 * Revision 1.3  89/02/09  18:35:24  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/12/05  17:20:44  rcs
 * Changed the value of UNDEFINED_STRING to a two byte value.
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

#ifndef global
#define global

/*--------------------------------------------------------------------------*\
|  Name: private
| 
|  Description:  
|     Private is used to define types for which are private to the data
|     abstraction, for example handles.
| 
\* Syntax:                                                                  */

	typedef char *private;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: string
| 
|  Description:  
|     The string type is used to define a pointer to an array of characters
|     terminated by /0.
| 
\* Syntax:                                                                  */

	typedef char *string;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: boolean
| 
|  Description:  
|     The boolean type is used to define variables 
| 
\* Syntax:                                                                  */

	typedef int boolean;

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: undefined values
| 
|  Description:  
|     The following constants define undefined values for all types 
|     supported by Serpent.  Record is kind of funny; also it is 
|     best to determine undefined buffer by examining the buffer length.
| 
\* Syntax:                                                                  */

#define UNDEFINED_BOOLEAN	(boolean)0xAAAAAAAA
#define UNDEFINED_INTEGER	(int)0xAAAAAAAA
#define UNDEFINED_REAL		-7.57912e-14
#define UNDEFINED_STRING	"\252"
#define UNDEFINED_RECORD	(caddr_t)0xAAAAAAAA
#define UNDEFINED_ID		(iid_id_type)0xAAAAAAAA
#define UNDEFINED_BUFFER_LENGTH	(int)-1
#define UNDEFINED_BUFFER_BODY	(caddr_t)0xAAAAAAAA

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: true
| 
|  Description:  
|     True condition for boolean variables.
| 
\* Syntax:                                                                  */

#ifndef true
#define true 1 
#endif
#ifndef True
#define True 1 
#endif
#ifndef TRUE
#define TRUE 1 
#endif

#define on 1 

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: false
| 
|  Description:  
|     False condition for boolean variables.
| 
\* Syntax:                                                                  */

#ifndef false
#define false 0 
#endif
#ifndef False
#define False 0 
#endif
#ifndef FALSE
#define FALSE 0 
#endif

#define off 0 

/*
\*--------------------------------------------------------------------------*/
#endif
