	static char rcsid [] = "$Header: /u/04c/mnt/new_integ/saw/src/RCS/xdb.c,v 1.2 89/10/08 01:13:23 serpent Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xdb.c
|
|  Description:
|	parses argv and sets serpent debug variables
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xdb.h
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
$Log:	xdb.c,v $
 * Revision 1.2  89/10/08  01:13:23  serpent
 * was referring to old sxt debugs in error
 * 
 * Revision 1.1  89/04/26  10:38:39  rnk
 * Initial revision
 * 
 * Revision 1.2  89/02/09  21:47:57  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:02:36  ejh
 * Initial revision
 * 
|	 3 Nov 88	project standards imposed	eri
|	14 Sep 88	updated to reflect xpw	erik
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

#define	memoryPack
#define	xdb_TOP

#include	"x22.h"
#include	"string.h"
#include	"hashPack.h"
#include	"xdb.h"

typedef	struct	{
	char	*switch_name;
	int	*debug_value;
	}	xdb_hash_node;

HASH debug_switch_table;

/*--------------------------------------------------------------------------*\
|  Name: xdb_hash_string
|
|  Description:
|	xdb_hash_string is the hashing function for the Zimmerman hash table
|	package when using a string as the lookup key. It returns the sum of
|	the individual character values modulo HASH_SIZE. This function is
|	called by the Zimmerman package.
|
|  Parameters:
|	name	The string used to identify a particular nore in a hash table.
|		Converted by this function into an integer between 0 and
|		(HASH_SIZE - 1).
\*--------------------------------------------------------------------------*/

int	xdb_hash_string	(name)
char	*name;

	{
	register	int	hash_value = 0;
	register	char	*pointer;

/*	Loop through each character in the string 'name' and add the character
	value to the accumulator 'hash_value'. */

	for (pointer = name; *pointer != '\0'; pointer++)
		hash_value += *pointer;

/*	Return a value in the right range. */

	return (hash_value % HASH_SIZE);
	}

/*--------------------------------------------------------------------------*\
|  Name: xdb_match_component_name
|
|  Description:
|	xdb_match_component is called by the Zimmerman hash table package in
|	order to match queries to nodes in a table. It returns true if a match
|	occurs and false if there is not a match.
|
|  Parameters:
|	id		The string used to look up a particular node in a hash
|			table keyed by a string.
|	component	The node from the hash table whose hash value matched
|			the hash value of id and is possibly the node being
|			looked up.
\*--------------------------------------------------------------------------*/

int	xdb_match_component_name	(id, component)
char	*id;
xdb_hash_node	*component;

	{
	return (strcmp (id, component->switch_name) == 0);
	}

/*--------------------------------------------------------------------------*\
|  Name: xdb_make_hash_node
|
|  Description:
|	xdb_make_hash_node constructs a node for hashing argv switched based
|	on their three letter designator. The function allocates the node,
|	assigns values to it and returns the node.
|
|  Parameters:
|	name		The key for this node.
|	value_ptr	The value which will be stored in the table.
\*--------------------------------------------------------------------------*/

xdb_hash_node	*xdb_make_hash_node	(name, value_ptr)
char	*name;
int	*value_ptr;

	{
	xdb_hash_node *temp;

	temp = (xdb_hash_node *) make_node (sizeof (xdb_hash_node));
	temp->switch_name = name;
	temp->debug_value = value_ptr;

	return (temp);
	}

/*--------------------------------------------------------------------------*\
|  Name: xdb_make_switch_table
|
|  Description:
|	xdb_make_switch_table fills in the switch hash table with the nodes
|	necessary to match a switch name, which is a string, with the actual
|	debug variable.
|
|  Parameters
|	None.
\*--------------------------------------------------------------------------*/

void	xdb_make_switch_table	()

	{
	xdb_hash_node	*temp;

	debug_switch_table = make_hashtable (HASH_SIZE, xdb_hash_string,
		xdb_match_component_name);

/*	Add the record component names and component addresses to the table */

	temp = xdb_make_hash_node ("-isd", &(isd_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-idd", &(idd_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-iid", &(iid_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-ipc", &(ipc_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-ids", &(ids_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-ism", &(ism_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-isc", &(isc_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-ubs", &(ubs_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-x22", &(x22_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xht", &(xht_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xct", &(xct_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xcp", &(xcp_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xfw", &(xfw_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xlw", &(xlw_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xcm", &(xcm_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xbw", &(xbw_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xsb", &(xsb_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xvp", &(xvp_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xvt", &(xvt_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xdw", &(xdw_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xtw", &(xtw_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	temp = xdb_make_hash_node ("-xpw", &(xpw_debug));
	add_to_hashtable (debug_switch_table, temp, temp->switch_name);

	return;
	}

/*--------------------------------------------------------------------------*\
|  Name: xdb_parse_debug
|
|  Description:
|	xdb_parse_debug will find the switches it wants and se the appropriate
|	debug variables.
|
|  Parameters
|	argc	Length of argv.
|	argv	Command line arguments from main.
\*--------------------------------------------------------------------------*/

void	xdb_parse_debug	(argc, argv)
int	argc;
char	**argv;

	{
	int		i,
			value;
	xdb_hash_node	*temp;

	if (debug_switch_table == NULL)
		xdb_make_switch_table ();

	for (i = 0; i < argc; i++)
		{
		temp = (xdb_hash_node *) get_from_hashtable
			(debug_switch_table, argv[i]);
		if (temp != NULL)
			{
			value = atoi (argv[i+1]);
			*(temp->debug_value) = value;
			}
		}
	}
