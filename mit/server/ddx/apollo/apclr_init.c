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

/*
 * Functions implementing Apollo color display-specific parts of the driver
 * having to do with driver initialization.
 */

#include "apollo.h"
#include "mi.h"
#include "mipointer.h"
#include "mfb.h"

/*
 * Functions for mi software cursor function vector.
 */
extern long apEventTime();
extern Bool apCursorOffScreen();
extern void apCrossScreen();
extern void apQueueEvent();

/*
 * Vector of mi pointer funcs.
 */
static miPointerCursorFuncRec apClrCursorFuncs = {
    apEventTime,
    apCursorOffScreen,
    apCrossScreen,
    apQueueEvent
};


/*
 * Variables controlling allocation of color screen privates.
 */
int                     apClrScreenIndex;
static unsigned long    apClrGeneration = 0;

/*
 * extern for GPR driver initialization
 */
extern Bool     apcScreenInit();

/*
 * extern for pseudocolor colormap initialization
 */
extern void     apClrInitColormap();


/*
 * apClrScreenClose -- DDX (screen)
 *      Perform color screen-specific closedown.  Free all resources,
 *      and clear the screen (server reset is more obvious if the screen is black).
 */
static Bool
apClrScreenClose(index, pScreen)
    int         index;
    ScreenPtr   pScreen;
{
    status_$t           status;
    Bool                retval;
    apDisplayDataPtr    pDisp;
    apClrPrivScreenPtr  pPrivScreen;
    DepthPtr            pDep;
    int                 i;

    pPrivScreen = (apClrPrivScreenPtr) pScreen->devPrivates[apClrScreenIndex].ptr;
    pScreen->CloseScreen = pPrivScreen->CloseScreen;
    retval = (*pScreen->CloseScreen) (index, pScreen);

    xfree (pPrivScreen);

    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;
    apSetBitmap (pDisp->display_bitmap);
    gpr_$set_clipping_active (false, &status);
    gpr_$set_plane_mask_32 ((gpr_$mask_32_t) 0xFFFFFFFF, &status);
    gpr_$clear ((gpr_$pixel_value_t) 0, &status);
    gpr_$enable_direct_access (&status);

    return retval;
}

/*
 * apClrScreenInit -- DDX (screen)
 *      Perform color screen-specific initialization.
 *      This basically involves device initialization, and filling in
 *      the apDisplayData record and the Screen record.
 */
Bool
apClrScreenInit(index, pScreen, argc, argv)
    int         index;
    ScreenPtr   pScreen;
    int         argc;   /* these two may NOT be changed */
    char      **argv;
{
    apDisplayDataPtr        pDisp;
    apClrPrivScreenPtr      pPrivScreen;
    Bool                    retval;
    status_$t               status;

    if (apClrGeneration != serverGeneration)
    {
        if ((apClrScreenIndex = AllocateScreenPrivateIndex()) < 0)
            return FALSE;
        apClrGeneration = serverGeneration;
    }

    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;

    pPrivScreen = (apClrPrivScreenPtr) xalloc(sizeof(apClrPrivScreenRec));
    if (!pPrivScreen)
        return FALSE;
    
    pScreen->devPrivates[apClrScreenIndex].ptr = (pointer) pPrivScreen;

    retval = apcScreenInit(pScreen, pDisp, pPrivScreen);
    
    if (retval)
    {
        pPrivScreen->CloseScreen = pScreen->CloseScreen;
        pScreen->CloseScreen = apClrScreenClose;
        
        pScreen->SaveScreen = apSaveScreen;
        pScreen->blackPixel = 0;
        pScreen->whitePixel = 1;
        apClrInitColormap (pScreen, pDisp, pPrivScreen);
        
        retval = apcDCInitialize (pScreen, &apClrCursorFuncs);
        apMICursorScreen = pScreen;
        
        if (retval)
            retval = mfbCreateDefColormap (pScreen);
        
        gpr_$enable_direct_access (&status);
    }

    if (!retval)
        xfree(pPrivScreen);

    return retval;
}
