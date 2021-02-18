/*
 * 		    Copyright 1989 TOSHIBA CORPORATION
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of TOSHIBA Corporation not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. TOSHIBA Corporation makes no 
 * representations about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 * TOSHIBA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
 * TOSHIBA BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
 * SOFTWARE.
 *
 * Author:  
 * 		Masaki Takahashi
 * 		Katsuhisa Yano
 *		Personal Computer Design Dept.
 *		TOSHIBA CORPORATION OME WORKS
 *		5-20-7 SORD BLD. MASAGO 
 *		CHIBA-CITY CHIBA 260 JAPAN
 *		(0472) 77-8670
 */

#ident	"@(#)XJEuc.h	1.1"


#ifndef	_XJEuc_h
#define	_XJEuc_h

#define	XJPSS2	0x008e
#define	XJPSS3	0x008f

/*----------------------------------------------------------------------*/
/*	Euc Font Structure 						*/
/*----------------------------------------------------------------------*/

typedef struct _XJeFontStruct {
	XFontStruct	*font_std[4];	/* STD Font Struct		*/
	XFontStruct	*font_ext[4];	/* EXT Font Struct		*/
	int		ascent;		/* Max ascent			*/
	int		descent;	/* Max descent			*/
} XJeFontStruct;


/*----------------------------------------------------------------------*/
/*	Euc FontName Structure						*/
/*----------------------------------------------------------------------*/

typedef struct _XJeFontNameStruct {
	char	*fontname_std[4];	/* STD Font Name		*/
	char	*fontname_ext[4];	/* EXT Font Name		*/
} XJeFontNameStruct;

#endif	/* _XJEuc_h */
