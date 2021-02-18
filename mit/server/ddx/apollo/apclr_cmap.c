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
 * Functions implementing color-specific parts of the driver
 * having to do with colormaps.
 */

#include "apollo.h"
#include "colormapst.h"

extern int  TellLostMap(), TellGainedMap();

/* Constant value for GPR colormap entry to be ignored */
#define DONT_UPDATE (0xFFFFFFFF)

/*
 * apClrCreateColormap -- DDX interface (screen)
 *      If the colormap being created is the default one, allocate whitePixel and
 *      blackPixel.
 */
static Bool
apClrCreateColormap(pmap)
    ColormapPtr pmap;
{
    ScreenPtr       pScreen;
    unsigned short  dark = 0;
    unsigned short  bright = ~0;

    pScreen = pmap->pScreen;

    if ((pmap->flags) & IsDefault)
    {
        if (AllocColor(pmap, &bright, &bright, &bright, &(pScreen->whitePixel), 0) != Success)
            return FALSE;
        if (AllocColor(pmap, &dark, &dark, &dark, &(pScreen->blackPixel), 0) != Success)
            return FALSE;
    }
    return TRUE;
}

/*
 * apClrDestroyColormap -- DDX interface (screen)
 *      Nothing to do, since we didn't allocate any dynamic resources
 *      in apClrCreateColormap.
 */
static void
apClrDestroyColormap(pmap)
    ColormapPtr pmap;
{
}

/*
 * apUpdateColormap -- Driver internal code
 *      Update the hardware color map of the specified Apollo display.
 *      Special hack:  if an entry has the value DONT_UPDATE,
 *      that means don't change this color slot.  This may reduce
 *      technicolor effects when installing a new color map.
 */
static void
apUpdateColormap(pDisp, start_index, n_entries, gpr_map)
    apDisplayDataPtr    pDisp;
    int                 start_index;
    int                 n_entries;
    gpr_$color_vector_t gpr_map;
{
    gpr_$bitmap_desc_t  valid_bitmap;
    status_$t           status;
    gpr_$pixel_value_t  i1, i2, l;
    short               n;

    valid_bitmap = apSetBitmap(pDisp->display_bitmap);

    i1 = 0;
    while (i1 < n_entries)
    {
        if (gpr_map[i1] == DONT_UPDATE)
            i1++;
        else
        {
            i2 = i1;
            do i2++; while ( (i2 < n_entries) && (gpr_map[i2] != DONT_UPDATE) );
            n = i2 - i1;
            gpr_$set_color_map (i1+start_index, n, &gpr_map[i1], &status);
            i1 = i2;
        }
    }

    apSetBitmap(valid_bitmap);
}

/*
 * apScanAndUpdateColormap -- Driver internal code
 *      Given the DIX colormap, convert it to GPR form and call
 *      apUpdateColormap on it.
 */
static void
apScanAndUpdateColormap(pDisp, pmap)
    apDisplayDataPtr    pDisp;
    ColormapPtr         pmap;
{
    int                 i;
    int                 nEnt;
    Entry              *pEnt;
    gpr_$color_vector_t gpr_map;
    unsigned long       red, green, blue;

    nEnt = pmap->pVisual->ColormapEntries;
    for (i = 0, pEnt = pmap->red; i < nEnt; i++, pEnt++)
        if (pEnt->refcnt)
        {
            if (pEnt->fShared)
            {
                red = pEnt->co.shco.red->color >> 8;
                green = pEnt->co.shco.green->color >> 8;
                blue = pEnt->co.shco.blue->color >> 8;
            }
            else
            {
                red = pEnt->co.local.red >> 8;
                green = pEnt->co.local.green >> 8;
                blue = pEnt->co.local.blue >> 8;
            }
            gpr_map[i] = (red << 16) | (green << 8) | blue;
        }
        else
            gpr_map[i] = DONT_UPDATE;

    apUpdateColormap (pDisp, 0, nEnt, gpr_map);
}

/*
 * apClrInstallColormap -- DDX interface (screen)
 *      Install the given colormap on its screen.  If it's already installed,
 *      do nothing.  If another is installed, uninstall it first (we only support
 *      one color map at a time).
 */
static void
apClrInstallColormap(pmap)
    ColormapPtr pmap;
{
    ScreenPtr           pScreen;
    apClrPrivScreenPtr  pPrivScreen;
    apDisplayDataPtr    pDisp;

    pScreen = pmap->pScreen;
    pPrivScreen = (apClrPrivScreenPtr) pScreen->devPrivates[apClrScreenIndex].ptr;
    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;

    if (pmap != pPrivScreen->installedCmap)
    {
        if (pPrivScreen->installedCmap)
            WalkTree (pScreen, TellLostMap, (char *) &(pPrivScreen->installedCmap->mid));
        pPrivScreen->installedCmap = pmap;

        apScanAndUpdateColormap (pDisp, pmap);

        WalkTree (pmap->pScreen, TellGainedMap, (char *) &(pmap->mid));
    }
}

/*
 * apClrUninstallColormap -- DDX interface (screen)
 *      Uninstall the given colormap on its screen, assuming it's the installed
 *      one and not the default.  Install the default colormap instead.
 */
static void
apClrUninstallColormap(pmap)
    ColormapPtr pmap;
{
    ScreenPtr           pScreen;
    apClrPrivScreenPtr  pPrivScreen;
    Colormap            defaultMapID;
    ColormapPtr         pDefaultMap;

    pScreen = pmap->pScreen;
    pPrivScreen = (apClrPrivScreenPtr) pScreen->devPrivates[apClrScreenIndex].ptr;

    if (pmap == pPrivScreen->installedCmap)
    {
        defaultMapID = pmap->pScreen->defColormap;
        if (pmap->mid != defaultMapID)
        {
            pDefaultMap = (ColormapPtr) LookupIDByType (defaultMapID, RT_COLORMAP);
            (*pScreen->InstallColormap) (pDefaultMap);
        }
    }
}

/*
 * apClrListInstalledColormaps -- DDX interface (screen)
 *      Return the list of installed colormaps for a screen, which for us will
 *      always be of length 1.
 */
static int
apClrListInstalledColormaps(pScreen, pCmapList)
    ScreenPtr   pScreen;
    Colormap   *pCmapList;
{
    apClrPrivScreenPtr  pPrivScreen;

    pPrivScreen = (apClrPrivScreenPtr) pScreen->devPrivates[apClrScreenIndex].ptr;
    *pCmapList = pPrivScreen->installedCmap->mid;
    return 1;
}

/*
 * apClrStoreColors -- DDX interface (screen)
 *      If the given colormap is the installed one, modify the hardware
 *      colormap entries with the given list of new entries.
 */
static void
apClrStoreColors(pmap, ndef, pdefs)
    ColormapPtr pmap;
    int         ndef;
    xColorItem *pdefs;
{
    ScreenPtr           pScreen;
    apClrPrivScreenPtr  pPrivScreen;
    apDisplayDataPtr    pDisp;

    pScreen = pmap->pScreen;
    pPrivScreen = (apClrPrivScreenPtr) pScreen->devPrivates[apClrScreenIndex].ptr;

    if (pmap == pPrivScreen->installedCmap)
    {
        int                 i, npix;
        gpr_$pixel_value_t  pix, minpix, maxpix;
        unsigned long       red, green, blue;
        xColorItem         *pd;
        gpr_$color_vector_t gpr_map;

        for (i = ndef , pd = pdefs , maxpix = 0 , minpix = 0x000000FF ; i-- ; pd++)
        {
            pix = (pd->pixel) & 0x000000FF;
            if (maxpix < pix) maxpix = pix;
            if (minpix > pix) minpix = pix;
        }
        npix = maxpix - minpix + 1;
        for (i = 0 ; i < npix ; i++)
            gpr_map[i] = DONT_UPDATE;

        for (i = ndef , pd = pdefs ; i-- ; pd++)
        {
            pix = (pd->pixel) & 0x000000FF;
            red = (pd->red) >> 8;
            green = (pd->green) >> 8;
            blue = (pd->blue) >> 8;

            gpr_map[pix-minpix] = (red << 16) | (green << 8) | blue;
        }
        pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;
        apUpdateColormap (pDisp, minpix, npix, gpr_map);
    }
}

/*
 * apClrResolveColor -- DDX interface (screen)
 *      Find the nearest displayable color to a given color.
 *      Adjust the input red, green and blue values accordingly.
 *
 *      Code lifted from cfb.
 */
static void
apClrResolveColor(pred, pgreen, pblue, pVisual)
    unsigned short     *pred;
    unsigned short     *pgreen;
    unsigned short     *pblue;
    VisualPtr           pVisual;
{
    int         shift = 16 - pVisual->bitsPerRGBValue;
    unsigned    lim = (1 << pVisual->bitsPerRGBValue) - 1;

    if ((pVisual->class == PseudoColor) || (pVisual->class == DirectColor))
    {
	/* rescale to rgb bits */
	*pred = ((*pred >> shift) * 65535) / lim;
	*pgreen = ((*pgreen >> shift) * 65535) / lim;
	*pblue = ((*pblue >> shift) * 65535) / lim;
    }
    else if (pVisual->class == GrayScale)
    {
	/* rescale to gray then rgb bits */
	*pred = (30L * *pred + 59L * *pgreen + 11L * *pblue) / 100;
	*pblue = *pgreen = *pred = ((*pred >> shift) * 65535) / lim;
    }
    else if (pVisual->class == StaticGray)
    {
	unsigned limg = pVisual->ColormapEntries - 1;
	/* rescale to gray then [0..limg] then [0..65535] then rgb bits */
	*pred = (30L * *pred + 59L * *pgreen + 11L * *pblue) / 100;
	*pred = ((((*pred * (limg + 1))) >> 16) * 65535) / limg;
	*pblue = *pgreen = *pred = ((*pred >> shift) * 65535) / lim;
    }
    else
    {
	unsigned limr, limg, limb;

	limr = pVisual->redMask >> pVisual->offsetRed;
	limg = pVisual->greenMask >> pVisual->offsetGreen;
	limb = pVisual->blueMask >> pVisual->offsetBlue;
	/* rescale to [0..limN] then [0..65535] then rgb bits */
	*pred = ((((((*pred * (limr + 1)) >> 16) *
		    65535) / limr) >> shift) * 65535) / lim;
	*pgreen = ((((((*pgreen * (limg + 1)) >> 16) *
		      65535) / limg) >> shift) * 65535) / lim;
	*pblue = ((((((*pblue * (limb + 1)) >> 16) *
		     65535) / limb) >> shift) * 65535) / lim;
    }
}

/*
 * apClrInitColormap -- Driver internal code
 *      Initialize the screen for a pseudocolor colormap.
 */
void
apClrInitColormap(pScreen, pDisp, pPrivScreen)
    ScreenPtr           pScreen;
    apDisplayDataPtr    pDisp;
    apClrPrivScreenPtr  pPrivScreen;
{
    ColormapPtr         pColormap;

    pScreen->CreateColormap = apClrCreateColormap;
    pScreen->DestroyColormap = apClrDestroyColormap;
    pScreen->InstallColormap = apClrInstallColormap;
    pScreen->UninstallColormap = apClrUninstallColormap;
    pScreen->ListInstalledColormaps = apClrListInstalledColormaps;
    pScreen->StoreColors = apClrStoreColors;
    pScreen->ResolveColor = apClrResolveColor;

    /* create colormap data for screen */

    pScreen->minInstalledCmaps = 1;
    pScreen->maxInstalledCmaps = 1;
    pScreen->defColormap = FakeClientID (0);

    pPrivScreen->installedCmap = NULL;
}
