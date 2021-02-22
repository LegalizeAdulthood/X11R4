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
 * Draw a picture of the IBM Instruments S9000 workstation.
 *
 */
#include <stdio.h>
#include <xgks.h>
#include "demo.h"

draw_9000(wsid)
        Gint wsid;
{
        Glimit Window;

        Window.xmin = 0.0;
        Window.xmax = 78.0;
        Window.ymin = 0.0;
        Window.ymax = 62.4;

        gsetwindow( wsid, &Window );
        gselntran(1);

        do_9000_computer();
        do_9000_monitor();
        do_9000_disk();
        do_9000_keyboard();

}


do_9000_computer()
{
        Gpoint pts[20];
        
        gsetfillintstyle(GSOLID);
        gsetfillcolorind(BEIGE);

        gsetlinecolorind(BLACK);
/* box */
        pts[0].x=31.0; pts[0].y=16.0;
        pts[1].x=74.0; pts[1].y=16.0;
        pts[2].x=74.0; pts[2].y=31.5;
        pts[3].x=31.0; pts[3].y=31.5;
        pts[4].x=31.0; pts[4].y=16.0;
        gfillarea(5, pts);

/* left leg */
        pts[0].x=30.0; pts[0].y=14.0;
        pts[1].x=34.0; pts[1].y=14.0;
        pts[2].x=34.0; pts[2].y=16.0;
        pts[3].x=31.0; pts[3].y=16.0;
        pts[4].x=31.0; pts[4].y=29.0;
        pts[5].x=30.0; pts[5].y=29.0;
        pts[6].x=30.0; pts[6].y=14.0;
        gfillarea(7, pts);
        gpolyline(7, pts);

/* right leg */
        pts[0].x=71.0; pts[0].y=14.0;
        pts[1].x=75.0; pts[1].y=14.0;
        pts[2].x=75.0; pts[2].y=29.0;
        pts[3].x=74.0; pts[3].y=29.0;
        pts[4].x=74.0; pts[4].y=16.0;
        pts[5].x=71.0; pts[5].y=16.0;
        pts[6].x=71.0; pts[6].y=14.0;
        gfillarea(7, pts);
        gpolyline(7, pts);

/* switch */
        gsetfillcolorind(RED);
        pts[0].x=74.0; pts[0].y=27.0;
        pts[1].x=75.0; pts[1].y=27.0;
        pts[2].x=75.0; pts[2].y=29.0;
        pts[3].x=74.0; pts[3].y=29.0;
        pts[4].x=74.0; pts[4].y=27.0;
        gfillarea(5, pts);

/* IBM label */
        gsetfillcolorind(SILVER);
        pts[0].x=33.0; pts[0].y=28.0;
        pts[1].x=39.0; pts[1].y=28.0;
        pts[2].x=39.0; pts[2].y=30.0;
        pts[3].x=33.0; pts[3].y=30.0;
        pts[4].x=33.0; pts[4].y=28.0;
        gfillarea(5, pts);
}

do_9000_monitor()
{
        Gpoint pts[20];
        Gfloat x;

        gsetfillcolorind(BEIGE);
        
        gsetfillintstyle(GSOLID);

        gsetlinecolorind(BLACK);

/* base */
        pts[0].x=31.0;  pts[0].y=31.5;
        pts[1].x=74.0;  pts[1].y=31.5;
        pts[2].x=74.0;  pts[2].y=33.0;
        pts[3].x=72.5;  pts[3].y=33.0;
        pts[4].x=72.5;  pts[4].y=34.0;
        pts[5].x=71.5;  pts[5].y=34.0;
        pts[6].x=71.5;  pts[6].y=33.0;
        pts[7].x=33.5;  pts[7].y=33.0;
        pts[8].x=33.5;  pts[8].y=34.0;
        pts[9].x=32.5;  pts[9].y=34.0;
        pts[10].x=32.5; pts[10].y=33.0;
        pts[11].x=31.0; pts[11].y=33.0;
        pts[12].x=31.0; pts[12].y=31.5;
        gfillarea(13, pts);
        gpolyline(13, pts);

/* monitor box */
        pts[0].x=31.5;  pts[0].y=36.0;
        pts[1].x=73.0;  pts[1].y=36.0;
        pts[2].x=73.0;  pts[2].y=62.0;
        pts[3].x=31.5;  pts[3].y=62.0;
        pts[4].x=31.5;  pts[4].y=36.0;
        gfillarea(5, pts);
        gpolyline(5, pts);

/* bezel */
        gsetfillcolorind(MEDIUMGRAY);
        pts[0].x=35.0;  pts[0].y=39.0;
        pts[1].x=59.0;  pts[1].y=39.0;
        pts[2].x=59.0;  pts[2].y=60.0;
        pts[3].x=35.0;  pts[3].y=60.0;
        pts[4].x=35.0;  pts[4].y=39.0;
        gfillarea(5, pts);
        gpolyline(5, pts);

/* crt */
        gsetfillcolorind(DARKGREEN);
        pts[0].x=36.5;  pts[0].y=42.0;
        pts[1].x=57.5;  pts[1].y=42.0;
        pts[2].x=57.5;  pts[2].y=58.5;
        pts[3].x=36.5;  pts[3].y=58.5;
        pts[4].x=36.5;  pts[4].y=42.0;
        gfillarea(5, pts);
        gpolyline(5, pts);

/* buttons */
        gsetfillcolorind(WHITE);
        for (x=37.5; x<56.0; x+=2.0) {
                pts[0].x=x;             pts[0].y=40.5;
                pts[1].x=x+1.0; pts[1].y=40.5;
                pts[2].x=x+1.0; pts[2].y=41.5;
                pts[3].x=x;             pts[3].y=41.5;
                pts[4].x=x;             pts[4].y=40.5;
                gfillarea(5, pts);
                gpolyline(5, pts);
        }

/* floppy */
        pts[0].x=62.0;  pts[0].y=42.0;
        pts[1].x=71.0;  pts[1].y=42.0;
        pts[2].x=71.0;  pts[2].y=57.5;
        pts[3].x=62.0;  pts[3].y=57.5;
        pts[4].x=62.0;  pts[4].y=42.0;
        gpolyline(5, pts);

/* IBM label */
        gsetfillcolorind(SILVER);
        pts[0].x=63.5;  pts[0].y=59.0;
        pts[1].x=69.5;  pts[1].y=59.0;
        pts[2].x=69.5;  pts[2].y=61.0;
        pts[3].x=63.5;  pts[3].y=61.0;
        pts[4].x=63.5;  pts[4].y=59.0;
        gfillarea(5, pts);

/* yoke */
        gsetfillcolorind(BLACK);
        pts[0].x=47.0;  pts[0].y=33.5;
        pts[1].x=49.0;  pts[1].y=33.5;
        pts[2].x=50.0;  pts[2].y=33.0;
        pts[3].x=55.0;  pts[3].y=33.0;
        pts[4].x=56.0;  pts[4].y=33.5;
        pts[5].x=58.0;  pts[5].y=33.5;
        pts[6].x=58.0;  pts[6].y=35.5;
        pts[7].x=56.0;  pts[7].y=35.5;
        pts[8].x=55.0;  pts[8].y=36.0;
        pts[9].x=50.0;  pts[9].y=36.0;
        pts[10].x=49.0; pts[10].y=35.5;
        pts[11].x=47.0; pts[11].y=35.5;
        pts[12].x=47.0; pts[12].y=33.5;
        gfillarea(13, pts);
}

do_9000_disk()
{
        Gpoint pts[20];

        gsetfillcolorind(0);
        
        gsetfillintstyle(GSOLID);

        gsetlinecolorind(BEIGE);

/* hard disk */
        pts[0].x=6.0;   pts[0].y=38.0;
        pts[1].x=6.0;   pts[1].y=60.0;
        pts[2].x=21.0;  pts[2].y=60.0;
        pts[3].x=21.0;  pts[3].y=38.0;
        pts[4].x=6.0;   pts[4].y=38.0;
        gfillarea(5, pts);
        gpolyline(4, pts);

/* left foot */
        pts[0].x=8.0;   pts[0].y=38.0;
        pts[1].x=8.0;   pts[1].y=37.0;
        pts[2].x=9.0;   pts[2].y=37.0;
        pts[3].x=9.0;   pts[3].y=38.0;
        pts[4].x=8.0;   pts[4].y=38.0;
        gfillarea(5, pts);

/* right foot */
        pts[0].x=18.0;  pts[0].y=38.0;
        pts[1].x=18.0;  pts[1].y=37.0;
        pts[2].x=19.0;  pts[2].y=37.0;
        pts[3].x=19.0;  pts[3].y=38.0;
        pts[4].x=18.0;  pts[4].y=38.0;
        gfillarea(5, pts);

/* CAUTION label*/
        gsetfillcolorind(WHITE);
        pts[0].x= 9.5; pts[0].y=39.0;
        pts[1].x=14.0; pts[1].y=39.0;
        pts[2].x=14.0; pts[2].y=42.0;
        pts[3].x= 9.5; pts[3].y=42.0;
        pts[4].x= 9.5; pts[4].y=39.0;
        gfillarea(5, pts);

/* IBM label*/
        gsetfillcolorind(SILVER);
        pts[0].x=14.5; pts[0].y=40.5;
        pts[1].x=20.0; pts[1].y=40.5;
        pts[2].x=20.0; pts[2].y=42.0;
        pts[3].x=14.5; pts[3].y=42.0;
        pts[4].x=14.5; pts[4].y=40.5;
        gfillarea(5, pts);

/* switch */
        gsetfillcolorind(RED);
        pts[0].x=7.5; pts[0].y=39.5;
        pts[1].x=9.0; pts[1].y=39.5;
        pts[2].x=9.0; pts[2].y=42.0;
        pts[3].x=7.5; pts[3].y=42.0;
        pts[4].x=7.5; pts[4].y=39.5;
        gfillarea(5, pts);

/* bottom led */
        pts[0].x=9.0; pts[0].y=45.0;
        pts[1].x=10.0; pts[1].y=45.0;
        pts[2].x=10.0; pts[2].y=45.5;
        pts[3].x=9.0; pts[3].y=45.5;
        pts[4].x=9.0; pts[4].y=45.0;
        gfillarea(5, pts);

/* top led */
        pts[0].x=9.0; pts[0].y=53.5;
        pts[1].x=10.0; pts[1].y=53.5;
        pts[2].x=10.0; pts[2].y=54.0;
        pts[3].x=9.0; pts[3].y=54.0;
        pts[4].x=9.0; pts[4].y=53.5;
        gfillarea(5, pts);

/* floppies*/
        gsetfillcolorind(BLACK);
        
        gsetfillintstyle(GSOLID);

        gsetlinecolorind(BEIGE);

        pts[0].x=1.0;   pts[0].y=15.0;
        pts[1].x=1.0;   pts[1].y=37.0;
        pts[2].x=26.0;  pts[2].y=37.0;
        pts[3].x=26.0;  pts[3].y=15.0;
        pts[4].x=1.0;   pts[4].y=15.0;
        gfillarea(5, pts);
        gpolyline(4, pts);

/* left foot */
        pts[0].x=4.0;   pts[0].y=15.0;
        pts[1].x=4.0;   pts[1].y=14.0;
        pts[2].x=5.0;   pts[2].y=14.0;
        pts[3].x=5.0;   pts[3].y=15.0;
        pts[4].x=4.0;   pts[4].y=15.0;
        gfillarea(5, pts);

/* right foot */
        pts[0].x=22.0;  pts[0].y=15.0;
        pts[1].x=22.0;  pts[1].y=14.0;
        pts[2].x=23.0;  pts[2].y=14.0;
        pts[3].x=23.0;  pts[3].y=15.0;
        pts[4].x=22.0;  pts[4].y=15.0;
        gfillarea(5, pts);

/* drive doors */
        pts[0].x= 6.0;  pts[0].y=15.0;
        pts[1].x= 7.0;  pts[1].y=15.0;
        pts[2].x= 7.0;  pts[2].y=37.0;
        pts[3].x= 6.0;  pts[3].y=37.0;
        pts[4].x= 6.0;  pts[4].y=15.0;
        gpolyline(5, pts);

/*
        pts[0].x=13.0;  pts[0].y=15.0;
        pts[1].x=14.0;  pts[1].y=15.0;
        pts[2].x=14.0;  pts[2].y=37.0;
        pts[3].x=13.0;  pts[3].y=37.0;
        pts[4].x=13.0;  pts[4].y=15.0;
        gpolyline(5, pts);
*/

        pts[0].x=19.0;  pts[0].y=15.0;
        pts[1].x=20.0;  pts[1].y=15.0;
        pts[2].x=20.0;  pts[2].y=37.0;
        pts[3].x=19.0;  pts[3].y=37.0;
        pts[4].x=19.0;  pts[4].y=15.0;
        gpolyline(5, pts);

        pts[0].x=8.5;   pts[0].y=24.0;
        pts[1].x=9.5;   pts[1].y=24.0;
        pts[2].x=9.5;   pts[2].y=29.0;
        pts[3].x=8.5;   pts[3].y=29.0;
        pts[4].x=8.5;   pts[4].y=24.0;
        gpolyline(5, pts);

        pts[0].x=21.5;  pts[0].y=24.0;
        pts[1].x=22.5;  pts[1].y=24.0;
        pts[2].x=22.5;  pts[2].y=29.0;
        pts[3].x=21.5;  pts[3].y=29.0;
        pts[4].x=21.5;  pts[4].y=24.0;
        gpolyline(5, pts);

}

do_9000_keyboard()
{
        Gpoint pts[20];
        Gfloat x, y;

        gsetfillcolorind(BEIGE);
        
        gsetfillintstyle(GSOLID);

        gsetlinecolorind(BLACK);

/* keyboard case */
        pts[0].x=30.0;  pts[0].y=0.0;
        pts[1].x=77.0;  pts[1].y=0.0;
        pts[2].x=76.0;  pts[2].y=10.0;
        pts[3].x=74.5;  pts[3].y=10.0;
        pts[4].x=74.5;  pts[4].y=13.0;
        pts[5].x=32.5;  pts[5].y=13.0;
        pts[6].x=32.5;  pts[6].y=10.0;
        pts[7].x=31.0;  pts[7].y=10.0;
        pts[8].x=30.0;  pts[8].y=0.0;
        gfillarea(9, pts);
        gpolyline(9, pts);

/* function key case */
        pts[0].x=32.5;  pts[0].y=10.0;
        pts[1].x=32.5;  pts[1].y=8.0;
        pts[2].x=74.5;  pts[2].y=8.0;
        pts[3].x=74.5;  pts[3].y=10.0;
        gpolyline(4, pts);

/* all function keys*/
        gsetfillcolorind(WHITE);
        pts[0].x=35.5;  pts[0].y=9.0;
        pts[1].x=71.5;  pts[1].y=9.0;
        pts[2].x=71.5;  pts[2].y=12.0;
        pts[3].x=35.5;  pts[3].y=12.0;
        pts[4].x=35.5;  pts[4].y=9.0;
        gfillarea(5, pts);

/* outline them keys in black */
        for (x=35.5; x<72.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=9.0;
                pts[1].x = x;   pts[1].y=12.0;
                gpolyline(2, pts);
        }
        for (y=9.0; y<13.0; y++) {
                pts[0].x = 35.5;        pts[0].y=y;
                pts[1].x = 71.5;        pts[1].y=y;
                gpolyline(2, pts);
        }


/* function keys */
        gsetfillcolorind(WHITE);
        pts[0].x=33.0;  pts[0].y=2.0;
        pts[1].x=37.0;  pts[1].y=2.0;
        pts[2].x=37.0;  pts[2].y=7.0;
        pts[3].x=33.0;  pts[3].y=7.0;
        pts[4].x=33.0;  pts[4].y=2.0;
        gfillarea(5, pts);

        for (x=33.0; x<38.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=2.0;
                pts[1].x = x;   pts[1].y=7.0;
                gpolyline(2, pts);
        }
        for (y=2.0; y<8.0; y++) {
                pts[0].x = 33.0;        pts[0].y=y;
                pts[1].x = 37.0;        pts[1].y=y;
                gpolyline(2, pts);
        }
        
/* esc to alt column */
        pts[0].x=37.5;  pts[0].y=2.0;
        pts[1].x=41.5;  pts[1].y=2.0;
        pts[2].x=41.5;  pts[2].y=3.0;
        pts[3].x=40.0;  pts[3].y=3.0;
        pts[4].x=40.0;  pts[4].y=4.0;
        pts[5].x=41.0;  pts[5].y=4.0;
        pts[6].x=41.0;  pts[6].y=5.0;
        pts[7].x=40.0;  pts[7].y=5.0;
        pts[8].x=40.0;  pts[8].y=6.0;
        pts[9].x=39.0;  pts[9].y=6.0;
        pts[10].x=39.0; pts[10].y=7.0;
        pts[11].x=37.5; pts[11].y=7.0;
        pts[12].x=37.5; pts[12].y=2.0;
        gfillarea(13, pts);

/* gray right keys */
        pts[0].x=60.5;  pts[0].y=2.0;
        pts[1].x=64.5;  pts[1].y=2.0;
        pts[2].x=64.5;  pts[2].y=3.0;
        pts[3].x=66.5;  pts[3].y=3.0;
        pts[4].x=66.5;  pts[4].y=6.0;
        pts[5].x=72.5;  pts[5].y=6.0;
        pts[6].x=72.5;  pts[6].y=2.0;
        pts[7].x=74.5;  pts[7].y=2.0;
        pts[8].x=74.5;  pts[8].y=7.0;
        pts[9].x=62.5;  pts[9].y=7.0;
        pts[10].x=62.5; pts[10].y=6.0;
        pts[11].x=64.5; pts[11].y=6.0;
        pts[12].x=64.5; pts[12].y=4.0;
        pts[13].x=62.0; pts[13].y=4.0;
        pts[14].x=62.0; pts[14].y=3.0;
        pts[15].x=60.5; pts[15].y=3.0;
        pts[16].x=60.5; pts[16].y=2.0;
        gfillarea(17, pts);

/* outline keys */
        for (y=2.0; y<8.0; y++) {
                pts[0].x = 37.5;        pts[0].y=y;
                pts[1].x = 74.5;        pts[1].y=y;
                gpolyline(2, pts);
        }

/* fix up a couple of spots */
        gsetlinecolorind(MEDIUMGRAY);
        pts[0].x = 72.5;        pts[0].y=3.0;
        pts[1].x = 74.5;        pts[1].y=3.0;
        gpolyline(2, pts);
        pts[0].x = 72.5;        pts[0].y=4.0;
        pts[1].x = 74.5;        pts[1].y=4.0;
        gpolyline(2, pts);
        pts[0].x = 64.5;        pts[0].y=5.0;
        pts[1].x = 66.5;        pts[1].y=5.0;
        gpolyline(2, pts);

        gsetlinecolorind(BLACK);
/* alt row */
        pts[0].x = 41.5;        pts[0].y=2.0;
        pts[1].x = 41.5;        pts[1].y=3.0;
        gpolyline(2, pts);

        pts[0].x = 60.5;        pts[0].y=2.0;
        pts[1].x = 60.5;        pts[1].y=3.0;
        gpolyline(2, pts);

        pts[0].x = 64.5;        pts[0].y=2.0;
        pts[1].x = 64.5;        pts[1].y=3.0;
        gpolyline(2, pts);

        pts[0].x = 68.5;        pts[0].y=2.0;
        pts[1].x = 68.5;        pts[1].y=3.0;
        gpolyline(2, pts);

        pts[0].x = 72.5;        pts[0].y=2.0;
        pts[1].x = 72.5;        pts[1].y=3.0;
        gpolyline(2, pts);

/* shift row */
        for (x=40.0; x<63.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=3.0;
                pts[1].x = x;   pts[1].y=4.0;
                gpolyline(2, pts);
        }
        for (x=64.5; x<73.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=3.0;
                pts[1].x = x;   pts[1].y=4.0;
                gpolyline(2, pts);
        }

/* ctrl row */
        for (x=41.0; x<64.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=4.0;
                pts[1].x = x;   pts[1].y=5.0;
                gpolyline(2, pts);
        }
        for (x=64.5; x<73.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=4.0;
                pts[1].x = x;   pts[1].y=5.0;
                gpolyline(2, pts);
        }

/* tab row */
        for (x=40.0; x<63.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=5.0;
                pts[1].x = x;   pts[1].y=6.0;
                gpolyline(2, pts);
        }
        for (x=64.5; x<73.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=5.0;
                pts[1].x = x;   pts[1].y=6.0;
                gpolyline(2, pts);
        }

/* esc row */
        for (x=39.0; x<62.0; x+=2.0) {
                pts[0].x = x;   pts[0].y=6.0;
                pts[1].x = x;   pts[1].y=7.0;
                gpolyline(2, pts);
        }
        for (x=62.5; x<71.0; x+=4.0) {
                pts[0].x = x;   pts[0].y=6.0;
                pts[1].x = x;   pts[1].y=7.0;
                gpolyline(2, pts);
        }

/* outline the whole mess */
        pts[0].x = 37.5;        pts[0].y=2.0;
        pts[1].x = 74.5;        pts[1].y=2.0;
        pts[2].x = 74.5;        pts[2].y=7.0;
        pts[3].x = 37.5;        pts[3].y=7.0;
        pts[4].x = 37.5;        pts[4].y=2.0;
        gpolyline(5, pts);
}
