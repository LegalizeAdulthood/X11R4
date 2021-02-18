	static char rcsid [] = "$Header: /u/04c/mnt/integ/saw/src/RCS/xlw.c,v 1.5 89/06/29 11:14:25 rnk Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xlw.c
|
|  Description:
|	The implementation of the various routines needed to interface with
|	the Label widget.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xlw.h
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
$Log:	xlw.c,v $
 * Revision 1.5  89/06/29  11:14:25  rnk
 * modified the translation table and made border width modifiable through
 * the XtResizeWidget routine
 * 
 * Revision 1.4  89/06/27  11:16:37  rnk
 * added widget name to values node and modified x22_add_widget_values calls
 * and x22_get_widget_values calls
 * 
 * Revision 1.3  89/06/13  13:12:44  rnk
 * added code to handle the "allow_user_resize" and "allow_user_move"
 * parameters
 * 
 * Revision 1.1  89/04/26  10:41:19  rnk
 * Initial revision
 * 
 * Revision 1.3  89/02/17  03:35:21  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  21:52:35  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:06:55  ejh
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
#include        <X11/Form.h>
#include	<X11/Label.h>

#include	"x22.h"
#include	"xcp.h"
#include	"xfw.h"
#include	"xlw.h"
#include	"hashPack.h"
#include	"serpent.h"

#include	"xdb.h"

extern	Display		*display ;

extern	XrmQuark	ParentQ;
extern	XrmQuark	ForegroundColorQ;
extern	XrmQuark	LabelTextQ;
extern	XrmQuark	HeightQ;
extern	XrmQuark	BorderWidthQ;
extern	XrmQuark	WidthQ;
extern	XrmQuark	FontQ;
extern	XrmQuark	XQ;
extern	XrmQuark	YQ;
extern	XrmQuark	JustifyQ;
extern	XrmQuark	InternalWidthQ;
extern	XrmQuark	InternalHeightQ;
extern	XrmQuark	AllowUserResizeQ;
extern	XrmQuark	AllowUserMoveQ;

static	char	widget_name [] = "label_widget";

static String labelTranslations =
 "<Btn2Down>,<Btn2Motion>:x22_child_resize()\n\
  <Btn2Down>,<Btn2Motion>,<Leave>,<Btn2Up>:x22_child_resize()\n\
  <Btn2Down>,<Btn2Motion>,<Btn2Up>:x22_child_resize()\n\
  <Btn3Down>,<Btn3Motion>:x22_child_move()\n\
  <Btn3Down>,<Btn3Motion>,<Leave>,<Btn3Up>:x22_child_move()\n\
  <Btn3Down>,<Btn3Motion>,<Btn3Up>:x22_child_move()" ;

/*--------------------------------------------------------------------------*\
|  Name: xlp_fill_table
|
|  Description:
|	xlp_fill_table inserts into a hash table with the nodes necessary to
|	match a label part component name, which is a string, with the actual
|	data for the component.
|
|  Parameters:
|	node		the xht_record_node in which to insert the data.
|	widget_name	the name of the widget whose table is being filled.
|
|  Returns: Modifies node.
\*--------------------------------------------------------------------------*/

void	xlp_fill_table (node, widget_name)
xht_record_node	*node;
char		*widget_name;

    {
    xht_comp_node	*temp;

    if (xlw_debug > 1)
	{
	fprintf (stderr, "xlp_fill_table: Entering.\n");
	fprintf (stderr, "xlp_fill_table: table = %x\n", node->comp_table);
	fprintf (stderr, "xlp_fill_table: widget_name = %s\n", widget_name);
	}

/*	add the record component names and component addresses to the table */

    temp = xht_make_comp_node (widget_name, "foreground_color",XtNforeground);
    ForegroundColorQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "label_text", XtNlabel);
    LabelTextQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "justify", XtNjustify);
    JustifyQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "font", XtNfont);
    FontQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "internal_width",
	XtNinternalWidth);
    InternalWidthQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "internal_height",
	XtNinternalHeight);
    InternalHeightQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "allow_user_resize", 
	"allow_user_resize") ;
    AllowUserResizeQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "allow_user_move", 
	"allow_user_move") ;
    AllowUserMoveQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    if (xlw_debug > 1)
	fprintf (stderr, "xlp_fill_table: Leaving.\n");

    return;
    }

/*--------------------------------------------------------------------------*\
|  Name: xlw_make_table
|
|  Description:
|	xlw_make_table creates a hash table with the nodes necessary to match
|	a label widget component name, which is a string, with the actual data
|	for the component.
|
|  Parameters: None.
|
|  Returns: A record suitbale for insertion into the toplevel hash table
|  for all widgets and objects.
\*--------------------------------------------------------------------------*/

xht_record_node	*xlw_make_table ()

    {
    xht_record_node	*temp;
    HASH		record_hash;

    if (xlw_debug > 1)
	fprintf (stderr, "xlw_make_table: Entering.\n");

    record_hash = make_hashtable (HASH_SIZE, xht_hash_string,
	xht_match_component_name);

    if (xlw_debug > 2)
	{
	fprintf (stderr, "xlw_make_table: record_hash = %x.\n", record_hash);
	fprintf (stderr, "xlw_make_table: Adding record table data.\n");
	}

    temp = xht_make_record_node (widget_name, (char *) record_hash,
	(String) labelWidgetClass);

    xcp_fill_table (temp, widget_name);
    xfc_fill_table (temp, widget_name);
    xlp_fill_table (temp, widget_name);

    if (xlw_debug > 1)
	{
	fprintf (stderr, "xlw_make_table: node = %x.\n", temp);
	fprintf (stderr, "xlw_make_table: Leaving.\n");
	}

    return (temp);
    }

/*--------------------------------------------------------------------------*\
|  Name: xlp_process
|
|  Description:
|	xlp_process generates the argument list values for the Label widget.
|
|  Parameters:
|	node		the node retrived for the SD component.
|	data		the element values from the SD.
|	args		the X Toolkit argument list.
|	num_args	the length of args.
|
|  Returns:
|	args is modified to reflect the new value inserted.
|	num_args is incremented.
\*--------------------------------------------------------------------------*/

Boolean	xlp_process (node, data, args, num_args)
xht_comp_node	*node;
caddr_t		data;
ArgList		args;     /* MODIFIED */
Cardinal	*num_args; /* MODIFIED */

{
    char	*data_ptr;
    Pixel	color;
    Widget	widget;
    XFontStruct *font ;

    if (xlw_debug > 1)
    {
	fprintf (stderr, "xlp_process: Entering.\n");
	fprintf (stderr, "xlp_process: node = %x.\n", node);
	fprintf (stderr, "xlp_process: args = %x.\n", args);
	fprintf (stderr, "xlp_process: num_args = %d.\n", *num_args);
    }

    data_ptr = data + node->offset;

    if (xlw_debug > 2)
    {
	fprintf (stderr, "xlp_process: Address of data = %x.\n", data_ptr);
	fprintf (stderr, "xlp_process: Attribute name = %s.\n",
	    node->xt_arg_value);
	fprintf (stderr, "xlp_process: num_args before switch is %d.\n",
	    *num_args);
    }

    if (node->quark == ForegroundColorQ)
    {
	if (xlw_debug > 2)
	    fprintf (stderr, "xlp_process: string = %s\n", data_ptr);

	color = xct_get_pixel (data_ptr);

	XtSetArg (args [*num_args], node->xt_arg_value, color);
	(*num_args)++;

	if (xlw_debug > 1)
	    fprintf (stderr, "xlp_process: num_args = %d.\n", *num_args);

	return (True);
    }

    if (node->quark == FontQ)
    {
	if (xlw_debug > 2)
	    fprintf (stderr, "xlp_process: string = %s\n", data_ptr);

        if ((font = XLoadQueryFont (display, data_ptr)) != NULL)
        {
            XtSetArg (args [*num_args], node->xt_arg_value, font);
            (*num_args)++;
        } else
            fprintf(stderr, "xlp_process: font not found--using default\n") ;

	return (True);
    }

    if (node->quark == LabelTextQ)
	{
	if (xlw_debug > 2)
	    fprintf (stderr, "xlp_process: string = %s\n", data_ptr);

	XtSetArg (args [*num_args], node->xt_arg_value, data_ptr);
	(*num_args)++;

	if (xlw_debug > 1)
	    fprintf (stderr, "xlp_process: num_args = %d.\n", *num_args);

	return (True);
	}

    if ((node->quark == JustifyQ) ||
	(node->quark == InternalWidthQ) ||
	(node->quark == InternalHeightQ))
    {
	if (xlw_debug > 2)
	    fprintf (stderr, "xlp_process: integer = %d\n",
		*((int *) data_ptr));

	XtSetArg (args [*num_args], node->xt_arg_value, *((int *) data_ptr));
	(*num_args)++;

	if (xlw_debug > 1)
	    fprintf (stderr, "xlp_process: num_args = %d.\n", *num_args);

	return (True);
    }

    return (False);
}

/*--------------------------------------------------------------------------*\
|  Name: xlw_process_changes
|
|  Description:
|	xlw_process_changes processes label widget records from the dialogue
|	manager.
|
|  Parameters:
|	change		the change type from the dialogue manager.
|	transaction	the transaction that was sent.
|	id		the id of the record in shared data to process.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/


void	xlw_process_changes (change, transaction, id)
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

    if (xlw_debug > 1)
    {
	fprintf (stderr, "xlw_process_changes: Entering.\n");
	fprintf (stderr, "xlw_process_changes: change = %d\n", change);
	fprintf (stderr, "xlw_process_changes: transaction = %d\n",
	    transaction);
	fprintf (stderr, "xlw_process_changes: id = %d\n", id);
    }

    switch (change)
    {

	case isd_create:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    if (xlw_debug > 2) fprintf (stderr,
		"xlw_process_changes: Changes incorporated.\n");

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
		else if (xlp_process (node, data, args, &num_args)) {}

		/* store the allow_user_resize and allow_user_move	*/
		/* values for this widget				*/

                data_ptr = data + node->offset;
                if (node->quark == AllowUserResizeQ) 
                    saw_resize = *((boolean *) data_ptr) ;
                if (node->quark == AllowUserMoveQ)
                    saw_move = *((boolean *) data_ptr) ;

		component_name = get_list_next (change_list, component_name);
	    }

            /* override geometry management */

            XtSetArg(args[num_args], XtNtop, XtChainTop); num_args++;
            XtSetArg(args[num_args], XtNleft, XtChainLeft); num_args++;
            XtSetArg(args[num_args], XtNbottom, XtChainTop); num_args++;
            XtSetArg(args[num_args], XtNright, XtChainLeft); num_args++;

	    if (xlw_debug > 2) fprintf(stderr, 
		"xlw_process_changes: num_args = %d\n", num_args);

	    if (parent_is_default) parent = x22_build_default_parent ();

	    widget = XtCreateManagedWidget (element_name, labelWidgetClass,
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
                XtParseTranslationTable(labelTranslations)) ;

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_modify:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    if (xlw_debug > 2)
		fprintf (stderr,
		    "xlw_process_changes: Changes incorporated.\n");

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

		if (xcp_process (node, data, args, &num_args)) {}
		else if (xfc_process (node, data, args, &num_args)) {}
		else if (xlp_process (node, data, args, &num_args)) {}

		component_name = get_list_next (change_list, component_name);
	    }

	    if (xlw_debug > 2) fprintf(stderr, 
		"xlw_process_changes: num_args = %d\n", num_args);

	    XtSetValues (widget, args, num_args);

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_remove:

	    if (widget = (Widget) xht_get_item (id))
	    {
		xht_delete_item (id);
		x22_delete_parent(widget);
		x22_delete_widget_values(widget);
	    }

	    break;

	default:

	    break;
    }

    if (xlw_debug > 1) fprintf (stderr, "xlw_process_changes: Exiting.\n");
}
