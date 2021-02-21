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
static char rcsid[] = "$Header: font.c,v 4.0 89/09/01 14:23:36 amy Exp $";

#include <stdio.h>
#include <xgks.h>
#include "demo.h"

Gint    ws_id = 1;

Gint result;

Glimit w;       /*MIT*/
Glimit v;       /*MIT*/

Glimit wsw;     /*MIT*/
Glimit wsv;     /*MIT*/

main(argc,argv)
int argc;
char *argv[];
{
        double atof();
        int i;
        char *conn = (char *)NULL;

        for( i=1; i<argc; i++){
                if (index( argv[i], ':'))
                        conn = argv[i];
        /* Application dependent options here */
        }

        gopengks(stdout,0);
        if ((result = gopenws(ws_id, conn, conn)) != 0)
                perr(result, "...open_ws");
        gactivatews(ws_id);

        gopenws (200, "font.out", "MO");
        gactivatews (200);

        wsw.xmin = 0.0;         /*MIT*/
        wsw.xmax = 1.0;         /*MIT*/
        wsw.ymin = 0.0;         /*MIT*/
        wsw.ymax = 0.8;         /*MIT*/
        gsetwswindow(200, &wsw);

        wsv.xmin = 0.0;         /*MIT*/
        wsv.xmax = 1279.0;      /*MIT*/
        wsv.ymin = 0.0;         /*MIT*/
        wsv.ymax = 1023.0;      /*MIT*/
        gsetwsviewport(200, &wsv);

        w.xmin = 0.0;           /*MIT*/
        w.xmax = 1.0;           /*MIT*/
        w.ymin = 0.0;           /*MIT*/
        w.ymax = 1.0;           /*MIT*/
        gsetwindow(1, &w);

        v.xmin = 0.0;           /*MIT*/
        v.xmax = 1.0;           /*MIT*/
        v.ymin = 0.0;           /*MIT*/
        v.ymax = 0.8;           /*MIT*/
        gsetviewport(1, &v);

        gsetwswindow(ws_id, &wsw);
        gsetwsviewport(ws_id, &wsv);

        gselntran(1);
        test_font();

        for(i=1; i<10; i++) {
                getchar();
                gclearws(200, GALWAYS);
                gclearws(1, GALWAYS);
                show_font(i);
        }

        fprintf(stderr,"Done, press break to quit ...\n"); getchar();

        WaitForBreak (1);

        if ((result = gdeactivatews(200)) != 0)
                perr(result, "...deactivate_ws");

        if ((result = gclosews(200)) != 0)
                perr(result, "...close_ws");

        if ((result = gdeactivatews(ws_id)) != 0)
                perr(result, "...deactivate_ws");

        if ((result = gclosews(ws_id)) != 0)
                perr(result, "...close_ws");

        if ((result = gclosegks()) != 0)
                perr(result,"...close_gks");
        fprintf(stdout,"after close_gks\n");
        exit(0);
}

perr(i,s)
int i;
char *s;
{
        fprintf(stdout,"%s %d\n",s,i);
        exit(1);
}

Gasfs asf = {
        GINDIVIDUAL, GINDIVIDUAL, GINDIVIDUAL,          /* polyline */
        GINDIVIDUAL, GINDIVIDUAL, GINDIVIDUAL,          /* polymarker */
        GINDIVIDUAL, GINDIVIDUAL, GINDIVIDUAL, GINDIVIDUAL,     /* text */
        GINDIVIDUAL, GINDIVIDUAL, GINDIVIDUAL           /* fillarea */
};

test_font()
{
        Gpoint tpt;
        char s[100];
        int i;
        Gpoint up;
        Gtxfp txfp;
        Gtxalign align;

        txfp.font = 4;
        txfp.prec = GSTROKE;
        gsettextfontprec(&txfp);
        gsetcharexpan(0.5);
        gsetcharspace(0.2);
        gsettextcolorind(WHITE);                /* WHITE */

        gsetcharheight(0.05);
        up.x = 0.0; up.y = 1.0;
        gsetcharup(&up);
        align.hor = GTH_CENTER;
        align.ver = GTV_BASE;
        gsettextalign(&align);
        gsettextpath(GTP_RIGHT);

        gsetasf(&asf);

        tpt.x = 0.5; tpt.y = 0.9;
        gtext(&tpt,"GKS VECTOR FONTS");

        gsetcharheight(0.05);
        align.hor = GTH_LEFT;
        align.ver = GTV_BASE;
        gsettextalign(&align);

        tpt.x = 0.01; tpt.y = 0.90;
        for(i=1; i<10; i++) {
                txfp.font = i;
                txfp.prec = GSTROKE;
                gsettextfontprec(&txfp);
                SetColor(i);
                tpt.y -= 0.08;
                sprintf(s,"Font #%d  AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz",i);
                gtext(&tpt, s);
        }
}
show_font(font)
        int font;
{
        Gpoint tpt;
        char s[100];
        Gpoint up;
        Gtxfp txfp;
        Gtxalign align;

        txfp.font = 1;
        txfp.prec = GSTROKE;
        gsettextfontprec(&txfp);

        gsetcharexpan(0.5);
        gsetcharspace(0.2);
        gsettextcolorind(WHITE);                /* WHITE */

        gsetcharheight(0.05);
        up.x = 0.0; up.y = 1.0;
        gsetcharup(&up);
        align.hor = GTH_CENTER;
        align.ver = GTV_BASE;
        gsettextalign(&align);
        gsettextpath(GTP_RIGHT);

        tpt.x = 0.5; tpt.y = 0.9;
        sprintf(s,"GKS VECTOR FONT NUMBER %d",font);
        gtext(&tpt, s);

        gsetcharheight(0.05);
        align.hor = GTH_LEFT;
        align.ver = GTV_BASE;
        gsettextalign(&align);
        txfp.font = font;
        txfp.prec = GSTROKE;
        gsettextfontprec(&txfp);

        tpt.x = 0.01; tpt.y = 0.80;
        gtext(&tpt, " !\"#$%&'()*+,-./:;<=>?@");
        tpt.y -= 0.08;
        gtext(&tpt, "0123456789");
        tpt.y -= 0.08;
        gtext(&tpt, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        tpt.y -= 0.08;
        gtext(&tpt, "abcdefghijklmnopqrstuvwxyz");
        tpt.y -= 0.08;
        gtext(&tpt, "[\\]^_`{|}~");             /*MIT*/
}

SetColor (hash)
Gint hash;
{
        switch (hash) {
                case 1 : gsettextcolorind(WHITE);   return;
                case 2 : gsettextcolorind(RED);   return;
                case 3 : gsettextcolorind(GREEN);   return;
                case 4 : gsettextcolorind(BLUE);   return;
                case 5 : gsettextcolorind(YELLOW);   return;
                case 6 : gsettextcolorind(CYAN);   return;
                case 7 : gsettextcolorind(SILVER);   return;
                case 8 : gsettextcolorind(DARKGREEN);   return;
                case 9 : gsettextcolorind(BEIGE);   return;
        }
}
