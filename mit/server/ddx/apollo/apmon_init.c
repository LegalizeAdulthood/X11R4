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
 * Functions implementing Apollo monochrome display-specific parts of the driver
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

static miPointerCursorFuncRec apMonoCursorFuncs = {
    apEventTime,
    apCursorOffScreen,
    apCrossScreen,
    apQueueEvent
};

/*
 * Variables controlling allocation of monochrome screen privates.
 */
int                     apMonoScreenIndex;
static unsigned long    apMonoGeneration = 0;


/*
 * apMonoScreenClose -- DDX (screen)
 *      Perform monochrome screen-specific closedown.  Free all resources,
 *      and clear the screen (server reset is more obvious if the screen is black).
 */
static Bool
apMonoScreenClose(index, pScreen)
    int         index;
    ScreenPtr   pScreen;
{
    status_$t           status;
    Bool                retval;
    apMonoPrivScreenPtr pPrivScreen;

    pPrivScreen = (apMonoPrivScreenPtr) pScreen->devPrivates[apMonoScreenIndex].ptr;
    pScreen->CloseScreen = pPrivScreen->CloseScreen;
    retval = (*pScreen->CloseScreen) (index, pScreen);

    xfree (pPrivScreen);

    gpr_$clear ((gpr_$pixel_value_t) 0, &status);
    gpr_$enable_direct_access (&status);

    return retval;
}

/*
 * apMonoScreenInit -- DDX (screen)
 *      Perform monochrome screen-specific initialization.
 *      This basically involves device initialization, and filling in
 *      the apDisplayData record and the Screen record.
 */
Bool
apMonoScreenInit(index, pScreen, argc, argv)
    int         index;
    ScreenPtr   pScreen;
    int         argc;   /* these two may NOT be changed */
    char      **argv;
{
    apDisplayDataPtr    pDisp;
    apMonoPrivScreenPtr pPrivScreen;
    Bool                retval;
    int                 dpix, dpiy;
    status_$t           status;

    if (apMonoGeneration != serverGeneration)
    {
        if ((apMonoScreenIndex = AllocateScreenPrivateIndex()) < 0)
            return FALSE;
        apMonoGeneration = serverGeneration;
    }

    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;

    pPrivScreen = (apMonoPrivScreenPtr) xalloc(sizeof(apMonoPrivScreenRec));
    if (!pPrivScreen)
        return FALSE;

    /*
     * It sure is moronic to have to convert metric to English units
     * so that mfbScreenInit can undo it!
     */
    
    dpix = (pDisp->display_char.x_pixels_per_cm * 254) / 100;
    dpiy = (pDisp->display_char.y_pixels_per_cm * 254) / 100;
    retval = mfbScreenInit (pScreen, pDisp->bitmap_ptr,
                            pDisp->display_char.x_visible_size,
                            pDisp->display_char.y_visible_size,
                            dpix, dpiy,
                            (16 * pDisp->words_per_line));
    
    if (retval)
    {
        pScreen->devPrivates[apMonoScreenIndex].ptr = (pointer) pPrivScreen;
        pPrivScreen->CloseScreen = pScreen->CloseScreen;
        pScreen->CloseScreen = apMonoScreenClose;
        
        pScreen->SaveScreen = apSaveScreen;
        pScreen->blackPixel = 0;
        pScreen->whitePixel = 1;
        
        retval = miDCInitialize (pScreen, &apMonoCursorFuncs);
        apMICursorScreen = pScreen;
        
        if (retval)
            retval = mfbCreateDefColormap (pScreen);
    }

    if (!retval)
        xfree(pPrivScreen);

    return retval;
}
