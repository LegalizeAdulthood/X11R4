	static char rcsid [] = "$Header: /u/04c/mnt/integ/saw/src/RCS/xdw.c,v 1.2 89/06/07 17:37:32 rnk Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xdw.c
|
|  Description:
|	The implementation of the various routines needed to interface with
|	the Dialog widget
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xdw.h
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
$Log:	xdw.c,v $
 * Revision 1.2  89/06/07  17:37:32  rnk
 * set XtNtop, XtNleft, XtNbottom and XtNright to override geometry management
 * 
 * Revision 1.1  89/04/26  17:40:22  rnk
 * Initial revision
 * 
 * Revision 1.3  89/02/17  03:33:33  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  21:48:46  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:03:17  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	 2 Jun 88	created		kwh
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
#include	<X11/Dialog.h>

#include	"x22.h"
#include	"xcp.h"
#include	"xfw.h"
#include	"xdw.h"
#include	"hashPack.h"
#include	"serpent.h"

#include	"xdb.h"

extern	XrmQuark	ParentQ;
extern	XrmQuark	LabelTextQ;
extern	XrmQuark	MaxLengthQ;
extern	XrmQuark	ValueQ;
extern	XrmQuark	CallbackQ;
extern	XrmQuark	SendStringQ;

static	char	widget_name	[] = "dialog_widget";

/*--------------------------------------------------------------------------*\
|  Name: xdw_fill_table
|
|  Description:
|	xdw_fill_table inserts into a hash table with the nodes necessary to
|	match a dialog part component name, which is a string, with the actual
|	data for the component.
|
|  Parameters:
|	node		the xht_record_node in which to insert the data.
|	widget_name	the name of the widget whose table is being filled.
|
|  Returns: Modifies node.
\*--------------------------------------------------------------------------*/

void	xdw_fill_table (node, widget_name)
xht_record_node	*node;
char		*widget_name;

    {
    xht_comp_node	*temp;

    if (xdw_debug > 1)
	{
	fprintf (stderr, "xdw_fill_table: Entering.\n");
	fprintf (stderr, "xdw_fill_table: table = %x\n", node->comp_table);
	fprintf (stderr, "xdw_fill_table: widget_name = %s\n", widget_name);
	}

/*	add the record component names and component addresses to the table */

    temp = xht_make_comp_node (widget_name, "label_text", XtNlabel);
    LabelTextQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "max_length", XtNmaximumLength);
    MaxLengthQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "value", XtNvalue);
    ValueQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "method", XtNcallback);
    CallbackQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "send_string", XtNvalue);
    SendStringQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    if (xdw_debug > 1)
	fprintf (stderr, "xdw_fill_table: Leaving.\n");

    return;
    }

/*--------------------------------------------------------------------------*\
|  Name: xdw_make_table
|
|  Description:
|	xdw_make_table creates a hash table with the nodes necessary to match
|	a dialog widget component name, which is a string, with the actual
|	data for the component.
|
|  Parameters: None.
|
|  Returns: a record suitable for insertion into the toplevel hash table
|  for all widgets and objects.
\*--------------------------------------------------------------------------*/

xht_record_node	*xdw_make_table ()

    {
    xht_record_node	*temp;
    HASH		record_hash;

    if (xdw_debug > 1)
	fprintf (stderr, "xdw_make_table: Entering.\n");

    record_hash = make_hashtable (HASH_SIZE, xht_hash_string,
	xht_match_component_name);

    if (xdw_debug > 2)
	{
	fprintf (stderr, "xdw_make_table: record_hash = %x.\n", record_hash);
	fprintf (stderr, "xdw_make_table: Adding record table data.\n");
	}

    temp = xht_make_record_node (widget_name, (char *) record_hash,
	(String) dialogWidgetClass);

    xcp_fill_table (temp, widget_name);
    xfc_fill_table (temp, widget_name);
    xdw_fill_table (temp, widget_name);

    if (xdw_debug > 1)
	{
	fprintf (stderr, "xdw_make_table: node = %x.\n", temp);
	fprintf (stderr, "xdw_make_table: Leaving.\n");
	}

    return (temp);
    }

/*--------------------------------------------------------------------------*\
|  Name: xdw_process
|
|  Description:
|	xdw_process generates the argument list values for the dialog widget.
|
|  Parameters:
|	node		the node retrived for the SD component.
|	data		the element values from the SD.
|	args		the X Toolkit argument list.
|	num_args	the length of args.
|
|  Returns:
|	args is modified to reflect the new value inserted; num_args is
|	incremented.
\*--------------------------------------------------------------------------*/

Boolean	send_string = FALSE;

Boolean	xdw_process (node, data, args, num_args)
xht_comp_node	*node;
caddr_t		data;
ArgList		args;     /* MODIFIED */
Cardinal	*num_args; /* MODIFIED */

    {
    char 	*data_ptr;
    Pixel	color;
    Widget	widget;

    if (xdw_debug > 1)
	{
	fprintf (stderr, "xdw_process: Entering.\n");
	fprintf (stderr, "xdw_process: node = %x.\n", node);
	fprintf (stderr, "xdw_process: args = %x.\n", args);
	fprintf (stderr, "xdw_process: num_args = %d.\n", *num_args);
	}

    data_ptr = data + node->offset;

    if (xdw_debug > 2)
	{
	fprintf (stderr, "xdw_process: Address of data = %x.\n", data_ptr);
	fprintf (stderr, "xdw_process: Attribute name = %s.\n",
	    node->xt_arg_value);
	fprintf (stderr, "xdw_process: num_args before switch is %d.\n",
	    *num_args);
	}

    if ((node->quark == LabelTextQ) ||
	(node->quark == ValueQ))
	{
	if (xdw_debug > 2)
	    fprintf (stderr, "xdw_process: string = %s\n", data_ptr);

	XtSetArg (args [*num_args], node->xt_arg_value, data_ptr);
	(*num_args)++;

	if (xdw_debug > 1)
	    fprintf (stderr, "xdw_process: num_args = %d.\n", *num_args);

	return (True);
	}

    if (node->quark == MaxLengthQ)
	{
	if (xdw_debug > 2)
	    fprintf (stderr, "xdw_process: integer = %d\n",
		*((int *) data_ptr));

	XtSetArg (args [*num_args], node->xt_arg_value, *((int *) data_ptr));
	(*num_args)++;

	if (xdw_debug > 1)
	    fprintf (stderr, "xdw_process: num_args = %d.\n", *num_args);

	return (True);
	}

    if (node->quark == SendStringQ)
	{
	if (xdw_debug > 2)
	    fprintf (stderr, "xdw_process: send_string\n");

	if (!(*((int *) data_ptr)))
	    send_string = FALSE;
	else
	    send_string = TRUE;

	return (True);
	}

    return (False);
    }

/*--------------------------------------------------------------------------*\
|  Name: xdw_send_string
|
|  Description:
|	xdw_send_string sends a string back to the dialogue manager.
|
|  Parameters:
|      widget	The widget from which to get the string.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xdw_send_string (widget)
Widget	widget;

    {
    iid_id_type	widget_id;
    char	*value_string;

    if (xdw_debug > 1)
	{
	fprintf (stderr, "xdw_send_string: Leaving.\n");
	fprintf (stderr, "xdw_send_string: widget = %x.\n", widget);
	}

    value_string = XtDialogGetValueString (widget);

    widget_id = xht_get_dm_id (widget);

    if (xdw_debug > 2)
	{
	fprintf (stderr, "xdw_send_string: value_string = %s.\n",
	    value_string);
	fprintf (stderr, "xdw_send_string: widget_id = %d.\n", widget_id);
	fprintf (stderr, "xdw_send_string: Sending to dm.\n");
	}

    if (!x22_transaction_started)
	{
	x22_trans = isd_start_transaction();
	x22_transaction_started = TRUE;
	}

    isd_put_shared_data (x22_trans, widget_id, "dialog_widget", "method",
	"send");
    isd_put_shared_data (x22_trans, widget_id, "dialog_widget", "value",
	value_string);

    if (xdw_debug > 2)
	fprintf (stderr, "xdw_send_string: string into trans.\n");

    if (xdw_debug > 1)
	fprintf (stderr, "xdw_send_string: Leaving.\n");
    }

/*--------------------------------------------------------------------------*\
|  Name: xdw_process_changes
|
|  Description:
|	xdw_process_changes processes dialog widget records from the dialogue
|	manager.
|
|  Parameters:
|	change		the change type from the dialogue manager.
|	transaction	the transaction that was sent.
|	id		the id of the record in shared data to process.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xdw_process_changes (change, transaction, id)
isd_change_type	change;
isd_trans	transaction;
iid_id_type	id;

    {
    Widget		parent,
			widget;
    int			record_size,
			num_args;
    char		*data,
			*data_ptr,
			*value_string;
    string		element_name,
			component_name;
    LIST		change_list;
    Arg			args [50];
    xht_comp_node	*node;
    boolean		parent_is_default = FALSE;

    if (xdw_debug > 1)
	{
	fprintf (stderr, "xdw_process_changes: Entering.\n");
	fprintf (stderr, "xdw_process_changes: change = %d\n", change);
	fprintf (stderr, "xdw_process_changes: transaction = %d\n",
	    transaction);
	fprintf (stderr, "xdw_process_changes: id = %d\n", id);
	}

    switch (change)
	{

	case isd_create:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    if (xdw_debug > 2)
		fprintf (stderr,
		    "xdw_process_changes: Changes incorporated.\n");

	    change_list = isd_create_changed_component_list (transaction, id);

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
		else if (xdw_process (node, data, args, &num_args)) {}

		component_name = get_list_next (change_list, component_name);
		}

            /* override geometry management */

            XtSetArg(args[num_args], XtNtop, XtChainTop); num_args++;
            XtSetArg(args[num_args], XtNleft, XtChainLeft); num_args++;
            XtSetArg(args[num_args], XtNbottom, XtChainTop); num_args++;
            XtSetArg(args[num_args], XtNright, XtChainLeft); num_args++;

	    if (xdw_debug > 2)
		fprintf (stderr, "xdw_process_changes: num_args = %d\n",
		    num_args);

	    if (parent_is_default)
		parent = x22_build_default_parent ();

	    widget = XtCreateManagedWidget (element_name, dialogWidgetClass,
		parent, args, num_args);
	    xht_item_id_mapping (id, widget);

	    if (parent_is_default)
		x22_add_parent (widget, parent);

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_modify:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    if (xdw_debug > 2)
		fprintf (stderr,
		    "xdw_process_changes: Changes incorporated.\n");

	    change_list = isd_create_changed_component_list (transaction, id);

	    send_string = False;
	    num_args = 0;
	    component_name = get_list_next (change_list, NULL);

	    while (component_name != NULL)
		{
		node = xht_get_node (element_name, component_name);

		if (xcp_process (node, data, args, &num_args)) {}
		else if (xfc_process (node, data, args, &num_args)) {}
		else if (xdw_process (node, data, args, &num_args)) {}

		component_name = get_list_next (change_list, component_name);
		}

	    if (xdw_debug > 2)
		fprintf (stderr, "xdw_process_changes: num_args = %d\n",
		    num_args);

	    widget = (Widget) xht_get_item (id);
	    XtSetValues (widget, args, num_args);
	    if (send_string)
		xdw_send_string (widget);

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_remove:

	    if (widget = (Widget) xht_get_item (id))
		{
		xht_delete_item (id);
		x22_delete_parent (widget);
		}

	    break;

	default:

	    break;
	}

    if (xdw_debug > 1)
	fprintf (stderr, "xdw_process_changes: Exiting.\n");
    }
