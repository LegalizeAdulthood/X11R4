
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/mac/RCS/lcsite.h,v 1.2 89/10/18 16:52:39 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/mac/RCS/lcsite.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	lcsite.h,v $
 * Revision 1.2  89/10/18  16:52:39  mss
 * Remove suppression of error string formatting. MPW 3.0 can correctly
 * allocate read-only strings in code resources eliminating overflows
 * of indexes into global space.
 * 
 * Revision 1.1  89/09/21  06:12:25  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  13:16:39  mss
 * Fixed ones_complement and error_pointer
 * 
 * Revision 1.2  88/12/15  16:48:32  mss
 * Initial version of Macintosh specific site file
 * (disables formatting of error messages which causes
 * a global data segment overflow)
 * 
 * Revision 1.7  88/08/29  16:26:33  jr
 * Add ERROR_MESG_SIZE
 * 
 * Revision 1.6  88/08/15  16:07:31  mss
 * Casted (clothed?) all naked constants so they have the
 * appropriate type
 * 
 * Revision 1.5  88/07/28  10:00:41  mz0k
 * Typo error corrected.
 * 
 * Revision 1.4  88/07/27  12:46:17  mz0k
 * Added BITS_PER_INT macro
 * 
 * Revision 1.3  88/06/08  17:26:43  annm
 * no change
 * 
 * Revision 1.2  88/06/08  15:54:51  annm
 * added copyright notice
 * 
*/

/*

	Site-dependent include file for Andrew

*/

#ifndef TK_INCLUDED_SITE
#define TK_INCLUDED_SITE

    /* Integer type */
typedef int		    INT_type;

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
typedef unsigned char	    BYTE_type;
#define MakeByte(b)	    ( (BYTE_type) b)

    /* Boolean type */
typedef BYTE_type	    BOOL_type;

    /* TRUE & FALSE */
#define BOOL_false	    ( (BOOL_type) 0)
#define BOOL_true	    ( (BOOL_type) 1)

    /* Values */
typedef short int	    VALUE_type;


    /* Tags */
typedef short int	    TAG_type;

    /* Parameter mask */
typedef int		    PARM_MASK_type;

    /* Arbitrary pointers */
typedef char		    *POINTER_type;
#define LCL_ERROR_POINTER	    ((POINTER_type) 0)

    /* We use register's */
#define REGISTER	    register

    /* Size for error message buffer */
#define ERROR_MESG_SIZE	    (200)

    /* Global segment is too large for mac (mpw 2.0) with
	  all of the error strings, so eliminate them 
	  (3.0 knows how to detail with strings as part of code
	   segments, so enable for 2.0, leave commented out for 3.0) */
    /* #define DONT_FORMAT_ERRORS */

#endif
