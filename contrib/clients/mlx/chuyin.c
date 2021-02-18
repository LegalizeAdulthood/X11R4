

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

/* Filename: chuyin.c  15 Nov. 1989,	By Hu Xian Xiang */

#include	<X11/Xos.h>
#include	"defines.h"
#include	"chuyin.h"

extern	char	*TblAddr;
extern	char	*ext_string;
extern	char	hzbuf[];
extern	int	hcc;

cacis(c)
char	c;
{
	static	int	init;

	if (init)	return(chuyin(c));
	init++;

	radical_p= open(char_f,O_RDONLY);
	if (radical_p<0) {
		fprintf(stderr,"Cannot open file: %s!\n",char_f);
		return(1);
	}
	read(radical_p,TblAddr+10,13526);
	close(radical_p);

	radical_p = open(radical_f,O_RDONLY);
	if (radical_p <0) {
		fprintf(stderr,"Cannot open file: %s!\n",radical_f);
		return(1);
	}
	LoadRadical(radical_p,rad);


	char_buf[0] = buf0;
	char_buf[1] = buf1;
	char_buf[2] = buf2;
	char_buf[3] = buf3;

	return(chuyin(c));
}

keytochuyin(c)
char	c;
{
	int	i;

	for (i=0;i<MAX_RAD;i++)
		if (c==key[i])
			return(i);
	return(-1);
}
chuyin(c)
char	c;
{
	int	symbol;
	int	total,i,location,size;
	Char	*p;

	if (hcc>3) {
		drawcode(' ',hcc=0);
		beep();
		return;
	}
	symbol = keytochuyin(c);
	if (symbol<0) {
		hcc--;
		return;
	}
	drawmlcode(rad[symbol].code,hcc);
	UpdateBuf(hcc-1,symbol);

	for ( i=0, p=char_buf[hcc-1]; p->code != END ;p++) {
		location = p->code * 2;
		hzbuf[i++] = *(TblAddr+location +10);
		hzbuf[i++] = *(TblAddr+location +11);
	}
	return(i);
}


/* Load and select characters order no to char_buf[no] */

UpdateBuf(no,orderno)
int	no;		/* Radical Sequnece Number */
Hanzi	orderno;	/* Input key word */
{
	int	i,cnt,size;
	long	location;
	Char	*p,*Code, *buf;

	for(i=0,location=0;i<orderno;i++){
		location += rad[i].size*3;
		location += 4;	/*code,size */
	}
	location += 4;		/*code,size */
	lseek(radical_p,location,0);
	size = rad[orderno].size;
	for (i=0;i<size;i++)
		read(radical_p,&char_tmp[i],3);
/*
	Code= char_tmp;
	for (i=0,cnt=0;i<=no;i++)
		if (ext_string[i]==ext_string[no])
			cnt++;	
	for (size=0;p->code!=END;p++) 
		if (p->cnt >= (char)cnt) {
			Code->code =p->code;
			Code++;
			size++;
		}
	Code->code = END;
Number of same radicals */

	for (cnt=no+1,p=char_tmp,Code=char_tmp;size;p++,size--) 
		if (p->cnt == cnt) {
			Code->code =p->code;
			Code++;
		}
	Code->code = END;
	size = rad[orderno].size;

	buf = char_buf[no];
	if (no==0) { /* characters pointed by the first radical*/
		for (i=0; i<size;i++,buf++) {
			buf->code = char_tmp[i].code;
			buf->cnt  = char_tmp[i].cnt;
		}
		buf->code = END;
		return;
	}
	p   = char_tmp;


	Code =char_buf[no-1];
	for (i=0; i<size;i++){
		for (;Code->code!=END; Code++){
			if (char_tmp[i].code == Code->code) {
				buf->code = char_tmp[i].code;
				buf->cnt = char_tmp[i].cnt;
				buf++;
				continue;
			}
			if (char_tmp[i].code > Code->code) continue;
			if (char_tmp[i].code < Code->code) break;
		}
		if (Code->code == END) break;
	}
	buf->code = END;
	return;
}

/****************************************
 *  Load part of Radical-to-Char file	*
 *  to RAM.				*
 ****************************************/

LoadRadical(file,array)
int	file;
Radical	array[];
{
	int	i,size;
	Hanzi	tmp[2];

	for (i = 0; i < MAX_RAD; i++) {
		size = read(file,&array[i].code,2);
		if (size <2) break; 
		size = read(file,&array[i].size,2);
		if (size <2) break; 
		lseek(file,array[i].size * 3,1);
	}
}

