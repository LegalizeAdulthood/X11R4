/* 
 * @(#) olgx_impl.h 1.5 89/11/14 
 */

/*
 * OPEN LOOK object drawing package
 * Sun Microsystems, Inc.
 */

#ifndef OL_PRIVATE_DEFINED
#define OL_PRIVATE_DEFINED

#include <olgx/olgx.h>

#define STRING_SIZE 		32	/* max size of a glyph font string */

/*
 * OPEN LOOK (TM) constant definitions
 */


/*
 * Macro definitions
 */

#define VARIABLE_LENGTH_MACRO(start_pos, offset)		\
	for (i = 0; i < num_add; i++) {				\
		string[start_pos+i] = offset + add_ins[i];	\
	}

/*
 * Private function declarations
 */

int	calc_add_ins();
char 	*olgx_malloc();
void	olgx_free();

#endif	!OL_PRIVATE_DEFINED
