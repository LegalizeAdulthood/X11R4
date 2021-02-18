/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/prog/RCS/prodif.c,v 1.1 89/09/21 05:58:59 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/prog/RCS/prodif.c,v $

 *
 * (C) Copyright 1989 by Carnegie Mellon University
 *
 * Permission to use, copy, modify, and distribute these programs
 * and their documentation for any purpose and without fee is
 * hereby granted, provided that this copyright and permission
 * notice appear on all copies and supporting documentation, and
 * that the name of Carnegie Mellon University not be used in
 * advertising or publicity pertaining to distribution of the
 * programs without specific prior permission and notice be given
 * in supporting documentation that copying and distribution is
 * by permission of Carnegie Mellon University.
 *
 * Carnegie Mellon University makes no representations about the
 * suitability of this software for any purpose.  It is provided
 * as is, without express or implied warranty.
 *
 * Software by Ann Marks and James T. Lui,
 * Information Technology Center, Carnegie Mellon University,
 * except where noted.
 *

$Log:	prodif.c,v $
 * Revision 1.1  89/09/21  05:58:59  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/13  09:33:13  mss
 * Got rid of leading #
 * 
 * Revision 1.4  89/08/28  16:55:10  jl3p
 * added copyright notice
 * 
 * Revision 1.3  89/08/16  11:31:53  mss
 * fixed typo in include
 * 
 * Revision 1.2  89/08/16  10:02:12  mss
 * added use of odaiso.h in prodif
 * 
 * Revision 1.1  89/07/25  09:42:49  mss
 * Initial revision
 * 
 * Revision 1.1  89/07/11  15:29:13  annm
 * Initial revision
 * 
*/

#ifdef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/prog/RCS/prodif.c,v $";
#endif

#include <odatk.h>
#include <odaiso.h>
#include <stdio.h>


main( argc, argv, envp )
int argc;
char **argv;
char **envp;
{

    INT_type dummy;

    /*
	print an odif stream at stdin to the stdout
    */


    dummy = isode_PrintODIF( stdin );
    
}

