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
 * ethernet.c - function to draw the ethernet portion of the logo
 */

#include <xgks.h>

#include <stdio.h>
#include <string.h>

#include "demo.h"

#include "ethernet.h"

void do_text(void);
void do_ethernet(void);

void draw_ethernet( Gint wsid, Gint dotext)
{
        Glimit Window;

        Window.xmin = -100.0;
        Window.xmax = 100.0;
        Window.ymin = -100.0;
        Window.ymax = 100.0;

        gsetwindow(wsid, &Window );
        gselntran(1) ;

        if (dotext)
                do_text();
        do_ethernet();
}


#define BORDERSZ 87.0
#define INTFACTOR 0.97

void do_text(void)
{
        char c[2], words[25];
        Gpoint pt;
        double sin(), cos() ;
        int i ;
        Gfloat deg2rad = 3.14159 / 180.0 ;
        Gtxfp txfp;
        Gtxalign txalign;

        txfp.font = 4;
        txfp.prec = GSTROKE;
        txalign.hor = GTH_CENTER;
        txalign.ver = GTV_HALF;

        c[1] = '\0';
        strcpy(words,"PROFESSIONAL WORKSTATION");

        gsettextfontprec( &txfp ) ;
        gsetcharspace(0.0) ;
        gsetcharheight(10.0) ;
        gsettextalign( &txalign ) ;
        gsettextcolorind(YELLOW) ;

        for(i=0; i<strlen(words); i++)
        {
                pt.x = (BORDERSZ +7) * cos((180.0 -((360.0/46.0)*i)) * deg2rad) ;
                pt.y = (BORDERSZ +7) * sin((180.0 -((360.0/46.0)*i)) * deg2rad) ;
                c[0] = words[i];
                gtext(&pt,c);
        }

        strcpy(words,"RESEARCH GROUP");
        for(i=0; i<strlen(words); i++)
        {
                pt.x = (BORDERSZ +7) * cos((180.0 +((360.0/46.0)*(i+5))) * deg2rad) ;
                pt.y = (BORDERSZ +7) * sin((180.0 +((360.0/46.0)*(i+5))) * deg2rad) ;
                c[0] = words[i];
                gtext(&pt,c);
        }
}

void do_ethernet(void)
{

        Gpoint pts[51] ;
        double sin(), cos() ;
        int i ;
        Gfloat deg2rad = 3.14159 / 180.0 ;

        for(i=0; i<51; i++)
        {
                pts[i].x = BORDERSZ * cos((360.0/50.0) * i * deg2rad) ;
                pts[i].y = BORDERSZ * sin((360.0/50.0) * i * deg2rad) ;
        }
        gsetfillintstyle(GSOLID) ;
        gsetfillcolorind(YELLOW) ;
        gfillarea(51, pts) ;
        for(i=0; i<51; i++)
        {
                pts[i].x *= INTFACTOR ;
                pts[i].y *= INTFACTOR ;
        }
        gsetfillcolorind(BLUE) ;
        gfillarea(51, pts) ;
}
