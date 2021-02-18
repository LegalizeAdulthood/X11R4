	static char rcsid [] = "$Header: /u/04c/mnt/new_integ/saw/src/RCS/xht.c,v 1.5 89/10/08 01:10:30 serpent Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xht.c
|
|  Description:
|	This is the code for creating the various hash tables needed within
|	Saw.
|
|  Component of:
|	Saw
|
|  Related files:
|	xht.h
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
$Log:	xht.c,v $
 * Revision 1.5  89/10/08  01:10:30  serpent
 * was including xpg.h in error
 * 
 * Revision 1.4  89/07/20  12:09:35  rnk
 * added quark defines for maintain_size, selected_x and selected_y
 * 
 * Revision 1.3  89/06/27  11:14:56  rnk
 * no changes.
 * 
 * Revision 1.2  89/06/07  17:39:09  rnk
 * added new quarks for allow_user_move and allow_user_resize
 * 
 * Revision 1.1  89/04/26  10:40:20  rnk
 * Initial revision
 * 
 * Revision 1.3  89/02/17  03:34:36  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  21:51:03  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:05:41  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	12 Sep 88	took out code dealing with shell widget		erik
|	14 Mar 88	changed label widget; added command buttons, lines,
|			and id to widget table	kwh
|	11 Mar 88	created		kwh
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

#include	<X11/Intrinsic.h>
#include	<X11/IntrinsicP.h>
#include	<X11/Shell.h>

#include	"x22.h"
#include	"xht.h"
#include	"xfw.h"
#include	"xlw.h"
#include	"xcm.h"
#include	"xbw.h"
#include	"xsb.h"
#include	"xvp.h"
#include	"xvt.h"
#include	"xdw.h"
#include	"xtw.h"

#include	"xdb.h"

XrmQuark	ParentQ;
XrmQuark	XQ;
XrmQuark	YQ;
XrmQuark	WidthQ;
XrmQuark	HeightQ;
XrmQuark	BorderWidthQ;
XrmQuark	BorderColorQ;
XrmQuark	BackgroundColorQ;
XrmQuark	SensitiveQ;
XrmQuark	MappedWhenManagedQ;
XrmQuark	VertDistanceQ;
XrmQuark	FromVertQ;
XrmQuark	HorizDistanceQ;
XrmQuark	FromHorizQ;
XrmQuark	ResizableQ;
XrmQuark	ForegroundColorQ;
XrmQuark	LabelTextQ;
XrmQuark	JustifyQ;
XrmQuark	InternalWidthQ;
XrmQuark	InternalHeightQ;
XrmQuark	CallbackQ;
XrmQuark	WidgetQ;
XrmQuark	VisibleQ;
XrmQuark	SensitiveQ;
XrmQuark	LineColorQ;
XrmQuark	PolygonColorQ;
XrmQuark	RectangleColorQ;
XrmQuark	ArcColorQ;
XrmQuark	TextColorQ;
XrmQuark	LineWidthQ;
XrmQuark	NumPointsQ;
XrmQuark	PointListQ;
XrmQuark	FilledQ;
XrmQuark	TextQ;
XrmQuark	BitmapFileQ;
XrmQuark	Angle1Q;
XrmQuark	Angle2Q;
XrmQuark	HSpaceQ;
XrmQuark	VSpaceQ;
XrmQuark	LengthQ;
XrmQuark	OrientationQ;
XrmQuark	ShownQ;
XrmQuark	ThicknessQ;
XrmQuark	AllowHorizQ;
XrmQuark	AllowVertQ;
XrmQuark	ForceBarsQ;
XrmQuark	UseBottomQ;
XrmQuark	UseRightQ;
XrmQuark	AllowResizeQ;
XrmQuark	MinHeightQ;
XrmQuark	MaxHeightQ;
XrmQuark	SkipAdjustQ;
XrmQuark	GripIndentQ;
XrmQuark	RefigureModeQ;
XrmQuark	ValueQ;
XrmQuark	MaxLengthQ;
XrmQuark	SendStringQ;
XrmQuark	EditTypeQ;
XrmQuark	FontQ;
XrmQuark	EditableQ;
XrmQuark	ResizeHeightQ;
XrmQuark	ResizeWidthQ;
XrmQuark	ScrollHorizontalQ;
XrmQuark	ScrollOnOverflowQ;
XrmQuark	ScrollVerticalQ;
XrmQuark	WordBreakQ;
XrmQuark	TextBufferQ;
XrmQuark	SendBufferQ;
XrmQuark	SelectedObjectQ;
XrmQuark	PositionQ;
XrmQuark        AllowUserResizeQ;
XrmQuark        AllowUserMoveQ;
XrmQuark        MaintainSizeQ;
XrmQuark        SelectedXQ;
XrmQuark        SelectedYQ;

HASH	record_types_table,
	id_to_item_table,
	item_to_id_table;

typedef struct	{
	iid_id_type	dm_id;
	caddr_t		item_id;
	}	xht_id_item_node;

/*--------------------------------------------------------------------------*\
|  Name: xht_hash_string
|
|  Description:
|	xht_hash_string is the hashing function for the Zimmerman hash table
|	package when using a string as the lookup key. It returns the sum of
|	the individual character values modulo HASH_SIZE. This function is
|	called by the Zimmerman package.
|
|  Parameters:
|	name	The string used to identify a particular node in a hash table.
|		Converted by this function into an integer between 0 and
|		(HASH_SIZE - 1).
\*--------------------------------------------------------------------------*/

int	xht_hash_string	(name)
char	*name;

    {
    register	int	hash_value = 0;
    register	char	*pointer;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_hash_string: Entering.\n");
	fprintf (stderr, "xht_hash_string: name = %s.\n", name);
	}

/*	Loop through each character in the string 'name' and add the character
	value to the accumulator 'hash_value'. */

    for (pointer = name; *pointer != '\0'; pointer++)
	hash_value += *pointer;

/*	Return a value in the right range. */

    if (xht_debug > 1)
	fprintf (stderr, "xht_hash_string: Leaving.\n");

    return (hash_value % HASH_SIZE);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_hash_id
|
|  Description:
|	xht_hash_id is the hashing function for the Zimmerman hash table
|	package when using a id as the lookup key. It returns the id value
|	modulo HASH_SIZE. This function is called by the Zimmerman package.
|
|  Parameters:
|	name	The id used to identify a particular node in a hash table.
|		Converted by this function into an integer between 0 and
|		(HASH_SIZE - 1).
\*--------------------------------------------------------------------------*/

int	xht_hash_id	(name)
iid_id_type	name;

    {
    register	int	hash_value = (int) name;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_hash_id: Entering.\n");
	fprintf (stderr, "xht_hash_id: name = %d.\n", name);
	}

/*	Return a value in the right range. */

    if (xht_debug > 1)
	fprintf (stderr, "xht_hash_id: Leaving.\n");

    return ((hash_value & 077777) % HASH_SIZE);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_hash_item
|
|  Description:
|	xht_hash_item is the hashing function for the Zimmerman hash table
|	package when using a item as the lookup key. It returns the item
|	value modulo HASH_SIZE. This function is called by the Zimmerman
|	package.
|
|  Parameters:
|	name	The item used to identify a particular node in a hash table.
|		Converted by this function into an integer between 0 and
|		(HASH_SIZE - 1).
\*--------------------------------------------------------------------------*/

int	xht_hash_item	(name)
Widget	name;

    {
    register	int	hash_value = (int) name;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_hash_item: Entering.\n");
	fprintf (stderr, "xht_hash_item: name = %x.\n", name);
	}

/*	Return a value in the right range. */

    if (xht_debug > 1)
	fprintf (stderr, "xht_hash_item: Leaving.\n");

    return (hash_value % HASH_SIZE);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_match_component_name
|
|  Description:
|	xht_match_component is called by the Zimmerman hash table package in
|	order to match queries to nodes in a table. It returns true if a match
|	occurs and false if there is not a match.
|
|  Parameters:
|	id		The string used to look up a particular node in a hash
|			table keyed by a string.
|
|	component	The node from the hash table whose hash value matched
|			the hash value of id and is possibly the node being
|			looked up.
\*--------------------------------------------------------------------------*/

int	xht_match_component_name	(id, component)
char		*id;
xht_record_node	*component;

    {
    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_match_component_name: Entering.\n");
	fprintf (stderr, "xht_match_component_name: id = %s\n", id);
	fprintf (stderr, "xht_match_component_name: component = %s\n",
	    component->name);
	fprintf (stderr, "xht_match_component_name: Leaving.\n");
	}

    return (strcmp (id, component->name) == 0);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_match_id
|
|  Description:
|	xht_match_id is called by the Zimmerman hash table package in order to
|	match queries to nodes in a table. It returns true if a match occurs
|	and false if there is not a match.
|
|  Parameters:
|	dm_id		The id value used to look up a particular node in a
|			hash table keyed by an id value.
|
|	component	The node from the hash table whose hash value matched
|			the hash value of id and is possibly the node being
|			looked up.
\*--------------------------------------------------------------------------*/

int	xht_match_id	(dm_id, component)
iid_id_type		dm_id;
xht_id_item_node	*component;

    {
    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_match_id: Entering.\n");
	fprintf (stderr, "xht_match_id: dm_id = %d\n", dm_id);
	fprintf (stderr, "xht_match_id: component = %d\n", component->dm_id);
	fprintf (stderr, "xht_match_id: Leaving.\n");
	}

    return (dm_id == component->dm_id);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_match_item
|
|  Description:
|	xht_match_item is called by the Zimmerman hash table package in order
|	to match queries to nodes in a table. It returns true if a match
|	occurs and false if there is not a match.
|
|  Parameters:
|	item_id		The item id used to look up a particular node in a
|			hash table keyed by an Xt item.
|
|	component	The node from the hash table whose hash value matched
|			the hash value of id and is possibly the node being
|			looked up.
\*--------------------------------------------------------------------------*/

int	xht_match_item	(item_id, component)
caddr_t			item_id;
xht_id_item_node	*component;

    {
    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_match_item: Entering.\n");
	fprintf (stderr, "xht_match_item: item_id = %x\n", item_id);
	fprintf (stderr, "xht_match_item: component = %x\n",
	    component->item_id);
	fprintf (stderr, "xht_match_item: Leaving.\n");
	}

    return (item_id == component->item_id);
    }

/*--------------------------------------------------------------------------*\
|  Name:
|	xht_make_record_node
|
|  Description:
|	xht_make_record_node constructs a node for hashing X Toolkit items
|	based on their shared data definition name. The function allocates the
|	node, assigns values to it, and returns the node.
|
|  Parameters:
|	name		The name of this record type.
|	comp_table	The hash table for the record components.
|	class		The record's class.
\*--------------------------------------------------------------------------*/

xht_record_node	*xht_make_record_node	(name, comp_table, class)
char	*name;
HASH	comp_table;
caddr_t	class;

    {
    xht_record_node	*temp;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_make_record_node: Entering.\n");
	fprintf (stderr, "xht_make_record_node: name = %s.\n", name);
	fprintf (stderr, "xht_make_record_node: comp_table = %x.\n",
	    comp_table);
	fprintf (stderr, "xht_make_record_node: class = %x.\n", class);
	}

    temp = (xht_record_node *) make_node (sizeof (xht_record_node));
    temp->name = name;
    temp->comp_table = comp_table;
    temp->class = class;

    if (xht_debug > 1)
	fprintf (stderr, "xht_make_record_node: Leaving.\n");

    return (temp);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_make_comp_node
|
|  Description:
|	xht_make_comp_node constructs a node for hashing X Toolkit attributes
|	based on their shared data definition name.
|
|  Parameters:
|	element		The record name.
|	name		The record component name.
|	xt_arg_value	The X Toolkit argument list identifier for the value.
\*--------------------------------------------------------------------------*/

xht_comp_node	*xht_make_comp_node	(element, name, xt_arg_value)
char	*element;
char	*name;
String	xt_arg_value;

    {
    xht_comp_node	*temp;
    int	length,
	offset,
	position;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_make_comp_node: Entering.\n");
	fprintf (stderr, "xht_make_comp_node: element = %s.\n", element);
	fprintf (stderr, "xht_make_comp_node: name = %s.\n", name);
	fprintf (stderr, "xht_make_comp_node: xt_arg_value = %s.\n",
	    xt_arg_value);
	}

    temp = (xht_comp_node *) make_node (sizeof (xht_comp_node));
    temp->name = name;
    temp->quark = XrmStringToQuark (name);

    isd_get_component_lop (element, name, &length, &offset, &position);

    temp->offset = offset;
    temp->xt_arg_value = xt_arg_value;

    if (xht_debug > 1)
	fprintf (stderr, "xht_make_comp_node: Leaving.\n");

    return (temp);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_make_hash_tables
|
|  Description:
|	xht_make_hash_tables calls the above make_*_widget_table functions and
|	places those hash tables into another hash table keyed by the item
|	record type names in the shared data definition. The result is a hash
|	table of hash tables from which the user can traverse from a record
|	type name, which is a string, to the table of record components, and
|	from there access the data contained in the components. This function
|	constructs the global HASH variable record_types_table, which is the
|	top level hash table for saw's shared data definition records.
|
|  Parameters:	None.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xht_make_hash_tables	()

    {
    xht_record_node	*temp;

    if (xht_debug > 1)
	fprintf (stderr, "xht_make_hash_tables: Entering.\n");

/*	Create the hash table for the record type. */

    record_types_table = make_hashtable (HASH_SIZE, xht_hash_string,
	xht_match_component_name);
    id_to_item_table = make_hashtable (HASH_SIZE, xht_hash_id, xht_match_id);
    item_to_id_table = make_hashtable (HASH_SIZE, xht_hash_item,
	xht_match_item);

/*	Make form widget hash table */

    temp = xfw_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

/*	Make label widget hash table */

    temp = xlw_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

/*	Make command widget hash table */

    temp = xcm_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

/*	Make box widget hash table */

    temp = xbw_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

/*	Make scrollbar widget hash table */

    temp = xsb_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

/*	Make viewport widget hash table */

    temp = xvp_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

/*	Make vertical pane widget hash table */

    temp = xvt_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

/*	Make dialog widget hash table */

    temp = xdw_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

/*	Make text widget hash table */

    temp = xtw_make_table ();
    add_to_hashtable (record_types_table, temp, temp->name);

    if (xht_debug > 1)
	fprintf (stderr, "xht_make_hash_tables: Leaving.\n");

    return;
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_get_components_table
|
|  Description:
|	xht_get_components_table returns the record component hash table for
|	the given record type. It returns the table if one exists and returns
|	NULL if one does not exist.
|
|  Parameters:
|	record_name	The record for which the user needs the component
|			table.
\*--------------------------------------------------------------------------*/

HASH	xht_get_components_table	(record_name)
char	*record_name;

    {
    xht_record_node	*temp;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_get_components_table: Entering.\n");
	fprintf (stderr, "xht_get_components_table: record_name = %s.\n",
	    record_name);
	}

    temp = (xht_record_node *) get_from_hashtable (record_types_table,
	record_name);

    if (temp == NULL)
	{
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_get_components_table:");
	    fprintf (stderr, "Error -- No table found.\n");
	    fprintf (stderr, "xht_get_components_table: Leaving.\n");
	    }
	return NULL;
	}

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_get_components_table: Table found.\n");
	fprintf (stderr, "xht_get_components_table: Leaving.\n");
	}
    return (temp->comp_table);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_get_node
|
|  Description:
|	xht_get_node returns the node pointer given a record name and a
|	component name. It returns the argument value if found, otherwise it
|	returns NULL.
|
|  Parameters:
|	record_name	The record type name.
|	component_name	The record component for which the user needs the
|			node.
\*--------------------------------------------------------------------------*/

xht_comp_node	*xht_get_node	(record_name, component_name)
char	*record_name,
	*component_name;

    {
    xht_record_node	*temp;
    xht_comp_node	*temp2;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_get_node: Entering.\n");
	fprintf (stderr, "xht_get_node: record_name= %s.\n", record_name);
	fprintf (stderr, "xht_get_node: component_name = %s.\n",
	    component_name);
	}

    temp = (xht_record_node *) get_from_hashtable (record_types_table,
	record_name);

    if (temp == NULL)
	{
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_get_node: Error -- No table found.\n");
	    fprintf (stderr, "xht_get_node: Leaving.\n");
	    }
	return NULL;
	}

    if (xht_debug > 1)
	fprintf (stderr, "xht_get_node: Table found.\n");
    temp2 = (xht_comp_node *) get_from_hashtable (temp->comp_table,
	component_name);

    if (temp2 == NULL)
	{
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_get_node: Error -- No data found.\n");
	    fprintf (stderr, "xht_get_node: Leaving.\n");
	    }
	return NULL;
	}

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_get_node: Data found.\n");
	fprintf (stderr, "xht_get_node: Leaving.\n");
	}
    return (temp2);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_item_id_mapping
|
|  Description:
|	xht_item_id_mapping inserts ids sent in from the dialogue manager with
|	the item created for that id. It updates two tables, id_to_item_table,
|	which is keyed by the id, and item_to_id_table, which is keyed by the
|	item id.
|
|  Parameters:
|	dm_id	The id passed in from the dialogue manager.
|	item_id	The Xt item created for the data identified by dm_id.
\*--------------------------------------------------------------------------*/

void	xht_item_id_mapping	(dm_id, item_id)
iid_id_type	dm_id;
caddr_t		item_id;

    {
    xht_id_item_node	*temp;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_item_id_mapping: Entering.\n");
	fprintf (stderr, "xht_item_id_mapping: dm_id = %d.\n", dm_id);
	fprintf (stderr, "xht_item_id_mapping: item_id = %x.\n", item_id);
	}

    temp = (xht_id_item_node *) make_node (sizeof (xht_id_item_node));
    temp->dm_id = dm_id;
    temp->item_id = item_id;

    add_to_hashtable (id_to_item_table, temp, temp->dm_id);

    temp = (xht_id_item_node *) make_node (sizeof (xht_id_item_node));
    temp->dm_id = dm_id;
    temp->item_id = item_id;

    add_to_hashtable (item_to_id_table, temp, temp->item_id);

    if (xht_debug > 1)
	fprintf (stderr, "xht_item_id_mapping: Leaving.\n");
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_get_item
|
|  Description:
|	xht_get_item retrieves the item value for an id from id_to_item_table.
|	If the id does not exist, it returns NULL.
|
|  Parameters:
|	dm_id	The id for which the user needs the item id.
|
|  Returns:
|	The id of the item retrieved, or NULL if not found.
\*--------------------------------------------------------------------------*/

caddr_t	xht_get_item	(dm_id)
iid_id_type	dm_id;

    {
    xht_id_item_node	*temp;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_get_item: Entering.\n");
	fprintf (stderr, "xht_get_item: dm_id = %d.\n", dm_id);
	}

    if (dm_id == iid_null_id)
	return NULL;

    temp = (xht_id_item_node *) get_from_hashtable (id_to_item_table, dm_id);

    if (temp == NULL)
	{
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_get_item: Error -- No item found.\n");
	    fprintf (stderr, "xht_get_item: Leaving.\n");
	    }
	return NULL;
	}

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_get_item: item is %x.\n", temp->item_id);
	fprintf (stderr, "xht_get_item: Leaving.\n");
	}
    return (temp->item_id);
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_delete_item
|
|  Description:
|	xht_delete_item deletes a item from the item-id translation tables and
|	from the X Toolkit.
|
|  Parameters:
|	dm_id	The id of the item to be deleted.
\*--------------------------------------------------------------------------*/

void	xht_delete_item2	(dm_id)
iid_id_type	dm_id;

    {
    xht_id_item_node	*temp1,
			*temp2;
    Widget		w;
    CompositeWidget	cw;
    WidgetList		children;
    iid_id_type		child_dm_id;
    int			i;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_delete_item2: Entering.\n");
	fprintf (stderr, "xht_delete_item2: dm_id = %d.\n", dm_id);
	}

    temp1 = (xht_id_item_node *) delete_from_hashtable (id_to_item_table,
	dm_id);

    if (temp1 == NULL)
	{
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_delete_item2: Error -- No item found.\n");
	    fprintf (stderr, "xht_delete_item2: Leaving.\n");
	    }
	return;
	}

    if (xht_debug > 1)
	fprintf (stderr, "xht_delete_item2: item is %x.\n", temp1->item_id);

    temp2 = (xht_id_item_node *) delete_from_hashtable (item_to_id_table,
	temp1->item_id);

    if (temp2 == NULL)
    {
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_delete_item2: Error -- No id found.\n");
	    fprintf (stderr, "xht_delete_item2: Leaving.\n");
	    }
	return;
    }

    w = (Widget) temp1->item_id;

    if (XtIsComposite (w))
    {
	cw = (CompositeWidget) w;
	children = cw->composite.children;

	for (i = 0; i< cw->composite.num_children; i++)
	{
	    child_dm_id = xht_get_dm_id (children [i]);
	    xht_delete_item2 (child_dm_id);
	}
    }
    XtDestroyWidget (w);

    if (xht_debug > 1)
	fprintf (stderr, "xht_delete_item2: Leaving.\n");

    free_node ((NODE) temp1);
    free_node ((NODE) temp2);

    return;
    }

void	xht_delete_item	(dm_id)
iid_id_type	dm_id;

    {
    xht_id_item_node	*temp1,
			*temp2;
    Widget		w,
			parent;
    CompositeWidget	cw;
    int			i;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_delete_item: Entering.\n");
	fprintf (stderr, "xht_delete_item: dm_id = %d.\n", dm_id);
	}

    temp1 = (xht_id_item_node *) get_from_hashtable (id_to_item_table, dm_id);

    if (temp1 == NULL)
	{
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_delete_item: Error -- No item found.\n");
	    fprintf (stderr, "xht_delete_item: Leaving.\n");
	    }
	return;
	}

    if (xht_debug > 1)
	fprintf (stderr, "xht_delete_item: item is %x.\n", temp1->item_id);

    temp2 = (xht_id_item_node *) get_from_hashtable (item_to_id_table,
	temp1->item_id);

    if (temp2 == NULL)
	{
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_delete_item: Error -- No id found.\n");
	    fprintf (stderr, "xht_delete_item: Leaving.\n");
	    }
	return;
	}

    w = (Widget) temp1->item_id;

    if (XtIsComposite (w))
    {
	cw = (CompositeWidget) w;
	parent = cw->core.parent;

	if (XtIsSubclass(parent, topLevelShellWidgetClass))
	     xht_delete_item2(parent);
	else xht_delete_item2 (dm_id);
    } else
	xht_delete_item2 (dm_id);

    if (xht_debug > 1)
	fprintf (stderr, "xht_delete_item: Leaving.\n");

    return;
    }

/*--------------------------------------------------------------------------*\
|  Name: xht_get_dm_id
|
|  Description:
|	xht_get_dm_id retrieves the dialogue manager id value for a item from
|	item_to_id_table. If the id does not exist, it returns the null id.
|
|  Parameters:
|	item_id	The item for which the user needs the dialogue manager id.
|
|  Returns:
|	The dialogue manager id for the item, or iid_null_id if it does not
|	exist.
\*--------------------------------------------------------------------------*/

iid_id_type	xht_get_dm_id	(item_id)
caddr_t	item_id;

    {
    xht_id_item_node	*temp;

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_get_dm_id: Entering.\n");
	fprintf (stderr, "xht_get_dm_id: item_id = %x.\n", item_id);
	}

    temp = (xht_id_item_node *) get_from_hashtable (item_to_id_table,item_id);

    if (temp == NULL)
	{
	if (xht_debug > 1)
	    {
	    fprintf (stderr, "xht_get_dm_id: Error -- No id found.\n");
	    fprintf (stderr, "xht_get_dm_id: Leaving.\n");
	    }
	return (iid_null_id);
	}

    if (xht_debug > 1)
	{
	fprintf (stderr, "xht_get_dm_id: dm_id is %d.\n", temp->dm_id);
	fprintf (stderr, "xht_get_dm_id: Leaving.\n");
	}
    return (temp->dm_id);
    }
