/******************************************************************
Copyright 1987 by Apollo Computer Inc., Chelmsford, Massachusetts.
Copyright 1989 by Hewlett-Packard Company.

                        All Rights Reserved

Permission to use, duplicate, change, and distribute this software and
its documentation for any purpose and without fee is granted, provided
that the above copyright notice appear in such copy and that this
copyright notice appear in all supporting documentation, and that the
names of Apollo Computer Inc., the Hewlett-Packard Company, or MIT not
be used in advertising or publicity pertaining to distribution of the
software without written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.
******************************************************************/

/* $XConsortium: ap_input.c,v 1.2 90/02/22 10:36:19 rws Exp $ */

/*
 * Functions implementing Apollo-device-independent parts of the driver
 * having to do with input handling.
 */

#include "apollo.h"
#include <sys/param.h>
#include "osdep.h"

#include <apollo/tone.h>

#ifndef NO_PHYS_KEYS
extern void smd_$inq_kbd_type (
    short      &buf_len,
    char       *kbd_string,
    short      *string_len,
    status_$t  *status
);
#endif

/*
 * Pointers to the DeviceRec's for the keyboard and pointer.
 */
DevicePtr       apKeyboard;
DevicePtr       apPointer;

/*
 * Time of last input event.
 */
static int      lastEventTime;
static Bool     lastTimeSet = FALSE;

/*
 * File descriptor for both keyboard and pointer to give to OS code.
 */
static int      fdApollo = 0;

/*
 * Flag to set GPR position if cursor has just appeared on a new screen
 * or warped.
 */
static Bool             mustSetGPRPos;
static gpr_$position_t  gpr_pointer_pos;

/*
 * Values whose addresses we pass to SetInputCheck when we want
 * ProcessInputEvents to be unconditionally called.
 */
static long     alwaysCheckForInput[2] = {0, 1};

/*
 * Variables from the GPR type manager:
 * pointers to the current input eventcount and the last eventcount value noted.
 * Passed to SetInputCheck when we want DIX to skip calling ProcessInputEvents
 * unless real input exists.
 */
extern long            *apECV;
extern long            *apLastECV;

/*
 * Variables from the GPR type manager:
 * the most recent fundamental input event data
 */
extern gpr_$event_t     apEventType;
extern unsigned char    apEventData[1];
extern gpr_$position_t  apEventPosition;
#ifndef NO_PHYS_KEYS
extern time_$timeval_t  apEventTimestamp;
#endif

/*
 * List of externs for the GPR type manager routines.
 */
extern int      MakeGPRStream ();
extern Bool     GetGPREvent ();

/*
 * Write fd mask for WaitForSomething to select on smd events.
 */
long            apInputMask[mskcnt];

/*
 * List of externs for the different versions of the keyboard routines.
 */
#ifdef NO_PHYS_KEYS
extern void     apK2HWInitKbd();
extern void     apK2HWCloseKbd();
extern Bool     apK2GetMappings();
extern void     apK2HandleKey();
extern void     apK2LegalModifier();
extern void     apK2SetRepeat();

#else

extern void     apNAHWInitKbd();
extern void     apNAHWCloseKbd();
extern Bool     apNAGetMappings();
extern void     apNAHandleKey();
extern void     apNALegalModifier();
extern void     apNASetRepeat();

extern void     apMNHWInitKbd();
extern void     apMNHWCloseKbd();
extern Bool     apMNGetMappings();
extern void     apMNHandleKey();
extern void     apMNLegalModifier();
extern void     apMNSetRepeat();
#endif

/*
 * LegalModifier -- DDX interface (server)
 *      Predicate to say whether a given key code is a legal modifier
 *      (e.g. shift, control, ...).
 */
Bool
LegalModifier(key)
    BYTE    key;
{
    apPrivKbdPtr    pPrivK;

    pPrivK = (apPrivKbdPtr) apKeyboard->devicePrivate;
    return ( (*(pPrivK->LegalModifierPtr))(key) );
}

/*
 * apBell -- DDX interface (device)
 *      Ring the bell on the given device.
 */
static void
apBell(loud, pDevice)
    int         loud;
    DevicePtr   pDevice;
{
/*
 *  No volume control
 */
    if (loud > 0)
        tone_$time(((apPrivKbdPtr)(pDevice->devicePrivate))->beepTime);
}

/*
 * apChangeKeyboardControl -- DDX interface (device)
 *      Attempt to set various possible keyboard attributes, most of which
 *      don't exist on Apollos.
 */
static void
apChangeKeyboardControl(pDevice, ctrl)
    DevicePtr   pDevice;
    KeybdCtrl  *ctrl;
{
    apPrivKbdPtr    pPrivK;
    status_$t       status;

    pPrivK = (apPrivKbdPtr) pDevice->devicePrivate;
/*
 *  No keyclick, bell pitch control or LEDs
 */
    pPrivK->beepTime.high16 = 0;
    pPrivK->beepTime.low32 = 250 * ctrl->bell_duration;
/*
 *  Tell keyboard code about autorepeat
 */
    (*(pPrivK->SetRepeatPtr)) (ctrl);
}

/*
 * apGetMotionEvents -- DDX interface (device)
 *      Return the contents of the mythical motion event buffer.
 */
static int
apGetMotionEvents(buff, start, stop)
    CARD32      start, stop;
    xTimecoord *buff;
{
/*
 *  No motion buffer
 */
    return 0;
}

/*
 * apChangePointerControl -- DDX interface (device)
 *      Attempt to set various possible pointer attributes.
 *      We don't.
 */
static void
apChangePointerControl(pDevice, ctrl)
    DevicePtr   pDevice;
    PtrCtrl    *ctrl;
{
/*
 *  Not implementing mouse threshhold or acceleration factor
 */
}

/*
 * HWInitPointr -- Driver internal code
 *      Perform all "hardware" operations needed to initialize pointer.
 */
static void
HWInitPointr()
{
    gpr_$keyset_t   keyset;
    status_$t       status;

    lib_$init_set (keyset, 256);
    lib_$add_to_set (keyset, 256, 'A');
    lib_$add_to_set (keyset, 256, 'B');
    lib_$add_to_set (keyset, 256, 'C');
    lib_$add_to_set (keyset, 256, 'D');
    lib_$add_to_set (keyset, 256, 'E');
    lib_$add_to_set (keyset, 256, 'a');
    lib_$add_to_set (keyset, 256, 'b');
    lib_$add_to_set (keyset, 256, 'c');
    lib_$add_to_set (keyset, 256, 'd');
    lib_$add_to_set (keyset, 256, 'e');
    gpr_$enable_input (gpr_$buttons, keyset, &status);
    gpr_$enable_input (gpr_$locator_stop, keyset, &status);
    gpr_$enable_input (gpr_$locator_update, keyset, &status);
}

/*
 * apMouseProc -- DDX interface (device)
 *      Code to initialize, terminate, turn on and turn off the mouse device.
 */
int
apMouseProc(pDev, onoff, argc, argv)
    DevicePtr   pDev;
    int         onoff;
    int         argc;
    char       *argv[];
{
    BYTE            map[6];
    apPrivPointrPtr pPrivP;
    status_$t       status;
    Bool            retval;

    switch (onoff)
    {
        case DEVICE_INIT:
            apPointer = pDev;
            pPrivP = (apPrivPointrPtr) xalloc (sizeof(apPrivPointrRec));
            if (!pPrivP) return -1; /* Failure */
            apPointer->devicePrivate = (pointer) pPrivP;

            pPrivP->numCurScreen = -1;
            pPrivP->x = pPrivP->y = 0;

            HWInitPointr();
            pDev->on = FALSE;
            map[1] = 1;
            map[2] = 2;
            map[3] = 3;
            map[4] = 4;
            map[5] = 5;

            retval = InitPointerDeviceStruct (apPointer, map, 5,
                         apGetMotionEvents, apChangePointerControl, 0);
            if (!retval)
            {
                xfree (pDev->devicePrivate);
                return -1; /* Failure */
            }

            SetInputCheck(apECV, apLastECV);

            if (!fdApollo)
            {
                fdApollo = MakeGPRStream();
                CLEARBITS(apInputMask);
                BITSET(apInputMask, fdApollo);
            }
            break;
        case DEVICE_ON:
            pDev->on = TRUE;
            AddEnabledDevice(fdApollo);
            break;
        case DEVICE_OFF:
            pDev->on = FALSE;
            break;
        case DEVICE_CLOSE:
            xfree (pDev->devicePrivate);
            gpr_$disable_input (gpr_$buttons, &status);
            gpr_$disable_input (gpr_$locator_stop, &status);
            gpr_$disable_input (gpr_$locator_update, &status);
            break;
    }
    return Success;

}

/*
 * apKeybdProc -- DDX interface (device)
 *      Code to initialize, terminate, turn on and turn off the keyboard device.
 */
int
apKeybdProc(pDev, onoff, argc, argv)
    DevicePtr   pDev;
    int         onoff;
    int         argc;
    char       *argv[];
{
    KeySymsRec      keySyms;
    CARD8           modMap[MAP_LENGTH];
    status_$t       status;
    apPrivKbdPtr    pPrivK;
    Bool            retval;
#ifndef NO_PHYS_KEYS
#define SHORT_STRLEN 4
    char            kbdtypestr[SHORT_STRLEN];
    short           kbdtypestrlen;

    static char     kbdtype, kbdsubtype;
#endif

    switch (onoff)
    {
        case DEVICE_INIT:
            apKeyboard = pDev;
            apKeyboard->devicePrivate = (pointer) xalloc (sizeof(apPrivKbdRec));
            if (!(apKeyboard->devicePrivate)) return -1; /* Failure */
            pPrivK = (apPrivKbdPtr) apKeyboard->devicePrivate;

            pPrivK->beepTime.high16 = 0;
            pPrivK->beepTime.low32 = 250*250;    /* 1/4 second */

#ifdef NO_PHYS_KEYS
            pPrivK->LegalModifierPtr = apK2LegalModifier;
            pPrivK->HandleKeyPtr = apK2HandleKey;
            pPrivK->SetRepeatPtr = apK2SetRepeat;
            apK2HWInitKbd ();
            if (!apK2GetMappings (&keySyms, modMap))
            {
                xfree (pDev->devicePrivate);
                return -1; /* Failure */
            }
#else
            /*
             * Detect keyboard type and do initialization accordingly.
             */

            smd_$inq_kbd_type(SHORT_STRLEN, kbdtypestr, &kbdtypestrlen, &status);
            kbdtype = (kbdtypestrlen > 0) ? kbdtypestr[0] : '2';
            kbdsubtype = (kbdtypestrlen > 1) ? kbdtypestr[1] : ' ';

            if ((kbdtype == '3') && (kbdsubtype != ' '))
            {   /* multinational */
                pPrivK->LegalModifierPtr = apMNLegalModifier;
                pPrivK->HandleKeyPtr = apMNHandleKey;
                pPrivK->SetRepeatPtr = apMNSetRepeat;
                apMNHWInitKbd (kbdsubtype);
                if (!apMNGetMappings (&keySyms, modMap))
                {
                    xfree (pDev->devicePrivate);
                    return -1; /* Failure */
                }
            }
            else
            {   /* assume north american */
                pPrivK->LegalModifierPtr = apNALegalModifier;
                pPrivK->HandleKeyPtr = apNAHandleKey;
                pPrivK->SetRepeatPtr = apNASetRepeat;
                apNAHWInitKbd (kbdtype);
                if (!apNAGetMappings (&keySyms, modMap))
                {
                    xfree (pDev->devicePrivate);
                    return -1; /* Failure */
                }
            }
#endif

            pDev->on = FALSE;

	    /*
	     * Since most MIT servers do not provide the capability of
	     * individual control over autorepeat on each key, and provide
	     * only control of the autorepeat state of the keyboard as a
	     * whole, they ignore the default autorepeat key set (whose value
	     * as distributed from MIT is the null set).  This default value
	     * is probably not what you want.  This should be an argument to
	     * InitKeyboardDeviceStruct (and saved in the device structure)
	     */

	    {
		static unsigned char apolloAutoRepeats[32] = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0xA8, 0x02, 0x00, 0xF8, 0xFF, 0x1D, 0xC0,
			0xFF, 0x27, 0xFE, 0x5F, 0xFE, 0x1F, 0xFF, 0x7F };
    	    	extern KeybdCtrl	defaultKeyboardControl;
    	    
    	    	bcopy (apolloAutoRepeats, defaultKeyboardControl.autoRepeats,
	       	       sizeof (apolloAutoRepeats));
	    }

            retval = InitKeyboardDeviceStruct (apKeyboard, &keySyms, modMap,
                         apBell, apChangeKeyboardControl);
            xfree(keySyms.map);

            if (!retval)
            {
                xfree (pDev->devicePrivate);
                return -1; /* Failure */
            }

            SetInputCheck(apECV, apLastECV);

            if (!fdApollo)
            {
                fdApollo = MakeGPRStream();
                CLEARBITS(apInputMask);
                BITSET(apInputMask, fdApollo);
            }
            break;
        case DEVICE_ON:
            pDev->on = TRUE;
            AddEnabledDevice(fdApollo);
            break;
        case DEVICE_OFF:
            pDev->on = FALSE;
            break;
        case DEVICE_CLOSE:
#ifdef NO_PHYS_KEYS
            apK2HWCloseKbd ();
#else
            if ((kbdtype == '3') && (kbdsubtype != ' '))
            {   /* multinational */
                apMNHWCloseKbd ();
            }
            else
            {   /* assume north american */
                apNAHWCloseKbd ();
            }
#endif
            xfree (pDev->devicePrivate);
            gpr_$disable_input(gpr_$keystroke, &status);
            break;
    }
    return Success;
}

/*
 * handleButton -- Driver internal code
 *      Translate the Apollo event data into an X event, and make it happen.
 */
static void
handleButton (xEp)
    xEvent *xEp;
{
    if (apEventData[0] < 'a')
    {
        xEp->u.u.type = ButtonRelease;
        xEp->u.u.detail = apEventData[0] - ('A' - 1);
    }
    else
    {
        xEp->u.u.type = ButtonPress;
        xEp->u.u.detail = apEventData[0] - ('a' - 1);
    }
    (*apPointer->processInputProc) (xEp, apPointer, 1);
}

/*
 * We cheat by looking into this DIX (window.c) variable to see if
 * we should unsave the screen.
 */
extern int screenIsSaved;

/*
 * ProcessInputEvents -- DDX interface (server)
 *      While wasting as little time as possible if no events exist,
 *      get and process all pending input from the Apollo devices.
 */
void
ProcessInputEvents()
{
    xEvent          xE;
    apPrivKbdPtr    pPrivK;
    apPrivPointrPtr pPrivP;
    status_$t       status;
    int             cursorx, cursory;
    Bool            cursorMoved;

    if (mustSetGPRPos)
    {
        gpr_$set_cursor_position(gpr_pointer_pos, &status);
        mustSetGPRPos = FALSE;
        SetInputCheck(apECV, apLastECV);
    }

    cursorMoved = FALSE;
    while (GetGPREvent(TRUE, TRUE))
    {
        if (screenIsSaved == SCREEN_SAVER_ON)
            SaveScreens (SCREEN_SAVER_OFF, ScreenSaverReset);

#ifdef NO_PHYS_KEYS
        /*
         * The best we can do prior to SR10.2 is to timestamp events when dequeued.
         */
        lastEventTime = GetTimeInMillis();
#else
        lastEventTime = (apEventTimestamp.sec * 1000) + (apEventTimestamp.usec / 1000);
#endif
        lastTimeSet = TRUE;

        pPrivP = (apPrivPointrPtr) apPointer->devicePrivate;
        pPrivP->x = apEventPosition.x_coord;
        pPrivP->y = apEventPosition.y_coord;

        switch (apEventType)
        {
            case gpr_$locator_stop:
            case gpr_$locator_update:
                cursorMoved = TRUE;
                cursorx = pPrivP->x;
                cursory = pPrivP->y;
                break;
            case gpr_$buttons:
                if (cursorMoved)
                {
                    miPointerMoveCursor (screenInfo.screens[pPrivP->numCurScreen],
                                         cursorx, cursory, TRUE);
                    cursorMoved = FALSE;
                }
                xE.u.keyButtonPointer.time = lastEventTime;
                xE.u.keyButtonPointer.rootX = apEventPosition.x_coord;
                xE.u.keyButtonPointer.rootY = apEventPosition.y_coord;
                handleButton (&xE);
                break;
#ifdef NO_PHYS_KEYS
            case gpr_$keystroke:
#else
            case gpr_$physical_keys:
#endif
                if (cursorMoved)
                {
                    miPointerMoveCursor (screenInfo.screens[pPrivP->numCurScreen],
                                         cursorx, cursory, TRUE);
                    cursorMoved = FALSE;
                }
                xE.u.keyButtonPointer.time = lastEventTime;
                xE.u.keyButtonPointer.rootX = apEventPosition.x_coord;
                xE.u.keyButtonPointer.rootY = apEventPosition.y_coord;
                pPrivK = (apPrivKbdPtr) apKeyboard->devicePrivate;
                (*(pPrivK->HandleKeyPtr)) (&xE, apEventData[0]);
                break;
            default:
                break;
        }
    }
    if (cursorMoved)
        miPointerMoveCursor (screenInfo.screens[pPrivP->numCurScreen],
                             cursorx, cursory, TRUE);
}

/*
 * apSaveScreen -- DDX interface (screen)
 *      Control screen saver.
 */
extern boolean gpr_$video_ctl(
    boolean   &on_off,
    status_$t  *status
);
Bool
apSaveScreen(pScreen, on)
    ScreenPtr   pScreen;
    int         on;
{
    status_$t status;    
    boolean   video_switch;
    
    switch (on)
    {
    case SCREEN_SAVER_ON:
        video_switch = false;
        break;
    case SCREEN_SAVER_OFF:
        video_switch = true;
        break;
    case SCREEN_SAVER_FORCER:
        lastEventTime = GetTimeInMillis();
        return TRUE;
    default:                   
        return FALSE;
    }
    gpr_$video_ctl (video_switch, &status);
    return (status.all == status_$ok);
}

/*
 * TimeSinceLastInputEvent -- DDX interface (server)
 *      Let the OS code know how long since last input, so the server can
 *      wake up when the screen saver is supposed to go off.
 */
int
TimeSinceLastInputEvent()
{
    int timeNow;

    if (!lastTimeSet)
    {
        lastEventTime = GetTimeInMillis();
        lastTimeSet = TRUE;
    }
    timeNow = GetTimeInMillis();
    return (timeNow - lastEventTime);
}

/*
 * apEventTime -- mi cursor support
 *      Return lastEventTime.
 */
long
apEventTime(pScreen)
    ScreenPtr   pScreen;
{
    if (!lastTimeSet)
    {
        lastEventTime = GetTimeInMillis();
        lastTimeSet = TRUE;
    }
    return lastEventTime;
}

/*
 * apCursorOffScreen -- mi cursor support
 *      Make the cursor wrap horizontally around the screens.
 */
Bool
apCursorOffScreen(ppScreen, px, py)
    ScreenPtr  *ppScreen;
    int        *px;
    int        *py;
{
    int         index;
    ScreenPtr   pScreenOld;

    if (screenInfo.numScreens > 1 && (*px >= (*ppScreen)->width || *px < 0))
    {
        index = (*ppScreen)->myNum;
        pScreenOld = *ppScreen;
        if (*px < 0)
        {
            index = (index ? index : screenInfo.numScreens) - 1;
            *ppScreen = screenInfo.screens[index];
            *px += (*ppScreen)->width;
        }
        else
        {
            *px -= (*ppScreen)->width;
            index = (index + 1) % screenInfo.numScreens;
            *ppScreen = screenInfo.screens[index];
        }
        *py *= (*ppScreen)->height;
        *py /= pScreenOld->height;
        return TRUE;
    }
    return FALSE;
}

/*
 * apCrossScreen -- mi cursor support
 *      Do any necessary handling when the cursor crosses screens.
 *      We note the new screen number in the pointer device privates.
 */
void
apCrossScreen(pScreen, entering)
    ScreenPtr   pScreen;
    Bool        entering;
{
    short   x, y;

    if (entering) {
        ((apPrivPointrPtr) apPointer->devicePrivate)->numCurScreen = pScreen->myNum;
        miPointerPosition(pScreen, &x, &y);
        gpr_pointer_pos.x_coord = x;
        gpr_pointer_pos.y_coord = y;
        mustSetGPRPos = TRUE;
        SetInputCheck(&alwaysCheckForInput[0], &alwaysCheckForInput[1]);
    }
}

/*
 * apQueueEvent -- mi cursor support
 *      Do any necessary handling when the cursor is warped.
 *      We remember to tell GPR the new position.
 */
void
apQueueEvent(pxE, pPointer, pScreen)
    xEvent     *pxE;
    DevicePtr   pPointer;
    ScreenPtr   pScreen;
{
    gpr_pointer_pos.x_coord = pxE->u.keyButtonPointer.rootX;
    gpr_pointer_pos.y_coord = pxE->u.keyButtonPointer.rootY;
    mustSetGPRPos = TRUE;
    SetInputCheck(&alwaysCheckForInput[0], &alwaysCheckForInput[1]);
    (*pPointer->processInputProc) (pxE, pPointer, 1);
}
