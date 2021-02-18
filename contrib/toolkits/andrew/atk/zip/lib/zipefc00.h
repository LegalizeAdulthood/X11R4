/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* zipefc00.h	Zip Subroutine Library Symbolic Status Values		      */
/* Author	TC Peters						      */
/* Information Technology Center		   Carnegie-Mellon University */

/*
$Log:	zipefc00.h,v $
 * Revision 2.2  89/02/08  16:49:28  ghoti
 * change copyright notice
 * 
 * Revision 2.1  88/09/27  18:12:28  ghoti
 * adjusting rcs #
 * 
 * Revision 1.2  88/09/15  17:29:52  ghoti
 * copyright fix
 * 
 * Revision 1.1  88/09/14  17:43:54  tom
 * Initial revision
 * 
 * Revision 1.1  87/10/28  21:37:40  tom
 * Initial revision
 * 
 * Revision 3.5  87/07/15  19:11:37  tom
 * Clean-ups.
 * 
 * Revision 3.4  87/03/04  14:32:39  tom
 * ===== Log of changes checked in by user tom on Wed Mar  4 14:31:11 EST 1987
 * Checked in zipefp00.h Makefile zipife04.c zipefi00.c zipefc00.h zipef002.c zipifp05.c
 * Change log:
 * 
 * (See Prose Prologs)
 * 
 * Revision 3.3  86/08/04  23:10:14  tom
 * ===== Log of changes checked in on Mon Aug  4 23:09:57 EDT 1986
 * Checked in zipefc00.h zipef002.c
 * Change log:
 * 
 * Add more status values and messages.
 * 
 * Revision 3.2  86/06/24  16:31:28  rl0t
 * ===== Log of changes checked in on Tue Jun 24 16:30:40 EDT 1986
 * Checked in zipefc00.h
 * Change log:
 * 
 * Corrected zip_highest_status_value.
 * 
 * Revision 3.1  86/02/06  02:24:46  daemon
 * new root
 * 
 * Revision 2.1  86/02/06  02:24:41  daemon
 * release root
 * 
 * Revision 1.1  85/12/31  19:30:57  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	Zip Subroutine Library Symbolic Status Values

MODULE	zipefc00.h

NOTICE	IBM Internal Use Only

DESCRIPTION
	This file is to be included in the compilation of both client-programs
	and the Zip Subroutine Library modules. It defines the symbolc "status"
	values returned by Zip Subroutine Procedures.

HISTORY
  05/01/85	Created (TCP)
  08/04/86	Add editing messages (TCP)
  02/09/87	Add zip_ok (TCP)
  02/11/87	Add Duplicate_Image_Name and Duplicate_Figure_name (TCP)

END-SPECIFICATION  ************************************************************/

#define  zip_system_status_value_boundary	1000


#define  zip_lowest_status_value		0

#define  zip_success				0
#define  zip_ok					0
#define  zip_failure				1
#define  zip_X					2
#define  zip_Y					3
#define  zip_Z					4


#define  zip_insufficient_space			5
#define  zip_insufficient_stream_space		6
#define  zip_insufficient_image_space		7
#define  zip_insufficient_figure_space		8
#define  zip_insufficient_pane_space		9

#define  zip_insufficient_X_space		10
#define  zip_pane_non_existent			11
#define  zip_stream_non_existent		12
#define  zip_pane_not_exposed			13
#define  zip_pane_not_hidden			14

#define  zip_stream_syntax_error		15
#define  zip_stream_positioning_error		16
#define  zip_inappropriate_stream_attribute	17
#define  zip_unrecognized_stream_object_attribute 18
#define  zip_unrecognized_stream_figure_type	19

#define  zip_image_non_existent			20
#define  zip_figure_non_existent		21
#define  zip_redraw_procedure_non_existent	22
#define  zip_quit_requested			23
#define  zip_redraw_requested			24

#define  zip_editing_complete			25
#define  zip_duplicate_image_name		26
#define  zip_duplicate_figure_name		27
#define  zip_image_already_hooked		28
#define  zip_image_already_unhooked		29

#define  zip_figure_already_hooked		30
#define  zip_figure_already_unhooked		31


#define  zip_highest_status_value		31

