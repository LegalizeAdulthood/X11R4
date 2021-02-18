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
/***********************************************************************
			convert_key.c			 ������
						87/12/19�������
							 ������
	�����ॢ���Ƚ�����ȼ�����ե��󥯥���󥭡�����С��Ƚ�����
***********************************************************************/

/*	Version 4.0
 */

#include "cvt_head.h"
#include <signal.h>
#include <setjmp.h>

/*
	keyin1()��ƤӽФ�����ˤϡ����μ�³����ɬ�פǤ���

     �� tgetent()��4.2BSD�ˤ⤷����setupterm()��System V�ˤˤ�äơ�
	�����ܡ��ɤ�term�������äƤ��륨��ȥ���ɤ߹��ߤޤ���
     �� convert_getstrs() �ˤ�äơ��ɤ����������ѤΥ��ꥢ�˥��ԡ����ޤ���
	����ˤ�äơ�¾�Υ���ȥ��term������ɤ߹���Ǥ⡢������������
	�����ʤ��ʤ�ޤ���
     �� convert_key_setup()��Ƥӡ��Ѵ��ν������򤷤ޤ���

	convert_getterm()�ˤ�äơ����ȣ������٤ˤǤ��ޤ���
	�ޤ���convert_key_init()�ˤ�äƣ��ȣ�����
	convert_getterm_init()�ˤ�äƣ����飳�ޤǤ������줾����٤ˤǤ��ޤ���

	�ʤ��������� convert_ �ǻϤޤ�饤�֥��ؿ��ϡ�convert_read.c �ˤ�
	�������Ƥ��ޤ���

	4.2BSD�ξ��˵��ǤǸƤ�ɬ�פΤ��ä�getterm_fkeydata()�ϡ�
	��ɬ�פˤʤä������ѻߤ���ޤ�����
*/

static	jmp_buf env;

extern	struct	CONVCODE tbl[];
extern	int	cnv_tbl_cnt;	/* convert table count */

 /** �������Ѵ���ȼ���������ϴؿ� */
keyin1(get_ch)
int	(*get_ch)();	/* getchar() ��Ʊ�ͤδؿ� */
{
	int ret;

	for ( ; ; ) {
	    if(cnv_tbl_cnt == 0) {
		ret = (*get_ch)();
		if (ret >= 0) 
		    return(ret);
	    } else {
		return(convert_key(get_ch, tbl, cnv_tbl_cnt, 0));
	    }
	}
}

 /** int������ h ��ӥåȥ٥����Ȥߤʤ�����i�ӥåȤ�����å����뤤��Ω�Ƥ� */
#define BITONP(h, i) (h[i / BITSIZ] &  (1 << (i % BITSIZ)))
#define BITOFP(h, i) (!BITONP(h, i))
#define BIT_UP(h, i) (h[i / BITSIZ] |= (1 << (i % BITSIZ)))
#define BITDWN(h, i) (h[i / BITSIZ] &= ~(1 << (i % BITSIZ)))


 /** ���ꤵ�줿�Ѵ��ơ��֥�˽��äƥ������Ѵ����롣*/
convert_key(inkey, conv_tbl, tbl_cnt, matching_flg)
int	(*inkey)();		/* �������ϴؿ� */
struct	CONVCODE conv_tbl[];	/* �Ѵ��ơ��֥� */
int	tbl_cnt;		/* conv_tbl[] �θĿ� */
int	matching_flg;		/* �ޥå��󥰤��ʤ��ä����ȥ�󥰤ν�������
					0 : ���ͤȤ����֤�
					1 : ���Υ��ȥ�󥰤ϼΤƤ�	*/
{
#define MAX	20		/* �������ϥХåե��κ����� */

	static	int	inbuf[MAX];	/* �������ϥХåե� */
		/* �Хåե��ν�ü�ϡ�-1 �Ǽ�����롣*/

	int	out_cnt;		/* ���ϥХåե��ν��ϥ������ */

	static	int	buf_cnt = 0;	/* inbuf �����ϻ��Υ�����   */

	int	check_flg[CHANGE_MAX];
	 /* �ӥåȥ٥����Ȥ��ư���졢�ޥå��󥰻����оݤȤʤäƤ���conv_tbl[]
	    �򼨤���1�λ��оݤȤʤꡢ0�����о� */

	int	i, c, flg = 0;		/* work */

	for(i = 0; i < div_up(tbl_cnt, BITSIZ); check_flg[i++] = ~0);
     /* ����check_flg��ӥåȥ٥�����������������0��tbl_cnt �ӥåȤ�Ω�Ƥ롣
	â�����ºݤϤ��ξ�����ޤ�Ω�� */

	for(;;){
	    if(flg != 0 || buf_cnt == 0){
		if(buf_cnt > 0){
		    if ((inbuf[buf_cnt] = (*inkey)()) == -1) { /* ��ʸ������ */
			c = -2;	/* �����ॢ���� */
			goto LABEL;
		    } else {
			inbuf[++buf_cnt] = -1; /* �����ߥ͡��� */
		    }
		} else {
		    if ((inbuf[buf_cnt] = (*inkey)()) == -1) { /* ��ʸ������ */
			continue;
		    } else {
			inbuf[++buf_cnt] = -1; /* �����ߥ͡��� */
		    }
		}
	    }
	    flg++;

	    if(buf_cnt >= MAX - 1) return(-1);	/* ERROR */

	    c = key_check(inbuf, conv_tbl, tbl_cnt, check_flg);
LABEL:
	    switch(c){
		    case -1:	/* ̤���� */
			    continue;

		    case -2:	/* �Ѵ��оݤǤʤ����Ȥ����ꤷ�� */
			    buf_cnt--;
			    out_cnt = 0;
			    c = inbuf[out_cnt++];
			    for(i = 0; inbuf[i] != -1;
				inbuf[i++] = inbuf[out_cnt++]);
			    if(matching_flg != 0){
				    flg = 0;
				    continue;
			    }
			    return(c);

		    default:	/* �Ѵ����줿������ */
			    buf_cnt = 0;
			    return(c);
	    }
	}
}

 /** �Ѵ������ɤΥ����å��ȥ������Ѵ� */
key_check(inbuf, conv_tbl, tbl_cnt, check_flg)
int	inbuf[];		/* ���������ȥ�� */
struct	CONVCODE conv_tbl[];	/* �������Ѵ��ơ��֥� */
int	tbl_cnt;
int	check_flg[];
{
	register int	dist, base;
	register char	*code_p;
	int	i;

	for(base = 0; inbuf[base] != -1; base++){
		for(dist = 0; dist < tbl_cnt; dist++){
			if(BITONP(check_flg, dist) &&
			   conv_tbl[dist] . fromkey != 0){
				code_p = conv_tbl[dist] . fromkey + base;
				if(*code_p == (char)inbuf[base]){
					if(*(code_p + 1) == (char)0){
						 /* �ޥå����� */
						for(i = 0, base++;
						    (inbuf[i] = inbuf[base])
							!= -1;
						    i++, base++);
						return(conv_tbl[dist]. tokey);
					}
					/* �ޤ��ޥå����Ƥ��ʤ� */
				} else {
					BITDWN(check_flg, dist); /* ̵�� */
				}
			}
		}
	}

	 /* �ӥåȥ٥��� check_flg[] ����0��tblcnt�ӥåȤ�Ω�ä��ޤ޻ĤäƤ���
	    ��Τ����뤫Ĵ�٤롣*/
	for(i = 0; i < tbl_cnt / BITSIZ; i++){
		if(check_flg[i]) return(-1);
	}
	if((tbl_cnt %= BITSIZ) && (check_flg[i]& ~(~0 << tbl_cnt)))return(-1);
	 /* return -1 �� �ޤ�̤�����ʪ������ */

	return(-2);	/* �Ѵ��оݤȤʤ�ʪ�Ϥʤ� */
}
