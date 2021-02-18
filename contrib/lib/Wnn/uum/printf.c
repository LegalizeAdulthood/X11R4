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
#include "jllib.h"

extern int cursor_colum;
extern int (*code_trans[])();

fprintf(file , x2 , x3 , x4 , x5 , x6 ,x7 , x8 , x9 , x10 , x11 , x12 , x13)
char *x2;
FILE *file;
{
    char buf2[512];

    sprintf(buf2 , x2, x3 , x4 , x5 , x6 ,x7 , x8 , x9, x10 , x11 , x12 , x13);
    cursor_colum += eu_columlen(buf2);
    
    puteustring(buf2, file); 
}

puteustring(buf2, file)
     char *buf2;
     FILE *file;
{
  char *c;
  char buf[512];

  (*code_trans[(J_EUJIS<<2) | tty_c_flag])(buf , buf2 , strlen(buf2) + 1);
  for(c = buf ; *c ; c++){
    putc(*c , file);
  }
}

printf(format , x3 , x4 , x5 , x6 ,x7 , x8 , x9, x10 , x11 , x12 , x13)
char *format;
{
    fprintf(stdout ,format, x3 , x4 , x5 , x6 ,x7 , x8 , x9, x10 , x11 , x12 , x13);

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
    register char *c;
    register int len;
    
    static char buf[W_BUFLEN * 8];
    len=(*code_trans[(J_IUJIS<<2)|tty_c_flag])(buf , w_buf , sizeof(w_char) * w_maxbuf);
    for(c = buf ; len > 0 ; len --,c++){
	    putchar(*c);
    }
    w_maxbuf = 0;
    flush();
}

extern char *wnn_perror();
errorkeyin()
{
    push_cursor();
    throw_c(0);
    clr_line();
    printf(wnn_perror());
    printf(" (Ç¡²¿?)");
    flush();
    romkan_next();
    pop_cursor();
}


print_out(x2 , x3 , x4 , x5 , x6 ,x7 , x8 , x9 , x10 , x11 , x12 , x13)
char *x2;
{
  fprintf(stderr , x2 , x3 , x4 , x5 , x6 ,x7 , x8 , x9 , x10 , x11 , x12 , x13);
  fprintf(stderr, "\r\n");
  fflush(stderr);
}
