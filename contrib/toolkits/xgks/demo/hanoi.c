/*
 *              Copyright IBM Corporation 1989
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
 * Urbana, IL   61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 *      All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 */
static char *rcsid = "$Header: hanoi.c,v 4.0 89/09/01 14:23:40 amy Exp $";

/*
 * solve "tower of hanoi" problem for n disks
 * with graphical narration of solution
 */
#include <stdio.h>
#include <math.h>
#include <xgks.h>
#include "demo.h"

#define TOWERSPC 9.0
#define WINDWD 40.0
#define WINDHT WINDWD*0.75

/*
 * default values for command line options
 */
int n = 5 ;

main(argc, argv)
        int argc ;
        char *argv[] ;
{
        Glimit WsWindow;        /*MIT*/
        Gint ws_id = 1;
        Gint mo_id = 3;
        char *conn = (char *)NULL;
        int i;

        WsWindow.xmin = 0.0;    /*MIT*/
        WsWindow.xmax = 1.0;    /*MIT*/
        WsWindow.ymin = 0.0;    /*MIT*/
        WsWindow.ymax = 0.8;    /*MIT*/

        for( i=1; i<argc; i++){
                if (index( argv[i], ':'))
                        conn = argv[i];
        /* Application dependent options here */
                else
                        n = atoi(argv[i]);
        }

        gopengks(stdout,0);

        gopenws(ws_id, conn, conn);
        gactivatews(ws_id);

        gopenws(mo_id, "hanoi.out", "MO");
        gactivatews(mo_id);

        gsetwswindow( ws_id, &WsWindow );
        gsetwswindow( mo_id, &WsWindow );

        title() ;

/*
 * solve the problem
 */
        inittower(n) ;
        f(n, 0, 1, 2) ;
/*
 * close workstation and GKS
 */

        WaitForBreak( ws_id );
        gdeactivatews(mo_id);
        gdeactivatews(ws_id);
        gclosews(mo_id);
        gclosews(ws_id);
        gclosegks();
}

/*
 * transfer n disks from tower a to tower b using c as a spare
 *
 *              transfer n-1 from a to c using b
 *              move 1 from a to b
 *              transfer n-1 from c to b using a
 */
f(n, a, b, c)
int n ;
int a ;
int b ;
int c ;
{
        if(n == 0)
                return ;

        f(n-1, a, c, b) ;
        movedisk(a,b) ;
        f(n-1, c, b, a) ;
}

box( l )
        Glimit *l;
{
        Gpoint pts[5];
#define e 0.01

        pts[0].x = l->xmin+e; pts[0].y = l->ymin+e;
        pts[1].x = l->xmin+e; pts[1].y = l->ymax-e;
        pts[2].x = l->xmax-e; pts[2].y = l->ymax-e;
        pts[3].x = l->xmax-e; pts[3].y = l->ymin+e;
        pts[4].x = l->xmin+e; pts[4].y = l->ymin+e;
        gsetfillcolorind( 1 );
        gfillarea( 5, pts );

}
/*
 * print title across top of page
 */
title()
{
        Gpoint p ;
        Glimit Window;          /*MIT*/
        Glimit Viewport;        /*MIT*/
        Gtxfp txfp;             /*MIT*/
        Gtxalign txalign;       /*MIT*/

        Window.xmin = 0.0;      /*MIT*/
        Window.xmax = 16.0;     /*MIT*/
        Window.ymin = 0.0;      /*MIT*/
        Window.ymax = 2.0;      /*MIT*/

        Viewport.xmin = 0.1;    /*MIT*/
        Viewport.xmax = 0.9;    /*MIT*/
        Viewport.ymin = 0.58;   /*MIT*/
        Viewport.ymax = 0.74;   /*MIT*/

        txfp.font = 2;          /*MIT*/
        txfp.prec = GSTROKE;    /*MIT*/

        txalign.hor = GTH_CENTER;       /*MIT*/
        txalign.ver = GTV_HALF;         /*MIT*/

        gsetdeferst(1, GASAP, GALLOWED);
        gsetwindow(1, &Window );
        gsetviewport(1, &Viewport);
        gselntran(1) ;

/*      box( &Window ); */

        txfp.font = 1;  txfp.prec = GSTROKE;
        gsettextfontprec( &txfp );
        gsetcharspace(0.2);
        gsetcharheight(1.0);
        gsettextalign( &txalign );
        gsettextcolorind( YELLOW ) ;

        p.x = 8.0 ;
        p.y = 1.0 ;
        gtext( &p, "Tower of Hanoi") ;
}

/*
 * initialize towers with all disks on tower 0.
 */
inittower(n)
int n ;
{
        Glimit Window;          /*MIT*/
        Glimit Viewport;        /*MIT*/
        int i ;

        Window.xmin = 0.0;      /*MIT*/
        Window.xmax = WINDWD;   /*MIT*/
        Window.ymin = 0.0;      /*MIT*/
        Window.ymax = WINDHT;   /*MIT*/

        Viewport.xmin = 0.1;    /*MIT*/
        Viewport.xmax = 0.9;    /*MIT*/
        Viewport.ymin =  0.06;  /*MIT*/
        Viewport.ymax =  0.54;  /*MIT*/

        gsetwindow(1, &Window );
        gsetviewport(1, &Viewport);
        border(0.0, WINDWD, 0.0, WINDHT) ;

        for(i=n; i>0; i--)
                placedisk(0, i) ;
}

/*
 * tower data structures and manipulation
 */

Gfloat tcount[3] = {0.2, 0.2, 0.2} ;

int towers[3][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};   /*MIT*/

int towerx[3] = {0, 0, 0} ;

movedisk(a,b)
{
        int diskno ;

        path(a,b) ;
        diskno = remove(a) ;
        unpath() ;
        placedisk(b, diskno) ;
}

placedisk(tower, diskno)
int tower, diskno ;
{
        gsetfillcolorind(100 + diskno) ;
        gsetfillintstyle(GSOLID) ;

        disk(diskno, TOWERSPC * (1 + tower) - (Gfloat)(diskno)/2.0, tcount[tower]) ;
        tcount[tower] += (Gfloat)diskno ;
        pushdisk(tower, diskno) ;
}

remove(tower)
int tower ;
{
        int diskno ;

        diskno = popdisk(tower) ;
        gsetfillcolorind( 0 );
        gsetfillintstyle( GSOLID );

        tcount[tower] -= (Gfloat)diskno ;
        disk(diskno, TOWERSPC * (1 + tower) - (Gfloat)(diskno)/2.0, tcount[tower]) ;
        return(diskno) ;
}

disk(diskno, x, y)
        int diskno ;
        Gfloat x, y ;
{
        Gpoint pts[4] ;

#ifdef DEBUG
        printf("disk(%d, %8.3f, %8.3f\n", diskno, x, y) ;
#endif
        pts[0].x = x ;
        pts[0].y = y ;
        pts[1].x = x ;
        pts[1].y = y + (Gfloat)diskno ;
        pts[2].x = x + (Gfloat)diskno ;
        pts[2].y = y + (Gfloat)diskno ;
        pts[3].x = x + (Gfloat)diskno ;
        pts[3].y = y ;

        gfillarea(4, pts) ;
}

Gpoint pathpts[4] = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}} ;  /*MIT*/

/*
 * draw a line from top disk of tower a to top of screen, then over
 * to tower b.
 */
path(a,b)
int a,b ;
{
        gsetlinecolorind( GREEN ) ;
        gsetlinetype( GLN_DASH) ;
        gsetlinewidth(3.0) ;
        pathpts[0].x = (a + 1)*TOWERSPC ;
        pathpts[0].y = tcount[a] + (WINDHT * .05) ;
        pathpts[1].x = (a + 1)*TOWERSPC ;
        pathpts[1].y = WINDHT - 1.0 ;
        pathpts[2].x = (b + 1)*TOWERSPC ;
        pathpts[2].y = WINDHT - 1.0 ;
        pathpts[3].x = (b + 1)*TOWERSPC ;
        pathpts[3].y = tcount[b] + (WINDHT * .05) ;

        gpolyline(4, pathpts) ;
}

unpath()
{
        gsetlinecolorind(0) ;
        gpolyline(4, pathpts) ;
}


border(x1, x2, y1, y2)
        Gfloat x1, x2, y1, y2 ;
{
        Gpoint pts[5] ;

        gsetlinecolorind(8) ;
        gsetlinetype(GSOLID) ;
        gsetlinewidth(2.0) ;
        pts[0].x = x1 ;
        pts[0].y = y1 ;
        pts[1].x = x1 ;
        pts[1].y = y2 ;
        pts[2].x = x2 ;
        pts[2].y = y2 ;
        pts[3].x = x2 ;
        pts[3].y = y1 ;
        pts[4].x = x1 ;
        pts[4].y = y1 ;

        gpolyline(5, pts) ;
}

pushdisk(tower, diskno)
int tower, diskno ;
{
        towers[tower][towerx[tower]] = diskno ;
        towerx[tower]++ ;
}

popdisk(tower)
int tower ;
{

        towerx[tower]-- ;
        return(towers[tower][towerx[tower]]) ;
}


