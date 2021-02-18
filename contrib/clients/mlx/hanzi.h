

#include <X11/copyright.h>

/*
 * Copyright 1989    Institute of Systems Science (ISS),
 *		     National University of Singapore , Singapore
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of ISS not be used 
 * in advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. ISS makes no representations
 * about the suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
 *
 *
 * ISS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * ISS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * CONTACT:      HU XIAN XIANG,
 * 		 INSTIUTE OF SYSTEMS SCIENCE, 
 *               NATIONAL UNIVERSITY OF SINGAPORE, 	
 *               REPUBLIC OF SINGAPORE  0511
 * FAX:          (65)7782571
 * TELEX:        ISSNUS RS 39988
 * BITNET:       ISSMLX@NUSVM 
*/

/* Filename: hanzi.h  15 Nov. 1989,	By Hu Xian Xiang */

#include <X11/Xlib.h>

#define isflag(c)	(c&0x80) 

typedef	struct	{
	short int	l_no;		/* ON / OFF 		*/
	char		l_dir;		/* writting direction L / R */
	char  		l_fontname[20]; /* input font name	*/
	char		l_boldname[20];	/* display font name 	*/
	char		l_style;	/* current style       	*/
	Font		l_fontid;	/* current font ID     	*/
	Font		l_bold_fontid;	/* current bold font ID	*/
	GC		revboldGC;
	GC		norboldGC;
	GC		reverseGC;
	GC		 normalGC;
	int		width;		/* font_width		*/
	int		height		/* font_height		*/
}ML_FONT;

typedef	struct	{
	unsigned char	language;		/* Lingual Code */
	unsigned char	class;			/* Class	*/
}ML_CODE;

extern	ML_FONT	L_FONT[];
extern	int	ML_MAX;
