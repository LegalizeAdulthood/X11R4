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
	Nihongo	Henkan	Library Header File
*/

#ifndef JLLIB
#ifndef JSLIB
#include "jslib.h"
#endif

#define JLLIB

/*
   �桼�����ץ����ϡ�ľ�ܤ��ι�¤�Τ����Ƥ򥢥���������ɬ�פ�̵���Ϥ���
   �饤�֥��ؿ����Ѥ��뤳�ȡ�
*/

#define WNN_YOMI_SIZE 10

typedef struct wnn_jl_bun WNN_BUN;

struct wnn_jl_bun {
	int	jirilen;	/* ����ʸ��μ�Ω�� */
	int	dic_no;
	int	entry;
	int	kangovect;	/* ��³�٥��ȥ�ơ��֥�ؤΥݥ��� */
	int	hinsi;		/* �ʻ� */
	int	hindo  :16;		/* ����(������) */
	int	ref_cnt :4;		/* ʸ��ꥹ�Ȥ˻Ȥ��Ƥ��� */
	/* BUG FIX signed --> unsigned */
	unsigned int ima :1;		/* ���Ȥä���ӥå� */
	unsigned int hindo_updated :1;    /* ����ʸ������٤Ϲ�������Ƥ��� */
	unsigned int nobi_top :1;	/* ���Ф��̤ߤ�Ԥä����򼨤���*/
	/* egg �ߤ����˿��Ф��̤ߤ򷫤��֤��Ԥ����ˡ�����˸�������κ�
	   �Ȥä��� �ӥåȤ� ��Ȥ��ƤϤʤ�ʤ���*/
	unsigned int dai_top :1;	/* ��ʸ�����Ƭ */
	unsigned int dai_end :1; /* ��ʸ���saigo *//* ������ nomi */
	unsigned int from_zenkouho :2;	/*�����䤫�����򤵤줿ʸ�ᤫ�ɤ��� */
	unsigned int bug :1;			/* BUG FIX �Ȥꤢ���� */
	/* BUG FIX signed --> unsigned */
	int 	hyoka;
	int 	daihyoka;
	short yomilen;
	short kanjilen;
	WNN_BUN *down; /* ���Ȥä���ӥåȤ���Ȥ��оݤ�ʸ�� */
	wchar   yomi[WNN_YOMI_SIZE];        /* �ɤߡ����������롣�����ڤ�ʤ����ˤ�
				    ���ˤĤʤ��� */
	WNN_BUN *next;		/* ���Υ��ȥ饯�ȤؤΥݥ��� */
	WNN_BUN *free_next;		/* ���Υ��ȥ饯�ȤؤΥݥ��� */
};
    

struct wnn_buf {
    struct wnn_env *env;	/* �Ķ� */
    int bun_suu;		/* ʸ��� */
    int zenkouho_suu;		/* ������ο� */
    WNN_BUN **bun;		/* ʸ��ؤΥݥ��� */
    WNN_BUN **down_bnst; /* ���Ȥä���ӥåȤ���Ȥ��оݤ�ʸ�� */

    WNN_BUN **zenkouho;		/* ������ؤΥݥ��� */
    int *zenkouho_dai;		/* daibunsetsu zenkouho */
    int zenkouho_dai_suu;	/* daibunsetsu zenkouho suu */

    short c_zenkouho;		/* �������桢���ܤ��Ƥ������ */
    short zenkouho_daip;
    int zenkouho_bun;		/* ��������äƤ���ʸ�� */
    int zenkouho_end_bun;	/* ������(dai)���äƤ���Ǹ��ʸ�� */
    int zenkouho_endvect;	/* ��������ä� endvect *//* ADD KURI */

    WNN_BUN *free_heap;
    char *heap;		/* linked list of alloced area */
    int msize_bun;
    int msize_zenkouho;
};


extern struct wnn_buf *jl_open();
extern struct wnn_env *jl_connect();
extern struct wnn_env *jl_env_get();
extern struct wnn_jdata *jl_word_info_e();
extern wchar *jl_hinsi_name_e();


#define WNN_SHO 0
#define WNN_DAI 1

#define WNN_NO_USE    0
#define WNN_USE_MAE   1
#define WNN_USE_ATO   2
#define WNN_USE_ZENGO (WNN_USE_MAE | WNN_USE_ATO)
/* #define WNN_ZENGO_YUUSEN 4 */

#define WNN_UNIQ_KNJ 2
#define WNN_UNIQ 1
#define WNN_NO_UNIQ  0

#define WNN_NO_CREATE 0
#define WNN_CREATE (-1)

#define WNN_DIC_PRIO_DEFAULT 5

#define WNN_YOMI 0
#define WNN_KANJI 1

#define jl_next(buf) jl_set_jikouho((buf), (buf)->c_zenkouho + 1)

#define jl_previous(buf) jl_set_jikouho((buf), (buf)->c_zenkouho - 1)

#define jl_next_dai(buf) jl_set_jikouho_dai((buf), (buf)->c_zenkouho + 1)

#define jl_previous_dai(buf) jl_set_jikouho_dai((buf), (buf)->c_zenkouho - 1)

#define jl_get_kanji(buf, bun_no, bun_no2, area)\
                   wnn_get_area(buf, bun_no, bun_no2, area, WNN_KANJI)

#define jl_get_yomi(buf, bun_no, bun_no2, area)\
                   wnn_get_area(buf, bun_no, bun_no2, area, WNN_YOMI)

#define jl_bun_suu(buf) ((buf)->bun_suu)

#define jl_zenkouho_suu(buf) ((buf)->zenkouho_daip? (buf)->zenkouho_dai_suu:(buf)->zenkouho_suu)

#define jl_zenkouho_bun(buf) ((buf)->zenkouho_bun)

#define jl_c_zenkouho(buf) ((buf)->c_zenkouho)

#define jl_zenkouho_daip(buf) ((buf)->zenkouho_daip)

#define jl_dai_top(buf, k) ((buf)->bun[k]->dai_top)

#define jl_jiri_len(buf, k) ((buf)->bun[k]->jirilen)

/* ADD KURI */
#define jl_fuzoku_len(buf, k) \
		(jl_yomi_len((buf), k, k+1) - jl_jiri_len((buf), k))

#define jl_jiri_kanji_len(buf, k) \
		(jl_kanji_len((buf), k, k+1) - jl_fuzoku_len((buf), k))
/* ADD KURI END */

/* Macros to create library functions from "_e" libraries */

#define jl_env(buf) 	((buf)->env)

#define jl_dic_add(buf,dic_name,hindo_name,rev, prio,rw, hrw, pwd_dic, pwd_hindo, error_handler, message_handler)\
    jl_dic_add_e(jl_env(buf),dic_name,hindo_name,rev,prio,rw, hrw, pwd_dic,\
		      pwd_hindo, error_handler, message_handler)

#define jl_fuzokugo_set(buf, fname)  jl_fuzokugo_set_e(jl_env(buf), fname) 

#define jl_fuzokugo_get(buf, fname)  jl_fuzokugo_get_e(jl_env(buf), fname) 


#define jl_dic_save(buf,dic_no) jl_dic_save_e(jl_env(buf),dic_no)

#define jl_dic_save_all(buf)	jl_dic_save_all_e(jl_env(buf))

#define jl_dic_list(buf, dip)	jl_dic_list_e(jl_env(buf), dip)

#define jl_word_search(buf,dic_no, yomi, jdp) \
    jl_word_search_e(jl_env(buf),dic_no, yomi, jdp)

#define jl_word_search_by_env(buf, yomi, jdp) \
    jl_word_search_by_env_e(jl_env(buf), yomi, jdp)

#define jl_word_use(buf, dic_no, entry) \
    jl_word_use_e(jl_env(buf), dic_no, entry)


#define jl_param_set(buf, param) jl_param_set_e(jl_env(buf), param)

#define jl_param_get(buf, param) jl_param_get_e(jl_env(buf), param)

#define jl_dic_delete(buf, dic_no) jl_dic_delete_e(jl_env(buf), dic_no)

#define jl_dic_use(buf, dic_no, flag) jl_dic_use_e(jl_env(buf), dic_no, flag)

#define jl_word_add(buf, dic, yomi, kanji, com, hinsi, hindo) \
    jl_word_add_e(jl_env(buf), dic, yomi, kanji, com, hinsi, hindo)

#define jl_word_delete(buf, dic, entry)\
    jl_word_delete_e(jl_env(buf), dic, entry)

#define jl_word_info(buf, dic_no, entry)\
    jl_word_info_e(jl_env(buf), dic_no, entry)

#define jl_inspect(buf, bun_no)\
    jl_word_info(buf, (buf)->bun[bun_no]->dic_no,(buf)->bun[bun_no]->entry)

#define jl_hinsi_number(buf, name) jl_hinsi_number_e(jl_env(buf), name)

#define jl_hinsi_name(buf, no) jl_hinsi_name_e(jl_env(buf), no)

#define jl_word_comment_set(buf, dic, entry, comm) \
    jl_word_comment_set_e((buf)->env, dic, entry, comm)

#define jl_dic_comment_set(buf, dic, comm) \
    jl_dic_comment_set_e((buf)->env, dic, comm)

#define jl_hinsi_list(buf, dic_no, name, area) \
    jl_hinsi_list_e(jl_env(buf), dic_no, name, area)

#define jl_hinsi_dicts(buf, no, area) \
    jl_hinsi_dicts_e(jl_env(buf), no, area)

#ifdef nodef  /*���̤δ֡����٥ե�����Υ����Ȥϥ桼���˸����ʤ���*/
    #define jl_hindo_comment_set(buf, dic, comm) \
        jl_hindo_comment_set_e((buf)->env, dic, comm)
#endif

#define jl_isconnect(buf)	jl_isconnect_e((buf)->env)

#define jl_env_sticky(buf)	js_env_sticky((buf)->env)
#define jl_env_sticky_e(env)	js_env_sticky(env)
#define jl_env_un_sticky(buf)	js_env_un_sticky((buf)->env)
#define jl_env_un_sticky_e(env)	js_env_un_sticky(env)

#endif	JLLIB
