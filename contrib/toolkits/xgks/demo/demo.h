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
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 *	All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * $Header: demo.h,v 4.0 89/09/01 14:32:05 amy Exp $
 */
#define BLACK 0
#define WHITE 1
#define RED 224
#define GREEN 28
#define BLUE 11
#define YELLOW 252
#define CYAN 31
#define VIOLET 227
#define SILVER 223
#define BEIGE 250
#define DARKGREEN 45

#define DARKGRAY BEIGE
#define MEDIUMGRAY BEIGE

static
WaitForBreak( ws_id )
	Gint ws_id;
{
	Gchoice init;
	Gchoicerec record;
	Glimit earea;
	earea.xmin = 0.0;
	earea.xmax = 1279;
	earea.ymin = 0.0;
	earea.xmax = 1023;

	init.status = GC_NOCHOICE;
	init.choice = 0;
	record.pet1.data = NULL;
	ginitchoice( ws_id, 1, &init, 1, &earea, &record );
	gsetchoicemode( ws_id, 1, GREQUEST, GECHO );
	for( ; init.status != GC_NONE ; )
		greqchoice( ws_id, 1, &init );
}
