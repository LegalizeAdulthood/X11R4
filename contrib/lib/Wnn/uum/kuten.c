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
**  句点入力 の 為の プログラム 
**   programs for KUTEN NYUURYOKU
*/


#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define MAX_TEN_LENGTH  94

int 
in_kuten()		/*  returns code for a moji  */
{
	char buffer[6];
	int code;
	int c_p = 0; /* 何文字入力されたかを示す。*/
	int c , k;
	int ret1 , ret2;
	int mode = -1;
	 /* '.'が入力されてなかったら-1。されていたら何桁めに
	    入っているかを保持する。*/
	extern int t_quit(); /* added by Nide !! */
	extern int henkan_off();
	int not_redrawtmp = not_redraw;
	not_redraw = 1;	/* リドローしない */

	for(k=0;k < 6;buffer[k++] = ' ');

    completely_start:
	throw_c(0);	/* モード表示の後に出す */
	printf("区点 : ");
	clr_line();	/* 62/06/22 kuri */
	fflush(stdout);
    start:
	while(1){

	    throw_c(7);
	    h_r_on();
	    for(k = 0 ; k < 5 ; k++){
		putchar(buffer[k]);
	    }
	    h_r_off();
	    clr_line();
	    fflush(stdout);

	    c = romkan_next();
	    if(c < 256){
		if ((c == ESC) || (t_quit == main_table[5][c])){
	    	    throw_c(0);
		    clr_line();
		    fflush(stdout);
		    not_redraw = not_redrawtmp;
		    return(-1);
		} else if (henkan_off == main_table[5][c]) { /*きたない!*/
		  henkan_off();
		}
	    }
	    if((mode >= 0 ? (c_p - mode) <= 2 : c_p < 4)
			 && c <= '9' && c >= '0'){
		buffer[c_p++] = (char)c;
	    }if((mode == -1) &&(c ==(int)'.') && 0 < c_p && c_p <= 2){
		mode = c_p;
		buffer[c_p++] = (char)c;
	    }else if(c == rubout_code && c_p){
		if(buffer[c_p - 1] == '.')
		  mode = -1;
		buffer[--c_p] = ' ';
	    }else if(c == rubout_code && c_p == 0){
		throw_c(0);
		clr_line();
		fflush(stdout);
		not_redraw = not_redrawtmp;
		return(-1);
	    }else if( ((c == NEWLINE) || (c == CR))  && (c_p >= 3)
				&& (buffer[c_p - 1] != '.')){
		if((!(sscanf(buffer , "%d.%d" , &ret1 , &ret2 ) == 2) && 
		    !((c_p == 4) && 
			sscanf(buffer , "%2d%2d",&ret1 , &ret2) == 2) &&
		    !((c_p == 3) && 
			sscanf(buffer , "%1d%2d",&ret1 , &ret2) == 2) )
		   || ret1 <= 0 || ret1 > 94 || ret2 <= 0 || ret2 > 94){
		    ring_bell();	/* ADD KURI */
		    goto start;
		}
		code = (((ret1 + 32) | 0x80) << 8) |((ret2 + 32) | 0x80);
		break;
	    }else if((c == NEWLINE || c == CR) && c_p <= 2 && mode == -1){
		if(sscanf(buffer , "%d" , &ret1 ) != 1 ||
		   (ret1 <= 0 || ret1 > 94 )){
		    goto start;
		}
		if((code = get_ichiran(ret1)) == -1){
		    goto completely_start;
		} else {
		    break;
		}
	    }
	}
	throw_c(0);
	clr_line();
	not_redraw = not_redrawtmp;
	return(code);
    }

/** 候補一覧を表示してその中から選択する。*/
static int 
get_ichiran(ku)
int ku; /* 区 */
{
  char buf[1024];
  char *buf_ptr[MAX_TEN_LENGTH]; 
  int i;
  char *c;

  for(i = 0,c = buf ; i < MAX_TEN_LENGTH; i++){
    buf_ptr[i] = c;
    *c++ = (ku + 32) | 0x80;
    *c++ = ((i + 1) + 32) | 0x80;
    *c++ = 0;
  }
  if((i = select_one_element(buf_ptr,MAX_TEN_LENGTH,0,"",0,0,main_table[4]))
      == -1){
    return(-1);
  }
  return((((ku + 32) | 0x80) << 8) | ((i + 1) + 32) | 0x80);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
