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
	int	s_ichi;		/*	���ʥХåե��ؤΥ��ե��å�	*/
	int	jl;		/*	��Ω���Ĺ��			*/
	int	fl;		/*	��°���Ĺ��			*/
	int	pl;		/*	�ǥ����ץ쥤����Ĺ��		*/
	int	jishono;	/*	��Ω��μ����ֹ�		*/
	int	serial;		/*	��Ω����ֹ�			*/
	wchar_t	*k_data;	/*	�����Хåե��ؤΥݥ���	*/
};
#define	KOUHO_ENT	struct	kouho_entry


struct	jikouho_entry {
	int	jl;		/*	��Ω���Ĺ��			*/
	int	jishono;	/*	��Ω��μ����ֹ�		*/
	int	serial;		/*	��Ω����ֹ�			*/
	wchar_t	*k_data;	/*	�����Хåե��ؤΥݥ���	*/
};
#define	JIKOUHO_ENT	struct	jikouho_entry


struct	bunjoho {
	wchar_t	*kana_buf;	/*	���ʥХåե�			*/
	int	kana_size;	/*	���ʥХåե���Ĺ��		*/
	KOUHO_ENT *klist;	/*	����ꥹ�ȡ��Хåե�		*/
	int	klist_size;	/*	����ꥹ�ȡ��Хåե���Ĺ��	*/
	wchar_t	*kanji_buf;	/*	�����Хåե�			*/
	int	kanji_buf_size;	/*	�����Хåե� ��Ĺ��		*/
};


struct	jikouhojoho{
	JIKOUHO_ENT *jlist;	/*	������Хåե�			*/
	int	jlist_size;	/*	������Хåե���Ĺ��		*/
	wchar_t	*kanji_buf;	/*	�����Хåե�			*/
	int	kanji_buf_size;	/*	�����Хåե���Ĺ��		*/
};
#define	JIKOUHOJOHO		struct	jikouhojoho


struct	jishojoho{
	wchar_t	*k_data;	/*	�����Хåե��ؤΥݥ���	*/
	int	bumpo;		/*	ʸˡ����			*/
	int	hindo;		/*	����				*/
	int	jisho;		/*	�����ֹ�			*/
	int	serial;		/*	�����ֹ�			*/
};
#define	JISHOJOHO	struct	jishojoho


struct dicinfo{
	int	dic_no;		/*	�����ֹ�			*/
	int	dic_size;	/*	ñ���				*/
	int	ttl_hindo;	/*	�������			*/
	int	dic_type;	/*	1:�桼��,2:�����ƥ�		*/
	int	udp;		/*	0:������Ǥʤ�,1:������		*/
	int	prio;		/*	����ץ饤����ƥ�		*/
	int	rdonly;		/*	0:�饤�Ȳ�ǽ,1:�꡼�ɥ���꡼	*/
	char	*file_name;	/*	����ե�����̾			*/
	char	*h_file_name;	/*	���٥ե�����̾			*/
};
#define	DICINFO	struct	dicinfo

#define	wchar	wchar_t

extern	jmp_buf	jd_server_dead_env;	/* �����Ф�������������Ǥ���env */
extern	int	jd_server_dead_env_flg; /* jd_server_dead_env��ͭ�����ݤ�  */
extern	int	wnn_errorno;		/* Wnn�Υ��顼�Ϥ����ѿ�����𤵤�� */

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
