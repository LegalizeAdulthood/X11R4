	static char rcsid [] = "$Header: /u/04c/mnt/integ/saddle/src/RCS/acg.c,v 1.5 89/10/18 10:24:12 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  acg.c
|
|  Description:
|	This is most of the SADDLE code generator back end, missing only the
|	ada representation clause part
|
|  Component of:
|	SADDLE processor
|
|  Related files:
|	acg.h
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version: 1.0
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
$Log:	acg.c,v $
 * Revision 1.5  89/10/18  10:24:12  ejh
 * added checks on fopen; switched over to u22_convert_string and
 * u22_copy_string
 * 
 * Revision 1.4  89/02/09  19:29:00  ejh
 * added copyright notice
 * 
 * Revision 1.3  89/01/12  01:00:18  ejh
 * added the constant string magic_number to all output files
 * 
 * Revision 1.2  89/01/11  16:16:50  ejh
 * new preamble in ill file; no path info in file names in output files
 * 
 * Revision 1.1  88/12/06  14:26:27  ejh
 * Initial revision
 * 
|	22 Nov 1988:	created; includes all of the old code generator files:
|			saddle_c.c, saddle_ada.c, saddle_ill.c
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

#define	acg_TOP
#define	memoryPack

#include	"global.h"
#include	"string.h"
#include	"listPack.h"
#include	<stdio.h>
#include	"idd.h"

#include	"u22.h"
#include	"a22.h"
#include	"acg.h"

void	acg_c_code (file_part, mailbox_name, short_name, ill_short_name,
	    magic_number)
string	file_part,
	mailbox_name,
	short_name,
	ill_short_name,
	magic_number;

    {

#define	H_EXT	".h"
#define	HSDD	"_H_SDD"

    FILE	*fopen (),
		*hfp;
    boolean	in_record = FALSE;
    string	h_file,
		define_name;
    a22_saddle_node	*this_node,
			*comment_node,
			*record_pointer,
			*id_pointer = NULL;

/*	build .h file name and open it */

    h_file = strcpy (make_node (strlen (file_part) + strlen (H_EXT) + 1),
	file_part);
    (void) strcat (h_file, H_EXT);

    DEBUG2 (fprintf (stderr, "building %s\n", h_file));

    hfp = fopen (h_file, "w");
    if (hfp == NULL)
	{
	fprintf (stderr, "couldn't open %s\n", h_file);
	exit (-1);
	}

/*	tell 'em what version of SADDLE this is */

    fprintf (hfp, "/* %s */\n\n", a22_saddle_magic_id);

/*	protect against the multiple inclusion problem */

    define_name = strcpy (make_node (strlen (short_name) + strlen (HSDD) + 1),
	u22_convert_string (short_name, UPPERCASE));
    (void) strcat (define_name, HSDD);
    fprintf (hfp, "#ifndef %s\n", define_name);
    fprintf (hfp, "#define %s\n\n", define_name);

/*	include serpent.h */

    fprintf (hfp, "#include\t\"serpent.h\"\n\n");

/*	provide pointers to the mail box and .ill file for this sdd, and the
	magic number for this run */

    fprintf (hfp, "#define MAIL_BOX \"%s\"\n", mailbox_name);
    fprintf (hfp, "#define ILL_FILE \"%s\"\n", ill_short_name);
    fprintf (hfp, "#define SERPENT_CONTROL_NUMBER \"%s\"\n\n", magic_number);

/*	print out the executable instructions */

    this_node = (a22_saddle_node *) get_list_head (a22_saddle_list);
    fprintf (hfp, "/* execution command:\t%s */\n\n", this_node -> name);

/*	starting at the second node in the list, read in the type; if not
	appropriate for C, skip it, otherwise, write out the appropriate C
	equivalent; if we get past the bottom, close the output file, then
	stop */

    while ((this_node = (a22_saddle_node *) get_list_next (a22_saddle_list,
	this_node)) != NULL)
	{

	DEBUG3 (fprintf (stderr, "acg_c_code: name: %s, type: %d\n",
	    this_node -> name, this_node -> stype));

	switch (this_node -> stype)

	    {

/*	for dd and end_dd, just skip it */

	    case RECORD:

/*	if it's a record it must be a typedef; save a pointer to this record
	so that at the end of the record, we can pick up the name; print out
	any inline comment; set a flag showing we're inside a record */

		fprintf (hfp, "typedef struct\t{");
		record_pointer = this_node;
		comment_node = (a22_saddle_node *) get_list_next
		    (a22_saddle_list, this_node);
		if (comment_node -> stype == INLINE_COMMENT)
		    {
		    fprintf (hfp, "\t/* %s */",	comment_node -> name);
		    this_node = comment_node;
		    }
		fprintf (hfp, "\n");
		in_record = TRUE;
		break;

	    case END_RECORD:

/*	pick up the name from the last record typedef; reset the flag to show
	we're not currently in a record */

		fprintf (hfp, "\t}\t%s;", record_pointer -> name);
		comment_node = (a22_saddle_node *) get_list_next
		    (a22_saddle_list, this_node);
		if (comment_node -> stype == INLINE_COMMENT)
		    {
		    fprintf (hfp, "\t/* %s */",	comment_node -> name);
		    this_node = comment_node;
		    }
		fprintf (hfp, "\n\n");
		in_record = FALSE;
		break;

	    case BOOLEAN:
	    case INTEGER:
	    case REAL:
	    case STRING:
	    case BUFFER:
	    case ID:

/*	if we're not in a record, this must be a scalar, so typedef */

		if (!in_record)
		    fprintf (hfp, "typedef");
		if (this_node -> stype == BOOLEAN)
		    fprintf (hfp, "\tboolean\t%s;", this_node -> name);
		else if (this_node -> stype == INTEGER)
		    fprintf (hfp, "\tint\t%s;", this_node -> name);
		else if (this_node -> stype == REAL)
		    fprintf (hfp, "\tdouble\t%s;", this_node -> name);
		else if (this_node -> stype == STRING)
		    fprintf (hfp, "\tchar\t%s [%d];", this_node -> name,
			this_node -> size);
		else if (this_node -> stype == BUFFER)
		    fprintf (hfp, "\tbuffer\t%s;", this_node -> name);
		else if (this_node -> stype == ID)
		    {
		    fprintf (hfp, "\tid_type\t%s;\t/* ", this_node -> name);
		    id_pointer = this_node;
		    }
		comment_node = (a22_saddle_node *) get_list_next
		    (a22_saddle_list, this_node);
		if (comment_node -> stype == INLINE_COMMENT)
		    {
		    if (this_node -> stype == ID)
			fprintf (hfp, "%s ", comment_node -> name);
		    else
			fprintf (hfp, "\t/* %s */", comment_node -> name);
		    this_node = comment_node;
		    }
		if (id_pointer != NULL)
		    {
		    if (id_pointer -> id_of == NULL)
			fprintf (hfp, "(no element pointer) */");
		    else
			fprintf (hfp, "(ID OF %s) */", id_pointer -> id_of);
		    id_pointer = NULL;
		    }
		fprintf (hfp, "\n");
		if (!in_record)
		    fprintf (hfp, "\n");
		break;

	    case COMMENT:

		fprintf (hfp, "\n/*\n");

/*	pick up all the comments we find and put them between a single pair
	of comment delimiters */

		comment_node = this_node;
		while (comment_node -> stype == COMMENT)
		    {
		    fprintf (hfp, "%s\n", comment_node -> name);
		    this_node = comment_node;
		    comment_node = (a22_saddle_node *) get_list_next
			(a22_saddle_list, comment_node);
		    }
		fprintf (hfp, "*/\n\n");
		break;

	    default:

		break;

	    }	/* end switch */
	}	/* end while */

/*	when done, print the endif, close the .h file, and print a success
	message */

    fprintf (hfp, "#endif\n");
    fclose (hfp);

    DEBUG2 (fprintf (stderr, "done building %s\n", h_file));
    return;

    }

void	acg_ada_code (file_part, mailbox_name, short_name, ill_short_name,
	    magic_number)
string	file_part,
	mailbox_name,
	short_name,
	ill_short_name,
	magic_number;

    {

#define	ADA_EXT	".a"

    FILE	*fopen (),
		*afp;
    boolean	in_record = FALSE;
    string	ada_file;
    a22_saddle_node	*this_node,
			*comment_node,
			*id_pointer = NULL,
			*record_pointer;

/*	build .a file name and open it */

    ada_file = strcpy (make_node (strlen (file_part) + strlen (ADA_EXT) + 1),
	file_part);
    (void) strcat (ada_file, ADA_EXT);

    DEBUG2 (fprintf (stderr, "building %s\n", ada_file));

    afp = fopen (ada_file, "w");
    if (afp == NULL)
	{
	fprintf (stderr, "couldn't open %s\n", ada_file);
	exit (-1);
	}

/*	tell 'em which version of SADDLE this is */

    fprintf (afp, "-- %s\n\n", a22_saddle_magic_id);

/*	make sure the serpent data package is used */

    fprintf (afp, "with serpent_type_definitions;\n");
    fprintf (afp, "use serpent_type_definitions;\n\n");

/*	print out the executable instructions */

    this_node = (a22_saddle_node *) get_list_head (a22_saddle_list);
    fprintf (afp, "-- execution command:\t%s\n\n", this_node -> name);

/*	starting at the second node in the list, read in the type; if not
	appropriate for Ada, skip it, otherwise, write out the appropriate Ada
	equivalent; if we get past the bottom, close the output file, then
	stop */

    while ((this_node = (a22_saddle_node *) get_list_next (a22_saddle_list,
	this_node)) != NULL)
	{
	switch (this_node -> stype)

	    {

/*	for end_dd, just skip it */

	    case DD:

/*	declare the package name, and provide pointers to the mail box and
	.ill file for this sdd, and the magic number for this run */

		fprintf (afp, "package %s is \n\n", this_node -> name);
		fprintf (afp, "\tMAIL_BOX: constant string := \"%s\";\n",
		    mailbox_name);
		fprintf (afp, "\tILL_FILE: constant string := \"%s\";\n",
		    ill_short_name);
		fprintf (afp, "\tSERPENT_CONTROL_NUMBER: constant string ");
		fprintf (afp, ":= \"%s\";\n\n", magic_number);
		break;

	    case RECORD:

/*	declare the record; print out any inline comment; set a flag showing
	we're inside a record */

		fprintf (afp, "\ttype %s is record", this_node -> name);
		comment_node = (a22_saddle_node *) get_list_next
		    (a22_saddle_list, this_node);
		if (comment_node -> stype == INLINE_COMMENT)
		    {
		    fprintf (afp, "\t-- %s", comment_node -> name);
		    this_node = comment_node;
		    }
		fprintf (afp, "\n");
		in_record = TRUE;
		break;

	    case END_RECORD:

/*	end the record; reset the flag to show we're not currently in a
	record */

		fprintf (afp, "\tend record;");
		comment_node = (a22_saddle_node *) get_list_next
		    (a22_saddle_list, this_node);
		if (comment_node -> stype == INLINE_COMMENT)
		    {
		    fprintf (afp, "\t-- %s", comment_node -> name);
		    this_node = comment_node;
		    }
		fprintf (afp, "\n\n");
		in_record = FALSE;
		break;

	    case BOOLEAN:
	    case INTEGER:
	    case REAL:
	    case STRING:
	    case BUFFER:
	    case ID:

/*	if we're not in a record, this must be a scalar, so type; otherwise,
	component of an element */

		if (!in_record)
		    fprintf (afp, "\tsubtype %s is ", this_node -> name);
		else
		    fprintf (afp, "\t\t%s:\t", this_node -> name);
		if (this_node -> stype == BOOLEAN)
		    fprintf (afp, "boolean;");
		else if (this_node -> stype == INTEGER)
		    fprintf (afp, "integer;");
		else if (this_node -> stype == REAL)
		    fprintf (afp, "real;");
		else if (this_node -> stype == STRING)
		    fprintf (afp, "string (1..%d);", this_node -> size);
		else if (this_node -> stype == BUFFER)
		    fprintf (afp, "buffer;");
		else if (this_node -> stype == ID)
		    {
		    fprintf (afp, "id_type;");
		    id_pointer = this_node;
		    }
		comment_node = (a22_saddle_node *) get_list_next
		    (a22_saddle_list, this_node);
		if (comment_node -> stype == INLINE_COMMENT)
		    {
		    fprintf (afp, "\t-- %s\n", comment_node -> name);
		    this_node = comment_node;
		    }
		if (id_pointer != NULL)
		    {
		    if (id_pointer -> id_of == NULL)
			fprintf (afp, "\t-- (no element pointer)\n");
		    else
			fprintf (afp, "\t-- (ID OF %s)\n",
			    id_pointer -> id_of);
		    id_pointer = NULL;
		    }
		else if (this_node != comment_node)
		    fprintf (afp, "\n");
		if (!in_record)
		    fprintf (afp, "\n");
		break;

	    case COMMENT:

/*	pick up all the comments we find and put them between a single pair
	of comment delimiters */

		fprintf (afp, "-- %s\n", this_node -> name);
		comment_node = this_node;
		while (comment_node -> stype == COMMENT)
		    {
		    this_node = comment_node;
		    fprintf (afp, "-- %s\n", comment_node -> name);
		    comment_node = (a22_saddle_node *) get_list_next
			(a22_saddle_list, comment_node);
		    }
		fprintf (afp, "\n");
		break;

	    default:

		break;

	    }	/* end switch */
	}	/* end while */

/*	when done, close the .a file, and print a success message */

    fclose (afp);
    DEBUG2 (fprintf (stderr, "done building %s\n", ada_file));
    return;

    }

void	acg_ill_code (file_part, mailbox_name, short_name, ill_short_name,
	    magic_number)
string	file_part,
	short_name,
	mailbox_name,
	ill_short_name,
	magic_number;

    {

#define	THE	"the_"
#define	DOT	"."
#define	H_EXT	".h"

    string	element_name,
		component_name,
		variable_name,
		variable_component_name,
		c_file,
		h_file,
		include_file,
		ill_file_temp1,
		ill_file_temp2,
		getenv ();
    FILE	*fopen (),
		*cfp,
		*hfp,
		*t1fp;
    idd_data_types	ill_type;
    boolean	in_record = FALSE;
    a22_saddle_node	*this_node;

/*	build the names of the files that will be added to and open them */

    c_file = u22_copy_string (getenv ("second_c"));
    cfp = fopen (c_file, "w");
    h_file = u22_copy_string (getenv ("second_h"));
    hfp = fopen (h_file, "w");
    ill_file_temp1 = u22_copy_string (getenv ("ill_1"));
    t1fp = fopen (ill_file_temp1, "w");
    ill_file_temp2 = u22_copy_string (getenv ("ill_2"));
    if (cfp == NULL || hfp == NULL || t1fp == NULL)
	{
	fprintf (stderr, "could not open an intermediate file; make sure\n");
	fprintf (stderr, "that you have write permission in this\n");
	fprintf (stderr, "directory and run saddle again\n");
	exit (-1);
	}

    DEBUG2 (fprintf (stderr, "building ill file generator\n"));

/*	the .ill file will be created by the program, and the .h file will
	be included by it */

    include_file = strcpy (make_node (strlen (file_part) + strlen (H_EXT) +
	1), file_part);
    (void) strcat (include_file, H_EXT);

/*	standard includes for a serpent program */

    fprintf (cfp, "#include\t<stdio.h>\n");
    fprintf (cfp, "#include\t\"serpent.h\"\n");
    fprintf (cfp, "#include\t\"string.h\"\n");

/*	include the SADDLE generated C declarations; include the file that
	will hold the variable declarations for each element */

    fprintf (cfp, "#include\t\"%s\"\n", include_file);
    fprintf (cfp, "#include\t\"%s\"\n\n", h_file);

/*	print the header stuff */

    fprintf (t1fp, "<<\n%s\n", a22_saddle_magic_id);
    fprintf (t1fp, "%s\n", magic_number);
    fprintf (t1fp, "%s\n", short_name);
    fprintf (t1fp, "%s\n", ill_short_name);
    fprintf (t1fp, "%s\n", mailbox_name);
    fprintf (t1fp, "%s_EXE\n", u22_convert_string (short_name, UPPERCASE));

    this_node = (a22_saddle_node *) get_list_head (a22_saddle_list);
    fprintf (t1fp, "%s\n|||\n", this_node -> name);
    fclose (t1fp);

/*	start the program; open the temp2 file; print appropriate messages */

    fprintf (cfp, "main ()\n\n\t{\n\n");
    fprintf (cfp, "\tFILE\t*ifp, *fopen ();\n\n");
    fprintf (cfp, "\tifp = fopen (\"%s\", \"w\");\n\n", ill_file_temp2);
/*
    fprintf (cfp, "\tprintf (\"building %s\\n\");\n\n", ill_file_name);
*/
    fprintf (hfp, "/* all of the record types */\n\n");

    while ((this_node = (a22_saddle_node *) get_list_next (a22_saddle_list,
	this_node)) != NULL)
	{
	component_name = u22_copy_string (this_node -> name);
	switch (this_node -> stype)

	    {
	    case RECORD:

/*	element name is the name in the saddle record; to make the variable
	name, add 'the_' to the front of the element name; when it's a record
	declaration, variable component name is equal to variable name; print
	the declaration in the .h file; show that we're in a record */

		element_name = u22_copy_string (this_node -> name);
		variable_name = strcpy (make_node (strlen (THE) + strlen
		    (element_name) + 1), THE);
		(void) strcat (variable_name, element_name);
		variable_component_name = u22_copy_string (variable_name);
		fprintf (hfp, "\t%s\t%s;\n", element_name, variable_name);
		ill_type = idd_record;
		in_record = TRUE;
		break;

	    case END_RECORD:

/*	jump out of the record */

		in_record = FALSE;
		break;

	    case BOOLEAN:
	    case INTEGER:
	    case REAL:
	    case STRING:
	    case BUFFER:
	    case ID:

/*	if we're not in a record, this is a scalar; compute element name,
	component name, and variable component name as in RECORD */

		if (!in_record)
		    {
		    element_name = u22_copy_string (component_name);
		    variable_name = strcpy (make_node (strlen (THE) + strlen
			(element_name) + 1), THE);
		    (void) strcat (variable_name, element_name);
		    variable_component_name = u22_copy_string (variable_name);
		    fprintf (hfp, "\t%s\t%s;\n", element_name, variable_name);
		    }
		else

/*	if we are in a record, append the component name onto the variable
	name (with a DOT in between) to get the variable component name */

		    {
		    variable_component_name = strcpy (make_node (strlen
			(variable_name) + strlen (DOT) + strlen
			(component_name) + 1), variable_name);
		    (void) strcat (variable_component_name, DOT);
		    (void) strcat (variable_component_name, component_name);
		    }

/*	set the type */

		if (this_node -> stype == INTEGER)
		    ill_type = idd_integer;
		else if (this_node -> stype == REAL)
		    ill_type = idd_real;
		else if (this_node -> stype == STRING)
		    ill_type = idd_string;
		else if (this_node -> stype == BUFFER)
		    ill_type = idd_buffer;
		else if (this_node -> stype == ID)
		    ill_type = idd_id;
		else if (this_node -> stype == BOOLEAN)
		    ill_type = idd_boolean;
		break;

	    default:

		break;

	    }	/* end switch */

	if (this_node -> stype == RECORD || this_node -> stype == INTEGER
	    || this_node -> stype == REAL || this_node -> stype == STRING
	    || this_node -> stype == BUFFER || this_node -> stype == ID
	    || this_node -> stype == BOOLEAN)

/*	print out the element name and component name (both in lower case);
	print out %d's for the rest of the fields in the ill file record */

	    {
	    if (this_node -> stype == RECORD)
		fprintf (cfp, "\tfprintf (ifp, \"0 \");");
	    else
		fprintf (cfp, "\tfprintf (ifp, \"1 \");");
	    fprintf (cfp, "\tfprintf (ifp, \"%s\\t%s", element_name,
		component_name);
	    fprintf (cfp, "\\t%%d\\t%%d\\t%%d\\n\",\n");

/*	this is where we compute the offset, from the address of the variable
	component name minus the address of the variable name (beginning of
	the record); if either of the names represents a string, append [0]
	to the end */

	    fprintf (cfp, "\t\t(int) &(%s", variable_component_name);
	    if (this_node -> stype == STRING)
		fprintf (cfp, " [0]");
	    fprintf (cfp, ")\n\t\t- (int) &%s", variable_name);

/*	this second string test is a little different than the last one, as
	the beginning address could be that of a scalar */

	    if (this_node -> stype == STRING && !in_record)
		fprintf (cfp, " [0]");
	    fprintf (cfp, ",\n");

/*	finally, compute the size and print it and the type */

	    fprintf (cfp, "\t\tsizeof (%s),\n", variable_component_name);
	    fprintf (cfp, "\t\t%d);\n", ill_type);
	    }
	}	/* end while */

/*	close the .ill file in main and print out a success message */

    fprintf (cfp, "\tfclose (ifp);\n\n");
    fprintf (cfp, "\treturn (0);\n\n\t}\n");

/*	close both ill generator files and print out a success message */

    fclose (hfp);
    fclose (cfp);

    DEBUG2 (fprintf (stderr, "done building ill file generator\n"));

    return;

    }
