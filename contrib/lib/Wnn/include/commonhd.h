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
/****************
 * Common header 
 ****************/
#include <stdio.h>
#define	JSERVER_VERSION	0x4020

#include "wnnerror.h"

#ifndef JS
typedef  unsigned int  UINT;
typedef  unsigned char UCHAR;
typedef  unsigned short w_char;
#endif /*JS */
#ifndef WCHAR_T
typedef  unsigned short wchar_t;
#define WCHAR_T
#endif

/*
#ifndef JSLIB
#include "jslib.h"
#endif
*/

#ifdef SX8870
# ifndef	SYSVR2
#  define	SYSVR2
# endif
# ifndef	TERMINFO
#  define	TERMINFO
# endif
#endif

#ifdef SX9100BSD
# ifndef	BSD42
#  define	BSD42
# endif
# ifndef	BSD43
#  define	BSD43
# endif
# ifndef	TERMCAP
#  define	TERMCAP
# endif
#endif

#ifdef SUN
# ifndef	BSD42
#  define	BSD42
# endif
# ifndef	TERMCAP
#  define	TERMCAP
# endif
#endif

#define MAXBUNSETSU	80
#define LIMITBUNSETSU   400
#define MAXJIKOUHO	400

#define J_IUJIS 0
#define J_EUJIS 1
#define J_JIS   2
#define J_SJIS  3


#ifdef	True
#undef	True
#endif
#define True 		1 /* to avoid "TRUE redefined" */
#define NIL 		0
#define KANJI(x)	((x) & 0x80)


#define Ctrl(X)		('X' & 0x1f)

#define NEWLINE		Ctrl(J)
#define CR		Ctrl(M)
#define ESC		'\033'

#if defined(SX8870) || defined(SX9100BSD)
#define RUBOUT		0x08	/* BS */
#else
#define RUBOUT		'\177'
#endif
#define SPACE		' '

#define KANJIJIKOUHO	0x1EF2 /* (036 * 256 + 0362) */
#define ONLINEONKUN	0x1EF1 /* (036 * 256 + 0361) */


#define LENGTHBUNPOU	32

#define BAR		0xA1BC	/* ー	*/
#define KUTEN_NUM	0xA1A3	/* 。	*/
#define TOUTEN_NUM	0xA1A2	/* 、	*/
#define S_NUM		0xA3B0	/* ０	*/
#define E_NUM		0xA3B9	/* ９	*/
#define S_HIRA		0xA4A1	/* ぁ	*/
#define E_HIRA		0xA4F3	/* ん	*/
#define S_KATA		0xA5A1	/* ァ	*/
#define E_KATA		0xA5F6	/* ヶ	*/
#define S_HANKATA	0x00A1	/* ｡	*/
#define E_HANKATA	0x00DF	/* ﾟ    */

#define HIRAP(X) ((X) >= S_HIRA && (X) <= E_HIRA)
#define KATAP(X) (((X) >= S_KATA && (X) <= E_KATA) || ((X) == BAR))
#define ASCIIP(X) ((X) < 0x7f)
#define KANJIP(X) (!(HIRAP(X) || KATAP(X) || ASCIIP(X)))

#define YOMICHAR(X) ((HIRAP(X)) || \
		     ('0'<=(X)&&'9'>=(X)) || \
		     ('A'<=(X)&&'Z'>=(X)) || \
		     ('a'<=(X)&&'z'>=(X)) || \
		     (BAR == X) \
		    )
#define HIRA_OF(X) ((KATAP(X) && !(BAR == (X)))? ((X) & ~0x0100) : (X))

#define LENGTHYOMI      256	/* jisho ni touroku suru yomi no nagasa */
#define LENGTHKANJI     256	/* jisho ni touroku suru kanji no nagasa */
#define LENGTHBUNSETSU  264    /* 文節の最大長 */
#define LENGTHCONV      512	/* 変換可能最大文字数 */

#define JISHOKOSUU     20	

/* システム辞書の最初の4バイト */
/* #define MAGIC_SD (('s' << 24) | ('d' << 16) ) */
/* ユーザー辞書の最初の4バイト */
/* #define MAGIC_UD (('u' << 24) | ('d' << 16) ) */

#define DIC_RDONLY 1   /* 辞書がリード・オンリーである。*/


/* 複数のファイルにまたがって用いられているバッファサイズの定義 */
#define EXPAND_PATH_LENGTH 256 /* expand_expr()が用いるバッファのサイズ */


#define WNN_FILE_STRING "Ｗｎｎのファイル"

#define WNN_FILE_STRING_LEN 16


#define F_STAMP_NUM 64
#define FILE_ALREADY_READ -2
#define FILE_NOT_READ -3

/*	file ID	*/
/*
  Local Variables:
  kanji-flag: t
  End:
*/

