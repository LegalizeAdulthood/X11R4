/*
 *	SunTroff -  A program to display the output of Device Independent
 *		Troff as a window on a Sun Workstation.
 *
 *	Authors - The original version of this program was written by
 *			Richard L. Hyde (Purdue)
 *			David Slattengren (Berkeley)
 *		It was nearly all rewritten, cleaned up and a more elegant
 *		user interface installed by
 *			Malcolm Slaney (Schlumberger Palo Alto Research)
 *
 *	Legalese -  This command was developed as an independent project
 *		to satisfy a need of the author.  This program may contain
 *		bugs and the user is cautioned to independently verify that
 *		the program is suitable for the user's intended purpose.
 *		The program is made available on an ``as is'' basis with
 *		all faults and without any implied or expressed warranties
 *		or support from either the author, Malcolm Slaney, or
 *		Schlumberger Palo Alto Research Laboratory.
 *
 *		I am putting this program in the public domain.  You are
 *		free to use it as you wish.  In return I ask two things.
 *		First, that you do not remove the names of the authors
 *		from this work.  Secondly, if you make changes or
 *		improvements to this program that you pass these back to
 *		the author so that everybody can benefit from the
 *		improvements.
 *
 *					Malcolm Slaney  (December 1986)
 *					Schlumberger Palo Alto Research
 *					3340 Hillview Avenue
 *					Palo Alto, CA 94304
 *					(415) 496-4669
 *					spar!malcolm@decwrl.dec.com
 *					malcolm@ecn.purdue.edu
 *					malcolm@spar.slb.com (Someday)
 */

/*
 *
 *	"long" fixed-point arithmetic package library
 *
 *	This DEPENDS on a sign-filled arithmetic right-shift
 */

#include "fixpoint.h"


#define F_INTEGER	(((1<<F_INTSHFT)-1)<<F_INTSHFT)
#define F_FRACTION	((1<<F_INTSHFT)-1)
#define F_FRACT		((1<<F_QSHFT)-1)
#define	F_SIGN_BIT	(1 << ((F_INTSHFT * 2) - 1))


/*----------------------------------------------------------------------------*
 | Routine:	fmult (fixed x, fixed y)
 |
 | Results:	returns fixed-point product of x and y (fixed-point numbers)
 |
 | Bugs:	Requires arithmetic right-shift
 *----------------------------------------------------------------------------*/

fixed fmult(x, y)
register fixed x;
register fixed y;
{
    return
    ((x >> F_QSHFT) * (y >> F_QSHFT))			/* int * int */
    + (((x >> F_QSHFT) * (y & F_FRACT)) >> F_QSHFT)	/* int * fract */ 
    + (((y >> F_QSHFT) * (x & F_FRACT)) >> F_QSHFT);	/* fract * int */
}


/*----------------------------------------------------------------------------*
 | Routine:	fdiv (fixed x, fixed y)
 |
 | Results:	returns the fixed-point quotient of x / y (fixed-point numbers)
 |
 | Bugs:	returns largest possible numbers on divide by zero.
 |		Requires arithmetic right-shift
 *----------------------------------------------------------------------------*/

fixed fdiv(x, y)
register fixed x;
register fixed y;
{
    register int i = F_INTSHFT;
    register fixed temp;

    if (y == 0)
	return (x < 0 ? (fixed) F_SIGN_BIT : (fixed)~F_SIGN_BIT);

    while (((temp = x << 1) & F_SIGN_BIT) == (x & F_SIGN_BIT)) {
	x = temp;
	i--;
    }
    while (!(y & 1)) {
	y >>= 1;
	i--;
    }
    if(i<0)
	return ((x / y) >> -i);
    return ((x / y) << i);
}
