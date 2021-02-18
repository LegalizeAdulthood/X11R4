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
/***********************************************************************
			convert_key.c			 ＿＿＿
						87/12/19｜訂補｜
							 ￣￣￣
	タイムアウト処理を伴う、ファンクションキーコンバート処理。
***********************************************************************/

/*	Version 4.0
 */

#include "cvt_head.h"
#include <signal.h>
#include <setjmp.h>

/*
	keyin1()を呼び出すためには、次の手続きが必要です。

     １ tgetent()（4.2BSD）もしくはsetupterm()（System V）によって、
	キーボードのterm情報が入っているエントリを読み込みます。
     ２ convert_getstrs() によって、読んだ情報を専用のエリアにコピーします。
	これによって、他のエントリのterm情報を読み込んでも、１で得た情報が
	失われなくなります。
     ３ convert_key_setup()を呼び、変換の初期設定をします。

	convert_getterm()によって、１と２が一度にできます。
	また、convert_key_init()によって２と３が、
	convert_getterm_init()によって１から３までが、それぞれ一度にできます。

	なお、これらの convert_ で始まるライブラリ関数は、convert_read.c にて
	定義されています。

	4.2BSDの場合に旧版で呼ぶ必要のあったgetterm_fkeydata()は、
	不必要になったため廃止されました。
*/

static	jmp_buf env;

extern	struct	CONVCODE tbl[];
extern	int	cnv_tbl_cnt;	/* convert table count */

 /** コード変換を伴うキー入力関数 */
keyin1(get_ch)
int	(*get_ch)();	/* getchar() と同様の関数 */
{
	int ret;

	for ( ; ; ) {
	    if(cnv_tbl_cnt == 0) {
		ret = (*get_ch)();
		if (ret >= 0) 
		    return(ret);
	    } else {
		return(convert_key(get_ch, tbl, cnv_tbl_cnt, 0));
	    }
	}
}

 /** intの配列 h をビットベクタとみなし、第iビットをチェックあるいは立てる */
#define BITONP(h, i) (h[i / BITSIZ] &  (1 << (i % BITSIZ)))
#define BITOFP(h, i) (!BITONP(h, i))
#define BIT_UP(h, i) (h[i / BITSIZ] |= (1 << (i % BITSIZ)))
#define BITDWN(h, i) (h[i / BITSIZ] &= ~(1 << (i % BITSIZ)))


 /** 指定された変換テーブルに従ってコード変換する。*/
convert_key(inkey, conv_tbl, tbl_cnt, matching_flg)
int	(*inkey)();		/* キー入力関数 */
struct	CONVCODE conv_tbl[];	/* 変換テーブル */
int	tbl_cnt;		/* conv_tbl[] の個数 */
int	matching_flg;		/* マッチングしなかったストリングの処理指定
					0 : 返値として返す
					1 : そのストリングは捨てる	*/
{
#define MAX	20		/* キー入力バッファの最大値 */

	static	int	inbuf[MAX];	/* キー入力バッファ */
		/* バッファの終端は、-1 で示される。*/

	int	out_cnt;		/* 出力バッファの出力カウント */

	static	int	buf_cnt = 0;	/* inbuf の入力時のカウンタ   */

	int	check_flg[CHANGE_MAX];
	 /* ビットベクタとして扱われ、マッチング時に対象となっているconv_tbl[]
	    を示す。1の時対象となり、0で非対象 */

	int	i, c, flg = 0;		/* work */

	for(i = 0; i < div_up(tbl_cnt, BITSIZ); check_flg[i++] = ~0);
     /* 配列check_flgをビットベクタ扱いし、その第0〜tbl_cnt ビットを立てる。
	但し、実際はその少し先まで立つ */

	for(;;){
	    if(flg != 0 || buf_cnt == 0){
		if(buf_cnt > 0){
		    if ((inbuf[buf_cnt] = (*inkey)()) == -1) { /* 一文字入力 */
			c = -2;	/* タイムアウト */
			goto LABEL;
		    } else {
			inbuf[++buf_cnt] = -1; /* ターミネータ */
		    }
		} else {
		    if ((inbuf[buf_cnt] = (*inkey)()) == -1) { /* 一文字入力 */
			continue;
		    } else {
			inbuf[++buf_cnt] = -1; /* ターミネータ */
		    }
		}
	    }
	    flg++;

	    if(buf_cnt >= MAX - 1) return(-1);	/* ERROR */

	    c = key_check(inbuf, conv_tbl, tbl_cnt, check_flg);
LABEL:
	    switch(c){
		    case -1:	/* 未決定 */
			    continue;

		    case -2:	/* 変換対象でないことが決定した */
			    buf_cnt--;
			    out_cnt = 0;
			    c = inbuf[out_cnt++];
			    for(i = 0; inbuf[i] != -1;
				inbuf[i++] = inbuf[out_cnt++]);
			    if(matching_flg != 0){
				    flg = 0;
				    continue;
			    }
			    return(c);

		    default:	/* 変換されたコード */
			    buf_cnt = 0;
			    return(c);
	    }
	}
}

 /** 変換コードのチェックとコード変換 */
key_check(inbuf, conv_tbl, tbl_cnt, check_flg)
int	inbuf[];		/* ソースストリング */
struct	CONVCODE conv_tbl[];	/* コード変換テーブル */
int	tbl_cnt;
int	check_flg[];
{
	register int	dist, base;
	register char	*code_p;
	int	i;

	for(base = 0; inbuf[base] != -1; base++){
		for(dist = 0; dist < tbl_cnt; dist++){
			if(BITONP(check_flg, dist) &&
			   conv_tbl[dist] . fromkey != 0){
				code_p = conv_tbl[dist] . fromkey + base;
				if(*code_p == (char)inbuf[base]){
					if(*(code_p + 1) == (char)0){
						 /* マッチした */
						for(i = 0, base++;
						    (inbuf[i] = inbuf[base])
							!= -1;
						    i++, base++);
						return(conv_tbl[dist]. tokey);
					}
					/* まだマッチしていない */
				} else {
					BITDWN(check_flg, dist); /* 無効 */
				}
			}
		}
	}

	 /* ビットベクタ check_flg[] の第0〜tblcntビットに立ったまま残っている
	    ものがあるか調べる。*/
	for(i = 0; i < tbl_cnt / BITSIZ; i++){
		if(check_flg[i]) return(-1);
	}
	if((tbl_cnt %= BITSIZ) && (check_flg[i]& ~(~0 << tbl_cnt)))return(-1);
	 /* return -1 … まだ未決定の物がある */

	return(-2);	/* 変換対象となる物はない */
}
