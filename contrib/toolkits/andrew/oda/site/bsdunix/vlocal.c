
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/bsdunix/RCS/vlocal.c,v 1.2 89/09/29 11:46:20 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/bsdunix/RCS/vlocal.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	vlocal.c,v $
 * Revision 1.2  89/09/29  11:46:20  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:09:18  mss
 * Initial revision
 * 
 * Revision 1.8  89/09/06  17:20:50  annm
 * fixing problems with names
 * 
 * Revision 1.7  89/08/31  21:13:37  jg2f
 * no change
 * 
 * Revision 1.6  89/07/12  15:20:56  jg2f
 * Changed defalut file protection mode in VALID_WriteOpen
 * 
 * Revision 1.5  89/07/10  11:13:56  jg2f
 * VALID_WriteOpen now creates file if previously nonexistant.
 * 
 * Revision 1.4  89/07/06  12:52:31  jg2f
 * Fixed open file to create file when openign for reading.
 * 
 * Revision 1.3  88/10/02  14:15:57  mss
 * Changed "s to <>s
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
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/bsdunix/RCS/vlocal.c,v $";
#endif

/*

	Site-dependent validation stuff -- UNIX version.

*/

#include <fcntl.h>

#define VLOCAL_IMPL

#include <lcsite.h>
#include <errors.h>
#include <vlocal.h>

#undef VLOCAL_IMPL


/*
    Return output descriptor for standard output -- place for
    validation output to go.
*/

INT_type VALID_GetStdout()
{
    return 1;
}

INT_type VALID_ReadOpen(name)
    /*IN*/ CHAR_type	*name;
{
    INT_type rval;

    rval = open(name, O_RDONLY);
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

    rval = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (rval >= 0) {
	goto LEAVE;
    }

    /* There was a problem -- map to error */
    rval = ERROR_INT;

LEAVE:
    return rval;
}
