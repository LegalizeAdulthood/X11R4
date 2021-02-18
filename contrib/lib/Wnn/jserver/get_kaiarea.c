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
#include <stdio.h>
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "jdata.h"
#include "jservercf.h"
#include "kaiseki.h"
#include "wnn_malloc.h"

get_kaiseki_area(kana_len)
int kana_len ;
{
	char *c;
	char *area_pter;

	int maxj_len = kana_len * sizeof(int);
	int jmtp_len = kana_len * sizeof(struct jdata **);
	int jmt_len  = SIZE_JISHOTABLE * sizeof(struct jdata *);
	int jmtw_len = SIZE_JISHOHEAP * sizeof(struct jdata);

	int bun_len = (kana_len + 1) * sizeof(wchar_t);

	if((area_pter = 
		malloc(bun_len + jmtw_len + jmt_len + jmtp_len + maxj_len)) == NULL){
		wnn_errorno = WNN_MALLOC_INITIALIZE;
		error1("malloc in get_kaiseki_area");
		return(-1);
	}

	maxj = (int *)area_pter; 
	area_pter += maxj_len;
	for(c = (char *)maxj;c < area_pter ; ){
		*(c++) = 0;
	}

	jmtp = (struct jdata ***)area_pter; 
	area_pter += jmtp_len;
	for(c = (char *)jmtp ;c < area_pter ; ){
		*(c++) = 0;
	}

	jmt_ = (struct jdata **)area_pter;
	area_pter += jmt_len;
	jmtw_ = (struct jdata *)area_pter;
	area_pter += jmtw_len;
	jmt_end = jmt_ + SIZE_JISHOTABLE;
	jmtw_end = jmtw_ + SIZE_JISHOHEAP;

	bun = (wchar_t *)area_pter;
	area_pter += bun_len;

	maxchg = kana_len;
	initjmt = maxchg - 1;
	bun[maxchg] = NULL;

	return(0);
}


/* サーバーが起きた時に呼ぶ
	解析ワークエリアをクリアする
	変換がエラーリターンした時にも呼んでね。*/

init_work_areas()
{
	init_ichbnp();
	init_bzd();
	init_sbn();
	init_jktdbn();
	init_jktsbn();
	init_jktsone();
}
