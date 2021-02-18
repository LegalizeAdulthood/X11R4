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
#include <sys/hft.h>
#include <sys/time.h>
#include <fcntl.h>

#include "hftUtils.h"

#include "OScompiler.h"
#include "ibmTrace.h"
#include "ibmXhftMap.h"

extern	int	hftPending;
extern	int	hftQFD;
extern	int	hftNeedsReset;
extern	int	AIXDefaultDisplay ;

static	CurrentKeyClick = 0;	/* track current key click volume */

/***====================================================================***/

	/*
	 * Defines to simplify dealing with AIX.
	 */

#define	HFWDCHARS(n)	(n)>>8&0xff, (n)&0xff
#define	FLWDCHARS(n)	(n)>>24&0xff,(n)>>16&0xff,(n)>>8&0xff,(n)&0xff
#define	VTD(n)	HFINTROESC, HFINTROLBR, HFINTROEX, FLWDCHARS(n)


#define	HFTWRITE(s,m)	\
	if (write(hftQFD,&s,sizeof(s))!=sizeof(s)) {\
		ErrorF(m);\
	}

/***====================================================================***/
#ifdef AIXrt
int	mpel_hft_id_list[]    = {HFT_MEGAPEL_ID, 0} ;
int	apa16_hft_id_list[]   = {HFT_APA16_ID,   0} ;
int	apa8_hft_id_list[]    = {HFT_APA8_ID,    0} ;
int	apa8c_hft_id_list[]   = {HFT_APA8C_ID,   0} ;
int	ega_hft_id_list[]     = {HFT_EGA_ID,     0} ;
#else
#ifdef AIX386
int	vga_hft_id_list[]     = {HFT_VGA_8503_ID,   HFT_VGA_8512_ID,
				 HFT_VGA_8513_ID,   HFT_VGA_8514_ID,
				 HFT_VGA_8507_ID,   HFT_VGA_8604_ID,   0} ;
int	ibm8514_hft_id_list[] = {HFT_8514A_8503_ID, HFT_8514A_8512_ID,
				 HFT_8514A_8513_ID, HFT_8514A_8514_ID,
				 HFT_8514A_8507_ID, HFT_8514A_8604_ID, 0} ;
#else
ERROR!!! MUST BE RT/PC or PS/2 386
#endif /* AIX386 */
#endif /* AIXrt */

int
hftProbe( adapter )
int adapter ;
{
	int		*plist, *pcurrent ;
        hftDeviceID     *pDevices ;
        int             nDevices ;
        int             deviceID ;

        TRACE( ( "hftProbe(adapter=%d)\n", adapter ) ) ;

	switch ( adapter ) {
#if defined(AIXrt)
	case MEGAPELxHFTid :
		plist = mpel_hft_id_list ;
		break ;
	case APA16xHFTid :
		plist = apa16_hft_id_list ;
		break ;
	case APA8xHFTid :
		plist = apa8_hft_id_list ;
		break ;
	case EGAxHFTid :
		plist = ega_hft_id_list ;
		break ;
	case APA8CxHFTid :
		plist = apa8c_hft_id_list ;
		break ;
#endif /* AIXrt */
#if defined(AIX386)
	case VGAxHFTid :
		plist = vga_hft_id_list ;
		break ;
	case IBM8514xHFTid :
		plist = ibm8514_hft_id_list ;
		break ;
#endif /* AIX386 */
	default:
		ErrorF( "hftProbe: unknown adapter=%d\n" ) ;
		return -1 ;
	} /* End Switch */

        if ( !( nDevices = hftQueryDeviceIDs( &pDevices ) ) )
        	return -1 ; /* Can't Find it ! Nothing Available ! */

	while ( nDevices-- ) {
        	deviceID =
			HF_INT( pDevices[nDevices].hftDevID ) & HFT_DEVID_MASK ;
		for ( pcurrent = plist ; *pcurrent ; )
			if ( *pcurrent++ == deviceID ) {
				AIXDefaultDisplay = deviceID ;
				/* We MUST have permissions on return !! */
#if !defined(AIXrt)
				/* This is the only place we hftInitQueue */
				hftInitQueue( deviceID, FALSE ) ;
				return hftQFD ; /* "Magic" number */
#else
				return MAX( hftQFD, 0 ) ; /* "Magic" number */
#endif
			}
    	}
   	/* Didn't Find it ! */
   	return -1 ;
}

/***====================================================================***/

static	struct	hfkled	hftProtoLED =
    { VTD(sizeof(hftProtoLED)-3),  HFKLEDCH, HFKLEDCL, 2, 0 };

/*
 * Turn all lock LED indicators on or off
 */

SetLockLEDs (onoff)
int       onoff;
{
	hftProtoLED.hf_ledselect = HFCAPSLOCK | HFSCROLLOCK | HFNUMLOCK;
	hftProtoLED.hf_ledvalue = onoff;
    	HFTWRITE(hftProtoLED,"set all LEDs write failed\n");
}

/*
 * Turn caps lock LED indicator on or off
 */

void
SetCapsLockLED (onoff)
int       onoff;
{
	hftProtoLED.hf_ledselect = HFCAPSLOCK;
	hftProtoLED.hf_ledvalue = ( onoff ? HFCAPSLOCK : 0 );
    	HFTWRITE(hftProtoLED,"set CapsLockLED write failed\n");
    	return ;
}

/*
 * Turn num lock LED indicator on or off
 */
void
SetNumLockLED (onoff)
int       onoff;
{
	hftProtoLED.hf_ledselect = HFNUMLOCK;
	hftProtoLED.hf_ledvalue = ( onoff ? HFNUMLOCK : 0 );
    	HFTWRITE(hftProtoLED,"set NumLockLED write failed\n");
    	return ;
}

/*
 * Turn scroll lock LED indicator on or off
 */
void
SetScrollLockLED (onoff)
int       onoff;
{
	hftProtoLED.hf_ledselect = HFSCROLLOCK;
	hftProtoLED.hf_ledvalue = ( onoff ? HFSCROLLOCK : 0 );
    	HFTWRITE(hftProtoLED,"set ScrollLockLED write failed\n");
    	return ;
}

void
hftSetLEDS(which,num)
int which ;
int num ;
{
	/* take a short cut to analyze the mask */
	if (num & 1)
		SetCapsLockLED(1);
	else
		SetCapsLockLED(0);

	if (num & 2 )
		SetNumLockLED(1);
	else
		SetNumLockLED(0);

	if (num & 4 )
		SetScrollLockLED(1);
	else
		SetScrollLockLED(0);

	/* who cares !! */
    	return ;
}

/***====================================================================***/

static struct hfloth	hftProtoLocThresh =
    { VTD(sizeof(hftProtoLocThresh)-3), HFLOTHCH, HFLOTHCL, 2, 1 };

void
hftSetLocatorThresholds(horz,vert)
unsigned	horz,vert;
{
    TRACE(("hftSetLocatorThresholds(0x%x,0x%x)\n",horz,vert));
    hftProtoLocThresh.hf_hthresh[0]= (horz>>8)&0xff;
    hftProtoLocThresh.hf_hthresh[1]= (horz&0xff);
    hftProtoLocThresh.hf_vthresh[0]= (vert>>8)&0xff;
    hftProtoLocThresh.hf_vthresh[1]= (vert&0xff);
    HFTWRITE(hftProtoLocThresh,"set locator threshold write failed\n");
    return;
}

/***====================================================================***/

static struct hftdzone	hftProtoDZone =
    { VTD(sizeof(hftProtoDZone)-3), HFTDZCH, HFTDZCL, 2, 1 };

void
hftSetTableDeadZone(horz,vert)
unsigned	horz,vert;
{
    TRACE(("hftSetTabletDeadZone(0x%x,0x%x)\n",horz,vert));
    hftProtoDZone.hf_horizontal[0]= (horz>>8)&0xff;
    hftProtoDZone.hf_horizontal[1]= (horz&0xff);
    hftProtoDZone.hf_vertical[0]= (vert>>8)&0xff;
    hftProtoDZone.hf_vertical[1]= (vert&0xff);
    HFTWRITE(hftProtoDZone,"set tablet dead zone write failed\n");
    return;
}

/***====================================================================***/

static struct hfdial_lpfk	hftProtoSetLPFK =
    { VTD(sizeof(hftProtoSetLPFK)-3), HFLPFKSCH, HFLPFKSCL, 2, 1 };

void
hftSetLPFK(keys,flags)
unsigned	keys,flags;
{
    TRACE(("hftSetLPFK(0x%x,0x%x)\n",keys,flags));
    hftProtoSetLPFK.hf_mask.keys[0]= (keys>>24)&0xff;
    hftProtoSetLPFK.hf_mask.keys[1]= (keys>>16)&0xff;
    hftProtoSetLPFK.hf_mask.keys[2]= (keys>>8)&0xff;
    hftProtoSetLPFK.hf_mask.keys[3]= (keys)&0xff;
    hftProtoSetLPFK.hf_data2.lpfk.flags[0]=	(flags>>24)&0xff;
    hftProtoSetLPFK.hf_data2.lpfk.flags[1]=	(flags>>16)&0xff;
    hftProtoSetLPFK.hf_data2.lpfk.flags[2]=	(flags>>8)&0xff;
    hftProtoSetLPFK.hf_data2.lpfk.flags[3]=	(flags)&0xff;
    HFTWRITE(hftProtoSetLPFK,"set LPFK write failed\n");
    return;
}

/***====================================================================***/

static struct hfdial_lpfk	hftProtoSetGranularity =
    { VTD(sizeof(hftProtoSetGranularity)-3), HFDIALSCH, HFDIALSCL, 2, 1 };

void
hftSetDialGranularity(dials,settings)
unsigned	dials;
unsigned char	settings[16];
{
int	i;

    TRACE(("hftSetDialGranularity(0x%x,0x%x)\n",dials,settings));
    hftProtoSetGranularity.hf_mask.dials[0]= (dials>>24)&0xff;
    hftProtoSetGranularity.hf_mask.dials[1]= (dials>>16)&0xff;
    hftProtoSetGranularity.hf_mask.dials[2]= (dials>>8)&0xff;
    hftProtoSetGranularity.hf_mask.dials[3]= (dials)&0xff;
    for (i=0;i<16;i++) {
	hftProtoSetGranularity.hf_data2.granularity[i]=	settings[i];
    }
    HFTWRITE(hftProtoSetGranularity,"set dial granularity write failed\n");
    return;
}

/***====================================================================***/

static	struct	hfrconf	hftProtoReconfigure;

static struct hfsound	hftProtoSound =
    { VTD(sizeof(hftProtoSound)-3), HFSOUNDCH, HFSOUNDCL, 2, 1, HFEXECALWAYS};

void
hftSound(vol, duration,frequency)
int		vol;
unsigned	duration;
unsigned	frequency;
{
int	real_vol;

     TRACE(("hftSound(%d,%d)\n",duration,frequency));

     real_vol = getVolume (vol);
     if (real_vol == 0)
	return;

     /* set bell volume */
     if (real_vol != CurrentKeyClick) {
	hftProtoReconfigure.hf_op= HFCHGVOLUME;
	hftProtoReconfigure.hf_obj= real_vol;
	ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
     }

     /* set bell duration and frequency, then ring bell */
	hftProtoSound.hf_dur[0]= (duration>>8)&0xff;
	hftProtoSound.hf_dur[1]= (duration&0xff);
	hftProtoSound.hf_freq[0]= (frequency>>8)&0xff;
	hftProtoSound.hf_freq[1]= (frequency&0xff);
	HFTWRITE(hftProtoSound,"generate sound write failed\n");

     /* restore old value of keyclick */
     if (real_vol != CurrentKeyClick) {
	hftProtoReconfigure.hf_obj= CurrentKeyClick;
	ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
     }

     return;
}

/***====================================================================***/

static int
getVolume (volume)
int	volume;
{
	/* volume range 0-100 */

	if ((volume >= 1) && (volume <= 33))
            volume = 1;			/* low */
	else if ((volume >= 34) && (volume <= 66))
            volume = 2;			/* medium */
	else if (volume >= 67)
            volume = 3;			/* high */

	return (volume);
}

/***====================================================================***/

void
hftSetTypematicDelay(delay)
unsigned	delay;
{
    TRACE(("hftSetTypematicDelay(%d)\n",delay));
    hftProtoReconfigure.hf_op= HFCHGKBDDEL;
    hftProtoReconfigure.hf_obj= delay;
    ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
    return;
}

/***====================================================================***/

void
hftSetTypematicRate(rate)
unsigned	rate;
{
    TRACE(("hftSetTypematicRate(%d)\n",rate));
    hftProtoReconfigure.hf_op= HFCHGKBDRATE;
    hftProtoReconfigure.hf_obj= rate;
    ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
    return;
}

/***====================================================================***/

void
hftSetKeyClick(volume)
int	volume;
{

    TRACE(("hftSetKeyClick(%d)\n",volume));

    if (volume != 0) {
	hftProtoReconfigure.hf_op = HFCHGCLICK;
	hftProtoReconfigure.hf_obj = 1;		/* turn click on */
	ioctl(hftQFD, HFRCONF, &hftProtoReconfigure);
    }

    hftProtoReconfigure.hf_obj = CurrentKeyClick = getVolume(volume);
    hftProtoReconfigure.hf_op= HFCHGVOLUME;
    ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
    return;
}

/***====================================================================***/

void
hftSetLocatorSampleRate(rate)
unsigned	rate;
{
    TRACE(("hftSetLocatorSampleRate(%d)\n",rate));
    hftProtoReconfigure.hf_op= HFCHGLOCRATE;
    hftProtoReconfigure.hf_obj= rate;
    ioctl(hftQFD,HFRCONF,&hftProtoReconfigure);
    return;
}

/***====================================================================***/

volatile char hftQr[12] = {
	0, VTD(0), HFQDEVIDRH, HFQDEVIDRL, 2, 1
};

static struct hfqdevidc qdevidcmd =
	{ VTD(sizeof(qdevidcmd)-3), HFQDEVIDCH, HFQDEVIDCL };

static struct hfquery   query =    { qdevidcmd.hf_intro, sizeof( qdevidcmd ) };
static struct hfquery   tquery =   { qdevidcmd.hf_intro, sizeof( qdevidcmd ) };
static struct hfrconf   reconfig = { HFSETDD };

static struct {
		char    pad;
		char    hf_esc;
		char    hf_lbr;
		char    hf_ex;
		int     hf_len;
		char    hf_typehi;
		char    hf_typelo;
		char    hf_sublen;
		char    hf_subtype;
		short	hf_numdev;
		struct {
		    short	hf_idhi,hf_idlo;
		    short	hf_classhi,hf_classlo;
		} hf_devices[HFT_MAXDEVICES];
}  qDevIDResponse;

static	hftDeviceID hftDevices[HFT_MAXDEVICES];

int
hftQueryDeviceIDs(ppDevices)
hftDeviceID	**ppDevices;
{
int	tempfd= -1;
int	mustClose= FALSE;

    TRACE(("hftQueryDeviceIDs(0x%x)\n",ppDevices));

    if (hftQFD < 0) {
	if ( ( tempfd = open("/dev/console",O_WRONLY) ) < 0 ) {
	    ErrorF("Cannot open /dev/console to query device ids\n");
	    return 0;
	}
	mustClose= TRUE;
    }
    else
	tempfd= hftQFD;

    query.hf_resp=	&qDevIDResponse.hf_esc;
    query.hf_resplen=	sizeof(qDevIDResponse)-1;
    qDevIDResponse.hf_len= sizeof( qDevIDResponse ) - 4;

    if ( ioctl( tempfd, HFQUERY, &query ) ) {
/*	ErrorF( "unable to query display device ids\n" );*/
	if (mustClose)
	    close(tempfd);
	return 0;
    }
    if (mustClose)
	close(tempfd);
    MOVE( qDevIDResponse.hf_devices, hftDevices, sizeof hftDevices ) ;
    *ppDevices= hftDevices;
    return qDevIDResponse.hf_numdev ;
}

static struct hfqhftc qhft = { 0x1b,'[','x',(6)>>24, (6)>>16, (6)>>8, (6),
	0x01, 0x45 };
static struct hfquery   ttquery =    { qhft.hf_intro, sizeof( qhft ) };

unsigned
hftQueryCurrentDevice()
{
int		tempfd= -1;
unsigned	*retval;
volatile struct hfqhftr resp;

    TRACE(("hftQueryCurrentDevice()\n"));

    if (hftQFD<0) {
	tempfd = open("/dev/tty",O_WRONLY);
	if (tempfd<0) {
	    ErrorF("Cannot open /dev/tty to query device id\n");
	    return 0;
	}
    }
    else tempfd= hftQFD;

    ttquery.hf_resp    = resp.hf_intro;
    ttquery.hf_resplen = sizeof( resp ) + 1 ;

    if ( ioctl( tempfd, HFQUERY, &ttquery ) ) {
	ErrorF( "unable to query tty device id\n" );
	resp.hf_phdevid = 0 ;
    }

    if (tempfd!=hftQFD)
	close(tempfd);
    return resp.hf_phdevid ;
}

/***====================================================================***/

static struct hfqhftc   qhft_kb =   { VTD(6),
				      HFQHFTCH, HFQHFTCL } ;

volatile struct hfqhftr resp1;

static struct hfquery   query1 =    { qhft.hf_intro,
				      sizeof( qhft_kb ) };
int
hftGetKeyboardID ()
{
int	tempfd;

    TRACE(("hftGetKeyboardID(0x%x)\n"));

    query1.hf_resp    = resp1.hf_intro;
    query1.hf_resplen = sizeof( resp1 ) + 1 ;

    tempfd= hftQFD;
    if ( ioctl( tempfd, HFQUERY, &query1 ) ) {
	ErrorF("unable to query current keyboard");
	return HFT_ILLEGAL_KEYBOARD ;
    }
    return resp1.hf_phkbdid ;
}

/***====================================================================***/

int
hftSetDefaultDisplay(idWanted)
int	idWanted;
{
int        	 fd;

    TRACE(("hftSetDefaultDisplay(idWanted)\n",idWanted));
    if ( !idWanted )
	return TRUE;

    if (reconfig.hf_obj=hftHasAnAttached(idWanted)) {
	if ((hftQueryDefaultDisplay()&HFT_DEVID_MASK)!=idWanted) {
	    fd= open("/dev/console",O_WRONLY);
	    if (fd<0) {
		ErrorF("Unable to open /dev/console to set default display\n");
		return FALSE;
	    }
	    if ( ioctl( fd, HFRCONF, &reconfig ) ) {
		ErrorF( "unable to change default display\n" );
		(void) close(fd);
		return FALSE;
	    }
	    (void) close(fd) ;
	}
    }
    else {
	ErrorF( "requested default display not attached\n" );
	return FALSE;
    }
    return TRUE;
}

extern void ibmInfoMsg() ;

/***====================================================================***/

unsigned
hftQueryDefaultDisplay()
{
hftDeviceID *pDevices ;

    TRACE( ( "hftQueryDefaultDisplay()\n" ) ) ;
    return hftQueryDeviceIDs( &pDevices ) 
	? HF_INT( pDevices[0].hftDevID ) & HFT_DEVID_MASK : HFT_ILLEGAL_ID ;
}

/***====================================================================***/

int
hftHasAnAttached(devId)
unsigned int devId;
{
int        	 i;
int		 nDevices;
hftDeviceID	*pDevices;

    TRACE(("hftHasAnAttached(0x%x)\n",devId));
    nDevices= hftQueryDeviceIDs(&pDevices);

    if (nDevices==0)
	return(FALSE);

    for ( i = 0;  i < nDevices;  ++i ) {
	if ( (HF_INT(pDevices[i].hftDevID) & HFT_DEVID_MASK) == (devId&HFT_DEVID_MASK) ) {
	    return HF_INT(pDevices[i].hftDevID);
	}
    }
    return FALSE;
}

/***====================================================================***/

static	struct	hfchgdsp	changeDisplay = {
	VTD(sizeof(changeDisplay)), HFCHGDSPCH, HFCHGDSPCL, 2, 0,
	HFNONDEF, 0 };

void
hftChangePhysicalDisplay(device)
unsigned	device;
{
    TRACE(("hftChangePhysicalDisplay(0x%x)\n",device));
    if (hftQFD>=0) {
	changeDisplay.hf_devid[0]= (device>>24)&0xff;
	changeDisplay.hf_devid[1]= (device>>16)&0xff;
	changeDisplay.hf_devid[2]= (device>>8)&0xff;
	changeDisplay.hf_devid[3]= (device)&0xff;
	write( hftQFD, &changeDisplay, sizeof changeDisplay );
    }
    return;
}

/***====================================================================***/

int
hftFindAScreen(device)
unsigned	device;
{
unsigned oldDefault,thisDevice;
char	*thisTtyName;

    TRACE(("hftFindAScreen()\n"));
    if (hftQFD<0) {
	thisTtyName= (char *)ttyname(2);
	thisDevice= hftQueryCurrentDevice();
	if ((device!=0)&&(device!=thisDevice)) {
	    oldDefault= hftQueryDefaultDisplay();
	    if (oldDefault!=device)
		hftSetDefaultDisplay(device);
	    if ((hftQFD= open("/dev/hft",O_RDWR)) < 0) {
		ErrorF("error opening /dev/hft\nexiting\n");
		exit(1);
	    }
	    if (oldDefault!=device)
		hftSetDefaultDisplay(oldDefault);

	    /* 
	     * This is so that you don't go into hft wait when not starting
	     * X on the console.  (jsw)
	     */
	    hftNeedsReset= TRUE;
	}
	else if ((strncmp(thisTtyName,"/dev/hft/",9)==0)||
		 (strcmp(thisTtyName,"/dev/console")==0)) {
	    hftQFD= 2;
	    hftNeedsReset= TRUE;
	}

	/* 10/9/88 (ef) -- need to close stdout so that "unix:0" (or	*/
	/*	some other number) is flushed to xinit. sigh.		*/
	/* close(1) ; no nore sigh ... */
    }
    return(hftQFD);
}

/***====================================================================***/

	/*
	 * BlockHandler is called just before the server goes into select
	 * forever (effectively),  WakeupHandler is called when select
	 * returns (times out or is interrupted).
	 *
	 * Normally, the hft interrupt knocks us out of select, but not
	 * back to Dispatch() -- only to WaitForSomething().  WaitForSomething()
	 * does *not* check device input -- it loops back around to the block
	 * handler and select.  It doesn't return to dispatch until a client
	 * does something.  The WakeupHandler only needs to dispatch input
	 * events when we're waiting for clients in WaitForSomething().
	 *
	 * We need to set a timeout if input is pending, because
	 * hftDispatchEvents wants to be called periodically if it has
	 * a pending timeout, and it won't be generating any interrupts to
	 * blow the select away.
	 *
	 * waittime is used to get a reasonable amount of time for timeouts
	 * in the hft queue.  The value of the timeout below was derived
	 * experimentally -- I played with the length of the timeout
	 * until the mouse felt "right."
	 */

static	int	handleInput;

void
hftBlockHandler(scrNdx,blockData,ppTimeout,pReadmask)
	int	scrNdx;
	char	*blockData;
	struct timeval	**ppTimeout;
	unsigned	*pReadmask;
{
static	struct timeval waittime = { 0, 40000 };

    TRACE(("hftBlockHandler()\n"));

    if (hftPending) {
	*ppTimeout= &waittime;
	handleInput= 1;
    }
    else {
	hftInterruptAlways();
	handleInput= 0;
    }
    return;
}

/***====================================================================***/

void
hftWakeupHandler(scrNdx,blockData,pTimeout,pReadmask)
	int	scrNdx;
	char	*blockData;
	struct timeval	*pTimeout;
	unsigned	*pReadmask;
{
    TRACE(("hftWakeupHandler()\n"));
    if (handleInput) {
	hftDispatchEvents();
    }
    hftInterruptOnFirst();
    return;
}

#ifdef AIXEXTENSIONS
#include "AIX.h"
#include "AIXext.h"
#include "hftUtils.h"

extern AIXInfoRec aixInfo;
extern int aixTabletScaleX,aixTabletScaleY ;

  union
  {
    struct hfqdev   qdev;
    struct hfqdresp qdresp;
  } hfqdstuff;

static struct hfqhftc   qhft =      { VTD(6),
				      HFQHFTCH, HFQHFTCL } ;
volatile struct hfqhftr resp1;

static struct hfquery   query1 =    { qhft.hf_intro,
				      sizeof( qhft ) };
static struct hfqgraphdev qlocdev = { VTD( sizeof(qlocdev) - 3 ),
				      HFQLOCCH, HFQLOCCL };
union {
   struct hfqlocr loc;
   char junk[sizeof(struct hfqlocr)+1];
}   resp3;

static struct hfquery   query3 =    { qlocdev.hf_intro, sizeof(qlocdev),
				      resp3.junk, sizeof( resp3 ) };

void
hftQueryHardwareConfig(device)
unsigned	device;
{

    TRACE(("hftQueryHardwareConfig(0x%x)\n",device));

    hfqdstuff.qdev.hf_qdrsvd = 0;
    hfqdstuff.qdev.hf_qdopts = 2;
    hfqdstuff.qdev.hf_qdlen =
       sizeof(hfqdstuff) - sizeof(hfqdstuff.qdev);

    if (ioctl(hftQFD, HFQDEV, &hfqdstuff)) {
	ErrorF("Fatal!      unable to query hardware config\n" );
	ErrorF("            exiting\n");
	exit(1);
    }
    else {
	aixInfo.kbdiodn = hfqdstuff.qdresp.hf_keyiodn ;
	aixInfo.lociodn = hfqdstuff.qdresp.hf_lociodn ;
	aixInfo.dialiodn = hfqdstuff.qdresp.hf_dialsiodn ;
	aixInfo.lpfkiodn = hfqdstuff.qdresp.hf_lpfkiodn ;
    }

    aixInfo.vrmid =  device & 0xffff0000 ;

    switch(aixInfo.vrmid) {
	case HFT_APA8_ID:	aixInfo.displayid = XDEV_IBM_APA8 ;
				break;
	case HFT_APA8C_ID:	aixInfo.displayid = XDEV_IBM_APA8C ;
				break;
	case HFT_APA16_ID:	aixInfo.displayid = XDEV_IBM_APA16 ;
				break;
	case HFT_MEGAPEL_ID:	aixInfo.displayid = XDEV_IBM_MPEL ;
				break;
	default:		aixInfo.displayid = XDEV_IBM_GSL ;
    }

    if ( ioctl(hftQFD , HFQUERY, &query3 ) ) {
	ErrorF("Fatal!      unable to query locator\n" );
	ErrorF("            exiting\n");
    }

    if ( resp3.loc.hf_devinfo[0] & HFLOCABS ) {
	aixTabletScaleX = (resp3.loc.hf_horzmax_cnt[0] << 8)
			  + resp3.loc.hf_horzmax_cnt[1];
	aixTabletScaleY = (resp3.loc.hf_vertmax_cnt[0] << 8)
			  + resp3.loc.hf_vertmax_cnt[1];
	aixInfo.loctype = DEVTABLET ;
    }
    else {
	aixInfo.loctype = DEVMOUSE ;
    }

    aixInfo.kbdid = hftGetKeyboardID();
    aixInfo.inputfd = hftQFD ;
    return;
}

#endif
