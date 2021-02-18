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
 *	Original Source: ./lib/Wnn/uum/jis_in.c
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
 *	JIS 入力のためのプログラム
 *	programs for JIS NYUURYOKU
 */


#include <stdio.h>
#include <ctype.h>
#include "xwnmo.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"


#define FROM_JIS 	0x21
#define TO_JIS		0x7E

int 
in_jis()		/*  returns code for a moji  */
{
	static char buffer[5];
	static int code;
	static int c_p = 0; /* 何文字はいったかを示す */
	static int c , k;
	extern int	t_quit();   /* added by Nide !! */
	extern int henkan_off();
	static char	*jis_title = "JISコード入力";
	static char	*jis_subtitle = "JISコードを入力して下さい";
	static char	*jis_exec = "実行";
	static struct wnmo_client *c_c_sv = 0;
	static int jis_step = 0;

	if (c_c_sv != 0 && c_c != c_c_sv) {
	    ring_bell();
	    return(-1);
	}
	if (c_c_sv == 0) {
	    for(k=0;k < 5;buffer[k++] = ' ');
	    buffer[5] = NULL;
	    c_c_sv = c_c;
	    c_p = 0;
	}
start:
	if (jis_step == 0) {
	    if (init_ichiran(NULL, 0, -1,jis_title, jis_subtitle, jis_exec,
				5, NYUURYOKU) == -1) {
		ring_bell();
		c_c_sv = 0;
		return(-1);
	    }
	    JW3Mputc(buffer, jfew->ichi->nyuu_w , 0, 1, 0);
	    jis_step++;
	    return(BUFFER_IN_CONT);
	}
	if (jis_step == 1) {
#ifdef	WNMO
	    c = rk_getkey();
#else	WNMO
	    c = romkan_next();
#endif	WNMO
	    if (select_return == -2) {
		end_ichiran();
		select_return = -1;
		c_c_sv = 0;
		jis_step = 0;
		return(-1);
	    }
	    if((c == ESC) || (t_quit == main_table[5][c])){
		end_ichiran();
		c_c_sv = 0;
		jis_step = 0;
		return(-1);
	    } else if (henkan_off == main_table[5][c]){ /*きたない!*/
		ring_bell();
		return(BUFFER_IN_CONT);
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
			goto RET;
		} else {
			ring_bell();	/* ADD KURI */
			return(BUFFER_IN_CONT);
		}
	    }else if(c == rubout_code && c_p){
		buffer[--c_p] = ' ';
		goto RET;
	    }else if(c == rubout_code && c_p == 0){
		end_ichiran();
		c_c_sv = 0;
		jis_step = 0;
		return(-1);
	    }else if( c == NEWLINE || c == CR ){
		if (c_p <= 1){
		    goto RET;
		} else if (c_p == 4){
		    code = 0;
		    for(k = 0; k < 4; k++){
			code = code * 16 + hextodec(buffer[k]);
		    }
		    code |= 0x8080; /* ujis にして返す */
		    end_ichiran();
		    goto LAST;
		}
		end_ichiran();
		jis_step++;
		goto NEXT;
	    } else {
		ring_bell();	/* ADD KURI */
	    }
RET:
	    JW3Mputc(buffer, jfew->ichi->nyuu_w , 0, 1, 0);
	    return(BUFFER_IN_CONT);
	}
NEXT:
	if (jis_step == 2) {
	    if((code = get_jis_ichiran(buffer, c_p)) == -1){
		jis_step = 0;
		goto start;
	    } else if (code == BUFFER_IN_CONT) {
		return(BUFFER_IN_CONT);
	    }
	}
LAST:
	c_c_sv = 0;
	jis_step = 0;
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
  static int from;
  static int i;
  static char *c;
  static int maxh;
  static int code;
  static char buf[1024];
  static char *buf_ptr[96]; 
  static char jis_title[32];
  static int jis_ichi_step = 0;

  if (jis_ichi_step == 0) {
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
  sprintf(jis_title, "JISコード入力 0x%s", st);
  jis_ichi_step++;
  }
  if((i = xw_select_one_jikouho(buf_ptr,maxh,0,jis_title,
				JIS_IN, main_table[4])) == -1){
    jis_ichi_step = 0;
    return(-1);
  } else if (i == BUFFER_IN_CONT) {
    return(BUFFER_IN_CONT);
  }
  jis_ichi_step = 0;
  return(from + i);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
