

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

/* Filename: mlinput.h  15 Nov. 1989,	By Hu Xian Xiang */

#include	<stdio.h>
#include	<X11/Xlib.h>
#include	<X11/Xos.h>
#include	"defines.h"
#include	"conf.h"

Config	record[32];			/* mldonfig record buffer     */
char	*displayname;
char	ExternalCode[12]=" ";
char	trans_buf[32];
int 	trans_ptr;
char	*TblAddr;
char	save_dialog[10];
char	save_hanzi[100];
char	Inmethod[10][2*METHOD];	/* input methods list	      */
char	CODE[4];		/* internal code 	      */
char	mapbuf[256];		/* ASCII-Local code table <256*/
int	map_len;		/* really ASCII-Local table length */
int	current=256;		/* current language number    */
int	keymap;			/* keyboard mapping file ID   */
int	MethodNum;
int	ML_MAX;
int	flag;
char	help[]="input.help";

Widget toplevel,frame,fontbut,lings_w,last_in_w;
Widget Method[10], Funwidget[2+fkeyno];

Window root, dialogwin, promptwin, CurWin; 

XColor	bg1,fg1;
Pixel	bg,fg;
XFontStruct * font;

Display *CurDpy;

Atom	hanzi_code, hanzi_type;

Cursor		ChoiceCursor;
GC		normalGC;		/* Current Language's GC*/
GC		asciiGC;
int		char_width;		/* font_width		*/
int		char_height;		/* font_height		*/

static	char	*FKEY[22]={
		"   =>  ",
		"   <=  ",
		"SP",
		" F1",
		" F2",
		" F3",
		" F4",
		" F5",
		" F6",
		" F7 ",
		" F8",
		" F9",
		"F10",
		"F11",
		"F12 ",
		"F13",
		"F14",
		"F15",
		"F16 ",
		"F17",
		"F18",
		"F19"
	};

