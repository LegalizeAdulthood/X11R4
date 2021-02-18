/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	November 1985.
 *
 *	%W%	%G%
*/
#include "fig.h"

/*VARARGS1*/
put_err(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
char   *format, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8;
{
	fprintf(stderr, format, arg1, arg2, arg3, arg4, arg5, 
		arg6, arg7, arg8);
	put_msg(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
