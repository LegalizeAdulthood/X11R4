
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/vms/RCS/vlocal.c,v 1.2 89/09/29 11:59:41 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/vms/RCS/vlocal.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	vlocal.c,v $
 * Revision 1.2  89/09/29  11:59:41  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:11:30  mss
 * Initial revision
 * 
 * Revision 1.3  88/10/02  14:20:37  mss
 * Got rid of extraneous .h extension for system
 * include files
 * 
 * Revision 1.2  88/09/30  15:32:20  mss
 * Changed "s to <>s
 * 
 * Revision 1.1  88/09/19  12:48:05  mss
 * Initial revision
 * 
 * Revision 1.2  88/06/15  12:41:42  jr
 * Change sys/file.h -> fcntl.h
 * (works on PC's & SYS V)
 * 
 * Revision 1.1  88/06/14  14:27:26  jr
 * Initial revision
 * 
 * Revision 1.3  88/06/08  17:31:17  annm
 * dealt with line too long in copyright notice
 * and changed #end to #endif
 * 
 * Revision 1.2  88/06/08  15:35:46  annm
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/vms/RCS/vlocal.c,v $";
#endif

/*

	Site-dependent validation stuff -- UNIX version.

*/

#include stdio
#include file

#define VLOCAL_IMPL

#include <lcsite.h>
#include <errors.h>
#include <vlocal.h>

/*
    Return output descriptor for standard output -- place for
    validation output to go.
*/

INT_type VALID_GetStdout()
{
    return (int) fileno(stdout);
}

INT_type VALID_ReadOpen(name)
    /*IN*/ CHAR_type	*name;
{
    INT_type rval;

    rval = open(name, (int) O_RDONLY, (int) 0);

    if (rval >= 0) {
	goto LEAVE;
    }

    /* There was a problem -- map to error */
    rval = ERROR_INT;

LEAVE:
    return rval;
}

INT_type VALID_WriteOpen(name)
    /*IN*/ CHAR_type	*name;
{
    INT_type rval;

    rval = open(name, (int) (O_RDWR | O_CREAT | O_TRUNC), (int) 0);

    if (rval >= 0) {
	goto LEAVE;
    }

    /* There was a problem -- map to error */
    rval = ERROR_INT;

LEAVE:
    return rval;
}
