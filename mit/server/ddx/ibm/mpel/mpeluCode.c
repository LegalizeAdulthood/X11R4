/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
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
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /andrew/X11/R3src/tape/server/ddx/ibm/mpel/RCS/mpeluCode.c,v 6.1 88/10/25 01:57:40 kbg Exp $ */
/* $Source: /andrew/X11/R3src/tape/server/ddx/ibm/mpel/RCS/mpeluCode.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/tape/server/ddx/ibm/mpel/RCS/mpeluCode.c,v 6.1 88/10/25 01:57:40 kbg Exp $";
#endif

#include <sys/types.h>
#include <fcntl.h>

#include "Xmd.h"

#include "ibmTrace.h"

#include "mpelHdwr.h"

#ifndef AIXrt

#define FALSE 0
#define TRUE (-1)

/***==================================================================***/

static void
mpelClearMemory()
{
    volatile unsigned short int *tmp ;

    TRACE(("mpelClearMemory()\n"));

    /* zero adapter data space */
    for ( tmp = MPEL_LOWDATA ; tmp <= MPEL_HIGHDATA ; tmp++ )
	*tmp= 0 ;

    /* zero adapter program space */
    for ( tmp = MPEL_LOWPROGRAM ; tmp <= MPEL_HIGHPROGRAM ; tmp++ ) *tmp= 0 ;

    return;
}

/***==================================================================***/

void
mpelLoad(file,addr)
    char	*file;
    volatile unsigned short *addr;
{
    int		fd, nWords, done ;
    unsigned short int *tmp, buf[2000] ;

    TRACE(("mpelLoad(%s,0x%x)\n",file,addr));

    if ((fd=open(file,O_RDONLY))<0) {
	ErrorF("can't open %s.\nExiting\n",file);
	exit(1);
	/*NOTREACHED*/
    }
    done= FALSE;
    while (!done) {
	nWords= read(fd,buf,2000*sizeof(short))/sizeof(short);
	if (nWords<0) {
	    ErrorF("WSGO!! Read failed in mpelLoad.\nExiting.\n");
	    exit(1);
	}
	else if (nWords<2000) 
	    done= TRUE;
	tmp= &buf[0];
	while (nWords-->0) {
	    *addr= *tmp;
	    addr++; tmp++;
	}
    }
    (void) close(fd);
    return;
}

/***==================================================================***/

void
mpelLoaduCode()
{
register int fd, i ;

    TRACE(("mpelLoadUCode()\n"));

    if ( ( fd = open( "/dev/bus", O_RDWR ) ) < 0 ) {
       ErrorF( "couldn't open /dev/bus\nExiting.\n" ) ;
       exit( 1 ) ;
    }
    MPEL_PCR= PCR_HOLD|PCR_RESET_TMS|PCR_BYTE_ORDER;
    while (MPEL_PSR&PSR_NOT_HOLD_ACK);	/* wait for TMS to stop */

    mpelClearMemory();
    MPEL_CA_PTR= 0x0000480;	/* set up comm area ptr */
    /* someday set annotation text font location table (maybe) */
    /* someday set programmable character set font location table (maybe) */
    mpelLoad(MPEL_CIRCLE_FILE,MPEL_CIRCLE_ADDR);
    mpelLoad(MPEL_SINE_FILE,MPEL_SINE_ADDR);
    mpelLoad(MPEL_MARKER_LOC_FILE,MPEL_MARKER_LOC_ADDR);
    mpelLoad(MPEL_HATCH_FILE,MPEL_HATCH_ADDR);
    mpelLoad(MPEL_MARKER_FNT_FILE,MPEL_MARKER_FNT_ADDR);
    MPEL_ATF_LOC_PTR= mpelAddr(MPEL_ATF_REGION);
    mpelInitFonts();
    mpelLoad(MPEL_UCODE_FILE,MPEL_UCODE_ADDR);
    /* we want to do this before starting the tms */
    mpelInitPatterns(); 

    MPEL_PCR= PCR_BYTE_ORDER;		/* start tms */
    for (i=100000;MPEL_COMM_REASON!=MPELRSN_INIT_DONE;i--) {
	if (i<0) {
	    ErrorF("initialization load failed (%d)\n",MPEL_COMM_REASON);
	    break;
	}
    }
    MPEL_COMM_REASON = 0;

    MPEL_COMM_REQ = MPELCMD_ENTER_FIFO;
    MPEL_PCR &= ~PCR_INTR_TMS ;
    MPEL_PCR |= PCR_INTR_TMS;
    (void) close(fd);
    return;
}

#else /* defined AIXrt */

void
mpelLoaduCode()
{
register int i ;

    TRACE(("mpelLoadUCode()\n"));

    MPEL_PCR= 		PCR_BYTE_ORDER;
    MPEL_ATF_LOC_PTR=	mpelAddr(MPEL_ATF_REGION);
    mpelInitFonts();
    /* we want to do this before starting the tms */
    mpelInitPatterns(); 

    MPEL_PCR= PCR_BYTE_ORDER;		/* start tms */
    MPEL_COMM_REASON = 0;
    MPEL_COMM_REQ = MPELCMD_ENTER_FIFO;
    MPEL_PCR &= ~PCR_INTR_TMS ;
    MPEL_PCR |= PCR_INTR_TMS;
    return;
}

#endif /* defined AIXrt */
