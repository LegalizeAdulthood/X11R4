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
#include <fcntl.h>
#include <signal.h>

#define	HFRDATA	8192

#include <sys/hft.h>
#include <sys/time.h>

#include "ibmTrace.h"

#include "hftQueue.h"

	/*
	 * Defines to simplify dealing with AIX.
	 */

#define	HFWDCHARS(n)	(n)>>8&0xff, (n)&0xff
#define	FLWDCHARS(n)	(n)>>24&0xff,(n)>>16&0xff,(n)>>8&0xff,(n)&0xff
#define	VTD(n)	HFINTROESC, HFINTROLBR, HFINTROEX, FLWDCHARS(n)

#define	ringlen	sizeof(hftRing.buf)
#define	RINGLEN	HFWDCHARS(ringlen)
#define	RINGVTD	VTD(sizeof(hftRing) - 3)
#define	RINGOFFSET	FLWDCHARS(sizeof(hftRing) - sizeof(hftRing.buf) -\
				  sizeof(hftRing.req.hf_intro) - \
				  sizeof(hftRing.req.hf_sublen) -\
				  sizeof(hftRing.req.hf_subtype))
#define RINGBASE	((unsigned char *)&hftRing.buf.hf_rsvd[0])
#define	RINGFIRST	32
#define	RINGLAST	sizeof(hftRing.buf)

#define	HFT_ESC_KEYBOARD	0x77
#define	HFT_ESC_LOCATOR		'y'
#define	HFT_ESC_ADAPT		'r'

	/*
	 * Package global variables.
	 */

	unsigned	hftPending;
	unsigned	hftGrantPending;
	unsigned	hftRetractPending;
static	int		hftSetInterrupts= 0;
static	int		hftHaveScreen=	FALSE;
	int		hftQFD=	-1;
	int		hftNeedsReset=  FALSE;

static	hftEvHandler	hftHandlers[HFT_NEVENTS]= {
				HFT_IGNORE,
				HFT_IGNORE,
				HFT_IGNORE,
				HFT_IGNORE,
				HFT_IGNORE,
				HFT_IGNORE,
				HFT_IGNORE,
				HFT_IGNORE,
				HFT_IGNORE,
				HFT_IGNORE
			};

static	int		hftEvSize[HFT_NEVENTS]= { 
				sizeof(hftKeyEv),
				sizeof(hftLocEv),
				sizeof(hftAdaptEv),
				sizeof(hftLocEv),
				sizeof(hftLPFKEv),
				sizeof(hftDialEv)
			};

#define	HFT_N_LOC_STYPES	4
static	int		hftLocatorSubtype[HFT_N_LOC_STYPES] = {
			HFT_LOCATOR,	/* mouse report */
			HFT_TABLET,	/* locator report */
			HFT_LPFK,	/* LPFK report */
			HFT_DIAL	/* Dial report */
};

static	struct	hfprotocol	hftProtoReq= { 
		VTD(sizeof(hftProtoReq)-3),	/* escape sequence introducer */
		HFMOMPROH, HFMOMPROL, 2, 0	/* random garbage */
	};

static	struct {
	struct hfmomscreq	req;
	struct 	hfmomring	buf;
} hftRing= {
	RINGVTD,HFMOMREQH,HFMOMREQL,2,0,RINGLEN,RINGOFFSET,
	0,0,0xff,0,RINGFIRST,RINGFIRST,0,0,0,0,0,
 	};

static	hftEvent	hftProtoArrived= { HFT_EVENT_ARRIVED };
static	hftEvent	hftProtoGranted= { HFT_GRANTED };
static	hftEvent	hftProtoRetracted= { HFT_RETRACTED };
static	hftEvent	hftProtoBefore= { HFT_BEFORE_EVENTS };
static	hftEvent	hftProtoAfter= { HFT_AFTER_EVENTS };

static	hftEvHandler	hftTimeoutHndlr;
static	int		hftTimeoutCount;

/***====================================================================***/

void
hftInterruptAlways()
{
    TRACE(("hftInterruptAlways()\n"));
    hftRing.buf.hf_intreq= hftSetInterrupts= 0xff;
    return;
}

/***====================================================================***/

void
hftInterruptOnFirst()
{
    TRACE(("hftInterruptOnFirst()\n"));
    hftRing.buf.hf_intreq= hftSetInterrupts= 0;
    return;
}

/***====================================================================***/

static	int
hftIgnoreMsg()
{
    TRACE(("hftIgnoreMsg()\n"));
    hftRing.buf.hf_intreq= hftSetInterrupts;
    hftPending++;
    if (hftHandlers[HFT_EVENT_ARRIVED])
	(*hftHandlers[HFT_EVENT_ARRIVED])(&hftProtoArrived);
    return 1;
}

static int
hftGrant()
{
    TRACE(("hftGrant()\n"));
    hftGrantPending++;
    hftPending++;
    if (hftHandlers[HFT_EVENT_ARRIVED])
	(*hftHandlers[HFT_EVENT_ARRIVED])(&hftProtoArrived);
    return 1;
}

static int
hftRetract()
{
    TRACE(("hftRetract()\n"));
    hftRetractPending++;
    hftPending++;
    if (hftHandlers[HFT_EVENT_ARRIVED])
	(*hftHandlers[HFT_EVENT_ARRIVED])(&hftProtoArrived);
    return 1;
}

static void
hftSetUpSignals(save)
int	save;
{
struct	sigvec	svgrant;
struct	sigvec	svretract;
struct	sigvec	svmessage;
struct	sigvec	oldsig;

    TRACE(("hftSetUpSignals()\n"));
    svgrant.sv_handler=		hftGrant;
    svgrant.sv_mask= 		(1<<SIGGRANT)|(1<<SIGRETRACT);
    svgrant.sv_onstack=		0;
    svretract.sv_handler=	hftRetract;
    svretract.sv_mask= 		(1<<SIGGRANT)|(1<<SIGRETRACT);
    svretract.sv_onstack=	0;
    svmessage.sv_handler=	hftIgnoreMsg;
    svmessage.sv_mask= 		(1<<SIGMSG);
    svmessage.sv_onstack=	0;
    sigvec(SIGGRANT,&svgrant,&oldsig);
    if (save) {
	if ((oldsig.sv_handler!=SIG_DFL)&&(oldsig.sv_handler!=SIG_IGN))
	    hftHandlers[HFT_GRANTED]= oldsig.sv_handler;
    }
    sigvec(SIGRETRACT,&svretract,&oldsig);
    if (save) {
	if ((oldsig.sv_handler!=SIG_DFL)&&(oldsig.sv_handler!=SIG_IGN))
	    hftHandlers[HFT_RETRACTED]= oldsig.sv_handler;
    }
    sigvec(SIGMSG,&svmessage,&oldsig);
    if (save) {
	if ((oldsig.sv_handler!=SIG_DFL)&&(oldsig.sv_handler!=SIG_IGN))
	    hftHandlers[HFT_EVENT_ARRIVED]= oldsig.sv_handler;
    }
    return;
}

	
/***====================================================================***/

	/*
	 * defined in hftQueue.h
	 */

int
hftInitQueue(deviceId,saveOldHandlers)
int	deviceId;
int	saveOldHandlers;
{
struct	hfsmon	flags;
unsigned	offset;

    TRACE(("hftInitQueue()\n"));

    if (hftQFD<0) {
	if ( ( hftQFD = hftFindAScreen(deviceId) )<0 ) {
	    ErrorF("Fatal!      error opening /dev/hft\n");
	    ErrorF("            exiting\n");
	    exit(1);
	}
    }
    hftSetUpSignals(saveOldHandlers);
    flags.hf_momflags= HFSINGLE;

/*
 * once they fix hft.h this should be un-necessary, but will still work
 */
#ifndef MAX_MON_ADDRS
#define MAX_MON_ADDRS 1
#endif /* MAX_MON_ADDRS */

#ifdef AIX386
    flags.hf_momscnt= MAX_MON_ADDRS;
#endif /* AIX386 */

    if (ioctl(hftQFD,HFSMON,&flags)!=0) {
	ErrorF("Fatal!      error entering monitor mode\n");
	ErrorF("            exiting\n");
	exit(1);
    }
    hftProtoReq.hf_select[0]=		HFXLATKBD;
    hftProtoReq.hf_select[1]=		HFLOCATOR | HFDIALS | HFLPFKS ;
    hftProtoReq.hf_value[0]=		0;
    hftProtoReq.hf_value[1]=		HFLOCATOR | HFDIALS | HFLPFKS ;
    if (write(hftQFD,&hftProtoReq,sizeof(hftProtoReq))!=sizeof(hftProtoReq)) {
	ErrorF("Fatal!      protocol write failed\n");
	ErrorF("            exiting\n");
	exit(1);
    }

    hftRing.buf.hf_intreq= hftSetInterrupts;
    if (write(hftQFD,&hftRing,sizeof(hftRing))!=sizeof(hftRing)) {
	ErrorF("Fatal!      request write failed\n");
	ErrorF("            exiting\n");
	exit(1);
    }
    hftPending= 0;

#ifdef AIXEXTENSIONS
    hftQueryHardwareConfig(deviceId);
#endif
    return(TRUE);
}

/***====================================================================***/

static	struct	hfprotocol	hftKSRProtoReq= { 
		VTD(sizeof(hftProtoReq)-3),	/* escape sequence introducer */
		HFKSRPROH, HFKSRPROL, 2, 0	/* random garbage */
	};

static	struct	hfmomscrel	hftVTDRelease= {
					VTD(sizeof(hftVTDRelease)-3),
					HFMOMRELH,HFMOMRELL
				};

#include <stdio.h>
void
hftTermQueue()
{
int	rtrn;

   TRACE(("hftTermQueue()\n"));
   if (hftNeedsReset) {
	rtrn= write(hftQFD,&hftVTDRelease,sizeof(hftVTDRelease));
	if (rtrn!=sizeof(hftVTDRelease)) {
	    ErrorF("WSGO!	hftTermQueue release screen VTD failed (%d)\n",
									rtrn);
	}
	hftKSRProtoReq.hf_select[0]= HFHOSTS|HFXLATKBD;
	hftKSRProtoReq.hf_value[0]= HFXLATKBD;
	hftKSRProtoReq.hf_select[1]= HFWRAP|HFLOCATOR|HFLPFKS|HFDIALS;
	hftKSRProtoReq.hf_value[1]= HFWRAP;
	rtrn= write(hftQFD,&hftKSRProtoReq,sizeof(hftKSRProtoReq));
	if (rtrn!=sizeof(hftKSRProtoReq)) {
	    ErrorF("WSGO!	hftTermQueue KSR protocol VTD failed (%d)\n",
									rtrn);
	}
	rtrn= ioctl(hftQFD,HFCMON,0);
	if (rtrn!=0) {
	    ErrorF("WSGO!	hftTermQueue CMON ioctl failed\n",rtrn);
	}
	hftQFD= -1;
	hftNeedsReset= 0;
   }
   return;
}

/***====================================================================***/

	/*
	 * described in hftQueue.h
	 */

hftEvHandler
hftInstallHandler(evType,pHandler)
int	evType;
hftEvHandler	pHandler;
{
hftEvHandler	pOld;

    TRACE(("hftInstallHandler(%d,0x%x)\n",evType,pHandler));
    if ((hftQFD==-1)||(evType<0)||(evType>=HFT_NEVENTS)) {
	return(HFT_ERROR);
    }
    pOld= hftHandlers[evType];
    hftHandlers[evType]= pHandler;
    return(pOld);
}

	/*
	 *  described in hftQueue.h
	 */

void
hftAddTimeout(pHandler,count)
hftEvHandler	pHandler;
int		count;
{
    TRACE(("hftAddTimeout(0x%x,%d)\n",pHandler,count));

    if ((count>0)&&(pHandler!=NULL)) {
	hftTimeoutHndlr=	pHandler;
	hftTimeoutCount=	count;
    }
    else {
	hftTimeoutHndlr=	NULL;
	hftTimeoutCount=	0;
    }
    return;
}

#define	HFT_HEADERSIZE	3
#define	HFT_BYTESLEFT	(source>sink?source-sink:source-sink+HFRDATA)
#define	HFT_INCR(s)	(++s>=RINGLAST?s=RINGFIRST:0)

void
hftDispatchEvents()
{
register 	  char	*input= hftRing.buf.hf_rdata-RINGFIRST;
register unsigned	 source= hftRing.buf.hf_source;
register unsigned	 sink= hftRing.buf.hf_sink;
register unsigned char	*buf;
	 hftEvent  	 thisEvent;
	 int		 nPending,tmp;
	 int	 	 device;

    TRACE(("hftDispatchEvents()\n"));

    if ((hftPending)&&(hftHandlers[HFT_BEFORE_EVENTS])) {
	(*hftHandlers[HFT_BEFORE_EVENTS])(&hftProtoBefore);
    }

    if (hftGrantPending) {
	if (hftHandlers[HFT_GRANTED])
	    (*hftHandlers[HFT_GRANTED])(&hftProtoGranted);
	hftGrantPending= 0;
    }
    while (sink!=source) {
	if (HFT_BYTESLEFT<HFT_HEADERSIZE)  {
	    if (hftRing.buf.hf_ovflow)	goto incomplete;
	    else			break;
	}
	if (input[sink]!=27)	goto illegal;	/* XXX -- UGLY!! */
	HFT_INCR(sink);
	if (input[sink]!='[')	goto illegal;	/* XXX -- UGLY!! */
	HFT_INCR(sink);
	if	(input[sink]==HFT_ESC_KEYBOARD)		device=	HFT_KEYBOARD;
	else if	(input[sink]==HFT_ESC_LOCATOR)		device=	HFT_LOCATOR;
	else if	(input[sink]==HFT_ESC_ADAPT)		device=	HFT_ADAPT;
	else 						goto illegal;
	HFT_INCR(sink);
	nPending= hftEvSize[device];

	if (HFT_BYTESLEFT<nPending)
	    goto incomplete;
	buf= (unsigned char *)&thisEvent.hftEv;
	while (nPending--) {
	    *buf++= input[sink];
	    HFT_INCR(sink);
	}
	hftRing.buf.hf_sink= sink;

	if (device==HFT_LOCATOR)
	    device= hftLocatorSubtype[thisEvent.hftEv.hftLoc.leStype];

	if (hftHandlers[device]!=NULL) {
	    (*hftHandlers[device])(&thisEvent);
	}
    }
    if (hftRetractPending) {
	if (hftHandlers[HFT_RETRACTED]) 
	    (*hftHandlers[HFT_RETRACTED])(&hftProtoRetracted);
	hftRetractPending= 0;
	write(hftQFD,&hftVTDRelease,sizeof(hftVTDRelease));
    }

    if (hftTimeoutCount>0) {
	if (--hftTimeoutCount==0)
	    (*hftTimeoutHndlr)();
    }

    if ((sink==hftRing.buf.hf_source)&&(hftTimeoutCount<=0))
	hftPending= 0;


    if (hftHandlers[HFT_AFTER_EVENTS]) {
	(*hftHandlers[HFT_AFTER_EVENTS])(&hftProtoAfter);
    }

    return;
	/*
	 * Called when a partial event is sitting on the queue.
	 * if there is an overflow, print a warning and reset the
	 * queue.  Could do some fancy attempts to parse event if
	 * we really wanted to.  If there is no overflow, just return.
	 */
incomplete:
	if (hftRing.buf.hf_ovflow) {
	    ErrorF("warning!    ring buffer overflow!\n");
	    ErrorF("            events may be lost\n");
	    hftRing.buf.hf_sink= hftRing.buf.hf_source;
	    hftRing.buf.hf_ovflow= 0;
	    hftPending= 0;
	}
	return;
	/*
	 * called when an illegal character is found in a ring buffer
	 * structure header (or an illegal device is specified).
	 * prints a warning and finds the next legal header.
	 */
illegal:
	ErrorF("internal error:  illegal character in ring buffer header\n");
	ErrorF("                 events may be lost\n");
	while (sink!=hftRing.buf.hf_source) {
	    if (input[sink]==27) {
		hftRing.buf.hf_sink= sink;
		HFT_INCR(sink);
		if (input[sink]!='[')	continue;
		HFT_INCR(sink);
		if ((input[sink]==HFT_ESC_KEYBOARD)||
					(input[sink]==HFT_ESC_LOCATOR)||
					(input[sink]==HFT_ESC_ADAPT)) {
		    break;
		}
	    }
	    else HFT_INCR(sink);
	}
	hftRing.buf.hf_sink= sink;
	if (sink==hftRing.buf.hf_source)
	    hftPending= 0;
	return;
}

