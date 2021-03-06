/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL   61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * gerrorhand.c - GKS functions  gerrorhand()
 *
 * $Header: gerrorhand.c,v 4.0 89/08/31 16:20:58 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/gerrorhand.c,v $
 *
 * $Log:	gerrorhand.c,v $
 * Revision 4.0  89/08/31  16:20:58  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.5  89/06/05  10:16:28  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 1.4  89/02/04  15:23:59  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 1.3  88/12/12  11:21:34  amy
 * PTR c1130	gerrorhand:  changed funcname from an int point to an int.
 * 
 * Revision 1.2  88/12/07  17:07:12  amy
 * No change.
 * 
 * Revision 1.1  88/12/01  13:17:34  amy
 * Initial revision
 * 
 * Revision 1.2  88/12/01  11:07:59  todd
 * Removed function gerrorhand from gks_error.c
 * 
 */

static char *rcsid = "$Header: gerrorhand.c,v 4.0 89/08/31 16:20:58 amy Exp $";

#include "gks_implem.h" /* d1 */

/*$F
 * ERROR HANDLING
 *
 *
 *      Called whenever an error is detected by a gks function.  This routine
 *      can be replaced by a user's own err_hand() routine.
 *
 * returns: 0 (always)
 *
 * See also: Ansi standard p. 195
 */

gerrorhand(errnum, funcname, perrfile)
        Gint errnum;            /* number for the error that was detected. */
        Gint funcname;          /* name of function that detected the error. */
        Gfile *perrfile;        /* file where error message is to be printed. */
{
        if (xgks_state.gks_state == GGKCL) /* don't use the perrfile it's not defined */ /*c1147*/
                perrfile = stderr;
        (void) gerrorlog(errnum, funcname, perrfile);

        return( OK );
}

