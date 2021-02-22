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

#include "demo.h"

int main(int argc, char *argv[])
{

        Gint  error;
        Gchar *record;
        Ggksmit gksmit;

        if (argc < 3) {
                fprintf(stderr,"usage: %s host:display.server# MI-FileName\n", argv[0]);
                exit(0);
        }

        gopengks (stderr, 0);

        gopenws (1,argv[1],argv[1]);
        gactivatews(1);

        gopenws (5,argv[2],"MI");


        error = 0;
        while (error == 0) {
                error = ggetgksm (5, &gksmit);
                record = malloc (gksmit.length);
                error = greadgksm (5, gksmit.length, record);
                ginterpret (&gksmit, record);
        }

        fprintf(stderr, " ... done press break....");

        WaitForBreak (1);

        gdeactivatews(1);
        gclosews (1);
        fprintf(stderr, "workstation 1 is closed\n\n");
        gclosews (5);
        fprintf(stderr, "workstation 5 is closed\n\n");
        gclosegks();
        fprintf(stderr, "GKS is closed\n\n");
}
