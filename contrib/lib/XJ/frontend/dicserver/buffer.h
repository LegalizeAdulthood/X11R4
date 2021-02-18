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
 *	Original Source: ./lib/Wnn/uum/buffer.h
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

/* bufferの表示のされ方は、hantenによって決定される。
反転するか、下線状態にするかを各独立に、
バッファーの内で、文頭からmarkまで、markからcursorまで、
cursorからbufferの最後までの状態を独立に設定することができる。
	 when mark is set
        	hanten bit0,bit1:      --- mark
		bit2,bit3:        mark --- cursor
		bit4,bit5:      cursor --- 
	  when mark is not set(-1)
		bit4,bit5:      
    first one....hanten
    second one...under_line
*/

struct buf{
  wchar  *buffer;	/* 登録用バッファ */
  char *buf_msg; /** 仮名を入力する時のバッファーの前に表示する文字列 */
  int start_col; /** 仮名入力時の画面上のコラムのスタート位置 */
  int t_c_p; /** 仮名入力用バッファー上でのカーソル位置 */
  int hanten; /** 仮名入力用バッファーを反転表示させているかどうか */
  int maxlen; /**仮名入力用バッファーの中にはいっている文字数 */
  int buflen; /** length of the buffer */
  int t_m_start;  	/* -1 : markが設定されていない。*/
			/* 0 以上 : マークの位置*/
  int t_b_st;	/**ボールド表示の開始位置 */ /* add by KUWA */
  int t_b_end;	/**ボールド表示の終了位置 */ /* add by KUWA */
  int vlen;    /*バッファー中でスクリーンに表示される幅*/
  int duplicate;/* スクリーンがスクロールする時に、重複して表示する文字数*/

  int vst;    /* bufferの中で画面に表示されている最初の位置*/
		/*これは、勝手に上のルーチンでいじらない方が望ましい。*/
  int (*key_in_fun)(); /*ここに関数が設定されていると、key_tableにバインドが
  ない文字の入力を
  受けた時にバッファーにその文字を入れずに代わりにその関数が呼ばれる*/
  int (*redraw_fun)();/* romkan kara redraw ga kaette kitatoki */
  int (*ctrl_code_fun)();/*ここに関数が設定されていると、key_tableにバインドが
  ないコントロール文字の入力を受けた時にベルを鳴らす代わりに
  その関数が呼ばれる*/
  int (**key_table)();	/*キーバインドのテーブル*/
  char *rk_clear_tbl;	/* romakn_clear が hituyouka */
};

extern int *bunsetsu;
extern int *bunsetsuend;
extern int *touroku_bnst;
/* extern int bunsetsucnt; */
extern int touroku_bnst_cnt;

extern struct buf *c_b; /* 現在用いられているbufを示すグローバル変数*/
extern w_char *kill_buffer; /* キルバッファー大きさは、maxchgとなっている*/
extern int kill_buffer_offset;/* キルバッファー使われている長さ*/
/* ADD KURI */
extern w_char *remember_buf; /* リメンバァバッファ大きさは、maxchgとなっている*/
