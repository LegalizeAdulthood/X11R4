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
 * ramtek.c - GKS function to draw the Ramtek
 */

#include <stdio.h>
#include <xgks.h>
#include "demo.h"

draw_ramtek(wsid)
Gint wsid;
{
	Glimit Window;		/*MIT*/

	Window.xmin = 0.0;	/*MIT*/
	Window.xmax = 57.0;	/*MIT*/
	Window.ymin = 0.0;	/*MIT*/	
	Window.ymax = 45.6;	/*MIT*/

	gsetwindow( wsid, &Window );
	gselntran(1);

	do_ram_monitor();

}


do_ram_monitor()
{
	Gpoint pts[20];		/*MIT*/
	
	gsetfillcolorind(DARKGRAY);
	
	gsetfillintstyle( GSOLID );

	gsetlinecolorind(BLACK);

/* base */
	pts[0].x=0.0;	pts[0].y=0.0;
	pts[1].x=49.0;	pts[1].y=0.0;
	pts[2].x=49.0;	pts[2].y=45.5;
	pts[3].x=0.0;	pts[3].y=45.5;
	pts[4].x=0.0;	pts[4].y=0.0;
	gfillarea(5, pts);

/* bezel */
	gsetfillcolorind(BLACK);
	
	pts[0].x=1.5;	pts[0].y= 8.0;
	pts[1].x=47.5;	pts[1].y= 8.0;
	pts[2].x=47.5;	pts[2].y=44.0;
	pts[3].x=1.5;	pts[3].y=44.0;
	pts[4].x=1.5;	pts[4].y= 8.0;
	gfillarea(5, pts);
/* screen */
	gsetfillcolorind(BLUE);
	pts[0].x=5.0;	pts[0].y=11.5;
	pts[1].x=44.0;	pts[1].y=11.5;
	pts[2].x=44.0;	pts[2].y=40.5;
	pts[3].x=5.0;	pts[3].y=40.5;
	pts[4].x=5.0;	pts[4].y=11.5;
	gfillarea(5, pts);

/* handles */
	gsetfillcolorind(SILVER);
	pts[0].x= 0.5;	pts[0].y=1.0;
	pts[1].x= 2.0;	pts[1].y=1.0;
	pts[2].x= 2.0;	pts[2].y=7.0;
	pts[3].x= 0.5;	pts[3].y=7.0;
	pts[4].x= 0.5;	pts[4].y=1.0;
	gfillarea(5, pts);

/* handles */
	gsetfillcolorind(SILVER);
	pts[0].x=47.0;	pts[0].y=1.0;
	pts[1].x=48.5;	pts[1].y=1.0;
	pts[2].x=48.5;	pts[2].y=7.0;
	pts[3].x=47.0;	pts[3].y=7.0;
	pts[4].x=47.0;	pts[4].y=1.0;
	gfillarea(5, pts);

}
