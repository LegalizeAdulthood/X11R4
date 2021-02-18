/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OMRON not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  OMRON makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Original Source: ./lib/Wnn/uum/printf.c
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

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
	xwnmo
 */
#include <stdio.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "jllib.h"

#undef	putchar

putchar(x)
unsigned char	x;
{
	if((x & 0x80) != 0) {
		if(c_c->two_byte_first == 1) {
			c_c->wc += x;
#ifndef	X11
			TTYMflushw_buf(&c_c->wc, 1);
#else	X11
			JWMflushw_buf(&c_c->wc, 1);
#endif	!X11
			c_c->two_byte_first = 0;
		} else {
			c_c->two_byte_first = 1;
			c_c->wc = x * 0x100;
		}
	} else {
		c_c->wc = x;
		c_c->two_byte_first = 0;
#ifndef	X11
		TTYMflushw_buf(&c_c->wc, 1);
#else	X11
		JWMflushw_buf(&c_c->wc, 1);
#endif	!X11
	}
}


printf(format , x3 , x4 , x5 , x6 ,x7 , x8 , x9, x10 , x11 , x12 , x13)
char *format;
{
    char buf2[256];
    register char *c;

    sprintf(buf2 , format, x3 , x4 , x5 , x6 ,x7 , x8 , x9, x10 , x11 , x12 , x13);
    cursor_colum += eu_columlen(buf2);

    for(c = buf2 ; *c ; c++){
	putchar(*c);
    }

}

#define W_BUFLEN 32
static w_char w_buf[W_BUFLEN];
static int w_maxbuf = 0;

w_putchar(wch)
w_char wch;
{
    cursor_colum += char_len(wch);
    if(ESCAPE_CHAR(wch)){
	w_buf[w_maxbuf++] =(w_char)('^');
	if(wch == 0x7f)w_buf[w_maxbuf++] =(w_char)('?');
	else w_buf[w_maxbuf++] =(w_char)(wch + 'A' - 1);
    }else{
	w_buf[w_maxbuf++] = wch;
    }
    if(w_maxbuf >= W_BUFLEN - 2){
	flushw_buf();
    }
}

putchar_norm(c)
{
push_hrus();
putchar1(c);
pop_hrus();
}

putchar1(c)
int c;
{
  putchar(c);
  flush();
  cursor_colum += 1;
}

flushw_buf()
{
    register w_char *c;
    
#ifndef	X11
    TTYMflushw_buf(w_buf, w_maxbuf);
#else	X11
    JWMflushw_buf(w_buf, w_maxbuf);
#endif	X11
    w_maxbuf = 0;
}

extern char *wnn_perror();
errorkeyin()
{
    print_msg_getc("%s", wnn_perror());
}


print_out(x2 , x3 , x4 , x5 , x6 ,x7 , x8 , x9 , x10 , x11 , x12 , x13)
char *x2;
{
  fprintf(stderr , x2 , x3 , x4 , x5 , x6 ,x7 , x8 , x9 , x10 , x11 , x12 , x13);
  fprintf(stderr, "\r\n");
  fflush(stderr);
}
/*************************************/

#ifdef	WNMO
#ifdef	nodef
print_msg(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
register char	*format;
{
	char *buf[256];
	sprintf(buf, format, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
/*
	tty_err_print(buf);
*/
	init_yes_or_no(buf, 3);
}
#endif	nodef

print_msg_getc(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
register char	*format;
{
	register char c;
	char *buf[256];
	XJClient *xjc_sv;

	xjc_sv = cur_x;
	sprintf(buf, format, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	init_yes_or_no(buf, 2);
	c = keyin0();
	end_yes_or_no();
	cur_cl_change3(xjc_sv);
	return(c);
}
#endif	WNMO
