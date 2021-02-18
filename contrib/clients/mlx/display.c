

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

/* Filename: display.c	15 Nov. 1989,  By Hu Xian Xiang */

#include	"extern.h"
#include	"defines.h"

drawmlcode(mlcode,cx)
short	int	mlcode;
int	cx;
{
	char	code;

	if (mlcode == 8 ) {
		cx--;
		save_dialog[2*cx] ='<';
		save_dialog[2*cx+1] =0;
		RedrawDialog();
		return;
	}
	cx--;
	code = (mlcode>>8);
	drawchar(code,cx*2);
	code  = mlcode;
	drawchar(code,cx*2+1);
	drawchar('<',cx*2+2);
}
/********************************************************
 *   Display code[0~n]  in Dialog-Area              	*
 *   n is counter, =0 indicates clearing dialogwin 	*
 ********************************************************/
drawcode(code,cx)
char	code;
int	cx;
{
	if (!cx) {
		XClearWindow(CurDpy,dialogwin);
		drawchar('<',cx);
	}else
		if (code == bspace ) {
			drawchar(' ',cx);
		}else {
			drawchar(code,cx-1);
			drawchar('<',cx);
		}
}

/********************************************************
 *   Display keyin-code in Dialog-window		*
 *   column  is position.			       	*
 ********************************************************/
drawchar(c,column)
char c;
int  column;
{
	short	int MLcode;
	static	char	byte1;

	if (normalGC==NULL) return;
	if (column <0) return;
	save_dialog[column  ] =c;
	save_dialog[column+1] =0;
	if (c&0x80) {
		if (byte1){
			MLcode = byte1<<8 | (c & 0xff);
			byte1 = 0;
			XDrawImageString16(CurDpy,dialogwin,normalGC,
				(column-1)*10 ,16,&MLcode,1);
		}else
			byte1 = c;
	}else
		XDrawImageString(CurDpy,dialogwin,asciiGC,
			column*10,16,&c,1);
}

/*******************************************************
 *   Display Hanzi in Hanzi-prompt-window.	       *
 *   (line,column) is coordinate.		       *
 *******************************************************/
Drawstr(p,column)
char	*p;
int	column;	/* 16 bit each column. */
{
	register len;
	static	int	i;

	len = strlen(p)/2;
	if (!len) return;
	if (normalGC==NULL) return;
	XDrawImageString16(CurDpy,promptwin,normalGC,
		column*16,18,(XChar2b *)p,len);
	for (;i<column*2;) {
		save_hanzi[i++] = 0xa9 ;
		save_hanzi[i++] = 0xa1 ;
	}
	for (i=column*2; save_hanzi[i] = *p++; i++);
}

/*******************************************************
 *   Display ASCII in Hanzi-prompt-window.	       *
 *   (column*8,16) is coordinate.		       *
 *******************************************************/
drawstr(p,column)
char	*p;
int	column;
{
	register	i;

	XDrawImageString(CurDpy,promptwin,asciiGC,
		column*char_width,18,p,strlen(p));
}

cls_prompt(i)
char	i;
{
	static char p[] = "                                                                                       ";

	if (!i){
		XClearWindow(CurDpy,promptwin);
		return;
	}
	XDrawImageString(CurDpy,promptwin,asciiGC,
		i*16,10,p,strlen(p));
	XDrawImageString(CurDpy,promptwin,asciiGC,
		i*16,19,p,strlen(p));
}

beep()
{
	XBell(CurDpy,50);
}
