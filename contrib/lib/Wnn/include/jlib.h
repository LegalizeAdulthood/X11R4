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
/*	Version 3.0
 */
/*
	Nihongo	Henkan	Library Header File

	08/Jul/1987	(wnn_errorno)
	24/Apr/1987
	09/Mar/1987	(update server_dead)
	25/Feb/1987	(update wchar_t)
	18/Feb/1987	(update Comment)
	28/Nov/1986
	23/Oct/1986
*/
#include <setjmp.h>

#ifdef SX8870
#include <wchar.h>
#endif
#ifndef NEC
#ifndef WCHAR_T
typedef unsigned short	wchar_t;
#define	WCHAR_T
#endif
#endif /* NEC */

#define JLIB

#define	S_BUF_SIZ	1024	/* NEVER change this */
#define	R_BUF_SIZ	1024	/* NEVER change this */



struct	kouho_entry {
	int	s_ichi;		/*	かなバッファへのオフセット	*/
	int	jl;		/*	自立語の長さ			*/
	int	fl;		/*	付属語の長さ			*/
	int	pl;		/*	ディスプレイ時の長さ		*/
	int	jishono;	/*	自立語の辞書番号		*/
	int	serial;		/*	自立語の番号			*/
	wchar_t	*k_data;	/*	漢字バッファへのポインタ	*/
};
#define	KOUHO_ENT	struct	kouho_entry


struct	jikouho_entry {
	int	jl;		/*	自立語の長さ			*/
	int	jishono;	/*	自立語の辞書番号		*/
	int	serial;		/*	自立語の番号			*/
	wchar_t	*k_data;	/*	漢字バッファへのポインタ	*/
};
#define	JIKOUHO_ENT	struct	jikouho_entry


struct	bunjoho {
	wchar_t	*kana_buf;	/*	かなバッファ			*/
	int	kana_size;	/*	かなバッファの長さ		*/
	KOUHO_ENT *klist;	/*	候補リスト・バッファ		*/
	int	klist_size;	/*	候補リスト・バッファの長さ	*/
	wchar_t	*kanji_buf;	/*	漢字バッファ			*/
	int	kanji_buf_size;	/*	漢字バッファ の長さ		*/
};


struct	jikouhojoho{
	JIKOUHO_ENT *jlist;	/*	次候補バッファ			*/
	int	jlist_size;	/*	次候補バッファの長さ		*/
	wchar_t	*kanji_buf;	/*	漢字バッファ			*/
	int	kanji_buf_size;	/*	漢字バッファの長さ		*/
};
#define	JIKOUHOJOHO		struct	jikouhojoho


struct	jishojoho{
	wchar_t	*k_data;	/*	漢字バッファへのポインタ	*/
	int	bumpo;		/*	文法情報			*/
	int	hindo;		/*	頻度				*/
	int	jisho;		/*	辞書番号			*/
	int	serial;		/*	項目番号			*/
};
#define	JISHOJOHO	struct	jishojoho


struct dicinfo{
	int	dic_no;		/*	辞書番号			*/
	int	dic_size;	/*	単語数				*/
	int	ttl_hindo;	/*	頻度総数			*/
	int	dic_type;	/*	1:ユーザ,2:システム		*/
	int	udp;		/*	0:使用中でない,1:使用中		*/
	int	prio;		/*	辞書プライオリティ		*/
	int	rdonly;		/*	0:ライト可能,1:リードオンリー	*/
	char	*file_name;	/*	辞書ファイル名			*/
	char	*h_file_name;	/*	頻度ファイル名			*/
};
#define	DICINFO	struct	dicinfo

#define	wchar	wchar_t

extern	jmp_buf	jd_server_dead_env;	/* サーバが死んだ時に飛んでいくenv */
extern	int	jd_server_dead_env_flg; /* jd_server_dead_envが有効か否か  */
extern	int	wnn_errorno;		/* Wnnのエラーはこの変数に報告される */

/* header file for dic_syurui */
#define NO_EXIST -1
#define NOT_A_JISHO 0
#define USER_DIC 1
#define SYSTEM_DIC 3

struct jwho {
    int sd;   /* socket discripter in jserver */
    char *user_name;   /* user name */
    char *host_name;   /* host name */
};

#define JWHO struct jwho

/*
  Local Variables:
  kanji-flag: t
  End:
*/
