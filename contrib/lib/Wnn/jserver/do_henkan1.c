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
#include <ctype.h>

#include "demcom.h"
#include "commonhd.h"
#include "de_header.h"
#include "ddefine.h"
#include "jdata.h"
#include "fzk.h"

#include "kaiseki.h"
#ifdef SYSVR2
#include <fcntl.h>
#endif

#define NULL 0

#define FZK_L 256
#ifdef	putwchar
#undef	putwchar
#endif

static int bun_pter_;

henkan_setup()
{
	register int index;
	register w_char c;

	wnn_loadhinsi(hinsi_file_name);
	index = maxchg-1;
	while((c = get2_cur()) != 0){
#ifdef	DEBUG
putwchar(c);
#endif	DEBUG
		if (make_buns_par(c,index) == -1){
			break;
		}
		index--;
	}
	bun_pter_ = index + 1;
	init_jmt(0);
#ifdef	DEBUG
wsputs("\n");
#endif	DEBUG
}

get_fzk_vec(hinsi,fzk,vec,vec1)
register int *hinsi;
register int *vec;
register int *vec1;
register wchar *fzk;
{
	*hinsi=get4_cur();
	getws_cur(fzk);
	*vec=get4_cur();
	*vec1=get4_cur();
}

/* RENBUN */
do_kanren()
{int end,eid;
 int vec;
 int vec1;
 int vec2;
 wchar fzk[FZK_L];
 struct DSD_DBN *d_list;
 register int b_suu_;
 int hinsi;

	eid = get4_cur();
	c_env=env[eid];
	end = maxchg - 1;
	henkan_setup();
	get_fzk_vec(&hinsi,fzk,&vec,&vec1);
	vec2=get4_cur();

	b_suu_ = renbn_kai(bun_pter_, end + 1, hinsi, fzk, vec, vec1,
			   vec2, c_env->nbun, c_env->nshobun, &d_list);
	if (b_suu_ < 0){
		error_ret();	/* ERROR */
		return;
	}
	ret_dai(d_list, b_suu_);
	putc_purge();
#ifdef	DEBUG
print_dlist(d_list, b_suu_);
#endif	DEBUG
	return;
}

/* TANBUN (dai)*/
do_kantan_dai()
{int end, eid;
 int vec;
 int vec1;
 wchar fzk[FZK_L];
 struct DSD_DBN *d_list;
 register int b_suu_;
 int hinsi;

	eid = get4_cur();
	c_env=env[eid];
	end = maxchg - 1;
	henkan_setup();
	get_fzk_vec(&hinsi,fzk,&vec,&vec1);

	b_suu_ = tan_dai(bun_pter_, end + 1, hinsi, fzk, vec, vec1,
			 c_env->nshobun, &d_list);
	if (b_suu_ < 0){
		error_ret();	/* ERROR */
		return;
	}
	ret_dai(d_list, b_suu_);
	putc_purge();
#ifdef	DEBUG
print_dlist(d_list, b_suu_);
#endif	DEBUG
	return;
}

/* TANBUN (sho)*/
do_kantan_sho()
{int end, eid;
 int vec;
 int vec1;
 wchar fzk[FZK_L];
 struct DSD_SBN *d_list1;
 register int b_suu_;
 int hinsi;

	eid = get4_cur();
	c_env=env[eid];
	end = maxchg - 1;
	henkan_setup();
	get_fzk_vec(&hinsi,fzk,&vec,&vec1);

	b_suu_ = tan_syo(bun_pter_, end + 1, hinsi, fzk, vec, vec1, &d_list1);
	if (b_suu_ < 0){
		error_ret();	/* ERROR */
		return;
	}
	ret_sho(d_list1, b_suu_);
	putc_purge();
#ifdef	DEBUG
print_dlist1(d_list1, b_suu_);
#endif	DEBUG
	return;
}

/* JIKOUHO (dai)*/
do_kanzen_dai()
{int end, eid;
 int vec;
 int vec1;
 wchar fzk[FZK_L];
 struct DSD_DBN *d_list;
 register int b_suu_;
 int hinsi;

	eid = get4_cur();
	c_env=env[eid];
	end = maxchg - 1;
	henkan_setup();
	get_fzk_vec(&hinsi,fzk,&vec,&vec1);

	b_suu_ = jkt_get_dai(bun_pter_, end + 1, hinsi, fzk, vec, vec1,
			     c_env->nshobun, &d_list);
	if (b_suu_ < 0){
		error_ret();	/* ERROR */
		return;
	}
	ret_dai(d_list, b_suu_);
	putc_purge();
#ifdef	DEBUG
print_dlist(d_list, b_suu_);
#endif	DEBUG
	return;
}


/* JIKOUHO (sho)*/
do_kanzen_sho()
{int end, eid;
 int vec;
 int vec1;
 wchar fzk[FZK_L];
 struct DSD_SBN *d_list1;
 register int b_suu_;
 int hinsi;

	eid = get4_cur();
	c_env=env[eid];
	end = maxchg - 1;
	henkan_setup();
	get_fzk_vec(&hinsi,fzk,&vec,&vec1);

	b_suu_ = jkt_get_syo(bun_pter_, end + 1, hinsi, fzk, vec, vec1,
			     &d_list1);
	if (b_suu_ < 0){
		error_ret();	/* ERROR */
		return;
	}
#ifdef	DEBUG
print_dlist1(d_list1, b_suu_);
#endif	DEBUG
	ret_sho(d_list1, b_suu_);
	putc_purge();
	return;
}

make_buns_par (c, at_index)	/* make bun and bun	*/
register w_char c;
register int at_index;
{
	if ((at_index >= maxchg) || (at_index <= 0)){
	    wnn_errorno = WNN_LONG_MOJIRETSU;
	    return(-1);
	}
	bun[at_index] = c;

	jmtp[at_index] = (struct jdata **) -1;	/* 辞書をまだ引いていない */
	return(0);
}

Get_knj(jentptr, u, kouho, oy, oyl)
			/* get kanji and put it in  kouho*/
int u;
register struct jdata *jentptr;
w_char *kouho, *oy;
int oyl;
{
    UCHAR *kptr;
    register struct JT	*jtl;

    jtl = (struct JT *)(files[dic_table[jentptr->jishono].body].area);
    kptr = jtl->kanji + jentptr->kanji1;
    if(jentptr->which == D_YOMI){
	Get_knj1(kptr, oy, oyl, jentptr->kanji2 + u, kouho, NULL, NULL);
    }else{
	Get_knj1(kptr, oy, oyl, jentptr->kanji2 + u, NULL, kouho, NULL);
    }
}

get_yomi(jentptr, u, kouho)/* get yomi and put it in  kouho*/
int u;
register struct jdata *jentptr;
w_char *kouho;
{
    UCHAR *kptr;
    register struct JT	*jtl;

    jtl = (struct JT *)(files[dic_table[jentptr->jishono].body].area);
    kptr = jtl->kanji + jentptr->kanji1;
    Get_knj2(kptr, jentptr->kanji2 + u, NULL, kouho, NULL);
}

get_knj_com(jentptr, u, kouho, com)
			/* get kanji and put it in  kouho*/
int u;
register struct jdata *jentptr;
w_char *kouho;		/* kouho must be ended with NULL.	*/
w_char *com;
{
    register UCHAR *kptr;
    struct JT	*jtl;

    jtl = (struct JT *)(files[dic_table[jentptr->jishono].body].area);
    kptr = jtl->kanji + jentptr->kanji1;
    Get_knj2(kptr, jentptr->kanji2 + u, kouho, NULL,  com);
}
