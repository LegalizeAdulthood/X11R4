/*      $Header: /u/04c/mnt/integ/int/include/RCS/isd1.h,v 1.2 89/02/09 18:46:47 ejh Exp $ */

/*--------------------------------------------------------------------------*\
| 
|  Name:  Shared data abstraction (isd)
|
|  Description:  
|     The shared data abstraction provides a means of communicating data
|     between the various processes of the SERPENT system. 
|
|  Component of:
|     Interface Layer.
|
|  Related files:
|     isd1.c     - contains part 1 of the implementation for the shared data 
|                  abstraction.
|     isd2.c     - contains part 2 of the implementation for the shared data
|                  abstraction.
|     isd.h      - contains the external definition for the shared data
|                  abstraction.
|     isd_priv.h - contains the internal type definitions for the shared data 
|                  abstraction.
|     isd1.h     - contains internal definitions for part1 of the shared data 
|                  abstraction.
|     isdtest.c  - test file.
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
$Log:	isd1.h,v $
 * Revision 1.2  89/02/09  18:46:47  ejh
 * added copyright notice
 * 
|     Nov. 19, 1987 : Module created.
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

/*--------------------------------------------------------------------------*\
|  Routine: match_id
| 
|  Description:  
|     Routine to compare the identifiers with user elements.
|   
|
\* Syntax:                                                                  */

	int match_id(
	  /* id : in iid_id_type 				*/
	  /* element_change in out : element_change_record	*/
	);

/*
|  Parameters:
|      id - Match this ID.
|      element_change - In this element change record.
|
|  Returns:
|      True if match, false if no match.
|
|  Statuses:
|      ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: hash_id
| 
|  Description:  
|     Internal function which will convert an id into indecies
|     in the array of hash lists.
|
\* Syntax:                                                                  */

	int hash_id(
	  /* id : in iid_id_type */
	);

/*
|  Parameters:
|      id - Id to covert into hash index.
|
|  Returns:
|      Hash index.
|
|  Statuses:
|      ok.
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Routine: free_element_change_record
| 
|  Description:  
|     Internal routine to free storage associated with an element change 
|     record.
|
\* Syntax:                                                                  */

	void free_element_change_record(
	  /* nada : in char * 					*/
	  /* element_change in out : element_change_record	*/
	);

/*
|  Parameters:
|      nada - Dummy parameter.
|      element_change - Pointer to element change record to be destroyed.
|
|  Returns:
|      Void.
|
|  Statuses:
|      ok.
|
\*--------------------------------------------------------------------------*/
