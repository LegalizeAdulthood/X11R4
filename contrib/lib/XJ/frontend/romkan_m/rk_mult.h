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
 *	Original Source: ./lib/Wnn/romkan/rk_extvars.h
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
/**********************************************************************
			rk_mult.h
***********************************************************************/

#ifndef	ROMKAN_LIB
#define	ROMKAN_LIB

typedef struct	_dat /* 入力・出力・バッファ残り部へのポインタ */
{
	unsigned int	*code[3];
} dat;

typedef struct	_modestat {
	unsigned char	moderng; /* モードの状態の最大値＋１ */
	unsigned char	curmode; /* モードの状態 */
} modestat;

typedef struct	_hyo
 /* 一つの対応表の、変換データ及び各変数の変域の、格納場所へのポインタ。*/
{
	dat	*data;		/* 変換データの開始番地 */
	unsigned int		**hensudef;	/* 変域表の開始番地 */
} hyo; /* 表ごとの変域・対応データの開始番地 */

typedef struct	_matchpair /*現在マッチされている変数の番号と、マッチした文字の
				組を保持。*/
{
	int	hennum;
	unsigned int	ltrmch;
} matchpair;

typedef struct _Romkan {
char	rk_errstat; /* (error)によってエラーが引き起こされた時1（但し
			   (eofflg)が立っていたため末尾までの強制出力処理が
			   起こった場合は2）になる。romkan_henkan()実行ごとに
			   更新される。これはユーザにも開放。*/

int	flags;
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
char	nulstr[1];

char	*hcurread, *mcurread; /* 変換表・モード表の現在行bufへのポインタ */
char	*curfnm, *curdir; /* 現在読んでる変換表・モード表の名とディレクトリ。
     但しcurdirは、サーチパスを捜してかつ表のオープンに成功した時のみ有効 */
unsigned int	*ltrbufbgn; /* 使用中のletterのバッファの先頭を指す */
  /* これら五つは、readdata()などの関数内でローカルに定義する配列の先頭を指
     す。従って、その関数を抜けた後でこれらの値を使わないよう十分注意。特に、
     エラー警告ルーチンなどでは、longjmpで戻るより前にこれらのprintを行うこと
     （エラー処理用に使っているのは、上から四個まで）。*/
FILE	*nestfile[FILNST], **base; /* 変換対応表のファイル */

modestat  modesw[MODMAX];

char	hyoshu[HYOMAX]; /* 表の種別（前・本・後処理表の区別）を入れる配列 */
char	**modmeiptr, *modmeibgn[MODMAX]; /* モード名に対する上と同様のデータ*/
char	*modmeimem, modmeimem_[MODMEI]; /*		〃		*/
char	**dspnamptr, *dspnambgn[DMDMAX];
				/* モード表示文字列に対する同様のデータ */
char	*dspcod, dspcod_[DMDCHR]; /*		〃		*/
char	*dspmod[2][2];
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

unsigned int	*lptr; /* letterへの汎用ポインタ */

unsigned int	rk_input; /* 入力。3バイトまでは変更なしに対応可 */
unsigned int	disout[OUTSIZ]; /* 本処理からの出力のバッファ */
unsigned int	rk_output[OUTSIZ]; /* 後処理からの出力（最終出力）のバッファ */
unsigned int	keybuf[KBFSIZ], urabuf[KBFSIZ]; /* 本処理バッファとその退避領域 */
int	lastoutlen, lastkbflen;
unsigned int	oneletter[2], nil[1];
int	hyonum;

hyo hyo_n[HYOMAX];

unsigned int	memory[SIZALL]; /* 変換対応表や、変数の変域などを入れる領域 */
dat	data[LINALL]; /* 対応表の一行ごとのデータへのポインタ */
unsigned int	*hensudefhyo[VARTOT]; /* 変数ごとの変域データへのポインタ */
matchpair	henmatch[VARTOT]; /* 変数のマッチ状況をためておく */

#ifdef KDSP
#  ifdef MVUX
unsigned int	displine[DSPLIN]; /* デバッグ用	 表示の一行分を記憶 */
#  endif
#endif

unsigned int	*curdis;
int	codein_len;
unsigned int	*codeout, *remainkbf;
unsigned int	ungetc_buf; /* romkan_ungetcが一文字退避しておくバッファ */
unsigned int	unnext_buf; /* romkan_unnextが		   〃		  */

unsigned int	(*keyin_method)(); /* キーイン関数のアドレス */
int	(*bytcnt_method)(); /* バイトカウント関数のアドレス */
int	(*kbytcnt_method)(); /* キー入力解釈用バイトカウント関数のアドレス */
char	prv_modfnm[REALFN]; /* モード表名（又はそのパス名）を保存 */
	    /* 初期設定がいい加減! */

char	eofflg; /* romkan_next()からLTREOFが来たときに、romkan_henkan()内で1
		   にする。これの値が非0なら変換対応表の(error)を無視して、
		   本処理バッファ内の未確定コードも、強制的に変換させる。更に
		   LTREOFが来た時に特別にある出力を出すように指定されていれば
		   その処理も行う（但し、その処理は急ごしらえで不完全）。その
		   処理中はeofflgは2。*/

unsigned int	evalbuf[2][2][OUTSIZ];
char	ebf_sw;
unsigned int	saishu_out[OUTSIZ];
unsigned int	delchr, delchr2, nisedl; /* DELのキャラクタは二個まで持てる*/
} Romkan;

extern Romkan	*cur_rk;
extern Romkan	*romkan_init3();
#endif	ROMKAN_LIB
