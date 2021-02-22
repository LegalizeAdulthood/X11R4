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
 *
 * printer.c - GKS function to draw the IBM Personal Computer Color Printer.
 */

#include <stdio.h>
#include <xgks.h>
#include "demo.h"

draw_printer(wsid)
Gint wsid;
{
        Glimit Window;

        Window.xmin = 0.0;
        Window.xmax = 58.0;
        Window.ymin = 0.0;
        Window.ymax = 46.0;

        gsetwindow( wsid, &Window );
        gselntran(1);

        do_printer();
}

do_printer()
{
        Gpoint pts[20];
        
        gsetfillintstyle(GSOLID);
        gsetfillcolorind(BEIGE);

        gsetlinecolorind(BLACK);
/* box */
        pts[0].x=0.0;  pts[0].y=0.0;
        pts[1].x=58.0; pts[1].y=0.0;
        pts[2].x=58.0; pts[2].y=18.0;
        pts[3].x=55.0; pts[3].y=18.0;
        pts[4].x=55.0; pts[4].y=25.0;
        pts[5].x=3.0;  pts[5].y=25.0;
        pts[6].x=3.0;  pts[6].y=18.0;
        pts[7].x=0.0;  pts[7].y=18.0;
        pts[8].x=0.0;  pts[8].y=0.0;
        gfillarea(9, pts);

/* lines on it */
        pts[0].x=3.0;  pts[0].y=18.0;
        pts[1].x=4.0; pts[1].y=18.0;
        pts[2].x=4.0; pts[2].y=14.0;
        pts[3].x=54.0; pts[3].y=14.0;
        pts[4].x=54.0; pts[4].y=18.0;
        pts[5].x=55.0;  pts[5].y=18.0;
        gpolyline(6, pts); 

        
/* paper ins an outs */
        gsetfillcolorind(BLACK);

        pts[0].x=5.5;  pts[0].y=0.0;
        pts[1].x=52.5; pts[1].y=0.0;
        pts[2].x=52.5; pts[2].y=4.0;
        pts[3].x=5.5; pts[3].y=4.0;
        pts[4].x=5.5; pts[4].y=0.0;
        gfillarea(5, pts); 

        pts[0].x=5.0;  pts[0].y=16.0;
        pts[1].x=53.0; pts[1].y=16.0;
        pts[2].x=53.0; pts[2].y=25.0;
        pts[3].x=5.0; pts[3].y=25.0;
        pts[4].x=5.0; pts[4].y=16.0;
        gfillarea(5, pts); 

/* control panel */
        gsetfillcolorind(DARKGRAY);

        pts[0].x=37.0; pts[0].y=4.0;
        pts[1].x=52.5; pts[1].y=4.0;
        pts[2].x=52.5; pts[2].y=11.5;
        pts[3].x=37.0; pts[3].y=11.5;
        pts[4].x=37.0; pts[4].y=4.0;
        gfillarea(5, pts); 

        gsetfillcolorind(BLACK);

        pts[0].x=38.0; pts[0].y=6.0;
        pts[1].x=47.5; pts[1].y=6.0;
        pts[2].x=47.5; pts[2].y=10.5;
        pts[3].x=38.0; pts[3].y=10.5;
        pts[4].x=38.0; pts[4].y=6.0;
        gfillarea(5, pts); 

        pts[0].x=48.5; pts[0].y=6.0;    /* on off switch */
        pts[1].x=51.0; pts[1].y=6.0;
        pts[2].x=51.0; pts[2].y=10.5;
        pts[3].x=48.5; pts[3].y=10.5;
        pts[4].x=48.5; pts[4].y=6.0;
        gfillarea(5, pts); 

        gsetfillcolorind(DARKGRAY);

        pts[0].x=39.0; pts[0].y=8.5;    /* text mode */
        pts[1].x=41.0; pts[1].y=8.5;
        pts[2].x=41.0; pts[2].y=10.0;
        pts[3].x=39.0; pts[3].y=10.0;
        pts[4].x=39.0; pts[4].y=8.5;
        gfillarea(5, pts); 

        pts[0].x=45.0; pts[0].y=8.5;    /* power */
        pts[1].x=46.5; pts[1].y=8.5;
        pts[2].x=46.5; pts[2].y=10.0;
        pts[3].x=45.0; pts[3].y=10.0;
        pts[4].x=45.0; pts[4].y=8.5;
        gfillarea(5, pts); 

        pts[0].x=43.0; pts[0].y=6.5;    /* forms mode */
        pts[1].x=44.5; pts[1].y=6.5;
        pts[2].x=44.5; pts[2].y=8.0;
        pts[3].x=43.0; pts[3].y=8.0;
        pts[4].x=43.0; pts[4].y=6.5;
        gfillarea(5, pts); 

        gsetfillcolorind(BLUE);

        pts[0].x=41.0; pts[0].y=8.5;    /* line feed */
        pts[1].x=42.5; pts[1].y=8.5;
        pts[2].x=42.5; pts[2].y=10.0;
        pts[3].x=41.0; pts[3].y=10.0;
        pts[4].x=41.0; pts[4].y=8.5;
        gfillarea(5, pts); 

        pts[0].x=41.0; pts[0].y=6.5;    /* form feed */
        pts[1].x=42.5; pts[1].y=6.5;
        pts[2].x=42.5; pts[2].y=8.0;
        pts[3].x=41.0; pts[3].y=8.0;
        pts[4].x=41.0; pts[4].y=6.5;
        gfillarea(5, pts); 

        gsetfillcolorind(YELLOW);

        pts[0].x=39.0; pts[0].y=6.5;    /* Initialize */
        pts[1].x=40.5; pts[1].y=6.5;
        pts[2].x=40.5; pts[2].y=8.0;
        pts[3].x=39.0; pts[3].y=8.0;
        pts[4].x=39.0; pts[4].y=6.5;
        gfillarea(5, pts); 

        pts[0].x=45.0; pts[0].y=6.5;    /* fault */
        pts[1].x=46.5; pts[1].y=6.5;
        pts[2].x=46.5; pts[2].y=8.0;
        pts[3].x=45.0; pts[3].y=8.0;
        pts[4].x=45.0; pts[4].y=6.5;
        gfillarea(5, pts); 

        gsetfillcolorind(GREEN);

        pts[0].x=43.0; pts[0].y=8.5;    /* Ready */
        pts[1].x=44.5; pts[1].y=8.5;
        pts[2].x=44.5; pts[2].y=10.0;
        pts[3].x=43.0; pts[3].y=10.0;
        pts[4].x=43.0; pts[4].y=8.5;
        gfillarea(5, pts); 

        gsetfillcolorind(RED);

        pts[0].x=49.0; pts[0].y=8.0;    /* on off switch */
        pts[1].x=50.5; pts[1].y=8.0;
        pts[2].x=50.5; pts[2].y=10.0;
        pts[3].x=49.0; pts[3].y=10.0;
        pts[4].x=49.0; pts[4].y=8.0;
        gfillarea(5, pts);

        gsetfillcolorind(SILVER);

        pts[0].x=5.0; pts[0].y=10.0;    /* IBM label */
        pts[1].x=7.5; pts[1].y=10.0;
        pts[2].x=7.5; pts[2].y=12.5;
        pts[3].x=5.0; pts[3].y=12.5;
        pts[4].x=5.0; pts[4].y=10.0;
        gfillarea(5, pts);
}
