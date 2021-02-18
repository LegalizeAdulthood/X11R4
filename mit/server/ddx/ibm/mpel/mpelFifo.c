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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/mpel/RCS/mpelFifo.c,v 6.2 89/01/31 11:46:13 paul Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/mpel/RCS/mpelFifo.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/mpel/RCS/mpelFifo.c,v 6.2 89/01/31 11:46:13 paul Exp $" ;
#endif

/***==================================================================***/

#include "Xmd.h"

#include "mpelHdwr.h"
#include "mpelFifo.h"

#include "ibmTrace.h"

#define DELAY_ADDR	0xf00080E0		/* IOCC delay location */
#define DELAY(n) \
	{ register int N = (n) ; \
		while ( --N >= 0 ) \
		* (volatile char *) DELAY_ADDR = 0xff ;	}

/***==================================================================***/

#ifdef	PORTABLE
/*
 *  mpelWFifo writes commands to the megapel fifo.
 */

int
mpelWFifo( nargs, command, arg1, arg2 )
    int		 nargs ;
    unsigned short int command ;
    unsigned long int arg1 ;
    unsigned long int arg2 ;
{
    int		 retry ;		/* Current RETRY count	     */
    unsigned short int tmp[2] ;
    int		 out_data ;		/* num of bytes in command */
    register unsigned short int *data_ptr;
    register volatile unsigned short *psr= &MPEL_PSR;
    register volatile unsigned short *fifo= &MPEL_FIFO;
    register 		    int	 i;
    register int data_len ;		/* num of bytes we have to send... */


    TRACE(("mpelWFifo(%d,0x%x,0x%x,0x%x)\n",nargs,command,arg1,arg2)) ;

    if ( !command ) {
	data_len = nargs ;
	if ( data_len & 0x1 )
		data_len++ ;
	out_data = data_len ;
	data_ptr = (unsigned short int *) arg1 ;
    }
    else if ( nargs <= 2 ) {
	out_data = data_len = 4 + ( nargs * 2 ) ;
	tmp[0] = (unsigned short int) arg1 ;
	tmp[1] = (unsigned short int) arg2 ;
	data_ptr = &tmp[0] ;
    }
    else {
	data_len = nargs ;
	out_data = nargs + arg2 ;
	if ( out_data & 0x1 )
		out_data++ ;
	data_ptr = (unsigned short int *) arg1 ;
    }

    if ( command ) {
	while ( !( *psr & PSR_FIFO_NOT_FULL ) ) /* DO NOTHING */ ;
	MPEL_FIFO = out_data ;

	while ( !( *psr & PSR_FIFO_NOT_FULL ) ) /* DO NOTHING */ ;
	MPEL_FIFO = command ;
	data_len -= 4 ;
    }

    retry = 0 ;
    while ( data_len > 0 ) {
	/*
	 * figure out how many bytes we can safely write to the fifo.
         */
	if ( *psr & PSR_FIFO_NOT_HALF_FULL ) {
	    i = MIN( (MPEL_FIFO_LEN/2) , data_len ) ;
	    data_len -= i ;
	    while ( i > 0) {
		*fifo= *data_ptr++ ;
		i -= 2 ;
	    }
	}
	else if ( *psr & PSR_FIFO_NOT_FULL ) {
	    *fifo = *data_ptr++;
	    data_len -= 2;
	}
	else {
	    if ( retry > MPEL_MAXRETRY ) {
		ErrorF("WSGO!! FIFO jammed -- MPEL_MAXRETRY reached\n") ;
		return -1  ; /* give up */
	    }
	    DELAY( 1024 ) ;
	    retry++ ;
	    continue ;
	}
    } /* end of while loop */
    return 0 ;
}

#endif /* PORTABLE */

/***==================================================================***/

void mpelStartFifo()
{
    TRACE(("mpelStartFifo()\n")) ;

    MPEL_COMM_REQ = MPELCMD_ENTER_FIFO ;
    MPEL_COMM_REASON = 0 ;
    MPEL_PCR &= ~ PCR_INTR_TMS ;
    MPEL_PCR |= PCR_INTR_TMS ;
    return ;
}

/***==================================================================***/

void mpelContFifo()
{
    TRACE(("mpelContFifo()\n")) ;

    MPEL_COMM_REQ = MPELCMD_CONT_FIFO ;
    MPEL_COMM_REASON = 0 ;
    MPEL_PCR &= ~ PCR_INTR_TMS ;
    MPEL_PCR |= PCR_INTR_TMS ;
    return ;
}
