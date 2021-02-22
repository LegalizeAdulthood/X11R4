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
*/

#include <xgks.h>

#include <stdio.h>

/*
 * This function is called when the program is finished and we just want
 * to wait until the user is done looking at the output.  Here we set up
 * a choice device to use the function keys.  However, we ignore all choices
 * and wait for the user to press the Break key.
 */
WaitForBreak( wsid )
        Gint wsid;
{
        Gchoice init;
        Gchoicerec record;
        Glimit earea;

        earea.xmin = 0.0;
        earea.xmax = 1279.0;
        earea.ymin = 0.0;
        earea.ymax = 1023.0;

        gmessage(wsid, "Done, press Break to quit ...");

        init.status = GC_NOCHOICE;
        init.choice = 0;
        record.pet1.data = NULL;
        ginitchoice( wsid, 1, &init, 1, &earea, &record );
        gsetchoicemode( wsid, 1, GREQUEST, GECHO );
        for( ; init.status != GC_NONE ; )
                greqchoice( wsid, 1, &init );
}

/*
 * set up varius text parameters and print "Hello World"
 */
PrintHello()
{
        Gpoint tpt;
        Gpoint up;
        Gtxfp txfp;
        Gtxalign align;

        txfp.font = 4;                  /* use Serif Bold Roman font */
        txfp.prec = GSTROKE;
        gsettextfontprec(&txfp);

        gsetcharexpan(0.5);
        gsetcharspace(0.2);
        gsettextcolorind( 1 );          /* should be white */

        gsetcharheight(0.05);

        up.x = 0.0; up.y = 1.0;         /* characters are straight up */
        gsetcharup(&up);

        align.hor = GTH_CENTER;
        align.ver = GTV_HALF;
        gsettextalign(&align);

        gsettextpath(GTP_RIGHT);        /* print from left to right */

        tpt.x = 0.5; tpt.y = 0.5;       /* center of the window */
        gtext(&tpt,"Hello World!");

}
main(argc,argv)
int argc;
char *argv[];
{
        int i, wsid=1;
        char *conn = (char *)NULL;

        for( i=1; i<argc; i++){
                if (index( argv[i], ':'))
                        conn = argv[i];
        /* Application dependent options here */
        }

        gopengks(stdout,0);

        if (gopenws( wsid , conn, conn) != 0)
                exit(0);
        gactivatews( wsid );

        PrintHello();
        WaitForBreak ( wsid );

        gdeactivatews( wsid );
        gclosews( wsid );

        gclosegks();
        exit(0);
}
