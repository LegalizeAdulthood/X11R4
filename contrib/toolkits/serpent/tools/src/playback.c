static char rcsid [] =
	"$Header: /u/04c/mnt/integ/tools/src/RCS/playback.c,v 1.8 89/10/23 14:28:15 ljb Exp $";

/*--------------------------------------------------------------------------*\
| 
|  Name:  Playback 
|
|  Description:  
|     The playback utility calls the playback procedures to actually play
|     back a journalling file.  It is invoked with the following args:
|      journaling file: the name of the journaling file
|      host mbx: the mailbox for the process to be simulated
|      list of correspondents (default is "all")
|
|  Component of:
|     Utilities.
|
|  Related files:
|     ipb1.c - contains the recording portion of the playback
|     ipb3.c - contains the interface portion of the playback routines
|     ipb.h - contains the external definition for the playback abstr.
||
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
|     Len Bass
|     ljb@sei.cmu.edu  
|
|  Change History:
$Log:	playback.c,v $
 * Revision 1.8  89/10/23  14:28:15  ljb
 * make work when playing back non dm process
 * 
 * Revision 1.7  89/09/28  15:56:14  ljb
 * no change
 * 
 * Revision 1.6  89/09/19  16:20:12  ljb
 * test of end of file by treating ipb_get_next_trans as boolean
 * 
 * Revision 1.5  89/09/19  14:26:52  ljb
 * check status after some of the calls to ipb routines
 * 
 * Revision 1.4  89/06/01  15:39:45  ejh
 * general cleanup of code; fixed include list
 * 
 * Revision 1.3  89/02/13  14:37:19  ejh
 * added copyright notice
 * 
 * Revision 1.2  89/02/09  17:36:39  little
 * no changes
 * 
|     June 6, 1988 : Module created.
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

#include	<stdio.h>
#include	<sys/time.h>

#include	"global.h"
#include	"isc.h"
#include	"isd.h"
#include	"isd_priv.h"
#include	"ipb.h"

#define DEBUG1(statement)  SERPENT_DEBUG1(ipb_debug, statement)
#define DEBUG2(statement)  SERPENT_DEBUG2(ipb_debug, statement)
#define DEBUG3(statement)  SERPENT_DEBUG3(ipb_debug, statement)

main (argc,argv)
int	argc;
char	**argv;

    {
    char		message [255],
			dialogue_name [255],
			sender [32],
			receiver [32];
    int			numcorr,
			ind = 1;
    isd_trans		transact;
    struct	timeval	tod;
    boolean		action;
    process_record	*process;

    ipb_begin_playback (argv [1], argv [2], dialogue_name, message);
    check_status ("status from begin playback");
/*
   print out message, dialogue name and mail box being simulated
   to give feedback
*/
    printf ("Playing back journal file: %s\n", argv [1]);
    printf ("Message: %s\n", message);
/*
  now add all of the correspondents
*/
    numcorr = argc - 3;
    if (!numcorr)
	ipb_add_correspondent (ALL_CORRESP);
    while (ind <= numcorr)
	{
	ipb_add_correspondent (argv [ind+3]);
	ind++;
	}
/*
  this is the main loop of the playback.  Each transaction header is
   retrieved.  
     If the sender is host and the receiver is one of the correspondents 
       then simulate.  
     If the sender is host and the receiver is not one of the correspondents 
       then skip
     If the sender is a correspondent and the receiver is host then
       map the ids and accept the next transaction from the interface

     if the sender is a correspondent and the receiver is not host then
       skip
*/
    while (get_status () == ok)
	{
	if (!ipb_get_next_transaction_header (sender, receiver, &tod)) exit();
	action = FALSE;
	if (ipb_playback_status (sender) == host &&
	    ipb_playback_status (receiver) == correspondent)
	    {
	    ipb_playback_transaction ();
	    action = TRUE;
	    }
	else if (ipb_playback_status (sender) == correspondent &&
	    ipb_playback_status (receiver) == host)
	    {
	    process = (process_record *) ipb_get_process (sender);
	    transact = isd_get_transaction_from_sender (process->mailbox);
	    ipb_map_transaction (transact);
            check_status ("playback: from ipb_map_transaction");
	    action = TRUE;
	    }
	if (!action)
	    ipb_skip_transaction ();
	}
/*
   Print out playback completed message.
*/
    printf ("Playback completed successfully.\n");
/* 
   Cleanup and return.
*/
    ipb_end_playback ();
    return;
    }
