	static char rcsid [] = "$Header: /u/04c/mnt/integ/saddle/src/RCS/aap.c,v 1.9 89/10/26 16:06:34 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  aap.c
|
|  Description:
|	The analysis portion of SADDLE
|
|  Component of:
|	SADDLE processor
|
|  Related files:
|	aap.h
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
$Log:	aap.c,v $
 * Revision 1.9  89/10/26  16:06:34  ejh
 * changed 'em back to stderr
 * 
 * Revision 1.8  89/10/21  16:50:49  ejh
 * changed regular messages to stdout
 * 
 * Revision 1.7  89/10/18  10:25:47  ejh
 * added checks on fopen; switched over to u22_convert_string and
 * u22_copy_string
 * 
 * Revision 1.6  89/02/09  19:24:23  ejh
 * added copyright notice
 * 
 * Revision 1.5  89/01/12  00:58:11  ejh
 * added magic_number to calling sequence of all code generators
 * 
 * Revision 1.4  89/01/11  15:00:32  ejh
 * added checking for bad exec string; restricted input file name to *.sdd; no
 * longer use full path names for file names in ill file
 * 
 * Revision 1.3  88/12/21  13:10:54  ejh
 * fixed c preprocessor command to run on sun
 * 
 * Revision 1.2  88/12/16  15:25:39  ejh
 * fixed string length for file names with a .ext;
 * removed wait () call, which doesn't work with Saber
 * 
 * Revision 1.1  88/12/06  14:25:01  ejh
 * Initial revision
 * 
|	16 Nov 1988:	created
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

#define	aap_TOP
#define	a22_TOP
#define	memoryPack

#include	"global.h"
#include	"string.h"
#include	"listPack.h"
#include	<stdio.h>
#include	<sys/wait.h>

#include	"u22.h"
#include	"a22.h"
#include	"aap.h"
#include	"acg.h"

a22_saddle_node	*aap_make_node (the_name)
string	the_name;

    {
    a22_saddle_node	*the_node;

    the_node = (a22_saddle_node *) make_node (sizeof (a22_saddle_node));
    the_node -> name = the_name;
    the_node -> size = 0;
    the_node -> id_of = NULL;
    return (the_node);
    }

void	aap_add_node (the_type, the_node)
a22_saddle_type	the_type;
a22_saddle_node	*the_node;

    {
    static	int		record_size;
    static	a22_saddle_node	*record_pointer;

    DEBUG2 (fprintf (stderr, "aap_add_node: entering\n"));
    DEBUG3 (fprintf (stderr, "aap_add_node: adding a type of %d\n",
	the_type));

    the_node -> stype = the_type;

    switch (the_type)

	{

	case RECORD:

	    record_pointer = the_node;
	    record_size = 0;
	    aap_check_name (the_node);
	    break;

	case END_RECORD:

	    record_pointer -> size = record_size;
	    aap_check_name (the_node);
	    break;

	case BOOLEAN:
	case INTEGER:
	case REAL:
	case STRING:
	case BUFFER:
	case ID:

	    record_size = record_size + the_node -> size;
	    aap_check_name (the_node);
	    break;

	case DD:

	    fprintf (stdout, "dd_name=%s\n", the_node -> name);
	    fprintf (stdout, "export dd_name\n");
	    break;

	default:

	    break;

	}

    (void) add_to_tail (a22_saddle_list, the_node);

    DEBUG2 (fprintf (stderr, "aap_add_node: leaving\n"));

    }

void	aap_check_name (the_node)
a22_saddle_node	*the_node;

    {
    static	boolean	in_record = FALSE;
    a22_saddle_node	*this_node;

    DEBUG2 (fprintf (stderr, "aap_check_name: entering\n"));
    DEBUG3 (fprintf (stderr, "aap_check_name: name is: %s\n",
	the_node -> name ));

    switch (the_node -> stype)

	{

	case RECORD:

	    in_record = TRUE;
	    this_node = (a22_saddle_node *) get_list_head (aap_element_list);
	    while (this_node != NULL)
		{
		if (!strcmp (this_node -> name, the_node -> name))
		    aap_error (the_node -> name, NAME_ERROR, RECORD);
		this_node = (a22_saddle_node *) get_list_next
		    (aap_element_list, this_node);
		}
	    (void) add_to_tail (aap_element_list, the_node);
	    break;

	case END_RECORD:

	    in_record = FALSE;
	    aap_component_list = make_list ();
	    break;

	default:

	    if (in_record)
		{
		this_node = (a22_saddle_node *) get_list_head
		    (aap_component_list);
		while (this_node != NULL)
		    {
		    if (!strcmp (this_node -> name, the_node -> name))
			aap_error (the_node -> name, NAME_ERROR,
			    the_node -> stype);
		    this_node = (a22_saddle_node *) get_list_next
			(aap_component_list, this_node);
		    }
		(void) add_to_tail (aap_component_list, the_node);
		}
	    else
		{
		this_node = (a22_saddle_node *) get_list_head
		    (aap_element_list);
		while (this_node != NULL)
		    {
		    if (!strcmp (this_node -> name, the_node -> name))
			aap_error (the_node -> name, NAME_ERROR, RECORD);
		    this_node = (a22_saddle_node *) get_list_next
			(aap_element_list, this_node);
		    }
		(void) add_to_tail (aap_element_list, the_node);
		}
	    break;

	}

    DEBUG2 (fprintf (stderr, "aap_check_name: leaving\n"));

    }

void	aap_error (the_name, the_error, the_type)
string		the_name;
int		the_error;
a22_saddle_type	the_type;

    {

    DEBUG2 (fprintf (stderr, "aap_error: entering\n"));
    DEBUG3 (fprintf (stderr, "aap_error: error number %d on %s", the_error,
	the_name));
    DEBUG3 (if (the_error != YACC_ERROR)
		fprintf (stderr, "with type %d\n", the_type);
	    else
		fprintf (stderr, "\n");)

    switch (the_error)

	{

	case YACC_ERROR:

	    fprintf (stderr, "YACC detected a fatal error: %s\n", the_name);
	    fprintf (stderr, "We might be crashing real soon\n");
	    break;

	case NAME_ERROR:

	    fprintf (stderr, "FATAL ERROR: the name %s has already been used",
		the_name);
	    if (the_type == RECORD)
		fprintf (stderr, " as an element\n");
	    else
		fprintf (stderr, " as a component in this record\n");
	    break;

	case FATAL_ERROR:

	    fprintf (stderr, "SADDLE detected a fatal error; it's probably");
	    fprintf (stderr, " an illegal character\n");
	    break;

	case EXEC_ERROR:

	    fprintf (stderr, "illegal execution string: %s\n", the_name);
	    break;

	default:

	    break;

	}

    fprintf (stderr, "error was found at line #%d\n", yylineno);

    if ((int) the_error < OK)
	a22_continue = FALSE;

    DEBUG2 (fprintf (stderr, "aap_error: leaving\n"));

    }

main	(argc, argv)
int	argc;
string	argv [];

    {

#define	BOX_EXT	"_BOX"
#define	SDD_EXT ".sdd"
#define	ILL_EXT ".ill"

    string	source_file,
		file_part,
		short_name,
		ill_file,
		ill_short_name,
		new_source,
		mailbox_name,
		temp,
		source_file_env,
		magic_number,
		getenv ();
    FILE	*fopen (),
		*rfp;
    char	language = 'c';
    int		i,
		c,
		getopt ();
    extern	int	optind;
    extern	char	*optarg;
    boolean	cflag = FALSE,
		aflag = FALSE,
		dot = FALSE,
		slash = FALSE;

/*	say hi */

    fprintf (stderr, "Welcome to %s\n", a22_saddle_magic_id);
    sdd_debug = 0;

/*	bust the options out of the command line */

    while ((c = getopt (argc, argv, "acd:")) != EOF)
	switch (c)

	    {

	    case 'a':

		if (cflag)
		    {
		    fprintf (stderr, "illegal use of options\n");
		    return (-1);
		    }
		else
		    {
		    aflag = TRUE;
		    language = 'a';
		    }
		break;

	    case 'c':

		if (aflag)
		    {
		    fprintf (stderr, "illegal use of options\n");
		    return (-1);
		    }
		else
		    {
		    cflag = TRUE;
		    language = 'c';
		    }
		break;

	    case 'd':

		sdd_debug = atoi (optarg);
		optind++;
		break;

	    case '?':
	    default:

		fprintf (stderr, "illegal option\n");
		break;

	    }	/* end switch (and while) */

/*	compute the source file, file part, and short name */

    temp = u22_copy_string (argv [argc - 1]);
    if (strcmp (temp + strlen (temp) - 4, SDD_EXT))
	{
	source_file = strcpy (make_node (strlen (temp) + strlen (SDD_EXT)
	    + 1), temp);
	(void) strcat (source_file, SDD_EXT);
	file_part = u22_copy_string (temp);
	}
    else
	{
	source_file = u22_copy_string (temp);
	file_part = u22_copy_string (temp);
	file_part [strlen (file_part) - 4] = '\0';
	}
    i = strlen (file_part) - 1;
    slash = FALSE;
    while (i >= 0 && !slash)
	{
	if (file_part [i] == '/')
	    slash = TRUE;
	else
	    i--;
	}
    short_name = u22_copy_string (file_part + i + 1);

/*	set the default execution string */

    aap_tap_exec = short_name;

/*	tell the shell what the source file is, run the c preprocessor, and
	get the new source file name */

    source_file_env = strcpy (make_node (strlen ("source_file=") +
	strlen (source_file) + 1), "source_file=");
    (void) strcat (source_file_env, source_file);
    (void) putenv (source_file_env);
#define	cpp_command	"/lib/cpp ${source_file} > ${new_source}"
    temp = u22_copy_string (cpp_command);
    system (temp);
    (void) wait ();

/*	compute the ill file and the short name of the ill file */

    ill_file = strcpy (make_node (strlen (file_part) + strlen (ILL_EXT) + 1),
	file_part);
    (void) strcat (ill_file, ILL_EXT);
    ill_short_name = strcpy (make_node (strlen (short_name) + strlen (ILL_EXT)
	+ 1), short_name);
    strcat (ill_short_name, ILL_EXT);

/*	export necessary shell variables */

    fprintf (stdout, "language=%c\n", language);
    fprintf (stdout, "debug_level=%d\n", sdd_debug);
    fprintf (stdout, "file_part=%s\n", file_part);
    fprintf (stdout, "ill_file=%s\n", ill_file);
    fprintf (stdout, "export debug_level file_part ill_file\n");

/*	compute the mailbox name */

    mailbox_name = strcpy (make_node (strlen (short_name) + strlen (BOX_EXT)
	+ 1), u22_convert_string (short_name, UPPERCASE));
    (void) strcat (mailbox_name, BOX_EXT);

/*	initialize the SADDLE list */

    a22_saddle_list = make_list ();

/*	open the file to parse */

    new_source = u22_copy_string (getenv ("new_source"));

    yyin = fopen (new_source, "r");
    if (yyin != NULL)
	{
	DEBUG2 (fprintf (stderr, "opening %s\n", new_source));
	fprintf (stderr, "reading from %s\n", source_file);
	yyparse ();
	DEBUG2 (fprintf (stderr, "done parsing\n"));

	DEBUG3 (
	    a22_saddle_node	*the_node = NULL;
	    while ((the_node = (a22_saddle_node *) get_list_next
		(a22_saddle_list, the_node)) != NULL)
		fprintf (stderr, "name: %s, type: %d\n", the_node -> name,
		    the_node -> stype);
	    )

	if (a22_continue)
	    {

/*	get the magic number, then call the c, ada (if appropriate), and ill
	back ends */

	    magic_number = u22_copy_string (getenv ("procname"));
	    acg_c_code (file_part, mailbox_name, short_name, ill_short_name,
		magic_number);
	    if (language == 'a')
		acg_ada_code (file_part, mailbox_name, short_name,
		    ill_short_name, magic_number);
	    acg_ill_code (file_part, mailbox_name, short_name,
		ill_short_name, magic_number);
	    }

/*	if there was a fatal error, print appropriate message and exit */

	else
	    {
	    fprintf (stderr, "code generation suppressed due to error(s)\n");
	    return (-1);
	    }
	}

/*	if we couldn't open the source file, print and exit */

    else
	{
	fprintf (stderr, "couldn't open %s\n", source_file);
	return (-1);
	}

/*	if we got here, everything worked fine */

    return (0);

    }
