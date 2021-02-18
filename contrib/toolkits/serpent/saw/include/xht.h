	/* $Header: /u/04c/mnt/integ/sxt/include/RCS/xht.h,v 1.3 89/02/17 03:23:11 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xht.h
|
|  Description:
|	the interface to the hash tables for SerpentX.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xht.c
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
$Log:	xht.h,v $
 * Revision 1.3  89/02/17  03:23:11  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  20:27:33  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:57:14  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	25 Oct 88	removed non hash table functions	erik
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

#ifndef XHT_H
#define XHT_H

#ifndef	memoryPack
#define	memoryPack
#endif

#include	<X11/Intrinsic.h>
#include	"hashPack.h"
#include	"iid.h"

/*--------------------------------------------------------------------------*\
|  Name: 
|	xht_record_node
|
|  Description:
|	xht_record_node describes a structure for storing records in a hash
|	table.
|
|  Syntax:
\*                                                                          */

	typedef struct {
  		char	*name;
  		HASH	comp_table;
		caddr_t	class;
		}	xht_record_node;

/*                                                                          *\
|  Components:
|	name		the name of the record
|
|	comp_table	the name of the hash table it's stored in
|
|	class		what type it is
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: 
|	xht_comp_node
|
|  Description:
|	xht_record_node describes a structure for storing components in a
|	hash table.
|
|  Syntax:
\*                                                                          */

	typedef struct {
		char		*name;
		XrmQuark	quark;
		int		offset;
		String		xt_arg_value;
		}	xht_comp_node;

/*                                                                          *\
|  Components:
|	name		the name of the component
|
|	quark		its equivalent quark value
|
|	offset		where it can be found in the widget record
|
|	xt_arg_value	the current value of it
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: 
|	xht_hash_string
|
|  Description:
|	xht_hash_string is the hashing function for the Zimmerman hash table
|	package when using a string as the lookup key. It returns the sum of
|	the individual character values modulo MASX_HASH. This function is
|	called by the Zimmerman package.
|
|  Syntax:
\*                                                                          */
	int	xht_hash_string (
	/*	name:	in	*char
	*/
	);
/*                                                                          *\
|  Parameters:
|	name	the string used to identify a particular node in a hash
|		table. Converted by this function into an integer between 0
|		and (MAX_HASH - 1).
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	xht_match_component_name
|
|  Description:
|	xht_match_component is called by the Zimmerman hash table package in
|	order to match queries to nodes in a table. It returns true if a
|	match occurs and false if there is not a match.
|
|  Syntax:
\*                                                                          */
	int	xht_match_component_name (
	/*	id:		in	*char,
		component:	in	*xht_record_node
	*/
	);
/*                                                                          *\
|  Parameters:
|	id		the string used to look up a particular node in a
|			hash table keyed by a string.
|	component	the node from the hash table whose hash value matched
|			the hash value of id and is possibly the node being
|			looked up.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	xht_make_record_node
|
|  Description:
|	xht_make_record_node constructs a node for hashing X Toolkit items
|	based on their shared data definition name. The function allocates the
|	node, assigns values to it and returns the node.
|
|  Syntax:
\*                                                                          */
	xht_record_node	*xht_make_record_node (
	/*	name:		in	*char,
		comp_table:	in	HASH,
		class:		in	caddr_t
	*/
	);
/*                                                                          *\
|  Parameters:
|	name		the name of this record type.
|	comp_table	the hash table for the record components.
|	class		the record's class.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	xht_make_comp_node
|
|  Description:
|	xht_make_comp_node constructs a node for hashing X Toolkit attributes
|	based on their shared data definition name.
|
|  Syntax:
\*                                                                          */
	xht_comp_node	*xht_make_comp_node (
	/*	element:	in	*char,
		name:		in	*char,
		xt_arg_value:	in	String
	*/
	);
/*                                                                          *\
|  Parameters:
|	element		the record name.
|	name		the record component name.
|	xt_arg_value	the X Toolkit argument list identifier for the value.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	xht_make_hash_tables
|
|  Description:
|	xht_make_hash_tables calls the above *_make_table functions and places
|	those hash tables into another hash table keyed by the record type
|	names in the shared data definition. The result is a hash table of
|	hash tables from which the user can traverse from a record type name,
|	which is a string, to the table of record components, and from there
|	access the data contained in the components. This function constructs
|	the global HASH variable record_types_table, which is the top level
|	hash table for Serpent_X's shared data definition records.
|
|  Syntax:
\*                                                                          */
	void	xht_make_hash_tables (
	/*	NONE
	*/
	);
/*                                                                          *\
|
|  Parameters:
|	none.
|
|  Returns:
|	nothing.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	xht_get_node
|
|  Description:
|	xht_get_node returns the node pointer given a record name and a
|	component name.
|
|  Syntax:
\*                                                                          */
	xht_comp_node	*xht_get_node (
	/*	record_name:	in	*char,
		component_name:	in	*char
	*/
	);
/*                                                                          *\
|  Parameters:
|	record_name	the widget type name.
|	component_name	the record component for which the user needs the
|			X Toolkit argument name.
|
|  Returns:
|	returns the node if found, otherwise NULL
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: 
|	xht_item_id_mapping
|
|  Description:
|	xht_item_id_mapping inserts ids sent in from the dialogue manager with
|	the item created for that id. It updates two tables, id_to_item_table,
|	which is keyed by the id, and item_to_id_table, which is keyed by the
|	item id.
|
|  Syntax:
\*                                                                          */
	void	xht_item_id_mapping (
	/*	dm_id:		in	iid_id_type,
		item_id:	in	caddr_t
	*/
	);
/*                                                                          *\
|  Parameters:
|	dm_id	the id passed in from the dialogue manager.
|	item_id	the item created for the data identified by dm_id.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name: 
|	xht_get_item
|
|  Description:
|	xht_get_item retrieves the item value for an id from id_to_item_table.
|	If the id does not exist, it returns NULL.
|
|  Syntax:
\*                                                                          */
	caddr_t	xht_get_item (
	/*	dm_id:	in	iid_id_type
	*/
	);
/*                                                                          *\
|  Parameters:
|	dm_id	the id for which the user needs the item id.
|
|  Returns:
|	the id of the item retrieved, or NULL if not found.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	xht_delete_item
|
|  Description:
|	xht_delete_item deletes a item from the item-id translation tables and
|	from the X Toolkit.
|
|  Syntax:
\*                                                                          */
	void	xht_delete_item (
	/*	dm_id:	in	iid_id_type
	*/
	);
/*                                                                          *\
|  Parameters:
|	dm_id	the id of the item to be deleted.
|
|  Returns:
|	nothing.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*\
|  Name:
|	xht_get_dm_id
|
|  Description:
|	xht_get_dm_id retrieves the dialogue manager id value for a item from
|	item_to_id_table. If the id does not exist, it returns iid_null_id.
|
|  Syntax:
\*                                                                          */
	iid_id_type	xht_get_dm_id (
	/*	item_id:	in	caddr_t
	*/
	);
/*                                                                          *\
|  Parameters:
|	item_id	the item for which the user needs the dialogue
|		manager id.
|
|  Returns:
|	the dialogue manager id for the item, or iid_null_id if it does not
|	exist.
\*--------------------------------------------------------------------------*/

#endif
