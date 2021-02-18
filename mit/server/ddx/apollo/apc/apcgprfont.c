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
 * Package to convert a given snf font into an apollo_nbit format font
 * containing the same glyphs, for use by the apc driver.
 */

#include "apc.h"
#include "apctext.h"
#include "fontstruct.h"
#include "snfstruct.h"
#include "dixfontstr.h"

#include <apollo/ms.h>
#include <apollo/fontn.h>

#ifndef NO_LOAD_FONT_PTR
extern void gpr_$load_font_ptr (
    char *                          & font_ptr,                 /* pointer to font image in memory */
    unsigned long int               & font_len,                 /* font image length */
    short int                       * font_id,                  /* returned font id */
    status_$t                       * status);                  /* returned status */
#endif

/* length in bytes of a scanline in the bag-o-bits */
#define LINELEN     28
#define ROWLEN      256
#define MAXCODE     ((1<<16) - 1)
#define NBYTES(x)   (((x)+7)>>3)

/*
 * bytes_needed_per_char -- Driver internal code
 *      Helper for mkGprFont.
 */
static int
bytes_needed_per_char(code)
    int code;
{
    int             bytes = 0;
    unsigned long   maxv  = 0;

    while (maxv < code)
    {
        bytes++;
        maxv <<= 8;
        maxv += 0xFF;
    }
    return bytes;
}

/*
 * set_offset -- Driver internal code
 *      Helper for mkGprFont.
 */
static void
set_offset(ap_font, code, val)
    fontn_$table_ptr_tver1  ap_font;
    int                     code;
    int                     val;
{
    int     i = ap_font->bytes_per_char;
    char   *tab = (char *)ap_font + ap_font->index_table_offset;

    tab += (code*i) + i;

    if ((tab-i) >= ((char *)((char *)ap_font) + ap_font->desc_table_offset))
        return;

    while (i--)
    {
        *--tab = val&0xFF;
        val >>= 8;
    }
}

/*
 * Statics to keep running track of the font image as it is constructed.
 */
static int      top, bottom, left, right, down;
static char    *image_ptr;

/*
 * init_image_computation -- Driver internal code
 *      Helper for mkGprFont.
 */
static void
init_image_computation()
{
    top = bottom = 0;
    left = down = 0;
    right = LINELEN;
}

/*
 * compute_image -- Driver internal code
 *      Helper for mkGprFont.
 */
static void
compute_image(width, height)
    int width, height;
{
    int hi;

    if (!width || !height)
    {
        width = 1;
        height = 0;
    }
    else
        width = NBYTES(width);

    if (width > (right - left))
    {
        top += down;
        down = left = 0;
    }

    if ((hi = height - (bottom - top)) > 0)
        bottom += hi;

    left += width;
    if (down < height)
        down = height;
}

/*
 * computed_image_size -- Driver internal code
 *      Helper for mkGprFont.
 */
static int
computed_image_size()
{
    return bottom * LINELEN;
}

/*
 * init_image_storage -- Driver internal code
 *      Helper for mkGprFont.
 */
static void
init_image_storage(ap_font, length)
    fontn_$table_ptr_tver1  ap_font;
    int                     length;
{
    top = bottom = 0;
    left = down = 0;
    right = LINELEN<<3;
    bzero (((char *)ap_font) + ap_font->index_table_offset,
           ap_font->desc_table_offset - ap_font->index_table_offset);
    bzero (((char *)ap_font) + ap_font->image_offset,
           length - ap_font->image_offset);
    image_ptr = (char *)(((char *)ap_font) + ap_font->image_offset);
}

/*
 * close_image_storage -- Driver internal code
 *      Helper for mkGprFont.
 */
static void
close_image_storage(ap_font)
    fontn_$table_ptr_tver1  ap_font;
{
    ap_font->raster_lines = bottom;
    ap_font->image_size = bottom * LINELEN;
}

/*
 * bitmap_copyout -- Driver internal code
 *      Helper for store_image.
 */
static void
bitmap_copyout(bits, x, y, width, height)
    char   *bits;
    int     x, y;
    int     width, height;
{
    int     wb = NBYTES(width);
    int     wbe = (wb + 1) & ~1;
    int     i, j;
    char   *ptr = image_ptr;

    if (x < 0 || y < 0)
        /* bail out if x or y is negative */
        return;

    ptr += y * LINELEN;
    ptr += x >> 3;

    for (i=0; i<height; i++)
    {
        bcopy (bits, ptr, wb);
        bits += wbe;
        ptr += LINELEN;
    }
}

/*
 * store_image -- Driver internal code
 *      Helper for mkGprFont.
 */
static void
store_image(desc, bits)
    fontn_$desc *desc;
    char        *bits;
{
    int hi, width, height;

    left += 7;
    left &= ~7;
    height = desc->up + desc->down;
    width = desc->left + desc->right;

    if (!width || !height)
    {
        width = 1;
        height = 0;
    }
    if (width > (right - left))
    {
        top += down;
        down = left = 0;
    }
    desc->x_pos = left;
    desc->y_pos = top;

    if ((hi = height - (bottom - top)) > 0)
        bottom += hi;

    if (height)
        bitmap_copyout (bits, left, top, width, height);
    left += width;
    if (down < height)
        down = height;
}

#ifdef NO_LOAD_FONT_PTR
#define fontPName "/tmp/gprXfont"
#define fontPNLen ((short)(sizeof(fontPName) - 1))
#endif

/*
 * cleanup -- Driver internal code.
 *        Delete our private storage, unload the GPR font.
 */
static gprFIDPtr
cleanup(gfp)
    gprFIDPtr   gfp;
#ifdef NO_TEXT16    /* two bodies, one for NO_TEXT16 (pre-sr10.2) and one otherwise */
{
    status_$t   status;
    int         fx;

    unlink (fontPName);
    for (fx=gfp->nGprFonts; --fx >= 0 ;)
        if (gfp->fontIds[fx])
            gpr_$unload_font_file (gfp->fontIds[fx], &status);
    xfree (gfp);
    return NULL;
}
#else   /* NO_TEXT16 */
{
    status_$t   status;

#ifdef NO_LOAD_FONT_PTR
    unlink (fontPName);
#else
    if (gfp->fontLoc)
        xfree (gfp->fontLoc);
#endif
    if (gfp->fontId)
        gpr_$unload_font_file (gfp->fontId, &status);
    xfree (gfp);
    return NULL;
}
#endif  /* NO_TEXT16 */

#define ABORT return (cleanup(gfp))
#define GPRVERSION  3
#define FONTTYPE    0
#define MAXFILESIZE (256*1024)

/*
 * mkGprFont -- Driver internal code
 *      Process the SNF font into a GPR font.
 */
static gprFIDPtr
mkGprFont(snfFont)
    FontPtr snfFont;
#ifdef NO_TEXT16    /* two bodies, one for NO_TEXT16 (pre-sr10.2) and one otherwise */
{
    status_$t               status;
    fontn_$table_ptr_tver1  alloc_font;
    int                     chars, j, k, n, cols, colOffset, chrs;
    int                     bytes, last_chr, d;
    CharInfoPtr             pci, pciStart;
    char                   *p;
    fontn_$desc            *desc;
    gprFIDPtr               gfp;
    int                     mapbuflen;
    unsigned long int       mapfilelen;
    int                     nfonts, fx;

    nfonts = (snfFont->pFI->lastRow - snfFont->pFI->firstRow + 1) *
             ((snfFont->pFI->lastCol / 256) + 1);
    gfp = (gprFIDPtr) xalloc (sizeof(gprFIDRec) + (nfonts-1)*sizeof(short));
    if (!gfp)
        return gfp;

    gfp->theXFont = snfFont;
    gfp->nGprFonts = 0;

    colOffset = snfFont->pFI->firstCol;
    pci = snfFont->pCI;

    for (fx=0 ; fx < nfonts ; fx++, gfp->nGprFonts++)
    {
        init_image_computation();
        if (colOffset > snfFont->pFI->lastCol)
            colOffset = snfFont->pFI->firstCol;
        cols = snfFont->pFI->lastCol - colOffset + 1;
        if (cols > 256)
            cols = 256;
        pciStart = pci;
        chrs = 0;
        for (j = 0; j < cols; j++)
        {
            if (pci->exists)
            {
                chrs++;
                compute_image(pci->metrics.rightSideBearing -
                              pci->metrics.leftSideBearing,
                              pci->metrics.ascent + pci->metrics.descent);
            }
            pci++;
        }

        if (chrs)
        {
            bytes = bytes_needed_per_char (chrs);
            last_chr = colOffset + cols - 1;
            mapbuflen = sizeof(fontn_$table_tver1) + 2; /* header + final ptr */
            mapbuflen += bytes * last_chr + 1;          /* index table + possible pad of 1 */
            mapbuflen += sizeof(fontn_$desc) * chrs;    /* size of descripter table */
            mapbuflen += computed_image_size();         /* size of bitmap */

            unlink (fontPName);
            alloc_font = (fontn_$table_ptr_tver1)
                         ms_$crmapl (fontPName, fontPNLen, 0, MAXFILESIZE, ms_$nr_xor_1w, &status);
            if (status.all != status_$ok)
                ABORT;
            if (!alloc_font)
                ABORT;

            alloc_font->version = GPRVERSION;
            alloc_font->font_type = FONTTYPE;
            alloc_font->release_number.fontn_major = 0;
            alloc_font->release_number.fontn_minor = 0;
            alloc_font->header_length = sizeof(fontn_$table_tver1);
            alloc_font->comparison_set = 0;
            alloc_font->scaling_factor = 1.0;
            alloc_font->v_spacing = 0;
            alloc_font->lowest_char_code = snfFont->pFI->firstRow * ROWLEN + snfFont->pFI->firstCol;
            alloc_font->highest_char_code = last_chr;
            alloc_font->chars_in_font = chrs;
            alloc_font->bytes_per_char = bytes;
            alloc_font->max_char_index = MAXCODE;
            alloc_font->max_width  = snfFont->pFI->maxbounds.characterWidth;
            alloc_font->space_type = snfFont->pFI->constantMetrics ?
                                         fontn_$mono_spacing : fontn_$proportional_spacing;
            alloc_font->max_right   = snfFont->pFI->maxbounds.rightSideBearing;
            alloc_font->max_down    = snfFont->pFI->maxbounds.descent;
            alloc_font->max_up      = snfFont->pFI->maxbounds.ascent;
            alloc_font->max_height  = alloc_font->max_down + alloc_font->max_up;
            alloc_font->font_height = alloc_font->max_height;
            alloc_font->index_table_offset = alloc_font->header_length;
            alloc_font->default_char = ' ';
            alloc_font->pad_01 = alloc_font->pad_02 =
                alloc_font->pad_03 = alloc_font->pad_04 = 0;

            alloc_font->desc_table_offset = alloc_font->index_table_offset +
                                            ((last_chr + 1) * bytes) + 1;   /* round up mod 2 */
            alloc_font->desc_table_offset &= ~1;    /* make sure it is even */
            alloc_font->image_offset = alloc_font->desc_table_offset +
                                       chrs * sizeof(fontn_$desc);

            init_image_storage (alloc_font, mapbuflen);
            d = 0;
            pci = pciStart;
            for (j = 0, k = snfFont->pFI->firstCol; j < cols; j++, k++)
            {
                if (pci->exists)
                {
                    desc = (fontn_$desc *)(((char *)alloc_font) +
                                           alloc_font->desc_table_offset);
                    desc += d;
                    set_offset (alloc_font, k, ++d);
                    desc->left  = -pci->metrics.leftSideBearing;
                    desc->right = pci->metrics.rightSideBearing;
                    desc->down  = pci->metrics.descent;
                    desc->up    = pci->metrics.ascent;
                    desc->width = pci->metrics.characterWidth;
                    store_image (desc, snfFont->pGlyphs + pci->byteOffset);
                }
                pci++;
            }
            close_image_storage (alloc_font);

            alloc_font->char_string_offset = alloc_font->image_offset +
                                             alloc_font->image_size;
            p = (char *)((char *)alloc_font) + alloc_font->char_string_offset;
            *p = 0;
            mapfilelen = alloc_font->char_string_offset + 1;

            if ((snfFont->pFI->firstRow == 0)   /* is <space> in glyph set? */
                && ((n = ' ' - snfFont->pFI->firstCol) >= 0)
                && (snfFont->pCI+n)->exists)
                alloc_font->space_size = (snfFont->pCI+n)->metrics.characterWidth;
            else
                alloc_font->space_size = alloc_font->max_right;
            alloc_font->em_space = alloc_font->en_space = alloc_font->space_size;
            alloc_font->inter_space_dflt = 0;      /* ???? */

            ms_$truncate ((void *)alloc_font, mapfilelen, &status);
            if (status.all != status_$ok)
                ABORT;
            ms_$unmap ((void *)alloc_font, mapfilelen, &status);
            if (status.all != status_$ok)
                ABORT;
            gpr_$load_font_file (fontPName, fontPNLen, &gfp->fontIds[fx], &status);
            if (status.all != status_$ok)
                ABORT;
        }
        else
            /* no characters in this section */
            gfp->fontIds[fx] = 0;

        colOffset += cols;
    }
    unlink (fontPName);

    return gfp;
}
#else   /* NO_TEXT16 */
{
    status_$t               status;
    fontn_$table_ptr_tver1  alloc_font;
    int                     chars, i, j, k, n, rows, cols, chrs;
    int                     bytes, last_chr, d;
    CharInfoPtr             pci;
    char                   *p;
    fontn_$desc            *desc;
    gprFIDPtr               gfp;
    int                     mapbuflen;
    unsigned long int       mapfilelen;

    gfp = (gprFIDPtr) xalloc (sizeof(gprFIDRec));
    if (!gfp)
        return gfp;

    gfp->theXFont = snfFont;
    gfp->fontId = 0;
#ifndef NO_LOAD_FONT_PTR
    gfp->fontLoc = NULL;
#endif

    rows = snfFont->pFI->lastRow - snfFont->pFI->firstRow + 1;
    cols = snfFont->pFI->lastCol - snfFont->pFI->firstCol + 1;

    init_image_computation();
    chrs = 0;
    pci = snfFont->pCI;
    for (n = rows*cols; n--; )
    {
        if (pci->exists)
        {
            chrs++;
            compute_image(pci->metrics.rightSideBearing -
                          pci->metrics.leftSideBearing,
                          pci->metrics.ascent + pci->metrics.descent);
        }
        pci++;
    }

    bytes = bytes_needed_per_char (chrs);
    last_chr = snfFont->pFI->lastRow * ROWLEN + snfFont->pFI->lastCol;
    mapbuflen = sizeof(fontn_$table_tver1) + 2; /* header + final ptr */
    mapbuflen += bytes * last_chr + 1;          /* index table + possible pad of 1 */
    mapbuflen += sizeof(fontn_$desc) * chrs;    /* size of descripter table */
    mapbuflen += computed_image_size();         /* size of bitmap */

#ifdef NO_LOAD_FONT_PTR
    unlink (fontPName);
    alloc_font = (fontn_$table_ptr_tver1)
                 ms_$crmapl (fontPName, fontPNLen, 0, MAXFILESIZE, ms_$nr_xor_1w, &status);
    if (status.all != status_$ok)
        ABORT;
#else
    alloc_font = (fontn_$table_ptr_tver1) xalloc (mapbuflen);
#endif
    if (!alloc_font)
        ABORT;

    alloc_font->version = GPRVERSION;
    alloc_font->font_type = FONTTYPE;
    alloc_font->release_number.fontn_major = 0;
    alloc_font->release_number.fontn_minor = 0;
    alloc_font->header_length = sizeof(fontn_$table_tver1);
    alloc_font->comparison_set = 0;
    alloc_font->scaling_factor = 1.0;
    alloc_font->v_spacing = 0;
    alloc_font->lowest_char_code = snfFont->pFI->firstRow * ROWLEN + snfFont->pFI->firstCol;
    alloc_font->highest_char_code = last_chr;
    alloc_font->chars_in_font = chrs;
    alloc_font->bytes_per_char = bytes;
    alloc_font->max_char_index = MAXCODE;
    alloc_font->max_width  = snfFont->pFI->maxbounds.characterWidth;
    alloc_font->space_type = snfFont->pFI->constantMetrics ?
                                 fontn_$mono_spacing : fontn_$proportional_spacing;
    alloc_font->max_right   = snfFont->pFI->maxbounds.rightSideBearing;
    alloc_font->max_down    = snfFont->pFI->maxbounds.descent;
    alloc_font->max_up      = snfFont->pFI->maxbounds.ascent;
    alloc_font->max_height  = alloc_font->max_down + alloc_font->max_up;
    alloc_font->font_height = alloc_font->max_height;
    alloc_font->index_table_offset = alloc_font->header_length;
    alloc_font->default_char = ' ';
    alloc_font->pad_01 = alloc_font->pad_02 =
        alloc_font->pad_03 = alloc_font->pad_04 = 0;

    alloc_font->desc_table_offset = alloc_font->index_table_offset +
                                    ((last_chr + 1) * bytes) + 1;   /* round up mod 2 */
    alloc_font->desc_table_offset &= ~1;    /* make sure it is even */
    alloc_font->image_offset = alloc_font->desc_table_offset +
                               chrs * sizeof(fontn_$desc);

    init_image_storage (alloc_font, mapbuflen);
    d = 0;
    pci = snfFont->pCI;
    for (i=snfFont->pFI->firstRow; i<=snfFont->pFI->lastRow; i++)
        for (j = 0, k = snfFont->pFI->firstCol + i*ROWLEN;
             j < cols; j++, k++)
        {
            if (pci->exists)
            {
                desc = (fontn_$desc *)(((char *)alloc_font) +
                                       alloc_font->desc_table_offset);
                desc += d;
                set_offset (alloc_font, k, ++d);
                desc->left  = -pci->metrics.leftSideBearing;
                desc->right = pci->metrics.rightSideBearing;
                desc->down  = pci->metrics.descent;
                desc->up    = pci->metrics.ascent;
                desc->width = pci->metrics.characterWidth;
                store_image (desc, snfFont->pGlyphs + pci->byteOffset);
            }
            pci++;
        }
    close_image_storage (alloc_font);

    alloc_font->char_string_offset = alloc_font->image_offset +
                                     alloc_font->image_size;
    p = (char *)((char *)alloc_font) + alloc_font->char_string_offset;
    *p = 0;
    mapfilelen = alloc_font->char_string_offset + 1;

    if ((snfFont->pFI->firstRow == 0)   /* is <space> in glyph set? */
        && ((n = ' ' - snfFont->pFI->firstCol) >= 0)
        && (snfFont->pCI+n)->exists)
        alloc_font->space_size = (snfFont->pCI+n)->metrics.characterWidth;
    else
        alloc_font->space_size = alloc_font->max_right;
    alloc_font->em_space = alloc_font->en_space = alloc_font->space_size;
    alloc_font->inter_space_dflt = 0;      /* ???? */

#ifdef NO_LOAD_FONT_PTR
    ms_$truncate ((void *)alloc_font, mapfilelen, &status);
    if (status.all != status_$ok)
        ABORT;
    ms_$unmap ((void *)alloc_font, mapfilelen, &status);
    if (status.all != status_$ok)
        ABORT;
    gpr_$load_font_file (fontPName, fontPNLen, &gfp->fontId, &status);
    if (status.all != status_$ok)
        ABORT;
    unlink (fontPName);
#else
    gpr_$load_font_ptr ((char *)alloc_font, mapfilelen, &gfp->fontId, &status);
    if (status.all != status_$ok)
        ABORT;
    gfp->fontLoc = (char *)alloc_font;
#endif

    return gfp;
}
#endif  /* NO_TEXT16 */

/*
 * This list exists mainly in case there are multiple screens.
 */
static gprFIDPtr fontList = NULL;

/*
 * gprRealizeFont - DDX interface (Screen)
 *      Build corresponding gpr font on the fly, and store font id in X font's devPriv.
 */
Bool
gprRealizeFont(pScreen, pFont)
    ScreenPtr   pScreen;
    FontPtr     pFont;
{
    gprFIDPtr   gfp;

    gfp = fontList;

    /* look for existing font */
    while (gfp)
    {
        if (gfp->theXFont == pFont)
        {
            gfp->use_count++;
            pFont->devPriv[pScreen->myNum] = (pointer) gfp;
            return TRUE;
        }
        gfp = gfp->nfr;
    }

    /* make new gpr font */
    gfp = mkGprFont (pFont);
    if (gfp)
    {
        if (fontList)
            fontList->pfr = gfp;
        gfp->nfr = fontList;
        gfp->pfr = NULL;
        fontList = gfp;
        gfp->use_count = 1;
    }

    /* and store as devPriv */
    pFont->devPriv[pScreen->myNum] = (pointer) gfp;
    return TRUE;
}

/*
 * gprUnrealizeFont - DDX interface (Screen)
 *      Deallocate, clean up GPR font.
 */
Bool
gprUnrealizeFont(pScreen, pFont)
    ScreenPtr   pScreen;
    FontPtr     pFont;
{
    gprFIDPtr           gfp;
    apcPrivScreenPtr    pPrivScreen;

    gfp = (gprFIDPtr) pFont->devPriv[pScreen->myNum];

    /* if all uses gone, then unlink and remove */
    if (gfp && !--(gfp->use_count))
    {
        if (gfp->nfr) gfp->nfr->pfr = gfp->pfr;
        if (gfp->pfr) gfp->pfr->nfr = gfp->nfr;
                 else fontList = gfp->nfr;
        cleanup (gfp);
    }
    return TRUE;
}
