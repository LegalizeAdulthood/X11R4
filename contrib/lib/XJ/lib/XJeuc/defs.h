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

#ident	"@(#)defs.h	1.0"


/*----------------------------------------------------------------------*/
/*	Macros								*/
/*----------------------------------------------------------------------*/

#define MAX(a, b) 	((a) < (b) ? (b) : (a))
#define MIN(a, b) 	((a) > (b) ? (b) : (a))

#define CSET0		0x0000
#define CSET1		0x0001
#define CSET2		0x0002
#define CSET3		0x0003
#define EXTFLAG		0x8000

#define	NONE		0x0000
#define STANDARD	0x0001
#define EXTENSION	0x0002
#define ALL		(STANDARD | EXTENSION)


/*----------------------------------------------------------------------*/
/*	Temporary Buffer Size						*/
/*----------------------------------------------------------------------*/

#define	BUFSIZE		256


/*----------------------------------------------------------------------*/
/*	XLIB Functions							*/
/*----------------------------------------------------------------------*/

extern int		XDrawText();
extern int		XDrawText16();
extern int		XDrawString();
extern int		XDrawString16();
extern int		XDrawImageString();
extern int		XDrawImageString16();
