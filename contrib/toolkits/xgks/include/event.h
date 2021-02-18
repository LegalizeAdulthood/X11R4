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
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 *  Define file for : Xevent 
 *
 * 
 * $Header: event.h,v 4.0 89/08/31 16:33:52 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/event.h,v $
 *
 * $Log:	event.h,v $
 * Revision 4.0  89/08/31  16:33:52  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.3  89/02/04  15:31:40  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.2  88/08/19  13:09:29  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:47:07  david
 * Initial revision
 *  
 *
 */

#ifndef EVENT_H
#define EVENT_H

extern int xXgksSIGIOStart ();					/* c1147 */
/* c1147:  xProcessEvents no longer needed by other modules */

#endif
