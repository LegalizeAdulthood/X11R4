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
			rk_vars.c
						88. 6.16  訂 補

	プログラム内で二つ以上のファイルにまたがって使う変数を
	まとめて定義している。
***********************************************************************/

/*	Version 3.0
 */

#include "rk_header.h"

char	rk_errstat = 0; /* (error)によってエラーが引き起こされた時1（但し
			   (eofflg)が立っていたため末尾までの強制出力処理が
			   起こった場合は2）になる。romkan_henkan()実行ごとに
			   更新される。これはユーザにも開放。*/

int	flags = 0;
 /* 以下のフラグのOR。
	RK_CHMOUT モードチェンジを知らせるかのフラグ 
	RK_KEYACK キーインに対し必ず何か返すかのフラグ 
	RK_DSPNIL romkan_disp(off)modeの返値のデフォルトが空文字列であるかの
		  フラグ。立たなければデフォルトはNULL。互換性のためromkan_
		  init2まではこれが立つ。
	RK_NONISE 偽コードを出さないようにするかのフラグ
	RK_REDRAW Wnnで使う特殊なフラグ。これが立っている場合、romkan_henkan
		  の結果としてdisoutにnisedl（偽物のdel）以後が特殊コードのみ
		  で終わる文字列が返ってきたら、その後ろにREDRAWをつなぐ。Wnn
		  で変換行のredrawをするために必要な措置。
	RK_SIMPLD deleteの動作を単純にするかのフラグ
	RK_VERBOS verboseで起こすかのフラグ
 */

jmp_buf env0;

FILE	*modefile; /* モード定義表のファイル */
char	nulstr[1] = {'\0'};

char	*hcurread, *mcurread; /* 変換表・モード表の現在行bufへのポインタ */
char	*curfnm, *curdir; /* 現在読んでる変換表・モード表の名とディレクトリ。
     但しcurdirは、サーチパスを捜してかつ表のオープンに成功した時のみ有効 */
letter	*ltrbufbgn; /* 使用中のletterのバッファの先頭を指す */
  /* これら五つは、readdata()などの関数内でローカルに定義する配列の先頭を指
     す。従って、その関数を抜けた後でこれらの値を使わないよう十分注意。特に、
     エラー警告ルーチンなどでは、longjmpで戻るより前にこれらのprintを行うこと
     （エラー処理用に使っているのは、上から四個まで）。*/
FILE	*nestfile[FILNST], **base; /* 変換対応表のファイル */

struct	modestat {
	modetyp	moderng; /* モードの状態の最大値＋１ */
	modetyp	curmode; /* モードの状態 */
} modesw[MODMAX];

char	hyoshu[HYOMAX]; /* 表の種別（前・本・後処理表の区別）を入れる配列 */
char	**modmeiptr, *modmeibgn[MODMAX]; /* モード名に対する上と同様のデータ*/
char	*modmeimem, modmeimem_[MODMEI]; /*		〃		*/
char	**dspnamptr, *dspnambgn[DMDMAX];
				/* モード表示文字列に対する同様のデータ */
char	*dspcod, dspcod_[DMDCHR]; /*		〃		*/
char	*dspmod[2][2] = {NULL, NULL, NULL, NULL};
	 /* 現在及び一つ前の、romkanをそれぞれon・offしている時のモード表示
	    文字列へのポインタ。romkan_dispmode()で返る値はdspmod[0][0]。*/

char	**hyomeiorg, **hyomeiptr;
	 /* 表の名へのポインタを入れる配列 *hyomeibgn[HYOMAX] を readdata() で
	    使うが、その先頭及び特定の要素へのポインタ */
char	*hyomeimem;
	 /* 表の名の実際の文字列を入れる配列 hyomeimem_[HYOMEI] を readdata()
	    で使うが、その特定の要素へのポインタ */
char	**pathmeiorg, **pathmeiptr;
	 /* 対応表のサーチパス名へのポインタを入れる配列 *pathmeibgn[PTHMAX]を
	    readdata() で使うが、その先頭及び特定の要素へのポインタ */
char	*pathareaorg, *pathmeimem;
	 /* 上記サーチパス名の実際の文字列を入れる配列 pathmeimem_[PTHMEI] を
	    readdata() で使うが、その先頭及び特定の要素へのポインタ。
	    但しpathmeimem_の先頭にはモード表のあるディレクトリの名が入り、後
	    で使うので、pathareaorgにはその続きのエリアの先頭の番地が入る。*/

int	usemaehyo[HYOMAX], usehyo[HYOMAX], useatohyo[HYOMAX];
	 /* 前・本・後処理表のどれが選択されているかのデータ */
int	naibu_[NAIBMX], *naibu; /* モード表の内部表現を入れる配列 */

letter	*lptr; /* letterへの汎用ポインタ */

letter	rk_input; /* 入力。3バイトまでは変更なしに対応可 */
letter	disout[OUTSIZ]; /* 本処理からの出力のバッファ */
letter	rk_output[OUTSIZ]; /* 後処理からの出力（最終出力）のバッファ */
letter	keybuf[KBFSIZ], urabuf[KBFSIZ]; /* 本処理バッファとその退避領域 */
int	lastoutlen, lastkbflen;
letter	oneletter[2] = {EOLTTR, EOLTTR}, nil[1] = {EOLTTR};
int	hyonum;

struct	funstr /* 組み込み関数の性質を保持するstruct */
{
	char	*fnname;	/* 関数名 */
	char	appear;		/* 入力コード部・出力コード部・バッファ残り部
				   に書けるかどうかのフラグ３ビット */
	char	argnum;		/* 引数の個数 */
	char	fntype;		/* 結果のタイプ */
};

struct	dat /* 入力・出力・バッファ残り部へのポインタ */
{
	letter	*code[3];
};

struct	hyo
 /* 一つの対応表の、変換データ及び各変数の変域の、格納場所へのポインタ。*/
{
	struct	dat	*data;		/* 変換データの開始番地 */
	letter		**hensudef;	/* 変域表の開始番地 */
};

#define bit3(x, y, z) ((x) | ((y) << 1) | ((z) << 2))

struct	funstr	func[] = /* 関数の追加にはここと mchevl(), evlis() をいじる */
{
/*			引数の数（-1…この値未使用）
	  名		    出現     ｜	 タイプ…0:文字関数 1:文字列関数
	  ↓		     ↓	     ↓	 ↓	 2:特殊関数 3:機能 4:宣言 */
	"defvar",	bit3(1,0,0), -1, 4, /*	0 */
	"include",	bit3(1,0,0), -1, 4,
	"toupper",	bit3(1,1,1),  1, 0,
	"tolower",	bit3(1,1,1),  1, 0,
	"off",		bit3(0,1,0), -1, 2,
	"on",		bit3(0,1,0), -1, 2, /*	5 */
	"switch",	bit3(0,1,0), -1, 2,
	"toupdown",	bit3(1,1,1),  1, 0,
	"tozenalpha",	bit3(1,1,1),  1, 0,
	"tohira",	bit3(1,1,1),  1, 0,
	"tokata",	bit3(1,1,1),  1, 0, /* 10 */
	"tohankata",	bit3(1,1,1),  1, 1,
	"tozenhira",	bit3(1,1,1),  1, 0,
	"tozenkata",	bit3(1,1,1),  1, 0,
	"+",		bit3(1,1,1),  2, 0,
	"-",		bit3(1,1,1),  2, 0, /* 15 */
	"*",		bit3(1,1,1),  2, 0,
	"/",		bit3(1,1,1),  2, 0,
	"%",		bit3(1,1,1),  2, 0,
	"last=",	bit3(1,0,0),  1, 2,  /* 直前のマッチコード==引数か? */
	"if",		bit3(1,0,0), -1, 2, /* 20 */
	"unless",	bit3(1,0,0), -1, 2,
	"restart",	bit3(0,1,0),  0, 3,
	"delchr",	bit3(1,1,1),  0, 0,  /* delchrを返す。隠しコマンド */
	"alloff",	bit3(0,1,0),  0, 2,  /* 全モードをいっぺんにOFF */
	"allon",	bit3(0,1,0),  0, 2, /* 25 */
	"bitand",	bit3(1,1,1),  2, 0,
	"bitor",	bit3(1,1,1),  2, 0,
	"bitnot",	bit3(1,1,1),  1, 0,
	"!",		bit3(0,1,0),  0, 2,  /* 裏バッファの明示的クリア */
	"atEOF",	bit3(1,0,0),  0, 2, /* 30 */
	"todigit",	bit3(1,1,1),  2, 1,
	"dakuadd",	bit3(1,1,1),  1, 1,  /* 濁点の付加 */
	"handakuadd",	bit3(1,1,1),  1, 1,  /* 半濁点の付加 */
	"value",	bit3(1,1,1),  1, 0,
	"error",	bit3(0,1,0),  0, 3, /* 35 */
	"defconst",	bit3(1,0,0), -1, 4,
	"setmode",	bit3(0,1,0), -1, 2,
	"mode+",	bit3(0,1,0), -1, 2,
	"mode-",	bit3(0,1,0), -1, 2,
	"mode=",	bit3(1,0,0), -1, 2, /* 40 */
	"mode!=",	bit3(1,0,0), -1, 2,
	"mode<",	bit3(1,0,0), -1, 2,
	"mode>",	bit3(1,0,0), -1, 2,
	"send",		bit3(0,1,0),  1, 1,  /* 試作 */
	NULL
};
	/* last=〜unless… 入力コードの位置にしか置けず、文字列関数扱いなので
			本処理表にしか書けない。*/
	/* +,-,*,/,bitand,bitor,bitnot… 3バイト分の演算を行う。*/
	/* atEOF… ファイル末尾での特別動作を指定するためのもの。但し、その
		動作は不完全。*/

struct	hyo	hyo_n[HYOMAX]; /* 表ごとの変域・対応データの開始番地 */
