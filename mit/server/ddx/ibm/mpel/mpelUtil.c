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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelUtil.c,v 6.5 89/04/29 21:38:24 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelUtil.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelUtil.c,v 6.5 89/04/29 21:38:24 jeff Exp $";
#endif

/*
 * mpelutil.c
 */

#define BADREAD

#include "X.h"
#include "Xmd.h"
#include "misc.h"
#include "regionstr.h"
#include "miscstruct.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

#include "ibmTrace.h"

#define DELAY_ADDR	0xf00080E0		/* IOCC delay location */
#define DELAY(n)	{ register int N = (n); \
				while (--N >= 0) \
					* (char *) DELAY_ADDR = 0xff;	}

#define RESET_INT_TMS_BIT (MPEL_PCR &= ~PCR_INTR_TMS)
#define SET_INT_TMS_BIT (MPEL_PCR |= PCR_INTR_TMS)

#define MAXRETRY 500

int MPELNoOpData = 0;

mpelRectangle mpelDfltScissor = {
	{ 0, 0 }, { MPEL_WIDTH - 1, MPEL_HEIGHT - 1 }
} ;

/***==================================================================***/

/*
 * IDENTIFICATION: mpelicmd (copy from vtt8icmd)
 * DESCRIPTIVE NAME: Issue an Adapte command for the Megapel
 *		     Virtual Display Driver (VDD)
 * FUNCTION: Check for completion of any previous commands.
 *	     When done issue the requested command to the adapter
 * END OF SPEC
 */

void
mpelicmd2( command_code, parameters, parm_count )
unsigned short command_code;
unsigned short parameters[];
int   parm_count;
{
   int register i;
   int retry = 0;
   volatile unsigned short *COMM_Request_parm;
   unsigned short reg;

	/*
	 * Issue COMM Area Command
	 */
   MPEL_COMM_REQ = command_code;     /* Store request code into COMM area  */
   COMM_Request_parm = MPEL_COMM_PARM_ADDR;
   for ( i = 0 ; i < parm_count ; i++ )  /* Store parameters into COMM area */
	COMM_Request_parm[i] = parameters[i];
   RESET_INT_TMS_BIT;
   SET_INT_TMS_BIT;   		     /* Interrupt adapter */

	/*
	 * check for interrupt pending: clear it.
	 */
	retry = 0;
	while (retry < MAXRETRY) {
		reg = MPEL_PSR;
		if (reg & PSR_INTR_PENDING) {
			reg = MPEL_IPR;

			break;
		}
		DELAY(1000);
		retry++;
	}

	MPEL_COMM_REASON = 0x0000;
	MPEL_COMM_REQ = 0x0000;
	return;
}

/*
 * IDENTIFICATION: mpelicmd (copy from vtt8icmd)
 * DESCRIPTIVE NAME: Issue an Adapte command for the Megapel
 *		     Virtual Display Driver (VDD)
 * FUNCTION: Check for completion of any previous commands.
 *	     When done issue the requested command to the adapter
 * END OF SPEC
 */

void
mpelicmd(command_code, parameters, parm_count)
    unsigned short int command_code;
    unsigned short int parameters[];
    int   	parm_count;

{
    int		i;
    int		retry = 0;
    volatile unsigned short int *COMM_Request_parm;

    TRACE(("mpelicmd(0x%x,0x%x,%d)\n",command_code,parameters,parm_count));

    while(MPEL_COMM_REASON == 0) {   /* The previous command has not yet   */
       				     /* completed.			   */
	if( retry > MAXRETRY ) {     /* Adapter is taking too long         */
	    break;
	}
	DELAY( 1000 ) ;
	retry++;		     /* Another chance			   */
    }

    MPEL_COMM_REQ = command_code;     /* Store request code into COMM area  */
    MPEL_COMM_REASON = 0;
    COMM_Request_parm = MPEL_COMM_PARM_ADDR;
    for(i = 0; i < parm_count ; i++)  /* Store parameters into COMM area    */
	COMM_Request_parm[i] = parameters[i];
    RESET_INT_TMS_BIT;
    SET_INT_TMS_BIT;   		     /* Interrupt adapter		   */
    return;
}

/***==================================================================***/

/*
 * Poke the adapter -- execute some command (in this case
 * reset blink, but it really doesn't matter which) and wait
 * for it to complete.  Useful for making sure the adapter is
 * alive and talking to us.
 */

void
mpelPoke()
{
    int	i;

    TRACE(("mpelPoke()\n"));

    MPEL_COMM_REQ= MPELCMD_RESET_BLINK;
    MPEL_COMM_REASON= 0;
    RESET_INT_TMS_BIT;
    SET_INT_TMS_BIT;
    for (i=1000000;MPEL_COMM_REQ!=MPELRSN_RESET_BLINK_DONE;i--) {
	if (i<0) {
	    ErrorF("poke failed (%d)\n",(*(short *)0xf4c00904));
	    break;
	}
    }
    return;
}

/***==================================================================***/

void
mpelInitPatterns()
{
    TRACE(("mpelInitPatterns()\n"));

    MPEL_STATE_LIST_PTR = mpelAddr(MPEL_PAT_TABLE);
    MPEL_PAT_TABLE[0] = 4;
    MPEL_PAT_TABLE[1] = 4;
    *(volatile unsigned long int *)(MPEL_PAT_TABLE+2) = mpelAddr(MPEL_PAT1);
    *(volatile unsigned long int *)(MPEL_PAT_TABLE+4) = mpelAddr(MPEL_PAT2);
    *(volatile unsigned long int *)(MPEL_PAT_TABLE+6) = mpelAddr(MPEL_PAT3);
    *(volatile unsigned long int *)(MPEL_PAT_TABLE+8) = mpelAddr(MPEL_PAT4);
    return;
}

/***==================================================================***/

    /*
     * figure out how prect1 and prect2 overlap.
     * returns:
     *    rgnIN:	if prect1 contains prect2
     *	  rgnPART:	if prect1 and prect2 intersect
     *			but prect1 does not contain prect2
     *	  rgnOUT:	if prect1 and prect2 do not intersect
     */

int
mpelRectIntersect(prect1, prect2)
    register BoxPtr	prect1;
    register BoxPtr     prect2;
{

    TRACE(("mpelRectIntersect(0x%x,0x%x)\n",prect1,prect2));

    if ((prect2->x1>=prect1->x1)&&(prect2->x2<prect1->x2)&&
	(prect2->y1>=prect1->y1)&&(prect2->y2<prect1->y2)) {
	return rgnIN ;
    }
    if (prect1->x1 > prect2->x2)	/* rect1 right of rect2 */
	return rgnOUT ;

    if (prect1->x2 < prect2->x1)	/* rect1 left of rect2  */
	return rgnOUT ;

    if (prect1->y1 > prect2->y2)	/* rect1 below rect2 */
	return rgnOUT ;

    if (prect1->y2 < prect2->y1)	/* rect1 above rect2 */
	return rgnOUT ;

    return rgnPART ;
}
