	static char rcsid [] = "$Header: /u/04c/mnt/integ/saw/src/RCS/xfw.c,v 1.6 89/09/03 15:45:52 rnk Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xfw.c
|
|  Description:
|	The implementation of the various routines needed to interface with
|	the Form widget and part of the Vertical Pane widget
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xfw.h
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
$Log:	xfw.c,v $
 * Revision 1.6  89/09/03  15:45:52  rnk
 * made allow_resize and resizable boolean attributes
 * 
 * Revision 1.5  89/07/20  11:59:44  rnk
 * allowed the overriding of form geometry management through the use of
 * the maintain_size attribute (default is to override) to be under dialog
 * control.  Also added code to implement the selected_x and selected_y
 * attributes.
 * 
 * Revision 1.4  89/06/29  11:40:52  rnk
 * modified the translation table and made border width modifiable through
 * the XtResizeWidget routine
 * 
 * Revision 1.3  89/06/27  22:28:37  rnk
 * added widget name to values_node and modified x22_add_widget_values and
 * x22_get_widget_values calls to accept a returned pointer to the values_node
 * 
 * Revision 1.2  89/06/07  17:38:11  rnk
 * set XtNtop, XtNleft, XtNbottom and XtNright to override geometry management
 * 
 * Revision 1.1  89/04/26  10:39:21  rnk
 * Initial revision
 * 
 * Revision 1.3  89/02/17  03:33:52  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  21:49:30  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:03:59  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	30 May 88	created		kwh
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
#include	<X11/Form.h>
#include	<X11/Label.h>
#include	<X11/VPaned.h>

#include	"x22.h"
#include	"xcp.h"
#include	"xfw.h"
#include	"hashPack.h"
#include	"serpent.h"

#include	"xdb.h"
	
extern	XrmQuark	ParentQ;
extern	XrmQuark	BackgroundColorQ;
extern	XrmQuark	XQ;
extern	XrmQuark	YQ;
extern	XrmQuark	BorderWidthQ;
extern	XrmQuark	WidthQ;
extern	XrmQuark	HeightQ;
extern	XrmQuark	VertDistanceQ;
extern	XrmQuark	FromVertQ;
extern	XrmQuark	HorizDistanceQ;
extern	XrmQuark	FromHorizQ;
extern	XrmQuark	ResizableQ;
extern	XrmQuark	AllowResizeQ;
extern	XrmQuark	MinHeightQ;
extern	XrmQuark	MaxHeightQ;
extern	XrmQuark	SkipAdjustQ;
extern  XrmQuark        AllowUserResizeQ;
extern  XrmQuark        AllowUserMoveQ;
extern  XrmQuark        MaintainSizeQ;
extern  XrmQuark        SelectedXQ;
extern  XrmQuark        SelectedYQ;

static	char	widget_name [] = "form_widget";

static String formTranslations =
 "<Btn1Down>,<Btn1Up>:x22_select_point()\n\
  <Btn2Down>,<Btn2Motion>:x22_child_resize()\n\
  <Btn2Down>,<Btn2Motion>,<Leave>,<Btn2Up>:x22_child_resize()\n\
  <Btn2Down>,<Btn2Motion>,<Btn2Up>:x22_child_resize()\n\
  <Btn3Down>,<Btn3Motion>:x22_child_move()\n\
  <Btn3Down>,<Btn3Motion>,<Leave>,<Btn3Up>:x22_child_move()\n\
  <Btn3Down>,<Btn3Motion>,<Btn3Up>:x22_child_move()" ;

/*--------------------------------------------------------------------------*\
|  Name: xfc_fill_table
|
|  Description:
|	xfc_fill_table inserts into a hash table with the nodes necessary to
|	match a form widget constraint part component name or a vertical pane
|	widget constraint part component, which is a string, with the actual
|	data for the component.
|
|  Parameters:
|	node		the node into which to insert the data
|	widget_name	the name of the widget whose table is being filled.
|
|  Returns: Modifies table.
\*--------------------------------------------------------------------------*/

void	xfc_fill_table (node, widget_name)
xht_record_node	*node;
char		*widget_name;

{
    xht_comp_node	*temp;

    if (xfw_debug > 1)
    {
	fprintf (stderr, "xfc_fill_table: Entering.\n");
	fprintf (stderr, "xfc_fill_table: table = %x\n", node->comp_table);
	fprintf (stderr, "xfc_fill_table: widget_name = %s\n", widget_name);
    }

    /* add the record component names and component addresses to the table */

    temp = xht_make_comp_node (widget_name, "vert_distance", XtNvertDistance);
    VertDistanceQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "from_vert", XtNfromVert);
    FromVertQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "horiz_distance",
	XtNhorizDistance);
    HorizDistanceQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "from_horiz", XtNfromHoriz);
    FromHorizQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "resizable", XtNresizable);
    ResizableQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "allow_resize", XtNallowResize);
    AllowResizeQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "max_height", XtNmax);
    MaxHeightQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "min_height", XtNmin);
    MinHeightQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "skip_adjust", XtNskipAdjust);
    SkipAdjustQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "maintain_size", "maintain_size");
    MaintainSizeQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "selected_x", "selected_x");
    SelectedXQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name); 

    temp = xht_make_comp_node (widget_name, "selected_y", "selected_y");
    SelectedYQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    if (xfw_debug > 1) fprintf (stderr, "xfc_fill_table: Leaving.\n");
    return;
}

/*--------------------------------------------------------------------------*\
|  Name: xfw_make_table
|
|  Description:
|	xfw_make_table creates a hash table with the nodes necessary to match
|	a form widget component name, which is a string, with the actual data
|	for the component.
|
|  Parameters: None.
|
|  Returns: a record suitbale for insertion into the toplevel hash table
|  for all widgets and objects.
\*--------------------------------------------------------------------------*/

xht_record_node	*xfw_make_table ()

    {
    xht_record_node	*temp;
    HASH		record_hash;

    if (xfw_debug > 1)
	fprintf (stderr, "xfw_make_table: Entering.\n");

    record_hash = make_hashtable (HASH_SIZE, xht_hash_string,
	xht_match_component_name);

    if (xfw_debug > 2)
	{
	fprintf (stderr, "xfw_make_table: record_hash = %x.\n", record_hash);
	fprintf (stderr, "xfw_make_table: Adding record table data.\n");
	}

    temp = xht_make_record_node (widget_name, (char *) record_hash,
	(String) formWidgetClass);

    xcp_fill_table (temp, widget_name);
    xfc_fill_table (temp, widget_name);

    if (xfw_debug > 2)
	{
	fprintf (stderr, "xfw_make_table: node = %x.\n", temp);
	fprintf (stderr, "xfw_make_table: Leaving.\n");
	}

    return (temp);
    }

/*--------------------------------------------------------------------------*\
|  Name: xfc_process
|
|  Description:
|	xfc_process generates the argument list values for the form or vpane
|	constraints for any widget which is part of a form or vertical pane
|	widget.
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

Boolean	xfc_process (node, data, args, num_args)
xht_comp_node	*node;
caddr_t		data;
ArgList		args;     /* MODIFIED */
Cardinal	*num_args; /* MODIFIED */

{
    char	*data_ptr;
    Pixel	color;
    Widget	widget;

    if (xfw_debug > 1)
    {
	fprintf (stderr, "xfc_process: Entering.\n");
	fprintf (stderr, "xfc_process: node = %x.\n", node);
	fprintf (stderr, "xfc_process: args = %x.\n", args);
	fprintf (stderr, "xfc_process: num_args = %d.\n", *num_args);
    }

    data_ptr = data + node->offset;

    if (xfw_debug > 2)
    {
	fprintf (stderr, "xfc_process: Address of data = %x.\n", data_ptr);
	fprintf (stderr, "xfc_process: Attribute name = %s.\n",
	    node->xt_arg_value);
	fprintf (stderr, "xfc_process: num_args before switch is %d.\n",
	    *num_args);
    }

    if ((node->quark == FromVertQ) ||
	(node->quark == FromHorizQ))
    {
	widget = (Widget) xht_get_item (*((iid_id_type *) data_ptr));

	if (xfw_debug > 2)
	{
	    fprintf (stderr, "xfc_process: id = %d\n",
		*((iid_id_type *) data_ptr));
	    fprintf (stderr, "xfc_process: widget = %x\n", widget);
	}

	XtSetArg (args [*num_args], node->xt_arg_value, widget);
	(*num_args)++;

	if (xfw_debug > 1)
	    fprintf (stderr, "xfc_process: num_args = %d.\n", *num_args);

	return (True);
    }

    if ((node->quark == VertDistanceQ) ||
	(node->quark == HorizDistanceQ) ||
	(node->quark == ResizableQ) ||
	(node->quark == MaxHeightQ) ||
	(node->quark == MinHeightQ) ||
	(node->quark == SkipAdjustQ))
    {
	if (xfw_debug > 2) fprintf (stderr, 
		"xfc_process: integer = %d\n", *((int *) data_ptr));

	XtSetArg (args [*num_args], node->xt_arg_value, *((int *) data_ptr));
	(*num_args)++;

	if (xfw_debug > 1)
	    fprintf (stderr, "xfc_process: num_args = %d.\n", *num_args);

	return (True);
    }

    if ((node->quark == AllowResizeQ) ||
	(node->quark == ResizableQ))
    {
	if (xfw_debug > 2) fprintf (stderr, 
		"xfc_process: boolean = %d\n", *((boolean *) data_ptr));

	XtSetArg (args [*num_args], node->xt_arg_value, 
		*((boolean *) data_ptr)); 
	(*num_args)++;

	if (xfw_debug > 1)
	    fprintf (stderr, "xfc_process: num_args = %d.\n", *num_args);

	return (True);
    }

    return (False);
}

/*--------------------------------------------------------------------------*\
|  Name: xfw_process_changes
|
|  Description:
|	xfw_process_changes processes form widget records from the dialogue
|	manager.
|
|  Parameters:
|	change		the change type from the dialogue manager.
|	transaction	the transaction that was sent.
|	id		the id of the record in shared data to process.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xfw_process_changes (change, transaction, id)
isd_change_type	change;
isd_trans	transaction;
iid_id_type	id;

{
    Widget		parent,	anchor_widget, widget ;
    Pixel		color = 0 ;
    Dimension		saw_x, saw_y, saw_bw ;
    Position		saw_w, saw_h ;
    int			record_size, num_args, n ;
    char		*data, *data_ptr ;
    string		element_name, component_name ;
    LIST		change_list ;
    Arg			args [50], local_args[50] ;
    xht_comp_node	*node ;
    boolean             maintain_size = TRUE ;
    boolean             saw_move = FALSE ;
    boolean             saw_resize = FALSE ;
    boolean		parent_is_default = FALSE ;
    boolean		geom_request = FALSE ;
    x22_widget_values_node	*values_node ;

    if (xfw_debug > 1)
    {
	fprintf (stderr, "xfw_process_changes: Entering.\n");
	fprintf (stderr, "xfw_process_changes: change = %d\n", change);
	fprintf (stderr, "xfw_process_changes: transaction = %d\n",
	    transaction);
	fprintf (stderr, "xfw_process_changes: id = %d\n", id);
    }

    switch (change)
    {

	case isd_create:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    if (xfw_debug > 2) fprintf (stderr,
		    "xfw_process_changes: Changes incorporated.\n");

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
	    } else
		parent_is_default = TRUE;

	    while (component_name != NULL)
	    {
		node = xht_get_node (element_name, component_name);

		data_ptr = data + node->offset ;
		if (node->quark == WidthQ) saw_x = *((int *) data_ptr) ;
		if (node->quark == HeightQ) saw_y = *((int *) data_ptr) ;
		if (node->quark == BackgroundColorQ) 
		    color = xct_get_pixel (data_ptr);
                if (node->quark == AllowUserResizeQ)
                    saw_resize = *((boolean *) data_ptr) ;
                if (node->quark == AllowUserMoveQ)
                    saw_move = *((boolean *) data_ptr) ;
                if (node->quark == MaintainSizeQ)
                    maintain_size = *((boolean *) data_ptr) ;

		if (xcp_process (node, data, args, &num_args)) {}
		else if (xfc_process (node, data, args, &num_args)) {}

		component_name = get_list_next (change_list, component_name);
	    }

	    if (maintain_size)
	    {
		/* override geometry management */
		XtSetArg(args[num_args], XtNtop, XtChainTop); num_args++;
		XtSetArg(args[num_args], XtNleft, XtChainLeft); num_args++;
		XtSetArg(args[num_args], XtNbottom, XtChainTop); num_args++;
		XtSetArg(args[num_args], XtNright, XtChainLeft); num_args++;
	    }

            /* add translation tables.  */
	    XtSetArg(args[num_args], XtNtranslations, 
                XtParseTranslationTable(formTranslations)) ; num_args++;

	    if (xfw_debug > 2) fprintf (stderr, 
		"xfw_process_changes: num_args = %d\n", num_args);

	    if (parent_is_default) parent = x22_build_default_parent ();

	    widget = XtCreateManagedWidget ("", formWidgetClass,
		parent, args, num_args);
	    xht_item_id_mapping (id, widget);

	    /* Set up the argument list for and create an invisible 	*/
	    /* label in the bottom right hand corner to ensure that 	*/
	    /* the form doesn't shrink-wrap its children.		*/
	    if (maintain_size)
	    {
		num_args = 0 ;
		XtSetArg(args[num_args], XtNhorizDistance, (XtArgVal) saw_x); 
		num_args++;
		XtSetArg(args[num_args], XtNvertDistance, (XtArgVal) saw_y);
		num_args++;
		XtSetArg(args[num_args], XtNwidth, (XtArgVal) 1) ; num_args++;
		XtSetArg(args[num_args], XtNheight, (XtArgVal) 1); num_args++;
		XtSetArg(args[num_args], XtNborderWidth, (XtArgVal) 0) ; 
		num_args++;
		if (color) 
		{
		    XtSetArg(args[num_args], XtNbackground, (XtArgVal)color);
		    num_args++;
		}
		anchor_widget = XtCreateManagedWidget("", labelWidgetClass,
                        widget, args, num_args);
	    }

	    if (parent_is_default) x22_add_parent (widget, parent);

            /* store widget name, allow_user_resize and move values */
            values_node = (x22_widget_values_node *)
		x22_add_widget_values(widget) ;
	    values_node->allow_user_resize = saw_resize ;
	    values_node->allow_user_move = saw_move ;
	    values_node->widget_name = widget_name ;

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_modify:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    if (xfw_debug > 2) fprintf (stderr,
		    "xfw_process_changes: Changes incorporated.\n");

	    change_list = isd_create_changed_component_list (transaction, id);

	    num_args = 0;
	    component_name = get_list_next (change_list, NULL);

	    widget = (Widget) xht_get_item (id);

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

                /* store the allow_user_resize and allow_user_move	*/
                /* values for this widget				*/
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

		if (node->quark == VertDistanceQ || 
			node->quark == HorizDistanceQ) 
		    geom_request = TRUE ;

		if (xcp_process (node, data, args, &num_args)) {}
		else if (xfc_process (node, data, args, &num_args)) {}

		component_name = get_list_next (change_list, component_name);
	    }

	    if (xfw_debug > 2)
		fprintf (stderr, "xfw_process_changes: num_args = %d\n",
		    num_args);

	    XtSetValues (widget, args, num_args);

/***	    if (geom_request) 
	    {
		XtUnrealizeWidget(widget) ;
		XtRealizeWidget(widget) ;
	    } 
***/
	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_remove:

	    if (widget = (Widget) xht_get_item(id))
	    {
		xht_delete_item(id);
		x22_delete_parent(widget);
                x22_delete_widget_values(widget);
	    }

	    break;

	default:

	    break;
}

    if (xfw_debug > 1)
	fprintf (stderr, "xfw_process_changes: Exiting.\n");
}
