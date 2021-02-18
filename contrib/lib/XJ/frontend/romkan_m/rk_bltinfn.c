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
						87.12. 6  Äû Êä

	ÊÑ´¹ÍÑ¤ÎÁÈ¤ß¹þ¤ß´Ø¿ô¤Î¤¦¤ÁÊ£»¨¤Ê¤â¤Î¤òÄêµÁ¤·¤Æ¤¢¤ë¡£
	Á´³Ñ¢«¢ªÈ¾³Ñ¤ÎÊÑ´¹¤¬¼çÂÎ¡£
***********************************************************************/

/*	Version 3.0
 */

#include "rk_header.h"

 /* È¾³ÑÊ¸»ú¤Î¥³¡¼¥É¤Îdefine */
#define HKCHOU	(HNKAK1 * 0x100 + 0xB0) /* ¨° */
#define HKDKTN	(HNKAK1 * 0x100 + 0xDE) /* ¨Þ */
#define HKHNDK	(HNKAK1 * 0x100 + 0xDF) /* ¨ß */
#define HKMARU	(HNKAK1 * 0x100 + 0xA1) /* ¨¡ */
#define HKHRKG	(HNKAK1 * 0x100 + 0xA2) /* ¨¢ */
#define HKTJKG	(HNKAK1 * 0x100 + 0xA3) /* ¨£ */
#define HKTTEN	(HNKAK1 * 0x100 + 0xA4) /* ¨¤ */
#define HKNKPT	(HNKAK1 * 0x100 + 0xA5) /* ¨¥ */

 /* Á´³ÑÊ¸»ú¤Î¥³¡¼¥É¤Îdefine */
#define CHOUON	(0xA1BC) /* ¡¼ */
#define DAKUTN	(0xA1AB) /* ¡« */
#define HNDAKU	(0xA1AC) /* ¡¬ */
#define MNMARU	(0xA1A3) /* ¡£ */ /* Ì¾Á°¤Ï MaNMARU¡Ê¤Þ¤ó¤Þ¤ë¡Ë¤ÎÎ¬ */
#define HRKKAG	(0xA1D6) /* ¡Ö */
#define TJIKAG	(0xA1D7) /* ¡× */
#define TOUTEN	(0xA1A2) /* ¡¢ */
#define NKPOTU	(0xA1A6) /* ¡¦ */


 /** ASCIIÊ¸»ú¢ªÁ´³Ñ */
letter	to_zenalpha(l)
letter	l;
{
	letter	retval;

	static	uns_chr *data = (uns_chr *)
	   "¡¡¡ª¡É¡ô¡ð¡ó¡õ¡Ç¡Ê¡Ë¡ö¡Ü¡¤¡Ý¡¥¡¿£°£±£²£³£´£µ£¶£·£¸£¹¡§¡¨¡ã¡á¡ä¡©\
¡÷£Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï£Ð£Ñ£Ò£Ó£Ô£Õ£Ö£×£Ø£Ù£Ú¡Î¡ï¡Ï¡°¡²¡®£á£â£ã£ä£å\
£æ£ç£è£é£ê£ë£ì£í£î£ï£ð£ñ£ò£ó£ô£õ£ö£÷£ø£ù£ú¡Ð¡Ã¡Ñ¡±";

	if(' ' <= l && l <= '~') {
		l = (l - ' ') << 1;
		retval = data[l] << 8;
		retval += data[++l];
		return(retval);
	} else return(l);
}

static	char	*hankdata[] = {
	"¨§","¨±","¨¨","¨²","¨©","¨³","¨ª","¨´","¨«","¨µ",
	"¨¶","¨¶¨Þ","¨·","¨·¨Þ","¨¸","¨¸¨Þ","¨¹","¨¹¨Þ","¨º","¨º¨Þ",
	"¨»","¨»¨Þ","¨¼","¨¼¨Þ","¨½","¨½¨Þ","¨¾","¨¾¨Þ","¨¿","¨¿¨Þ",
	"¨À","¨À¨Þ","¨Á","¨Á¨Þ","¨¯","¨Â","¨Â¨Þ","¨Ã","¨Ã¨Þ","¨Ä","¨Ä¨Þ",
	"¨Å","¨Æ","¨Ç","¨È","¨É",
	"¨Ê","¨Ê¨Þ","¨Ê¨ß","¨Ë","¨Ë¨Þ","¨Ë¨ß","¨Ì","¨Ì¨Þ","¨Ì¨ß",
	 "¨Í","¨Í¨Þ","¨Í¨ß","¨Î","¨Î¨Þ","¨Î¨ß",
	"¨Ï","¨Ð","¨Ñ","¨Ò","¨Ó",
	"¨¬","¨Ô","¨­","¨Õ","¨®","¨Ö",
	"¨×","¨Ø","¨Ù","¨Ú","¨Û",
	"¥î","¨Ü","¥ð","¥ñ","¨¦","¨Ý",
	"¨³¨Þ","¥õ","¥ö"
};  /* Á´³Ñ¤¬º®¤¸¤Ã¤Æ¤ë¤Î¤ÇÃí°Õ */

 /**	¾å¤Îhankdata¤¬¡¢¼ÂºÝ¤Ë»È¤¦È¾³Ñ¥³¡¼¥É¤òÉ½¤·¤Æ¤¤¤Ê¤¤¤È¤­¡¢¼ÂºÝ¤Î¤â¤Î¤Ë
	½¤Àµ¤¹¤ë¡£½é´üÀßÄê»þ¤Ë°ì²ó¤À¤±¸Æ¤Ö */
hank_setup()
{
	int	i;
	char	*s, orig_hnkak1;

	orig_hnkak1 = *hankdata[0];
 /*	*hankdata[] ¤Ç¤ÎÈ¾³ÑÊ¸»ú¤Î£±¥Ð¥¤¥È¤á¡£È¾³ÑÊ¸»ú¤Î£±¥Ð¥¤¥È¤á¤À¤±¤¬°Û¤Ê¤ë
	¤è¤¦¤ÊÂ¾µ¡¼ï¤Ë°Ü¿¢¤¹¤ë¤È¤­¤Ï¡¢HNKAK1¤Îdefine¤òÊÑ¤¨¤ì¤ÐOK¡£Ã¢¤·romkan¤Î
	¥½¡¼¥¹Ãæ¤ÎÈ¾³ÑÊ¸»ú¡Ê¤³¤Î¥Õ¥¡¥¤¥ë¤Ë¤Î¤ßÂ¸ºß¡Ë¤â¥³¥ó¥Ð¡¼¥È¤·¤Æ¡¢¤½¤Îµ¡¼ï
	¤Ë¹ç¤ï¤»¤ë¤Û¤¦¤¬Ë¾¤Þ¤·¤¤¡£¤·¤«¤·¡¢¥¨¥Ç¥£¥¿¤Ç¤³¤Î¥Õ¥¡¥¤¥ë¤ò½¤Àµ¤·¤¿¤ê
	¤¹¤ë¾ì¹ç¤Ë¡¢È¾³ÑÊ¸»ú¤Î°·¤¤¤¬¤¦¤Þ¤¯¤¤¤«¤Ê¤¤¾ì¹ç¤¬¤¢¤ë¤Î¤Ç¡¢ÆÃ¤Ë
	¥³¥ó¥Ð¡¼¥È¤ò¤·¤Ê¤¯¤È¤âÆ°ºî¤¹¤ë¤è¤¦¤Ë½èÃÖ¤Ï¤·¤Æ¤¢¤ë¡£¤½¤ì¤¬¡¢¤³¤Î
	hank_setup()¤Ç¤¢¤ë¡£hankdata¤Ï¡¢½é´üÀßÄê»þ¤Ë hank_setup()¤Ë¤è¤Ã¤Æ
	¼ÂºÝ¤ÎÈ¾³Ñ¥³¡¼¥É¤ËÄ¾¤µ¤ì¤ë¡£*/

	if(orig_hnkak1 == (char)HNKAK1) return;
	for(i = 0; i < numberof(hankdata); i++){
		for(s = hankdata[i]; *s; s += 2)
			if(*s == orig_hnkak1) *s = HNKAK1;
	}
}

 /** ¤«¤Ê¢ªÈ¾³Ñ¥«¥¿¥«¥Ê¡£·ë²Ì¤¬ÆóÊ¸»ú¤Ë¤Ê¤ë¤³¤È¤â¤¢¤ë¡£*/
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

 /**	È¾³Ñ¥«¥¿¥«¥Ê¢ª¤Ò¤é¤¬¤Ê¡£Ã¢¤·¡¢ÂùÅÀ¤ò»ý¤ÄÊ¸»ú¤ò°ì¤Ä¤Ë¤Þ¤È¤á¤Æ¤Ï
	¤¯¤ì¤Ê¤¤¤Î¤ÇÃí°Õ¡£*/
letter	to_zenhira(l)
letter	l;
{
	letter	retval;

	static	uns_chr *data = (uns_chr *)
	   "¡£¡Ö¡×¡¢¡¦¤ò¤¡¤£¤¥¤§¤©¤ã¤å¤ç¤Ã¡¼¤¢¤¤¤¦¤¨¤ª¤«¤­¤¯¤±¤³¤µ¤·¤¹¤»¤½¤¿\
¤Á¤Ä¤Æ¤È¤Ê¤Ë¤Ì¤Í¤Î¤Ï¤Ò¤Õ¤Ø¤Û¤Þ¤ß¤à¤á¤â¤ä¤æ¤è¤é¤ê¤ë¤ì¤í¤ï¤ó¡«¡¬";

	if(is_hankata(l)){
		l = (l - HKKBGN) << 1;
		retval = data[l] << 8;
		retval += data[++l];
		return(retval);
	} else return(l);
}

 /**	È¾³Ñ¥«¥¿¥«¥Ê¢ªÁ´³Ñ¡£Ã¢¤·¡¢ÂùÅÀ¤ò»ý¤ÄÊ¸»ú¤ò°ì¤Ä¤Ë¤Þ¤È¤á¤Æ¤Ï
	¤¯¤ì¤Ê¤¤¤Î¤ÇÃí°Õ¡£*/
letter	to_zenkata(l)
letter	l;
{
	return(is_hankata(l) ? (l = to_zenhira(l), to_kata(l)) : l);
}

 /* ¥Ó¥Ã¥È¥Ù¥¯¥¿¤Î¹½À® */
#define bitvec(b0, b1, b2, b3, b4, b5, b6, b7) (			 \
	b0 | (b1 << 1) | (b2 << 2) | (b3 << 3) | (b4 << 4) | (b5 << 5) | \
	(b6 << 6) | (b7 << 7)						 \
)

 /** char¤ÎÇÛÎó h ¤ò¥Ó¥Ã¥È¥Ù¥¯¥¿¤È¸«¤Æ¤½¤ÎÂèi¥Ó¥Ã¥È¤ò¥Á¥§¥Ã¥¯¤¹¤ë */
#define bitlook(h, i) (h[(i) >> 3] & (1 << ((i) & 7)))

#define KATRPT	0xA1B3 /* ¡³ */
#define HIRRPT	0xA1B5 /* ¡µ */
#define KATA_U	0xA5A6 /* ¥¦ */
#define KAT_VU	0xA5F4 /* ¥ô */
#define HIR_KA	0xA4AB /* ¤« */
#define HIR_HO	0xA4DB /* ¤Û */
#define KAT_KA	0xA5AB /* ¥« */
#define KAT_HO	0xA5DB /* ¥Û */
#define HIR_HA	0xA4CF /* ¤Ï */
#define KAT_HA	0xA5CF /* ¥Ï */

 /**	¸å¤í¤ËÈ¾ÂùÅÀ¤ò¤¯¤Ã¤Ä¤±¤ë¡£·ë²Ì¤Ï°ìËô¤ÏÆóÊ¸»ú¡£*/
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

 /**	¸å¤í¤ËÂùÅÀ¤ò¤¯¤Ã¤Ä¤±¤ë¡£·ë²Ì¤Ï°ìËô¤ÏÆóÊ¸»ú¡£*/
dakuadd(in, outp)
letter	in, **outp;
{
	static	char	flgbit[] = {
		bitvec(1, 0, 1, 0, 1, 0, 1, 0), /* ¤«¤¬¤­¤®¤¯¤°¤±¤² */
		bitvec(1, 0, 1, 0, 1, 0, 1, 0), /* ¤³¤´¤µ¤¶¤·¤¸¤¹¤º */
		bitvec(1, 0, 1, 0, 1, 0, 1, 0), /* ¤»¤¼¤½¤¾¤¿¤À¤Á¤Â */
		bitvec(0, 1, 0, 1, 0, 1, 0, 0), /* ¤Ã¤Ä¤Å¤Æ¤Ç¤È¤É¤Ê */
		bitvec(0, 0, 0, 0, 1, 0, 0, 1), /* ¤Ë¤Ì¤Í¤Î¤Ï¤Ð¤Ñ¤Ò */
		bitvec(0, 0, 1, 0, 0, 1, 0, 0), /* ¤Ó¤Ô¤Õ¤Ö¤×¤Ø¤Ù¤Ú */
		bitvec(1, 0, 0, 0, 0, 0, 0, 0)	/* ¤Û */
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

 /** in¤ÇÍ¿¤¨¤é¤ì¤¿¥³¡¼¥É¤òbase¿Ê¤Î¿ô»ú¤Ë¤·¤Æoutp¤ËÆþ¤ì¤ë¡£*/
to_digit(in, base, outp)
letter	in, base, **outp;
{
	letter	c, vtol();

	if(c = in, c /= base) to_digit(c, base, outp);
	*(*outp)++ = vtol(in % base);
	**outp = EOLTTR;
}
