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

static int fmode;
 /* JS_FILE_SEND (server receives) */
fopen_read_cur(fn)
char *fn;
{
	fmode = 'r';
	return 1;
}

/* JS_FILE_RECEIVE (server sends) */
fopen_write_cur(fn)
char *fn;
{
	fmode = 'w';
/*
	puts_cur(fn);putc_purge();
	if(get4_cur()== -1) return NULL;
	else return 1;
*/
	return 1;
}

/* JS_FILE_SEND (server recieves) */
fread_cur(p, size, nitems)
char *p;
register int size,nitems;
{
 register int i,j,xx;
 for(i=0;i<nitems;i++){
     for(j=0;j<size;j++){
	*p++ = xx =  xgetc_cur();
	if(xx== -1) return i;
    }
 }
 return nitems;
}

static int store = -1;

xgetc_cur()
{register int x;
 if(store != -1){ x = store; store = -1; return(x);}
 if((x= getc_cur()) != 0xFF) return x;
 if(getc_cur() == 0xFF) return -1; /* EOF */
 return 0xFF;
}

xungetc_cur(x)			/* H.T. */
int x;
{
    store = x;
}

/* JS_FILE_SRECEIVE (server sends) */
fwrite_cur(p, size, nitems)
unsigned char *p;
{register int i,x;
 x= size*nitems;
 for(i=0;i<x;i++){
	xputc_cur(*p++);
 }
}

xputc_cur(x)
unsigned char x;/* H.T. */
{
 putc_cur(x);
 if(x==0xFF){ putc_cur(0x00); }
}

fclose_cur()
{
 if(fmode!='w') return;
 putc_cur(0xFF);
 putc_cur(0xFF); /* EOF */
}
