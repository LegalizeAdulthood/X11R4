

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

/* Filename: xtermtest.c  15 Nov. 1989,	By Hu Xian Xiang */

/***************************************
*   FUNCTION: Testing if multilingual
*   character code is readable/printable
*   diretly from/to multilingual XTERM
*   environment.
****************************************/

#include	<stdio.h>

main()
{
	int	i;
	char	char1,buf[256];

	printf("\nPLEASE INPUT A MULTILINGUAL STRING: ");
	for (;1;){
		char1 = getchar();
		if (char1=='\n') break;
		fprintf(stderr,"[%x] ",char1); 
		buf[i++] = char1;
	}
	buf[i] = 0;
	printf("\nThis massage was received by using getchar():\n%s\n",
		buf);
	buf[i++] = char1;
	buf[i] = 0;
	fprintf(stderr,"\nThe following characters are printed by calling putchar().\n  ");
	for(i=0; char1=buf[i++]; )
		putchar(char1);
}
