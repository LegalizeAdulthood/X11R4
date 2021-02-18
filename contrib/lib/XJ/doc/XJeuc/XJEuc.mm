.\"
.\" 		    Copyright 1989 TOSHIBA CORPORATION
.\"
.\" Permission to use, copy, modify, and distribute this software and its
.\" documentation for any purpose and without fee is hereby granted, provided
.\" that the above copyright notice appear in all copies and that both that
.\" copyright notice and this permission notice appear in supporting
.\" documentation, and that the name of TOSHIBA Corporation not be used in 
.\" advertising or publicity pertaining to distribution of the software 
.\" without specific, written prior permission. TOSHIBA Corporation makes no 
.\" representations about the suitability of this software for any purpose.  
.\" It is provided "as is" without express or implied warranty.
.\"
.\" TOSHIBA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
.\" ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
.\" TOSHIBA BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
.\" ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
.\" WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
.\" ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
.\" SOFTWARE.
.\"
.\" Author:  
.\" 		Katsuhisa Yano
.\" 		Tomoko Koi
.\"		Personal Computer Design Dept.
.\"		TOSHIBA CORPORATION OME WORKS
.\"		5-20-7 SORD BLD. MASAGO 
.\"		CHIBA-CITY CHIBA 260 JAPAN
.\"
.H 1 概要
.PP
ここで説明する各関数は Xlib の文字列のサイズの問い合わせ・描画などを行う関数を EUC 文字列が扱えるように拡張したものです。
拡張された関数では環境変数 CSWIDTH を参照しますので、関数を呼びだす前に CSWIDTH を正しく設定する必要があります。
また、プログラム中では次のファイルをインクルードして下さい。
.DS
#include <X11/XJEuc.h>
.DE
.P
関数の引数などのプログラム・インタフェースはできる限り Xlib に合わせていますので、特にことわり書きのないものについては Xlib の関数を参照して下さい。
.P
プログラム・インタフェースで Xlib と大きく違う点は XFontStruct 構造体の取り扱いです。
拡張された関数では XFontStruct 構造体の代わりに次の構造体を使用して下さい。
.DS
typedef struct {
	XFontStruct	*font_std[4];	/* standard font structure table */
	XFontStruct	*font_ext[4];	/* extension font structure table */
	int		ascent;		/* maximum ascent */
	int		descent;	/* maximum descent */
} XJeFontStruct;
.DE
.P
XJeFontStruct 構造体には各コードセットごとに２種類（標準、拡張）のフォントを指定することができます。
EUC 文字列を処理する各関数はデフォルトで標準フォントを使用しますが、標準フォントで用意されている範囲外のコードの文字については拡張用フォントを使用します。
標準フォントとして使う XFontStruct 構造体へのポインタを各コードセットごとに font_std[] に設定して下さい。
拡張フォントを使用する場合は font_ext[] に設定して下さい。
使用しないコードセットにはＮＵＬＬポインタを設定して下さい。
.H 1 フォントのロード
.H 2 XJeLoadQueryFont
.DS
XJeFontStruct *XJeLoadQueryFont(display, name_table)
    Display *display;
    XJeFontNameStruct *name_table;
.DE
.VL 16 0 1
.LI display
Ｘサーバへのコネクションを指定します。
.LI name_table
XJeFontNameStruct 構造体へのポインタを指定します。
.LE
.P
XJeLoadQueryFont 関数は name_table で指定された名前のフォントをロードし、XJeFontStruct 構造体へのポインタを返します。
XJeFontNameStruct 構造体は次のようになっています。
.DS
typedef struct {
	char	*fontname_std[4];	/* standard font name */
	char	*fontname_ext[4];	/* extension font name */
} XJeFontNameStruct;
.DE
.P
fontname_std[] には標準フォントとして使用するフォント名を各コードセットごとに設定し、fontname_ext[] には拡張用フォントのフォント名を各コードセットごとに設定して下さい。
使用しないコードセットにはＮＵＬＬポインタを設定して下さい。
関数が返す XJeFontStruct 構造体の font_std[] と font_ext[] には、各コードセットで使用する XFontStruct 構造体へのポインタが入ります。
また ascent と descent には指定されたフォントの中での ascent と descent の最大値が入ります。
フォントが１つもロードできない場合は、ＮＵＬＬが返されます。
.H 1 文字列サイズの問い合わせ
.H 2 文字列サイズの計算
.DS
int XJeTextWidth(font_struct, string, count)
    XJeFontStruct *font_struct;
    unsigned char *string;
    int count;
.DE
.VL 16 0 1
.LI font_struct
XJeFontStruct 構造体を指定します。
.LI string
EUC 文字列を指定します。
.LI count
計算させるバイト数（文字数ではありません）を指定します。
.LE
.P
XJeTextWidth は EUC 文字列の幅を返します。値はピクセル単位です。
font_struct には XJeFontStruct 構造体を指定して下さい。
count には文字数ではなくバイト数を指定して下さい。
文字列中のコードセット１〜３に対応するフォントが指定されていない場合は、
コードセット０のフォントが使用されます。
.H 2 論理サイズの計算
.DS
XJeTextExtents(font_struct, string, nchars, direction_return, 
	       font_ascent_return, font_descent_return, overall_return)
    XJeFontStruct *font_struct;
    unsigned char *string;
    int nchars;
    int *direction_return;
    int *font_ascent_return, *font_descent_return;
    XCharStruct	*overall_return;
.DE
.VL 24 0 1
.LI font_struct 
XJeFontStruct 構造体を指定します。
.LI string
EUC 文字列を指定します。
.LI nchars
計算するバイト数（文字数ではありません）を指定します。
.LI direction_return
描画方向が返されます。
.LI font_ascent_return
フォントの ascent の最大値が返されます。
.LI font_descent_return
フォントの descent の最大値が返されます。
.LI overall_return
XCharStruct 構造体が返されます。
.LE
.P
XJeTextExtents は EUC 文字列に対する論理的なサイズをローカルに計算します。
font_struct には XJeFontStruct 構造体を指定して下さい。
count には文字数ではなくバイト数を指定して下さい。
direction_return には最後の文字の描画方向が返されます。
font_ascent_return には使用したフォントの ascent の最大値が返されます。
font_descent_return には使用したフォントの descent の最大値が返されます。
overall_return については XTextExtents を参照して下さい。
文字列中のコードセット１〜３に対応するフォントが指定されていない場合は、
コードセット０のフォントが使用されます。
.H 1 文字列の描画
.H 2 テキスト文字列の描画
.DS
XJeDrawString(display, d, gc, font_struct, x, y, string, length)
Display *display;
Drawable d;
GC gc;
XJeFontStruct *font_struct;
int x, y;
char *string;
int length;
.DE
.VL 16 0 1
.LI display
Ｘサーバへのコネクションを指定します。
.LI d
描画対象物を指定します。
.LI gc
GC を指定します。
.LI font_struct
XJeFontStruct 構造体を指定します。
.LI x

.LI y
描画位置を指定します。
.LI string
EUC 文字列を指定します。
.LI length
描画するバイト数（文字数ではありません）を指定します。
.LE
.P
XJeDrawString は EUC 文字列を描画します。
font_struct には XJeFontStruct 構造体を指定して下さい。
count には文字数ではなくバイト数を指定して下さい。
文字列中のコードセット１〜３に対応するフォントが指定されていない場合は、
コードセット０のフォントが使用されます。
描画中にフォントが切り替わる場合は、GC 内のフォントも変わります。
従って、関数が復帰したときの GC 内のフォントは、最後に使用したフォントとなります。
エラーが発生しても途中まで描画されていることがあります。
エラーの値については XDrawString を参照して下さい。
.H 2 イメージ・テキスト文字列の描画
.DS
XJeDrawImageString(display, d, gc, font_struct, x, y, string, length)
Display *display;
Drawable d;
GC gc;
XJeFontStruct *font_struct;
int x, y;
char *string;
int length;
.DE
.VL 16 0 1
.LI display
Ｘサーバへのコネクションを指定します。
.LI d
描画対象物を指定します。
.LI gc
GC を指定します。
.LI font_struct
XJeFontStruct 構造体を指定します。
.LI x

.LI y
描画位置を指定します。
.LI string
EUC 文字列を指定します。
.LI length
描画するバイト数（文字数ではありません）を指定します。
.LE
.P
XJeDrawImageString は EUC 文字列を描画します。
font_struct には XJeFontStruct 構造体を指定して下さい。
count には文字数ではなくバイト数を指定して下さい。
バックグランドの矩形の高さは、使用したフォントの最大の ascent と descent をたしたものとなります。
文字列中のコードセット１〜３に対応するフォントが指定されていない場合は、
コードセット０のフォントが使用されます。
描画中にフォントが切り替わる場合は、GC 内のフォントも変わります。
従って、関数が復帰したときの GC 内のフォントは、最後に使用したフォントとなります。
エラーが発生しても途中まで描画されていることがあります。
エラーの値については XDrawImageString を参照して下さい。
