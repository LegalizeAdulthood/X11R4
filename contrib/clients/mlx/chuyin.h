

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

/* Filename: chuyin.h  15 Nov. 1989,	By Hu Xian Xiang */

#include	<stdio.h>
#include	"cacisdef.h"

char		radical_f[] = "chuyin.tbl";
char		char_f[]    = "char.tbl";

int		radical_p;	/* file ID */

Radical		rad[MAX_RAD];

Char		buf0[1200],buf1[300], buf2[103], buf3[36],
		char_tmp[1200], *char_buf[4];	/* Point to 4 char_buffer */

Hanzi		rad_buf[MAX_RAD];

key[MAX_RAD]	= {    '1','2','3','4',    
		       '5','6','7','8',    
		   '9','0','-','q','w','e',
		       'r','t','y','u',    
		   'i','o','p','a','s','d',
		       'f','g','h','j',    
		       'k','l',';','z',    
		       'x','c','v','b','n'
		};

