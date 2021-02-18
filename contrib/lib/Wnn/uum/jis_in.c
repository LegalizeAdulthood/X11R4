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
/*
 *	JIS 入力のためのプログラム
 *	programs for JIS NYUURYOKU
 */


#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define FROM_JIS 	0x21
#define TO_JIS		0x7E

int 
in_jis()		/*  returns code for a moji  */
{
	char buffer[5];
	int code;
	int c_p = 0; /* 何文字はいったかを示す */
	int c , k;
	extern int	t_quit();   /* added by Nide !! */
	extern int henkan_off();
	int not_redrawtmp = not_redraw;
	not_redraw = 1;	/*リドローしない */
	
	for(k=0;k < 5;k++){
	    buffer[k] = ' ';
	}

    completely_start:
	throw_c(0);	/* モード表示の後に出す */
	printf("JISコード: ");
	clr_line();	/* 62/06/22 kuri */
	fflush(stdout);
    start:
	while(1){

	    throw_c(10);
	    h_r_on();
	    for(k = 0 ; k < 4 ; k++){
		putchar(buffer[k]);
	    }
	    h_r_off();
	    clr_line();
	    fflush(stdout);

	    c = romkan_next();
	    if((c == ESC) || (t_quit == main_table[5][c])){
		throw_c(0);
		clr_line();
		fflush(stdout);
		not_redraw = not_redrawtmp;
		return(-1);
	      } else if (henkan_off == main_table[5][c]){ /*きたない!*/
		henkan_off();
	    }else if(c_p < 4 && 
		     ((c <= '9' && c >= '0') || 
		      (c <= 'F' && c >= 'A') || 
		      (c <= 'f' && c >= 'a'))) {
		if ((c_p == 0 || c_p == 2) ?
			(c <= 'F' && c >= 'A') || /* For EUC */
			(c <= 'f' && c >= 'a') || /* For EUC */
			(c <= '7' && c >= '2') :
			!((buffer[c_p-1]=='2' || 
			   buffer[c_p-1]=='A' || buffer[c_p-1]=='a')
			  && c == '0' ||
			  (buffer[c_p-1]=='7' ||
			   buffer[c_p-1]=='F' || buffer[c_p-1]=='f')
			  && (c == 'F' || c == 'f'))){
			buffer[c_p++] = (char)c;
		    } else {
			ring_bell();	/* ADD KURI */
		    }
	    }else if(c == rubout_code && c_p){
		buffer[--c_p] = ' ';
	    }else if(c == rubout_code && c_p == 0){
		throw_c(0);
		clr_line();
		fflush(stdout);
		not_redraw = not_redrawtmp;
		return(-1);
	    }else if( c == NEWLINE || c == CR ){
		if (c_p <= 1){
		    goto start;
		} else if (c_p == 4){
		    code = 0;
		    for(k = 0; k < 4; k++){
			code = code * 16 + hextodec(buffer[k]);
		    }
		    code |= 0x8080; /* ujis にして返す */
		    break;
		}
		if((code = get_jis_ichiran(buffer, c_p)) == -1){
		    goto completely_start;
		}
		break;
	    } else {
		ring_bell();	/* ADD KURI */
	    }
	}
	throw_c(0);
	clr_line();
	not_redraw = not_redrawtmp;
	return(code);
}

hextodec(c)
char c;
{
    if (c >= '0' && c <= '9'){
	return(c - '0');
    } else if (c >= 'A' && c <= 'F'){
	return(c - 'A' + 10);
    } else if (c >= 'a' && c <= 'f'){
	return(c - 'a' + 10);
    } else
	return(-1);
}


static int
get_jis_ichiran(st, num)
char *st;
int num;
{
  int from;
  int i;
  char *c;
  int maxh;
  int code;
  char buf[512];
  char *buf_ptr[96]; 

  if (num == 2){
    from = hextodec(st[0]) << (8 + 4) | hextodec(st[1]) << 8 | 0x21 ;
    from |= 0x8080;
    maxh = 94;
  }else{
    from = hextodec(st[0]) << (8 + 4) | hextodec(st[1]) << 8 |
    	   hextodec(st[2]) << 4;
    from |= 0x8080;
    maxh = 16;
    if ((from & 0xf0) == 0xf0) {
	maxh--;
    } else if ((from & 0xf0) == 0xa0) {
	maxh--;
	from++;
    }
  }

  for(i = 0,c = buf ; i < maxh; i++){
    code = from + i;
    buf_ptr[i] = c;
    *c++ = (code & 0xff00 ) >> 8;
    *c++ = (code & 0x00ff );
    *c++ = 0;
  }
  if((i = select_one_element(buf_ptr,maxh,0,"",0,0,main_table[4])) == -1){
    return(-1);
  }
  return(from + i);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
