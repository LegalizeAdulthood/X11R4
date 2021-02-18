

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

/* Filename: looktbl.h  15 Nov. 1989,	By Hu Xian Xiang */

#include	"defines.h"

short	int  lmax[7][3]={{4,8,12},{3,6,9},{2,5,7},{2,4,6},
			 {3,6,9},{2,5,7},{3,6,9}};
short	int 	modu[]={11,27,37,95,14,48,28,38};
	 /* 18 is WBZX , 38 is CACIS, and 28 is Japanese*/
char	HiF_name[2][7]={
			{ 0xb3,0xa3,0xd3,0xc3,0xd7,0xd6,0},
			{ 0xbe,0xef,0xcd,0xd2,0xbb,0xfa,0}
       };
unsigned	int	next;

int 	TabNum,wordflag;
char	ext_string[8],hzcode[32],save[32],hzbuf[1200];
int 	n=24,m=0,hcc;
int     Hifreq_flg=0;
char	modeflag;
char    space8[]="             " ;
char    spaces[]="                                                                                                              " ; /* 110 */
extern	*Inmethod[];
