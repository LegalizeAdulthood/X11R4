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

#include "apc.h"

/*
 * apcCreatePixmapFromData -- Driver internal code
 *      This routine will create and fill an X pixmap.
 *      pdata points to contiguous planes of contiguous longword-padded scanlines.
 *
 *      NOTE:  caller is responsible for freeing the data!
 */
PixmapPtr
apcCreatePixmapFromData (pScreen, width, height, depth, format, pdata)
    ScreenPtr       pScreen;
    int             width;
    int             height;
    int             depth;
    int             format;
    unsigned char  *pdata;
{
    PixmapPtr           pPixmap;
    apcPrivPMPtr        pPrivPM;
    apcPrivScreenPtr    pPrivScreen;

    pPixmap = (PixmapPtr)xalloc(sizeof(PixmapRec));
    if (!pPixmap)
        return NullPixmap;
    pPixmap->drawable.type = DRAWABLE_PIXMAP;
    pPixmap->drawable.class = 0;
    pPixmap->drawable.pScreen = pScreen;
    pPixmap->drawable.depth = depth;
    pPixmap->drawable.bitsPerPixel = 1;
    pPixmap->drawable.id = 0;
    pPixmap->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    pPixmap->drawable.x = 0;
    pPixmap->drawable.y = 0;
    pPixmap->drawable.width = width;
    pPixmap->drawable.height = height;
    pPixmap->devKind = -1;  /* not used */
    pPixmap->refcnt = 1;

    if ( !(pPrivPM = (apcPrivPMPtr)xalloc(sizeof(apcPrivPM))))
    {
        xfree (pPixmap);
        return NullPixmap;
    }
    pPixmap->devPrivate.ptr = (pointer)pPrivPM;

    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;
    pPrivPM->bitmap_desc = apMakeBitmapFromArray (width, height, depth,
                                                  pPrivScreen->attribute_block, format, pdata);
    if (!pPrivPM->bitmap_desc)
    {
        xfree (pPrivPM);
        xfree (pPixmap);
        return NullPixmap;
    }

    pPrivPM->size.x_size = width;
    pPrivPM->size.y_size = height;
    pPrivPM->bitmap_ptr = (int *)pdata;
    pPrivPM->width = (format == ZPixmap) ? PixmapBytePad(width, depth)
                                         : PixmapBytePad(width, 1);
    pPrivPM->width >>= 1;       /* words */
    pPrivPM->freeData = FALSE;  /* caller must set to TRUE if needed */

    return pPixmap;
}

/*
 * apcCreatePixmap -- DDX interface (screen)
 *      Create a pixmap usable by the apc driver.
 */
PixmapPtr
apcCreatePixmap (pScreen, width, height, depth)
    ScreenPtr   pScreen;
    int         width;
    int         height;
    int         depth;
{
    unsigned int    linebytes, totalbytes;
    unsigned char  *pdata;
    PixmapPtr       pPixmap;
    apcPrivPMPtr    pPrivPM;
    int             format;

    format = XYPixmap;
    linebytes = PixmapBytePad(width, 1);
    totalbytes = linebytes * height * depth;

    pdata = (unsigned char *) xalloc(totalbytes);
    if (!pdata)
        return NullPixmap;
    bzero ((char *)pdata, totalbytes);

    pPixmap = apcCreatePixmapFromData (pScreen, width, height, depth, format, pdata);
    if (!pPixmap)
    {
        xfree (pdata);
        return NullPixmap;
    }

    /* tell apcDestroyPixmap to free the bits */
    pPrivPM = (apcPrivPMPtr)pPixmap->devPrivate.ptr;
    pPrivPM->freeData = TRUE;

    return pPixmap;
}

/*
 * apcDestroyPixmap -- DDX interface (screen)
 *      Destroy an apc pixmap, freeing all storage.
 */
Bool
apcDestroyPixmap(pPixmap)
    PixmapPtr   pPixmap;
{
    apcPrivPMPtr    pPrivPM;

    if (--pPixmap->refcnt)
        return TRUE;

    pPrivPM = (apcPrivPMPtr)pPixmap->devPrivate.ptr;
    apDeallocateBitmap (pPrivPM->bitmap_desc);
    if (pPrivPM->freeData)
        xfree (pPrivPM->bitmap_ptr);
    xfree (pPrivPM);
    xfree (pPixmap);
    return TRUE;
}
