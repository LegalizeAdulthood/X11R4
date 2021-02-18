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
/*********************
 *  header of daemon 
**********************/
#define	_MININT	0x80000000
#define	HIRA_HYOUKA -2000;	/* ��������Ф��λ���ʿ��̾��ɾ���� */
#define	KATA_HYOUKA -1000;	/* ��������Ф��λ����Ҳ�̾��ɾ���� */

#define	_DIVID(val, div)	(((val) > 0) ? ((val) / (div)) : ((val) * (div)))

extern struct fzkkouho *getfzkoh_body();
extern struct fzkkouho *getfzkoh1_body();

#define	getfzkoh(ichbnp_p,no) \
	(((no) < FZKIBNO) ? &((ichbnp_p)->fzkib[no]) : getfzkoh_body(ichbnp_p,no))

#define	getfzkoh1(ichbnp_p,no) \
	(((no) < FZKIBNO) ? &((ichbnp_p)->fzkib1[no]) : getfzkoh1_body(ichbnp_p,no))

#define kan_ck_vector(h, v) \
    ((((*((v) + ((h) / (sizeof(int)<<3))) >> ((h) % (sizeof(int)<<3))) & 1) == 1) ? \
	WNN_CONNECT_BK : WNN_NOT_CONNECT_BK)

extern	struct	FT	*ft;		/* �����Ȥ���°��ơ��֥� */

/* ���ϥ��ꥢ�δ����ѤΥꥹ�� */
struct	free_list {
	struct	free_list *lnk;
	int	num;
};

#define	FIRST_BZD_KOSUU	200		/* �ǽ�˼��Ф���ʸ����ϥ��ꥢ */
#define	BZD_KOSUU	20 		/* ­��󤫤ä����դ��ä����礭�� */

#define FIRST_SBN_KOSUU 200		/* �ǽ�˼��Ф���ʸ����ϥ��ꥢ */
#define SBN_KOSUU	20		/* ­��󤫤ä����դ��ä����礭�� */

#define	FIRST_ICHBN_KOSUU	20	/* �ǽ�˼��Ф���°����ϥ��ꥢ */
#define	ICHBN_KOSUU	4 		/* ­��󤫤ä����դ��ä����礭�� */

#define	FIRST_JKDBN_KOSUU	50	/* �ǽ�˼��Ф���ʸ�ἡ���䥨�ꥢ */
#define	JKDBN_KOSUU	10 		/* ­��󤫤ä����դ��ä����礭�� */

#define	FIRST_JKSBN_KOSUU	50	/* �ǽ�˼��Ф���ʸ�ἡ���䥨�ꥢ */
#define	JKSBN_KOSUU	10 		/* ­��󤫤ä����դ��ä����礭�� */

#define	FIRST_JKSONE_KOSUU	300	/* �ǽ�˼��Ф���ʸ�ἡ���䥨�ꥢ */
#define	JKSONE_KOSUU	20 		/* ­��󤫤ä����դ��ä����礭�� */

/************************************************
 *	����ʸ�����ɾ���롼����		*
 ************************************************/
/*
#define	HYOKAVAL(jiritugo_len, bnst_len) \
		hyoka1(0x80, 0, (jiritugo_len), (bnst_len), 0)
*/
/*
#define	HYOKAVAL(hindo, jiritugo_len, bnst_len) \
		hyoka1(0, hindo, (jiritugo_len), (bnst_len), 0)
*/
#define	HYOKAVAL(hindo, jiritugo_len, bnst_len) \
  (c_env->hindoval * (hindo) \
   + c_env->jirival * (jiritugo_len) \
   + c_env->lenval * (bnst_len) )

/************************************************
 *	��ʸ�����ɾ���롼����			*
 ************************************************/
#define hyoka1(HINDO, HINDO_IN, LENGTH, BUNLNGTH, PRIO) \
  (c_env->hindoval * (((HINDO) & 0x7f) + ((HINDO_IN) & 0x7f)) \
   + c_env->jirival * (LENGTH) \
   + c_env->lenval * (BUNLNGTH) \
   + c_env->jishoval * (PRIO)\
   + c_env->flagval * (((HINDO) & 0x80) >> 7))

/************************************************
 *	��ʸ�����ɾ���롼����			*
 ************************************************/
/* ��ʸ���ɾ����
	sbn_val * ��ʸ���ɾ���ͤ��� / ��ʸ���
	   + sbn_cnt_val * ��ʸ���
	   + dbn_len_val * ��ʸ��Ĺ
*/
#define hyoka_dbn(SON_V, SBNCNT, DAILEN) \
	((c_env->sbn_val * (SON_V)) / (SBNCNT) \
	 + c_env->sbn_cnt_val * (SBNCNT) \
	 + c_env->dbn_len_val * (DAILEN))

/* ��ʸ���ɾ���ͤ�����ʸ�����³�Ǥ��뤫�ݤ����Ѥ��롣
   divid :  1	����ʸ�����³�Ǥ���	hyouka * 2
	    0	��ʸ�����Ƭ		hyouka
	   -1	����ʸ�����³�Ǥ��ʤ�	hyouka / 2
*/
#define	DIVID_HYOUKA(hyouka, divid) \
	(((divid) == 0) ? (hyouka) : \
		(((divid)) > 0 ? ((hyouka) << 1) : ((hyouka) >> 1)))

/*********************
 * kaiseki work area
**********************/
extern	wchar_t	*bun;	/* D */

extern	wchar_t giji_eisuu[];	/* �����ֱѿ��פ���� */

extern	int	maxchg;
extern	int	initjmt;	/* I think initjmt is the length of jmt_ */

extern	int	*maxj;	/* maxj is counts to entries in jmt_ */

/* jmt_ptr is used in jmt0.c only.
   but it must be changed by clients.
   jishobiki does not use it. this is sent to jishobiki by arguments
   */

/* j_e_p is used to hold the current point to which jmtw_ is used */
extern	struct	jdata	*j_e_p;
extern	struct	jdata	**jmt_;
extern	struct	jdata	*jmtw_;
extern	struct	jdata	**jmt_end;
extern	struct	jdata	*jmtw_end;
extern	struct	jdata	**jmt_ptr;
extern	struct	jdata	***jmtp;

/********************************
 *	������Ω����ʻ�	*
 ********************************/
extern	int	sentou_no;
extern	int	suuji_no;
extern	int	katakanago_no;
extern	int	eisuu_no;
extern	int	kigou_no;
extern	int	toji_kakko_no;
extern	int	fuzokugo_no;
extern	int	kai_kakko_no;
extern	int	giji_no;
