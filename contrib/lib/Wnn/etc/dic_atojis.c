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

#include "commonhd.h"
#include "jslib.h"
#include "jdata.h"

static int	keta_4;

static wchar_t	_tatojis[] = {
	0xa1a1, 0xa1aa, 0xa1c9, 0xa1f4, 0xa1f0, 0xa1f3, 0xa1f5, 0xa1c7, 
	0xa1ca, 0xa1cb, 0xa1f6, 0xa1dc, 0xa1a4, 0xa1dd, 0xa1a5, 0xa1bf, 
	0xa3b0, 0xa3b1, 0xa3b2, 0xa3b3, 0xa3b4, 0xa3b5, 0xa3b6, 0xa3b7, 
	0xa3b8, 0xa3b9, 0xa1a7, 0xa1a8, 0xa1e3, 0xa1e1, 0xa1e4, 0xa1a9, 
	0xa1f7, 0xa3c1, 0xa3c2, 0xa3c3, 0xa3c4, 0xa3c5, 0xa3c6, 0xa3c7, 
	0xa3c8, 0xa3c9, 0xa3ca, 0xa3cb, 0xa3cc, 0xa3cd, 0xa3ce, 0xa3cf, 
	0xa3d0, 0xa3d1, 0xa3d2, 0xa3d3, 0xa3d4, 0xa3d5, 0xa3d6, 0xa3d7, 
	0xa3d8, 0xa3d9, 0xa3da, 0xa1ce, 0xa1ef, 0xa1cf, 0xa1b0, 0xa1b2, 
	0xa1ae, 0xa3e1, 0xa3e2, 0xa3e3, 0xa3e4, 0xa3e5, 0xa3e6, 0xa3e7, 
	0xa3e8, 0xa3e9, 0xa3ea, 0xa3eb, 0xa3ec, 0xa3ed, 0xa3ee, 0xa3ef, 
	0xa3f0, 0xa3f1, 0xa3f2, 0xa3f3, 0xa3f4, 0xa3f5, 0xa3f6, 0xa3f7, 
	0xa3f8, 0xa3f9, 0xa3fa, 0xa1d0, 0xa1c3, 0xa1d1, 0xa1b1, 0x007f
};


static int
atojis(c)
register wchar_t c;
{
	if (c >= ' ' && c <= (unsigned char)'\376')
		c = _tatojis[c - ' '];
	return(c);
}

static wchar_t	_tatokan[] = {
	0xa1bb, 0xb0ec, 0xc6f3, 0xbbb0, 0xbbcd, /* �����󻰻� */
	0xb8de, 0xcfbb, 0xbcb7, 0xc8ac, 0xb6e5	/* ��ϻ��Ȭ�� */
};

static wchar_t	_tatokan_o[] = {
	0xceed, 0xb0ed, 0xc6f5, 0xbbb2, 0xbbcd, /* ��������� */
	0xb8e0, 0xcfbb, 0xbcb7, 0xc8ac, 0xb6e5	/* ��ϻ��Ȭ�� */
};

static wchar_t
atokan(c)
register wchar_t c;
{
	if (c >= '0' && c <= '9')
		c = _tatokan[c - '0'];
	return (c);
}

static wchar_t
atokan_o(c)
register wchar_t c;
{
	if (c >= '0' && c <= '9')
		c = _tatokan_o[c - '0'];
	return (c);
}

static wchar_t	_tatoket[] = {
	0xbdbd, 0xc9b4, 0xc0e9,			/* ��ɴ�� */
	0xcbfc, 0xb2af, 0xc3fb, 0xb5fe,		/* �������� */
	0xd4b6, 0xa4b7, 0xbef7, 0xb9c2,		/* Զ������ */
	0xb4c2, 0xc0b5, 0xbadc, 0xb6cb		/* �����ܶ� */
	/* ���Ϻ� */ /* ���ε� */ /* ��ͳ¾ */ /* �ԲĻ׵� */ /* ̵����� */
};

static wchar_t	_tatoket_o[] = {
	0xbda6, 0xeff9, 0xeff4,			/* ������ */
	0xe8df					/* �� */
};

static
getketa(k, kouho, col)
int	k;
wchar_t	kouho[];
int	*col;
{
	if (k % 4) {
		if (keta_4 != 0) {
			keta_4 = 0;
			getketa(k / 4 * 4, kouho, col);
		}
		kouho[(*col)++] = _tatoket[k % 4 - 1];
	} else if ((k / 4 > 0) && (k / 4 < 13)) {
		kouho[(*col)++] = _tatoket[k / 4 + 2];
	} else if (k / 4 == 13) {
		kouho[(*col)++] = 0xbabb;
		kouho[(*col)++] = 0xb2cf;
		kouho[(*col)++] = 0xb9b1;	/* ���Ϻ� */
	} else if (k / 4 == 14) {
		kouho[(*col)++] = 0xb5c0;
		kouho[(*col)++] = 0xc1ce;
		kouho[(*col)++] = 0xb0a4;	/* ���ε� */
	} else if (k / 4 == 15) {
		kouho[(*col)++] = 0xc2be;
		kouho[(*col)++] = 0xcdb3;
		kouho[(*col)++] = 0xc6e1;	/* ��ͳ¾ */
	} else if (k / 4 == 16) {
		kouho[(*col)++] = 0xb5c4;
		kouho[(*col)++] = 0xbbd7;
		kouho[(*col)++] = 0xb2c4;
		kouho[(*col)++] = 0xc9d4;	/* �ԲĻ׵� */
	} else if (k / 4 == 17) {
		kouho[(*col)++] = 0xbff4;
		kouho[(*col)++] = 0xc2e7;
		kouho[(*col)++] = 0xcecc;
		kouho[(*col)++] = 0xccb5;	/* ̵����� */
	} else {
		kouho[(*col)++] = 0xa1a9;	/* �� */
	}
	keta_4 = 0;
}

static
getketa_o(k, kouho, col)
int	k;
wchar_t	kouho[];
int	*col;
{
	if (k % 4) {
		if (keta_4 != 0) {
			keta_4 = 0;
			if (k / 4 == 1)
				kouho[(*col)++] = _tatoket_o[3];        /* �� */
			else
				getketa(k / 4 * 4, kouho, col);
		}
		kouho[(*col)++] = _tatoket_o[k % 4 - 1];
	} else if (k / 4 == 1) {
		kouho[(*col)++] = _tatoket_o[3];	/* �� */
	} else {
		getketa(k, kouho, col);
	}
	keta_4 = 0;
}

substr(c1, c2)
char *c1;
w_char *c2;
{
    for(;*c1;c1++, c2++){
	if(*c1 != *c2) break;
    }
    if(*c1) return(0);
    return(1);
}

wchar_t *
kanji_giji_str(bun, bunl, c, kouho)
w_char *bun;
int	bunl;
wchar *c;
wchar	*kouho;
{
    register int	k;
    w_char	revkouho[LENGTHYOMI];
    char tmp[LENGTHYOMI];
    int	col = 0, keta = 0;

    if(substr(DIC_HIRAGANA, c)){
	for (k = bunl - 1; k >=0 ; k--) {
	    *kouho++ =bun[k];
	}
    }else if(substr(DIC_KATAKANA, c)){	/* �������� */
	for (k = bunl - 1; k >=0 ; k--){
	    if ((bun[k] & 0xff00) == 0xa400)
		*kouho++ = bun[k] | 0x100;
	    else 
		*kouho++ =bun[k];
	}
    }else if(substr(DIC_ZENKAKU, c)){	/* ���ѿ��� *//* ������ */
	for (k = bunl - 1; k >=0 ; k--){
	    *kouho++ = atojis(bun[k]);
	}
    }else if(substr(DIC_NUM_KAN, c)){	/* ������ *//* ���� */
	for (k = bunl - 1; k >=0 ; k--){
	    *kouho++ = atokan(bun[k]);
	}	    
    }else if(substr(DIC_NUM_HANCAN, c)){	/* Ⱦ�ѿ��� *//* 1,234 */
	for (k = 0; k < bunl ; k++){
	    if ((keta != 0) && (keta % 3 == 0))
		revkouho[col++] = ',';	/* , */
	    revkouho[col++] = bun[k];
	    keta++;
	}
	while (--col >= 0) {
	    *kouho++ = revkouho[col];
	}
    }else if(substr(DIC_NUM_ZENCAN, c)){	/* ���ѿ��� *//* ���������� */
	for (k = 0; k < bunl ; k++){
	    if ((keta != 0) && (keta % 3 == 0))
		revkouho[col++] = 0xa1a4; /* �� */
	    revkouho[col++] = atojis(bun[k]);
	    keta++;
	}
	while (--col >= 0) {
	    *kouho++ = revkouho[col];
	}
    }else if(substr(DIC_NUM_KANSUUJI, c)){	/* ������ *//* ɴ�󽽻� */
	keta_4 = 0;
	for (k = 0; k < bunl ; k++){
	    if (bun[k] != '0') {
		if (keta != 0)
		    getketa(keta, revkouho, &col);
		if (bun[k] != '1' ||
		    keta % 4 == 0 || keta % 4 == 3)
		    revkouho[col++] = atokan(bun[k]);
	    } else if ((keta != 0) && (keta % 4 == 0)) {
		keta_4 = 1;
	    }
	    keta++;
	}
	if (col == 0)
	    *kouho++ = _tatokan[0];
	while (--col >= 0) {
	    *kouho++ = revkouho[col];
	}
    }else if(substr(DIC_NUM_KANOLD, c)){	/* ������ *//* ��ɴ������ chao*/
	keta_4 = 0;
	for (k = 0; k < bunl; k++) {
	    if (bun[k] != '0') {
		if (keta != 0)
		    getketa_o(keta, revkouho, &col);
		if (bun[k] != '1' ||
		    keta % 4 == 0 || keta % 4 == 3)
		    revkouho[col++] = atokan_o(bun[k]);
	    } else if ((keta != 0) && (keta % 4 == 0)) {
		keta_4 = 1;
	    }
	    keta++;
	}
	if (col == 0)
	    *kouho++ = _tatokan_o[0];
	while (--col >= 0) {
	    *kouho++ = revkouho[col];
	}
    }else if(substr(DIC_ESC, c)){
	*kouho++ = '\\';
    }else if(substr(DIC_HEX, c) || substr(DIC_HEXc, c)){
	int num;
	sStrcpy(tmp, c + strlen(DIC_HEX));
	sscanf(tmp,"%x",&num);
	*kouho++ = num;
    }else if(substr(DIC_OCT, c)){
	int num;
	sStrcpy(tmp, c + strlen(DIC_OCT));
	sscanf(tmp,"%o",&num);
	*kouho++ = num;
    }else{
	for(;*c && *c != '\\';c++){
	    *kouho++ = *c;
	}
	if(*c == '\\') *kouho++ = *c;
    }
    *kouho = 0;
    return (kouho);
}


kanji_esc_str(w, oy, oyl)
w_char *w, *oy;
int oyl;
{
    register wchar *ret;
    register wchar *c = w, next;
    wchar tmp[LENGTHKANJI];

    for(;*c;c++){
	if(*c == '\\') break;
    }
    if(!*c) return;
    Strcpy(tmp, w);
    c = tmp;
    while(*c){
	if(*c == '\\' &&
	   (ret = kanji_giji_str(oy, oyl, c, w))!= NULL){
	    w = ret;
	    for(++c; *c; c++){
		if(*c == '\\'){
		    c++;
		    break;
		}
	    }
	}else{
	    *w++ = *c++;
	}
    }
    *w = 0;
}

Get_kanji(kptr, oy, oyl, kanji, yomi, comment)
UCHAR *kptr;
w_char *kanji, *comment, *yomi, *oy;
int oyl;
{
    get_kanji_str(kptr, kanji, yomi, comment);
    if(kanji){
	kanji_esc_str(kanji, oy, oyl);
    }
    if(yomi){
	kanji_esc_str(yomi, oy, oyl);
    }
}


Get_knj1(kptr, oy, oyl,kanji2, kouho, yomi, comment)
UCHAR *kptr;
int kanji2;
w_char *kouho, *comment, *yomi, *oy;
int oyl;
{
	int tcnt;
	for (tcnt = 0; tcnt < kanji2; tcnt++){
		kptr += *kptr;
	}
	Get_kanji(kptr, oy, oyl, kouho, yomi, comment);
	return;
}

