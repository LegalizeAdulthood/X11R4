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
 *	Original Source: ./lib/Wnn/uum/kuten.c
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
 */
/*
**  句点入力 の 為の プログラム 
**   programs for KUTEN NYUURYOKU
*/


#include <stdio.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#define MAX_TEN_LENGTH  94

int 
in_kuten()		/*  returns code for a moji  */
{
	static char buffer[5];
	static int code;
	static int c_p = 0; /* 何文字入力されたかを示す。*/
	static int c , k;
	static int ret1 , ret2;
	static int mode = -1;
	 /* '.'が入力されてなかったら-1。されていたら何桁めに
	    入っているかを保持する。*/
	extern int t_quit(); /* added by Nide !! */
	extern int henkan_off();
	static char	*kuten_title = "区点入力";
	static char	*kuten_subtitle = "区点を入力して下さい";
	static char	*kuten_exec = "実行";
	static struct wnmo_client *c_c_sv = 0;
	static int kuten_step = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
    }
    if (c_c_sv == 0) {
	for(k=0;k < 5;buffer[k++] = ' ');
	buffer[5] = NULL;
	c_c_sv = c_c;
	c_p = 0;
	mode = -1;
    }

start:
    if (kuten_step == 0) {
	if (init_ichiran(NULL, 0, -1,kuten_title, kuten_subtitle, kuten_exec,
				5, NYUURYOKU) == -1) {
	    ring_bell();
	    c_c_sv = 0;
	    return(-1);
	}
	JW3Mputc(buffer, jfew->ichi->nyuu_w, 0, 1, 0);
	kuten_step++;
	return(BUFFER_IN_CONT);
    }
    if (kuten_step == 1) {
#ifdef	WNMO
	/*romkan_clear(); Add by kuwari 1/3/19 */
	/*
	c = rk_getc();
	*/
	c = rk_getkey();
#else	WNMO
	c = romkan_next();
#endif	WNMO
	if (select_return == -2) {
	    /*
	    JWMline_clear(0);
	    */
	    end_ichiran();
	    select_return = -1;
	    c_c_sv = 0;
	    kuten_step = 0;
	    /*romkan_clear(); Add by kuwari 1/3/19 */
	    return(-1);
	}
	if(c < 256){
	    if ((c == ESC) || (t_quit == main_table[5][c])){
	    	/*
	    	JWMline_clear(0);
	    	*/
		end_ichiran();
		c_c_sv = 0;
		kuten_step = 0;
		/*romkan_clear(); Add by kuwari 1/3/19 */
		return(-1);
	    } else if (henkan_off == main_table[5][c]) { /*きたない!*/
		ring_bell();
		return(BUFFER_IN_CONT);
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
	    /*
	    JWMline_clear(0);
	    */
	    end_ichiran();
	    c_c_sv = 0;
	    kuten_step = 0;
	    /*romkan_clear(); Add by kuwari 1/3/19 */
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
		    goto RET;
	    }
	    code = (((ret1 + 32) | 0x80) << 8) |((ret2 + 32) | 0x80);
	    /*
	    JWMline_clear(0);
	    */
	    end_ichiran();
	    goto LAST;	
	}else if((c == NEWLINE || c == CR) && c_p <= 2 && mode == -1){
	    if(sscanf(buffer , "%d" , &ret1 ) != 1 ||
		(ret1 <= 0 || ret1 > 94 )){
		goto RET;
	    }
	    /*
	    JWMline_clear(0);
	    */
	    end_ichiran();
	    kuten_step++;
	    goto NEXT;
	}
RET:
	JW3Mputc(buffer, jfew->ichi->nyuu_w, 0, 1, 0);
	return(BUFFER_IN_CONT);
    }
NEXT:
    if (kuten_step == 2) {
	if((code = get_ichiran(ret1)) == -1){
	    kuten_step = 0;
	    goto start;
	} else if (code == BUFFER_IN_CONT) {
	    return(BUFFER_IN_CONT);
	}
    }
LAST:
    c_c_sv = 0;
    kuten_step = 0;
    /*romkan_clear(); Add by kuwari 1/3/19 */
    return(code);
}

/** 候補一覧を表示してその中から選択する。*/
static int 
get_ichiran(ku)
int ku; /* 区 */
{
  static char buf[1024];
  static char *buf_ptr[MAX_TEN_LENGTH]; 
  static int i;
  static char *c;
  static char	kuten_title[32];
  static int kuten_ichi_step = 0;

  if (kuten_ichi_step == 0) {
  for(i = 0,c = buf ; i < MAX_TEN_LENGTH; i++){
    buf_ptr[i] = c;
    *c++ = (ku + 32) | 0x80;
    *c++ = ((i + 1) + 32) | 0x80;
    *c++ = 0;
  }
  sprintf(kuten_title, "区点入力 %2d 区", ku);
  kuten_ichi_step++;
  }
  if((i = xw_select_one_jikouho(buf_ptr,MAX_TEN_LENGTH,0,kuten_title,
				KUTEN_IN, main_table[4])) == -1){
    kuten_ichi_step = 0;
    return(-1);
  } else if (i == BUFFER_IN_CONT) {
    return(BUFFER_IN_CONT);
  }
  kuten_ichi_step = 0;
  return((((ku + 32) | 0x80) << 8) | ((i + 1) + 32) | 0x80);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
