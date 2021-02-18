/*      $Header: /u/04c/mnt/integ/int/include/RCS/serpent.h,v 1.3 89/02/09 18:52:18 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:  Serpent.h
|
|  Description:  
|     Interface file to be included by application to access Serpent 
|     functions.
|
|  Component of:
|     Interface Layer.
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
|     Jan 11, 1988 : Module created.
|     $Log:	serpent.h,v $
 * Revision 1.3  89/02/09  18:52:18  ejh
 * added copyright notice
 * 
 * Revision 1.2  88/10/19  14:51:07  rcs
 * moved serpent define to top of file - rCs
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

#ifndef serpent
#define serpent

/*--------------------------------------------------------------------------*\
|  Includes: serpent
| 
|  Description:  
|     The following is a list of all the necessary includes for the isd
|     abstraction.
| 
\* Definition:                                                              */

#include "global.h"             /* Global type defs and constants           */
#include "isc.h"                /* status code                              */
#include "iid.h"                /* id abstraction                           */
#include "isd.h"                /* shared data abstraction  	            */

/* 
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constant:  External Names
| 
|  Description:  
|     The following contants define external names for SERPENT functions,
|     data types and constants.
| 
\* Definition:                                                              */

/*
   Types and defines required and used in saddle.
*/
#define buffer  idd_buffer_type
#define id_type iid_id_type
#define null_id iid_null_id

#define serpent_data_types idd_data_types
#define serpent_boolean idd_boolean
#define serpent_integer idd_integer
#define serpent_real idd_real
#define serpent_string idd_string
#define serpent_record idd_record
#define serpent_id idd_id
#define serpent_buffer idd_buffer

#define transaction_type isd_trans

#define change_type isd_change_type
#define no_change isd_no_change 
#define create isd_create
#define modify isd_modify
#define remove isd_remove
#define get isd_get

#define serpent_init isd_init
#define start_transaction isd_start_transaction
#define start_transaction isd_start_transaction
#define add_shared_data isd_add_shared_data
#define put_shared_data isd_put_shared_data
#define remove_shared_data isd_remove_shared_data
#define rollback_transaction isd_rollback_transaction
#define commit_transaction isd_commit_transaction

#define get_transaction isd_get_transaction
#define get_transaction_no_wait isd_get_transaction_no_wait
#define get_first_changed_element isd_get_first_changed_element
#define get_next_changed_element isd_get_next_changed_element
#define get_element_name isd_get_element_name
#define get_change_type isd_get_change_type
#define create_changed_component_list isd_create_changed_component_list
#define destroy_changed_component_list isd_destroy_changed_component_list
#define incorporate_changes isd_incorporate_changes
#define get_shared_data isd_get_shared_data
#define get_shared_data_type isd_get_shared_data_type
#define create_component_list isd_get_component_list
#define destroy_component_list isd_get_component_list
#define purge_transaction isd_purge_transaction
#define serpent_cleanup isd_cleanup

#define start_recording ipb_start_recording
#define stop_recording ipb_stop_recording
#define playback ipb_playback

#define status_codes isc_status_codes
#define print_status isc_print_status
/* #define get_status get_status */

/* 
\*--------------------------------------------------------------------------*/
#endif
