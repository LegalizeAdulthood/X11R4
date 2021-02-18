	static char rcsid [] = "$Header: /u/04c/mnt/integ/saw/src/RCS/xcp.c,v 1.3 89/06/27 11:28:17 rnk Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xcp.c
|
|  Description:
|	The implementation of the various routines needed to interface with
|	the Core part of a widget.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xcp.h
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
$Log:	xcp.c,v $
 * Revision 1.3  89/06/27  11:28:17  rnk
 * added AllowUserMoveQ and AllowUserResizeQ and added allow_user_move and
 * allow_user_resize to xcp_fill_table
 * 
 * Revision 1.2  89/06/22  10:10:31  rnk
 * changed "sensitive" and "mappedWhenManaged" to booleans
 * 
 * Revision 1.1  89/04/26  10:37:58  rnk
 * Initial revision
 * 
 * Revision 1.3  89/02/17  03:33:06  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  21:46:22  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:01:26  ejh
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

#include	"xcp.h"
#include	"hashPack.h"
#include	"serpent.h"

#include	"xdb.h"

extern	XrmQuark	ParentQ;
extern	XrmQuark	XQ;
extern	XrmQuark	YQ;
extern	XrmQuark	WidthQ;
extern	XrmQuark	HeightQ;
extern	XrmQuark	BorderWidthQ;
extern	XrmQuark	BorderColorQ;
extern	XrmQuark	BackgroundColorQ;
extern	XrmQuark	SensitiveQ;
extern	XrmQuark	MappedWhenManagedQ;
extern  XrmQuark        AllowUserResizeQ;
extern  XrmQuark        AllowUserMoveQ;

/*--------------------------------------------------------------------------*\
|  Name: xcp_fill_table
|
|  Description:
|	xcp_fill_table inserts into a hash table with the nodes necessary to
|	match a core widget part component name, which is a string, with the
|	actual data for the component.
|
|  Parameters:
|	node		the node into which to insert the data
|	widget_name	the name of the widget whose table is being filled.
|
|  Returns: modifies node
\*--------------------------------------------------------------------------*/

void	xcp_fill_table (node, widget_name)
xht_record_node	*node;
char		*widget_name;

    {
    xht_comp_node	*temp;

    if (xcp_debug > 1)
	{
	fprintf (stderr, "xcp_fill_table: Entering.\n");
	fprintf (stderr, "xcp_fill_table: table = %x\n", node->comp_table);
	fprintf (stderr, "xcp_fill_table: widget_name = %s\n", widget_name);
	}

/*	add the record component names and component addresses to the table */

    temp = xht_make_comp_node (widget_name, "parent", "parent");
    ParentQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "x", XtNx);
    XQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "y", XtNy);
    YQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "width", XtNwidth);
    WidthQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "height", XtNheight);
    HeightQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "border_width", XtNborderWidth);
    BorderWidthQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "border_color", XtNborderColor);
    BorderColorQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "background_color",XtNbackground);
    BackgroundColorQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "sensitive", XtNsensitive);
    SensitiveQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "mapped_when_managed",
	XtNmappedWhenManaged);
    MappedWhenManagedQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "allow_user_resize",
        "allow_user_resize") ;
    AllowUserResizeQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "allow_user_move",
        "allow_user_move") ;
    AllowUserMoveQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    if (xcp_debug > 1)
	fprintf (stderr, "xcp_fill_table: Leaving.\n");

    return;
    }

/*--------------------------------------------------------------------------*\
|  Name: xcp_process
|
|  Description:
|	xcp_process generates the argument list values for the Core part of
|	any widget.
|
|  Parameters:
|	node		the node retrived for the SD component.
|	data		the element values from the SD.
|	args		the X Toolkit argument list.
|	num_args	the length of args.
|
|  Returns:
|	args is modified to reflect the new value inserted; num_args is
|	incremented; returns true if the component is used, false if not.
\*--------------------------------------------------------------------------*/

Boolean	xcp_process (node, data, args, num_args)
xht_comp_node	*node;
caddr_t		data;
ArgList		args;     /* MODIFIED */
Cardinal	*num_args; /* MODIFIED */

    {
    char	*data_ptr;
    Pixel	color;
    Widget	widget;

    if (xcp_debug > 1)
	{
	fprintf (stderr, "xcp_process: Entering.\n");
	fprintf (stderr, "xcp_process: node = %x.\n", node);
	fprintf (stderr, "xcp_process: args = %x.\n", args);
	fprintf (stderr, "xcp_process: num_args = %d.\n", *num_args);
	}

    data_ptr = data + node->offset;

    if (xcp_debug > 2)
	{
	fprintf (stderr, "xcp_process: Address of data = %x.\n", data_ptr);
	fprintf (stderr, "xcp_process: Attribute name = %s.\n",
	    node->xt_arg_value);
	fprintf (stderr, "xcp_process: num_args before switch is %d.\n",
	    *num_args);
	}

    if ((node->quark == XQ) ||
	(node->quark == YQ) ||
	(node->quark == WidthQ) ||
	(node->quark == HeightQ) ||
	(node->quark == BorderWidthQ))
	{
	if (xcp_debug > 2)
	    fprintf (stderr, "xcp_process: integer = %d\n",
		*((int *) data_ptr));

	XtSetArg (args[*num_args], node->xt_arg_value, *((int *) data_ptr));
	(*num_args)++;

	if (xcp_debug > 1)
	    fprintf (stderr, "xcp_process: num_args = %d.\n", *num_args);

	return (True);
	}

    if ((node->quark == SensitiveQ) ||
	(node->quark == MappedWhenManagedQ))
	{
	if (xcp_debug > 2)
	    fprintf (stderr, "xcp_process: boolean = %d\n",
		*((boolean *) data_ptr));

	XtSetArg (args[*num_args], node->xt_arg_value, 
	    *((boolean *) data_ptr));
	(*num_args)++;

	if (xcp_debug > 1)
	    fprintf (stderr, "xcp_process: num_args = %d.\n", *num_args);

	return (True);
	}


    if ((node->quark == BorderColorQ) ||
	(node->quark == BackgroundColorQ))
	{
	if (xcp_debug > 2)
	    fprintf (stderr, "xcp_process: string = %s\n", (data_ptr));

	color = xct_get_pixel (data_ptr);
	XtSetArg (args [*num_args], node->xt_arg_value, color);
	(*num_args)++;

	if (xcp_debug > 1)
	    fprintf (stderr, "xcp_process: num_args = %d.\n", *num_args);

	return (True);
	}

    return (False);
    }
