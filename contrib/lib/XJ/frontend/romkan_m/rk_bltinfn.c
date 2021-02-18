/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OMRON not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  OMRON makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Original Source: ./lib/Wnn/romkan/rk_bltinfn.c
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

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
			rk_bltinfn.c
						87.12. 6  �� ��

	�Ѵ��Ѥ��Ȥ߹��ߴؿ��Τ���ʣ���ʤ�Τ�������Ƥ��롣
	���Ѣ���Ⱦ�Ѥ��Ѵ������Ρ�
***********************************************************************/

/*	Version 3.0
 */

#include "rk_header.h"

 /* Ⱦ��ʸ���Υ����ɤ�define */
#define HKCHOU	(HNKAK1 * 0x100 + 0xB0) /* �� */
#define HKDKTN	(HNKAK1 * 0x100 + 0xDE) /* �� */
#define HKHNDK	(HNKAK1 * 0x100 + 0xDF) /* �� */
#define HKMARU	(HNKAK1 * 0x100 + 0xA1) /* �� */
#define HKHRKG	(HNKAK1 * 0x100 + 0xA2) /* �� */
#define HKTJKG	(HNKAK1 * 0x100 + 0xA3) /* �� */
#define HKTTEN	(HNKAK1 * 0x100 + 0xA4) /* �� */
#define HKNKPT	(HNKAK1 * 0x100 + 0xA5) /* �� */

 /* ����ʸ���Υ����ɤ�define */
#define CHOUON	(0xA1BC) /* �� */
#define DAKUTN	(0xA1AB) /* �� */
#define HNDAKU	(0xA1AC) /* �� */
#define MNMARU	(0xA1A3) /* �� */ /* ̾���� MaNMARU�ʤޤ�ޤ�ˤ�ά */
#define HRKKAG	(0xA1D6) /* �� */
#define TJIKAG	(0xA1D7) /* �� */
#define TOUTEN	(0xA1A2) /* �� */
#define NKPOTU	(0xA1A6) /* �� */


 /** ASCIIʸ�������� */
letter	to_zenalpha(l)
letter	l;
{
	letter	retval;

	static	uns_chr *data = (uns_chr *)
	   "�����ɡ������ǡʡˡ��ܡ��ݡ������������������������������䡩\
�����£ãģţƣǣȣɣʣˣ̣ͣΣϣУѣңӣԣգ֣ףأ٣ڡΡ�ϡ�����������\
�����������������������������Сáѡ�";

	if(' ' <= l && l <= '~') {
		l = (l - ' ') << 1;
		retval = data[l] << 8;
		retval += data[++l];
		return(retval);
	} else return(l);
}

static	char	*hankdata[] = {
	"��","��","��","��","��","��","��","��","��","��",
	"��","����","��","����","��","����","��","����","��","����",
	"��","����","��","����","��","����","��","����","��","����",
	"��","����","��","����","��","��","�¨�","��","�è�","��","�Ĩ�",
	"��","��","��","��","��",
	"��","�ʨ�","�ʨ�","��","�˨�","�˨�","��","�̨�","�̨�",
	 "��","�ͨ�","�ͨ�","��","�Ψ�","�Ψ�",
	"��","��","��","��","��",
	"��","��","��","��","��","��",
	"��","��","��","��","��",
	"��","��","��","��","��","��",
	"����","��","��"
};  /* ���Ѥ������äƤ�Τ���� */

 /**	���hankdata�����ºݤ˻Ȥ�Ⱦ�ѥ����ɤ�ɽ���Ƥ��ʤ��Ȥ����ºݤΤ�Τ�
	�������롣���������˰������Ƥ� */
hank_setup()
{
	int	i;
	char	*s, orig_hnkak1;

	orig_hnkak1 = *hankdata[0];
 /*	*hankdata[] �Ǥ�Ⱦ��ʸ���Σ��Х��ȤᡣȾ��ʸ���Σ��Х��Ȥ�������ۤʤ�
	�褦��¾����˰ܿ�����Ȥ��ϡ�HNKAK1��define���Ѥ����OK��â��romkan��
	���������Ⱦ��ʸ���ʤ��Υե�����ˤΤ�¸�ߡˤ⥳��С��Ȥ��ơ����ε���
	�˹�碌��ۤ���˾�ޤ����������������ǥ����Ǥ��Υե��������������
	������ˡ�Ⱦ��ʸ���ΰ��������ޤ������ʤ���礬����Τǡ��ä�
	����С��Ȥ򤷤ʤ��Ȥ�ư���褦�˽��֤Ϥ��Ƥ��롣���줬������
	hank_setup()�Ǥ��롣hankdata�ϡ����������� hank_setup()�ˤ�ä�
	�ºݤ�Ⱦ�ѥ����ɤ�ľ����롣*/

	if(orig_hnkak1 == (char)HNKAK1) return;
	for(i = 0; i < numberof(hankdata); i++){
		for(s = hankdata[i]; *s; s += 2)
			if(*s == orig_hnkak1) *s = HNKAK1;
	}
}

 /** ���ʢ�Ⱦ�ѥ������ʡ���̤���ʸ���ˤʤ뤳�Ȥ⤢�롣*/
to_hankata(in, outp)
letter	in, **outp;
{
	uns_chr *p, c;
	letter	*out;

	out = *outp;
	switch(in){
		case CHOUON: *out++ = HKCHOU; break;
		case DAKUTN: *out++ = HKDKTN; break;
		case HNDAKU: *out++ = HKHNDK; break;
		case MNMARU: *out++ = HKMARU; break;
		case HRKKAG: *out++ = HKHRKG; break;
		case TJIKAG: *out++ = HKTJKG; break;
		case TOUTEN: *out++ = HKTTEN; break;
		case NKPOTU: *out++ = HKNKPT; break;
		default:
			if(is_kata(in)){
				for(p = (uns_chr *)hankdata[in - KATBGN];
				    c = *p; p++)
					*out++ = (c << 8) + *++p;
			} else 
			if(is_hira(in)){
				for(p = (uns_chr *)hankdata[in - HIRBGN];
				    c = *p; p++)
					*out++ = (c << 8) + *++p;
			} else {
				*out++ = in;
			}
	}
	*out = EOLTTR;
	*outp = out;
}

 /**	Ⱦ�ѥ������ʢ��Ҥ餬�ʡ�â�������������ʸ�����ĤˤޤȤ�Ƥ�
	����ʤ��Τ���ա�*/
letter	to_zenhira(l)
letter	l;
{
	letter	retval;

	static	uns_chr *data = (uns_chr *)
	   "���֡ס����򤡤�����������á���������������������������������\
���ĤƤȤʤˤ̤ͤΤϤҤդؤۤޤߤ������������󡫡�";

	if(is_hankata(l)){
		l = (l - HKKBGN) << 1;
		retval = data[l] << 8;
		retval += data[++l];
		return(retval);
	} else return(l);
}

 /**	Ⱦ�ѥ������ʢ����ѡ�â�������������ʸ�����ĤˤޤȤ�Ƥ�
	����ʤ��Τ���ա�*/
letter	to_zenkata(l)
letter	l;
{
	return(is_hankata(l) ? (l = to_zenhira(l), to_kata(l)) : l);
}

 /* �ӥåȥ٥����ι��� */
#define bitvec(b0, b1, b2, b3, b4, b5, b6, b7) (			 \
	b0 | (b1 << 1) | (b2 << 2) | (b3 << 3) | (b4 << 4) | (b5 << 5) | \
	(b6 << 6) | (b7 << 7)						 \
)

 /** char������ h ��ӥåȥ٥����ȸ��Ƥ�����i�ӥåȤ�����å����� */
#define bitlook(h, i) (h[(i) >> 3] & (1 << ((i) & 7)))

#define KATRPT	0xA1B3 /* �� */
#define HIRRPT	0xA1B5 /* �� */
#define KATA_U	0xA5A6 /* �� */
#define KAT_VU	0xA5F4 /* �� */
#define HIR_KA	0xA4AB /* �� */
#define HIR_HO	0xA4DB /* �� */
#define KAT_KA	0xA5AB /* �� */
#define KAT_HO	0xA5DB /* �� */
#define HIR_HA	0xA4CF /* �� */
#define KAT_HA	0xA5CF /* �� */

 /**	����Ⱦ�����򤯤äĤ��롣��̤ϰ�������ʸ����*/
handakuadd(in, outp)
letter	in, **outp;
{
	if((HIR_HA <= in && in <= HIR_HO) ? 0 == (in - HIR_HA) % 3 :
	   (KAT_HA <= in && in <= KAT_HO && 0 == (in - KAT_HA) % 3)){
		*(*outp)++ = in + 2;
	} else {
		*(*outp)++ = in;
		*(*outp)++ = HNDAKU;
	}
	**outp = EOLTTR;
}

 /**	���������򤯤äĤ��롣��̤ϰ�������ʸ����*/
dakuadd(in, outp)
letter	in, **outp;
{
	static	char	flgbit[] = {
		bitvec(1, 0, 1, 0, 1, 0, 1, 0), /* ���������������� */
		bitvec(1, 0, 1, 0, 1, 0, 1, 0), /* ���������������� */
		bitvec(1, 0, 1, 0, 1, 0, 1, 0), /* ���������������� */
		bitvec(0, 1, 0, 1, 0, 1, 0, 0), /* �äĤŤƤǤȤɤ� */
		bitvec(0, 0, 0, 0, 1, 0, 0, 1), /* �ˤ̤ͤΤϤФѤ� */
		bitvec(0, 0, 1, 0, 0, 1, 0, 0), /* �ӤԤդ֤פؤ٤� */
		bitvec(1, 0, 0, 0, 0, 0, 0, 0)	/* �� */
	};
	letter	c;

	if((HIR_KA <= in && in <= HIR_HO) ? (c = in - HIR_KA, 1) :
	   (KAT_KA <= in && in <= KAT_HO && (c = in - KAT_KA, 1))){
		if(bitlook(flgbit, c)){
			*(*outp)++ = in + 1;
		} else {
			*(*outp)++ = in;
			*(*outp)++ = DAKUTN;
		}
	} else switch(in){
		case KATRPT:
		case HIRRPT:
			*(*outp)++ = in + 1; break;
		case KATA_U:
			*(*outp)++ = KAT_VU; break;
		default:
			*(*outp)++ = in;
			*(*outp)++ = DAKUTN;
	}
	**outp = EOLTTR;
}

 /** in��Ϳ����줿�����ɤ�base�ʤο����ˤ���outp������롣*/
to_digit(in, base, outp)
letter	in, base, **outp;
{
	letter	c, vtol();

	if(c = in, c /= base) to_digit(c, base, outp);
	*(*outp)++ = vtol(in % base);
	**outp = EOLTTR;
}
