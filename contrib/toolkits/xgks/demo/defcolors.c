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
 */
static char *rcsid = "$Header: defcolors.c,v 4.0 89/09/01 14:23:32 amy Exp $";

#include <stdio.h>
#include <xgks.h>
#include "demo.h"

FillBox( org, wid, ht, color )
	Gpoint *org;
	Gfloat wid, ht;
	Gint color;
{
	Gpoint pts[5];
	Gchar num[5];

	pts[0].x = org->x;		pts[0].y = org->y;
	pts[1].x = org->x;		pts[1].y = org->y + ht;
	pts[2].x = org->x + wid;	pts[2].y = org->y + ht;
	pts[3].x = org->x + wid;	pts[3].y = org->y;
	pts[4].x = org->x;		pts[4].y = org->y;

	gsetfillcolorind( color );
	gfillarea( 5, pts );

	sprintf(num, "%d", color );
	pts[0].x = org->x + wid/2.0;
	pts[0].y = org->y + ht/2.0;
	gtext( pts, num );
}

main( argc, argv )
	int argc;
	char *argv[];
{
	Gint ws_id = 1;
	char *conn = (char *)NULL;
	Glimit Window;		/*MIT*/
	Glimit WsWindow;	/*MIT*/
	Gpoint org;
	Gint color;
	Gtxfp txfp;
	Gtxalign txalign;

	int i;

	Window.xmin = 0.0;		/*MIT*/
	Window.xmax = 1279.0;		/*MIT*/
	Window.ymin = 0.0;		/*MIT*/
	Window.ymax = 1023.0;		/*MIT*/

	WsWindow.xmin = 0.0;		/*MIT*/
	WsWindow.xmax = 1.0;		/*MIT*/
	WsWindow.ymin = 0.0;		/*MIT*/
	WsWindow.ymax = 0.8;		/*MIT*/

	txfp.font = 1;			/*MIT*/
	txfp.prec = GSTROKE;		/*MIT*/

	txalign.hor = GTH_CENTER;	/*MIT*/
	txalign.ver = GTV_HALF;		/*MIT*/

	for( i=1; i<argc; i++){
		if (index( argv[i], ':'))
			conn = argv[i];
	/* Application dependent options here */
	}

	gopengks(stdout,0);
	gopenws(ws_id, conn, conn);
	gactivatews(ws_id);

	gsetwindow( 1, &Window );
	gsetviewport( 1, &WsWindow );
	gselntran( 1 );
	gsetwswindow( ws_id, &WsWindow );

	gsetfillintstyle( GSOLID );

	gsettextfontprec( &txfp );
	gsetcharheight( 28.0 );
	gsetcharspace( 0.2 );
	gsettextalign( &txalign );
	gsettextcolorind( 0 );

	color = 0;
	for( org.y = 0.0; org.y < 1023.0; org.y += 64.0 )
	    for( org.x = 0.0; org.x < 1280.0; org.x += 80.0 )
		FillBox( &org, 80.0, 64.0, color++);

	WaitForBreak( ws_id );

	gdeactivatews(ws_id);
	gclosews(ws_id);
	gclosegks();
}
