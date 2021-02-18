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
#include "ddefine.h"
#include "de_header.h"
#include "kaiseki.h"

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
	0xa1bb, 0xb0ec, 0xc6f3, 0xbbb0, 0xbbcd, /* ¡»°ìÆó»°»Í */
	0xb8de, 0xcfbb, 0xbcb7, 0xc8ac, 0xb6e5	/* ¸ÞÏ»¼·È¬¶å */
};

static wchar_t	_tatokan_o[] = {
	0xceed, 0xb0ed, 0xc6f5, 0xbbb2, 0xbbcd, /* Îí°íÆõ»²»Í */
	0xb8e0, 0xcfbb, 0xbcb7, 0xc8ac, 0xb6e5	/* ¸àÏ»¼·È¬¶å */
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
	0xbdbd, 0xc9b4, 0xc0e9,			/* ½½É´Àé */
	0xcbfc, 0xb2af, 0xc3fb, 0xb5fe,		/* Ëü²¯Ãûµþ */
	0xd4b6, 0xa4b7, 0xbef7, 0xb9c2,		/* Ô¶¤·¾÷¹Â */
	0xb4c2, 0xc0b5, 0xbadc, 0xb6cb		/* ´ÂÀµºÜ¶Ë */
	/* ¹±²Ïº» */ /* °¤ÁÎµÀ */ /* ÆáÍ³Â¾ */ /* ÉÔ²Ä»×µÄ */ /* ÌµÎÌÂç¿ô */
};

static wchar_t	_tatoket_o[] = {
	0xbda6, 0xeff9, 0xeff4,			/* ½¦ïùïô */
	0xe8df					/* èß */
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
		kouho[(*col)++] = 0xb9b1;	/* ¹±²Ïº» */
	} else if (k / 4 == 14) {
		kouho[(*col)++] = 0xb5c0;
		kouho[(*col)++] = 0xc1ce;
		kouho[(*col)++] = 0xb0a4;	/* °¤ÁÎµÀ */
	} else if (k / 4 == 15) {
		kouho[(*col)++] = 0xc2be;
		kouho[(*col)++] = 0xcdb3;
		kouho[(*col)++] = 0xc6e1;	/* ÆáÍ³Â¾ */
	} else if (k / 4 == 16) {
		kouho[(*col)++] = 0xb5c4;
		kouho[(*col)++] = 0xbbd7;
		kouho[(*col)++] = 0xb2c4;
		kouho[(*col)++] = 0xc9d4;	/* ÉÔ²Ä»×µÄ */
	} else if (k / 4 == 17) {
		kouho[(*col)++] = 0xbff4;
		kouho[(*col)++] = 0xc2e7;
		kouho[(*col)++] = 0xcecc;
		kouho[(*col)++] = 0xccb5;	/* ÌµÎÌÂç¿ô */
	} else {
		kouho[(*col)++] = 0xa1a9;	/* ¡© */
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
				kouho[(*col)++] = _tatoket_o[3];        /* èß */
			else
				getketa(k / 4 * 4, kouho, col);
		}
		kouho[(*col)++] = _tatoket_o[k % 4 - 1];
	} else if (k / 4 == 1) {
		kouho[(*col)++] = _tatoket_o[3];	/* èß */
	} else {
		getketa(k, kouho, col);
	}
	keta_4 = 0;
}

wchar_t *
get_giji_knj(j, i, t, kouho)
int	j;
int	i;
int	t;
wchar_t	*kouho;
{
	register int	k;
	wchar_t	revkouho[1000];
	int	col = 0, keta = 0;

	switch (t) {
	case	WNN_NUM_ZEN:	/* Á´³Ñ¿ô»ú *//* £±£²£³ */
			for (k = j; k >=i ; k--) {
				if (k == j &&
				    (isspace(bun[k]) || isjspace(bun[k])))
					*kouho++ = bun[k];
				else
					*kouho++ = atojis(bun[k]);
			}
			break;
	case	WNN_ALP_ZEN:	/* Á´³Ñ */
	case	WNN_KIG_JIS:	
	case	WNN_KIG_ASC:
			for (k = j; k >=i ; k--)
				*kouho++ = atojis(bun[k]);
			break;

	case	WNN_KATAKANA:	/* ¥«¥¿¥«¥Ê */
			for (k = j; k >=i ; k--) {
				if ((bun[k] & 0xff00) == 0xa400)
					*kouho++ = bun[k] | 0x100;
				else 
					*kouho++ =bun[k];
			}
			break;

	case	WNN_NUM_KAN:	/* ´Á¿ô»ú *//* °ìÆó»° */
			for (k = j; k >=i ; k--)
				*kouho++ = atokan(bun[k]);
			break;
	case	WNN_NUM_HANCAN:	/* È¾³Ñ¿ô»ú *//* 1,234 */
			for (k = i; k <= j; k++) {
				if (k == j &&
				    (isspace(bun[k]) || isjspace(bun[k]))) {
					revkouho[col++] = bun[k];
					break;
				}
				if ((keta != 0) && (keta % 3 == 0))
					revkouho[col++] = ',';	/* , */
				revkouho[col++] = bun[k];
				keta++;
			}

			while (--col >= 0) {
				*kouho++ = revkouho[col];
			}
			break;
	case	WNN_NUM_ZENCAN:	/* Á´³Ñ¿ô»ú *//* £±¡¤£²£³£´ */
			for (k = i; k <= j; k++) {
				if (k == j &&
				    (isspace(bun[k]) || isjspace(bun[k]))) {
					revkouho[col++] = bun[k];
					break;
				}
				if ((keta != 0) && (keta % 3 == 0))
					revkouho[col++] = 0xa1a4; /* ¡¤ */
				revkouho[col++] = atojis(bun[k]);
				keta++;
			}

			while (--col >= 0) {
				*kouho++ = revkouho[col];
			}
			break;
	case	WNN_NUM_KANSUUJI:	/* ´Á¿ô»ú *//* É´Æó½½»° */
			keta_4 = 0;
			for (k = i; k <= j; k++) {
				if (k == j &&
				    (isspace(bun[k]) || isjspace(bun[k]))) {
					revkouho[col++] = bun[k];
					break;
				}
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
			break;
	case	WNN_NUM_KANOLD:	/* ´Á¿ô»ú *//* °íÉ´Æõ½¦»² */
			keta_4 = 0;
			for (k = i; k <= j; k++) {
				if (k == j &&
				    (isspace(bun[k]) || isjspace(bun[k]))) {
					revkouho[col++] = bun[k];
					break;
				}
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
			break;
	default:	/* ¤Ò¤é¤¬¤Ê */
			for (k = j; k >=i ; k--) {
				*kouho++ =bun[k];
			}
			break;
	}
	*kouho++ = 0;
	return (kouho);
}

giji_hindoup(t_jc)
int	t_jc;
{
	if (t_jc >= 0)
		return ;
	switch(t_jc) {
	case WNN_NUM_HAN:
	case WNN_NUM_ZEN:
	case WNN_NUM_KAN:
	case WNN_NUM_KANSUUJI:
	case WNN_NUM_KANOLD:
	case WNN_NUM_HANCAN:
	case WNN_NUM_ZENCAN:
			c_env->giji.number = t_jc;
			break;
	case WNN_ALP_HAN:
	case WNN_ALP_ZEN:
			c_env->giji.eisuu = t_jc;
			break;
	case WNN_KIG_HAN:
	case WNN_KIG_JIS:
	case WNN_KIG_ASC:
			c_env->giji.kigou = t_jc;
			break;
	default:
			return ;
	}
}
