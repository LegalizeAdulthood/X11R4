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

#include "commonhd.h"
#include <stdio.h>

int 
sStrcpy(c , w)
register char *c;
register w_char *w;
{
    char *c0 = c;
    for(;*w != 0 ; w++){
	if(*w & 0xff00)
	  *c++ = (*w & 0xff00) >> 8;
	else if (*w & 0x80)
	  *c++ = 0x8e;
	*c++ = *w & 0x00ff;
    }
    *c++ = 0;
    return (c - c0);
}


Sstrcpy(w , c )
w_char *w;
unsigned char *c;
{
    w_char *w0 = w;

    for(;*c;){
	if(*c & 0x80){
	    if (*c == 0x8e)
		*c = 0;
	    *w++ = (int)*c << 8 | *(c + 1);
	    c += 2;
	}else{
	    *w++ = *c;
	    c += 1;
	}
    }
    *w = 0;
    return(w - w0);
}


Sreverse(d, s)
w_char *d, *s;
{
  w_char *s0;

  s0 = s;
  for(;*s++;);
  s--;
  for(;--s >= s0;){
    *d++ = *s;
  }
  *d = 0;
}

char *
Stos(c)
w_char *c;
{
    char *c1 = (char *)c;
    for(;*c;c++){
	if(ASCIIP(*c)){
	    *c1++ = *c;
	}else{
	    *c1++ = (*c << 8);
	    *c1++ = *c;
	}	
    }
    return((char *)c);
}

char *sStrncpy(s1,s2,n)
register char *s1;
register w_char *s2;
register int n;
{
	register char *d;

	for (d = s1;n > 0;n--,s2++) {
	    if(*s2 & 0xff00)
		*d++ = (*s2 & 0xff00) >> 8;
	    *d++ = *s2 & 0x00ff;
	}
	return s1;
}
