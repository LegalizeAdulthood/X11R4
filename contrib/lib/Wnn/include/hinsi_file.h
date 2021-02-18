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
/*    品詞ファイルに関する、定義
*/
     

#define MAXHINSI 	256
#define MAXFUKUGOU 	256
#define MAXNODE 	256
#define FUKUGOU_START 	0xffff - RESERVE_FUKUGOU_USIRO
#define HEAP_LEN	(10 * (MAXHINSI + MAXFUKUGOU))
/* 10 bytes for each hinsi */
#define WHEAP_LEN	(10 * MAXFUKUGOU * sizeof(short))
/* 10 bytes for each fukugou hinsi */


#define RESERVE_FUKUGOU_USIRO 512   /* fukugou hinsi no usiro ni 
				       koredake reserve wo site oku */
/* hitoru ha delete sareta to iu jouhou no 
   ta me*/
#define TERMINATE 0xffff
#define ERR -2
/* #define HINSIDATA_FILE "/usr/local/lib/dic/fuzokugo/hinsi.data"*/

#define WNN_NODE_SUFFIX '/'
#define NODE_CHAR '|'
#define COMMENT_CHAR ';'
#define DEVIDE_CHAR '$'
#define HINSI_SEPARATE_CHAR ':'
#define IGNORE_CHAR1 ' '
#define IGNORE_CHAR2 '\t'
#define YOYAKU_CHAR '@'
#define CONTINUE_CHAR '\\'

struct wnn_fukugou{
    wchar *name;	/* 複合品詞の名前 */
/* fukugou hinsi is held as a string */
    unsigned short *component;	/* 複合品詞の構成要素となる品詞の番号の配列
				   (最後は 0xffff で終る)
				 */
};

struct wnn_hinsi_node{
    wchar *name;	/* 品詞ノードの名前 */
    int kosuu;			/* 子どもの数 */
    wchar *son;	/* 最初の子どもへのポインタ */
};
