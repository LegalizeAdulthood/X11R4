	/* $Header: /u/04c/mnt/integ/saw/include/RCS/x22.h,v 1.4 89/07/20 11:56:07 rnk Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:
|	x22.h
|
|  Description:
|	The interface to the "global" serpent_x routines.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	x22.c
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version:
|	1.0
|
|  Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|  Author:
|	Erik J. Hardy
|	erik@sei.cmu.edu
|
|  Change History:
$Log:	x22.h,v $
 * Revision 1.4  89/07/20  11:56:07  rnk
 * added declaration for x22_select_point
 * 
 * Revision 1.3  89/06/27  22:21:28  rnk
 * modified declaration of x22_widget_values_node and added declarations
 * for x22_{add,delete,get}_widget_values
 * 
 * Revision 1.2  89/06/12  11:31:36  rnk
 * added declaration for x22_values_list
 * 
 * Revision 1.1  89/04/26  17:11:48  rnk
 * Initial revision
 * 
 * Revision 1.4  89/02/17  03:20:09  ejh
 * fixed x includes
 * 
 * Revision 1.3  89/02/09  20:19:25  ejh
 * added copyright notice
 * 
 * Revision 1.2  89/01/26  16:39:03  ejh
 * took out x22_VERSION
 * 
 * Revision 1.1  88/11/14  16:49:11  ejh
 * Initial revision
 * 
|	25 Oct 88	created		erik
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

#ifndef X22_H
#define X22_H

#ifdef	x22_TOP
#define	EXTERN
#else
#define	EXTERN	extern
#endif

#ifndef	memoryPack
#define	memoryPack
#endif

#include	<X11/Intrinsic.h>
#include	"serpent.h"
#include	"isd.h"
#include	"listPack.h"

/*--------------------------------------------------------------------------*\
|  Name:
|       x22_widget_values_node
|
|  Description:
|       x22_widget_values_node describes a structure for storing widget
|       values in a linked list.
|
|  Syntax:
\*                                                                          */

typedef struct  {
        Widget  widget ;
        char    *widget_name ;
        boolean allow_user_move ;
        boolean allow_user_resize ;
        }       x22_widget_values_node ;

/*                                                                          *\
|  Components:
|       widget             the X widget id
|       widget_name        the name of the widget 
|       allow_user_move    whether this widget can be moved by the user
|       allow_user_resize  whether this widget can be resized by the user
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Constant:
|	HASH_SIZE
|
|  Description:
|	the size of hash tables
|
\*                                                                          */

#define	HASH_SIZE	257

/*                                                                          *\
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variable:
|	x22_parent_list
|
|  Description:
|	a list of all the widgets with default parents
|
\*                                                                          */

	EXTERN	LIST	x22_parent_list;

/*                                                                          *\
|
|	x22_parent_list	see above
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variable:
|	x22_values_list
|
|  Description:
|	a list of all the widgets with their defined values (currently
|	only allow_user_move and allow_user_resize are implemented)
|
\*                                                                          */

	EXTERN	LIST	x22_values_list;

/*                                                                          *\
|
|	x22_values_list	see above
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variable:
|	x22_trans
|
|  Description:
|	the name of the transaction to send back to the dm
|
\*                                                                          */

	EXTERN	transaction_type	x22_trans;

/*                                                                          *\
|
|	x22_trans	see above
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Variable:
|	x22_transaction_started
|
|  Description:
|	to tell if x22_trans has been started
|
\*                                                                          */

	EXTERN	boolean	x22_transaction_started;

/*                                                                          *\
|
|	x22_transaction_started	see above
|
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	x22_register_new_shell
|
|  Description:
|	x22_register_new_shell informs the dialogue manager of the id of a new
|	shell or widget whose window parent is the root window.
|
|  Syntax:
\*                                                                          */
        void	x22_register_new_shell (
	/*	widget:	in	Widget
	*/
	);
/*                                                                          *\
|  Parameters:
|	widget	the widget to register with the dialogue manger.
|
|  Returns:
|	nothing.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	x22_build_default_parent
|
|  Description:
|	x22_build_default_parent is called when a widget comes through without
|	a parent
|
|  Syntax:
\*                                                                          */
	Widget	x22_build_default_parent (
	/*	NONE
	*/
	);
/*                                                                          *\
|  Parameters:
|	none
|
|  Returns:
|	a default parent (topLevelShell)
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	x22_add_parent
|
|  Description:
|	x22_add_parent registers a widget-parent pair for any widget that has
|	a default parent
|
|  Syntax:
\*                                                                          */
	void	x22_add_parent	(
	/*	widget:	in	Widget,
		parent:	in	Widget
	*/
	);
/*                                                                          *\
|  Parameters:
|	widget	a widget
|	parent	the widget's parent
|
|  Returns:
|	nothing
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: x22_select_point
|
|  Description:
|       x22_select_point records the position (relative to the upper left
|       corner of the widget) at which the user clicked the left button
|       of the mouse.
|
|  Syntax:
\*                                                                          */
	void 	x22_select_point(
	/* 	w:          in	Widget, 
		event:      in	XEvent, 
		params:     in	String, 
		num_params: in  Cardinal
	*/
	);
/*                                                                          *\
|  Parameters:
|       widget          the widget which generated the resize event.
|       event           not used.
|       params          not used.
|       num_params      not used.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: x22_child_resize
|
|  Description:
|       x22_child_resize resizes child widgets within their parents, 
|       according to the position selected by the mouse.
|
|  Syntax:
\*                                                                          */
	void 	x22_child_resize(
	/* 	w:          in	Widget, 
		event:      in	XEvent, 
		params:     in	String, 
		num_params: in  Cardinal
	*/
	);
/*                                                                          *\
|  Parameters:
|       widget          the widget which generated the resize event.
|       event           not used.
|       params          not used.
|       num_params      not used.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: x22_child_move
|
|  Description:
|       x22_child_move moves child widgets within their parents, 
|       according to the position selected by the mouse.
|
|  Parameters:
|       widget          the widget which generated the move event.
|       event           not used.
|       params          not used.
|       num_params      not used.
|
|  Syntax:
\*                                                                          */
	void 	x22_child_move(
	/* 	w:          in	Widget, 
		event:      in	XEvent, 
		params:     in	String, 
		num_params: in  Cardinal
	*/
	);
/*                                                                          *\
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	x22_delete_parent
|
|  Description:
|	x22_delete_parent deletes a widget-parent pair for any widget that has
|	a default parent
|
|  Syntax:
\*                                                                          */
	void	x22_delete_parent	(
	/*	widget:	in	Widget
	*/
	);
/*                                                                          *\
|  Parameters:
|	widget	a widget
|
|  Returns:
|	Nothing.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	x22_get_widget_values
|
|  Description:
|	x22_get_widget_values returns the values_node associated with a widget
|
|  Syntax:
\*                                                                          */
	NODE x22_get_widget_values(
	/*	widget:	in	Widget
	*/
	);
/*                                                                          *\
|  Parameters:
|	widget	a widget
|
|  Returns:
|	pointer to a x22_widget_values_node
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	x22_delete_widget_values
|
|  Description:
|	x22_delete_widget_values deletes the values_node associated with a 
|	widget
|
|  Syntax:
\*                                                                          */
	void x22_delete_widget_values(
	/*	widget:	in	Widget
	*/
	);
/*                                                                          *\
|  Parameters:
|	widget	a widget
|
|  Returns:
|	Nothing
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	x22_add_widget_values
|
|  Description:
|	x22_add_widget_values adds a values_node for a given widget
|
|  Syntax:
\*                                                                          */
	NODE x22_add_widget_values(
	/*	widget:	in	Widget
	*/
	);
/*                                                                          *\
|  Parameters:
|	widget	a widget
|
|  Returns:
|	pointer to a x22_widget_values_node
\*--------------------------------------------------------------------------*/

#endif
