	static char rcsid[] = "$Header: /u/04c/mnt/new_integ/tools/src/RCS/getit.c,v 1.1 89/10/08 22:45:18 serpent Exp $";
/*
$Log:	getit.c,v $
 * Revision 1.1  89/10/08  22:45:18  serpent
 * Initial revision
 * 
*/

/*--------------------------------------------------------------------------*\
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

#include <stdio.h>


#ifndef INDEX
#define INDEX index
#endif


extern char	*INDEX();
extern int	 optind;
extern char	*optarg;


main(ac, av)
    register int	 ac;
    register char 	 *av[];
{
    register char 	*flags;
    register int	 c;

    /* Check usage. */
    if (ac < 2) {
	fprintf(stderr, "usage: %s flag-specification arg-list\n", av[0]);
	exit(2);
    }

    /* Play games; remember the flags (first argument), then splice
       them out so it looks like a "standard" command vector. */
    flags = *(av+1);
    *(av+1) = *av;
    av++;
    ac--;

    /* Print flags. */
    while ((c = getopt_sys(ac, av, flags)) != EOF) {
	if (c == '?')
	    exit(1);
	/* We assume that shells collapse multiple spaces in `` expansion. */
	printf("-%c %s ", c, INDEX(flags, c)[1] == ':' ? optarg : "");
    }

    /* End of flags; print rest of options. */
    printf("-- ");
    for (av += optind; *av; av++)
	printf("%s ", *av);
    exit(0);
}
