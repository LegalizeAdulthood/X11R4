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
/*
	de_header.h
*/

#define	DEMON

#include "jslib.h"

#define FILENAME 128

#define CL_MAX 32
#define	ST_MAX 10
#define	MAX_ENV		256
#define	MAX_FILES	300
#define	MAX_DIC		300

/* constants for one client */
#define USER_NAME_LEN 20
#define HOST_NAME_LEN 20
/* constants for one env. */
#define	FILE_NAME_L	100

/* value of file.localf */
#define	LOCAL	1
#define	REMOTE	0


/*
	Client
*/
struct cli{
	char user_name[USER_NAME_LEN];
	char host_name[HOST_NAME_LEN];
	int  env[WNN_MAX_ENV_OF_A_CLIENT];
};

typedef struct cli	CLIENT;

/*
	Environment for Conversion
 */
/*	jisho Table for an Environment		*/

/* 疑似文節の選択した候補 */

struct  GIJI {				/* ADD */
	int	eisuu;	/* 半角/全角英数字 */
	int	number;	/* 半角/全角/漢数字 */
	int	kigou;	/* 半角/全角(JIS/ASCII)記号 */
};

struct cnv_env {
	char	env_name[WNN_ENVNAME_LEN];
	int	ref_count;
	int	sticky;		/* クライアントがいなくなってもその環境を残す */
	int	nbun;		/* Ｎ(大)文節解析のＮ */
	int	nshobun;	/* 大文節中の小文節の最大数 ADD KURI */
	int	hindoval;	/* 幹語の頻度のパラメータ */
	int	lenval;		/* 小文節長のパラメータ */
	int	jirival;	/* 幹語長のパラメータ */
	int	flagval;	/* 今使ったよbitのパラメータ */
	int	jishoval;	/* 辞書のパラメータ */
	int	sbn_val;	/* 小文節の評価値のパラメータ */
	int	dbn_len_val;	/* 大文節長のパラメータ */
	int	sbn_cnt_val;	/* 小文節数のパラメータ */

	int	suuji_val;	/* 疑似品詞 数字の頻度 */
	int	kana_val;	/* 疑似品詞 カナの頻度 */
	int	eisuu_val;	/* 疑似品詞 英数の頻度 */
	int	kigou_val;	/* 疑似品詞 記号の頻度 */
	int	toji_kakko_val;	/* 疑似品詞 閉括弧の頻度 */
	int	fuzokogo_val;	/* 疑似品詞 付属語の頻度 */
	int	kaikakko_val;	/* 疑似品詞 開括弧の頻度 */

	struct	GIJI	giji;	/* ADD */

	int	fzk_fid;

	int jishomax;
	int	jisho[WNN_MAX_JISHO_OF_AN_ENV];
	int	file[WNN_MAX_FILE_OF_AN_ENV];
};


/*
	structure of Dictionary
 */

struct wnn_dic {
	int	body;	/* fid */
	int	hindo;	/* fid */
	int	rw;
	int	hindo_rw;
	int	enablef;
	int	nice;
	int     rev;		/* reverse dict */
};

/*
	structure of File
 */

struct wnn_file {
	char	name[FILE_NAME_L];
	struct wnn_file_uniq f_uniq;
	struct wnn_file_uniq f_uniq_org;
	int	localf;
	int	file_type;
	int	ref_count;
	char	*area;
	char passwd[WNN_PASSWD_LEN];
};


/*
	external variables of demon
*/

extern CLIENT *client;	/* ask about Mr. Takeoka */
/* extern CLIENT client[];	/* ask about Mr. Takeoka */
extern int max_client;
extern CLIENT *c_c;		/* this means current client */ 
extern int cur_client;
extern int clientp;

/*	env,dic,file	*/
extern struct cnv_env *c_env; 

extern	struct	cnv_env	*env[];
extern	struct	wnn_file	files[];
extern struct	wnn_dic	dic_table[];

extern int max_sticky_env;	/* ADD KURI */
extern struct cnv_env *s_env[];	/* ADD KURI */

extern int wnn_errorno;

extern int noisy;

extern char jserver_dir[];
extern char jserverrcfile[];

extern char SER_VERSION[];

extern char *hinsi_file_name;


