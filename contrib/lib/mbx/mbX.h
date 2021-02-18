/*@(#)mbX.h	1.11*/
/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* 
 * Name: mbX.h - include file for mbX functions
 *
 * Description: 
 *
 * This file describes the format of the locale information used 
 * the mbX functions.  The following describe the parsing alogorithm
 * used to map from an character set code point to a glyph index.  Note
 * that the index table only contains glyph indexes and the CSid table 
 * identifies which font (CSid - character set id) to use.
 *
 * PARSING ALGORITHM
 *
 * Each byte in the output data stream is used as an index into the Character
 * Index array.  The array contains modifiers and Character Glyph offsets.
 * There are two types of modifiers: base modifiers and index modifiers.
 * Base modifiers are used to modify all following data stream bytes.  The
 * initial base modifying value is zero.  Index modifiers are used to modify
 * the next sequential data stream byte.
 *
 * Each Character Index array entry has control information in the high
 * order 4 bits and a value in the low order 28 bits.  The 4-bit control
 * code is used to interpret the 28-bit value as follows:
 *
 *    4-bit code                     28-bit value
 *    ----------  ---------------------------------------------------------
 *       0000      offset to a glyph which is also a code point
 *       0100      offset to a glyph which is not a code point
 *       1000      index modifier (added to next data stream byte)
 *       0010      base modifier (added to all following data stream bytes)
 *
 * NOTE:  The Character Index array must contain at least 256 entries.
 *        Unused array entries can be either an offset to a default glyph
 *        or an index modifier of zero.
 *
 * The Character Glyphs contain information unique to each character plus
 * the (possibly encoded) raster image.
 *
 *                  EFFECTIVE  PROCESSING  ALGORITHM
 * ----------------------------------------------------------------------
 *
 *   unsigned base_modifier, index_modifier, value, n;
 *   Both of base_modifier and index_modifier are to be initialized to 0
 *   for each independent character stream to be processed.
 *
 *   if ((n = data_stream_byte + index_modifier + base_modifier)
 *                                                   <= font_head->last_index)
 *   {
 *     index_modifier = 0;
 *     value = CHAR_INDEX[n];
 *     if ( value & INDBASE )
 *       base_modifier = value & ~INDMASK;
 *       process next character in data stream
 *     else if ( value & INDMOD )
 *       index_modifier = value & ~INDMASK;
 *       process next character in data stream
 *     else if ((value & INDMASK) == 0)
 *       glyph_offset = value & ~INDMASK;
 *       process glyph
 *       process next character in data stream
 *     else
 *       character index array data error
 *   {
 *   else
 *       offset into CHAR_INDEX is in error
 */
#include <X11/Xlib.h>

typedef char	mbXindex[4];
typedef int	mbX4byte;	/* Must be 4 bytes */
		
#define INDMOD  0x80            /* 28-bit value is an index modifier */
#define INDCPT  0x40            /* index is Not a code point if set */
#define INDBASE 0x20            /* 28-bit value is a base modifier */
#define INDR_L  0x20            /* Right to left value */
#define INDMASK 0xF0000000      /* 4-bit control bit mask */

/*
 * The mbXLocaleHeader will contain most of the information needed 
 * by the mbX functions.  It is intended that locale information 
 * will be located as a seperate file identified by the locale 
 * setting (eg. /usr/lib/X11/nls/$LC_CTYPE).  
 *
 * The encoding array will contain patterns that can be used to 
 * validate the fonts opened with the locale being used.  The encoding
 * pattern should be matched to the CHARSET_ENCODING property of the 
 * font requested.  If the property exist and it does not match the 
 * encoding the font may be refused dependent on implementations.    
 *
 * Following the header info will be the index array used to map locale 
 * code points to glyph index. 
 *
 */  

typedef struct {
   int version;			/* mbXversion */
   int num_CSid;		/* number of code sets for this locale   */
   int last_index;		/* number of indexes */
   char	CSid[256];		/* table of CSid - derived from 1st byte */
				/* This table is used to identify which font */
				/* a given code point is using in the fontlist */
   char encoding[256];		/* valide CHARSET_ENCODING ATOMs */
                      		/* Indexes start here - See Processing Alogorithm */
				/* This table is used to identify the glyph index */
				/* within a font.  Font id needs to be gotten from*/
				/* CSid table. */
} mbXLocaleHeader;

typedef struct {
   mbXLocaleHeader hdr;		/* header */
   mbX4byte indexes[256];		/* Default Locale table */
} mbXLocale;

#define MBX_VERSION 0x00010001	/* 1.1 */ 

extern mbXLocale *_mbXlocale;

XFontStruct **mbXLoadQueryFont();
char *mbXsetlocale();

