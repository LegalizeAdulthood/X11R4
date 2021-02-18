/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/ext.h,v 1.3 89/09/08 17:48:40 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/ext.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
CMU-ITC-071) for full text & permitted uses.

$Log:	ext.h,v $
 * Revision 1.3  89/09/08  17:48:40  mss
 * /tmp/msg
 * 
 * Revision 1.2  89/08/30  16:50:19  mss
 * Changed capitalization of *font.[ch]
 * 
 * Revision 1.1  89/08/30  16:36:19  mss
 * Initial revision
 * 
 * Revision 1.2  89/01/12  17:54:29  jaap
 * better handling of escape sequences in character content portion
 * As a side effect we now also know about STAB although it is not done properly to the standard
 * 
 * Revision 1.1  89/01/05  18:46:34  jaap
 * Initial revision
 * 
 */

/* externals declarations, global definitions etc. */

#include <stdio.h>
#include <odatk.h>

#include <ckcontent.h>
#include <initfonts.h>
#include <invokefont.h>

#define	INHERIT	    BOOL_true
#define	NOINHERIT   BOOL_false
#define	LOCAL	    BOOL_false
#define	IGNORE	    ( PARM_MASK_type *) 0

/* for scaling int pairs */
#define	num	first
#define	denom	second

/* character escape sequences */
#define ESC	'\033'
#define SGR	'\155'

extern char *progname;
extern char msgsbuf[];
extern at_CONCATENATION_type concat;
extern dp_UNIT_SCALING_type smu;
