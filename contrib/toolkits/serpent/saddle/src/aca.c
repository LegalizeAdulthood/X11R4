	static char rcsid [] = "$Header: /u/04c/mnt/integ/saddle/src/RCS/aca.c,v 1.4 89/10/18 10:39:14 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:  aca.c
|
|  Description:
|	The Ada representation clause portion of the Ada back end code
|	generator
|
|  Component of:
|	SADDLE processor
|
|  Related files:
|	aca.h?
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
$Log:	aca.c,v $
 * Revision 1.4  89/10/18  10:39:14  ejh
 * added checks on fopen; switched over u22_copy_string
 * 
 * Revision 1.3  89/02/09  19:28:01  ejh
 * no changes
 * 
 * Revision 1.2  89/02/09  19:26:16  ejh
 * added copyright noticed
 * 
 * Revision 1.1  88/12/06  14:25:42  ejh
 * Initial revision
 * 
|	22 Nov 1988:	created; includes the old ada_reps.c
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

#define	a22_TOP

#include	<stdio.h>
#include	"serpent.h"
#include	"string.h"

#include	"u22.h"
#include	"a22.h"

/*	just so we have no problems with yacc variables */

int	yylineno;
FILE	*yyin;

main ()

    {

#define	ADA_EXT	".a"
#define	BYTE_SIZE	8

    char	element_name [100],
		component_name [100],
		barrier [100];
    int		i,
		fscanf (),
		offset,
		length;
    idd_data_types	type_type;
    FILE	*fopen (),
		*afp,
		*ifp;
    string	file_part,
		ada_file,
		ill_file,
		package_name,
		getenv ();
    boolean	in_record;

/*	get the environment variables needed */

    file_part = u22_copy_string (getenv ("file_part"));
    ill_file = u22_copy_string (getenv ("ill_2"));
    package_name = u22_copy_string (getenv ("dd_name"));
    sdd_debug = atoi (getenv ("debug_level"));

/*	compute the .a file name and open it and the ill file */

    ada_file = strcpy (make_node (strlen (file_part) + strlen (ADA_EXT) + 1),
	file_part);
    (void) strcat (ada_file, ADA_EXT);

    DEBUG2 (fprintf (stderr, "appending representation clauses to %s\n",
	ada_file));

    afp = fopen (ada_file, "a");
    if (afp == NULL)
	{
	fprintf (stderr, "couldn't open %s\n", ada_file);
	return (-1);
	}
    ifp = fopen (ill_file, "r");
    if (ifp == NULL)
	{
	fprintf (stderr, "couldn't open %s\n", ill_file);
	return (-1);
	}

    in_record = FALSE;
    while ((i = fscanf (ifp, "%s%s%d%d%d ", element_name, component_name,
	&offset, &length, &type_type)) != EOF)
	{
	switch (type_type)

	    {

	    case idd_record:

/*	if we're already in a record, end it; start new record */

		if (in_record)
		    fprintf (afp, "\tend record;\n\n");
		in_record = TRUE;
		fprintf (afp, "\tfor %s use record\n", element_name);
		break;

	    default:

/*	if we're in a record, compare the first two names read; if they're
	equal, then this is a scalar, so end the previous record; if they're
	not equal, then this is a component of an element, so put out a
	representation based on the ill record; if we're not in a record,
	then this is a scalar, so do nothing (it also means that it
	immediately follows another scalar) */

		if (in_record)
		    {
		    if (!strcmp (element_name, component_name))
			{
			fprintf (afp, "\tend record;\n\n");
			in_record = FALSE;
			}
		    else
			fprintf (afp,"\t\t%s at %d range 0..(%d * %d - 1);\n",
			    component_name, offset, length, BYTE_SIZE);
		    }

		break;

	    }	/* end switch */
	}	/* end while */

/*	at EOF, if we're in a record, end it; in any case, end the package */

    if (in_record)
	fprintf (afp, "\tend record;\n\n");
    fprintf (afp, "end %s;\n", package_name);

/*	close the files */

    fclose (afp);
    fclose (ifp);

    DEBUG2 (fprintf (stderr,
	"done appending representation clauses to %s\n\n", ada_file));

    return (0);
    }
