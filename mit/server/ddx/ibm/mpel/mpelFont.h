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
#ifndef MPEL_FONT_SEEN
#define	MPEL_FONT_SEEN 1
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelFont.h,v 6.2 88/10/25 01:39:49 kbg Exp Locker: paul $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelFont.h,v $ */

#ifndef lint
static char *rcsidmpelfont = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelFont.h,v 6.2 88/10/25 01:39:49 kbg Exp Locker: paul $";
#endif

	/*
	 * Megapel fonts can be up to 32k words, so use BANK2 (64k) 
	 * to hold all of the megapel fonts.  The eight fonts share
	 * a pattern area (technically, header 1 is in the pattern area
	 * of font 0, but hey...)
	 * Format of the font region is:
	 * 	:-------------------------------:
	 *	|	ATF Location Table	|	52 bytes
	 *	|	Bookeeping info		|	12 bytes
	 *	|	ATF Header 0		|	44 bytes each
	 *	|		:		|		:
	 *	|	ATF Header 7		|		:
	 *	|	Pad			|	96 bytes
	 *	|-------------------------------|
	 *	|  	Pattern area		|  
	 *	|  	238 bands of 256 bytes	|	60928 bytes
	 *	|	each			|
	 *	|-------------------------------|
	 *	|	Lookup table 0      	|	512 bytes each
	 *	|		:		|		:
	 *	|	Lookup table 7		|		:
	 *	`-------------------------------'
	 *
	 * One megapel font may hold characters from many X fonts
	 * (all that are the same width).  Bands in the pattern
	 * area are allocated to X fonts (not megapel fonts) and each band
	 * has a (one byte) index of the previous band in the font, a (one
	 * byte) index of the next free (16 bit) word in the band, a sequence
	 * number (so bands within a font can be freed LRU) and 252 bytes (126 
	 * words) of pattern data.
	 *
	 * For each X font we need to maintain:
	 *	mpelFont:	(0-7) index of the corresponding megapel font
	 *	mpelId:		(0-1024) megapel id of the corresponding font
	 *	currentBand:	(0-237) index of the most recently allocated
	 *			pattern band.  A value of MPEL_ATF_NOBAND
	 *			indicates that no bands have been allocated.
	 *	numMapped:	The number of characters mapped in this font.
	 *			(used by FreeSpace to figure out which fonts
	 *			to clean out)
	 *	width,height:	width and height (in pels) of the font.
	 *	nChars		the total number of characters in the X font.
	 *	mpelMap:	mapping from X character index onto
	 *			megapel character index.  Since many X fonts
	 *			may share a single megapel font, this is not
	 *			a direct mapping.  A value of 0 indicates that
	 *			the character is not currently mapped.
	 *	lastUse:	fonts are freed on a least recently used basis,
	 *			so we need to keep the sequence number of the
	 *			most recent use of this X font.
	 *	nextXFont:	each megapel font must keep a linked list of X
	 *			fonts using it, so it can free things up when
	 *			it has to.
	 * For the megapel font region we need to maintain:
	 *	myFonts:	(mpelMpelFont *[8]) array of pointers to 
	 *			structures (in system memory) describing the
	 *			megapel fonts that live in the font region.
	 *	nextFreeBand:	(0-238) next free pattern band.  A vaue of
	 *			MPEL_ATF_NOBAND indicates there are no more free
	 *			bands.
	 *	nextUse:	highest sequence number for the megapel font
	 *			to date.
	 * For each megapel font we need to maintain (in system memory):
	 *	myRegion:	(mpelSysRegion *) Pointer to the region this
	 *			font lives in.
	 *	mpelHeader:	(mpelATFHeader *) pointer to header (on megapel
	 *			adapter) describing font.
	 *	myLookup:	pointer to font lookup table for this megapel 
	 *			font.
	 *	nextFreePt:	(0-255) next free code point.  Lookup table 
	 *			entries that do not correspond to X font 
	 *			characters should form a list of free points.
	 *			A value of zero indicates there are no more free
	 *			points.
	 *	xFonts:		(mpelXFont *) pointer to a linked list of X 
	 *			fonts using this megapel font.
	 */

	/*
	 * band stuff
	 */

#define	MPEL_ATF_NUMBANDS	238
#define	MPEL_ATF_MAXBAND	(MPEL_ATF_NUMBANDS-1)
#define	MPEL_ATF_NOBAND		255
#define	MPEL_BAND_PTRNSIZE	127

#define	bandSpaceLeft(b)	(MPEL_BAND_PTRNSIZE-(b)->nextPtrn)
#define	bandNextSpace(b)	(&((b)->pattern[(b)->nextPtrn]))

	/*
	 * mpelMpelFont stuff
	 */

#define	fontWordSize(m)		(((m)->myHeader->fd.bitsperchar)/16)
#define	mfontWidth(m)		((m)->myHeader->fd.width)
#define	mfontHeight(m)		((m)->myHeader->fd.height)

	/*
	 * mpelXFont stuff
	 */
#define	MPEL_ATF_NUMFONTS	8
#define	MPEL_XCHAR_UNDEFINED	0
#define	fontWidth(x)		((x)->width)
#define	fontHeight(x)		((x)->height)
#define	fontNChars(x)		((x)->nChars)
#define	setFontDimensions(x)	\
	{\
		if ((mfontHeight((x)->myMpelFont)!=(x)->height)||\
		    (mfontWidth((x)->myMpelFont)!=(x)->width)) {\
			MPELWaitFifo();\
			(x)->myMpelFont->myHeader->fd.width=(x)->width;\
			(x)->myMpelFont->myHeader->fd.height=(x)->height;\
		}\
	}

	/*
	 * miscellany
	 */

#define	MPEL_ATF_PADSIZE		96

	/*
	 * structures located in adapter memory
	 */

typedef	struct {
	CARD16	numEntries;
	struct {
		CARD16	fid;
		CARD16	addrHigh;
		CARD16	addrLow;
	} entries[8];
	CARD16	pad;
} mpelATFLocTable;

typedef struct {
	CARD32		size;
	CARD16		class;
	CARD16		fontId;
	CARD32		fontStyle;
	CARD32		fontAttribute;
	struct FontDescriptor {
		CARD32	fontTotalChars;
		CARD32	tblWordSize;
		CARD16	baseline;
		CARD16	capline;
		CARD16	width;
		CARD16	height;
		CARD16	bitsperchar;
		CARD16	underlineTop;
		CARD16	underlineBase;
		CARD8	monoPitch;
		CARD8	pad;
		CARD32	lookupTblOffset;
	} fd;
} mpelATFHeader;

typedef struct {
	CARD32		 nextFreeBand;
	CARD32		 nextUse;
	struct MPMFONT	*myFonts;
} mpelATFBookeeping;

typedef struct {
	CARD8	nextBand;
	CARD8	nextPtrn;
	CARD16	pattern[MPEL_BAND_PTRNSIZE];
} mpelATFBand;

typedef CARD16	mpelATFLookup[256];

typedef struct {
	mpelATFLocTable		atfLocations;
	mpelATFBookeeping	usage;
	mpelATFHeader		atfHeaders[MPEL_ATF_NUMFONTS];
	CARD8			pad[MPEL_ATF_PADSIZE];
	mpelATFBand		bands[MPEL_ATF_NUMBANDS];
	mpelATFLookup		lookup[MPEL_ATF_NUMFONTS];
} mpelATFRegion;

	/*
	 * structures in system memory:
	 */


typedef struct MPMFONT {
	mpelATFRegion	*myRegion;
	mpelATFHeader	*myHeader;
	CARD16		*myLookup;
	int		 nextFreePt;
	struct MPXFONT	*xFonts;
} mpelMpelFont;

typedef struct MPXFONT {
	mpelMpelFont	*myMpelFont;
	unsigned int	 mpelId;
	unsigned int	 currentBand;
	int		 numMapped;
	int		 nChars;
	int		 width,height;
	CARD8		 *mpelMap;
	unsigned int	 lastUse;
	struct MPXFONT	*nextXFont;
} mpelXFont;

#endif /* ndef MPEL_FONT_SEEN */
