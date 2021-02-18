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
 * Utility functions for Apollo-display-independent parts of the driver.
 */

#include "apollo.h"

/*
 * Variable cacheing the GPR current bitmap.
 */
static gpr_$bitmap_desc_t apCurrentBitmap = gpr_$nil_bitmap_desc;

/*
 * apSetBitmap -- Driver internal code
 *      Wrapper for gpr_$set_bitmap.  Caches current bitmap.
 *      Returns the ID of the previously current bitmap.
 *
 *      Unfortunately, side-effecty parts of GPR may change the
 *      current bitmap (such as gpr_$[cond_]event_wait and
 *      gpr_$deallocate_bitmap).  Consistent use of these wrappers
 *      allows us to keep track of the current bitmap.
 */
gpr_$bitmap_desc_t
apSetBitmap (new)
    gpr_$bitmap_desc_t  new;
{
    if (new == apCurrentBitmap)
        return new;
    else
    {
        status_$t           status;
        gpr_$bitmap_desc_t  old;

        gpr_$set_bitmap (new, &status);
        old = apCurrentBitmap;
        apCurrentBitmap = new;
        return old;
    }
}

/*
 * apDeallocateBitmap -- Driver internal code
 *      Wrapper for gpr_$deallocate_bitmap.  Updates current bitmap cache.
 */
void
apDeallocateBitmap (bitmap)
    gpr_$bitmap_desc_t  bitmap;
{
    status_$t   status;

    gpr_$deallocate_bitmap (bitmap, &status);
    if (bitmap == apCurrentBitmap)
        gpr_$inq_bitmap (&apCurrentBitmap, &status);
}

/*
 * apMakeBitmapFromArray -- Driver internal code
 *      Wrapper for gpr_$make_bitmap_from_array.
 *      It is up to the caller to free pdata - apDeallocateBitmap won't do it.
 */

gpr_$bitmap_desc_t
apMakeBitmapFromArray(width, height, depth, attr, format, pdata)
    int                     width;
    int                     height;
    int                     depth;
    gpr_$attribute_desc_t   attr;
    int                     format;
    pointer                *pdata;
{
    status_$t               status;
    gpr_$offset_t           size;
    gpr_$bmf_group_header_t group_header;
    gpr_$bitmap_desc_t      bitmap;
    unsigned int            linebytes;

    /*
     * Compute the group header information needed to call gpr_$make_bitmap_from_array.
     */
    if (format == ZPixmap)
    {   /* we create a pixel-oriented GPR bitmap */
        linebytes = PixmapBytePad(width, depth);
        group_header.n_sects = 1;
        group_header.pixel_size = depth;
    }
    else
    {   /* we create a planewise GPR bitmap */
        linebytes = PixmapBytePad(width, 1);
        group_header.n_sects = depth;
        group_header.pixel_size = 1;
    }

    group_header.allocated_size = 0;
    group_header.bytes_per_line = linebytes;
    group_header.bytes_per_sect = linebytes * height;
    group_header.storage_offset = (char *)pdata;
    size.x_size = width;
    size.y_size = height;

    gpr_$make_bitmap_from_array (size, attr, (short)1, &group_header, &bitmap, &status);
    if (status.all != status_$ok)
        return (gpr_$bitmap_desc_t) gpr_$nil_bitmap_desc;
    else
        return bitmap;
}
