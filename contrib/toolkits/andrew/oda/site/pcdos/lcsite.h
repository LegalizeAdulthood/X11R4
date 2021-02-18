
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/pcdos/RCS/lcsite.h,v 1.1 89/09/21 06:13:29 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/pcdos/RCS/lcsite.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	lcsite.h,v $
 * Revision 1.1  89/09/21  06:13:29  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/11  13:20:24  mss
 * casted constants, fixed one's complement, changed error_pointer
 * 
 * Revision 1.4  88/08/29  16:27:37  jr
 * Add ERROR_MESG_SIZE
 * 
 * Revision 1.3  88/08/03  12:43:52  jr
 * Some fix-ups for a PC
 * 
 * Revision 1.2  88/06/15  13:17:42  jr
 * Can be same as unix
 * 
 * Revision 1.3  88/06/08  17:26:43  annm
 * no change
 * 
 * Revision 1.2  88/06/08  15:54:51  annm
 * added copyright notice
 * 
*/

/*

	Site-dependent include file for PC-DOS

*/

#ifndef TK_INCLUDED_SITE
#define TK_INCLUDED_SITE

    /* Integer type */
typedef long int	    INT_type;

    /* We use 2's complement.  If using 1's complement, put definition for ONES_COMPLEMENT  here */

    /* Maximum & minimum integers */
#define MAXIMUM_INT	    (0x7FFFFFFF)
#define MINIMUM_INT	    (0x80000001)

    /* Bits in an integer */
#define	BITS_PER_INT	    ( (INT_type) 32)

    /* Character type */
typedef char		    CHAR_type;
#define MakeChar(c)	    ( (CHAR_type) c)

    /* Byte type */
typedef char		    BYTE_type;
#define MakeByte(b)	    ( (BYTE_type) b)

    /* Boolean type */
typedef BYTE_type	    BOOL_type;

    /* TRUE & FALSE */
#define BOOL_false	    ( (BOOL_type) 0)
#define BOOL_true	    ( (BOOL_type) 1)

    /* Values */
typedef int		    VALUE_type;


    /* Tags */
typedef int		    TAG_type;

    /* Parameter mask */
typedef int		    PARM_MASK_type;

    /* Arbitrary pointers */
typedef char		    *POINTER_type;
#define LCL_ERROR_POINTER	    ((POINTER_type) 0)

    /* We use register's */
#define REGISTER	    register

    /* Error message buffer size */
#define ERROR_MESG_SIZE	    (200)

#endif
