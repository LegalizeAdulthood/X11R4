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
 * logo.c - GKS program to draw the various parts of our (old) hardware 
 *
 * usage:logo  [hostname:display]
 *
 */
static char rcsid[] = "$Header: demo.c,v 4.0 89/09/01 14:41:30 amy Exp $";

#include <stdio.h>
#include <xgks.h>
#include "demo.h"

char *Menu[] = {
	"Printer",
	"Ramtek",
	"9000",
	"PWG Logo",
	"Exit"
};
Glimit Marea = { 0.0, 150.0, 770.0, 1023.0 };

Gint	ws_id = 1;
Gint    mo_id = 200;

main(argc,argv)
	int argc;
	char *argv[];
{
	Glimit WsWindow;	/*MIT*/
	char *conn = (char *)NULL;
	Gchoice init;
	Gchoicerec record;

	int i;

	WsWindow.xmin = 0.0;	/*MIT*/
	WsWindow.xmax = 1.0;	/*MIT*/
	WsWindow.ymin = 0.0;	/*MIT*/	
	WsWindow.ymax = 0.8;	/*MIT*/

	for( i=1; i<argc; i++){
		if (index( argv[i], ':'))
			conn = argv[i];
	/* Application dependent options here */
	}

	gopengks(stdout,0);
	gopenws(ws_id, conn, conn);
	gactivatews(ws_id);

	gopenws(mo_id, "logo.out", "MO");
	gactivatews(mo_id);

/* do it here */

	gsetwswindow(1, &WsWindow );
	gsetwswindow(mo_id, &WsWindow );
	gsetdeferst(1, GASAP, GALLOWED);

	init.status = GC_NOCHOICE;
	init.choice = 0;
	record.pet3.data = NULL;
	record.pet3.number = (sizeof Menu) / sizeof (char *) ;
	record.pet3.strings = Menu;
	ginitchoice( ws_id, 1, &init, 3, &Marea, &record );

	gsetchoicemode( ws_id, 1, GREQUEST, GECHO );
	while( init.choice != 5 ) {
		greqchoice( ws_id, 1, &init );
		if (init.status != GC_OK)
			continue;
		switch( init.choice ) {
		case 1:
			gclearws(ws_id, GALWAYS);
			gclearws(mo_id, GALWAYS);
			show_printer();
			break;
		case 2:
			gclearws(ws_id, GALWAYS);
			gclearws(mo_id, GALWAYS);
			show_ramtek();
			break;
		case 3:
			gclearws(ws_id, GALWAYS);
			gclearws(mo_id, GALWAYS);
			show_9000();
			break;
		case 4:
			gclearws(ws_id, GALWAYS);
			gclearws(mo_id, GALWAYS);
			show_logo();
			break;
		case 5:
		default:
			break;
		}
	}

	gdeactivatews(ws_id);
	gclosews(ws_id);
	gdeactivatews(mo_id);
	gclosews(mo_id);
	gclosegks();
}

do_connections()
{
	Gpoint pts[22];
	Glimit Window;		/*MIT*/
	
	Window.xmin = 0.0;	/*MIT*/
	Window.xmax = 1.0;	/*MIT*/
	Window.ymin = 0.0;	/*MIT*/
	Window.ymax = 0.8;	/*MIT*/

	gsetwindow( 1, &Window );
	gsetviewport( 1, &Window );
	gsetfillcolorind(YELLOW);

	pts[0].x = .25;	 pts[0].y = .39;
	pts[1].x = .25;	 pts[1].y = .40;
	pts[2].x = .1;	 pts[2].y = .29;
	pts[3].x = .1;	 pts[3].y = .28;
	pts[4].x = .25;	 pts[4].y = .39;
	gfillarea(5, pts);

	pts[0].x = .5;	 pts[0].y = .19;
	pts[1].x = .5;	 pts[1].y = .2;
	pts[2].x = .4;	 pts[2].y = .07;
	pts[3].x = .4;	 pts[3].y = .061;
	pts[4].x = .5;	 pts[4].y = .19;
	gfillarea(5, pts);

	pts[0].x = .80;	 pts[0].y = .397;
	pts[1].x = .80;	 pts[1].y = .39;
	pts[2].x = .93;	 pts[2].y = .39;
	pts[3].x = .93;	 pts[3].y = .397;
	pts[4].x = .80;	 pts[4].y = .397;
	gfillarea(5, pts);

	pts[0].x = .58;	 pts[0].y = .58;
	pts[1].x = .58;	 pts[1].y = .59;
	pts[2].x = .70;	 pts[2].y = .70;
	pts[3].x = .71;	 pts[3].y = .70;
	pts[4].x = .58;	 pts[4].y = .58;
	gfillarea(5, pts);
}


/* fill the screen with the background color */
background(bkg)
	Gint bkg;
{
	Gpoint pts[20];		/*MIT*/
	Glimit Viewport;	/*MIT*/
	Glimit Window;		/*MIT*/

	Viewport.xmin = 0.0;	/*MIT*/
	Viewport.xmax = 1.0;	/*MIT*/	
	Viewport.ymin = 0.0;	/*MIT*/
	Viewport.ymax = 0.8;	/*MIT*/

	Window.xmin = 0.0;	/*MIT*/
	Window.xmax = 100.0;	/*MIT*/
	Window.ymin = 0.0;	/*MIT*/
	Window.ymax = 100.0;	/*MIT*/

	gsetwindow(1, &Window );
	gsetviewport( 1, &Viewport );
	gselntran(1);

	gsetfillcolorind( bkg );
	gsetfillintstyle(GSOLID);

	pts[0].x=0.0;	pts[0].y=0.0;
	pts[1].x=0.0;	pts[1].y=100.0;
	pts[2].x=100.0;	pts[2].y=100.0;
	pts[3].x=100.0;	pts[3].y=0.0;
	pts[4].x=0.0;	pts[4].y=0.0;
	gfillarea(5, pts);
}

Glimit Viewport;
#define s_viewport(w, xl, xh, yl, yh ) \
	Viewport.xmin = xl; Viewport.xmax = xh; \
	Viewport.ymin = yl; Viewport.ymax = yh; \
	gsetviewport(w, &Viewport )

show_logo()
{
	Gpoint mid;

	background(0);
	s_viewport( 1, 0.0, 1.0, 0.0, 0.8 );
	draw_ethernet(ws_id, 1);

	mid.x = .25;
	mid.y = .4;
	s_viewport(1,mid.x-0.1,mid.x+0.1,mid.y-0.08,mid.y+0.08);
	draw_9000(ws_id);

	mid.x = .31;
	mid.y = .6;
	s_viewport(1,mid.x-0.07435,mid.x+0.07435,mid.y-0.05895,mid.y+0.05895);
	draw_printer(ws_id);

	mid.x = .5;
	mid.y = .6;
	s_viewport(1,mid.x-0.1,mid.x+0.1,mid.y-0.08,mid.y+0.08);
	draw_9000(ws_id);

	mid.x = .69;
	mid.y = .6;
	s_viewport(1,mid.x-0.07435,mid.x+0.07435,mid.y-0.05895,mid.y+0.05895);
	draw_printer(ws_id);

	mid.x = .75;
	mid.y = .4;
	s_viewport(1,mid.x-0.1,mid.x+0.1,mid.y-0.08,mid.y+0.08);
	draw_9000(ws_id);

	mid.x = .69;
	mid.y = .25;
	s_viewport(1,mid.x-0.07435,mid.x+0.07435,mid.y-0.05895,mid.y+0.05895);
	draw_printer(ws_id);

	mid.x = .5;
	mid.y = .2;
	s_viewport(1,mid.x-0.1,mid.x+0.1,mid.y-0.08,mid.y+0.08);
	draw_9000(ws_id);

	mid.x = .31;
	mid.y = .25;
	s_viewport(1,mid.x-0.07435,mid.x+0.07435,mid.y-0.05895,mid.y+0.05895);
	draw_printer(ws_id);

	mid.x = .51;
	mid.y = .4;
	s_viewport(1,mid.x-0.10,mid.x+0.10,mid.y-0.08,mid.y+0.08);
	draw_ramtek(ws_id);

	mid.x = .5;
	mid.y = .4;
	s_viewport(1,mid.x-0.05,mid.x+0.05,mid.y-0.04,mid.y+0.04);
	draw_9000(ws_id);
}

show_printer()
{
	Gpoint mid;

	background(BLUE);
	mid.x = .5;
	mid.y = .4;
	s_viewport(1,mid.x-0.4,mid.x+0.4,mid.y-0.32,mid.y+0.32);
	draw_printer(ws_id);
}

show_ramtek()
{
	Gpoint mid;

	background(BLUE);
	mid.x = .5;
	mid.y = .4;
	s_viewport(1,mid.x-0.4,mid.x+0.4,mid.y-0.32,mid.y+0.32);
	draw_ramtek(ws_id);
}

show_9000()
{
	Gpoint mid;

	background(BLUE);

	mid.x = .5;
	mid.y = .4;
	s_viewport(1,mid.x-0.4,mid.x+0.4,mid.y-0.32,mid.y+0.32);
	draw_9000(ws_id);
}
