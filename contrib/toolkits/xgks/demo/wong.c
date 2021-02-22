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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "demo.h"

Gint ws_id = 1;

Gint result;

/* xmin , xmax, ymin, ymax */
Glimit w1;
Glimit v1;
Glimit wsw = { 0.0, 1.0, 0.0, 0.8 };
Glimit wsv = { 0.0, 1280.0, 0.0, 1024.0 };

Gchar *conn[20];
Gint wscnt = 1;
Gint i = 0, j = 1;

Gfloat x[15];

void perr(int i, char *s);
void plot_data(int i);

int main(int argc, char *argv[])
{
    Gint num, ds;

    conn[wscnt] = NULL;

    for (i = 1; i < argc; i++)
    {
        if (index(argv[i], ':'))
            conn[wscnt++] = argv[i];
    }

    gopengks(stderr, 0);

    for (j = 1; j <= wscnt; j++)
    {
        gopenws(j, conn[j], conn[j]);
        gsetwswindow(j, &(wsw));
        gsetwsviewport(j, &(wsv));
        gactivatews(j);
        gsetdeferst(j, GASAP, GALLOWED);
    }

    for (ds = 1; ds < 4; ds++)
    {
        scanf("%f %f %f %f", &v1.xmin, &v1.xmax, &v1.ymin, &v1.ymax);
        scanf("%f %f %f %f %d", &w1.xmin, &w1.xmax, &w1.ymin, &w1.ymax, &num);

        for (j = 0; j < wscnt; j++)
        {
            if ((result = gsetwindow(ds, &w1)) != 0)
                perr(result, "...gsetwindow()\n");
            if ((result = gsetviewport(ds, &v1)) != 0)
                perr(result, "...gsetviewport()\n");
        }

        if ((result = gselntran(ds)) != 0)
            perr(result, "...gselntran()");

        for (i = 0; i < 15; i++)
        {
            scanf("%f", &x[i]);
        }

        for (i = 0; i < num; i++)
            plot_data(i);
    }

    WaitForBreak(1);

    for (j = 1; j <= wscnt; j++)
    {
        gdeactivatews(j);
        gclosews(j);
        fprintf(stderr, "workstation %d is closed\n\n", j);
    }
    gclosegks();
    fprintf(stderr, "GKS is closed\n\n");
}

void perr(int i, char *s)
{
    fprintf(stdout, "%s %d\n", s, i);
    exit(1);
}

void plot_data(int i)
{
    Gpoint pts[15];
    Gint num_pts;

    for (num_pts = 0; num_pts < 15; num_pts++)
    {
        pts[num_pts].x = x[num_pts];
        scanf("%f", &(pts[num_pts].y));
    }

    if ((result = gsetmarkertype(i % 4 + 2)) != 0)
        perr(result, "...s_marker_type()");
    if ((result = gsetmarkercolorind(i % 6 + 1)) != 0)
        perr(result, "...gsetmarkercolorind()");

    if ((result = gsetlinetype(i % 4 + 1)) != 0)
        perr(result, "...s_linetype()");
    if ((result = gsetlinecolorind(i % 6 + 1)) != 0)
        perr(result, "...gsetlinecolorind()");

    if ((result = gpolyline(num_pts, pts)) != 0)
        perr(result, "...polyline()");

    if ((result = gpolymarker(num_pts, pts)) != 0)
        perr(result, "...polymarker()");
}
