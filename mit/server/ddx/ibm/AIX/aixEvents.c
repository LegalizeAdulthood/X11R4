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

#ifndef lint
static char sccsid[] = "@(#)hftevents.c	3.PA 88/12/06";
#endif

#include <sys/types.h>
#include <sys/times.h>
#include <sys/hft.h>

#define NEED_EVENTS
#define NEED_REPLIES
#include "X.h"
#include "Xproto.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "input.h"
#include "inputstr.h"

#include "OScompiler.h"
#include "OSio.h"
#include "OScursor.h"

#include "ibmIO.h"
#include "ibmMouse.h"
#include "ibmKeybd.h"
#include "ibmScreen.h"
#include "ibmCmdArgs.h"

#include "aixModKeys.h"
#include "hftQueue.h"

#include "ibmTrace.h"

#ifdef AIXEXTENSIONS

#include "AIX.h"
#include "AIXproto.h"
#include "AIXext.h"
extern DevicePtr rtDial ;
extern DevicePtr rtLpfk ;

#endif

extern  int      screenIsSaved;
extern  int	 kbdType ;

static  hftEvent delayedEvent;
static  int      delayed_left;
static  int      delayed_right;
static  int      delayed_middle;
static  unsigned char lastButtons = 0x00 ;
static	short	 lastModKeys;
static  int      pendingX;
static  int      pendingY;

static long clock = 0;

#define GET_OS_TIME() (GetTimeInMillis())

/***====================================================================***/

	/*
	 * 11/19/88 (ef) -- XXX!! Not really sure where these should live.
	 *	aixCursor, maybe?
	 *	Actually, once all of our systems give relative coordinates,
	 *	this can move to common.
	 */

int	AIXCurrentX = 0;
int	AIXCurrentY = 0;

/***====================================================================***/

static void
AIXFlushMouse()
{
register	int	x,y;
register	ibmPerScreenInfo	*screenInfo;
		int	oldScr;
		int	setCursor;
		xEvent	e;

    TRACE(("AIXFlushMouse\n"));
    if ((pendingX)||(pendingY)) {
	ibmAccelerate(pendingX,pendingY);
	x= AIXCurrentX+pendingX;
	y= AIXCurrentY-pendingY;
	screenInfo = ibmScreens[ oldScr = ibmCurrentScreen ];
	setCursor = FALSE;

	if (ibmYWrapScreen) {
	    while ( y<0 ) {
		y+=	screenInfo->ibm_ScreenBounds.y2;
		setCursor= TRUE;
	    }
	    while ( y>screenInfo->ibm_ScreenBounds.y2 ) {
		y-=	screenInfo->ibm_ScreenBounds.y2;
		setCursor= TRUE;
	    }
	}

	while ( x<0 ) {
	    if ( screenInfo == ibmScreens[ 0 ] ) {
		/* Already at first possible screen */
		if ( ibmXWrapScreen ) {
		    screenInfo = 
			ibmScreens[ ibmCurrentScreen = ibmNumScreens -1 ];
		    x+= screenInfo->ibm_ScreenBounds.x2;
		}
		else {
		    x= 0;
		}
	    }
	    else {
		screenInfo= ibmScreens[ --ibmCurrentScreen ];
		x+= screenInfo->ibm_ScreenBounds.x2;
	    }
	    setCursor= TRUE;
	}
	while ( x>screenInfo->ibm_ScreenBounds.x2 ) {
	    if ( screenInfo == ibmScreens[ ibmNumScreens - 1 ] ) {
		/* Already at last possible screen */
		if ( ibmXWrapScreen ) {
		    x-= screenInfo->ibm_ScreenBounds.x2;
		    screenInfo= ibmScreens[ ibmCurrentScreen = 0 ];
		}
		else {
		    x= screenInfo->ibm_ScreenBounds.x2;
		}
	    }
	    else {
		x-=  screenInfo->ibm_ScreenBounds.x2;	/* now x>0 */
		screenInfo= ibmScreens[ ++ibmCurrentScreen ];
	    }
	    setCursor= TRUE;
	}
	if (y>screenInfo->ibm_ScreenBounds.y2) {
	    y= screenInfo->ibm_ScreenBounds.y2;
	    setCursor= TRUE;
	}
	if (y<0) {
	    y= 0;
	    setCursor= TRUE;
	}
	if (setCursor) {
	    /* OS-DEPENDENT MACRO GOES HERE!!
	     * MACRO DEFINED IN FILE ibmos.h
	     * TELL OS THAT CURSOR HAS MOVED 
	     * TO A NEW POSITION
	     */
	    OS_TellCursorPosition(x,y);
	}
#if !defined(AIXrt) || defined(MULTIPLE_HEADS)
	if ( oldScr != ibmCurrentScreen ) {
	    (*ibmHideCursor( oldScr ) )( oldScr ) ;
	    NewCurrentScreen( screenInfo->ibm_Screen, x, y ) ;
	    if ( screenInfo->ibm_CurrentCursor != ibmCurrentCursor( oldScr ) ) {
		(* screenInfo->ibm_Screen->RealizeCursor )(
			screenInfo->ibm_Screen,
			ibmCurrentCursor( oldScr ) ) ;
	    }
	    (* screenInfo->ibm_Screen->DisplayCursor )(
			screenInfo->ibm_Screen,
			ibmCurrentCursor( oldScr ) ) ;
	}
#endif /* MULTIPLE HEADS */

	(* screenInfo->ibm_CursorShow )( x, y ) ;

	e.u.u.type= 			MotionNotify;
	e.u.keyButtonPointer.rootX=	AIXCurrentX= x;
	e.u.keyButtonPointer.rootY=	AIXCurrentY= y;
	e.u.keyButtonPointer.time=	lastEventTime = GET_OS_TIME();
	pendingX= pendingY= 0;
	(*ibmPtr->processInputProc)(&e,ibmPtr,1);
    }
}

/***====================================================================***/

	/*
	 * All of this keyboard stuff needs to
	 * be reorganized anyway (into osKeymap.h and a single keyboard.c)
	 */


#define	LSHIFT	(HFUXLSH)
#define	RSHIFT	(HFUXRSH)
#define	RALT	(HFUXRALT)
#define	LALT	(HFUXLALT)
#ifdef DO_CTRL_KEY_CORRECTLY
#define	CTRL	(HFUXCTRL<<8)
#else
#define CTRL	0
#endif

#define	MODKEYMASK	(LSHIFT|RSHIFT|RALT|LALT|CTRL)
#define	setModKeyState(ms,pe)	\
		((ms)=((((pe)->keStatus[0]<<8)|(pe)->keStatus[1])&MODKEYMASK))

#define	FAKEEVENT(ev,key,up)	{\
	(ev)->u.u.detail= (key);\
	(ev)->u.u.type= ((up)?KeyRelease:KeyPress);\
	(*ibmKeybd->processInputProc)((ev),ibmKeybd,1);\
	}

void
AIXFakeModKeyEvent(changedModKeys)
unsigned short	changedModKeys;
{
xEvent          e;

    TRACE(("AIXFakeModKeyEvent(0x%x)\n",changedModKeys));
    e.u.keyButtonPointer.rootX= AIXCurrentX;
    e.u.keyButtonPointer.rootY= AIXCurrentY;
    e.u.keyButtonPointer.time=  lastEventTime= GET_OS_TIME();
    if (changedModKeys&LSHIFT) {
	FAKEEVENT(&e,Aix_Shift_L,(lastModKeys&LSHIFT));
    }
    if (changedModKeys&RSHIFT) {
	FAKEEVENT(&e,Aix_Shift_R,(lastModKeys&RSHIFT));
    }
    if (changedModKeys&LALT) {
	FAKEEVENT(&e,Aix_Alt_L,(lastModKeys&LALT));
    }
    if (changedModKeys&RALT) {
	FAKEEVENT(&e,Aix_Alt_R,(lastModKeys&RALT));
    }


#ifdef DO_CTRL_KEY_CORRECTLY
    if (changedModKeys&CTRL) {
	FAKEEVENT(&e,Aix_Control_L,(lastModKeys&CTRL));
    }
#endif
    return;
}

/***====================================================================***/

/* 12/22/88 (ef) -- tracking down right-ctrl weirdness */
static int evdebug= 0;

	/*
	 * 11/17/88 (ef) -- XXX This function is too big -- the kanji
	 *	stuff and caps/num lock stuff should be seperated out.
	 */

static int
AIXKbdEvent(pEv)
hftEvent	*pEv;
{
hftKeyEv	*pKey= &pEv->hftEv.hftKey;
unsigned short	theseModKeys;
DeviceIntPtr	KeyboardPtr;
xEvent		e;
int             key;
static	int	kanjiCaps = 0 ;
static	int	kanjiLeftAlt = 0 ;

    TRACE(("AIXKbdEvent (%d)\n",pKey->kePos));

    key = pKey->kePos + AIX_MIN_KEY ;

    /* toggle 106 Caps Lock */
    /* wait for internal apar */

    /*
    if (kbdType == HF106KBD) {

    switch(key) {
    case   Aix_Alt_R:
    if( KeyboardPtr->key->modifierMap[key] & Mod1Mask )
    {
	if( KeyboardPtr->key->down[key >> 3] & (1 << (key & 7)) )
		kanjiLeftAlt = 1 ;
	else
		kanjiLeftAlt = 0 ;
    }
	break ;
   case   Aix_Alt_L :
   if (kanjiCaps == 0 && key == Aix_Alt_L && kanjiLeftAlt)
   {
	     SetCapsLockLED(1);
	     kanjiCaps = 1 ;
   }
	break ;
   default :
	break ;

   }

   }
   */

    if ((pendingX)||(pendingY))
	AIXFlushMouse();

    setModKeyState(theseModKeys,pKey);
    if (theseModKeys!=lastModKeys) {
	AIXFakeModKeyEvent(theseModKeys^lastModKeys);
    }

    lastModKeys= theseModKeys;

    switch (key) {
	case Aix_Alt_L:     
	/*
	 * 11/19/88 (ef) -- XXX!  Special case -- don't know where to get
	 *	keyboard IDs from, Kanji keyboard broken for now.
	 */
#ifdef NOTDEF
		{
		if (kbdType == HF106KBD)	/* special case */
			break ;
		else
			return(1);
		}
#endif
	case Aix_Shift_L:  case Aix_Shift_R:
        case Aix_Alt_R:
#ifdef DO_CTRL_KEY_CORRECTLY
	case Aix_Control_L:
#endif
			return(1);
	case Aix_Backspace:
		if ((!ibmDontZap)&&(pKey->keStatus[0]&HFUXCTRL)&&
					(pKey->keStatus[0]&HFUXALT)) {
		    GiveUp();	
		}
		break;
	default:		
		break;
    }

    if ((!ibmKeyRepeat)&&pKey->keStatus[1]&HFUXRPT)
	return(1);

    e.u.keyButtonPointer.rootX=	AIXCurrentX;
    e.u.keyButtonPointer.rootY=	AIXCurrentY;
    e.u.keyButtonPointer.time=	lastEventTime= GET_OS_TIME();
    e.u.u.detail= key ;
    if (pKey->keStatus[1]&HFUXRPT)      {
	e.u.u.type= KeyRelease;
	(*ibmKeybd->processInputProc)(&e,ibmKeybd,1);
	e.u.u.type= KeyPress;
    }
    else if (pKey->keStatus[0]&HFUXMAKE)	e.u.u.type= KeyPress;
    else					e.u.u.type= KeyRelease;

	/*
	 * 11/19/88 (ef) -- XXX! Weird!  can't find HF10?KBD anywhere!!
	 */
#ifdef NOTDEF
    if (kbdType == HF101KBD || kbdType == HF102KBD)
#endif
    {

    /*
     *	toggle lock shift keys:
     *	ignore releases, toggle on & off on presses
     */

    KeyboardPtr = (DeviceIntPtr)ibmKeybd;

    if( KeyboardPtr->key->modifierMap[key] & LockMask )
    {
	if( e.u.u.type == KeyRelease )
	     return (1);
	if( KeyboardPtr->key->down[key >> 3] & (1 << (key & 7)) )
	{
	     e.u.u.type = KeyRelease;
	     SetCapsLockLED(0);
	} else
	     SetCapsLockLED(1);
    }

    /*
     *	toggle num lock key:
     *	ignore releases, toggle on & off on presses
     */

    if( KeyboardPtr->key->modifierMap[key] & NumLockMask )
    {
       if( e.u.u.type == KeyRelease )
		return (1);
       if( KeyboardPtr->key->down[key >> 3] & (1 << (key & 7)) )
       {
	    e.u.u.type = KeyRelease;
	    SetNumLockLED(0);
       } else
	    SetNumLockLED(1);
    }

    }

    (*ibmKeybd->processInputProc)(&e,ibmKeybd,1);
    return(1);
}

/***====================================================================***/

#define NONE    0x00
#define LEFT    Button1
#define MIDDLE  Button2 
#define RIGHT   Button3

#define UP      ButtonRelease
#define DOWN    ButtonPress

static int AIX3ButtonPtrEvent();


static void
AIXlocatorTimeout()
{
    TRACE(("AIXlocatorTimeout()\n"));
    AIXPtrEvent(&delayedEvent);
    return;
}

	/*
	 * 11/17/88 (ef) -- XXX!  This function should be revisited.  It
	 *	seems awfully large and complicated.  Two options leap to
	 *	to the fore:  a) just delay for a bit and check for new
	 *	events or go to a finite-state machine.
	 */

static int
AIXPtrEvent(pIn)
hftEvent        *pIn;
{
extern	int	AIXMouseChordDelay;
hftLocEv        *pEv= &pIn->hftEv.hftLoc;
unsigned char   buttons= pEv->leButtons&(HFT_BUTTONS|HFT_MBUTTON);
xEvent          e;


    TRACE(("AIXPtrEvent (%d,%d)\n",
			HF_SHORT(pEv->leDeltaX),HF_SHORT(pEv->leDeltaY)));
    pendingX+= HF_SHORT(pEv->leDeltaX);
    pendingY+= HF_SHORT(pEv->leDeltaY);

    if (pendingX||pendingY)
	AIXFlushMouse();
    e.u.keyButtonPointer.rootX=     AIXCurrentX;
    e.u.keyButtonPointer.rootY=     AIXCurrentY;
    e.u.keyButtonPointer.time=      GET_OS_TIME();

    if (buttons&HFT_MBUTTON) {
    /* We don't care about all the 2 button stuff anymore because
       we know there is a three button mouse.  Therefore, LEFT&RIGHT
       buttons down don't mean middle button( like they had to for a two
       button mouse), they mean left and right button down consecutively.*/
	if (delayed_right || (buttons&HFT_RBUTTON) ) {
	    hftAddTimeout(NULL,0);
	    e.u.u.detail= RIGHT;
	    e.u.u.type=   DOWN;
	    (*ibmPtr->processInputProc)(&e,ibmPtr,1);
	}
	if (delayed_left || (buttons&HFT_LBUTTON) ) {
	    hftAddTimeout(NULL,0);
	    e.u.u.detail= LEFT;
	    e.u.u.type=   DOWN;
	    (*ibmPtr->processInputProc)(&e,ibmPtr,1);
	}
	/* if we had previously generated a middle event from a LEFT&RIGHT
	   and we are waiting for it to go away */
	if (delayed_middle) {
	    hftAddTimeout(NULL,0);
	    e.u.u.detail= MIDDLE;
	    e.u.u.type=   UP;
	    (*ibmPtr->processInputProc)(&e,ibmPtr,1);
	}
	e.u.u.detail= MIDDLE;
	e.u.u.type=   DOWN;
	(*ibmPtr->processInputProc)(&e,ibmPtr,1);
	lastButtons = buttons;
	if (hftInstallHandler(HFT_LOCATOR,AIX3ButtonPtrEvent)==HFT_ERROR)
	    ErrorF("Couldn't install three button mouse handler\n");
	return(1);
    }


    switch(lastButtons)
    {
	case NONE  :
	     switch(buttons)
	     {
		 case NONE  :
		      break;
		 case HFT_LBUTTON  :
		      delayed_left = TRUE;
		      break;
		 case HFT_RBUTTON  :
		      delayed_right = TRUE;
		      break;
		 case HFT_BUTTONS  :
		      e.u.u.detail= MIDDLE;
		      e.u.u.type=   DOWN;
		      (*ibmPtr->processInputProc)(&e,ibmPtr,1);
		      break;
	      }
	     break;
	case HFT_LBUTTON  :
	     switch(buttons)
	     {
		 case NONE  :
		      if (delayed_left) {
			  hftAddTimeout(NULL,0);
			  e.u.u.detail= LEFT;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_left = FALSE;
		      }
		      if (delayed_middle) {
			  e.u.u.detail= MIDDLE;
			  e.u.u.type=   UP;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_middle = FALSE;
		      } else {
			  e.u.u.detail= LEFT;
			  e.u.u.type=   UP;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
		      }
		      break;
		 case HFT_LBUTTON  :
		      if (delayed_left) {
			  hftAddTimeout(NULL,0);
			  e.u.u.detail= LEFT;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_left = FALSE;
		      }
		      break;
		 case HFT_RBUTTON  :
		      if (delayed_left) {
			  hftAddTimeout(NULL,0);
			  e.u.u.detail= LEFT;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_left = FALSE;
		      }
			  e.u.u.detail= LEFT;
			  e.u.u.type=   UP;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
		      delayed_right = TRUE;
		      break;
		 case HFT_BUTTONS  :
		      if (delayed_left) {
			  hftAddTimeout(NULL,0);
			  e.u.u.detail= MIDDLE;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_left = FALSE;
		      } else if (!delayed_middle) {
			  e.u.u.detail= LEFT;
			  e.u.u.type=   UP;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  e.u.u.detail= MIDDLE;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
		      }
		      break;
	      }
	     break;
	case HFT_RBUTTON  :
	     switch(buttons)
	     {
		 case NONE  :
		      if (delayed_right) {
			  hftAddTimeout(NULL,0);
			  e.u.u.detail= RIGHT;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_right = FALSE;
		      }
		      if (delayed_middle) {
			  e.u.u.detail= MIDDLE;
			  e.u.u.type=   UP;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_middle = FALSE;
		      } else {
			  e.u.u.detail= RIGHT;
			  e.u.u.type=   UP;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
		      }
		      break;
		 case HFT_LBUTTON  :
		      if (delayed_right) {
			  hftAddTimeout(NULL,0);
			  e.u.u.detail= RIGHT;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_right = FALSE;
		      }
		      e.u.u.detail= RIGHT;
		      e.u.u.type=   UP;
		      (*ibmPtr->processInputProc)(&e,ibmPtr,1);
		      delayed_left = TRUE;
		      break;
		 case HFT_RBUTTON  :
		      if (delayed_right) {
			  hftAddTimeout(NULL,0);
			  e.u.u.detail= RIGHT;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_right = FALSE;
		      }
		      break;
		 case HFT_BUTTONS  :
		      if (delayed_right) {
			  hftAddTimeout(NULL,0);
			  e.u.u.detail= MIDDLE;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  delayed_right = FALSE;
		      } else if (!delayed_middle) {
			  e.u.u.detail= RIGHT;
			  e.u.u.type=   UP;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
			  e.u.u.detail= MIDDLE;
			  e.u.u.type=   DOWN;
			  (*ibmPtr->processInputProc)(&e,ibmPtr,1);
		      }
		      break;
	      }
	     break;
	case HFT_BUTTONS  :
	     switch(buttons)
	     {
		 case NONE  :
		      e.u.u.detail= MIDDLE;
		      e.u.u.type=   UP;
		      (*ibmPtr->processInputProc)(&e,ibmPtr,1);
		      break;
		 case HFT_LBUTTON  :
		 case HFT_RBUTTON  :
		      delayed_middle = TRUE;
		      break;
		 case HFT_BUTTONS  :
		      break;
	      }
	      break;
    }

    lastButtons = buttons;

    if (delayed_left || delayed_right) {
	delayedEvent = *pIn;
	HF_SETSHORT(delayedEvent.hftEv.hftLoc.leDeltaX,0);
	HF_SETSHORT(delayedEvent.hftEv.hftLoc.leDeltaY,0);
	hftAddTimeout(AIXlocatorTimeout,AIXMouseChordDelay);
    }
    return(1);
}



/***====================================================================***/

static int
AIX3ButtonPtrEvent(pIn)
hftEvent        *pIn;
{
extern  int     AIXMouseChordDelay;
hftLocEv        *pEv= &pIn->hftEv.hftLoc;
unsigned char   buttons= pEv->leButtons&(HFT_BUTTONS|HFT_MBUTTON);
unsigned char   changed;
xEvent          e;

    TRACE(("AIX3ButtonPtrEvent (%d,%d)\n",
			HF_SHORT(pEv->leDeltaX),HF_SHORT(pEv->leDeltaY)));
    pendingX+= HF_SHORT(pEv->leDeltaX);
    pendingY+= HF_SHORT(pEv->leDeltaY);

    if (pendingX||pendingY)
	AIXFlushMouse();
    e.u.keyButtonPointer.rootX=     AIXCurrentX;
    e.u.keyButtonPointer.rootY=     AIXCurrentY;
    e.u.keyButtonPointer.time=      GET_OS_TIME();

    changed = buttons ^ lastButtons;
    if (changed & HFT_LBUTTON) {
	if (buttons & HFT_LBUTTON)      e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	e.u.u.detail= LEFT;
	(*ibmPtr->processInputProc)(&e,ibmPtr,1);
    }
    if (changed & HFT_RBUTTON) {
	if (buttons & HFT_RBUTTON)      e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	e.u.u.detail= RIGHT;
	(*ibmPtr->processInputProc)(&e,ibmPtr,1);
    }
    if (changed & HFT_MBUTTON) {
	if (buttons & HFT_MBUTTON)      e.u.u.type = DOWN;
	    else                        e.u.u.type = UP;
	e.u.u.detail= MIDDLE;
	(*ibmPtr->processInputProc)(&e,ibmPtr,1);
    }
    lastButtons = buttons;
    return(1);
}

/***====================================================================***/

	/*
	 * 11/17/88 (ef) -- XXX! Not sure why this has to be much different
	 *	than AIXFlushMouse.   have to examine.
	 */

#ifdef AIXEXTENSIONS

#ifdef AIXSTAT
#include "AIXstat.h"
extern int   aixsetcursorcount ;
extern int   statenable ;
extern int   statcodemask ;
#endif

static int aixTabletSaveX = 1 ;
static int aixTabletSaveY = 1;
int aixTabletScaleX ;
int aixTabletScaleY ;

static void
AIXFlushTablet(deltax,deltay)
int deltax,deltay;
{
register	int	x,y;
register	ibmPerScreenInfo	*screenInfo;
		int	setCursor;
		xEvent	e;

    TRACE(("AIXFlushTablet()\n"));

    screenInfo = ibmScreens[ ibmCurrentScreen ];
    x = AIXCurrentX ;
    y = screenInfo->ibm_ScreenBounds.y2 - AIXCurrentY ;
    setCursor = TRUE ;

    /*
     * Analize horizontal movement for releative coordinates
     */

    if ( deltax <= aixTabletSaveX )
        x =  deltax * x / aixTabletSaveX ;
    else
     if ((screenInfo->ibm_ScreenBounds.x2 * aixTabletScaleX) != aixTabletSaveX)
	x += (deltax - aixTabletSaveX) * (screenInfo->ibm_ScreenBounds.x2 - x) /
	 (screenInfo->ibm_ScreenBounds.x2 * aixTabletScaleX - aixTabletSaveX);

    if ( !deltax )	
    	aixTabletSaveX = 1 ;
    else
    	aixTabletSaveX = deltax;

    /*
     * Analize vertical movement for releative coordinates
     */

    if ( deltay <= aixTabletSaveY )
       y = deltay * y / aixTabletSaveY ;
    else
     if ((screenInfo->ibm_ScreenBounds.y2 * aixTabletScaleY) != aixTabletSaveY)
       y += (deltay - aixTabletSaveY) * (screenInfo->ibm_ScreenBounds.y2 - y) /
	  (screenInfo->ibm_ScreenBounds.y2 * aixTabletScaleY - aixTabletSaveY);

    if ( !deltay)
    	aixTabletSaveY = 1 ;
    else
    	aixTabletSaveY = deltay ;

         y = screenInfo->ibm_ScreenBounds.y2 - y ;

	if ( y > screenInfo->ibm_ScreenBounds.y2)
	     y = screenInfo->ibm_ScreenBounds.y2;

	if ( x > screenInfo->ibm_ScreenBounds.x2)
	     x = screenInfo->ibm_ScreenBounds.x2;

	if ( y <= 0) 
	    y = 0;

	if ( x <= 0) 
	    x = 0;

	if (setCursor) {
	    /* OS-DEPENDENT MACRO GOES HERE!!
	     * MACRO DEFINED IN FILE ibmos.h
	     * TELL OS THAT CURSOR HAS MOVED 
	     * TO A NEW POSITION
	     */
	    OS_TellCursorPosition(x,y);
	}

#ifdef CROSSHAIR
	CrossHairSetCursorPosition(x,y);
#endif

     (* screenInfo->ibm_CursorShow )( x, y ) ;

     AIXCurrentX= x ;
     AIXCurrentY= y ;
     pendingX= pendingY= 0;
     return ;
}

/***====================================================================***/

static int
AIXTabletEvent(pIn)
hftEvent        *pIn;
{
hftLocEv        *pEv= &pIn->hftEv.hftLoc;
unsigned char   tmpbuttons,buttons ;
xEvent          e;

    TRACE(("AIXTabletEvent(0x%x)\n",pIn));
#ifdef AIXSTAT
	if (statenable && (statcodemask & aixsetcursorpositionCode))
   	   aixsetcursorcount++ ;	
#endif

	/* bug compatible */

	if (pEv->leButtons == 3 )
		return ;

	buttons = pEv->leButtons >> 3 ;

	switch (buttons)
        {
	case  1 :
		buttons = 0x80 ;
		break ;
	case  2 :
		buttons = 0x40 ;
		break ;
	case  3 :
		buttons = 0x20 ;
		break ;
	case  4 :
		buttons = 0x10 ;
		break ;
	}

	if (lastButtons != (buttons & HFT_TABLET_BUTTONS)) {
	    if (lastButtons) {
		e.u.u.type = ButtonRelease;
		buttons = 0x00 ;
		tmpbuttons = lastButtons;
	    } else {
		e.u.u.type = ButtonPress;
		tmpbuttons = buttons;
	    }

	    lastButtons = buttons & HFT_TABLET_BUTTONS ;

	    switch (tmpbuttons) {
		case  HFT_BUTTON1 : e.u.u.detail = Button1;  break;
		case  HFT_BUTTON2 : e.u.u.detail = Button2;  break;
		case  HFT_BUTTON3 : e.u.u.detail = Button3;  break;
		case  HFT_BUTTON4 : e.u.u.detail = Button4;  break;
	    }
        }
	else
	{
	    e.u.u.detail= 0 ;
	    e.u.u.type=  MotionNotify;
	}

	AIXFlushTablet(HF_SHORT(pEv->leDeltaX),HF_SHORT(pEv->leDeltaY));

	e.u.keyButtonPointer.rootX=     AIXCurrentX;
	e.u.keyButtonPointer.rootY=     AIXCurrentY;
	e.u.keyButtonPointer.time=      GET_OS_TIME();

        (*(ibmPtr->processInputProc))(&e,ibmPtr,1);

        return (1) ;
}

#endif AIXEXTENSIONS


/***====================================================================***/

	/*
	 * 11/27/88 (ef) -- XXX  -- Hack but not *too* gross, wouldn't be
	 *	bad if we could install it only when we knew it was
	 *	necessary -- in aixScreenSave or something.
	 *
	 * Called before any events are processed, IFF events are present on 
	 * the hft queue.  Turns off screen saver, if it is active and dumps
	 * malloc arena if requested.
	 */
static void
AIXPreEventHandler()
{
#ifdef IBM_SPECIAL_MALLOC
extern int ibmShouldDumpArena;
#endif /* IBM_SPECIAL_MALLOC */

    TRACE(("aixPreEventHandler()\n"));
#ifdef IBM_SPECIAL_MALLOC
    if (ibmShouldDumpArena) {
	ibmDumpArena();
    }
#endif /* IBM_SPECIAL_MALLOC */
    if (screenIsSaved == SCREEN_SAVER_ON) {
	SaveScreens( SCREEN_SAVER_OFF, ScreenSaverReset );
    }
    return;
}

/***====================================================================***/

void
ProcessInputEvents()
{

    TRACE(("ProcessInputEvents (pending=%d)\n",hftPending));
    hftDispatchEvents();
    if (pendingX||pendingY)
	AIXFlushMouse();
    return;
}

/***====================================================================***/

#ifdef AIXEXTENSIONS
static int
AIXDialEvent (pIn)
hftEvent	*pIn;
{
    xExtEvent	xe;
    hftDialEv	*pEv = &pIn->hftEv.hftDial;

    TRACE(("AIXDialEvent(0x%x)\n",pIn));
    xe.u.DialLpfk.rootX =  AIXCurrentX;
    xe.u.DialLpfk.rootY =  AIXCurrentY;
    lastEventTime = xe.u.DialLpfk.time = GET_OS_TIME();

    xe.u.u.type = DialRotate;
    xe.u.u.detail = (char) pEv->deDialNo;
    xe.u.DialLpfk.value = (char) pEv->deDelta;

    if (rtDial->on)
	(*(rtDial->processInputProc))((xEvent *)&xe, rtDial,1);


    return (1);
}


static int
AIXLpfkEvent (pIn)
hftEvent	*pIn;
{
    xExtEvent	xe;
    hftLPFKEv	*pEv = &pIn->hftEv.hftLpfk;

    TRACE(("AIXLpfkEvent(0x%x)\n",pIn));
    xe.u.DialLpfk.rootX =  AIXCurrentX;
    xe.u.DialLpfk.rootY =  AIXCurrentY;
    lastEventTime = xe.u.DialLpfk.time = GET_OS_TIME();

    xe.u.u.type = LPFKeyPress ;
    xe.u.u.detail = pEv->lpfkeKeyNo ;


    if (rtLpfk->on)
	(*(rtLpfk->processInputProc))((xEvent *)&xe, rtLpfk,1);


    return (1);
}
#endif

/***====================================================================***/

extern void     ibmReactivateScreens(), ibmDeactivateScreens();

void
AIXInitEventHandlers()
{
    TRACE(("AIXInitEventHandlers()\n"));
    if (hftInstallHandler(HFT_BEFORE_EVENTS,AIXPreEventHandler)==HFT_ERROR) {
	ErrorF("Couldn't install pre-event handler\n");
    }
    if (hftInstallHandler(HFT_LOCATOR,AIXPtrEvent)==HFT_ERROR) {
	ErrorF("Couldn't install mouse handler\n");
    }
    if (hftInstallHandler(HFT_KEYBOARD,AIXKbdEvent)==HFT_ERROR) {
	ErrorF("Couldn't install keyboard handler\n");
    }
    hftInstallHandler(HFT_EVENT_ARRIVED,HFT_IGNORE);
    if (hftInstallHandler(HFT_GRANTED,ibmReactivateScreens)==HFT_ERROR) {
	ErrorF("Couldn't install grant routine\n");
    }
    if (hftInstallHandler(HFT_RETRACTED,ibmDeactivateScreens)==HFT_ERROR) {
        ErrorF("Couldn't install retract handler\n");
    }
#ifdef AIXEXTENSIONS
    if (hftInstallHandler(HFT_TABLET,AIXTabletEvent)==HFT_ERROR) {
	ErrorF("Couldn't install tablet handler\n");
    }
    if (hftInstallHandler(HFT_DIAL,AIXDialEvent)==HFT_ERROR) {
        ErrorF("Couldn't install dial handler\n");
    }
    if (hftInstallHandler(HFT_LPFK,AIXLpfkEvent)==HFT_ERROR) {
        ErrorF("Couldn't install lpfk handler\n");
    }
#endif
}
