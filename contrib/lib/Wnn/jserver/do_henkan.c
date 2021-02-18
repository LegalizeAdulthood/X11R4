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
#include "kaiseki.h"
#include "jdata.h"
#include "ddefine.h"


/*	DAI BUN	*/
ret_dai(dai_list, cnt)
struct	DSD_DBN	*dai_list;
int	cnt;
{register int i;
register struct	DSD_DBN	*dp;
	if (dai_list == NULL){ put4_cur(0); return;}
	put4_cur(cnt);

	put4_cur(count_sho(dai_list,cnt));
	put4_cur(count_d_kanji(dai_list,cnt));

	for(i=0,dp=dai_list ; i<cnt ; dp++, i++){
/*
out("\tbun_jc:%d, sbncnt:%d, bun_m:%d ,sbn:%x\n",
	dp->bun_jc, dp->sbncnt, dp->bun_m, dp->sbn);
*/
		put4_cur(maxchg - 1 - dp->bun_m); /* end */
		put4_cur(maxchg - 1 - dp->bun_jc);/* start */
		put4_cur(dp->sbncnt);
		put4_cur(dp->v_jc);
	}
	for(i=0,dp=dai_list ; i<cnt ; dp++, i++){
		ret_sho_x(dp->sbn, dp->sbncnt);
	}
	for(i=0,dp=dai_list ; i<cnt ; dp++, i++){
		ret_sho_kanji(dp->sbn, dp->sbncnt);
	}
}

count_sho(dai_list, cnt)
struct	DSD_DBN	*dai_list;
{register int sho_sum,i;
 register struct	DSD_DBN	*dp;
 for(sho_sum=i=0,dp=dai_list ; i<cnt ; dp++, i++){
		sho_sum += dp->sbncnt;
 }
 return sho_sum;
}


count_d_kanji(dp, cnt)
struct	DSD_DBN	*dp;
{register int k_sum,i;
 for(k_sum=i=0; i<cnt ; dp++, i++){
		k_sum += count_s_kanji(dp->sbn, dp->sbncnt);
 }
 return k_sum;
}

count_s_kanji(sp, cnt)
struct	DSD_SBN	*sp;
{register int i,sho_sum;
 if(sp == NULL){ cnt =0;}
 sho_sum=0;
 for(i=0; i<cnt ; sp++, i++){
	sho_sum += count_kanji(sp);
 }
 return sho_sum;
}

count_kanji(sho)
struct	DSD_SBN	*sho;
{
 register int	tmp,k_sum;
 wchar_t	kouho[1000];

 if (sho->jentptr){
	Get_knj(sho->jentptr, sho->t_jc, kouho, bun + sho->i_jc, sho->bun_jc - sho->i_jc + 1);
 }else{
	get_giji_knj(sho->bun_jc, sho->i_jc, sho->t_jc, kouho);
 }

 k_sum=0;
 k_sum += Strlen(kouho) +1; /* kanji */
 /* yomi */
 for( tmp = sho->bun_jc; ; tmp--){
	if(tmp < sho->bun_m) break;
	k_sum++;	/* put2_cur(bun[tmp]); */
	if(tmp == sho->i_jc) break;
 }
 k_sum++;	/* put2_cur(0); */
 /* fuzoku */
 for( tmp = sho->i_jc-1; ; tmp--) {
	if(tmp < sho->bun_m) break;
	k_sum++;	/* put2_cur(bun[tmp]); */
 }
 k_sum++;	/* put2_cur(0); */
 return(k_sum);
}


ret_sho_x(sho_list, cnt)
register struct	DSD_SBN	*sho_list;
register int	cnt;
{
 if(sho_list == NULL){ cnt =0; }
 for( ; cnt > 0; cnt --){
/*
	out("\tbun_jc: %d, i_jc: %d, bun_m: %d \n",
		sho_list->bun_jc, sho_list->i_jc, sho_list->bun_m);
	out("\tjdata:%x, kangovect:%d, t_jc:%d, hinsi: %d:%s, status:%d\n",
		sho_list->jentptr, sho_list->kangovect, sho_list->t_jc, sho_list->hinsi, wnn_get_hinsi_name(sho_list->hinsi), sho_list->status);
*/

	put4_cur(maxchg - 1 - sho_list->bun_m);		/*end */
	put4_cur(maxchg - 1 - sho_list->bun_jc);	/*start */
	put4_cur(maxchg - 1 - sho_list->i_jc);		/*jiriend*/
	if(sho_list->jentptr != NULL){
	    int hindo;
	    put4_cur(sho_list->jentptr->jishono);	/*jisho No */
	    put4_cur(sho_list->jentptr->serial + sho_list->t_jc);
							/* entry + t_jc */
				/* Internally, we use "kasou-hindo"
				 * so return kasou-hindo as hindo-value"*/
	    hindo = *(sho_list->jentptr->hindo + sho_list->t_jc)&0x7F;
	    if(sho_list->jentptr->hindo_in){
		hindo += *(sho_list->jentptr->hindo_in + sho_list->t_jc)&0x7F;
	    }
	    put4_cur(hindo);
                                                        /* hindo (u_char) */
	    put4_cur(*(sho_list->jentptr->hindo + sho_list->t_jc) >> 7);
							/* ima */
	}else{
		put4_cur(-1);
		put4_cur(sho_list->t_jc);		/* CHANGE KURI */
		put4_cur(-1);
		put4_cur(-1);
	}
	put4_cur(sho_list->hinsi);			/* hinshi */
	put4_cur(sho_list->status);
	put4_cur(sho_list->status_bkwd);	/* ADD KURI */
	put4_cur(sho_list->kangovect);
	put4_cur(sho_list->v_jc);
	sho_list++;
 }
}

ret_sho_kanji(sho_list, cnt)
register struct	DSD_SBN	*sho_list;
register int	cnt;
{
 if(sho_list == NULL){ cnt =0; }
 for( ; cnt > 0; sho_list++, cnt --){
	ret_kanji(sho_list);
 }
}

ret_kanji(sho)
register struct	DSD_SBN	*sho;
{
 register int	tmp;
 wchar_t	kouho[1000];

 if (sho->jentptr){
	Get_knj(sho->jentptr, sho->t_jc, kouho, bun + sho->i_jc, sho->bun_jc - sho->i_jc + 1);
 }else{
	get_giji_knj(sho->bun_jc, sho->i_jc, sho->t_jc, kouho);
 }
 putws_cur(kouho); /* kanji */
 /* yomi */
 for( tmp = sho->bun_jc; tmp >= sho->i_jc; tmp--){
     put2_cur(bun[tmp]);
 }	
/*
 for( tmp = sho->bun_jc; ; tmp--){
	if(tmp < sho->bun_m) break;
	put2_cur(bun[tmp]);
	if(tmp == sho->i_jc) break;
 }
H. T*/
        
 put2_cur(0);
 /* fuzoku */
 for( tmp = sho->i_jc-1; ; tmp--) {
	if(tmp < sho->bun_m) break;
	put2_cur(bun[tmp]);
 }
 put2_cur(0);
}

/*	SHO BUN	*/
ret_sho(sho_list, cnt)
register struct	DSD_SBN	*sho_list;
register int	cnt;
{
	if (sho_list == NULL){ cnt =0;	put4_cur(cnt); return;}
	put4_cur(cnt);
	put4_cur(count_s_kanji(sho_list,cnt));

	ret_sho_x(sho_list, cnt);
	ret_sho_kanji(sho_list, cnt);
}

#ifdef	DEBUG

#ifdef	putwchar
#undef	putwchar
#endif
print_dlist(dlist, cnt)
struct	DSD_DBN	*dlist;
int	cnt;
{

    out("大文節数:%d\n", cnt);
	if (dlist == 0)
		return;
	for ( ; cnt > 0; dlist++, cnt --) {
		_print_dlist(dlist);
/*
	    out("\tbun_jc:%d, sbncnt:%d, bun_m:%d ,sbn:%x\n",
		    dlist->bun_jc, dlist->sbncnt, dlist->bun_m, dlist->sbn);
*/
	    print_dlist1(dlist->sbn, dlist->sbncnt);
	}
	fflush(stdout);
}

_print_dlist(dlist)
struct	DSD_DBN	*dlist;
{
	struct	DSD_SBN *sbn;
	int	i;
	int	tmp;
	wchar_t	kouho[512];

	out("大文節:%x\t", dlist);
	if (dlist == 0) return;

	sbn = dlist->sbn;
	for (i = dlist->sbncnt; i > 0; i--) {
		if (sbn == 0)	break ;

		if (sbn->jentptr)
		    Get_knj(sbn->jentptr, sbn->t_jc, kouho, bun + sbn->i_jc, sbn->bun_jc - sbn->i_jc + 1);
		else
			get_giji_knj(sbn->bun_jc, sbn->i_jc, sbn->t_jc, kouho);
		out(" ");
		wsputs(kouho);
		out(":");
		for (tmp = sbn->i_jc-1; tmp >= sbn->bun_m; tmp--)
			putwchar(bun[tmp]);
		
		sbn++;
	}
	out("\t\thyouka:%d\n" ,dlist->v_jc);
	fflush(stdout);
}

print_dlist1(dlist1, cnt)
struct	DSD_SBN	*dlist1;
int	cnt;
{
	unsigned int	hindo;

	for ( ; cnt > 0; dlist1++, cnt --) {
		_print_dlist1(dlist1);
		if (dlist1 == 0)
			return;
/*
		out("\tbun_jc: %d, i_jc: %d, bun_m: %d \n",
			dlist1->bun_jc, dlist1->i_jc, dlist1->bun_m);
*/
		if (dlist1->jentptr == 0)
		    hindo = 0;
		else
		    hindo = *(dlist1->jentptr->hindo + dlist1->t_jc);
		out("  jdata:%x vect:%d t_jc:%d hinsi:%d:%s hindo:%x hyoka:%d status:%d status_bkwd:%d\n",
		    dlist1->jentptr, dlist1->kangovect, dlist1->t_jc, dlist1->hinsi, 
		    wnn_get_hinsi_name(dlist1->hinsi), hindo,
		    dlist1->v_jc, dlist1->status, dlist1->status_bkwd);
	}
	fflush(stdout);
}

_print_dlist1(dlist1)
struct	DSD_SBN	*dlist1;
{
    int	tmp;
    wchar_t	kouho[512];

    out("小文節:%x\t", dlist1);
    if (dlist1 == 0) {
	    return;
    }
    for ( tmp = dlist1->bun_jc; tmp >= dlist1->bun_m; tmp--) {
	putwchar(bun[tmp]);
	if (tmp == dlist1->i_jc) {
	    if (dlist1->jentptr)
		Get_knj(dlist1->jentptr, dlist1->t_jc, kouho, bun + dlist1->i_jc, dlist1->bun_jc - dlist1->i_jc + 1);
	    else
		get_giji_knj(dlist1->bun_jc, dlist1->i_jc, dlist1->t_jc, kouho);
	    out(" ");
	    wsputs(kouho);
	    out(":");
	}
    }
    out("\n");	
    fflush(stdout);
}

#endif	DEBUG
