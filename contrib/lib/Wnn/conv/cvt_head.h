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
			cvt_head.h			 ＿＿＿
						87/11/24｜改正｜
							 ￣￣￣
	convert_key.c 及び convert_read.c で使われる
	へッダファイル。テーブルのサイズの定義など。
***********************************************************************/

/*	Version 4.0
 */

#if defined(SUN) || defined(SX9100BSD)
# ifndef	BSD42
#  define	BSD42
# endif
#endif

#ifdef SX8870
# ifndef	SYSVR2
#  define	SYSVR2
# endif
#endif

#define BITSIZ (sizeof(int) * 8)
#define CHANGE_MAX 2 /* 変換テーブルの最大個数÷BITSIZを下回らない整数 */
#define AREASIZE 1024

#ifdef WNNDEFAULT
#  include "config.h"
 /* マクロCONVERT_FILENAMEの定義（のためだけ）。コンパイル時は、ヘッダファイル
    のサーチパスに、Wnnのインクルードファイルのありかを設定しておくこと。*/
#else
#  define CONVERT_FILENAME "cvt_key_tbl"
#endif

#define div_up(a, b) ((a + b - 1) / b)
 /* a,bは非負整数。a/bを切り上げて整数にする */

struct	CONVCODE {
	int	tokey;		/* 変換されたコード */
	char	*fromkey;	/* 変換するコード */
};

/*
	make時に必要なdefine

	BSD42		BSD/termcapを使用
	SYSVR2		System V/terminfoを使用
	SX8870		terminfoのエントリkey_f11〜31を使用。
			この時SYSVR2も自動的にdefineされる。
	WNNDEFAULT	上記参照

	この他 デバッグ時は必要に応じDEBUGをdefine
*/
