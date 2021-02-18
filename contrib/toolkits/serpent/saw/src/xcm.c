	static char rcsid [] = "$Header: /u/04c/mnt/integ/saw/src/RCS/xcm.c,v 1.7 89/06/29 11:40:09 rnk Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xcm.c
|
|  Description:
|	The implementation of the various routines needed to interface with
|	the Command widget.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xcm.h
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
|  Authors:
|	Kurt W. Hoyt
|	kwh@sei.cmu.edu
|
|	Erik J. Hardy
|	erik@sei.cmu.edu
|
|  Change History:
$Log:	xcm.c,v $
 * Revision 1.7  89/06/29  11:40:09  rnk
 * made border width modifiable through the XtResizeWidget routine
 * 
 * Revision 1.6  89/06/27  11:24:33  rnk
 * added widget name to values_node and modified x22_add_widget_values and
 * x22_get_widget_values calls.  Also, added x22_delete_widget_values call.
 * 
 * Revision 1.5  89/06/22  10:11:49  rnk
 * modified translation tables for new move and resize (with boundary boxes)
 * 
 * Revision 1.4  89/06/13  13:14:53  rnk
 * fixed initialization of saw_move and saw_resize
 * 
 * Revision 1.3  89/06/13  12:12:15  rnk
 * added code to handle the "allow_user_resize" and "allow_user_move"
 * parameters
 * 
 * Revision 1.2  89/06/07  17:10:16  rnk
 * set XtNtop, XtNleft, XtNbottom and XtNright to override geometry management
 * 
 * Revision 1.1  89/04/26  10:36:40  rnk
 * Initial revision
 * 
 * Revision 1.3  89/02/17  03:32:27  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  21:44:45  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  15:59:47  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	 1 Jun 88	created		kwh
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

#define memoryPack

#include	<X11/Intrinsic.h>
#include	<X11/StringDefs.h>
#include	<X11/Cardinals.h>
#include	<X11/Shell.h>
#include        <X11/Form.h>
#include	<X11/Command.h>

#include	"x22.h"
#include	"xcp.h"
#include	"xfw.h"
#include	"xlw.h"
#include	"xcm.h"
#include	"hashPack.h"
#include	"serpent.h"

#include	"xdb.h"

extern	XrmQuark	ParentQ;
extern	XrmQuark	CallbackQ;
extern	XrmQuark	XQ;
extern	XrmQuark	YQ;
extern	XrmQuark	BorderWidthQ;
extern	XrmQuark	WidthQ;
extern	XrmQuark	HeightQ;
extern  XrmQuark        AllowUserResizeQ;
extern  XrmQuark        AllowUserMoveQ;

static	char	widget_name [] = "command_widget";

static String commandTranslations =
 "<Btn2Down>,<Btn2Motion>:x22_child_resize()\n\
  <Btn2Down>,<Btn2Motion>,<Leave>,<Btn2Up>:x22_child_resize()\n\
  <Btn2Down>,<Btn2Motion>,<Btn2Up>:x22_child_resize()\n\
  <Btn3Down>,<Btn3Motion>:x22_child_move()\n\
  <Btn3Down>,<Btn3Motion>,<Leave>,<Btn3Up>:x22_child_move()\n\
  <Btn3Down>,<Btn3Motion>,<Btn3Up>:x22_child_move()\n\
  <Btn1Down>:set()\n\
  <Btn1Up>: notify()unset()\n\
  <EnterWindow>:highlight()\n\
  <LeaveWindow>:unhighlight()" ;

/*--------------------------------------------------------------------------*\
|  Name: xcm_callback
|
|  Description:
|	xcm_callback is the routine all Command widgets call when the widget
|	is selected; this routine then notifies the dialogue manager.
|
|  Parameters:
|	widget		the widget which generated the callback.
|	client_data	not used.
|	call_data	not used.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xcm_callback (widget, client_data, call_data)
Widget	widget;
caddr_t	client_data;
caddr_t	call_data;

    {
    iid_id_type		widget_id;
    iid_id_type		event_id;
    transaction_type	trans;

    if (xcm_debug > 1)
	{
	fprintf (stderr, "xcm_callback: Entering.\n");
	fprintf (stderr, "xcm_callback: widget = %x.\n", widget);
	}

    widget_id = xht_get_dm_id (widget);

    if (xcm_debug > 2)
	{
	fprintf (stderr, "xcm_callback: widget_id = %d.\n", widget_id);
	fprintf (stderr, "xcm_callback: Sending to dm.\n");
	}

    trans = isd_start_transaction();

    isd_put_shared_data (trans, widget_id, "command_widget", "method",
	"notify");

    isd_commit_transaction (trans);

    if (xcm_debug > 2)
	fprintf (stderr, "xcm_callback: Sent to dm.\n");

    if (xcm_debug > 1)
	fprintf (stderr, "xcm_callback: Leaving.\n");
    }

/*--------------------------------------------------------------------------*\
|  Name: xcm_fill_table
|
|  Description:
|	xcm_fill_table inserts into a hash table with the nodes necessary to
|	match a command part component name, which is a string, with the
|	actual data for the component.
|
|  Parameters:
|	node		the xht_record_node in which to insert the data.
|	widget_name	the name of the widget whose table is being filled.
|
|  Returns: Modifies node.
\*--------------------------------------------------------------------------*/

void	xcm_fill_table (node, widget_name)
xht_record_node	*node;
char		*widget_name;

    {
    xht_comp_node	*temp;

    if (xcm_debug > 1)
	{
	fprintf (stderr, "xcm_fill_table: Entering.\n");
	fprintf (stderr, "xcm_fill_table: table = %x\n", node->comp_table);
	fprintf (stderr, "xcm_fill_table: widget_name = %s\n", widget_name);
	}

/*	add the record component names and component addresses to the table */

    temp = xht_make_comp_node (widget_name, "method", XtNcallback);
    CallbackQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    if (xcm_debug > 1)
	fprintf (stderr, "xcm_fill_table: Leaving.\n");

    return;
    }

/*--------------------------------------------------------------------------*\
|  Name: xcm_make_table
|
|  Description:
|	xcm_make_table creates a hash table with the nodes necessary to match
|	a command widget component name, which is a string, with the actual
|	data for the component.
|
|  Parameters: None.
|
|  Returns: a record suitbale for insertion into the toplevel hash table
|  for all widgets and objects.
\*--------------------------------------------------------------------------*/

xht_record_node	*xcm_make_table ()

    {
    xht_record_node	*temp;
    HASH		record_hash;

    if (xcm_debug > 1)
	fprintf (stderr, "xcm_make_table: Entering.\n");

    record_hash = make_hashtable (HASH_SIZE, xht_hash_string,
	xht_match_component_name);

    if (xcm_debug > 2)
	{
	fprintf (stderr, "xcm_make_table: record_hash = %x.\n", record_hash);
	fprintf (stderr, "xcm_make_table: Adding record table data.\n");
	}

    temp = xht_make_record_node (widget_name, (char *) record_hash,
	(String) labelWidgetClass);

    xcp_fill_table (temp, widget_name);
    xfc_fill_table (temp, widget_name);
    xlp_fill_table (temp, widget_name);
    xcm_fill_table (temp, widget_name);

    if (xcm_debug > 1)
	{
	fprintf (stderr, "xcm_make_table: node = %x.\n", temp);
	fprintf (stderr, "xcm_make_table: Leaving.\n");
	}

    return (temp);
    }

/*--------------------------------------------------------------------------*\
|  Name: xcm_process_changes
|
|  Description:
|	xcm_process_changes processes command widget records from the dialogue
|	manager.
|
|  Parameters:
|	change		the change type from the dialogue manager.
|	transaction	the transaction that was sent.
|	id		the id of the record in shared data to process.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xcm_process_changes (change, transaction, id)
isd_change_type	change;
isd_trans	transaction;
iid_id_type	id;

    {
    Widget		parent,	widget;
    int			record_size, num_args, n ;
    char		*data, *data_ptr;
    string		element_name, component_name;
    LIST		change_list;
    Arg			args [50], local_args[50] ;
    Dimension		saw_x, saw_y, saw_bw ;
    Position		saw_w, saw_h ;
    xht_comp_node	*node;
    boolean             saw_move = FALSE;
    boolean             saw_resize = FALSE;
    boolean             parent_is_default = FALSE;
    x22_widget_values_node      *values_node ;
    static	XtCallbackRec	cmd_callback[] =
	{{xcm_callback, NULL},
	 {NULL,         NULL},};

    if (xcm_debug > 1)
	{
	fprintf (stderr, "xcm_process_changes: Entering.\n");
	fprintf (stderr, "xcm_process_changes: change = %d\n", change);
	fprintf (stderr, "xcm_process_changes: transaction = %d\n",
	    transaction);
	fprintf (stderr, "xcm_process_changes: id = %d\n", id);
	}

    switch (change)
    {

	case isd_create:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    if (xcm_debug > 2) fprintf (stderr,
		"xcm_process_changes: Changes incorporated.\n");

	    change_list = isd_create_changed_component_list(transaction, id);

	    num_args = 0;
	    component_name = get_list_next (change_list, NULL);

	    node = xht_get_node (element_name, component_name);

	    if (node->quark == ParentQ)
	    {
		data_ptr = data + node->offset;
		if (*((int *) data_ptr) == iid_null_id)
		    parent_is_default = TRUE;
		else
		    parent = (Widget) xht_get_item
			(*((iid_id_type *) data_ptr));
		component_name = get_list_next (change_list, component_name);
	    }
	    else
		parent_is_default = TRUE;

	    while (component_name != NULL)
	    {
		node = xht_get_node (element_name, component_name);

		if (xcp_process (node, data, args, &num_args)) {}
		else if (xfc_process (node, data, args, &num_args)) {}
		else if (xlp_process (node, data, args, &num_args)) {}

                data_ptr = data + node->offset;
		if (node->quark == AllowUserResizeQ) 
		    saw_resize = *((boolean *) data_ptr) ;
		if (node->quark == AllowUserMoveQ) 
		    saw_move = *((boolean *) data_ptr) ;

		component_name = get_list_next (change_list, component_name);
	    }

	    /* add the callback */

	    XtSetArg (args[num_args], XtNcallback, cmd_callback);
	    num_args++;

	    /* override geometry management */

	    XtSetArg(args[num_args], XtNtop, XtChainTop); num_args++;
	    XtSetArg(args[num_args], XtNleft, XtChainLeft); num_args++;
	    XtSetArg(args[num_args], XtNbottom, XtChainTop); num_args++;
	    XtSetArg(args[num_args], XtNright, XtChainLeft); num_args++;

	    if (xcm_debug > 2) fprintf (stderr, 
		"xcm_process_changes: num_args = %d\n", num_args);

	    if (parent_is_default) parent = x22_build_default_parent();

	    widget = XtCreateManagedWidget (element_name, commandWidgetClass,
		parent, args, num_args);
	    xht_item_id_mapping (id, widget);

	    if (parent_is_default) x22_add_parent (widget, parent);

            /* store widget name, allow_user_resize and move values */
            values_node = (x22_widget_values_node *) 
		x22_add_widget_values(widget) ;
            values_node->allow_user_resize = saw_resize ;
            values_node->allow_user_move = saw_move ;
            values_node->widget_name = widget_name ;

	    /* override the default action and translation tables.  */

            XtOverrideTranslations(widget,
		XtParseTranslationTable(commandTranslations)) ; 

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_modify:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    if (xcm_debug > 2) fprintf (stderr,
		"xcm_process_changes: Changes incorporated.\n") ;

	    change_list = isd_create_changed_component_list (transaction, id);

	    widget = (Widget) xht_get_item (id);

	    num_args = 0;
	    component_name = get_list_next (change_list, NULL);

	    while (component_name != NULL)
	    {
		node = xht_get_node (element_name, component_name);

		n = 0 ;
		XtSetArg(local_args[n], XtNx, &saw_x) ; n++;
		XtSetArg(local_args[n], XtNy, &saw_y) ; n++;
		XtSetArg(local_args[n], XtNwidth, &saw_w) ; n++;
		XtSetArg(local_args[n], XtNheight, &saw_h) ; n++;
		XtSetArg(local_args[n], XtNborderWidth, &saw_bw) ; n++ ;
		XtGetValues(widget, local_args, n) ;

		data_ptr = data + node->offset ;

		/* Special processing for move and resize */
		if (node->quark == XQ) saw_x = *((int *) data_ptr) ;
		if (node->quark == YQ) saw_y = *((int *) data_ptr) ;
		if (node->quark == WidthQ) saw_w = *((int *) data_ptr) ;
		if (node->quark == HeightQ) saw_h = *((int *) data_ptr) ;
		if (node->quark == BorderWidthQ) saw_bw = *((int *) data_ptr);

		if (node->quark == WidthQ || node->quark == BorderWidthQ || 
			node->quark == HeightQ) 
		    XtResizeWidget (widget, saw_w, saw_h, saw_bw) ;

		if (node->quark == XQ || node->quark == YQ) 
		    XtMoveWidget (widget, saw_x, saw_y) ;

                /* store the allow_user_resize and allow_user_move      */
                /* values for this widget                               */
                if (node->quark == AllowUserResizeQ)
                {
                    values_node = (x22_widget_values_node *)
			x22_get_widget_values(widget) ;
                    values_node->allow_user_resize = *((boolean *) data_ptr) ;
		}
                if (node->quark == AllowUserMoveQ)
                {
                    values_node = (x22_widget_values_node *)
			x22_get_widget_values(widget) ;
                    values_node->allow_user_move = *((boolean *) data_ptr) ;
		}

		if (xcp_process (node, data, args, &num_args)) {}
		else if (xfc_process (node, data, args, &num_args)) {}
		else if (xlp_process (node, data, args, &num_args)) {}

		component_name = get_list_next (change_list, component_name);
	    }

	    if (xcm_debug > 2)
		fprintf (stderr, "xcm_process_changes: num_args = %d\n",
		    num_args);

	    XtSetValues (widget, args, num_args);

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_remove:

	    if (widget = (Widget) xht_get_item (id))
	    {
		xht_delete_item (id);
		x22_delete_parent (widget);
                x22_delete_widget_values(widget);
	    }
	    break;

	default:

	    break;
    }

    if (xcm_debug > 1)
	fprintf (stderr, "xcm_process_changes: Exiting.\n");
    }
