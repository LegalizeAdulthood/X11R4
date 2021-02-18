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
* MODULE NAME:		imloadable.c
*
* DESCRIPTIVE NAME:	IMCORE
*
* FUNCTION:		This file includes incomplete C source program and
*			is included by imload.c file.
*			The purpose of this file is to allow each IMFEP can
*			be bound to IMCORE statically.
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

/*
 *	Each *StaticLoad() function takes a parameter 'lang' of type
 *	IMLanguage (pointer to string).  These functions must returns a pointer
 *	to *IMInitialize() function (although this value is treated as
 *	integer in this file) if the parameter 'lang' matches its supported
 *	language. Otherwise it should return NULL and this routine tries
 *	the next one.  Also see the file imload.c.
 *
 *	Everyone who wants to add his own IMFEP and wants it to be bound
 *	statically must add a C 'if' statement to the bottom of this file
 *	like the following C code. This process will be done in the install
 *	procedure of the new IMFEP. Do as follows.
 *
 *	  echo 'if (ret = NEWStaticLoad(lang)) return ret;' >> imloadable.c
 *
 *	However, the static binding is not recommended because it binds all
 *	Input Method modules to the application program and they are usually
 *	very big.
 *
 *	The return value of load() function is of type 'int'. But the IMCORE
 *	expects it as of type a pointer to a function which returns a pointer
 *	to the IMFepRec structure. (namely, IMInitialize()).
 *	However, the acculate decralation or definition of these functions or
 *	data will be very complicated and the load() function itself doesn't
 *	need to know about it. So in this file and in the file imload.c,
 *	IMInitialize() is treated as a function which returns a value of type
 *	int.   It will be rarely problem but the size of int may be different
 *	from the size of a pointer to a function.
 *	
 */

/*
 *	imload.c contains the following C source flagment.
 *
 *	load(lang)
 *	IMLanguage	lang;
 *	{
 *		int	ret;
 *
 *	#include "imloadable.c"
 *
 *		return NULL;
 *	}
 *
 */

#ifdef	SIM
if (ret = SBCSStaticLoad(lang)) return ret;
#endif	SIM
#ifdef	JIM
if (ret = JIMStaticLoad(lang)) return ret;
#endif	JIM
