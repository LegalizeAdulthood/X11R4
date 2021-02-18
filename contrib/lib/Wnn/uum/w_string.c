/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/*	Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#define CONTLINE '$'


w_printf(x , len)
w_char *x;
int len;
{
	for(;*x != 0 && len > 0  ; x++ , len--){
	    w_putchar(*x);
	}
	flushw_buf();
	if(len <= 0) putchar(CONTLINE);
}

chartowchar(c , w)
w_char *w;
UCHAR *c;
{
    Sstrcpy(w, c);
}

wchartochar(w , c)
w_char *w;
UCHAR *c;
{
    int flg = 0;
    for(;*w != 0 ; w++){
	if((*w) > 256){
	    *c++ = (UCHAR)(*w & 0xff00) >> 8;
	    *c++ = (UCHAR)*w & 0x00ff;
	    flg = 1;
	}
	*c++ = *w & 0x00ff;
    }
    *c++ = 0;
    *c++ = 0;
    return(flg);
}


w_sttost(w, c)
w_char *w;
char *c;
{
    sStrcpy(c , w);
}

w_sttostn(w, c, n)
w_char *w;
char *c;
int n;
{
    sStrncpy(c, w, n);
}

eu_columlen(c)
unsigned char *c;
{
  int len = 0;
  w_char tmp;
  
  for(;*c;){
    if((*c & 0x80)){
      tmp = ((w_char)*c++) << 8;
      tmp |= *c++;
    }else{
      tmp = *c++;
    }
    if(hankaku(tmp)){
      len += 1;
    }else{
      len += 2;
    }
  }
  return(len);
}

#ifdef	not_use
#ifndef	putwchar
putwchar(x)
{
    putchar( x >> 8);
    putchar( x );
}
#endif

#ifndef	putws
putws(s)
unsigned short *s;
{
    while(*s) putwchar(*s++);
}
#endif
#endif
