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


/******************** START OF MODULE SPECIFICATIONS ********************
*
* MODULE NAME:		imerrno.h
*
* DESCRIPTIVE NAME:  
*
* MODULE TYPE:		C
*
* COMPILER:		AIX C
*
* CHANGE ACTIVITY:	0%G% - Modified
*
* STATUS:		Input Method Version %Q%
*
* CATEGORY:		Header
*
******************** END OF SPECIFICATIONS *****************************/
 
#ifndef	_h_IMERRNO
#define	_h_IMERRNO

#define	IMNoSuchLanguage	1	/* No such language is found */
#define	IMCouldNotLoad		2	/* Specified module couldn't load */
#define IMInitializeError	3	/* IMFEP returns error in init() */
#define IMKeymapInitializeError	4	/* Keymap isn't found or invalid */
#define IMInvalidParameter	5	/* Specified parm. is not valid */
#define IMCallbackError		6	/* Callback returns error */


/* To add Language Dependent Error Numbers */

/* SBCS Input Method */
#define SIMERN	('S' << 16)

/* Japanese Input Method */
#define JIMERN	('J' << 16)

/* Chinese (Hanji) Input Method */
#define HIMERN	('H' << 16)

/* Korean (Hangeul) Input Method */
#define KIMERN	('K' << 16)

#endif	_h_IMERRNO
