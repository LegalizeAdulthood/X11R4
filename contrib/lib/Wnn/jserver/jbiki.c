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

#include "de_header.h"
#include "jdata.h"
#include "kaiseki.h"


#ifndef min
#define min(a, b) ((a > b)? b:a)
#define max(a, b) ((a < b)? b:a)
#endif

static int maxlevel;/** �ɤߤβ�ʸ���ܤޤǥޥå����륨��ȥ꡼�����ä��� */
static struct jdata **jmt;/** ������֤����ؤΥݥ��� */
static int number;/** ���饤����Ȥμ����ֹ� */
static unsigned short *hinsistart;
static UCHAR *hindostart;
static UCHAR *hindo2start;

jishobiki(yomi , jmtx )
w_char *yomi;/** �ɤߤ���Ƭ�ؤΥݥ��� */
struct jdata **jmtx;/** ��̤��֤��ΰ����Ƭ */
{
    
    register struct JT *jtp;
    register int k;
    
    maxlevel = 0;
    jmt = jmtx;
    
    for(k = 0 ; k < c_env->jishomax; k++){
	number = c_env->jisho[k];
	if(dic_table[c_env->jisho[k]].enablef){
	    jtp = (struct JT *)(files[dic_table[c_env->jisho[k]].body].area);
	    hindo2start = (dic_table[c_env->jisho[k]].hindo != -1)?
		jtp->hindo:NULL;
	    hindostart = (dic_table[c_env->jisho[k]].hindo != -1)?
		((struct HJT *)(files[dic_table[c_env->jisho[k]].hindo].area))->hindo:jtp->hindo;
	    hinsistart = jtp->hinsi;
	    if(jtp->syurui == WNN_UD_DICT){
		if(ud_biki(jtp , yomi) == -1){
		    error1("No More Jishobiki Area\n");
		    return(-1);
		}
	    }else if(jtp->syurui == WNN_REV_DICT){
		if(rd_biki(jtp , yomi, dic_table[c_env->jisho[k]].rev) == -1){
		    error1("No More Jishobiki Area\n");
		    return(-1);
		}
	    }else{
		if(sd_biki(jtp , yomi) == -1){
		    error1("No More Jishobiki Area\n");
		    return(-1);
		}
	    }
	}
    }
    
    return(maxlevel);
}



ud_biki(jtl,yomi)
struct JT *jtl;
w_char *yomi;
{
    struct jdata *jep; 
    register struct uind2 *p; 
    register int ind1;
    register int len;
    
    struct uind1 *tary;
    
    tary = jtl->table;
    
    for(ind1 = binary(tary,yomi,jtl->maxtable, jtl);
	ind1 >= 0 ; ind1 = tary[ind1].pter1){
	
	for(p=((struct uind2 *)((tary[ind1].pter) + jtl->hontai));
	    ;p = ((struct uind2 *)((p->next) + jtl->hontai))){
	    len = p->yomi[0];
	    if(jmt + len > jmt_end)
		return( -1);
	    
	    /* set jmt */
	    if(*(jmt + len - 1)){
		for(jep = *(jmt + len - 1); jep->jptr != NULL;jep = jep->jptr);
		jep->jptr = j_e_p;
	    }else{				
		*(jmt + len - 1) = j_e_p;
	    }
	    
	    maxlevel = max(maxlevel ,  len);
	    if(j_e_p >= jmtw_end) return( -1);
	    j_e_p->kanji1 = p->kanjipter;
	    j_e_p->kanji2  = 0;
	    j_e_p->serial = p->serial;
	    j_e_p->kosuu = p->kosuu;
	    j_e_p->jishono = number;
	    j_e_p->jptr = NULL;
	    j_e_p->hinsi = hinsistart + p->serial;
	    j_e_p->hindo = hindostart + p->serial;
	    if(hindo2start)  j_e_p->hindo_in = hindo2start + p->serial;
	    else  j_e_p->hindo_in = NULL;
	    j_e_p->which = D_YOMI;
	    j_e_p += 1;
	    if(p->next == ENDPTR) break;
	}
    }
    return(maxlevel);
}

/*
 * Find the element of user dict which matches longest with the yomi.
 */

binary1(tary,yomi,end,jtl)
register struct uind1 *tary;
register w_char *yomi;
register int end;
struct JT *jtl;
{
    register int start = -1;
    register int key;
    
    for(; start + 1 < end;){
	key = (start + end) / 2;
	if(Strcmpud(&tary[key],yomi,jtl) > 0){
	    end = key;
	}else{
	    start = key;
	}
    }
    return(start);
}

binary(tary,yomi,end,jtl)
register struct uind1 *tary;
register w_char *yomi;
int end;
struct JT *jtl;
{
    register int start;
    register struct uind2 *hop;
    register int len;
    
    start = binary1(tary,yomi,end,jtl);
    for(;start >= 0; start = tary[start].pter1){
	hop = ((struct uind2 *)((tary[start].pter) + jtl->hontai));
	len = hop->yomi[0]; 
	if(Substrud(&tary[start],yomi,len,jtl)){
	    return(start);
	}
    }
    return(-1);
}

Strcmpud(ui1p,yomi,jtl)
struct uind1 *ui1p;
w_char *yomi;
struct JT *jtl;
{
    unsigned int y1,y2;
    int tmp;
    int len;
    struct uind2 *hop;
    
    if(yomi[0] == 0) return(ui1p->yomi1 > 0);
    y1 = yomi[0]<< 16 | yomi[1];
    if(tmp = (ui1p->yomi1 - y1)){
	return((ui1p->yomi1 > y1)?1:-1);
    }
    if(yomi[1] == 0) return(0);
    if(yomi[2] == 0) return(ui1p->yomi2 > 0);
    y2 = yomi[2] << 16 | yomi[3];
    if(tmp = (ui1p->yomi2 - y2)){
	return((ui1p->yomi2 > y2)?1:-1);
    }
    hop = ((struct uind2 *)((ui1p->pter) + jtl->hontai));
    if((len = hop->yomi[0]) < 4)return(0);
    if((tmp = Strncmp(hop->yomi + 1, yomi + 4,len - 4)) != 0){
	return(tmp);
    }
    return(0 - yomi[len]);
}

Substrud(ui1p,yomi,len,jtl)
struct uind1 *ui1p;
w_char *yomi;
int len;
struct JT *jtl;
{
    switch(len){
    case 0:
	return(1);
    case 1:
	return((ui1p->yomi1 >> 16 == yomi[0]));
    case 2:
	return(ui1p->yomi1 == (yomi[0]<< 16 | yomi[1]));
    case 3:
	return((ui1p->yomi1 == (yomi[0]<< 16 | yomi[1])) &&
	       ui1p->yomi2 >> 16 == yomi[2]);
    case 4:
	return((ui1p->yomi1 == (yomi[0]<< 16 | yomi[1])) &&
	       (ui1p->yomi2 == (yomi[2]<< 16 | yomi[3])));
    default:
	return((ui1p->yomi1 == (yomi[0]<< 16 | yomi[1])) &&
	       (ui1p->yomi2 == (yomi[2]<< 16 | yomi[3])) &&
	       !(Strncmp(((struct uind2 *)(ui1p->pter + jtl->hontai))->yomi + 1,
			 yomi + 4,len - 4)));
    }
}

Substrstud(yomi,ui1p, jtl)
w_char *yomi;
struct uind1 *ui1p;
struct JT *jtl;
{
    int len = Strlen(yomi);
    
    switch(len){
    case 0:
	return(1);
    case 1:
	return((ui1p->yomi1 >> 16 == yomi[0]));
    case 2:
	return(ui1p->yomi1 == (yomi[0]<< 16 | yomi[1]));
    case 3:
	return((ui1p->yomi1 == (yomi[0]<< 16 | yomi[1])) &&
	       ui1p->yomi2 >> 16 == yomi[2]);
    case 4:
	return((ui1p->yomi1 == (yomi[0]<< 16 | yomi[1])) &&
	       (ui1p->yomi2 == (yomi[2]<< 16 | yomi[3])));
    default:
	if(len > ((struct uind2 *)((ui1p->pter) + jtl->hontai))->yomi[0])return(0);
	return((ui1p->yomi1 == (yomi[0]<< 16 | yomi[1])) &&
	       (ui1p->yomi2 == (yomi[2]<< 16 | yomi[3])) &&
	       !(Strncmp(((struct uind2 *)((ui1p->pter) + jtl->hontai))->yomi + 1,
			 yomi + 4,len - 4)));
    }
}

Substrudud(ui1d, ui1s,jtl)
register struct uind1 *ui1d, *ui1s;
register struct JT *jtl;
{
    if(ui1d->yomi1 != ui1s->yomi1){
	if((ui1d->yomi1 == 0) ||
	   (((ui1d->yomi1 & 0xffff0000) == (ui1s->yomi1 & 0xffff0000)) &&
	    ((ui1d->yomi1 & 0x0000ffff) == 0))){
	    return(1);
	}else return(0);
    }else if(ui1d->yomi2 != ui1d->yomi2){
	if((ui1d->yomi2 == 0) ||
	   (((ui1d->yomi2 & 0xffff0000) == (ui1s->yomi2 & 0xffff0000)) &&
	    ((ui1d->yomi2 & 0x0000ffff) == 0))){
	    return(1);
	}else return(0);
    }else{
	return(!(Strncmp(
			 ((struct uind2 *)((ui1d->pter) + jtl->hontai))->yomi + 1,
			 ((struct uind2 *)((ui1s->pter) + jtl->hontai))->yomi + 1,
			 ((struct uind2 *)((ui1d->pter) + jtl->hontai))->yomi[0])));
    }
}
/* 
 *
 *
 *sd_biki
 *
 *
 */

UCHAR *hontaistart;

sd_biki(jtl,yomi)
struct JT *jtl;
w_char *yomi;
{
    
    hontaistart = jtl->hontai;
    return(sd_biki_one(jtl->hontai,0,yomi));
}

sd_biki_one(hopter,level,yomi)
char *hopter;
w_char *yomi;
int level;
{
    
    int tsize;
    w_char *charst;
    w_char *sumst;
    int *ptrst;
    int state;
    struct jdata *jep; 
    int index;
    int cnt;
    int serial;
    
    switch((state = *(w_char *)hopter)){
    case ST_NORMAL:
    case ST_NOPTER:
	tsize = *(w_char *)(hopter + 2);
	charst = (w_char *)(hopter + 12);
	sumst = ((w_char *)charst + tsize + 2); /* + 2 keeps two zero words */
	ptrst = (int *)((w_char *)sumst + tsize);
	if((index = sdbinary(*yomi,charst,tsize)) < 0)return(maxlevel);
	
	if((cnt = sumst[index] - sumst[index - 1]) > 0){
	    if(jmt + level  >= jmt_end)
		return( -1);
	    
	    /* set jmt */
	    if(*(jmt + level)){
		for(jep = *(jmt + level); jep->jptr != NULL;jep = jep->jptr);
		jep->jptr = j_e_p;
	    }else{				
		*(jmt + level) = j_e_p;
	    }
	    
	    maxlevel = max(maxlevel ,  level + 1);
	    if(j_e_p >= jmtw_end) return( -1);
	    j_e_p->kanji1 = *(int *)(hopter + 8);
	    j_e_p->kanji2  = sumst[index - 1];
	    j_e_p->serial = serial = *(int *)(hopter + 4) + sumst[index - 1];
	    j_e_p->kosuu = cnt;
	    j_e_p->jishono = number;
	    j_e_p->jptr = NULL;
	    j_e_p->hinsi = hinsistart + serial;
	    j_e_p->hindo = hindostart + serial;
	    if(hindo2start)  j_e_p->hindo_in = hindo2start + serial;
	    else  j_e_p->hindo_in = NULL;
	    j_e_p->which = D_YOMI;
	    j_e_p += 1;
	}
	if(state == ST_NORMAL){
	    if(ptrst[index] != ENDPTR){
		sd_biki_one(hontaistart + ptrst[index],level + 1,yomi + 1);
	    }
	}
	break;
    case ST_NOENT:
	tsize = *(w_char *)(hopter + 2);
	charst = (w_char *)(hopter + 4);
	ptrst = (int *)AL_INT((w_char *)charst + tsize);
	if((index = sdbinary(*yomi,charst,tsize)) < 0)return(maxlevel);
	sd_biki_one(hontaistart + ptrst[index],level + 1,yomi + 1);
	break;
    case ST_SMALL:
	if(*yomi != *(w_char *)(hopter + 2))return(maxlevel);
	sd_biki_one(hopter + 4,level + 1,yomi + 1);
	break;
	
    }
    return(maxlevel);
}


sdbinary(ch,chst,tsize)
register w_char ch;
register w_char *chst;
int tsize;
{
    register  w_char *chst1 = chst;
    register  w_char *chend = chst + tsize;
    register  w_char *key;
    
    for(; chst < chend;){
	key = chst + ((chend - chst) >> 1);
	if(*key == ch) return(key - chst1);
	if(*key < ch) chst = key + 1;
	else chend = key - 1;
    }
    if(*chst == ch) return(chst - chst1);
    return(-1);
}

word_search(dic_no,yomi, jmtx)
w_char *yomi;/** �ɤߤ���Ƭ�ؤΥݥ��� */
struct jdata **jmtx;/** ��̤��֤��ΰ����Ƭ */
{
    
    register struct JT *jtp;
    
    maxlevel = 0;
    jmt = jmtx;
    
    number = dic_no;
    jtp = (struct JT *)(files[dic_table[dic_no].body].area);
    hindo2start = (dic_table[c_env->jisho[dic_no]].hindo != -1)?
	jtp->hindo:NULL;
    hindostart = (dic_table[c_env->jisho[dic_no]].hindo != -1)?
	((struct HJT *)(files[dic_table[c_env->jisho[dic_no]].hindo].area))->hindo:jtp->hindo;

    hinsistart = jtp->hinsi;
    if(jtp->syurui == WNN_UD_DICT){
	if(ud_biki(jtp , yomi) == -1) return(-1);
    }else if(jtp->syurui == WNN_REV_DICT){
	if(rd_biki(jtp , yomi, dic_table[c_env->jisho[dic_no]].rev) == -1)
	    return(-1);
    }else{
	if(sd_biki(jtp , yomi) == -1) return(-1);
    }
    return(maxlevel);
}



/**************************************
 ******        Rev_Dic        *********
 **************************************/

rd_biki(jtl,yomi, which)
struct JT *jtl;
w_char *yomi;
register int which;
{
    register struct rind2 *p; 
    register int ind, serial;
    register int len;
    register int prev ;
    register struct jdata **jepp; 
    
    struct rind1 *tary;
    struct rind2 *ri2;
    UCHAR *kanji;
    
    tary = jtl->ri1[which];
    ri2 = jtl->ri2;
    kanji = jtl->kanji;
    
    for(ind = rd_binary(tary,yomi,jtl->maxri1[which], ri2, kanji, which);
	ind >= 0 ; ind = tary[ind].pter1){
	p=tary[ind].pter +ri2;
	len = Get_kanji_len(p->kanjipter + kanji, which);
	if(jmt + len > jmt_end)
	    return( -1);
	/* set jmt */
	jepp = &(jmt[len - 1]);
	for(;*jepp;jepp = &((*jepp)->jptr));
	maxlevel = max(maxlevel ,  len);
	prev = -2;		/* prev + 1 is not a serial number */
	for(;;p = p->next[which] + ri2){
	    serial = p - ri2;
	    if(serial == prev + 1){
		(j_e_p - 1)->kosuu ++;
	    }else{
		*jepp = j_e_p;	    
		if(j_e_p  >= jmtw_end) return( -1);
		j_e_p->kanji1 = p->kanjipter;
		j_e_p->kanji2  = 0;
		j_e_p->serial = serial;
		j_e_p->kosuu = 1;
		j_e_p->jishono = number;
		j_e_p->jptr = NULL;
		j_e_p->hinsi = hinsistart + serial;
		j_e_p->hindo = hindostart + serial;
		if(hindo2start)  j_e_p->hindo_in = hindo2start + serial;
		else  j_e_p->hindo_in = NULL;
		j_e_p->which = which;
		j_e_p += 1;
		jepp = &((*jepp)->jptr);
	    }
	    prev = serial;
	    if(p->next[which] == RD_ENDPTR) break;
	}
    }
    return(maxlevel);
}

/*
 * Find the element of user dict which matches longest with the yomi.
 */

rd_binary1(tary,yomi,end,ri2, kanji, which)
register struct rind1 *tary;
register w_char *yomi;
register int end;
struct rind2 *ri2;
UCHAR *kanji;
int which;
{
    register w_char *s1, *s2;
    register int start = -1;
    register int key;
    
    for(; start + 1 < end;){
	key = (start + end) / 2;
	s1 = KANJI_str(ri2[tary[key].pter].kanjipter + kanji, which);
	s2 = yomi;
				/* This is same as Strcmp(s1, s2) */
	for (;*s1 != 0 && *s1 == *s2;s1++, s2++);
	if(*s1 > *s2){
	    end = key;
	}else{
	    start = key;
	}
    }
    return(start);
}

/* returns the entry which matches longest with yomi.*/
rd_binary(tary,yomi,end,ri2, kanji, which)
register struct rind1 *tary;
register w_char *yomi;
register int end;
struct rind2 *ri2;
UCHAR *kanji;
int which;
{
    register w_char *s1, *s2;
    register int start;
    register int len;
    
    start = rd_binary1(tary,yomi,end,ri2, kanji, which);
    for(;start != RD_ENDPTR; start = tary[start].pter1){
	s1 = KANJI_str(ri2[tary[start].pter].kanjipter + kanji, which);
	s2 = yomi;
				/* This is same as Substr(s1, s2) */
	for (;*s1 != 0 && *s1 == *s2;s1++, s2++);
	if(!*s1){
	    return(start);
	}
    }
    return(-1);
}

/* returns the entry which matches accurately with yomi.*/

rd_binary_same(tary,yomi,end,ri2, kanji, which)
register struct rind1 *tary;
register w_char *yomi;
register int end;
struct rind2 *ri2;
UCHAR *kanji;
int which;
{
    register int start;
    register int len;
    w_char *tmp;
    
    start = rd_binary1(tary,yomi,end,ri2, kanji, which);
    for(;start >= 0; start = tary[start].pter1){
	tmp = KANJI_str(ri2[tary[start].pter].kanjipter + kanji, which);
	if(!Strcmp(tmp, yomi)){
	    return(start);
	}
    }
    return(-1);
}



/*
 *   Gd_biki( Giji (Regular Dict) biki routine.
 *
 *
 *  GD Format
 *  [ - ]  moji no syuugou.
 *   p*   p no 0 kai ijou no kurikaesi(p = syuugou)
 *  []+  p no 1 kai ijou no kurikaesi(p = syuugou)
 *
 *
 *  ri2 is only used to point kanji
 */
#define GD_SET_START  '['
#define GD_SET_END    ']'
#define GD_KURIKAESI0 '*'
#define GD_KURIKAESI1 '+'
#define GD_ESCAPE    '\\'
#define GD_FROMTO     '-'


