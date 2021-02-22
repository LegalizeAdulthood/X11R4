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

/*
 * bsort - graphical bubble sort.
 */

#include <xgks.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "demo.h"

#define FALSE 0
#define TRUE 1

Gint ws_id = 1;

int result;
int bkg;

#define CMAX 240
#define MAX 100

Gfloat ypos, xpos;
Gint color_list[MAX];

static void gen_colors(void);
static void init_list(void);
static void bsort(void);
static void show_it(void);
void quick_sort(int l, int r);
void perr(int i, char *s);

int main(int argc, char *argv[])
{
    char *conn = (char *) NULL;
    Glimit WsWindow;
    Glimit Viewport;
    Glimit Window;

    int i;

    for (i = 1; i < argc; i++)
    {
        if (index(argv[i], ':'))
            conn = argv[i];
        /* Application dependent options here */
    }

    gopengks(stdout, 0);
    gopenws(ws_id, conn, conn);
    gactivatews(ws_id);

    WsWindow.xmin = 0.0;
    WsWindow.xmax = 1.0;
    WsWindow.ymin = 0.0;
    WsWindow.ymax = 0.8;
    gsetwswindow(ws_id, &WsWindow);

    Viewport.xmin = 0.0;
    Viewport.xmax = 1.0;
    Viewport.ymin = 0.0;
    Viewport.ymax = 0.8;
    gsetviewport(ws_id, &Viewport);
    gselntran(1);

    gen_colors();

    Window.xmin = 0.0;
    Window.xmax = 1279.0;
    Window.ymin = 0.0;
    Window.ymax = 1023.0;
    gsetwindow(ws_id, &Window);

    gsetmarkertype(GMK_POINT);
    gsetmarkersize(2.0);
    init_list();
    ypos = 1023.0;
    xpos = 0.0;
    bsort();

    init_list();
    ypos = 1023.0;
    xpos = 640;
    show_it();
    quick_sort(0, MAX - 1);

    WaitForBreak(ws_id);

    gdeactivatews(ws_id);
    gclosews(ws_id);
    gclosegks();
}

void perr(int i, char *s)
{
    fprintf(stdout, "%s %d\n", s, i);
    exit(1);
}

static void show_it(void)
{
    Gpoint pt;
    int i;

    pt.y = ypos;
    for (i = 0; i < MAX; i++)
    {
        gsetmarkercolorind(color_list[i]);
        pt.x = xpos + (i * 6.0) + 2.0;
        gpolymarker(1, &pt);
    }
    ypos -= 6.0;
}

static void bsort(void)
{
    int n, m, t;
    int sorted;

    for (n = MAX - 2; n >= 0; n--)
    {
        show_it();
        sorted = TRUE;
        for (m = 0; m <= n; m++)
        {
            if (color_list[m] > color_list[m + 1])
            {
                sorted = FALSE;
                t = color_list[m];
                color_list[m] = color_list[m + 1];
                color_list[m + 1] = t;
            }
        }
        if (sorted)
            break;
    }
}

void quick_sort(int l, int r)
{
    int ll, rr, k, t;

    if (r > l)
    {
        ll = l;
        rr = r + 1;
        k = color_list[l];
        do
        {
            do
            {
                ll++;
            } while (color_list[ll] < k);
            do
            {
                rr--;
            } while (color_list[rr] > k);
            if (ll < rr)
            {
                t = color_list[ll];
                color_list[ll] = color_list[rr];
                color_list[rr] = t;
            }
        } while (rr > ll);
        color_list[l] = color_list[rr];
        color_list[rr] = k;
        quick_sort(l, rr - 1);
        quick_sort(rr + 1, r);
        show_it();
    }
}

static void gen_colors(void)
{
    Gcobundl cb;
    Gint color;

    cb.red = 1.0;
    cb.green = 0.0;
    cb.blue = 0.0;

    /* red to green */
    for (color = 1; color <= (CMAX / 3); color++)
    {
        gsetcolorrep(ws_id, color, &cb);
        cb.red -= 3.0 / CMAX;
        cb.green = ((cb.green + 3.0 / CMAX) > 1.0) ? 1.0 : cb.green + 3.0 / CMAX;
    }
    cb.red = 0.0;

    /* green to blue */
    for (; color <= (2 * CMAX / 3); color++)
    {
        gsetcolorrep(ws_id, color, &cb);
        cb.green -= 3.0 / CMAX;
        cb.blue = ((cb.blue + 3.0 / CMAX) > 1.0) ? 1.0 : cb.blue + 3.0 / CMAX;
    }
    cb.green = 0.0;

    /* blue to red */
    for (; color <= CMAX; color++)
    {
        gsetcolorrep(ws_id, color, &cb);
        cb.blue -= 3.0 / CMAX;
        cb.red = ((cb.red + 3.0 / CMAX) > 1.0) ? 1.0 : cb.red + 3.0 / CMAX;
    }
}

static void init_list(void)
{
    struct timeval tp;
    struct timezone tzp;
    Gint i;

    gettimeofday(&tp, &tzp);
    srandom(tp.tv_sec);

    for (i = 0; i < MAX; i++)
        color_list[i] = random() % CMAX;
}
