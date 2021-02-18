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
			rk_header.h
						88. 5.20  訂 補

	rk_main.c rk_read.c rk_modread.c rk_bltinfn.cの共通ヘッダ。
	中で取っている配列の大きさなどを定義。
***********************************************************************/

/*	Version 3.0
 */

/*	make時に必要なdefine

	BSD42		BSDにてstrings.hを使用（string.hを使う場合は不要）
	SYSVR2		System Vにて定義域の制限されたtoupper・tolowerを使用
			（なくても動く）
	MVUX		ECLIPSE MVでの運転時にdefine  IKISが自動defineされる

	RKMODPATH="文字列"
			その文字列をモード定義表のサーチパスの環境変数の
			名前にする
	WNNDEFAULT	「@LIBDIR」で標準設定表のあるディレクトリを表せる
			ようにする	
	IKIS		半角仮名の１バイト目を0xA8（デフォルトは0x8E）にする

	この他 デバッグ時は必要に応じて KDSP、CHMDSPをdefine
*/

#ifdef MVUX
#  define IKIS
#endif

#include <stdio.h>
#ifdef BSD42
#  include <strings.h>
#  define strchr  index
#  define strrchr rindex
#else
#  include <string.h>
#endif
#include "rk_macros.h"

#define ESCCHR	'\033'
#define BASEMX	(26 + 10)

#define REALFN	200 /* 表のフルネームの最大長 */

#define LINALL	1600 /* 対応表全部の行数合計 */
#define SIZALL	10000 /* 対応表の内部表現の最大サイズ・
			表一つの変数の変域長の合計としても使ってる */
#define LINSIZ	1000 /* 対応表の一行の最大サイズ */
#define TRMSIZ	500 /* 対応表の一項目の最大サイズ・
			モード名の最長としても使ってる */
#define KBFSIZ	100 /* 本処理バッファのサイズ */
#define DSPLIN	256 /* デバッグ用 */
#define OUTSIZ	200 /* 出力バッファのサイズ */
#define RSLMAX	20 /* 関数の値として返る文字列の最長 */

#define VARMAX	50 /* 表一個の変数個数 */
#define VARTOT	150 /* 全表の変数個数計 */
#define VARLEN	500 /* 変数名の長さの計 */

#define FILNST	20

	/* rk_modread.cで使うdefine */

#define HYOMAX	40 /* 変換対応表の最大個数 */
#define HYOMEI	500 /* 表名の文字数計 */
#define PTHMAX	30 /* サーチパスの最大個数 */
#define PTHMEI	800 /* サーチパス名の文字数計 */
#define MODMAX	50 /* モードの種類数 */
#define MODMEI	300 /* モードの全文字数 */
#define DMDMAX	40 /* モード表示の種類数 */
#define DMDCHR	250 /* モード表示の全文字数 */
#define MDHMAX	2500 /* モード表の最大サイズ */
  /* モード表の最初のlistscanの時は、エラーを考慮して、リスト1個のbufferに
	表のサイズ分取っておく。*/
#define MDT1LN	200 /* モード設定リスト1個の最大長 */
#define NAIBMX	400 /* モード定義表の内部表現の最大サイズ */
			/* Change KURI 200 --> 400 */

#define ARGMAX	2 /* 条件判断関数の引数の最大個数 */

 /* ディレクトリ名の区切りのdefine（UNIX用）。UNIX以外の環境で使うには
    これと、fixednamep()も変更の必要がある（readmode()のgetenv関係も勿論）。*/
#define KUGIRI '/'

 /* エラー処理用 */
#include <setjmp.h>
