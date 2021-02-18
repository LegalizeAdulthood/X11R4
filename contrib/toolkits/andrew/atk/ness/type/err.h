/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/type/RCS/err.h,v 1.1 89/08/22 15:29:22 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/type/RCS/err.h,v $ */

#ifndef _err_
#define _err_
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	static char *err_rcsid = "$Header:";
#endif

/* 
	err.h

	 header file for signal error handling

*/

/*
 * $Log:	err.h,v $
 * Revision 1.1  89/08/22  15:29:22  wjh
 * Initial revision
 * 
 */

extern jmp_buf err_env;

#define err_LookAhead() (_err_LookAhead(), setjmp(err_env))

int	_err_LookAhead();
void	err_MarkEnd();


#endif    /* _err_ */
