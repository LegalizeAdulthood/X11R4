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
 *	Original Source: ./lib/Wnn/uum/touroku.c
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
/* 登録のためのルーチン */

#include <stdio.h>
#include "xwnmo.h"
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "rk_spclval.h"
#include "buffer.h"
#include "jslib.h"

static int touroku_start,touroku_end;
/* these are used only for making yomi string for registered kanji */


static char *dic_choice = "    辞書    ";
static w_char msg_st[6] = {' ',' ',
	(((unsigned char)'\306')<< 8)|(unsigned char)'\311',
	(((unsigned char)'\244')<< 8)|(unsigned char)'\337',':', 0};

/* 登録のための外部関数*/
extern
touroku()
{
    wchar buffer[1024];
    int start;
    struct buf *c_btmp , c_b1;

    c_btmp = c_b;
    c_b = &c_b1;

    c_b->buffer = buffer;
    c_b->buflen = 1024;

    make_touroku_buffer(0, c_btmp);   
    xw_touroku(touroku_bnst[cur_bnst_]);
    c_b = c_btmp;
    return(0);
}

make_touroku_buffer(bnst, cbup)
int bnst;
struct buf *cbup;
{
  int k , l;
  wchar *cp;
  wchar *bp;
  wchar	*buffer_end; /** 仮名入力用のバッファーの最後 */

  buffer_end = c_b->buffer + c_b->buflen - 1;
  bp = c_b->buffer;
  for(k = bnst ; k < jl_bun_suu(bun_data_) ; k++){
      if (k < maxbunsetsu) touroku_bnst[k] = bp - c_b->buffer;
      l = jl_get_kanji(bun_data_, k, k+1, bp);
      bp += l;
      if (bp >= buffer_end) {
	  k--;
	  bp -= l;
	  goto GOT_IT;
      }
  }
  if (cbup->maxlen == 0) {	/* 確定後の登録 ADD KURI */
	/* ヒストリから持ってくる。*/
	get_end_of_history(bp);
  } else {
        /* 途中で解除したかな文字列 */
	Strncpy(bp, cbup->buffer + bunsetsu[jl_bun_suu(bun_data_)],
		cbup->maxlen - bunsetsu[jl_bun_suu(bun_data_)]);
	*(bp + cbup->maxlen - bunsetsu[jl_bun_suu(bun_data_)]) = 0;
  }

 GOT_IT:
  if(k < maxbunsetsu){
      touroku_bnst[k++] = bp - c_b->buffer;
      touroku_bnst_cnt = k;
  }else{
      touroku_bnst[maxbunsetsu - 1] = bp - c_b->buffer;
      touroku_bnst_cnt = maxbunsetsu;
  }
}

