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

#include <xgks.h>

#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

void clockface(void);
void bighand(void);
void littlehand(void);
void adjust(int minute);
void seg_trans(int num, Gpoint *org, Gpoint *trn);
void catchalrm(int signum);

#define GLOVE 1

#define BLACK 0
#define WHITE 1
#define RED 224
#define GREEN 28
#define BLUE 11
#define YELLOW 252
#define CYAN 31
#define VIOLET 227

struct sigaction alrmsig = {
    catchalrm,
};

#define TRUE 1
#define FALSE 0
int ChangeTime;

#define FACE 1
#define BIGHAND 2
#define LITTLEHAND 3

Gfloat trans[2][3] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

Gasfs asf = { GBUNDLED, GBUNDLED, GBUNDLED,             /* polyline asfs */
    GINDIVIDUAL, GINDIVIDUAL, GINDIVIDUAL,              /* polymarker asfs */
    GINDIVIDUAL, GINDIVIDUAL, GINDIVIDUAL, GINDIVIDUAL, /*text asfs */
    GINDIVIDUAL, GINDIVIDUAL, GBUNDLED };               /* fill area asfs */

char *roman[] = {
    "XII",
    "I",
    "II",
    "III",
    "IIII",
    "V",
    "VI",
    "VII",
    "VIII",
    "IX",
    "X",
    "XI",
};

int main(int argc, char *argv[])
{
    int Time = 0;
    int i;

    Gint ws_id = 1;
    char *conn = (char *) NULL;
    Glimit Window;
    Glimit WsWindow;
    Glimit WsViewport;
    Glnbundl plrep;

    Window.xmin = -100.0;
    Window.xmax = 100.0;
    Window.ymin = -100.0;
    Window.ymax = 100.0;

    WsWindow.xmin = 0.0;
    WsWindow.xmax = 1.0;
    WsWindow.ymin = 0.0;
    WsWindow.ymax = 1.0;

    WsViewport.xmin = 128.0;
    WsViewport.xmax = 1151.0;
    WsViewport.ymin = 0.0;
    WsViewport.ymax = 1023.0;

    plrep.type = 1;
    plrep.width = 1.0;
    plrep.colour = BLACK;

    for (i = 1; i < argc; i++)
    {
        if (index(argv[i], ':'))
            conn = argv[i];
        /* Application dependent options here */
    }

    gopengks(stdout, 0);
    gopenws(ws_id, conn, conn);
    gactivatews(ws_id);
    /* init_colors( ws_id ); */

    sigaction(SIGALRM, &alrmsig, NULL);

    /* establish clock coordinates */
    gsetwindow(1, &Window);
    gsetviewport(1, &WsWindow);
    gsetwswindow(1, &WsWindow);
    gsetwsviewport(1, &WsViewport);
    gselntran(1);

    /*
     * define attributes for clock
     *
     *  fabnd 2 - hand
     *  fabnd 3 - glove
     *
     *      plbnd 3 - finger
     *  txbnd 1 - numbers
     */

    gsetdeferst(1, GASAP, GALLOWED);
    gsetasf(&asf);

    gsetfillintstyle(GSOLID);
    gsetlinerep(1, 3, &plrep);

    /*
        create_seg(FACE) ;
        clockface() ;
        close_seg() ;

        create_seg(BIGHAND) ;
        s_sg_vis(BIGHAND, NO) ;
        s_sg_pri(BIGHAND, 0.1) ;
        bighand() ;
        close_seg() ;

        create_seg(LITTLEHAND) ;
        s_sg_vis(LITTLEHAND, NO) ;
        s_sg_pri(LITTLEHAND, 0.3) ;
        littlehand() ;
        close_seg() ;

        s_sg_vis(BIGHAND, YES) ;
        s_sg_vis(LITTLEHAND, YES) ;
    */

    for (;;)
    {
        Time = (time(NULL) / 60) - 6 * 60; /* compensate for time zone */
        alarm(60);
        adjust(Time);
        ChangeTime = FALSE;
        while (!ChangeTime)
            pause();
    }

    gdeactivatews(ws_id);
    gclosews(ws_id);
    gclosegks();
}

/*
 * draw clock
 */
void clockface(void)
{
#define BORDERSZ 90.0
#define INTFACTOR 0.95
#define TXFACTOR 0.87

    Gpoint pts[50];
    Gpoint txtloc;
    double sin(), cos();
    int i;
    Gfloat deg2rad = 3.14159 / 180.0;
    Gflbundl farep;
#define SetFillRep(w, n, i, s, c) \
    farep.inter = i;              \
    farep.style = s;              \
    farep.color = c;              \
    gsetfillrep(w, n, &farep)

    Gtxfp txfp;
    Gtxalign txalign;

    txfp.font = 2;
    txfp.prec = GSTROKE;
    txalign.hor = GTH_CENTER;
    txalign.ver = GTV_HALF;

    gclearws(1, GALWAYS);

    SetFillRep(1, 1, GSOLID, 1, BLUE);
    SetFillRep(1, 2, GSOLID, 1, WHITE);
    SetFillRep(1, 3, GSOLID, 1, WHITE);
    SetFillRep(1, 4, GSOLID, 1, WHITE);

    for (i = 0; i < 50; i++)
    {
        pts[i].x = BORDERSZ * cos((360.0 / 50.0) * i * deg2rad);
        pts[i].y = BORDERSZ * sin((360.0 / 50.0) * i * deg2rad);
    }
    gsetfillind(2);
    gfillarea(50, pts);
    for (i = 0; i < 50; i++)
    {
        pts[i].x *= INTFACTOR;
        pts[i].y *= INTFACTOR;
    }
    gsetfillind(1);
    gfillarea(50, pts);

    txfp.font = 4;
    txfp.prec = GSTROKE;
    gsettextfontprec(&txfp);
    gsetcharspace(0.2);
    gsetcharheight(16.0);
    gsettextalign(&txalign);
    gsettextcolorind(YELLOW);

    for (i = 1; i <= 12; i++)
    {
        txtloc.x = BORDERSZ * INTFACTOR * TXFACTOR * cos((360.0 / 12.0) * i * deg2rad);
        txtloc.y = BORDERSZ * INTFACTOR * TXFACTOR * sin((360.0 / 12.0) * i * deg2rad);
        gtext(&txtloc, roman[(2 + 13 - i) % 12]);
    }
    txtloc.x = 0;
    txtloc.y = 45.0;
    txfp.font = 4;
    txfp.prec = GSTROKE;
    gsettextfontprec(&txfp);
    gsetcharheight(12.0);
    gsetcharexpan(0.6);
    gsettextcolorind(RED);
    gtext(&txtloc, "Graphical Kernel System");
}

/*
 * draw big hand
 */

#ifdef ARROW
void bighand(void)
{
    static Gpoint bh[] = {
        { -10.0, -20.0 }, { -10.0, -15.0 }, { -5.0, -10.0 }, { -5.0, 55.0 },
        { -10.0, 50.0 }, { 0.0, 70.0 }, { 10.0, 50.0 },
        { 5.0, 55.0 }, { 5.0, -10.0 }, { 10.0, -15.0 }, { 10.0, -20.0 }, { 0.0, -15.0 }
    };
    Gpoint sbh[12];

    seg_trans(12, bh, sbh);
    s_fill_idx(3);
    fill_area((sizeof(bh) / sizeof(Gpoint)), sbh);
}
/*
 * draw little hand
 */
void littlehand(void)
{
    static Gpoint lh[] = {
        { -10.0, -20.0 }, { -10.0, -15.0 }, { -5.0, -10.0 }, { -5.0, 30.0 },
        { -10.0, 25.0 }, { 0.0, 45.0 }, { 10.0, 25.0 },
        { 5.0, 30.0 }, { 5.0, -10.0 }, { 10.0, -15.0 }, { 10.0, -20.0 }, { 0.0, -15.0 }
    };

    Gpoint slh[12];

    seg_trans(12, lh, slh);
    s_fill_idx(3);
    fill_area((sizeof(lh) / sizeof(Gpoint)), slh);
}
#endif

#ifdef GLOVE
static Gpoint bigarm[] = {
    { 0.0, -5.0 }, { -2.5, 0.0 }, { -2.5, 50.0 }, { 2.5, 50.0 }, { 2.5, 0.0 },
};
static Gpoint littlearm[] = {
    { 0.0, -5.0 }, { -2.5, 0.0 }, { -2.5, 25.0 }, { 2.5, 25.0 }, { 2.5, 0.0 }
};
static Gpoint bigglove[] = {
    { -2.5, 50.0 }, { -2.5, 55.0 }, { -5.0, 55.0 }, { -5.0, 70.0 },
    { -2.5, 70.0 }, { -2.5, 65.0 }, { 5.0, 65.0 }, { 5.0, 55.0 },
    { 2.5, 55.0 }, { 2.5, 50.0 }
};
static Gpoint litglove[] = {
    { -2.5, 25.0 }, { -2.5, 30.0 }, { -5.0, 30.0 }, { -5.0, 45.0 },
    { -2.5, 45.0 }, { -2.5, 40.0 }, { 5.0, 40.0 }, { 5.0, 30.0 },
    { 2.5, 30.0 }, { 2.5, 25.0 }
};
static Gpoint bigfingers[] = {
    { -2.5, 50.0 }, { -2.5, 55.0 }, { -5.0, 55.0 }, { -5.0, 70.0 },
    { -2.5, 70.0 }, { -2.5, 60.0 }, { -2.5, 65.0 }, { 0.0, 65.0 },
    { 0.0, 60.0 }, { 0.0, 65.0 }, { 2.5, 65.0 }, { 2.5, 60.0 },
    { 2.5, 65.0 }, { 5.0, 65.0 }, { 5.0, 55.0 }, { 2.5, 55.0 },
    { 2.5, 50.0 }
};
static Gpoint litfingers[] = {
    { -2.5, 25.0 }, { -2.5, 30.0 }, { -5.0, 30.0 }, { -5.0, 45.0 },
    { -2.5, 45.0 }, { -2.5, 35.0 }, { -2.5, 40.0 }, { 0.0, 40.0 },
    { 0.0, 35.0 }, { 0.0, 40.0 }, { 2.5, 40.0 }, { 2.5, 35.0 },
    { 2.5, 40.0 }, { 5.0, 40.0 }, { 5.0, 30.0 }, { 2.5, 30.0 },
    { 2.5, 25.0 }
};
void bighand(void)
{
    Gpoint spts[17];

    seg_trans(5, bigarm, spts);
    gsetfillind(3);
    gfillarea((sizeof(bigarm) / (sizeof(Gpoint))), spts);

    seg_trans(10, bigglove, spts);
    gsetfillind(4);
    gfillarea((sizeof(bigglove) / (sizeof(Gpoint))), spts);

    seg_trans(17, bigfingers, spts);
    gsetlineind(3);
    gpolyline((sizeof(bigfingers) / (sizeof(Gpoint))), spts);
}
void littlehand(void)
{
    Gpoint spts[17];

    seg_trans(5, littlearm, spts);
    gsetfillind(3);
    gfillarea((sizeof(littlearm) / (sizeof(Gpoint))), spts);

    seg_trans(10, litglove, spts);
    gsetfillind(4);
    gfillarea((sizeof(litglove) / (sizeof(Gpoint))), spts);

    seg_trans(17, litfingers, spts);
    gsetlineind(3);
    gpolyline((sizeof(litfingers) / (sizeof(Gpoint))), spts);
}
#endif

/*
 * adjust big & little hands via segment transformation
 */
void adjust(int minute)
{
    double sin(), cos();

    double angle, sinx, cosx;

    clockface();
    /* do little hand */

    angle = (minute % 720) * 0.5 * 3.14159 / 180.0;
    sinx = sin(angle);
    cosx = cos(angle);
    trans[0][0] = trans[1][1] = cosx;
    trans[1][0] = -(trans[0][1] = sinx);
    trans[0][2] = -0.5 * (-1.0 + sinx + cosx);
    trans[1][2] = -0.5 * (-1.0 + -sinx + cosx);
    /* s_sg_trans(LITTLEHAND, trans) ; */

    littlehand();
    /* do big hand */

    angle = (minute % 60) * 6.0 * 3.14159 / 180.0;
    sinx = sin(angle);
    cosx = cos(angle);
    trans[0][0] = trans[1][1] = cosx;
    trans[1][0] = -(trans[0][1] = sinx);
    trans[0][2] = -0.5 * (-1.0 + sinx + cosx);
    trans[1][2] = -0.5 * (-1.0 + -sinx + cosx);
    /* s_sg_trans(BIGHAND, trans) ; */
    bighand();
}

void seg_trans(int num, Gpoint *org, Gpoint *trn)
{
    while (num--)
    {
        trn->x = org->x * trans[0][0] + org->y * trans[0][1] + trans[0][2];
        trn->y = org->x * trans[1][0] + org->y * trans[1][1] + trans[1][2];
        org++;
        trn++;
    }
}

void catchalrm(int signum)
{
    ChangeTime = TRUE;
}
