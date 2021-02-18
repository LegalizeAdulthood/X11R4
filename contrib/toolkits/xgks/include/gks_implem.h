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
 * Define file for : XGKS implementation 
 *		     all implementation files have to include this and only this 
 *		     define file
 * 
 * $Header: gks_implem.h,v 4.0 89/08/31 16:34:28 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/gks_implem.h,v $
 *
 * $Log:	gks_implem.h,v $
 * Revision 4.0  89/08/31  16:34:28  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.3  88/08/19  13:15:43  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:47:19  david
 * Initial revision
 *  
 *
 */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "xgks.h"
#include "gks_defines.h"
#include "primitive.h"
#include "input.h"
#include "gkslist.h"
#include "wslist.h"
#include "wdt.h"
#include "gkserrmsg.h"
#include "gks_errors.h"
#include "event.h"
