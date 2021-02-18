.EH ''''
.OH ''''
.EF 'X-Window日本語Version研究会'3\-%'漢字入力システム仕様（第2版）'
.OF 'X-Window日本語Version研究会'3\-%'漢字入力システム仕様（第2版）'
.nr H1 2
.nr H2 0
.nr H3 0
.nr PI 2n
.NH
.LG
XJPライブラリインタフェース
.sp 1
.PP
ライブラリインタフェースとしては、2.システム概要でも触れたように以
下の２種類がある。なお両者を合わせてXJPライブラリ（XJP_library）と
呼ぶことにする。
.sp 1
.RS
.IP (1) 4
コミュニケーションキット
.IP (2) 4
簡易ライブラリ
.RE
.sp 1
.PP
(1)は、MMIをつかさどる日本語フロントエンドを別プロセス（サーバ
）として、これとの通信を行うためのライブラリである。
アプリケーションプログラムはこのライブラリを用いて、かな漢字変換
処理用の変換ウィンドウの
表示制御、あるいは変換結果の文字列（確定文字列）の獲得等を行う。
.PP
一方(2)は、XlibにあるXLookupStringと同様にKeyEventのバックエンド
として動作する簡易関数であり、変換ウィンドウの表示制御等の細かい
操作を行うことはできない。しかし既存のアプリケーションプログラムの日
本語化においては、少なくともXLookupStringの代わりにXLookupKanjiString
を用いることにより、日本語（漢字）入力は可能になる。
.PP
なお、説明は以下の形式で行う。
.sp 2
ライブラリ名(3K)
.sp 1
.nr PI 8n
.SH
名称
.RS
intro \- XJPライブラリの紹介
.RE
.sp 1
.SH
記述形式
.RS
.nf
.ta 12
#include <X11/Xlib.h>	使用するインクルードファイル
int kanji(a)	関数及び型名
int a;	引数及び型名
.RE
.sp 1
.SH
引数
.sp 1
.SH
説明
.RS
XJPライブラリについて説明する。
(3K)これらの関数は、XJPライブラリlibXJp.aを構成する。
コンパイラcc(1)はこれを自動的に呼び出さないが、\-lXJp
オプションを指定することによりリンカがこのライブラリを探索する。
.RE
.sp 1
.SH
戻り値
.sp 1
.nr PI 2n
.PP
以下、3.1ではコミュニケーションキットについて、3.2では簡易ライ
ブラリについて、その関数仕様を示す。
.RE
.bp
.NH 2
コミュニケーションキット
.LP
.sp 1
.IP (1) 4
機能一覧
.PP
コミュニケーションキットは機能別に、次のように分類できる。
.RS
.IP ・ 2
XJPフロントエンド通信制御に関わる機能
.RS
.RS
XJp_open, XJp_begin, XJp_end, XJp_close
.RE
.RE
.IP ・ 2
変換(確定)文字列の獲得に関わる機能
.RS
.RS
XJp_read, XJp_string
.RE
.RE
.IP ・ 2
かな漢字変換ウィンドウの表示制御に関わる機能
.RS
.RS
XJp_visible, XJp_invisible, XJp_move, XJp_change
.RE
.RE
.IP ・ 2
エラー処理に関わる機能
.RS
.RS
XJp_error, XJp_reset
.RE
.RE
.RE
.sp 1
.IP (2) 4
一般的な使用例
.PP
コミュニケーションキットを用いて、日本語(漢字)を入力する一般的な
使用方法(関数コールのシーケンス)を以下に示す。
.RS
.RS
.IP XJp_open() 20
XJPフロントエンドとの接続と初期化
.IP XJp_visible() 20
かな漢字変換ウィンドウの表示
.IP XJp_begin() 20
XJPフロントエンドへの変換開始指示
.IP XJp_string() 20
変換文字列の獲得
.IP XJp_end() 20
XJPフロントエンドへの変換終了指示
.IP XJp_close() 20
XJPフロントエンドとの接続断と終了処理
.RE
.RE
.PP
なお、上記以外の関数の使用については以下の通りである。
.RS
.IP ・ 2
XJp_stringの代わりにXJp_readを使用できる。
.IP ・ 2
表示制御に関わるXJp_visible, XJp_invisible, XJp_move, XJp_changeは
XJp_openとXJp_closeの間で随時使用できる。
.IP ・ 2
XJp_beginとXJp_endはXJp_openとXJp_closeの間で随時使用できる。
.RE
.sp 1
.IP (3) 4
エラー処理
.PP
XJPライブラリにはエラー処理を行うための関数が用意されている。
.RS
.RS
.IP XJp_error() 20
エラーの原因を示す簡単なメッセージを標準出力へ出力する。
.IP XJp_reset() 20
XJPフロントエンド、日本語サーバなどが異常終了した時に、
XJPライブラリをリセットしてかな漢字変換要求ウィンドウに対する
キー入力を有効にする。
.RE
.RE
.PP
キー入力を獲得したままXJPフロントエンドが異常終了した場合、
XJPライブラリはそのことを知ることができないので
かな漢字変換要求ウィンドウに対するキー入力ができなくなってしまう。
これを避け、XJPフロントエンドが異常終了したことを知るために、
クライアントはかな漢字変換ウィンドウが破壊されたことを通知
するDestroyNotifyイベントを受け取るようにする必要がある。
具体的には、かな漢字変換ウィンドウIDであるXJp_open()のリターン値を
XSelectInputの第二引数としてコールする。
.br
.nf
	xjpid = XJp_open(...);
	XSelectInput(display, xjpid, StructureNotifyMask);
.PP
さらにクライアントはかな漢字変換ウィンドウの
DestroyNotifyイベントをチェックし、
もしそのイベントが通知されたならXJp_reset()をコールして
XJp_open()をコールする以前の状態に戻す必要がある。
.sp 1
.IP (4) 4
利用方法
.PP
コミュニケーションキットを利用する場合には、以下のことが
必要である。
.RS
.IP ・ 2
/usr/include/X11/XJplib.hをインクルードする。
.IP ・ 2
/usr/lib/libXJp.aをリンクする。リンク時に以下のオプションを
指定する。
.br
               -lXJp
.RE
.sp 3
.LP
以下、インクルードファイルおよびライブラリ関数の詳細を示す。
.bp
.SH
［インクルードファイル　XJplib.h］
.LP
.nf
/* コミュニケーションキット・インクルード・ファイル */ 
.sp 1
/* コード系 */ 
.ta 6 14
#define	XJP_JIS7	0
#define	XJP_JIS8	1
#define	XJP_EUC	2
#define	XJP_SJIS	3
.sp 1
/* 文字セット */
.ta 6 16
#define	XJP_ascii	"(B"
#define	XJP_romanX0201	"(J"
#define	XJP_kanaX0201	"(I"
#define	XJP_kanjiX0208	"$B"
#define	XJP_gaiji1byte	"(0"
#define	XJP_gaiji2byte	"$0"
.sp 1
/* かな漢字変換ウィンドウ表示モード */ 
.ta 6 14
#define	XJP_UNDER	0
#define	XJP_XY	1
#define	XJP_ROOT	2
.sp 1
/* かな漢字変換ウィンドウの属性マスク */
.ta 6 14 
#define	XJP_PWIDTH	0x00000001L 
#define	XJP_PHEIGHT	0x00000002L 
#define	XJP_X	0x00000004L 
#define	XJP_Y	0x00000008L 
#define	XJP_WIDTH	0x00000010L 
#define	XJP_HEIGHT	0x00000020L 
#define	XJP_FG	0x00000040L 
#define	XJP_BG	0x00000080L 
#define	XJP_BP	0x00000100L 
#define	XJP_FONTS	0x00000200L 
#define	XJP_STATUS	0x00000400L 
.sp 1
/* フォント情報構造体 */ 
.ta 4 15
typedef struct {
	char	*id;
	char	*fontname;
} XJp_fontInfo;
.sp 1
/* かな漢字変換ウィンドウの属性構造体 */ 
.ta 4 15
typedef struct｛
	long	p_width;
	long	p_height;
	long	x;
	long	y;
	long	width;
	long	height;
	unsigned long	fg;
	unsigned long	bg;
	long	bp;
	long	status_x;
	long	status_y;
	long	status_width;
	long	status_height;
	XJp_fontInfo	*fonts;
	int	num_fonts;
｝XJp_attributes;
.sp 1
typedef	unsigned long	XJp_id;
extern int	XJp_errno;
.EH 'XJp_open(3K)'XJP_library Manual'XJp_open(3K)'
.OH 'XJp_open(3K)'XJP_library Manual'XJp_open(3K)'
.bp
.nr PI 8n
.SH
名称
.RS
XJp_open \- かな漢字変換ウィンドウの初期化
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
XJp_id XJp_open(\fIdisplay, w, valuemask, values, dispmode, aux_data\fP)
.RS
.nf
.ft B
Display \fI*display\fP;
Window \fIw\fP; 
unsigned long \fIvaluemask\fP;
XJp_attributes \fI*values\fP;
int \fIdispmode\fP;
char \fI*aux_data\fP;
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIdisplay\fP 14
ディスプレイ構造体へのポインタ
.IP \fIw\fP 14
かな漢字変換要求ウィンドウID
.IP \fIvaluemask\fP 14
valuesで定義する属性を指定する。このマスクは有効な
属性のビットマスクのビットORである。valuemaskがゼ
ロの場合, 次のvalues引数は無視される。なお、マスクとしては
以下のものが指定できる。
.RS
.IP XJP_PWIDTH 16
かな漢字変換要求ウィンドウの幅
.IP XJP_PHEIGHT 16
かな漢字変換要求ウィンドウの高さ
.IP XJP_X 16
変換ウィンドウの位置
.IP XJP_Y 16
変換ウィンドウの位置
.IP XJP_WIDTH 16
変換ウィンドウの幅
.IP XJP_HEIGHT 16
変換ウィンドウの高さ
.IP XJP_FG 16
文字の色
.IP XJP_BG 16
背景色
.IP XJP_BP 16
変換ウィンドウの境界幅
.IP XJP_FONTS 16
フォント名
.IP XJP_STATUS 16
ステータス表示領域の位置（XJp_openでは無視さ
れる。XJp_changeのときのみ有効である。）
.RE
.IP \fIvalues\fP 14
この構造体に属性を設定する。valuemaskにはvaluesに
設定した属性に該当するビットマスクを指定しなければ
ならない。なお、この構造体の要素は以下の通りである。
.RS
.IP long\ p_width; 20
かな漢字変換要求ウィンドウの幅（ピクセル単位）
.IP long\ p_height; 20
かな漢字変換要求ウィンドウの高さ（ピクセル単位）
.IP long\ x; 20
変換ウィンドウの位置（ピクセル単位）
.br
ただし、座標はかな漢字変換要求ウィンドウに対する相対座標
.IP long\ y; 20
変換ウィンドウの位置（ピクセル単位）
.IP long\ width; 20
変換ウィンドウの幅（ピクセル単位）
.IP long\ height; 20
変換ウィンドウの高さ（ピクセル単位）
.IP unsigned\ long\ fg; 20
変換ウィンドウにおける文字の色
.IP unsigned\ long\ bg; 20
変換ウィンドウにおける背景色
.IP long\ bp; 20
変換ウィンドウの境界幅（ピクセル単位）
.IP XJp_fontInfo\ *fonts; 20
フォント情報構造体の配列へのポインタ
.IP int\ num_fonts; 20
フォント情報構造体の配列数
.IP long\ status_x; 20
ステータス表示領域の位置（ピクセル単位）
.IP long\ status_y; 20
ステータス表示領域の位置（ピクセル単位）
.IP long\ status_width; 20
ステータス表示領域の幅（ピクセル単位）返値
.IP long\ status_height; 20
ステータス表示領域の高さ（ピクセル単位）返値
.IP char\ *aux_data; 20
フロントエンド用プライベートデータへのポインタ。ただし、データサイズ
は128バイト以内、内容はフロントエンドに依存する。
.RE
.IP \fIdispmode\fP 14
かな漢字変換ウィンドウの表示モードを以下の値で指定
する。
.RS
.IP XJP_UNDER 16
かな漢字変換ウィンドウを指定したウィンドウの下部に開く。
.IP XJP_XY 16
かな漢字変換ウィンドウを指定したウィンドウの指定した位置に開く。
.IP XJP_ROOT 16
かな漢字変換ウィンドウをルートウィンドウの指定した位置に開く。
.RE
.RE
.SH
説明
.RS
XJp_openは、かな漢字変換ウィンドウに関する初期化処理を行う。仮
名漢字変換の実行に先立ち、必ず呼び出す必要がある。通常は、各ク
ライアントの本体ウィンドウ生成後に呼び出す。
.br
\fIdispmode\fPによって、\fIvalues\fPで指定できるものが
異なる（表１を参照）。
.br
また、\fIdispmode\fPに応じて、以下の違いがある。
.IP XJP_UNDER 16
フロントエンドが作成する変換ウィンドウのために必要な領域を
クライアント側で確保しておく必要がある。ステータス表示領域の
作成に関しては、クライアント側で指定する必要はない。
.IP XJP_XY 16
本関数をコールすると、ステータス表示領域の幅、高さが\fIvalues\fPの
\fIstatus_width,status_height\fPにセットされてリターンされる。
ステータス表示領域の位置を指定する場合は、XJp_openの後に
XJp_changeをコールする必要がある。
.IP XJP_ROOT 16
変換ウィンドウ、ステータス表示領域に関してクライアント側では
何も指定する必要はない。
.LP
\fIfonts\fPにはフォント情報構造体の配列を指定する。
フォント情報構造体の配列は必要とするフォントの数だけ用意する。
構造体の要素idには以下に示すものが指定できる。
.IP XJP_ascii 20
ASCII文字
.IP XJP_romanX0201 20
JIS-X0201ローマ文字
.IP XJP_kanaX0201 20
JIS0201カタカナ文字
.IP XJP_kanjiX0208 20
JIS-X0208漢字
.IP XJP_gaiji1byte 20
１バイト外字
.IP XJP_gaiji2byte 20
２バイト外字
.RE
.sp 1
.SH
戻り値
.RS
正常終了の場合、かな漢字変換要求IDを、異常終了の場合\-1を返す。
.RE
.sp 1
.SH
［インプリメント上の注意］
.RS
.IP (1) 4
クライアントが死んだ時にかな漢字変換ウィンドウの資源を解放するた
めに、StructureNotifyMaskでDestroyWindow イベントを待つこと。
.IP (2) 4
XJp_openのリターン値はかな漢字変換ウィンドウIDとする。
.RE
.KS
.sp 3
.DS C
表１．XJp_openでの\fIdispmode\fPと\fIvaluemask\fP
.DE
.TS
center box tab(;);
c||c s s
c||c|c|c.
XJp_attrubutes;dispmode
;_;_;_
メンバー名;XJP_UNDER;XJP_XY;XJP_ROOT
=
XJP_PWIDTH;○;○;−
_
XJP_PHEIGHT;○;○;−
_
XJP_X;０;○;−
_
XJP_Y;p_height-height;○;−
_
XJP_WIDTH;p_width;○;−
_
XJP_HEIGHT;フォントの高さ;フォントの高さ;−
_
XJP_FG;フロントエンドの;フロントエンドの;−
;フォアグランド;フォアグランド
_
XJP_BG;フロントエンドの;フロントエンドの;−
;バックグランド;バックグランド
_
XJP_BP;１;１;−
_
XJP_FONTS;○;○;−
_
XJP_STATUS;−;−;−
_
.TE
.RS
.RS
.RS
.nf
○ : 必ず設定しなければいけないもの
− : 設定しても無効になるもの
これら以外のものは、設定されない場合のデフォルト値
.fi
.RE
.RE
.RE
.KE
.EH 'XJp_begin(3K)'XJP_library Manual'XJp_begin(3K)'
.OH 'XJp_begin(3K)'XJP_library Manual'XJp_begin(3K)'
.bp
.SH
名称
.RS
XJp_begin \- キー入力の開始
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_begin(\fIrequest_id, w_key\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
Window \fIw_key\fP;
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIrequest_id\fP 14
かな漢字変換要求ID
.IP \fIw_key\fP 14
キー入力を受け付けるウィンドウID
.RE
.SH
説明
.RS
XJp_beginは、指定したウィンドウw_keyからのキー入力を有効にす
る。
.RE
.SH
戻り値
.RS
正常終了の場合0を、異常終了の場合\-1を返す。
.RE
.EH 'XJp_visible(3K)'XJP_library Manual'XJp_visible(3K)'
.OH 'XJp_visible(3K)'XJP_library Manual'XJp_visible(3K)'
.bp
.SH
名称
.RS
XJp_visible \- かな漢字変換ウィンドウの表示
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_visible(\fIrequest_id\fP) 
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIrequest_id\fP 14
かな漢字変換要求ID
.RE
.SH
説明
.RS
XJp_visible は、指定したかな漢字変換要求IDに対応したかな漢字変換
ウィンドウを表示する。
.br
ただし、\fIdispmode\fPがXJP_ROOTの場合には、本関数は無効である。
.RE
.SH
戻り値
.RS
正常終了の場合0を、異常終了の場合\-1を返す。
.RE
.EH 'XJp_read(3K)'XJP_library Manual'XJp_read(3K)'
.OH 'XJp_read(3K)'XJP_library Manual'XJp_read(3K)'
.bp
.SH
名称
.RS
XJp_read \- かな漢字変換の結果を獲得（1）
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h>
#include <X11/XJplib.h>
int XJp_read(\fIxEv, buffer, buffer_size, code_length, character_set\fP)
.RS
.nf
.ft B
XClientMessageEvent \fI*xEv\fP; 
unsigned char \fI*buffer\fP; 
int \fIbuffer_size\fP; 
int \fI*code_length\fP; 
char \fI*character_set\fP;
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIxEv\fP 14
かな漢字変換の確定データを通知するクライアントメッセージ・イベント
.IP \fIbuffer\fP 14
変換結果を返すバッファ
.IP \fIbuffer_size\fP 14
バッファの大きさ（17バイト以上を確保しておくこと）
.IP \fIcode_length\fP 14
１文字のコード長（バイト）
.IP \fIcharacter_set\fP 14
確定データの文字セット（4バイト以上を確保しておくこと）
.RS
.IP XJP_ascii 20
ASCII文字
.IP XJP_romanX0201 20
JIS\-X0201ローマ文字
.IP XJP_kanaX0201 20
JIS\-X0201カタカナ文字
.IP XJP_kanjiX0208 20
JIS\-X0208漢字
.IP XJP_gaiji1byte 20
１バイト外字
.IP XJP_gaiji2byte 20
２バイト外字
.RE
.RE
.SH
説明
.RS
XJp_readは、指定したbufferに文字セットごとに変換結果を返す（ただし、
最後はNULL）。その際、code_length, character_set にはbufferに
格納されている１文字のコード長、文字セットの種類が入ってくる。この
関数は日本語フロントエンドより、かな漢字変換確定（クライアント・メ
ッセージ）が通知された後に呼ぶ必要がある。
.RE
.SH
戻り値
.RS
正常終了の場合はバッファ内に格納された\fBデータの文字数\fPを、異常終
了の場合は\-1を返す。
.RE
.EH 'XJp_string(3K)'XJP_library Manual'XJp_string(3K)'
.OH 'XJp_string(3K)'XJP_library Manual'XJp_string(3K)'
.bp
.SH
名称
.RS
XJp_string \- かな漢字変換の結果を獲得（2）
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_string(\fIxEv, buffer, buffer_size, code\fP) 
.RS
.nf
.ft B
XClientMessageEvent \fI*xEv\fP; 
unsigned char \fI*buffer\fP; 
int \fIbuffer_size\fP; 
int \fIcode\fP; 
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIxEv\fP 14
かな漢字変換の確定データを通知するクライアントメッセージ・イベント
.IP \fIbuffer\fP 14
変換結果を返すバッファ
.IP \fIbuffer_size\fP 14
バッファの大きさ（33バイト以上確保すること）
.IP \fIcode\fP
コード系
.RS
.nf
.IP XJP_JIS7 16
JIS7単位コード系
.IP XJP_JIS8 16
JIS8単位コード系
.IP XJP_EUC 16
EUCコード
.IP XJP_SJIS 16
シフトJISコード
.RE
.RE
.SH
説明
.RS
XJp_stringは、指定したbufferにcodeで指定されたコード系で
変換結果を返す（ただし、最後はNULL）。この関数は日本語
フロントエンドより、かな漢字変換確定（クライアント・メッセージ）が
通知された後に呼ぶ必要がある。
.RE
.SH
戻り値
.RS
正常終了の場合はバッファ内に格納された\fBデータのバイト数\fPを、異常
終了の場合は\-1を返す。
.RE
.EH 'XJp_invisible(3K)'XJP_library Manual'XJp_invisible(3K)'
.OH 'XJp_invisible(3K)'XJP_library Manual'XJp_invisible(3K)'
.bp
.SH
名称
.RS
XJp_invisible \- かな漢字変換ウィンドウの非表示
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_invisible(\fIrequest_id\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIrequest_id\fP 14
かな漢字変換要求ID
.RE
.SH
説明
.RS
XJp_invisibleは、かな漢字変換ウィンドウを非表示にする。
.br
ただし、本関数は\fIdispmode\fPがXJP_ROOTの場合には無効である。
.RE
.SH
戻り値
.RS
正常終了時には0を、異常終了時には\-1を返す。
.RE
.EH 'XJp_end(3K)'XJP_library Manual'XJp_end(3K)'
.OH 'XJp_end(3K)'XJP_library Manual'XJp_end(3K)'
.bp
.SH
名称
.RS
XJp_end \- キー入力の終了
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_end(\fIrequest_id, w_key\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
Window \fIw_key\fP;
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIrequest_id\fP 14
かな漢字変換要求ID
.IP \fIw_key\fP 14
キー入力を受け付けるウィンドウID
.RE
.SH
説明
.RS
XJp_endは、指定したウィンドウw_keyからのキー入力を無効にする。
.RE
.SH
戻り値
.RS
正常終了時には0を、異常終了時には\-1を返す。
.RE
.EH 'XJp_close(3K)'XJP_library Manual'XJp_close(3K)'
.OH 'XJp_close(3K)'XJP_library Manual'XJp_close(3K)'
.bp
.SH
名称
.RS
XJp_close \- かな漢字変換要求の終了
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_close(\fIrequest_id\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIrequest_id\fP 14
かな漢字変換要求ID
.RE
.SH
説明
.RS
XJp_close は、かな漢字変換要求request_IDによる変換を終了する。
.RE
.SH
戻り値
.RS
正常終了時には0を、異常終了時には\-1を返す。
.RE
.EH 'XJp_move(3K)'XJP_library Manual'XJp_move(3K)'
.OH 'XJp_move(3K)'XJP_library Manual'XJp_move(3K)'
.bp
.SH
名称
.RS
XJp_move \- 変換ウィンドウの移動
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_move(\fIrequest_id, x, y\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP;
int \fIx, y\fP;
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIrequest_id\fP 14
かな漢字変換要求ID
.IP \fIx,\ y\fP 14
かな漢字変換ウィンドウを表示する位置を、かな漢字変換要求ウ
ィンドウ左上からのピクセル単位で指定する。
.RE
.SH
説明
.RS
XJp_moveは、XJPフロントエンドに対し、既に送出したXJp_open、ま
たは直前に発行したXJp_moveで設定したかな漢字変換ウィンドウの属
性を変更する。
.br
このライブラリはかな漢字変換ウィンドウのdispmodeがXJP_XYの時の
み有効である。
.RE
.SH
戻り値
.RS
正常終了時には0を、異常終了時には\-1を返す。
.RE
.EH 'XJp_change(3K)'XJP_library Manual'XJp_change(3K)'
.OH 'XJp_change(3K)'XJP_library Manual'XJp_change(3K)'
.bp
.SH
名称
.RS
XJp_change \- ＸＪＰフロントエンドに対する制御
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_change(\fIrequest_id, valuemask, values\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP;
unsigned long \fIvaluemask\fP;
XJp_attributes \fI*values\fP;
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIrequest_id\fP 14
かな漢字変換要求ID
.IP \fIvaluemask\fP 14
valuesで定義する属性を指定する。このマスクは有
効な属性のビットマスクのビットORである。value
mask がゼロの場合, 次のvalues引数は無視される。
ビットマスクの詳細については、XJp_openを参照。
.IP \fIvalues\fP 14
この構造体に変更したい属性を設定する。valuemask
にはvaluesに設定した属性に該当するビットマスク
を指定しなければならない。この構造体の詳細については、
XJp_openを参照。
.RE
.SH
説明
.RS
XJp_changeは、ＸＪＰフロントエンドに対し、既に送出したXJp_open、
または直前に発行したXJp_changeで設定したかな漢字変換ウィンドウ
の属性を変更する。
.br
XJp_openした時の\fIdispmode\fPによって
\fIvalues\fPで指定できるものが違ってくる（表２を参照）。
また、ステータスの表示は、XJp_openの項で説明したように、
XJp_openで取得した\fIstatus_width,status_height\fPに基づいて
本関数で\fIstatus_x,status_y,status_width,status_height\fPを
指定する。ただし、ステータスの表示指定は、\fIdispmode\fPが
XJP_XYの場合のみ有効である。
.RE
.SH
戻り値
.RS
正常終了時には0を、異常終了時には\-1を返す。
.RE
.KS
.sp 3
.DS C
表２．XJp_changeにおける\fIdispmode\fPと\fIvaluemask\fP
.DE
.TS
center box tab(;);
c||c s s
c||c|c|c.
XJp_attrubutes;dispmode
;_;_;_
メンバー名;XJP_UNDER;XJP_XY;XJP_ROOT
=
XJP_PWIDTH;○;○;−
_
XJP_PHEIGHT;○;○;−
_
XJP_X;−;○;−
_
XJP_Y;−;○;−
_
XJP_WIDTH;−;○;−
_
XJP_HEIGHT;○;○;−
_
XJP_FG;○;○;−
_
XJP_BG;○;○;−
_
XJP_BP;○;○;−
_
XJP_FONTS;×;×;−
_
XJP_STATUS;−;○(注);−
_
.TE
.RS
.RS
.RS
.nf
○ : 変更可能なもの
× : 変更できないもの
− : 変更しても無視されるもの
.fi
.sp 1
.IP (注) 4
dispmode が XJP_XY で XJP_STATUS を設定しなかった場合、ステータス
表示領域の位置はフロントエンドに依存したものとなる。
.RE
.RE
.RE
.KE
.EH 'XJp_error(3K)'XJP_library Manual'XJp_error(3K)'
.OH 'XJp_error(3K)'XJP_library Manual'XJp_error(3K)'
.bp
.SH
名称
.RS
XJp_error \- エラーメッセージの取り出し
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
char *XJp_error()
.RE
.SH
説明
.RS
XJp_errorは、最新のコミュニケーションキットのエラーメッセージ
（英文）を返す。
エラー番号は外部int変数XJp_errnoに設定されている値を使う。
XJp_errnoの値はコミュニケーションキットでエラーが発生した時に
設定され、エラーでない時は更新されない。
.RE
.SH
戻り値
.RS
正常終了時にはエラーメッセージ文字列へのポインタを、異常終了
時（エラー番号に対応するエラーメッセージがないなど）にはNULLを返す。
.RE
.sp 1
.EH 'XJp_reset(3K)'XJP_library Manual'XJp_reset(3K)'
.OH 'XJp_reset(3K)'XJP_library Manual'XJp_reset(3K)'
.bp
.SH
名称
.RS
XJp_reset \- XJPライブラリのリセット
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
XJp_reset(\fIevent\fP)
.RS
.nf
.ft B
XEvent *\fIevent\fP;
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIevent\fP 14
通知されたDestroyNotifyイベント
.RE
.SH
説明
.RS
XJp_resetは、XJPライブラリをリセットしてXJp_openが
コールされる前の状態（SelectInputMaskの値など）にする。
.br
なお、本関数はDestroyNotifyイベントに対してのみ有効で、他の
イベントに対してはエラーリターンする。
.RE
.SH
戻り値
.RS
正常終了時には0を、異常終了時には\を返す。
.RE
.sp 1
.ta 4,8,12,16,20,24
.nr PI 2n
/* エラー処理のコーディング例（引数などは省略） */
.sp 1
.RS
.nf
main()
{
	Window wid;
	XJp_id request_id;

	wid = XCreateWindow(....);	/* かな漢字変換要求ウィンドウ */
	XSelectInput(display, wid, KeyPressMask);
	request_id = XJp_open(....);	/* かな漢字変換ウィンドウの初期化 */
	XSelectInput(display, request_id, StructureNotifyMask);
	.....
	XJp_begin(....);		/* キー入力の開始 */
	XJp_visible(....);		/* かな漢字変換ウィンドウの表示 */
	.....
	while(1) {
		XEvent event;

		XNextEvent(&event);
		switch(event.type) {
		case DestroyNotify:
			if (event.XDestroyWindowEvent.window == request_id) {
				/* ウィンドウのチェック */
				XJp_reset(&event);	/* XJPのリセット */
				fprintf(stderr, "XJPフロントエンドが異常終了しました。\\n");
				....
			}
			break;
		.....
		}
	}
}
.RE
.EH ''''
.OH ''''
.bp
.nr PI 2n
.NH 2
簡易ライブラリ
.PP
簡易ライブラリを使用する場合には、以下のことが必要で
ある。
.IP (1) 4
/usr/lib/libXJp.a
.PP
簡易ライブラリを組み込んだプログラムをリンクする場合
は以下のオプションを指定しなければならない。
.br
.ta 6
	\-lXJp
.sp 3
.LP
以下にライブラリ関数の詳細を示す。
.EH 'XLookupKanjiString(3K)'XJP_library'XLookupKanjiString(3K)'
.OH 'XLookupKanjiString(3K)'XJP_library'XLookupKanjiString(3K)'
.bp
.nr PI 8n
.SH
名称
.RS
XLookupKanjiString \- キーボード入力の漢字文字列への変換
.RE
.SH
記述形式
.RS
.nf
.ft B
#include <X11/XJplib.h>
int XLookupKanjiString(\fIevent_struct, buffer_return, bytes_buffer,\fP
             　　　\fIkeysym_return, status_return\fP)
.RS
.nf
.ft B
XKeyEvent \fI*event_struct\fP;
char \fI*buffer_return\fP;
int \fIbytes_buffer\fP;
KeySym \fI*keysym_return\fP;
XComposeStatus \fI*status_return\fP;
.ft R
.RE
.RE
.SH
引数
.RS
.IP \fIevent_struct\fP 16
キーイベントを指定する。
.IP \fIbuffer_return\fP 16
変換した漢字文字列が返される。
.IP \fIbytes_buffer\fP 16
バッファの長さを指定する。
.IP \fIkeysym_return\fP 16
キーシンボルを返す。NULLを指定することも可。
.IP \fIstatus_return\fP 16
XCompose構造体へのポインタを指定する。（現在は使われていない）
.RE
.SH
説明
.RS
\fIXLookupKanjiString\fPは、キーイベントを日本語文字列に
対応させる簡易ルーチンで、
キーイベントのモディファイアビットを使用してシフト、ロック、
コントロール等の処理を行う。
.br
\fIXLookupKanjiString\fPによって8bitアルファベット文字及び
16bit日本語文字を取り扱うことができる。
.br
アルファベット文字の取り扱いについては、
\fIXLookupKanjiString\fPは\fIXLookupString\fPと同様の動作になる。
.br
日本語文字の取り扱いについては、
この関数の内部においてローマ字かな変換、
かな漢字変換が行われる。変換が確定したときのみに
バッファに変換されたEUC文字列が格納され、
その文字列の長さがバイト単位で返される。そのとき
KeySymとしてはNoSymbolが返される。
.RE
.SH
戻り値
.RS
キーイベントに
よって変換が確定されない場合にはバッファには
何も格納されず、戻り値として長さ０が返される。
.br
エラーが検出された場合には、戻り値として\-1が返り、XJp_errnoに
エラー番号がセットされる。
.RE
.SH
使用例
.RS
.nf
    XNextEvent ( dpy, &ev );   /* イベントをとる　*/
    switch ( ev.xany.type )
    {
    case Expose:
            proc_expose ( &ev );
            break;
    case KeyPress:             /* キー押下　*/
            proc_keypress ( &ev );
            break;
    case ButtonPress:
            proc_buttonpress ( &ev );
            break;
    default:
            UnknownProc ( &ev );
    }

.......

proc_keypress ( ev )
    XKeyEvent *ev;
{
        char            buffer[BUFFERSIZE];
        int             len;
        KeySym          ks;
        XComposeStatus  xcs;

        len = \fIXLookupKanjiString\fP ( ev, buffer, BUFFERSIZE,
                                   &ks, &xcs);
        if ( len > 0 ) 　/*　lenが正ならば文字列が返ってきている　*/
                output_string ( buffer, len );
}
.RE
