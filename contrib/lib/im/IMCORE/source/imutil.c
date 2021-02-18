/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


/******************** START OF MODULE SPECIFICATIONS *********************
*
* MODULE NAME:		imutil.c
*
* DESCRIPTIVE NAME:	Input Method Utility Functions.
*
* FUNCTION:		This file includes utility functions.
*
* MODULE TYPE:		C
*
* COMPILER:		AIX C
*
* CHANGE ACTIVITY:	0%G% - Modified
*
* STATUS:		Input Method Version %Q%
*
******************** END OF SPECIFICATIONS ******************************/
 
/*-----------------------------------------------------------------------*
*	INCLUDE FILES
*-----------------------------------------------------------------------*/
#include <sys/types.h>
 
/*-----------------------------------------------------------------------*
*	EXTERNAL REFERENCES
*-----------------------------------------------------------------------*/
caddr_t	malloc(), realloc(), calloc();
void	free();

caddr_t	immalloc(size)
uint	size;
{
	caddr_t	addr;
	
	if (!(addr = malloc(size)))
		(void)abort();
	return addr;
}

caddr_t	imrealloc(ptr, size)
caddr_t	ptr;
uint	size;
{
	caddr_t	addr;
	
	if (!(addr = realloc(ptr, size)))
		(void)abort();
	return addr;
}

caddr_t	imcalloc(nelem, elsize)
uint	nelem, elsize;
{
	caddr_t	addr;
	
	if (!(addr = calloc(nelem, elsize)))
		(void)abort();
	return addr;
}

void	imfree(ptr)
caddr_t	ptr;
{
	free(ptr);
}
