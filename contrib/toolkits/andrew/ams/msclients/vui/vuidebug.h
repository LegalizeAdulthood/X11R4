/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* 
 * Control inclusion of debugging code in object code
 */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/msclients/vui/RCS/vuidebug.h,v 2.3 89/02/15 12:14:02 ghoti Exp $ */

#ifndef DEBUG
#define debug(xxx_foo)
#endif /* DEBUG */
#ifdef DEBUG
#define debug(xxx_foo) debugrtn xxx_foo
#endif /* DEBUG */
