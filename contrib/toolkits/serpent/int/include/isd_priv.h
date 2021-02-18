/*      $Header: /u/04c/mnt/integ/int/include/RCS/isd_priv.h,v 1.2 89/02/09 18:48:22 ejh Exp $ */

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
|     isd.c - contains the implementation for the shared data abstraction.
|     isd.h - contains the external definition for the shared data abstr.
|     isdtest.c - test file.
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
$Log:	isd_priv.h,v $
 * Revision 1.2  89/02/09  18:48:22  ejh
 * added copyright notice
 * 
|     Nov. 19, 1987 : Module created.
|     May  11, 1988 : put define protection in               ljb
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

#ifndef isd_priv
#define isd_priv

#define MAX_HASH 256

/*-------------------- Type Definitions ------------------------------------*/

typedef struct {                        /* change record structure          */
  isc_validation_stamp validation_stamp;
  iid_id_type id;
  string element_name;
  isd_change_type change_type;
  ubs_bitstring changed_components;
  caddr_t data;
} element_change_record;

typedef struct {
  isc_validation_stamp validation_stamp;
  ipc_mailbox mailbox;
  idd_data_definition data_definition;
  FILE *record_fp;
  boolean recording; 
  string ill_file_name;
  string mailbox_name;
} process_record;

typedef struct {			/* transaction structure	    */
  isc_validation_stamp validation_stamp;
  process_record *receiver;
  SHM_HASH change_table;
} transaction_record;

typedef struct {			/* received transaction structure   */
  isc_validation_stamp validation_stamp;
  ipc_mailbox sender;
  idd_data_definition data_definition;
  element_change_record *current_change;
  SHM_HASH change_table;
  SHM_LIST change_list;
} rcvd_tran;
#endif
