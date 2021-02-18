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
static char rcsid[] = "$Header: usa.c,v 4.0 89/09/01 14:23:58 amy Exp $";

#include <stdio.h>
#include <strings.h>		/*MIT*/
#include <xgks.h>
#include "demo.h"

/* input format should look like ....[ usa sherman:0.0 ] */

Gint   ws_id=1;
Gint result;

main(argc,argv)
int argc;
char *argv[];
{
	double atof();

        Glimit w;	/*MIT*/
        Glimit v;	/*MIT*/
        Glimit wsw;	/*MIT*/	
        Glimit wsv;	/*MIT*/

	char *conn = (char *)NULL;
	int i;

	for( i=1; i<argc; i++){
		if (index( argv[i], ':'))
			conn = argv[i];
	/* Application dependent options here */
	}

	if ((result = gopengks(stdout,0)) != 0) 
		perr(result,"...open_gks");

	if ((result = gopenws(ws_id, conn, argv[1])) != 0)
		perr(result, "...open_ws");

	if ((result = gactivatews(ws_id)) != 0)
		perr(result, "...activate_ws");

	/* i_max_display_size( ws_type, &result, &units, dc_size, raster_size); */

	w.xmin = 803770.5;	/*MIT*/
	w.xmax = 8103770.5;	/*MIT*/
	w.ymin = -903359.0;	/*MIT*/
	w.ymax = 4287752.0;	/*MIT*/
	gsetwindow(1, &w);

	v.xmin = 0.0;		/*MIT*/
	v.xmax = 1.0;		/*MIT*/
	v.ymin = 0.0;		/*MIT*/
	v.ymax = 1024.0/1280.0;	/*MIT*/
	gsetviewport(1, &v); 

	gselntran(1);
	wsw.xmin = 0.0;			/*MIT*/
	wsw.xmax = 1.0;			/*MIT*/
	wsw.ymin = 0.0;			/*MIT*/
	wsw.ymax = 1024.0/1280.0;	/*MIT*/
	gsetwswindow(ws_id, &wsw); 

	wsv.xmin = 0.0;			/*MIT*/
	wsv.xmax = 1279.0;		/*MIT*/
	wsv.ymin = 0.0;			/*MIT*/
	wsv.ymax = 1023.0;		/*MIT*/
	gsetwsviewport(ws_id, &wsv); 

	PlotMap();

	WaitForBreak (1);

	if ((result = gdeactivatews(ws_id)) != 0)
		perr(result, "...deactivate_ws");

	if ((result = gclosews(ws_id)) != 0)
		perr(result, "...close_ws");

	if ((result = gclosegks()) != 0)
		perr(result,"...close_gks");
}

perr(i,s)
int i;
char *s;
{
	fprintf(stdout,"%s %d\n",s,i);
	exit(1);
}

/*
 * Written 10:17 pm  Jun 26, 1987 by rassilon@eddie.MIT.EDU in uiucdcsm:comp.graphics 
 * The following is a map of the United States in the form of a mercator
 * projection.  There are approximately 2700 points here.  The first number
 * is either a 2 or a 3 where 3 means pen up and 2 means pen down.
 */

PlotMap()
{
	FILE *pointsfile;
	Gpoint pts[200], *pt;
	Glong ud, x, y, cnt;

	if ((pointsfile = fopen("usa.points", "r")) == NULL) {
		fprintf(stderr, "usa: Can't open usa.points\n");
		exit(0);
	}

	if ((result=gsetlinecolorind(RED)) != 0)
		perr(result, "PlotMap");  
	
	gsetlinewidth( 0.0 );
	cnt=0;
	pt = pts;
	while ( fscanf(pointsfile, "%ld %ld %ld", &ud, &x, &y) == 3)	/*MIT*/ 
		switch( ud ) {
		case 2:
			pt->x = x; (pt++)->y = y;
			cnt++;
			break;
		case 3:
			if (cnt>1)
				gpolyline(cnt, pts);
			pts[0].x = x; pts[0].y = y;
			cnt = 1;
			pt = &(pts[1]);
			break;
		}
	if (cnt>1)
		gpolyline(cnt, pts);
	fclose(pointsfile);
}

minmax()
{
	Gint ud, x, X, y, Y;
	Gint ix, iy;

	x = 99999999;
	X = -99999999;
	y = 99999999;
	Y = -99999999;

	while (scanf("%d %d %d", &ud, &ix, &iy) == 3) {
		if (ix<x)	x = ix;
		if (ix>X)	X = ix;
		if (iy<y)	y = iy;
		if (iy>Y)	Y = iy;
	}
	printf("x %d X %d y %d Y %d\n", x, X, y, Y);
}
