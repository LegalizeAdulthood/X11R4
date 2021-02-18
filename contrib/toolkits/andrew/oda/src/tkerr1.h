
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tkerr1.h,v 1.2 89/10/26 14:12:38 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tkerr1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	tkerr1.h,v $
 * Revision 1.2  89/10/26  14:12:38  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:41:16  mss
 * Initial revision
 * 
 * Revision 1.20  89/08/31  18:27:24  mss
 * Provides mapping from LCL_ERROR_POINTER to ERROR_POINTER
 * 
 * Revision 1.19  89/08/29  17:44:01  annm
 * added short real names for names that are too long
 * 
 * Revision 1.18  89/08/29  13:13:00  annm
 * added real names for TKError and TKErrorMesg
 * 
 * Revision 1.17  89/07/19  22:32:00  mss
 * Added new warning facility
 * 
 * Revision 1.16  88/12/15  09:21:21  mss
 * Moved the renaming of TKERR_{Init,Final}
 * so that it was independent of whether
 * messages were formatted
 * 
 * Revision 1.15  88/09/16  14:00:12  jr
 * Define TKERR_FormatStringType
 * 
 * Revision 1.14  88/09/06  14:54:49  annm
 * no change
 * 
 * Revision 1.13  88/09/06  12:06:45  annm
 * 
 * no change
 * 
 * Revision 1.12  88/09/02  11:42:53  jr
 * Some cleanup
 * 
 * Revision 1.11  88/08/31  12:27:44  jr
 * 1st routines for internal error
 * handling
 * 
 * Revision 1.10  88/06/15  17:25:28  jr
 * Make calls to TKERR_Init
 * 
 * Revision 1.9  88/06/08  16:58:51  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.8  88/06/08  14:37:58  annm
 * added Copyright notice
 * 
*/
    
#ifndef TK_INCLUDED_TKERR1
#define TK_INCLUDED_TKERR1

#ifdef TK_GVARS_TKERR1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/* the global error flag */

#ifndef USE_LONG_NAMES

#define TKError		gtkerr
#define TKErrorMesg	gtkmsg

#endif

#define	ERROR_POINTER	(LCL_ERROR_POINTER)

EXTERN INT_type		TKError;
EXTERN CHAR_type	TKErrorMesg[ERROR_MESG_SIZE+1];	/* Room for '\0' */

#ifndef USE_LONG_NAMES

#define GenericError	generr

#endif

EXTERN INT_type	    GenericError();

/*
    Declarations seen only inside the tool kit go here
*/

#ifdef TK_TKERR

#ifndef USE_LONG_NAMES

#define TKERR_Init	    gtkini
#define TKERR_Final	    gtkfnl

#define AddChars	    gtkach
#define FormattingErrors    gtkfer
#define Truncate	    gtktru
#define ErrorIndex	    gtkein
#define TruncMarker	    gtktrm

#endif
    /*USE_LONG_NAMES*/

#ifdef DONT_FORMAT_ERRORS

    /* Site doesn't want any error formatting */
#define TKERR_StartErrMsg()			((INT_type)0)
#define TKERR_StartWarnMsg()			((INT_type)0)
#define TKERR_Format(string)			((INT_type)0)
#define TKERR_FormatInt(format, value)		((INT_type)0)
#define TKERR_FormatStringType(format, value)	((INT_type)0)
#define TKERR_EndErrMsg()			((INT_type)0)
#define TKERR_EndWarnMsg()			((INT_type)0)

#else
    /*DONT_FORMAT_ERRORS not defined, i.e., do format error msgs */

    /* Site does want error formatting */
#ifndef USE_LONG_NAMES
#define TKERR_StartErrMsg	gtkste
#define TKERR_StartWarnMsg	gtkstw
#define TKERR_Format		gtkfor
#define TKERR_FormatInt		gtkfmi
#define TKERR_FormatStringType	gtkfsy
#define TKERR_EndErrMsg		gtkene
#define TKERR_EndWarnMsg	gtkenw
#endif
    /*USE_LONG_NAMES*/

EXTERN INT_type TKERR_StartErrMsg();
EXTERN INT_type TKERR_StartWarnMsg();
EXTERN INT_type TKERR_Format();
EXTERN INT_type TKERR_FormatInt();
EXTERN INT_type TKERR_FormatStringType();
EXTERN INT_type TKERR_EndErrMsg();
EXTERN INT_type TKERR_EndWarnMsg();

#endif
    /*DONT_FORMAT_ERRORS*/

EXTERN INT_type TKERR_Init();
EXTERN INT_type TKERR_Final();

#endif
    /*TK_TKERR*/

/*
    Declarations seen within the module go here.
*/

#ifdef TK_IMPL_TKERR

#endif

#endif
