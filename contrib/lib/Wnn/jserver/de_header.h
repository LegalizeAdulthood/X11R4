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

/* ����ʸ������򤷤����� */

struct  GIJI {				/* ADD */
	int	eisuu;	/* Ⱦ��/���ѱѿ��� */
	int	number;	/* Ⱦ��/����/������ */
	int	kigou;	/* Ⱦ��/����(JIS/ASCII)���� */
};

struct cnv_env {
	char	env_name[WNN_ENVNAME_LEN];
	int	ref_count;
	int	sticky;		/* ���饤����Ȥ����ʤ��ʤäƤ⤽�δĶ���Ĥ� */
	int	nbun;		/* ��(��)ʸ����ϤΣ� */
	int	nshobun;	/* ��ʸ����ξ�ʸ��κ���� ADD KURI */
	int	hindoval;	/* ��������٤Υѥ�᡼�� */
	int	lenval;		/* ��ʸ��Ĺ�Υѥ�᡼�� */
	int	jirival;	/* ����Ĺ�Υѥ�᡼�� */
	int	flagval;	/* ���Ȥä���bit�Υѥ�᡼�� */
	int	jishoval;	/* ����Υѥ�᡼�� */
	int	sbn_val;	/* ��ʸ���ɾ���ͤΥѥ�᡼�� */
	int	dbn_len_val;	/* ��ʸ��Ĺ�Υѥ�᡼�� */
	int	sbn_cnt_val;	/* ��ʸ����Υѥ�᡼�� */

	int	suuji_val;	/* �����ʻ� ���������� */
	int	kana_val;	/* �����ʻ� ���ʤ����� */
	int	eisuu_val;	/* �����ʻ� �ѿ������� */
	int	kigou_val;	/* �����ʻ� ��������� */
	int	toji_kakko_val;	/* �����ʻ� �ĳ�̤����� */
	int	fuzokogo_val;	/* �����ʻ� ��°������� */
	int	kaikakko_val;	/* �����ʻ� ����̤����� */

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


